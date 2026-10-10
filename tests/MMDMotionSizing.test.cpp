#include "libMMD/Model/MMD/MMDMotionSizing.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace
{
void Check(bool condition, const char* message)
{
    if (!condition) throw std::runtime_error(message);
}
}

int main(int argc, char** argv)
{
    try
    {
        Check(argc == 2, "Expected fixture directory");
        const std::string directory = argv[1];
        libmmd::PMXFile source, target;
        libmmd::VMDFile motion;
        Check(libmmd::ReadPMXFile(&source, (directory + "/source.pmx").c_str()), "Source parse");
        Check(libmmd::ReadPMXFile(&target, (directory + "/target.pmx").c_str()), "Target parse");
        Check(libmmd::ReadVMDFile(&motion, (directory + "/motion.vmd").c_str()), "Motion parse");
        const auto result = libmmd::sizing::Run(source, target, motion);
        if (!result.success) throw std::runtime_error(result.error);
        const auto compareReference = [&](const libmmd::sizing::Result& candidate,
                                          const libmmd::VMDFile& input, const std::string& baseline) {
            Check(candidate.success, "Reference case failed");
            std::ifstream reference(directory + "/" + baseline + "/reference.tsv");
            Check(reference.good(), "Missing actual upstream baseline");
            size_t compared = 0;
            double maxError = 0;
            std::string row;
            while (std::getline(reference, row))
            {
                const auto tab = row.find('\t');
                const std::string name = row.substr(0, tab);
                std::istringstream values(row.substr(tab + 1));
                uint32_t frame;
                Eigen::Vector3d expected;
                values >> frame >> expected.x() >> expected.y() >> expected.z();
                Check(!values.fail(), "Invalid reference row");
                for (const auto& key : candidate.stages[2].m_motions)
                    if (key.m_boneName.ToUtf8String() == name && key.m_frame == frame)
                    {
                        maxError = std::max(maxError, (key.m_translate.cast<double>() - expected).norm());
                        ++compared;
                    }
            }
            std::cout << baseline << " upstream_keys=" << compared << " max_position_error=" << maxError
                      << " cpp_ms=" << candidate.elapsedMilliseconds << '\n';
            Check(compared == input.m_motions.size() && maxError < 2.e-5, "Upstream movement mismatch");
        };
        compareReference(result, motion, "baseline");
        libmmd::VMDFile staggered;
        Check(libmmd::ReadVMDFile(&staggered, (directory + "/motion-staggered.vmd").c_str()), "Staggered motion parse");
        compareReference(libmmd::sizing::Run(source, target, staggered), staggered, "baseline-staggered");
        for (size_t i = 0; i < motion.m_motions.size(); ++i)
        {
            const auto& original = motion.m_motions[i];
            Check(original.m_translate == result.stages[0].m_motions[i].m_translate, "Original stage changed");
            for (size_t stage = 1; stage <= 2; ++stage)
            {
                const auto& key = result.stages[stage].m_motions[i];
                Check(key.m_interpolation == original.m_interpolation && key.m_quaternion.coeffs() == original.m_quaternion.coeffs(),
                      "Rotation/interpolation mutated");
            }
        }
        std::atomic_bool cancel{true};
        Check(libmmd::sizing::Run(source, target, motion, {}, &cancel).cancelled, "Cancellation");
        auto invalid = source;
        invalid.m_bones[0].m_parentBoneIndex = 1;
        Check(!libmmd::sizing::Run(invalid, target, motion).success, "Cycle accepted");
        invalid = source;
        invalid.m_bones[0].m_position.x() = std::numeric_limits<float>::quiet_NaN();
        Check(!libmmd::sizing::Run(invalid, target, motion).success, "NaN accepted");
        auto badMotion = motion;
        badMotion.m_motions.push_back(badMotion.m_motions.front());
        Check(!libmmd::sizing::Run(source, target, badMotion).success, "Duplicate key accepted");
        libmmd::sizing::Options options;
        options.movementMultiplier = 2;
        options.centerOffsets = options.legOffsets = false;
        const auto multiplied = libmmd::sizing::Run(source, target, motion, options);
        Check(multiplied.success, "Multiplier failed");
        Check(std::abs(multiplied.analysis.horizontalRatio - result.analysis.horizontalRatio * 2) < 1.e-12, "Multiplier ratio");
        Check(multiplied.stages[1].m_motions.back().m_translate == motion.m_motions.back().m_translate, "Non-movement track changed");
        options.movementMultiplier = -1;
        Check(!libmmd::sizing::Run(source, target, motion, options).success, "Negative multiplier accepted");
        invalid = source;
        invalid.m_bones[0].m_parentBoneIndex = static_cast<int>(invalid.m_bones.size());
        Check(!libmmd::sizing::Run(invalid, target, motion).success, "Out-of-range parent accepted");
        badMotion = motion;
        badMotion.m_motions[0].m_quaternion.coeffs().setZero();
        Check(!libmmd::sizing::Run(source, target, badMotion).success, "Zero quaternion accepted");
        // Translation-only append must get the same conservative P1 handling
        // as rotation append; the legacy offset formula cannot evaluate either.
        auto appended = target;
        libmmd::PMXBone donor{};
        donor.m_name = "append-offset-reference"; donor.m_parentBoneIndex = -1;
        donor.m_position.setZero();
        const int donorIndex = static_cast<int>(appended.m_bones.size());
        appended.m_bones.push_back(donor);
        for (auto& bone : appended.m_bones)
            if (bone.m_name == "センター")
            {
                bone.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(static_cast<uint16_t>(bone.m_boneFlag) | 0x200u);
                bone.m_appendBoneIndex = donorIndex; bone.m_appendWeight = .5f;
            }
        const auto appendResult = libmmd::sizing::Run(source, appended, motion);
        Check(appendResult.success, "Translation append offset preflight failed");
        bool skippedAppendOffset = false;
        for (const auto& warning : appendResult.analysis.warnings)
            skippedAppendOffset |= warning.find("Offset skipped for append/fixed-axis/outer-parent chain: センター") != std::string::npos;
        Check(skippedAppendOffset, "Translation-only append used an incompatible movement offset");
        // Compare serialized bytes after restoring the deliberately modified
        // motion channel. This covers every other public VMD channel at once.
        auto mixed = motion;
        libmmd::VMDString<15> morphName;
        morphName.Set("smile");
        libmmd::VMDString<20> ikName;
        ikName.Set("leftIK");
        mixed.m_morphs.emplace_back(morphName, 7, .35f);
        mixed.m_cameras.emplace_back(8, 12.f, Eigen::Vector3f(1, 2, 3), Eigen::Vector3f(.1f, .2f, .3f), 45, 1);
        mixed.m_lights.emplace_back(9, Eigen::Vector3f(.2f, .3f, .4f), Eigen::Vector3f(3, 4, 5));
        mixed.m_shadows.emplace_back(10, 2, 30.f);
        mixed.m_iks.emplace_back(11, 1, std::vector<libmmd::VMDIkInfo>{libmmd::VMDIkInfo(ikName, 0)});
        const auto mixedResult = libmmd::sizing::Run(source, target, mixed);
        Check(mixedResult.success, "Mixed channels failed");
        std::vector<uint8_t> originalBytes;
        Check(libmmd::WriteVMDFile(&mixed, originalBytes), "Serialize original channels");
        for (const auto& stage : mixedResult.stages)
        {
            auto restored = stage;
            restored.m_motions = mixed.m_motions;
            std::vector<uint8_t> stageBytes;
            Check(libmmd::WriteVMDFile(&restored, stageBytes) && stageBytes == originalBytes, "Non-motion channel changed");
        }
        std::cout << "movement sizing regression passed\n";
        return 0;
    }
    catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
