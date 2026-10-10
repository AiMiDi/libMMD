#include "../../../MotionSizing/Pose/MMDMotionIK.h"
#include "PMXModel.h"
#include "PMXFile.h"
#include "MMDIkSolver.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <mutex>
#include <stdexcept>

namespace libmmd::sizing::detail
{
namespace
{
class PlaybackIKImpl final : public PlaybackIK
{
public:
    PlaybackIKImpl(const PMXFile& model, const std::vector<int>& parentOrder, const std::vector<int>& appendOrder)
        : parentOrder_(parentOrder)
    {
        for (size_t i = 0; i < model.m_bones.size(); ++i) nodes_.push_back(std::make_unique<PMXNode>());
        for (size_t i = 0; i < nodes_.size(); ++i)
        {
            const auto& bone = model.m_bones[i];
            auto& node = *nodes_[i];
            node.SetName(bone.m_name);
            Eigen::Vector3f offset = bone.m_position;
            if (bone.m_parentBoneIndex >= 0)
            {
                nodes_[bone.m_parentBoneIndex]->AddChild(&node);
                offset -= model.m_bones[bone.m_parentBoneIndex].m_position;
            }
            node.SetTranslate(offset);
            node.SaveInitialTRS();
            node.BeginUpdateTransform();
            const uint16_t flags = static_cast<uint16_t>(bone.m_boneFlag);
            if ((flags & 0x300u) && bone.m_appendBoneIndex >= 0)
            {
                node.SetAppendNode(nodes_[bone.m_appendBoneIndex].get());
                node.EnableAppendRotate((flags & 0x100u) != 0);
                node.EnableAppendTranslate((flags & 0x200u) != 0);
                node.EnableAppendLocal((flags & 0x80u) != 0);
                node.SetAppendWeight(bone.m_appendWeight);
            }
            if ((flags & 0x20u) == 0) continue;
            if (bone.m_ikTargetBoneIndex < 0 || bone.m_ikTargetBoneIndex >= static_cast<int>(nodes_.size()) ||
                bone.m_ikIterationCount < 0 || bone.m_ikIterationCount > 10000 || !std::isfinite(bone.m_ikLimit) || bone.m_ikLimit < 0)
                throw std::runtime_error("Invalid PMX IK controller: " + bone.m_name);
            auto solver = std::make_unique<MMDIkSolver>();
            solver->SetIKNode(&node);
            solver->SetTargetNode(nodes_[bone.m_ikTargetBoneIndex].get());
            solver->SetIterateCount(bone.m_ikIterationCount <= 0 ? 4 : bone.m_ikIterationCount);
            solver->SetLimitAngle(bone.m_ikLimit);
            for (const auto& link : bone.m_ikLinks)
            {
                if (link.m_ikBoneIndex < 0 || link.m_ikBoneIndex >= static_cast<int>(nodes_.size()) ||
                    (link.m_enableLimit && (!link.m_limitMin.allFinite() || !link.m_limitMax.allFinite() ||
                     (link.m_limitMin.array() > link.m_limitMax.array()).any())))
                    throw std::runtime_error("Invalid PMX IK link: " + bone.m_name);
                nodes_[link.m_ikBoneIndex]->EnableIK(true);
                solver->AddIKChain(nodes_[link.m_ikBoneIndex].get(), link.m_enableLimit != 0,
                    link.m_enableLimit ? link.m_limitMin : Eigen::Vector3f::Zero().eval(),
                    link.m_enableLimit ? link.m_limitMax : Eigen::Vector3f::Zero().eval());
            }
            solvers_.emplace(static_cast<int>(i), std::move(solver));
        }
        // Parent links are all installed before path caching.
        for (auto& solver : solvers_) solver.second->BuildChainPath();
        for (int i : appendOrder)
        {
            const auto& bone = model.m_bones[i];
            const int phase = (static_cast<uint16_t>(bone.m_boneFlag) & 0x1000u) ? 1 : 0;
            groups_[{phase, std::max(0, bone.m_deformDepth)}].push_back(i);
        }
    }

    void Evaluate(const std::vector<PlaybackChannel>& local, const std::vector<uint8_t>& enabled,
                  std::vector<PlaybackChannel>& world) override
    {
        // A Rig can be sampled by more than one caller. No frame state leaks
        // across evaluations, and the reusable node storage is serialized.
        std::lock_guard<std::mutex> lock(mutex_);
        if (local.size() != nodes_.size() || enabled.size() != nodes_.size())
            throw std::runtime_error("Invalid IK playback channel count");
        for (size_t i = 0; i < nodes_.size(); ++i)
        {
            auto& node = *nodes_[i]; const auto& value = local[i];
            node.BeginUpdateTransform();
            node.SetTranslate(node.GetInitialTranslate() + Eigen::Vector3f(float(value[0]), float(value[1]), float(value[2])));
            node.SetAnimationRotate(Eigen::Quaternionf(float(value[6]), float(value[3]), float(value[4]), float(value[5])).normalized());
            node.UpdateLocalTransform();
        }
        UpdateGlobals();
        for (const auto& group : groups_)
        {
            for (int index : group.second)
            {
                nodes_[index]->UpdateAppendTransform();
                nodes_[index]->UpdateLocalTransform();
            }
            UpdateGlobals();
            // C4D runs IK by controller PMX index within each deform layer.
            std::vector<int> controllers;
            for (int index : group.second) if (enabled[index] && solvers_.count(index)) controllers.push_back(index);
            std::sort(controllers.begin(), controllers.end());
            for (int index : controllers) solvers_.at(index)->Solve();
        }
        world.resize(nodes_.size());
        for (size_t i = 0; i < nodes_.size(); ++i)
        {
            const auto& matrix = nodes_[i]->GetGlobalTransform();
            const Eigen::Quaternionf rotation(Eigen::Matrix3f(matrix.block<3, 3>(0, 0)));
            if (!matrix.allFinite() || !rotation.coeffs().allFinite()) throw std::runtime_error("Non-finite PMX IK pose");
            world[i] = {matrix(0, 3), matrix(1, 3), matrix(2, 3), rotation.x(), rotation.y(), rotation.z(), rotation.w()};
        }
    }
private:
    void UpdateGlobals()
    {
        for (int index : parentOrder_)
        {
            auto& node = *nodes_[index];
            if (node.GetParent()) node.SetGlobalTransform(node.GetParent()->GetGlobalTransform() * node.GetLocalTransform());
            else node.SetGlobalTransform(node.GetLocalTransform());
        }
    }
    std::mutex mutex_;
    std::vector<std::unique_ptr<PMXNode>> nodes_;
    std::vector<int> parentOrder_;
    std::map<std::pair<int, int>, std::vector<int>> groups_;
    std::map<int, std::unique_ptr<MMDIkSolver>> solvers_;
};
}
std::shared_ptr<PlaybackIK> CreatePlaybackIK(const PMXFile& model, const std::vector<int>& parentOrder,
                                          const std::vector<int>& appendOrder)
{
    return std::make_shared<PlaybackIKImpl>(model, parentOrder, appendOrder);
}
}
