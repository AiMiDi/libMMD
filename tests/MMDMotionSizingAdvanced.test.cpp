#include "libMMD/Model/MMD/MMDMotionSizing.h"
#include "MMDMotionPose.h"
#include "MMDMotionConstraints.h"
#include "libMMD/Model/MMD/VMDInterpolation.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

void SizingAppendReference(double rotations[4][4], double translations[4][3]);

namespace
{
using namespace libmmd::sizing;
using namespace libmmd::sizing::detail;
void Check(bool condition, const char* message) { if (!condition) throw std::runtime_error(message); }
int Find(const libmmd::PMXFile& model, const std::string& name) { return Rig(model).Find(name); }
libmmd::VMDMotion Key(const std::string& name, uint32_t frame, const Rotation& rotation = Rotation::Identity())
{
    libmmd::VMDMotion key;
    std::u16string utf16;
    libmmd::ConvU8ToU16(name, utf16);
    key.m_boneName.Set(libmmd::ConvertU16ToSjisString(utf16).c_str());
    key.m_frame = frame; key.m_quaternion = rotation.cast<float>();
    for (size_t i = 0; i < 64; ++i) key.m_interpolation[i] = i % 16 < 8 ? 20 : 107;
    return key;
}
void AddFinger(libmmd::PMXFile& model, const std::string& side, float scaleX, float scaleY)
{
    libmmd::PMXBone bone{};
    bone.m_name = side + "中指１";
    bone.m_parentBoneIndex = Find(model, side + "手首");
    bone.m_position = model.m_bones[static_cast<size_t>(bone.m_parentBoneIndex)].m_position +
        Eigen::Vector3f(side == "左" ? .8f * scaleX : -.8f * scaleX, -.4f * scaleY, 0);
    bone.m_boneFlag = libmmd::PMXBoneFlags::AllowRotate;
    model.m_bones.push_back(bone);
}
Options BaseOptions()
{
    Options options; options.centerOffsets = options.legOffsets = false;
    options.tolerance = .002; options.iterations = 100;
    return options;
}
void RequireResult(const Result& result) { if (!result.success) throw std::runtime_error(result.error); }
Vector Point(const libmmd::PMXFile& model, const libmmd::VMDFile& motion, const std::string& name, uint32_t frame = 0)
{
    Rig rig(model); Pose pose(rig, Motion(motion), frame); return pose.positions[static_cast<size_t>(rig.Find(name))];
}

void CheckAppendEvaluation()
{
    // Independent libMMD nodes verify chained/local/negative append channels.
    // Parent and append graphs deliberately have different dependency order.
    libmmd::PMXFile model;
    for (int i = 0; i < 4; ++i)
    {
        libmmd::PMXBone bone{};
        bone.m_name = std::to_string(i);
        bone.m_parentBoneIndex = i == 0 ? 3 : -1;
        bone.m_position = Eigen::Vector3f::Zero();
        bone.m_appendBoneIndex = i == 3 ? -1 : i + 1;
        bone.m_appendWeight = i == 0 ? -1.25f : .5f;
        bone.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(i == 3 ? 2 : (i == 1 ? 0x382 : 0x302));
        model.m_bones.push_back(bone);
    }
    const Rig rig(model);
    Pose pose(rig, Motion(libmmd::VMDFile()), 0);
    for (size_t i = 0; i < 4; ++i)
    {
        pose.local[i].rotation = Rotation(Eigen::AngleAxisd(.2 * (i + 1), Vector(1, 2, 3).normalized()));
        pose.local[i].translation = Vector(.3 * i, -.2 * i, .1 * i);
    }
    pose.Update();
    double rotations[4][4], translations[4][3];
    SizingAppendReference(rotations, translations);
    for (int i = 3; i >= 0; --i)
    {
        const auto* q = rotations[i];
        Check(pose.append[static_cast<size_t>(i)].rotation.angularDistance(Rotation(q[3], q[0], q[1], q[2]).normalized()) < 1.e-6, "Append rotation differs from libMMD");
        Check((pose.append[static_cast<size_t>(i)].translation - Vector(translations[i])).norm() < 1.e-6, "Append translation differs from libMMD");
    }
    Check(rig.SupportedChain(0), "Append arm chain incorrectly unsupported");
    auto invalid = model;
    invalid.m_bones[0].m_appendBoneIndex = 0;
    const Rig cycle(invalid);
    Check(!cycle.SupportedChain(0) && !cycle.warnings.empty(), "Append cycle not isolated and reported");
    invalid.m_bones[0].m_boneFlag = static_cast<libmmd::PMXBoneFlags>(0x382);
    const Rig localSelf(invalid); // Raw local channel has no append dependency.
    Check(localSelf.SupportedChain(0), "Legal local append dependency rejected");
    invalid.m_bones[0].m_appendBoneIndex = 99;
    bool rejected = false;
    try { Rig badIndex(invalid); } catch (const std::exception&) { rejected = true; }
    Check(rejected, "Invalid append index accepted");
    std::cout << "append_libmmd_reference=passed\n";
}

void CheckPlaybackInterpolation()
{
    libmmd::VMDFile file;
    auto first = Key("センター", 0), last = Key("センター", 40, Rotation(Eigen::AngleAxisd(1.4, Vector::UnitY())));
    first.m_translate = Eigen::Vector3f(2, 3, 4); last.m_translate = Eigen::Vector3f(9, -1, 8);
    first.m_interpolation.fill(0); // Redundant blocks need not repeat block zero.
    for (size_t axis = 0; axis < 4; ++axis)
    {
        first.m_interpolation[axis] = static_cast<uint8_t>(10 + axis * 8);
        first.m_interpolation[axis + 4] = static_cast<uint8_t>(90 - axis * 10);
        first.m_interpolation[axis + 8] = 100;
        first.m_interpolation[axis + 12] = 120;
    }
    file.m_motions = {first, last};
    const Motion motion(file);
    for (uint32_t frame = 1; frame < 40; ++frame)
    {
        const auto actual = motion.Sample("センター", frame);
        const auto expected = libmmd::InterpolateBoneKeys(libmmd::VMDBoneKeyframe(first), libmmd::VMDBoneKeyframe(last), static_cast<float>(frame));
        Check((actual.translation - expected.translate.cast<double>()).norm() < 1.e-6 &&
            actual.rotation.angularDistance(expected.rotate.cast<double>().normalized()) < 1.e-6, "Sizing differs from C4D playback interpolation");
    }
    std::cout << "playback_interpolation=passed\n";
}
}

