#include "MMDMotionConstraints.h"
#include "MMDMotionLegs.h"
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
        const int sourceWrist = source.Find(side + "手首"), targetWrist = target.Find(side + "手首");
        if (sourceWrist < 0 || targetWrist < 0) continue;
        // Normalize small contact offsets by palm size, not overall height.
        // Older rigs without palm landmarks fall back to forearm length.
        double handScale = 1.;
        for (const auto* landmark : {"中指１", "人指１", "ひじ"})
        {
            const int s = source.Find(side + landmark), t = target.Find(side + landmark);
            if (s < 0 || t < 0) continue;
            const double sourceLength = (source.model.m_bones[s].m_position - source.model.m_bones[sourceWrist].m_position).norm();
            const double targetLength = (target.model.m_bones[t].m_position - target.model.m_bones[targetWrist].m_position).norm();
            if (sourceLength > 1.e-8 && targetLength > 1.e-8) { handScale = targetLength / sourceLength; break; }
        }
        std::vector<std::string> names{side + "手首"};
        if (options.fingerContact)
            for (const auto* finger : {"親指２", "人指３", "中指３", "薬指３", "小指３"}) names.push_back(side + finger);
        for (const auto& name : names)
        {
            Effector effector;
            effector.name = name;
            effector.source = source.Find(name);
            effector.target = target.Find(name);
            effector.wrist = targetWrist;
            effector.handScale = handScale;
            effector.finger = name != side + "手首";
            const int arm = target.Find(side + "腕");
            if (effector.source < 0 || effector.target < 0 || arm < 0 || !target.Ancestor(arm, targetWrist) ||
                !source.Ancestor(sourceWrist, effector.source) || !target.Ancestor(targetWrist, effector.target) ||
                source.ikAffected[effector.source] || target.ikAffected[effector.target] ||
                !source.SupportedChain(effector.source) || !target.SupportedChain(effector.target))
            { analysis.warnings.push_back("Constraint skipped (missing/unsupported arm chain): " + name); continue; }
            // Contact moves the hand as a unit. Never use finger articulation
            // as extra degrees of freedom for making positional errors smaller.
            for (int node = target.model.m_bones[static_cast<size_t>(targetWrist)].m_parentBoneIndex; node >= 0;
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
           const Options& options, Analysis& analysis, const std::atomic_bool* cancel, const ProgressCallback& progress)
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
        ReportProgress(progress, ProgressPhase::Avoidance, frame, uint64_t(last) + 1);
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
    ReportProgress(progress, ProgressPhase::Avoidance, uint64_t(last) + 1, uint64_t(last) + 1);
}

struct ContactPoint { Vector source, target; double scale; };
using ContactEdge = std::pair<size_t, size_t>;

std::map<size_t, Vector> ContactGoals(const std::vector<ContactPoint>& points, const std::vector<ContactEdge>& edges)
{
    std::vector<size_t> parents(points.size());
    std::set<size_t> active;
    for (size_t i = 0; i < parents.size(); ++i) parents[i] = i;
    const auto root = [&](size_t node) { while (parents[node] != node) node = parents[node]; return node; };
    for (const auto& edge : edges)
    {
        parents[root(edge.second)] = root(edge.first);
        active.insert(edge.first); active.insert(edge.second);
    }
    struct Group { Vector source = Vector::Zero(), target = Vector::Zero(); double scale = 0.; size_t count = 0; };
    std::map<size_t, Group> groups;
    for (size_t i : active)
    {
        auto& group = groups[root(i)];
        group.source += points[i].source; group.target += points[i].target;
        group.scale += points[i].scale; ++group.count;
    }
    std::map<size_t, Vector> goals;
    for (size_t i : active)
    {
        const auto& group = groups.at(root(i));
        const double count = static_cast<double>(group.count);
        // One centroid anchors the group, but each landmark keeps its own
        // source offset. Nearby fingers/wrists must not collapse to one point.
        goals.emplace(i, group.target / count + (points[i].source - group.source / count) * (group.scale / count));
    }
    return goals;
}

std::set<int> ControlledHands(const std::vector<Effector>& effectors)
{
    auto bones = ControlledBones(effectors);
    for (const auto& effector : effectors) bones.insert(effector.wrist);
    return bones;
}

