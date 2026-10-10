#pragma once

#include "libMMD/Model/MMD/MMDMotionSizing.h"
#include <unordered_map>
#include <set>

namespace libmmd::sizing::detail
{
using Vector = Eigen::Vector3d;
using Rotation = Eigen::Quaterniond;

struct LocalPose
{
    Vector translation = Vector::Zero();
    Rotation rotation = Rotation::Identity();
};

// Immutable indexed motion; sampling shares the host's libMMD interpolation.
class Motion
{
public:
    explicit Motion(const libmmd::VMDFile& file);
    LocalPose Sample(const std::string& bone, uint32_t frame) const;
    uint32_t LastFrame() const { return last_; }
private:
    std::unordered_map<std::string, std::vector<libmmd::VMDMotion>> tracks_;
    uint32_t last_ = 0;
};

class Rig
{
public:
    explicit Rig(const libmmd::PMXFile& model);
    int Find(const std::string& name) const;
    bool Ancestor(int ancestor, int child) const;
    bool SupportedChain(int tip) const;
    const libmmd::PMXFile& model;
    std::vector<int> order;
    std::vector<int> appendOrder;
    std::vector<bool> appendValid;
    std::vector<bool> supported;
    std::vector<std::string> warnings;
private:
    std::unordered_map<std::string, int> names_;
};

struct Pose
{
    explicit Pose(const Rig& rig, const Motion& motion, uint32_t frame);
    void Update();
    const Rig& rig;
    std::vector<LocalPose> local;
    std::vector<LocalPose> append;
    std::vector<Vector> positions;
    std::vector<Rotation> rotations;
};

class Baker
{
public:
    void Store(const Pose& pose, int bone, uint32_t frame);
    void Finish(libmmd::VMDFile& file) const;
private:
    std::map<std::pair<std::string, uint32_t>, libmmd::VMDMotion> keys_;
};

void CheckCancel(const std::atomic_bool* cancel);
void Require(bool condition, const std::string& error);
uint32_t BakeLimit(const Motion& motion, const Options& options, size_t tracks);
void Record(Analysis& analysis, const Options& options, Stage stage, uint32_t frame,
            const std::string& bone, const Vector& target, const Vector& actual);
void Solve(Pose& pose, int effector, const std::vector<int>& joints, const Vector& target,
           const Options& options, const std::atomic_bool* cancel);
struct Goal
{
    int effector;
    std::vector<int> joints;
    Vector target;
};
void SolveGoals(Pose& pose, const std::vector<Goal>& goals, const Options& options, const std::atomic_bool* cancel);
}
