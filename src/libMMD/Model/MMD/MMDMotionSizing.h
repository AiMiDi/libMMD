#pragma once

#include "libMMD/Model/MMD/PMXFile.h"
#include "libMMD/Model/MMD/VMDFile.h"
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

// SDK-independent motion sizing. All distances are PMX/VMD
// units; host scale and scene/document lifetime belong to the caller.
namespace libmmd::sizing
{
enum class Stage : size_t { Original, Scale, Offset, Stance, Twist, Avoidance, Contact, MultiCharacter, Count };

struct Options
{
    double movementMultiplier = 1.;
    double legOffset = 0.;
    bool centerOffsets = true;
    bool legOffsets = true;
    bool stance = false;
    bool twist = false;
    bool avoidance = false;
    bool wristContact = false;
    bool fingerContact = false;
    bool floorContact = false;
    bool multiContact = false;
    double contactDistance = .3;
    double floorHeight = 0.;
    double collisionMargin = .05;
    double tolerance = .01;
    unsigned iterations = 40;
    // Limits bound dense baking and diagnostic memory; fail before allocating.
    uint32_t maxBakeFrames = 18000;
    size_t maxBakedKeys = 2000000;
    size_t maxDiagnostics = 20000;
    // Empty selects bone-following rigid bodies outside the controlled arm.
    std::vector<std::string> avoidanceBodies;
};

struct ConstraintSample
{
    Stage stage = Stage::Contact;
    uint32_t frame = 0;
    std::string bone;
    Eigen::Vector3d target = Eigen::Vector3d::Zero();
    Eigen::Vector3d actual = Eigen::Vector3d::Zero();
    double error = 0.;
};

struct Analysis
{
    double horizontalRatio = 1.;
    double verticalRatio = 1.;
    std::map<std::string, Eigen::Vector3d> localOffsets;
    std::vector<std::string> warnings;
    size_t matchedTracks = 0;
    size_t modifiedKeys = 0;
    size_t constraints = 0;
    size_t unresolved = 0;
    double maxResidual = 0.;
    std::vector<ConstraintSample> samples;
};

struct Result
{
    bool success = false;
    bool cancelled = false;
    std::string error;
    Analysis analysis;
    std::array<libmmd::VMDFile, static_cast<size_t>(Stage::Count)> stages;
    double elapsedMilliseconds = 0.;
};

Result Run(const libmmd::PMXFile& source, const libmmd::PMXFile& target,
           const libmmd::VMDFile& input, const Options& options = {},
           const std::atomic_bool* cancel = nullptr);

struct CharacterInput
{
    libmmd::PMXFile source;
    libmmd::PMXFile target;
    libmmd::VMDFile motion;
    Options options;
};

struct CameraOptions
{
    bool enabled = false;
    double maxDistanceRatio = 5.;
};

struct BatchResult
{
    bool success = false;
    bool cancelled = false;
    std::string error;
    std::vector<Result> characters;
    libmmd::VMDFile originalCamera;
    libmmd::VMDFile camera;
};

BatchResult RunBatch(const std::vector<CharacterInput>& characters,
                     const libmmd::VMDFile& camera = libmmd::VMDFile(), const CameraOptions& cameraOptions = {},
                     const std::atomic_bool* cancel = nullptr);
}