// Preserve the existing avoidance result at the wrist, elbow and sampled arm
// segments. This is a conservative acceptance guard, not a mesh collision solver.
std::vector<double> HandPenetrations(const Pose& pose, const std::vector<Effector>& effectors,
                                   const std::vector<size_t>& bodies, const Options& options)
{
    std::vector<double> depths;
    for (const auto& effector : effectors)
    {
        if (effector.finger) continue;
        const int arm = effector.joints.back(), elbow = pose.rig.Find(effector.name.substr(0, 3) + "ひじ");
        std::vector<Vector> points{pose.positions[effector.wrist]};
        if (elbow >= 0)
        {
            points.push_back(pose.positions[elbow]);
            for (int sample = 1; sample <= 3; ++sample)
            {
                const double alpha = sample / 4.;
                points.push_back((1. - alpha) * pose.positions[arm] + alpha * pose.positions[elbow]);
                points.push_back((1. - alpha) * pose.positions[elbow] + alpha * pose.positions[effector.wrist]);
            }
        }
        for (size_t index : bodies)
        {
            const auto& body = pose.rig.model.m_rigidbodies[index];
            if (body.m_boneIndex >= 0 && pose.rig.Ancestor(arm, body.m_boneIndex)) continue;
            for (const auto& point : points)
                depths.push_back((ProjectOutside(body, pose, point, options.collisionMargin) - point).norm());
        }
    }
    return depths;
}

void SolveHandContacts(Pose& pose, const std::vector<Effector>& effectors, const std::map<size_t, Vector>& targets,
                       const std::vector<size_t>& bodies, const Options& options, const std::atomic_bool* cancel)
{
    if (targets.empty()) return;
    const auto before = pose.local;
    const auto orientations = pose.rotations;
    const auto penetrations = HandPenetrations(pose, effectors, bodies, options);
    std::map<int, std::pair<Vector, size_t>> wristTargets;
    std::map<int, std::vector<int>> chains;
    for (const auto& entry : targets)
    {
        const auto& effector = effectors[entry.first];
        const auto flags = static_cast<uint16_t>(pose.rig.model.m_bones[effector.wrist].m_boneFlag);
        // A constrained/append-driven wrist cannot be freely counter-rotated.
        // Keep that hand unchanged and let diagnostics expose the residual.
        if ((flags & 2u) == 0 || (flags & 0x700u) != 0) continue;
        auto accumulated = wristTargets.emplace(effector.wrist, std::make_pair(Vector::Zero().eval(), size_t(0)));
        accumulated.first->second.first += entry.second - pose.positions[effector.target] + pose.positions[effector.wrist];
        ++accumulated.first->second.second;
        chains[effector.wrist] = effector.joints;
    }
    std::vector<Goal> goals;
    for (const auto& entry : wristTargets)
        goals.push_back({entry.first, chains.at(entry.first), entry.second.first / static_cast<double>(entry.second.second)});
    const auto error = [&]() {
        double sum = 0.;
        for (const auto& entry : targets) sum += (entry.second - pose.positions[effectors[entry.first].target]).squaredNorm();
        return sum;
    };
    const double initialError = error();
    constexpr double maximumAngle = .5235987755982988; // 30 degrees from the incoming pose, not per iteration.
    SolveGoals(pose, goals, options, cancel, maximumAngle);
    const auto solved = pose.local;
    for (int search = 0; search < 16; ++search)
    {
        CheckCancel(cancel);
        const double weight = std::ldexp(1., -search);
        pose.local = before;
        for (const auto& chain : chains)
            for (int joint : chain.second)
                pose.local[joint].rotation = before[joint].rotation.slerp(weight, solved[joint].rotation).normalized();
        pose.Update();
        bool bounded = true;
        for (const auto& entry : wristTargets)
        {
            const int wrist = entry.first, parent = pose.rig.model.m_bones[wrist].m_parentBoneIndex;
            const Rotation parentRotation = parent < 0 ? Rotation::Identity() : pose.rotations[parent];
            pose.local[wrist].rotation = (parentRotation.conjugate() * orientations[wrist]).normalized();
            bounded &= before[wrist].rotation.angularDistance(pose.local[wrist].rotation) <= maximumAngle + 1.e-9;
        }
        pose.Update();
        if (!bounded || error() > initialError + 1.e-12) continue;
        const auto candidateDepths = HandPenetrations(pose, effectors, bodies, options);
        bool collisionSafe = true;
        for (size_t i = 0; i < candidateDepths.size(); ++i)
            if (candidateDepths[i] > std::max(penetrations[i], options.tolerance) + 1.e-8) { collisionSafe = false; break; }
        if (collisionSafe) return;
    }
    pose.local = before;
    pose.Update();
}

