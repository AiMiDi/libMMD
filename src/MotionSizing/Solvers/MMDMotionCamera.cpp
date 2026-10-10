#include "MMDMotionConstraints.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <set>

namespace libmmd::sizing::detail
{
void ApplyCamera(const std::vector<CharacterInput>& input, BatchResult& result,
                 const CameraOptions& options, const std::atomic_bool* cancel)
{
    Require(!result.camera.m_cameras.empty(), "Camera sizing requires camera keys");
    Require(std::isfinite(options.maxDistanceRatio) && options.maxDistanceRatio >= 1., "Invalid camera distance limit");
    std::vector<std::unique_ptr<Rig>> sources, targets;
    std::vector<Motion> originals, motions;
    std::vector<std::vector<std::pair<int, int>>> landmarks;
    const std::vector<std::string> names = {"センター", "上半身", "首", "頭", "左肩", "右肩", "左腕", "右腕",
        "左手首", "右手首", "左足", "右足", "左足ＩＫ", "右足ＩＫ"};
    for (size_t i = 0; i < input.size(); ++i)
    {
        sources.push_back(std::make_unique<Rig>(input[i].source));
        targets.push_back(std::make_unique<Rig>(input[i].target));
        originals.emplace_back(input[i].motion);
        motions.emplace_back(result.characters[i].stages[static_cast<size_t>(Stage::MultiCharacter)]);
        landmarks.emplace_back();
        for (const auto& name : names)
        {
            const int s = sources.back()->Find(name), t = targets.back()->Find(name);
            if (s >= 0 && t >= 0 && sources.back()->SupportedChain(s) && targets.back()->SupportedChain(t))
                landmarks.back().emplace_back(s, t);
        }
        Require(landmarks.back().size() >= 2, "Insufficient common camera landmarks");
    }
    std::set<uint32_t> frames;
    for (auto& key : result.camera.m_cameras)
    {
        CheckCancel(cancel);
        Require(key.m_frame <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()) && frames.insert(key.m_frame).second &&
                key.m_interest.allFinite() && key.m_rotate.allFinite() && std::isfinite(key.m_distance) &&
                key.m_viewAngle > 0 && key.m_viewAngle < 180 && key.m_isPerspective <= 1, "Invalid or duplicate camera key");
        // VMD camera Euler convention: yaw/pitch/roll, with negative Y and Z.
        const Rotation rotation = Rotation(Eigen::AngleAxisd(-key.m_rotate.y(), Vector::UnitY()) *
            Eigen::AngleAxisd(key.m_rotate.x(), Vector::UnitX()) * Eigen::AngleAxisd(-key.m_rotate.z(), Vector::UnitZ()));
        Vector sourceMin = Vector::Constant(std::numeric_limits<double>::infinity()), sourceMax = -sourceMin;
        Vector targetMin = sourceMin, targetMax = sourceMax;
        for (size_t i = 0; i < input.size(); ++i)
        {
            const Pose source(*sources[i], originals[i], key.m_frame), target(*targets[i], motions[i], key.m_frame);
            for (const auto& landmark : landmarks[i])
            {
                const Vector s = rotation.conjugate() * source.positions[static_cast<size_t>(landmark.first)];
                const Vector t = rotation.conjugate() * target.positions[static_cast<size_t>(landmark.second)];
                sourceMin = sourceMin.cwiseMin(s); sourceMax = sourceMax.cwiseMax(s);
                targetMin = targetMin.cwiseMin(t); targetMax = targetMax.cwiseMax(t);
            }
        }
        const Vector sourceSize = sourceMax - sourceMin, targetSize = targetMax - targetMin;
        double ratio = 0.;
        for (int axis = 0; axis < 2; ++axis)
            if (sourceSize[axis] > 1.e-8) ratio = std::max(ratio, targetSize[axis] / sourceSize[axis]);
        Require(ratio > 0 && std::isfinite(ratio), "Degenerate camera landmark extent");
        ratio = std::clamp(ratio, 1. / options.maxDistanceRatio, options.maxDistanceRatio);
        const Vector sourceCenter = rotation * ((sourceMin + sourceMax) * .5);
        const Vector targetCenter = rotation * ((targetMin + targetMax) * .5);
        key.m_interest = (targetCenter + (key.m_interest.cast<double>() - sourceCenter) * ratio).cast<float>();
        key.m_distance = static_cast<float>(key.m_distance * ratio);
        Require(key.m_interest.allFinite() && std::isfinite(key.m_distance), "Camera output exceeds float range");
    }
}
}
