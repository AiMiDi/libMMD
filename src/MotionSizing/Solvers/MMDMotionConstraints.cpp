#include "MMDMotionConstraints.h"
#include <algorithm>
#include <cmath>
#include <memory>

namespace libmmd::sizing::detail
{
std::vector<Effector> Effectors(const Rig& source, const Rig& target, const Options& options, Analysis& analysis)
{
    std::vector<Effector> output;
    for (const std::string side : {"左", "右"})
    {
        std::vector<std::string> names{side + "手首"};
        if (options.fingerContact)
            for (const auto* finger : {"親指２", "人指３", "中指３", "薬指３", "小指３"}) names.push_back(side + finger);
        for (const auto& name : names)
        {
            Effector effector;
            effector.name = name;
            effector.source = source.Find(name);
            effector.target = target.Find(name);
            effector.finger = name != side + "手首";
            const int arm = target.Find(side + "腕");
            if (effector.source < 0 || effector.target < 0 || arm < 0 || !target.Ancestor(arm, effector.target) ||
                !source.SupportedChain(effector.source) || !target.SupportedChain(effector.target))
            { analysis.warnings.push_back("Constraint skipped (missing/unsupported arm chain): " + name); continue; }
            for (int node = target.model.m_bones[static_cast<size_t>(effector.target)].m_parentBoneIndex; node >= 0;
                 node = target.model.m_bones[static_cast<size_t>(node)].m_parentBoneIndex)
            {
                // Only bake writable rotations; FK evaluates append helpers.
                const auto flags = static_cast<uint16_t>(target.model.m_bones[static_cast<size_t>(node)].m_boneFlag);
                if ((flags & 0x300u) == 0 && (flags & 2u) != 0) effector.joints.push_back(node);
                if (node == arm) break;
            }
            if (!effector.joints.empty()) output.push_back(std::move(effector));
        }
    }
    return output;
}

Vector ProjectOutside(const libmmd::PMXRigidbody& body, const Pose& pose, const Vector& point, double margin)
{
    const Vector size = body.m_shapeSize.cast<double>();
    Require(size.allFinite() && (size.array() >= 0).all() && body.m_translate.allFinite() && body.m_rotate.allFinite(),
            "Invalid avoidance rigid body: " + body.m_name);
    Require(body.m_boneIndex >= -1 && body.m_boneIndex < static_cast<int>(pose.local.size()), "Invalid rigid-body bone index");
    Rotation rotation = Rotation(Eigen::AngleAxisd(body.m_rotate.z(), Vector::UnitZ()) *
        Eigen::AngleAxisd(body.m_rotate.y(), Vector::UnitY()) * Eigen::AngleAxisd(body.m_rotate.x(), Vector::UnitX()));
    Vector center = body.m_translate.cast<double>();
    if (body.m_boneIndex >= 0)
    {
        const size_t bone = static_cast<size_t>(body.m_boneIndex);
        center = pose.positions[bone] + pose.rotations[bone] * (center - pose.rig.model.m_bones[bone].m_position.cast<double>());
        rotation = pose.rotations[bone] * rotation;
    }
    const Vector local = rotation.conjugate() * (point - center);
    Vector projected = local;
    if (body.m_shape == libmmd::PMXRigidbody::Shape::Box)
    {
        const Vector half = size + Vector::Constant(margin);
        if ((local.cwiseAbs().array() >= half.array()).any()) return point;
        Eigen::Index axis = 0;
        (half - local.cwiseAbs()).minCoeff(&axis);
        projected[axis] = (local[axis] < 0 ? -1. : 1.) * half[axis];
    }
    else if (body.m_shape == libmmd::PMXRigidbody::Shape::Sphere || body.m_shape == libmmd::PMXRigidbody::Shape::Capsule)
    {
        Vector nearest = Vector::Zero();
        if (body.m_shape == libmmd::PMXRigidbody::Shape::Capsule)
            nearest.y() = std::clamp(local.y(), -size.y() / 2., size.y() / 2.);
        const Vector radial = local - nearest;
        const double radius = size.x() + margin;
        if (radial.norm() >= radius) return point;
        const Vector direction = radial.squaredNorm() < 1.e-16 ? Vector::UnitX().eval() : radial.normalized().eval();
        projected = nearest + direction * radius;
    }
    else throw std::runtime_error("Unsupported avoidance shape");
    return center + rotation * projected;
}

namespace
{
std::set<int> ControlledBones(const std::vector<Effector>& effectors)
{
    std::set<int> bones;
    for (const auto& effector : effectors) bones.insert(effector.joints.begin(), effector.joints.end());
    return bones;
}

std::vector<size_t> AvoidanceBodies(const Rig& rig, const Options& options, Analysis& analysis)
{
    std::vector<size_t> indices;
    for (size_t i = 0; i < rig.model.m_rigidbodies.size(); ++i)
    {
        const auto& body = rig.model.m_rigidbodies[i];
        Require(body.m_boneIndex >= -1 && body.m_boneIndex < static_cast<int>(rig.model.m_bones.size()), "Invalid rigid-body bone index");
        const bool selected = options.avoidanceBodies.empty() ? body.m_op == libmmd::PMXRigidbody::Operation::Static :
            std::find(options.avoidanceBodies.begin(), options.avoidanceBodies.end(), body.m_name) != options.avoidanceBodies.end();
        if (selected)
        {
            if (body.m_boneIndex >= 0 && !rig.SupportedChain(body.m_boneIndex))
                analysis.warnings.push_back("Avoidance skipped (unsupported body chain): " + body.m_name);
            else indices.push_back(i);
        }
    }
    for (const auto& name : options.avoidanceBodies)
        Require(std::any_of(rig.model.m_rigidbodies.begin(), rig.model.m_rigidbodies.end(),
            [&](const auto& body) { return body.m_name == name; }), "Selected avoidance body missing: " + name);
    if (indices.empty()) analysis.warnings.push_back("Avoidance has no eligible rigid bodies");
    return indices;
}

void Avoid(const Rig& rig, libmmd::VMDFile& motion, const std::vector<Effector>& effectors,
           const Options& options, Analysis& analysis, const std::atomic_bool* cancel)
{
    const auto bodies = AvoidanceBodies(rig, options, analysis);
    if (bodies.empty() || effectors.empty()) return;
    const Motion input(motion);
    const auto controlled = ControlledBones(effectors);
    const uint32_t last = BakeLimit(input, options, controlled.size());
    Baker baker;
    for (uint32_t frame = 0; frame <= last; ++frame)
    {
        CheckCancel(cancel);
        Pose pose(rig, input, frame);
        // Multiple passes let later obstacle pushes settle against earlier ones.
        // Record only the final residual, not transient inner-iteration errors.
        for (unsigned pass = 0; pass < 4; ++pass)
            for (const auto& effector : effectors)
            {
                if (effector.finger) continue;
                std::vector<int> points{effector.target};
                const int elbow = rig.Find(effector.name.substr(0, 3) + "ひじ");
                if (elbow >= 0) points.push_back(elbow);
                for (int point : points)
                {
                    std::vector<int> joints;
                    for (int joint : effector.joints)
                        if (joint != point && rig.Ancestor(joint, point)) joints.push_back(joint);
                    for (size_t bodyIndex : bodies)
                    {
                        const auto& body = rig.model.m_rigidbodies[bodyIndex];
                        // An arm must not push away from a collider on itself.
                        if (body.m_boneIndex >= 0 && rig.Ancestor(effector.joints.back(), body.m_boneIndex)) continue;
                        const Vector position = pose.positions[static_cast<size_t>(point)];
                        const Vector target = ProjectOutside(body, pose, position, options.collisionMargin);
                        if ((position - target).norm() > options.tolerance) Solve(pose, point, joints, target, options, cancel);
                    }
                }
            }
        for (const auto& effector : effectors)
        {
            if (effector.finger) continue;
            for (int point : {effector.target, rig.Find(effector.name.substr(0, 3) + "ひじ")})
            {
                if (point < 0) continue;
                for (size_t bodyIndex : bodies)
                {
                    const auto& body = rig.model.m_rigidbodies[bodyIndex];
                    if (body.m_boneIndex >= 0 && rig.Ancestor(effector.joints.back(), body.m_boneIndex)) continue;
                    const Vector actual = pose.positions[static_cast<size_t>(point)];
                    Record(analysis, options, Stage::Avoidance, frame, rig.model.m_bones[static_cast<size_t>(point)].m_name,
                           ProjectOutside(body, pose, actual, options.collisionMargin), actual);
                }
            }
        }
        for (int bone : controlled) baker.Store(pose, bone, frame);
    }
    baker.Finish(motion);
}

struct Pair { size_t first, second; Vector target; };

void Contact(const Rig& source, const Rig& target, const libmmd::VMDFile& original, libmmd::VMDFile& output,
             const std::vector<Effector>& effectors, const Options& options, Analysis& analysis, const std::atomic_bool* cancel)
{
    const Motion sourceMotion(original), targetMotion(output);
    auto controlled = ControlledBones(effectors);
    std::vector<std::pair<int, int>> feet;
    if (options.floorContact)
        for (const auto* name : {"左足ＩＫ", "右足ＩＫ"})
        {
            const int s = source.Find(name), t = target.Find(name);
            if (s >= 0 && t >= 0 && source.SupportedChain(s) && target.SupportedChain(t))
            { feet.emplace_back(s, t); controlled.insert(t); }
        }
    if (controlled.empty()) return;
    const uint32_t last = BakeLimit(targetMotion, options, controlled.size());
    Baker baker;
    for (uint32_t frame = 0; frame <= last; ++frame)
    {
        CheckCancel(cancel);
        Pose sourcePose(source, sourceMotion, frame), pose(target, targetMotion, frame);
        std::vector<Pair> pairs;
        for (size_t a = 0; a < effectors.size(); ++a)
            for (size_t b = a + 1; b < effectors.size(); ++b)
            {
                const auto& first = effectors[a]; const auto& second = effectors[b];
                if (first.name.substr(0, 3) == second.name.substr(0, 3) || first.finger != second.finger ||
                    (first.finger ? !options.fingerContact : !options.wristContact)) continue;
                if ((sourcePose.positions[static_cast<size_t>(first.source)] - sourcePose.positions[static_cast<size_t>(second.source)]).norm() <= options.contactDistance)
                    pairs.push_back({a, b, (pose.positions[static_cast<size_t>(first.target)] + pose.positions[static_cast<size_t>(second.target)]) * .5});
            }
        // Connected finger contacts share one goal. A finger touching two
        // others cannot simultaneously reach two different pair midpoints.
        std::vector<size_t> parents(effectors.size());
        std::vector<bool> active(effectors.size(), false);
        for (size_t i = 0; i < parents.size(); ++i) parents[i] = i;
        const auto root = [&](size_t node) { while (parents[node] != node) node = parents[node]; return node; };
        for (const auto& pair : pairs)
        {
            parents[root(pair.second)] = root(pair.first);
            active[pair.first] = active[pair.second] = true;
        }
        std::map<size_t, std::pair<Vector, size_t>> centers;
        for (size_t i = 0; i < effectors.size(); ++i)
            if (active[i])
            {
                auto center = centers.emplace(root(i), std::make_pair(Vector::Zero().eval(), size_t(0)));
                center.first->second.first += pose.positions[static_cast<size_t>(effectors[i].target)];
                ++center.first->second.second;
            }
        for (auto& pair : pairs)
        {
            const auto& center = centers.at(root(pair.first));
            pair.target = center.first / static_cast<double>(center.second);
        }
        std::vector<std::pair<size_t, Vector>> floors;
        if (options.floorContact)
            for (size_t i = 0; i < effectors.size(); ++i)
                if (std::abs(sourcePose.positions[static_cast<size_t>(effectors[i].source)].y() - options.floorHeight) <= options.contactDistance)
                {
                    Vector goal = pose.positions[static_cast<size_t>(effectors[i].target)];
                    goal.y() = options.floorHeight;
                    floors.emplace_back(i, goal);
                }
        // Simultaneous position goals prevent later fingers from undoing an
        // earlier finger through their shared arm. Average duplicate targets.
        std::map<size_t, std::pair<Vector, size_t>> accumulated;
        const auto addGoal = [&](size_t index, const Vector& goal) {
            auto entry = accumulated.emplace(index, std::make_pair(Vector::Zero().eval(), size_t(0)));
            entry.first->second.first += goal; ++entry.first->second.second;
        };
        for (const auto& pair : pairs)
            for (size_t index : {pair.first, pair.second}) addGoal(index, pair.target);
        for (const auto& floor : floors) addGoal(floor.first, floor.second);
        std::vector<Goal> goals;
        for (const auto& entry : accumulated)
        {
            const auto& effector = effectors[entry.first];
            goals.push_back({effector.target, effector.joints, entry.second.first / static_cast<double>(entry.second.second)});
        }
        SolveGoals(pose, goals, options, cancel);
        for (const auto& pair : pairs)
            for (size_t index : {pair.first, pair.second})
                Record(analysis, options, Stage::Contact, frame, effectors[index].name, pair.target, pose.positions[static_cast<size_t>(effectors[index].target)]);
        for (const auto& floor : floors)
            Record(analysis, options, Stage::Contact, frame, effectors[floor.first].name, floor.second, pose.positions[static_cast<size_t>(effectors[floor.first].target)]);
        for (const auto& foot : feet)
        {
            const size_t s = static_cast<size_t>(foot.first), t = static_cast<size_t>(foot.second);
            // IK goals are ankle-height controls. Ground contact is displacement
            // from the source bind ankle plane, not a forced ankle at y = 0.
            const double height = sourcePose.positions[s].y() - source.model.m_bones[s].m_position.y();
            if (std::abs(height - options.floorHeight) <= options.contactDistance)
            {
                Vector goal = pose.positions[t];
                goal.y() = target.model.m_bones[t].m_position.y() + options.floorHeight;
                const int parent = target.model.m_bones[t].m_parentBoneIndex;
                const Rotation parentRotation = parent < 0 ? Rotation::Identity() : pose.rotations[static_cast<size_t>(parent)];
                pose.local[t].translation += parentRotation.conjugate() * (goal - pose.positions[t]);
                pose.Update();
                Record(analysis, options, Stage::Contact, frame, target.model.m_bones[t].m_name, goal, pose.positions[t]);
            }
        }
        for (int bone : controlled) baker.Store(pose, bone, frame);
    }
    baker.Finish(output);
}
}

void ApplyConstraints(const Rig& source, const Rig& target, const Options& options, Result& result, const std::atomic_bool* cancel)
{
    const bool enabled = options.avoidance || options.wristContact || options.fingerContact || options.floorContact;
    const auto effectors = enabled ? Effectors(source, target, options, result.analysis) : std::vector<Effector>();
    auto& avoid = result.stages[static_cast<size_t>(Stage::Avoidance)];
    avoid = result.stages[static_cast<size_t>(Stage::Twist)];
    if (options.avoidance) Avoid(target, avoid, effectors, options, result.analysis, cancel);
    auto& contact = result.stages[static_cast<size_t>(Stage::Contact)];
    contact = avoid;
    if (options.wristContact || options.fingerContact || options.floorContact)
        Contact(source, target, result.stages[0], contact, effectors, options, result.analysis, cancel);
    result.stages[static_cast<size_t>(Stage::MultiCharacter)] = contact;
}

void ApplyBatchConstraints(const std::vector<CharacterInput>& input, BatchResult& result, const std::atomic_bool* cancel)
{
    if (input.size() < 2) return;
    std::vector<std::unique_ptr<Rig>> sources, targets;
    std::vector<Motion> originals, motions;
    std::vector<std::vector<Effector>> effectors;
    std::vector<Baker> bakers(input.size());
    uint32_t last = 0;
    for (size_t i = 0; i < input.size(); ++i)
    {
        sources.emplace_back(std::make_unique<Rig>(input[i].source));
        targets.emplace_back(std::make_unique<Rig>(input[i].target));
        originals.emplace_back(input[i].motion);
        motions.emplace_back(result.characters[i].stages[static_cast<size_t>(Stage::Contact)]);
        effectors.push_back(input[i].options.multiContact ? Effectors(*sources.back(), *targets.back(), input[i].options, result.characters[i].analysis) : std::vector<Effector>());
        last = std::max(last, motions.back().LastFrame());
    }
    for (size_t i = 0; i < input.size(); ++i)
        Require(static_cast<uint64_t>(last) + 1 <= input[i].options.maxBakeFrames &&
                ControlledBones(effectors[i]).size() <= input[i].options.maxBakedKeys / (static_cast<uint64_t>(last) + 1), "Batch exceeds bake budget");
    struct CrossPair { size_t a, ai, b, bi; Vector goal; };
    for (uint32_t frame = 0; frame <= last; ++frame)
    {
        CheckCancel(cancel);
        std::vector<Pose> sourcePoses, poses;
        for (size_t i = 0; i < input.size(); ++i)
        { sourcePoses.emplace_back(*sources[i], originals[i], frame); poses.emplace_back(*targets[i], motions[i], frame); }
        std::vector<CrossPair> pairs;
        for (size_t a = 0; a < input.size(); ++a)
            for (size_t b = a + 1; b < input.size(); ++b)
                for (size_t ai = 0; ai < effectors[a].size(); ++ai)
                    for (size_t bi = 0; bi < effectors[b].size(); ++bi)
                    {
                        const auto& first = effectors[a][ai]; const auto& second = effectors[b][bi];
                        if (first.finger != second.finger) continue;
                        const double threshold = std::min(input[a].options.contactDistance, input[b].options.contactDistance);
                        if ((sourcePoses[a].positions[static_cast<size_t>(first.source)] - sourcePoses[b].positions[static_cast<size_t>(second.source)]).norm() <= threshold)
                            pairs.push_back({a, ai, b, bi, (poses[a].positions[static_cast<size_t>(first.target)] + poses[b].positions[static_cast<size_t>(second.target)]) * .5});
                    }
        // A hand may touch several other hands. All edges in a connected
        // contact group must share one goal; independent pair midpoints can
        // issue contradictory targets for the same joint.
        using Node = std::pair<size_t, size_t>;
        std::map<Node, Node> parents;
        for (const auto& pair : pairs)
        {
            const Node a{pair.a, pair.ai}, b{pair.b, pair.bi};
            parents.emplace(a, a); parents.emplace(b, b);
        }
        const auto root = [&](Node node) { while (parents.at(node) != node) node = parents.at(node); return node; };
        for (const auto& pair : pairs) parents[root({pair.b, pair.bi})] = root({pair.a, pair.ai});
        std::map<Node, std::pair<Vector, size_t>> centers;
        for (const auto& entry : parents)
        {
            const auto representative = root(entry.first);
            if (!centers.count(representative)) centers.emplace(representative, std::make_pair(Vector::Zero().eval(), size_t(0)));
            auto& center = centers.at(representative);
            center.first += poses[entry.first.first].positions[static_cast<size_t>(effectors[entry.first.first][entry.first.second].target)];
            ++center.second;
        }
        for (auto& pair : pairs)
        {
            const auto& center = centers.at(root({pair.a, pair.ai}));
            pair.goal = center.first / static_cast<double>(center.second);
        }
        for (size_t i = 0; i < input.size(); ++i)
        {
            std::map<size_t, Vector> uniqueGoals;
            for (const auto& pair : pairs)
            {
                if (pair.a == i) uniqueGoals[pair.ai] = pair.goal;
                if (pair.b == i) uniqueGoals[pair.bi] = pair.goal;
            }
            std::vector<Goal> goals;
            for (const auto& goal : uniqueGoals)
                goals.push_back({effectors[i][goal.first].target, effectors[i][goal.first].joints, goal.second});
            SolveGoals(poses[i], goals, input[i].options, cancel);
        }
        for (const auto& pair : pairs)
        {
            const auto& a = effectors[pair.a][pair.ai]; const auto& b = effectors[pair.b][pair.bi];
            Record(result.characters[pair.a].analysis, input[pair.a].options, Stage::MultiCharacter, frame, a.name, pair.goal, poses[pair.a].positions[static_cast<size_t>(a.target)]);
            Record(result.characters[pair.b].analysis, input[pair.b].options, Stage::MultiCharacter, frame, b.name, pair.goal, poses[pair.b].positions[static_cast<size_t>(b.target)]);
        }
        for (size_t i = 0; i < input.size(); ++i)
            for (int bone : ControlledBones(effectors[i])) bakers[i].Store(poses[i], bone, frame);
    }
    for (size_t i = 0; i < input.size(); ++i) bakers[i].Finish(result.characters[i].stages[static_cast<size_t>(Stage::MultiCharacter)]);
}
}
