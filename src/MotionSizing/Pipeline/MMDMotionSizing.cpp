#include "libMMD/Model/MMD/MMDMotionSizing.h"
#include "MMDMotionConstraints.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <stdexcept>

namespace libmmd::sizing::detail
{
Result RunMovement(const libmmd::PMXFile&, const libmmd::PMXFile&, const libmmd::VMDFile&, const Options&, const std::atomic_bool*, const ProgressCallback&);
void ApplyStanceAndTwist(const Rig&, const Rig&, const Options&, Result&, const std::atomic_bool*, const ProgressCallback&);
}

namespace libmmd::sizing
{
Result Run(const libmmd::PMXFile& source, const libmmd::PMXFile& target, const libmmd::VMDFile& motion,
           const Options& options, const std::atomic_bool* cancel, const ProgressCallback& progress)
{
    const auto started = std::chrono::steady_clock::now();
    Result result;
    try
    {
        detail::CheckCancel(cancel);
        detail::Require(std::isfinite(options.contactDistance) && options.contactDistance >= 0 &&
            std::isfinite(options.floorHeight) && std::isfinite(options.collisionMargin) && options.collisionMargin >= 0 &&
            std::isfinite(options.tolerance) && options.tolerance > 0 && options.iterations > 0 && options.iterations <= 1000 &&
            options.maxBakeFrames > 0 && options.maxBakedKeys > 0, "Invalid solver options");
        result = detail::RunMovement(source, target, motion, options, cancel, progress);
        if (!result.success) return result;
        // Mark success only after every enabled stage completes.
        result.success = false;
        const detail::Rig sourceRig(source), targetRig(target);
        for (const auto& warning : sourceRig.warnings) result.analysis.warnings.push_back("Source: " + warning);
        for (const auto& warning : targetRig.warnings) result.analysis.warnings.push_back("Target: " + warning);
        detail::ApplyStanceAndTwist(sourceRig, targetRig, options, result, cancel, progress);
        detail::ApplyConstraints(sourceRig, targetRig, options, result, cancel, progress);
        detail::CheckCancel(cancel);
        result.success = true;
    }
    catch (const std::exception& error)
    {
        result.success = false;
        result.cancelled = cancel && cancel->load(std::memory_order_relaxed);
        result.error = result.cancelled ? "" : error.what();
        for (auto& stage : result.stages) stage = libmmd::VMDFile();
        result.analysis.samples.clear();
    }
    result.elapsedMilliseconds = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - started).count();
    return result;
}

BatchResult RunBatch(const std::vector<CharacterInput>& input, const libmmd::VMDFile& camera,
                     const CameraOptions& cameraOptions, const std::atomic_bool* cancel, const ProgressCallback& progress)
{
    BatchResult result;
    try
    {
        detail::CheckCancel(cancel);
        detail::Require(!input.empty() && input.size() <= 16, "A sizing batch requires 1 to 16 characters");
        std::vector<double> ratios;
        double total = 0.;
        for (size_t i = 0; i < input.size(); ++i)
        {
            const auto& character = input[i];
            const ProgressCallback validationProgress = progress ? ProgressCallback([&progress, i, &input](Progress value) {
                value.phase = ProgressPhase::Validation;
                value.characterIndex = i + 1; value.characterCount = input.size();
                progress(value);
            }) : ProgressCallback();
            // Run movement validation before constructing unchecked FK rigs.
            auto movement = detail::RunMovement(character.source, character.target, character.motion, character.options, cancel, validationProgress);
            detail::Require(movement.success, movement.error.empty() ? "Sizing cancelled" : movement.error);
            const double raw = movement.analysis.horizontalRatio / character.options.movementMultiplier;
            ratios.push_back(raw);
            total += raw;
        }
        // MOptions.calc_leg_ratio: a shared horizontal ratio preserves formation.
        const double sharedRatio = std::min(total / input.size(), 1.2);
        for (size_t i = 0; i < input.size(); ++i)
        {
            detail::CheckCancel(cancel);
            Options options = input[i].options;
            if (input.size() > 1) options.movementMultiplier *= sharedRatio / ratios[i];
            const ProgressCallback characterProgress = progress ? ProgressCallback([&progress, i, &input](Progress value) {
                value.characterIndex = i + 1; value.characterCount = input.size();
                progress(value);
            }) : ProgressCallback();
            auto character = Run(input[i].source, input[i].target, input[i].motion, options, cancel, characterProgress);
            detail::Require(character.success, "Character " + std::to_string(i + 1) + ": " + character.error);
            result.characters.push_back(std::move(character));
        }
        const ProgressCallback batchProgress = progress ? ProgressCallback([&progress, &input](Progress value) {
            value.characterIndex = 0; value.characterCount = input.size();
            progress(value);
        }) : ProgressCallback();
        if (std::any_of(input.begin(), input.end(), [](const auto& character) { return character.options.multiContact; }))
            detail::ApplyBatchConstraints(input, result, cancel, batchProgress);
        result.originalCamera = result.camera = camera;
        if (cameraOptions.enabled) detail::ApplyCamera(input, result, cameraOptions, cancel, batchProgress);
        detail::CheckCancel(cancel);
        result.success = true;
    }
    catch (const std::exception& error)
    {
        result = BatchResult();
        result.cancelled = cancel && cancel->load(std::memory_order_relaxed);
        result.error = result.cancelled ? "" : error.what();
    }
    return result;
}
}
