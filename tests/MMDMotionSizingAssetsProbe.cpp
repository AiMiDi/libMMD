// Opt-in integration probe. Local model assets are never copied into the repo.
#include "libMMD/Model/MMD/MMDMotionSizing.h"
#include "MMDMotionPose.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>

using namespace libmmd::sizing;
using namespace libmmd::sizing::detail;
namespace
{
std::vector<uint8_t> Read(const std::string& path)
{
    std::ifstream stream(std::filesystem::u8path(path), std::ios::binary);
    Require(stream.good(), "Cannot open asset: " + path);
    return {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
}
}

int main(int argc, char** argv)
{
    try
    {
        Require(argc == 3 || (argc == 4 && std::string(argv[3]) == "--batch"), "Expected UTF-8 line manifest, output directory and optional --batch");
        std::ifstream manifest(argv[1]);
        std::vector<std::string> paths;
        for (std::string line; std::getline(manifest, line); )
        {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            paths.push_back(line);
        }
        Require(paths.size() == 4, "Manifest requires source PMX, target PMX, motion VMD, camera VMD");
        CharacterInput character;
        libmmd::VMDFile camera;
        auto data = Read(paths[0]); Require(libmmd::ReadPMXFile(&character.source, data.data(), data.size()), "Source parse failed");
        data = Read(paths[1]); Require(libmmd::ReadPMXFile(&character.target, data.data(), data.size()), "Target parse failed");
        data = Read(paths[2]); Require(libmmd::ReadVMDFile(&character.motion, data.data(), data.size()), "Motion parse failed");
        data = Read(paths[3]); Require(libmmd::ReadVMDFile(&camera, data.data(), data.size()), "Camera parse failed");
        auto& options = character.options;
        options.stance = options.twist = options.avoidance = true;
        options.wristContact = options.fingerContact = options.floorContact = true;
        options.maxDiagnostics = 1000000;
        std::vector<CharacterInput> characters{character};
        if (argc == 4)
        {
            characters.front().options.multiContact = true;
            auto second = character;
            second.target = second.source;
            second.options.multiContact = true;
            characters.push_back(std::move(second));
        }
        const auto batch = RunBatch(characters, camera, CameraOptions{true, 5.});
        Require(batch.success, batch.error);
        const auto& result = batch.characters.front();
        const std::filesystem::path output(argv[2]);
        std::filesystem::create_directories(output);
        const Rig rig(character.target);
        const Motion before(result.stages[3]), after(result.stages[4]);
        double positionError = 0., rotationError = 0., transfer = 0.;
        for (uint32_t frame = 0; frame <= before.LastFrame(); ++frame)
        {
            const Pose a(rig, before, frame), b(rig, after, frame);
            for (const std::string side : {"左", "右"})
            {
                for (const auto* suffix : {"ひじ", "手首"})
                {
                    const size_t index = static_cast<size_t>(rig.Find(side + suffix));
                    Require(index < a.positions.size(), "Missing arm endpoint");
                    positionError = std::max(positionError, (a.positions[index] - b.positions[index]).norm());
                    // Forearm twist redistributes elbow rotation; wrist remains fixed.
                    if (std::string(suffix) == "手首") rotationError = std::max(rotationError, a.rotations[index].angularDistance(b.rotations[index]));
                }
                const size_t twist = static_cast<size_t>(rig.Find(side + "腕捩"));
                Require(twist < a.local.size(), "Missing twist bone");
                transfer = std::max(transfer, a.local[twist].rotation.angularDistance(b.local[twist].rotation));
            }
        }
        Require(positionError <= options.tolerance * 1.01 && rotationError < 1.e-5 && transfer > .001, "Real twist invariant failed");
        std::ofstream receipt(output / "core.json");
        receipt << std::setprecision(12) << "{\n\"success\":true,\n\"elapsed_ms\":" << result.elapsedMilliseconds
                << ",\n\"last_frame\":" << before.LastFrame() << ",\n\"twist_max_position_error\":" << positionError
                << ",\n\"twist_max_wrist_angle_error\":" << rotationError << ",\n\"twist_max_transfer\":" << transfer << ",\n\"stages\":[";
        for (size_t stage = 0; stage < result.stages.size(); ++stage)
        {
            const auto& motion = result.stages[stage];
            std::set<std::pair<std::string, uint32_t>> keys;
            for (const auto& key : motion.m_motions)
                Require(key.m_translate.allFinite() && key.m_quaternion.coeffs().allFinite() &&
                    keys.emplace(key.m_boneName.ToUtf8String(), key.m_frame).second, "Nonfinite or duplicate output key");
            Require(libmmd::WriteVMDFile(&motion, (output / ("stage-" + std::to_string(stage) + ".vmd")).string().c_str()), "Stage export failed");
            size_t count = 0, unresolved = 0; double maximum = 0.;
            for (const auto& sample : result.analysis.samples)
                if (static_cast<size_t>(sample.stage) == stage)
                { ++count; if (sample.error > options.tolerance) ++unresolved; maximum = std::max(maximum, sample.error); }
            receipt << (stage ? "," : "") << "{\"stage\":" << stage << ",\"keys\":" << keys.size()
                    << ",\"constraints\":" << count << ",\"unresolved\":" << unresolved << ",\"max_residual\":" << maximum << "}";
        }
        receipt << "],\n\"warnings\":[";
        for (size_t i = 0; i < result.analysis.warnings.size(); ++i)
            receipt << (i ? "," : "") << std::quoted(result.analysis.warnings[i]);
        receipt << "],\n\"characters\":[";
        for (size_t i = 0; i < batch.characters.size(); ++i)
        {
            const auto& analysis = batch.characters[i].analysis;
            receipt << (i ? "," : "") << "{\"constraints\":" << analysis.constraints << ",\"unresolved\":" << analysis.unresolved << ",\"max_residual\":" << analysis.maxResidual << "}";
        }
        receipt << "],\n\"camera_keys\":" << batch.camera.m_cameras.size() << "\n}\n";
        Require(libmmd::WriteVMDFile(&batch.camera, (output / "camera.vmd").string().c_str()), "Camera export failed");
        Require(batch.camera.m_cameras.size() == camera.m_cameras.size(), "Camera key count changed");
        for (size_t i = 0; i < camera.m_cameras.size(); ++i)
        {
            const auto& a = camera.m_cameras[i]; const auto& b = batch.camera.m_cameras[i];
            Require(a.m_frame == b.m_frame && a.m_rotate == b.m_rotate && a.m_viewAngle == b.m_viewAngle &&
                a.m_isPerspective == b.m_isPerspective && a.m_interpolation == b.m_interpolation && b.m_interest.allFinite() && std::isfinite(b.m_distance), "Camera invariants changed");
        }
        // Selected frame positions can be compared with an actual C4D import.
        std::ofstream poses(output / "poses.tsv");
        poses << std::setprecision(12);
        for (size_t stage = 3; stage < result.stages.size(); ++stage)
            for (uint32_t frame : {0u, 400u, 800u, 1200u, 1690u})
            {
                const Pose pose(rig, Motion(result.stages[stage]), frame);
                for (const std::string side : {"左", "右"})
                    for (const auto* suffix : {"腕", "ひじ", "手首"})
                    {
                        const auto index = static_cast<size_t>(rig.Find(side + suffix));
                        poses << stage << '\t' << frame << '\t' << side + suffix << '\t' << pose.positions[index].transpose() << '\n';
                    }
            }
        std::cout << "Real assets passed, receipt: " << (output / "core.json").string() << '\n';
        return 0;
    }
    catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