int main(int argc, char** argv)
{
    try
    {
        Check(argc >= 3, "Expected base and advanced fixture directories");
        CheckAppendEvaluation();
        CheckPlaybackInterpolation();
        const std::filesystem::path base(argv[1]), fixtures(argv[2]);
        libmmd::PMXFile source, target;
        libmmd::VMDFile motion;
        Check(libmmd::ReadPMXFile(&source, (base / "source.pmx").string().c_str()), "Read source");
        Check(libmmd::ReadPMXFile(&target, (base / "target.pmx").string().c_str()), "Read target");
        Check(libmmd::ReadVMDFile(&motion, (base / "motion.vmd").string().c_str()), "Read motion");
        for (const std::string side : {"左", "右"})
        {
            AddFinger(source, side, 1, 1); AddFinger(target, side, 1.7f, 1.35f);
            for (const auto* name : {"腕", "ひじ", "手首"})
                for (uint32_t frame : {0u, 15u, 30u})
                    motion.m_motions.push_back(Key(side + name, frame, Rotation(Eigen::AngleAxisd(.3 + frame * .01, Vector(1, 2, 3).normalized()))));
        }
        if (argc == 4 && std::string(argv[3]) == "--generate")
        {
            std::filesystem::create_directories(fixtures);
            Check(libmmd::WritePMXFile(&source, (fixtures / "source.pmx").string().c_str()), "Write source");
            Check(libmmd::WritePMXFile(&target, (fixtures / "target.pmx").string().c_str()), "Write target");
            Check(libmmd::WriteVMDFile(&motion, (fixtures / "motion.vmd").string().c_str()), "Write motion");
            libmmd::VMDFile camera;
            camera.m_header = motion.m_header;
            std::u16string cameraName;
            libmmd::ConvU8ToU16("カメラ・照明", cameraName);
            camera.m_header.m_modelName.Set(libmmd::ConvertU16ToSjisString(cameraName).c_str());
            camera.m_cameras.emplace_back(0, -35.f, Eigen::Vector3f(0, 9, 0), Eigen::Vector3f::Zero(), 45);
            camera.m_cameras.emplace_back(30, -25.f, Eigen::Vector3f(0, 11, 0), Eigen::Vector3f(.1f, .3f, 0), 35);
            Check(libmmd::WriteVMDFile(&camera, (fixtures / "camera.vmd").string().c_str()), "Write camera");
            libmmd::VMDFile reread;
            Check(libmmd::ReadVMDFile(&reread, (fixtures / "camera.vmd").string().c_str()) && reread.m_cameras.size() == 2, "Camera fixture roundtrip");
            return 0;
        }

        Options options = BaseOptions(); options.stance = true;
        const auto stance = Run(source, target, motion, options); RequireResult(stance);
        std::ifstream golden(fixtures / "baseline-stance/reference.tsv");
        Check(golden.good(), "Actual upstream stance reference missing");
        size_t compared = 0; double maxAngle = 0.; std::string row;
        while (std::getline(golden, row))
        {
            const auto tab = row.find('\t'); const std::string name = row.substr(0, tab);
            if (name.find("腕") == std::string::npos && name.find("ひじ") == std::string::npos && name.find("手首") == std::string::npos) continue;
            std::istringstream values(row.substr(tab + 1)); uint32_t frame = 0; double x, y, z, qx, qy, qz, qw;
            values >> frame >> x >> y >> z >> qx >> qy >> qz >> qw;
            Check(!values.fail(), "Malformed upstream rotation reference");
            for (const auto& key : stance.stages[3].m_motions)
                if (key.m_boneName.ToUtf8String() == name && key.m_frame == frame)
                { ++compared; maxAngle = std::max(maxAngle, key.m_quaternion.cast<double>().normalized().angularDistance(Rotation(qw, qx, qy, qz).normalized())); }
        }
        std::cout << "upstream_arm_keys=" << compared << " max_rotation_error_rad=" << maxAngle << '\n';
        Check(compared == 18 && maxAngle < 1.e-5, "Arm stance upstream mismatch");

        // A serial, collinear twist chain must preserve every descendant pose.
        auto twisted = source;
        const int arm = Find(twisted, "左腕"), elbow = Find(twisted, "左ひじ");
        const Vector axis = (twisted.m_bones[static_cast<size_t>(elbow)].m_position - twisted.m_bones[static_cast<size_t>(arm)].m_position).cast<double>().normalized();
        libmmd::PMXBone twistBone{}; twistBone.m_name = "左腕捩"; twistBone.m_parentBoneIndex = arm;
        twistBone.m_position = (twisted.m_bones[static_cast<size_t>(arm)].m_position + twisted.m_bones[static_cast<size_t>(elbow)].m_position) * .5f;
        twistBone.m_boneFlag = libmmd::PMXBoneFlags::FixedAxis; twistBone.m_fixedAxis = axis.cast<float>();
        twisted.m_bones[static_cast<size_t>(elbow)].m_parentBoneIndex = static_cast<int>(twisted.m_bones.size()); twisted.m_bones.push_back(twistBone);
        libmmd::VMDFile spin; spin.m_motions = {Key("センター", 0), Key("左腕", 0, Rotation(Eigen::AngleAxisd(.8, axis))), Key("左腕", 10, Rotation(Eigen::AngleAxisd(1.6, axis)))};
        options = BaseOptions(); options.twist = true;
        const auto twist = Run(source, twisted, spin, options); RequireResult(twist);
        for (uint32_t frame = 0; frame <= 10; ++frame)
        {
            const Rig rig(twisted); const Pose before(rig, Motion(twist.stages[3]), frame), after(rig, Motion(twist.stages[4]), frame);
            for (size_t i = 0; i < rig.model.m_bones.size(); ++i)
                Check((before.positions[i] - after.positions[i]).norm() < 1.e-5, "Twist changed joint position");
            Check(before.rotations[static_cast<size_t>(elbow)].angularDistance(after.rotations[static_cast<size_t>(elbow)]) < 1.e-5, "Twist changed end rotation");
        }

        // Offset fixed axis, existing non-axis rotation and writable child
        // compensation must survive a VMD bake without introducing translation.
        twisted.m_bones.back().m_position.z() += .18f;
        spin.m_motions.push_back(Key("左腕捩", 0, Rotation(Eigen::AngleAxisd(.21, Vector::UnitY()))));
        const auto offsetTwist = Run(source, twisted, spin, options); RequireResult(offsetTwist);
        double maxTwistError = 0., changedAngle = 0.;
        for (uint32_t frame = 0; frame <= 10; ++frame)
        {
            const Rig rig(twisted);
            const Pose before(rig, Motion(offsetTwist.stages[3]), frame), after(rig, Motion(offsetTwist.stages[4]), frame);
            maxTwistError = std::max(maxTwistError, (before.positions[static_cast<size_t>(elbow)] - after.positions[static_cast<size_t>(elbow)]).norm());
            changedAngle = std::max(changedAngle, before.local.back().rotation.angularDistance(after.local.back().rotation));
            Check(before.rotations[static_cast<size_t>(elbow)].angularDistance(after.rotations[static_cast<size_t>(elbow)]) < 1.e-5, "Offset twist changed end orientation");
            Check(after.local.back().translation.isZero() && after.local[static_cast<size_t>(elbow)].translation.isZero(), "Twist wrote unplayable translations");
        }
        Check(maxTwistError <= options.tolerance && changedAngle > .001, "Offset twist failed error bound or did no work");
        std::cout << "offset_twist_max_error=" << maxTwistError << " transferred_angle=" << changedAngle << '\n';

        auto contactSource = source;
        for (const std::string side : {"左", "右"})
            for (const auto* name : {"腕", "ひじ", "手首"}) contactSource.m_bones[static_cast<size_t>(Find(contactSource, side + name))].m_position.y() = 13;
        auto contactTarget = contactSource;
        for (auto& bone : contactTarget.m_bones)
            if (bone.m_name.find("腕") != std::string::npos || bone.m_name.find("ひじ") != std::string::npos || bone.m_name.find("手首") != std::string::npos || bone.m_name.find("指") != std::string::npos)
                bone.m_position.x() += bone.m_name.substr(0, 3) == "左" ? .8f : -.8f;
        libmmd::VMDFile touch; touch.m_motions.push_back(Key("センター", 0));
        for (const std::string side : {"左", "右"})
            for (const auto* name : {"腕", "ひじ"})
                for (uint32_t frame : {0u, 10u}) touch.m_motions.push_back(Key(side + name, frame, Rotation(Eigen::AngleAxisd(side == "左" ? 1.57079632679 : -1.57079632679, Vector::UnitZ()))));
        options = BaseOptions(); options.wristContact = true;
        const auto contact = Run(contactSource, contactTarget, touch, options); RequireResult(contact);
        const double gap = (Point(contactTarget, contact.stages[6], "左手首") - Point(contactTarget, contact.stages[6], "右手首")).norm();
        std::cout << "wrist_contact_gap=" << gap << " constraints=" << contact.analysis.constraints << '\n';
        Check(contact.analysis.constraints == 22 && gap < .02, "Reachable wrist contact did not converge");

        auto fingerSource = contactSource, fingerTarget = contactTarget;
        for (const std::string side : {"左", "右"})
            for (auto* model : {&fingerSource, &fingerTarget})
            {
                libmmd::PMXBone tip{}; tip.m_name = side + "中指３";
                tip.m_parentBoneIndex = Find(*model, side + "手首");
                tip.m_position = model->m_bones[static_cast<size_t>(tip.m_parentBoneIndex)].m_position + Eigen::Vector3f(0, 0, .2f);
                tip.m_boneFlag = libmmd::PMXBoneFlags::AllowRotate; model->m_bones.push_back(tip);
            }
        options = BaseOptions(); options.fingerContact = true;
        const auto fingers = Run(fingerSource, fingerTarget, touch, options); RequireResult(fingers);
        const double fingerGap = (Point(fingerTarget, fingers.stages[6], "左中指３") - Point(fingerTarget, fingers.stages[6], "右中指３")).norm();
        Check(fingers.analysis.constraints == 22 && fingerGap < .02, "Finger contact did not converge");
        auto floorTarget = contactTarget;
        for (auto& bone : floorTarget.m_bones)
            if (bone.m_name.find("腕") != std::string::npos || bone.m_name.find("ひじ") != std::string::npos || bone.m_name.find("手首") != std::string::npos) bone.m_position.y() += 1;
        options = BaseOptions(); options.floorContact = true; options.floorHeight = 15;
        const auto floor = Run(contactSource, floorTarget, touch, options); RequireResult(floor);
        Check(floor.analysis.constraints == 22 && std::abs(Point(floorTarget, floor.stages[6], "左手首").y() - 15.) < .01, "Wrist floor contact failed");
        libmmd::VMDFile footMotion; footMotion.m_motions = {Key("センター", 0), Key("左足ＩＫ", 0)};
        footMotion.m_motions.back().m_translate.y() = .1f;
        options = BaseOptions(); options.floorContact = true;
        const auto feet = Run(source, target, footMotion, options); RequireResult(feet);
        Check(std::abs(Point(target, feet.stages[6], "左足ＩＫ").y() - target.m_bones[static_cast<size_t>(Find(target, "左足ＩＫ"))].m_position.y()) < 1.e-5, "Foot IK floor contact failed");
        std::cout << "finger_gap=" << fingerGap << " floor_contacts=" << floor.analysis.constraints << '\n';

        // Shape projection is checked independently of CCD, including transforms.
        const Rig colliderRig(contactTarget); const Pose colliderPose(colliderRig, Motion(touch), 0);
        for (auto shape : {libmmd::PMXRigidbody::Shape::Sphere, libmmd::PMXRigidbody::Shape::Box, libmmd::PMXRigidbody::Shape::Capsule})
        {
            libmmd::PMXRigidbody body{}; body.m_boneIndex = -1; body.m_shape = shape;
            body.m_shapeSize = Eigen::Vector3f(1, 2, 1); body.m_translate = Eigen::Vector3f(3, 4, 5); body.m_rotate = Eigen::Vector3f::Zero();
            const Vector boundary = ProjectOutside(body, colliderPose, Vector(3, 4, 5), .1);
            Check((boundary - Vector(4.1, 4, 5)).norm() < 1.e-6, "Shape nearest surface mismatch");
            Check((ProjectOutside(body, colliderPose, boundary, .1) - boundary).norm() < 1.e-6, "Projected point still penetrates");
        }
        auto avoidedTarget = contactTarget;
        libmmd::PMXRigidbody body{}; body.m_name = "wrist obstacle"; body.m_boneIndex = -1;
        body.m_shape = libmmd::PMXRigidbody::Shape::Sphere; body.m_shapeSize = Eigen::Vector3f(.5f, 0, 0);
        body.m_translate = (Point(contactTarget, touch, "左手首") + Vector(0, .2, 0)).cast<float>(); body.m_rotate.setZero();
        body.m_op = libmmd::PMXRigidbody::Operation::Static; avoidedTarget.m_rigidbodies.push_back(body);
        options = BaseOptions(); options.avoidance = true;
        const auto avoid = Run(contactSource, avoidedTarget, touch, options); RequireResult(avoid);
        const Rig avoidRig(avoidedTarget); const Pose avoidedPose(avoidRig, Motion(avoid.stages[5]), 0);
        const Vector wrist = avoidedPose.positions[static_cast<size_t>(avoidRig.Find("左手首"))];
        const double penetration = (ProjectOutside(body, avoidedPose, wrist, options.collisionMargin) - wrist).norm();
        std::cout << "avoidance_penetration=" << penetration << '\n';
        Check(penetration < .01, "Reachable avoidance failed");

        // An unreachable target remains finite and explicitly unresolved.
        auto unreachableTarget = contactTarget;
        for (auto& bone : unreachableTarget.m_bones)
            if (bone.m_name.substr(0, 3) == "左") bone.m_position.x() += 30.f;
        options = BaseOptions(); options.wristContact = true;
        const auto unreachable = Run(contactSource, unreachableTarget, touch, options); RequireResult(unreachable);
        Check(unreachable.analysis.unresolved > 0 && std::isfinite(unreachable.analysis.maxResidual), "Unreachable contact not reported");
        options.maxBakeFrames = 5;
        Check(!Run(contactSource, contactTarget, touch, options).success, "Bake budget not enforced");
        std::atomic_bool cancel{true};
        Check(RunBatch({CharacterInput{source, target, motion, BaseOptions()}}, libmmd::VMDFile(), CameraOptions(), &cancel).cancelled, "Batch cancellation failed");

        libmmd::VMDFile camera; camera.m_cameras.emplace_back(0, -30.f, Eigen::Vector3f(0, 10, 0), Eigen::Vector3f::Zero(), 45);
        auto doubled = source;
        for (auto& bone : doubled.m_bones) bone.m_position *= 2;
        libmmd::VMDFile still; still.m_motions.push_back(Key("センター", 0));
        const auto cameraResult = RunBatch({CharacterInput{source, doubled, still, BaseOptions()}}, camera, CameraOptions{true, 5.});
        Check(cameraResult.success, "Camera solve failed");
        Check(std::abs(cameraResult.camera.m_cameras[0].m_distance + 60.f) < 1.e-5 &&
              (cameraResult.camera.m_cameras[0].m_interest - Eigen::Vector3f(0, 20, 0)).norm() < 1.e-5, "Uniform camera scaling mismatch");
        Check(cameraResult.camera.m_cameras[0].m_interpolation == camera.m_cameras[0].m_interpolation &&
              cameraResult.originalCamera.m_cameras[0].m_distance == -30.f, "Camera input/curve changed");
        auto invalidCamera = camera; invalidCamera.m_cameras.push_back(invalidCamera.m_cameras.front());
        Check(!RunBatch({CharacterInput{source, doubled, still, BaseOptions()}}, invalidCamera, CameraOptions{true, 5.}).success, "Duplicate camera key accepted");

        // Two coincident source performers with differently sized target arms.
        options = BaseOptions(); options.multiContact = true;
        const auto batch = RunBatch({CharacterInput{contactSource, contactSource, touch, options}, CharacterInput{contactSource, contactTarget, touch, options}});
        Check(batch.success && batch.characters.size() == 2, "Batch failed");
        const double crossGap = (Point(contactSource, batch.characters[0].stages[7], "左手首") - Point(contactTarget, batch.characters[1].stages[7], "左手首")).norm();
        std::cout << "cross_character_gap=" << crossGap << '\n';
        Check(crossGap < .05 && batch.characters[0].analysis.constraints > 0, "Cross-character contact failed");
        std::cout << "advanced sizing regression passed\n";
        return 0;
    }
    catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