void Contact(const Rig& source, const Rig& target, const libmmd::VMDFile& original, libmmd::VMDFile& output,
             const std::vector<Effector>& effectors, const Options& options, Analysis& analysis, const std::atomic_bool* cancel,
             const ProgressCallback& progress)
{
    const Motion sourceMotion(original), targetMotion(output);
    auto controlled = ControlledHands(effectors);
    const auto bodies = options.avoidance ? AvoidanceBodies(target, options, analysis) : std::vector<size_t>();
    const auto legPairs = options.legAvoidance ? LegCollisionPairs(target) : std::vector<LegPair>();
    std::vector<std::pair<int, int>> feet;
    if (options.floorContact)
        for (const auto* name : {"左足ＩＫ", "右足ＩＫ"})
        {
            const int s = source.Find(name), t = target.Find(name);
            if (s >= 0 && t >= 0 && source.SupportedChain(s) && target.SupportedChain(t))
            {
                const auto flags = static_cast<uint16_t>(target.model.m_bones[t].m_boneFlag);
                if ((flags & 0x304u) != 0x4u || target.ikAffected[t])
                { analysis.warnings.push_back(std::string("Floor contact skipped (driven/non-translatable foot): ") + name); continue; }
                feet.emplace_back(s, t); controlled.insert(t);
            }
        }
    if (controlled.empty()) return;
    const uint32_t last = BakeLimit(targetMotion, options, controlled.size());
    Baker baker;
    for (uint32_t frame = 0; frame <= last; ++frame)
    {
        CheckCancel(cancel);
        Pose sourcePose(source, sourceMotion, frame), pose(target, targetMotion, frame);
        ReportProgress(progress, ProgressPhase::Contact, frame, uint64_t(last) + 1);
        std::vector<ContactPoint> points;
        for (const auto& effector : effectors)
            points.push_back({sourcePose.positions[effector.source], pose.positions[effector.target], effector.handScale});
        std::vector<ContactEdge> pairs;
        for (size_t a = 0; a < effectors.size(); ++a)
            for (size_t b = a + 1; b < effectors.size(); ++b)
            {
                const auto& first = effectors[a]; const auto& second = effectors[b];
                if (first.name.substr(0, 3) == second.name.substr(0, 3) || first.finger != second.finger ||
                    (first.finger ? !options.fingerContact : !options.wristContact)) continue;
                if ((sourcePose.positions[static_cast<size_t>(first.source)] - sourcePose.positions[static_cast<size_t>(second.source)]).norm() <= options.contactDistance)
                    pairs.emplace_back(a, b);
            }
        const auto contactGoals = ContactGoals(points, pairs);
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
        for (const auto& goal : contactGoals) addGoal(goal.first, goal.second);
        for (const auto& floor : floors) addGoal(floor.first, floor.second);
        std::map<size_t, Vector> goals;
        for (const auto& entry : accumulated)
            goals.emplace(entry.first, entry.second.first / static_cast<double>(entry.second.second));
        SolveHandContacts(pose, effectors, goals, bodies, options, cancel);
        for (const auto& goal : contactGoals)
            Record(analysis, options, Stage::Contact, frame, effectors[goal.first].name, goal.second,
                   pose.positions[effectors[goal.first].target]);
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
                if (!sourcePose.ikEnabled[s] || !pose.ikEnabled[t]) continue;
                Vector goal = pose.positions[t];
                goal.y() = target.model.m_bones[t].m_position.y() + options.floorHeight;
                const int parent = target.model.m_bones[t].m_parentBoneIndex;
                const Rotation parentRotation = parent < 0 ? Rotation::Identity() : pose.rotations[static_cast<size_t>(parent)];
                const auto beforeFloor = pose.local[t].translation;
                const auto beforeDepths = LegPenetrations(pose, legPairs, options.collisionMargin);
                const auto& targetBone = target.model.m_bones[t];
                const int ankle = (static_cast<uint16_t>(targetBone.m_boneFlag) & 0x20u) != 0 ? targetBone.m_ikTargetBoneIndex : -1;
                const double beforeIK = ankle < 0 ? 0. : (pose.positions[ankle] - pose.positions[t]).norm();
                pose.local[t].translation += parentRotation.conjugate() * (goal - pose.positions[t]);
                pose.Update();
                const auto afterDepths = LegPenetrations(pose, legPairs, options.collisionMargin);
                bool rejected = ankle >= 0 && (pose.positions[ankle] - pose.positions[t]).norm() > beforeIK + options.tolerance;
                for (size_t i = 0; i < afterDepths.size(); ++i)
                    if (afterDepths[i] > std::max(beforeDepths[i], options.tolerance) + 1.e-8)
                    { rejected = true; break; }
                if (rejected) { pose.local[t].translation = beforeFloor; pose.Update(); }
                Record(analysis, options, Stage::Contact, frame, target.model.m_bones[t].m_name, goal,
                       pose.positions[ankle >= 0 ? ankle : static_cast<int>(t)]);
            }
        }
        for (int bone : controlled) baker.Store(pose, bone, frame);
    }
    baker.Finish(output);
    ReportProgress(progress, ProgressPhase::Contact, uint64_t(last) + 1, uint64_t(last) + 1);
}
}

