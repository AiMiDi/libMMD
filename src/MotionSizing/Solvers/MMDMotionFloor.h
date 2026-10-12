#pragma once
#include "MMDMotionPose.h"
#include "MMDMotionProgress.h"

namespace libmmd::sizing::detail
{
struct FootFloorTarget
{
    bool active = false;
    Vector goal = Vector::Zero();
    double correction = 0.;
    double initialError = 0.;
    size_t diagnosticIndex = static_cast<size_t>(-1);
};

// Limit only the added world-Y correction. Authored motion and IK switches
// remain untouched; every changed combination is evaluated with playback IK.
void ApplyFloorContinuity(const Rig& source, const Rig& target, const Motion& incoming,
    libmmd::VMDFile& output, const std::vector<std::pair<int, int>>& feet,
    const std::vector<std::vector<FootFloorTarget>>& targets,
    double maximumWithoutFeet, const Options& options, Analysis& analysis, const std::atomic_bool* cancel,
    const ProgressCallback& progress);
}
