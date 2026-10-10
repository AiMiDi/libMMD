#include "MMDMotionPose.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <stdexcept>
#include <Eigen/Cholesky>
#include "libMMD/Model/MMD/VMDInterpolation.h"

namespace libmmd::sizing::detail
{
namespace
{
Rotation FixedRotation(const Rotation& rotation, const Vector& axis)
{
    if (axis.squaredNorm() < 1.e-16) return rotation;
    const Vector unit = axis.normalized();
    const Vector projected = unit * rotation.vec().dot(unit);
    Rotation twist(rotation.w(), projected.x(), projected.y(), projected.z());
    return twist.squaredNorm() > 1.e-16 ? twist.normalized() : Rotation::Identity();
}
}

Motion::Motion(const libmmd::VMDFile& file)
{
    for (const auto& key : file.m_motions)
    {
        tracks_[key.m_boneName.ToUtf8String()].push_back(key);
        last_ = std::max(last_, key.m_frame);
    }
    for (auto& track : tracks_)
        std::sort(track.second.begin(), track.second.end(), [](const auto& a, const auto& b) { return a.m_frame < b.m_frame; });
}

LocalPose Motion::Sample(const std::string& bone, uint32_t frame) const
{
    const auto found = tracks_.find(bone);
    if (found == tracks_.end()) return {};
    const auto& keys = found->second;
    auto next = std::upper_bound(keys.begin(), keys.end(), frame, [](uint32_t value, const auto& key) { return value < key.m_frame; });
    if (next == keys.begin() || next == keys.end() || (next - 1)->m_frame == frame)
    {
        const auto& key = next == keys.begin() ? keys.front() : *(next - 1);
        return {key.m_translate.cast<double>(), key.m_quaternion.cast<double>().normalized()};
    }
    const auto& previous = *(next - 1);
    // Use the same channel layout and outgoing-key convention as C4D playback.
    // Reimplementing the upstream Python sampler here gave a different pose
    // between sparse keys, even when the keyframe values themselves matched.
    const auto value = libmmd::InterpolateBoneKeys(libmmd::VMDBoneKeyframe(previous), libmmd::VMDBoneKeyframe(*next), static_cast<float>(frame));
    return {value.translate.cast<double>(), value.rotate.cast<double>().normalized()};
}

Rig::Rig(const libmmd::PMXFile& file) : model(file)
{
    const size_t count = model.m_bones.size();
    std::vector<unsigned char> state(count);
    for (size_t i = 0; i < count; ++i)
        Require(names_.emplace(model.m_bones[i].m_name, static_cast<int>(i)).second, "Duplicate rig bone");
    // Validate first in Run; explicit stack also keeps pathological deep rigs safe.
    for (size_t i = 0; i < count; ++i)
    {
        std::vector<int> path;
        int node = static_cast<int>(i);
        while (node >= 0 && state[static_cast<size_t>(node)] == 0)
        {
            state[static_cast<size_t>(node)] = 1;
            path.push_back(node);
            node = model.m_bones[static_cast<size_t>(node)].m_parentBoneIndex;
            Require(node >= -1 && node < static_cast<int>(count), "Invalid rig parent");
        }
        Require(node < 0 || state[static_cast<size_t>(node)] != 1, "Cyclic rig hierarchy");
        for (auto it = path.rbegin(); it != path.rend(); ++it)
        { order.push_back(*it); state[static_cast<size_t>(*it)] = 2; }
    }
    // Append depends on animation-space channels, not source world transforms.
    // Keep its dependency graph separate from the transform hierarchy: an
    // append source may legally be a descendant in the parent hierarchy.
    std::fill(state.begin(), state.end(), 0);
    appendValid.assign(count, true);
    for (size_t i = 0; i < count; ++i)
    {
        const auto& bone = model.m_bones[i];
        if ((static_cast<uint16_t>(bone.m_boneFlag) & 0x300u) != 0)
            Require(bone.m_appendBoneIndex >= -1 && bone.m_appendBoneIndex < static_cast<int>(count) &&
                    std::isfinite(bone.m_appendWeight), "Invalid append source or weight: " + bone.m_name);
    }
    for (size_t i = 0; i < count; ++i)
    {
        std::vector<int> path;
        int node = static_cast<int>(i);
        while (node >= 0 && state[static_cast<size_t>(node)] == 0)
        {
            state[static_cast<size_t>(node)] = 1;
            path.push_back(node);
            const auto& bone = model.m_bones[static_cast<size_t>(node)];
            const auto flags = static_cast<uint16_t>(bone.m_boneFlag);
            node = (flags & 0x300u) != 0 && (flags & 0x80u) == 0 && bone.m_appendWeight != 0 ? bone.m_appendBoneIndex : -1;
        }
        if (node >= 0 && state[static_cast<size_t>(node)] == 1)
        {
            warnings.push_back("Unsupported cyclic append dependency: " + model.m_bones[static_cast<size_t>(node)].m_name);
            for (int dependency : path) appendValid[static_cast<size_t>(dependency)] = false;
        }
        else if (node >= 0 && !appendValid[static_cast<size_t>(node)])
            for (int dependency : path) appendValid[static_cast<size_t>(dependency)] = false;
        for (auto it = path.rbegin(); it != path.rend(); ++it)
        { appendOrder.push_back(*it); state[static_cast<size_t>(*it)] = 2; }
    }
    supported.assign(count, true);
    for (int index : order)
    {
        const auto& bone = model.m_bones[static_cast<size_t>(index)];
        supported[static_cast<size_t>(index)] = appendValid[static_cast<size_t>(index)] && (static_cast<uint16_t>(bone.m_boneFlag) & 0x2000u) == 0 &&
            (bone.m_parentBoneIndex < 0 || supported[static_cast<size_t>(bone.m_parentBoneIndex)]);
    }
}

int Rig::Find(const std::string& name) const
{
    const auto found = names_.find(name);
    return found == names_.end() ? -1 : found->second;
}

bool Rig::Ancestor(int ancestor, int child) const
{
    for (int node = child; node >= 0; node = model.m_bones[static_cast<size_t>(node)].m_parentBoneIndex)
        if (node == ancestor) return true;
    return false;
}

bool Rig::SupportedChain(int tip) const
{
    return tip >= 0 && static_cast<size_t>(tip) < supported.size() && supported[static_cast<size_t>(tip)];
}

Pose::Pose(const Rig& skeleton, const Motion& motion, uint32_t frame) : rig(skeleton)
{
    for (const auto& bone : rig.model.m_bones) local.push_back(motion.Sample(bone.m_name, frame));
    positions.resize(local.size());
    rotations.resize(local.size());
    append.resize(local.size());
    Update();
}

void Pose::Update()
{
    for (int index : rig.appendOrder)
    {
        const size_t i = static_cast<size_t>(index);
        const auto& bone = rig.model.m_bones[i];
        const auto flags = static_cast<uint16_t>(bone.m_boneFlag);
        append[i] = LocalPose();
        if (!rig.appendValid[i] || (flags & 0x300u) == 0 || bone.m_appendBoneIndex < 0 || bone.m_appendWeight == 0) continue;
        const size_t source = static_cast<size_t>(bone.m_appendBoneIndex);
        const auto& sourceBone = rig.model.m_bones[source];
        const bool sourceAppends = (static_cast<uint16_t>(sourceBone.m_boneFlag) & 0x300u) != 0 && sourceBone.m_appendBoneIndex >= 0;
        const auto& value = (flags & 0x80u) != 0 || !sourceAppends ? local[source] : append[source];
        // Match PMXNode::UpdateAppendTransform and C4D's evaluated append
        // channels, including negative and extrapolating weights.
        if ((flags & 0x100u) != 0)
            append[i].rotation = Rotation::Identity().slerp(bone.m_appendWeight, value.rotation.normalized()).normalized();
        if ((flags & 0x200u) != 0) append[i].translation = value.translation * bone.m_appendWeight;
    }
    for (int index : rig.order)
    {
        const size_t i = static_cast<size_t>(index);
        const auto& bone = rig.model.m_bones[i];
        const int parent = bone.m_parentBoneIndex;
        Rotation rotation = (local[i].rotation * append[i].rotation).normalized();
        // PMX playback retains the authored quaternion. Fixed axes constrain
        // newly solved corrections, not the animation being evaluated.
        const Vector offset = bone.m_position.cast<double>() + local[i].translation + append[i].translation;
        if (parent < 0) { positions[i] = offset; rotations[i] = rotation; }
        else
        {
            const size_t p = static_cast<size_t>(parent);
            positions[i] = positions[p] + rotations[p] * (offset - rig.model.m_bones[p].m_position.cast<double>());
            rotations[i] = (rotations[p] * rotation).normalized();
        }
    }
}

void Baker::Store(const Pose& pose, int bone, uint32_t frame)
{
    const size_t index = static_cast<size_t>(bone);
    const auto& value = pose.local[index];
    Require(value.translation.cast<float>().allFinite() && value.rotation.coeffs().allFinite(), "Non-finite baked pose");
    libmmd::VMDMotion key;
    std::u16string utf16;
    Require(libmmd::ConvU8ToU16(pose.rig.model.m_bones[index].m_name, utf16), "Invalid UTF-8 bone name");
    key.m_boneName.Set(libmmd::ConvertU16ToSjisString(utf16).c_str());
    Require(key.m_boneName.ToUtf8String() == pose.rig.model.m_bones[index].m_name, "Bone name cannot fit VMD: " + pose.rig.model.m_bones[index].m_name);
    key.m_frame = frame;
    key.m_translate = value.translation.cast<float>();
    key.m_quaternion = value.rotation.normalized().cast<float>();
    for (size_t axis = 0; axis < 4; ++axis)
        for (size_t channel = 0; channel < 4; ++channel)
        {
            const size_t offset = axis * 16 + channel;
            key.m_interpolation[offset] = key.m_interpolation[offset + 4] = 20;
            key.m_interpolation[offset + 8] = key.m_interpolation[offset + 12] = 107;
        }
    keys_[{pose.rig.model.m_bones[index].m_name, frame}] = key;
}

void Baker::Finish(libmmd::VMDFile& file) const
{
    std::set<std::string> replaced;
    for (const auto& key : keys_) replaced.insert(key.first.first);
    file.m_motions.erase(std::remove_if(file.m_motions.begin(), file.m_motions.end(),
        [&](const auto& key) { return replaced.count(key.m_boneName.ToUtf8String()) != 0; }), file.m_motions.end());
    for (const auto& key : keys_) file.m_motions.push_back(key.second);
}

uint32_t BakeLimit(const Motion& motion, const Options& options, size_t tracks)
{
    const uint64_t frames = static_cast<uint64_t>(motion.LastFrame()) + 1;
    Require(frames <= options.maxBakeFrames && tracks <= options.maxBakedKeys / frames,
            "Dense sizing exceeds frame/key budget; split the motion or increase the explicit core budget");
    return motion.LastFrame();
}

void Solve(Pose& pose, int effector, const std::vector<int>& joints, const Vector& target,
           const Options& options, const std::atomic_bool* cancel)
{
    if (joints.empty()) return;
    auto best = pose.local;
    double bestError = (pose.positions[static_cast<size_t>(effector)] - target).squaredNorm();
    for (unsigned iteration = 0; iteration < options.iterations; ++iteration)
    {
        CheckCancel(cancel);
        if (bestError <= options.tolerance * options.tolerance) break;
        bool moved = false;
        for (int joint : joints)
        {
            const size_t j = static_cast<size_t>(joint), e = static_cast<size_t>(effector);
            const Vector from = pose.positions[e] - pose.positions[j];
            const Vector to = target - pose.positions[j];
            if (from.squaredNorm() < 1.e-16 || to.squaredNorm() < 1.e-16) continue;
            Eigen::AngleAxisd delta(Rotation::FromTwoVectors(from, to));
            delta.angle() = std::min(delta.angle(), .35);
            if (delta.angle() < 1.e-9) continue;
            const int parent = pose.rig.model.m_bones[j].m_parentBoneIndex;
            const Rotation parentRotation = parent < 0 ? Rotation::Identity() : pose.rotations[static_cast<size_t>(parent)];
            Rotation correction = (parentRotation.conjugate() * Rotation(delta) * parentRotation).normalized();
            if ((static_cast<uint16_t>(pose.rig.model.m_bones[j].m_boneFlag) & 0x400u) != 0)
                correction = FixedRotation(correction, pose.rig.model.m_bones[j].m_fixedAxis.cast<double>());
            pose.local[j].rotation = (correction * pose.local[j].rotation).normalized();
            pose.Update();
            const double error = (pose.positions[e] - target).squaredNorm();
            if (error < bestError) { best = pose.local; bestError = error; moved = true; }
        }
        if (!moved) break;
    }
    pose.local = std::move(best);
    pose.Update();
}

void SolveGoals(Pose& pose, const std::vector<Goal>& goals, const Options& options, const std::atomic_bool* cancel)
{
    if (goals.empty()) return;
    struct Axis { int joint; Vector local; };
    std::set<int> joints;
    for (const auto& goal : goals) joints.insert(goal.joints.begin(), goal.joints.end());
    std::vector<Axis> axes;
    for (int joint : joints)
    {
        const auto& bone = pose.rig.model.m_bones[static_cast<size_t>(joint)];
        if ((static_cast<uint16_t>(bone.m_boneFlag) & 0x400u) != 0)
        {
            if (bone.m_fixedAxis.squaredNorm() > 1.e-16f) axes.push_back({joint, bone.m_fixedAxis.cast<double>().normalized()});
        }
        else for (int axis = 0; axis < 3; ++axis) axes.push_back({joint, Vector::Unit(axis)});
    }
    if (axes.empty()) return;
    const auto residual = [&]() {
        Eigen::VectorXd errors(static_cast<Eigen::Index>(goals.size() * 3));
        for (size_t i = 0; i < goals.size(); ++i)
            errors.segment<3>(static_cast<Eigen::Index>(i * 3)) = goals[i].target - pose.positions[static_cast<size_t>(goals[i].effector)];
        return errors;
    };
    double damping = .01;
    for (unsigned iteration = 0; iteration < options.iterations; ++iteration)
    {
        CheckCancel(cancel);
        const Eigen::VectorXd errors = residual();
        double maximum = 0.;
        for (size_t i = 0; i < goals.size(); ++i) maximum = std::max(maximum, errors.segment<3>(static_cast<Eigen::Index>(i * 3)).norm());
        if (maximum <= options.tolerance) break;
        Eigen::MatrixXd jacobian = Eigen::MatrixXd::Zero(errors.size(), static_cast<Eigen::Index>(axes.size()));
        for (size_t column = 0; column < axes.size(); ++column)
        {
            const auto& axis = axes[column];
            const size_t joint = static_cast<size_t>(axis.joint);
            const int parent = pose.rig.model.m_bones[joint].m_parentBoneIndex;
            const Vector world = parent < 0 ? axis.local : (pose.rotations[static_cast<size_t>(parent)] * axis.local).eval();
            for (size_t row = 0; row < goals.size(); ++row)
                if (std::find(goals[row].joints.begin(), goals[row].joints.end(), axis.joint) != goals[row].joints.end())
                    jacobian.block<3, 1>(static_cast<Eigen::Index>(row * 3), static_cast<Eigen::Index>(column)) =
                        world.cross(pose.positions[static_cast<size_t>(goals[row].effector)] - pose.positions[joint]);
        }
        Eigen::MatrixXd normal = jacobian * jacobian.transpose();
        normal.diagonal().array() += damping * damping;
        const Eigen::VectorXd step = jacobian.transpose() * normal.ldlt().solve(errors);
        Require(step.allFinite(), "Non-finite contact solve");
        std::map<int, Vector> corrections;
        for (size_t i = 0; i < axes.size(); ++i)
        {
            auto entry = corrections.emplace(axes[i].joint, Vector::Zero());
            entry.first->second += axes[i].local * step[static_cast<Eigen::Index>(i)];
        }
        double maxAngle = 0.;
        for (const auto& correction : corrections) maxAngle = std::max(maxAngle, correction.second.norm());
        const double scale = maxAngle > .35 ? .35 / maxAngle : 1.;
        const auto before = pose.local;
        bool accepted = false;
        for (int search = 0; search < 8; ++search)
        {
            pose.local = before;
            for (const auto& correction : corrections)
            {
                const double angle = correction.second.norm();
                if (angle < 1.e-12) continue;
                const size_t joint = static_cast<size_t>(correction.first);
                pose.local[joint].rotation = (Rotation(Eigen::AngleAxisd(angle * scale * std::ldexp(1., -search), correction.second / angle)) * before[joint].rotation).normalized();
            }
            pose.Update();
            if (residual().squaredNorm() < errors.squaredNorm() - 1.e-14) { accepted = true; break; }
        }
        if (!accepted)
        {
            pose.local = before; pose.Update();
            damping *= 10.;
            if (damping > 10.) break;
        }
        else damping = std::max(.001, damping * .5);
    }
}
}
