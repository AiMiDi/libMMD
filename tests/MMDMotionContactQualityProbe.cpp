// Opt-in contact-quality audit for local licensed assets. Not a CTest fixture.
#include "MMDMotionConstraints.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>

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
        Require(argc == 4, "Expected asset manifest, stage directory and output CSV");
        std::ifstream manifest(argv[1]);
        std::vector<std::string> paths;
        for (std::string line; std::getline(manifest, line); )
        {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            paths.push_back(line);
        }
        Require(paths.size() == 4, "Expected source PMX, target PMX, motion VMD and camera VMD");
        libmmd::PMXFile source, target;
        libmmd::VMDFile original, incoming, adjusted;
        auto bytes = Read(paths[0]);
        Require(libmmd::ReadPMXFile(&source, bytes.data(), bytes.size()), "Source parse failed");
        bytes = Read(paths[1]);
        Require(libmmd::ReadPMXFile(&target, bytes.data(), bytes.size()), "Target parse failed");
        bytes = Read(paths[2]);
        Require(libmmd::ReadVMDFile(&original, bytes.data(), bytes.size()), "Motion parse failed");
        const std::filesystem::path stages(argv[2]);
        Require(libmmd::ReadVMDFile(&incoming, (stages / "stage-5.vmd").string().c_str()), "Stage 5 parse failed");
        Require(libmmd::ReadVMDFile(&adjusted, (stages / "stage-6.vmd").string().c_str()), "Stage 6 parse failed");
        const Rig sourceRig(source), targetRig(target);
        const Motion sourceMotion(original), beforeMotion(incoming), afterMotion(adjusted);
        Options options; options.fingerContact = true;
        Analysis analysis;
        const auto effectors = Effectors(sourceRig, targetRig, options, analysis);
        std::ofstream output(argv[3]);
        Require(output.good(), "Cannot create audit CSV");
        output << std::setprecision(12)
               << "frame,bone,source_x,source_y,source_z,before_x,before_y,before_z,after_x,after_y,after_z,palm_angle,local_angle,hand_scale\n";
        for (uint32_t frame = 0; frame <= sourceMotion.LastFrame(); ++frame)
        {
            const Pose s(sourceRig, sourceMotion, frame), b(targetRig, beforeMotion, frame), a(targetRig, afterMotion, frame);
            for (const auto& effector : effectors)
            {
                const auto index = static_cast<size_t>(effector.target);
                output << frame << ',' << effector.name;
                for (const Vector* value : {&s.positions[effector.source], &b.positions[index], &a.positions[index]})
                    for (int axis = 0; axis < 3; ++axis) output << ',' << (*value)[axis];
                output << ',' << b.rotations[effector.wrist].angularDistance(a.rotations[effector.wrist])
                       << ',' << b.local[index].rotation.angularDistance(a.local[index].rotation)
                       << ',' << effector.handScale << '\n';
            }
        }
        Require(output.good(), "Contact audit write failed");
        std::cout << "Contact poses audited: " << sourceMotion.LastFrame() + 1 << " frames, "
                  << effectors.size() << " landmarks\n";
        return 0;
    }
    catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