void ApplyConstraints(const Rig& source, const Rig& target, const Options& options, Result& result, const std::atomic_bool* cancel,
                      const ProgressCallback& progress)
{
    const bool enabled = options.avoidance || options.wristContact || options.fingerContact || options.floorContact;
    const auto effectors = enabled ? Effectors(source, target, options, result.analysis) : std::vector<Effector>();
    auto& avoid = result.stages[static_cast<size_t>(Stage::Avoidance)];
    avoid = result.stages[static_cast<size_t>(Stage::Twist)];
    if (options.legAvoidance) ApplyLegAvoidance(target, avoid, options, result.analysis, cancel, progress);
    if (options.avoidance) Avoid(target, avoid, effectors, options, result.analysis, cancel, progress);
    auto& contact = result.stages[static_cast<size_t>(Stage::Contact)];
    contact = avoid;
    if (options.wristContact || options.fingerContact || options.floorContact)
        Contact(source, target, result.stages[0], contact, effectors, options, result.analysis, cancel, progress);
    result.stages[static_cast<size_t>(Stage::MultiCharacter)] = contact;
}

void ApplyBatchConstraints(const std::vector<CharacterInput>& input, BatchResult& result, const std::atomic_bool* cancel,
                           const ProgressCallback& progress)
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
                ControlledHands(effectors[i]).size() <= input[i].options.maxBakedKeys / (static_cast<uint64_t>(last) + 1), "Batch exceeds bake budget");
    std::vector<std::vector<size_t>> bodies;
    for (size_t i = 0; i < input.size(); ++i)
        bodies.push_back(input[i].options.avoidance ? AvoidanceBodies(*targets[i], input[i].options, result.characters[i].analysis) : std::vector<size_t>());
    for (uint32_t frame = 0; frame <= last; ++frame)
    {
        CheckCancel(cancel);
        std::vector<Pose> sourcePoses, poses;
        ReportProgress(progress, ProgressPhase::MultiCharacter, frame, uint64_t(last) + 1);
        for (size_t i = 0; i < input.size(); ++i)
        { sourcePoses.emplace_back(*sources[i], originals[i], frame); poses.emplace_back(*targets[i], motions[i], frame); }
        std::vector<ContactPoint> points;
        std::vector<size_t> offsets;
        for (size_t i = 0; i < input.size(); ++i)
        {
            offsets.push_back(points.size());
            for (const auto& effector : effectors[i])
                points.push_back({sourcePoses[i].positions[effector.source], poses[i].positions[effector.target], effector.handScale});
        }
        std::vector<ContactEdge> pairs;
        for (size_t a = 0; a < input.size(); ++a)
            for (size_t b = a + 1; b < input.size(); ++b)
                for (size_t ai = 0; ai < effectors[a].size(); ++ai)
                    for (size_t bi = 0; bi < effectors[b].size(); ++bi)
                    {
                        const auto& first = effectors[a][ai]; const auto& second = effectors[b][bi];
                        if (first.finger != second.finger) continue;
                        const double threshold = std::min(input[a].options.contactDistance, input[b].options.contactDistance);
                        if ((sourcePoses[a].positions[static_cast<size_t>(first.source)] - sourcePoses[b].positions[static_cast<size_t>(second.source)]).norm() <= threshold)
                            pairs.emplace_back(offsets[a] + ai, offsets[b] + bi);
                    }
        const auto contactGoals = ContactGoals(points, pairs);
        for (size_t i = 0; i < input.size(); ++i)
        {
            std::map<size_t, Vector> goals;
            for (size_t j = 0; j < effectors[i].size(); ++j)
            {
                const auto found = contactGoals.find(offsets[i] + j);
                if (found != contactGoals.end()) goals.emplace(j, found->second);
            }
            SolveHandContacts(poses[i], effectors[i], goals, bodies[i], input[i].options, cancel);
            for (const auto& goal : goals)
                Record(result.characters[i].analysis, input[i].options, Stage::MultiCharacter, frame,
                       effectors[i][goal.first].name, goal.second, poses[i].positions[effectors[i][goal.first].target]);
            for (int bone : ControlledHands(effectors[i])) bakers[i].Store(poses[i], bone, frame);
        }
    }
    for (size_t i = 0; i < input.size(); ++i) bakers[i].Finish(result.characters[i].stages[static_cast<size_t>(Stage::MultiCharacter)]);
    ReportProgress(progress, ProgressPhase::MultiCharacter, uint64_t(last) + 1, uint64_t(last) + 1);
}
}
