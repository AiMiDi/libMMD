// Arm-axis compensation follows StanceService e5c3035 (MIT). Twist distribution
// is a geometric swing/twist implementation; it is not upstream's elbow search.
#include "MMDMotionPose.h"
#include "MMDMotionProgress.h"
#include <algorithm>
#include <cmath>

namespace libmmd::sizing::detail
{
namespace
{
Rotation Segment(const Rig& rig, int from, int to, const Vector& axis)
{
    const Vector direction = (rig.model.m_bones[static_cast<size_t>(to)].m_position -
                              rig.model.m_bones[static_cast<size_t>(from)].m_position).cast<double>();
    Require(direction.squaredNorm() > 1.e-16, "Degenerate stance segment");
    return Rotation::FromTwoVectors(axis, direction).normalized();
}

struct Correction { Rotation from = Rotation::Identity(), to = Rotation::Identity(); };

void Stance(const Rig& source, const Rig& target, libmmd::VMDFile& motion, Analysis& analysis)
{
    std::map<std::string, Correction> corrections;
    const auto add = [&](const std::string& incoming, const std::string& name, const std::string& outgoing, const Vector& axis) {
        const int s = source.Find(name), t = target.Find(name);
        const int se = source.Find(outgoing), te = target.Find(outgoing);
        if (s < 0 || t < 0 || se < 0 || te < 0)
        { analysis.warnings.push_back("Stance skipped (missing segment): " + name + " -> " + outgoing); return; }
        if (!source.SupportedChain(s) || !target.SupportedChain(t))
        { analysis.warnings.push_back("Stance skipped (external-parent chain): " + name); return; }
        Correction correction;
        correction.to = Segment(target, t, te, axis).conjugate() * Segment(source, s, se, axis);
        if (!incoming.empty())
        {
            const int sp = source.Find(incoming), tp = target.Find(incoming);
            if (sp < 0 || tp < 0) { analysis.warnings.push_back("Stance missing parent: " + incoming); return; }
            correction.from = Segment(target, tp, t, axis).conjugate() * Segment(source, sp, s, axis);
        }
        corrections[name] = correction;
    };
    for (const std::string side : {"左", "右"})
    {
        const Vector axis(side == "左" ? 1. : -1., 0, 0);
        add("", side + "腕", side + "ひじ", axis);
        add(side + "腕", side + "ひじ", side + "手首", axis);
        add(side + "ひじ", side + "手首", side + "中指１", axis);
        // Shoulder compensation would otherwise rotate the already-corrected
        // arm a second time. Cancel its outgoing correction at the arm root.
        if (source.Find(side + "肩") >= 0 && target.Find(side + "肩") >= 0)
        {
            add("", side + "肩", side + "腕", axis);
            if (corrections.count(side + "肩") && corrections.count(side + "腕"))
                corrections[side + "腕"].from = corrections.at(side + "肩").to;
        }
    }
    // These corrections use actual named bind segments; unlike upstream they
    // do not synthesize virtual trunk/heel vertices or search for a best angle.
    add("", "上半身", source.Find("上半身2") >= 0 && target.Find("上半身2") >= 0 ? "上半身2" : "首", Vector::UnitY());
    if (source.Find("上半身2") >= 0 && target.Find("上半身2") >= 0)
        add("上半身", "上半身2", "首", Vector::UnitY());
    for (auto& key : motion.m_motions)
    {
        const auto correction = corrections.find(key.m_boneName.ToUtf8String());
        if (correction == corrections.end()) continue;
        key.m_quaternion = (correction->second.from.conjugate() * key.m_quaternion.cast<double>().normalized() *
                            correction->second.to).normalized().cast<float>();
    }
    // Bind-angle correction also matters for tracks absent from the source.
    // Add frame zero only for non-identity corrections, without touching others.
    Pose bind(target, Motion(motion), 0);
    Baker missing;
    for (const auto& entry : corrections)
    {
        const bool exists = std::any_of(motion.m_motions.begin(), motion.m_motions.end(),
            [&](const auto& key) { return key.m_boneName.ToUtf8String() == entry.first; });
        if (!exists && (entry.second.from.conjugate() * entry.second.to).angularDistance(Rotation::Identity()) > 1.e-8)
        {
            const int bone = target.Find(entry.first);
            bind.local[static_cast<size_t>(bone)].rotation = entry.second.from.conjugate() * entry.second.to;
            missing.Store(bind, bone, 0);
        }
    }
    missing.Finish(motion);
}

struct TwistChain { int parent, twist, child; Vector axis; };

void Twist(const Rig& target, libmmd::VMDFile& motion, const Options& options, Analysis& analysis, const std::atomic_bool* cancel,
           const ProgressCallback& progress)
{
    std::vector<TwistChain> chains;
    for (const std::string side : {"左", "右"})
        for (bool forearm : {false, true})
        {
            const std::string name = side + (forearm ? "手捩" : "腕捩");
            const int parent = target.Find(side + (forearm ? "ひじ" : "腕"));
            const int twist = target.Find(name), child = target.Find(side + (forearm ? "手首" : "ひじ"));
            if (parent < 0 || twist < 0 || child < 0)
            { analysis.warnings.push_back("Twist skipped (missing chain): " + name); continue; }
            const auto& p = target.model.m_bones[static_cast<size_t>(parent)];
            const auto& t = target.model.m_bones[static_cast<size_t>(twist)];
            const auto& c = target.model.m_bones[static_cast<size_t>(child)];
            const Vector axis = (c.m_position - p.m_position).cast<double>();
            if (t.m_parentBoneIndex != parent || c.m_parentBoneIndex != twist || axis.squaredNorm() < 1.e-16 || !target.SupportedChain(child))
            { analysis.warnings.push_back("Twist skipped (requires serial chain): " + name); continue; }
            // Append-driven channels are evaluated but cannot be directly
            // controlled through the existing VMD importer.
            if ((static_cast<uint16_t>(p.m_boneFlag) & 0x700u) != 0 ||
                (static_cast<uint16_t>(t.m_boneFlag) & 0x300u) != 0 ||
                (static_cast<uint16_t>(c.m_boneFlag) & 0x700u) != 0)
            { analysis.warnings.push_back("Twist skipped (driven control channel): " + name); continue; }
            Vector twistAxis = axis.normalized();
            if ((static_cast<uint16_t>(t.m_boneFlag) & 0x400u) != 0)
            {
                Require(t.m_fixedAxis.allFinite() && t.m_fixedAxis.squaredNorm() > 1.e-16f, "Invalid twist fixed axis: " + name);
                twistAxis = t.m_fixedAxis.cast<double>().normalized();
            }
            chains.push_back({parent, twist, child, twistAxis});
        }
    if (chains.empty()) return;
    const Motion original(motion);
    const uint32_t last = BakeLimit(original, options, chains.size() * 3);
    Baker baker;
    for (uint32_t frame = 0; frame <= last; ++frame)
    {
        CheckCancel(cancel);
        ReportProgress(progress, ProgressPhase::Twist, frame, uint64_t(last) + 1);
        Pose pose(target, original, frame);
        for (const auto& chain : chains)
        {
            const size_t p = static_cast<size_t>(chain.parent), t = static_cast<size_t>(chain.twist), c = static_cast<size_t>(chain.child);
            const Rotation oldParent = pose.local[p].rotation.normalized();
            const Rotation oldTwist = pose.local[t].rotation.normalized();
            const Rotation oldChild = pose.local[c].rotation.normalized();
            const Vector a = (target.model.m_bones[t].m_position - target.model.m_bones[p].m_position).cast<double>() + pose.local[t].translation;
            const Vector b = (target.model.m_bones[c].m_position - target.model.m_bones[t].m_position).cast<double>() + pose.local[c].translation;
            const Vector endpoint = oldParent * (a + oldTwist * b);
            const Vector projected = chain.axis * oldParent.vec().dot(chain.axis);
            Rotation transfer(oldParent.w(), projected.x(), projected.y(), projected.z());
            if (transfer.squaredNorm() < 1.e-16) transfer = Rotation::Identity();
            else transfer.normalize();
            const auto candidate = [&](double fraction) {
                const Rotation delta = Rotation::Identity().slerp(fraction, transfer).normalized();
                return std::make_pair(delta, (delta * oldTwist).normalized());
            };
            const auto error = [&](double fraction) {
                const auto value = candidate(fraction);
                // Parent rotation aligns the endpoint but cannot change its
                // radius: bound the irreducible error of an offset twist axis.
                return std::abs((a + value.second * b).norm() - endpoint.norm());
            };
            double fraction = 1.;
            if (error(fraction) > options.tolerance * .5)
            {
                double low = 0., high = 1.;
                for (int iteration = 0; iteration < 24; ++iteration)
                {
                    const double mid = (low + high) * .5;
                    if (error(mid) <= options.tolerance * .5) low = mid; else high = mid;
                }
                fraction = low;
            }
            const auto value = candidate(fraction);
            Rotation nextParent = (oldParent * value.first.conjugate()).normalized();
            const Vector moved = nextParent * (a + value.second * b);
            if (moved.squaredNorm() > 1.e-16 && endpoint.squaredNorm() > 1.e-16)
                nextParent = (Rotation::FromTwoVectors(moved, endpoint) * nextParent).normalized();
            const Vector before = pose.positions[c];
            pose.local[p].rotation = nextParent;
            pose.local[t].rotation = value.second;
            // Keep the child's orientation, hence all downstream local poses.
            pose.local[c].rotation = ((nextParent * value.second).conjugate() * oldParent * oldTwist * oldChild).normalized();
            pose.Update();
            Record(analysis, options, Stage::Twist, frame, target.model.m_bones[c].m_name, before, pose.positions[c]);
            baker.Store(pose, chain.parent, frame);
            baker.Store(pose, chain.twist, frame);
            baker.Store(pose, chain.child, frame);
        }
    }
    baker.Finish(motion);
    ReportProgress(progress, ProgressPhase::Twist, uint64_t(last) + 1, uint64_t(last) + 1);
}
}

void ApplyStanceAndTwist(const Rig& source, const Rig& target, const Options& options,
                         Result& result, const std::atomic_bool* cancel, const ProgressCallback& progress)
{
    auto& stance = result.stages[static_cast<size_t>(Stage::Stance)];
    stance = result.stages[static_cast<size_t>(Stage::Offset)];
    if (options.stance)
    {
        ReportProgress(progress, ProgressPhase::Stance);
        Stance(source, target, stance, result.analysis);
        ReportProgress(progress, ProgressPhase::Stance, 1, 1);
    }
    CheckCancel(cancel);
    auto& twist = result.stages[static_cast<size_t>(Stage::Twist)];
    twist = stance;
    if (options.twist) Twist(target, twist, options, result.analysis, cancel, progress);
}
}
