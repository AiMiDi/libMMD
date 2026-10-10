#pragma once
#include "MMDMotionPose.h"
#include "MMDMotionProgress.h"

namespace libmmd::sizing::detail
{
struct LegPair { size_t left, right; };
std::vector<LegPair> LegCollisionPairs(const Rig& rig);
std::vector<double> LegPenetrations(const Pose& pose, const std::vector<LegPair>& pairs, double margin);
void ApplyLegAvoidance(const Rig& rig, libmmd::VMDFile& motion, const Options& options, Analysis& analysis,
                       const std::atomic_bool* cancel, const ProgressCallback& progress);
}
