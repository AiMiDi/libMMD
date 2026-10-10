#pragma once
#include "MMDMotionPose.h"
#include "MMDMotionProgress.h"

namespace libmmd::sizing::detail
{
struct Effector
{
    std::string name;
    int source = -1;
    int target = -1;
    int wrist = -1;
    double handScale = 1.;
    std::vector<int> joints;
    bool finger = false;
};

std::vector<Effector> Effectors(const Rig& source, const Rig& target, const Options& options, Analysis& analysis);
// Returns the nearest permitted point; unchanged points are outside the shape.
Vector ProjectOutside(const libmmd::PMXRigidbody& body, const Pose& pose, const Vector& point, double margin);
void ApplyConstraints(const Rig& source, const Rig& target, const Options& options, Result& result,
                      const std::atomic_bool* cancel, const ProgressCallback& progress = {});
void ApplyBatchConstraints(const std::vector<CharacterInput>& input, BatchResult& result, const std::atomic_bool* cancel,
                           const ProgressCallback& progress = {});
void ApplyCamera(const std::vector<CharacterInput>& input, BatchResult& result,
                 const CameraOptions& options, const std::atomic_bool* cancel, const ProgressCallback& progress = {});
}
