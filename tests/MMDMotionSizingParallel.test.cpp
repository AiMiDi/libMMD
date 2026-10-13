#include "libMMD/Model/MMD/MMDMotionSizing.h"
#include "MMDMotionParallel.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <set>

using namespace libmmd::sizing;
using namespace libmmd::sizing::detail;

namespace
{
void Check(bool value, const char* error) { if (!value) throw std::runtime_error(error); }

std::vector<uint8_t> Bytes(const libmmd::VMDFile& motion)
{
    std::vector<uint8_t> bytes;
    Check(libmmd::WriteVMDFile(&motion, bytes), "Cannot serialize comparison motion");
    return bytes;
}

void Equal(const Result& reference, const Result& candidate)
{
    Check(reference.success && candidate.success, "Solve failed");
    for (size_t stage = 0; stage < reference.stages.size(); ++stage)
        Check(Bytes(reference.stages[stage]) == Bytes(candidate.stages[stage]), "Parallel stage bytes differ");
    const auto& a = reference.analysis; const auto& b = candidate.analysis;
    Check(a.horizontalRatio == b.horizontalRatio && a.verticalRatio == b.verticalRatio &&
          a.warnings == b.warnings && a.matchedTracks == b.matchedTracks && a.modifiedKeys == b.modifiedKeys &&
          a.constraints == b.constraints && a.unresolved == b.unresolved && a.maxResidual == b.maxResidual &&
          a.samples.size() == b.samples.size() && a.localOffsets.size() == b.localOffsets.size(), "Parallel analysis differs");
    for (const auto& item : a.localOffsets) Check(item.second == b.localOffsets.at(item.first), "Parallel offsets differ");
    for (size_t i = 0; i < a.samples.size(); ++i)
    {
        const auto& x = a.samples[i]; const auto& y = b.samples[i];
        Check(x.stage == y.stage && x.frame == y.frame && x.bone == y.bone && x.target == y.target &&
              x.actual == y.actual && x.error == y.error, "Parallel diagnostic order/values differ");
    }
}

void CheckScheduler()
{
    struct Context { std::atomic_bool busy{false}; };
    const auto caller = std::this_thread::get_id();
    unsigned merged = 0;
    std::atomic_int active{0}, maximum{0};
    ProcessFrameBlocks(257, 4, nullptr, [] { return std::make_unique<Context>(); },
        [&](Context* context, uint32_t first, uint32_t end, const std::atomic_bool* cancel) {
            Check(context && !context->busy.exchange(true), "IK lane shared concurrently");
            const int count = ++active;
            int prior = maximum.load();
            while (prior < count && !maximum.compare_exchange_weak(prior, count)) {}
            std::this_thread::sleep_for(std::chrono::milliseconds(4));
            CheckCancel(cancel);
            --active; context->busy.store(false);
            return first;
        }, [&](uint32_t first, uint32_t end, uint32_t value) {
            Check(std::this_thread::get_id() == caller && value == first && first == merged, "Unordered/background publication");
            merged = end;
        });
    Check(merged == 257 && maximum > 1 && active == 0, "Scheduler did not execute concurrently");

    std::atomic_bool cancel{false};
    bool caught = false;
    try
    {
        ProcessFrameBlocks(257, 4, &cancel, [] { return std::make_unique<int>(0); },
            [](int*, uint32_t first, uint32_t end, const std::atomic_bool* stop) {
                CheckCancel(stop); return first;
            }, [&](uint32_t, uint32_t, uint32_t) { cancel.store(true); });
    }
    catch (const std::runtime_error&) { caught = true; }
    Check(caught, "Parallel cancellation ignored");
    caught = false;
    try
    {
        ProcessFrameBlocks(257, 4, nullptr, [] { return std::make_unique<int>(0); },
            [](int*, uint32_t first, uint32_t, const std::atomic_bool*) { return first; },
            [](uint32_t, uint32_t, uint32_t) { throw std::runtime_error("observer failure"); });
    }
    catch (const std::runtime_error& error) { caught = std::string(error.what()) == "observer failure"; }
    Check(caught, "Observer failure not propagated");
    caught = false;
    try
    {
        ProcessFrameBlocks(257, 4, nullptr, [] { return std::make_unique<int>(0); },
            [](int*, uint32_t first, uint32_t, const std::atomic_bool* stop) {
                if (first == 64) throw std::runtime_error("worker failure");
                CheckCancel(stop); return first;
            }, [](uint32_t, uint32_t, uint32_t) {});
    }
    catch (const std::runtime_error& error) { caught = std::string(error.what()) == "worker failure"; }
    Check(caught, "Worker failure not propagated in frame order");
}

std::vector<uint8_t> Read(const std::string& path)
{
    std::ifstream stream(std::filesystem::u8path(path), std::ios::binary);
    Check(stream.good(), "Cannot open model/motion asset");
    return {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
}

Options AllOptions()
{
    Options options;
    options.stance = options.twist = options.avoidance = options.legAvoidance = true;
    options.wristContact = options.fingerContact = options.floorContact = true;
    return options;
}

void Fixtures(const std::string& directory)
{
    libmmd::PMXFile source, target; libmmd::VMDFile motion;
    Check(libmmd::ReadPMXFile(&source, (directory + "/source.pmx").c_str()), "Source fixture");
    Check(libmmd::ReadPMXFile(&target, (directory + "/target.pmx").c_str()), "Target fixture");
    Check(libmmd::ReadVMDFile(&motion, (directory + "/motion.vmd").c_str()), "Motion fixture");
    std::map<std::string, libmmd::VMDMotion> tails;
    for (const auto& key : motion.m_motions)
    {
        auto entry = tails.find(key.m_boneName.ToUtf8String());
        if (entry == tails.end() || entry->second.m_frame < key.m_frame) tails[key.m_boneName.ToUtf8String()] = key;
    }
    for (auto& item : tails) { item.second.m_frame = 256; motion.m_motions.push_back(item.second); }
    for (size_t cap : {size_t(0), size_t(2), size_t(20000)})
    {
        auto options = AllOptions(); options.maxDiagnostics = cap; options.workerCount = 1;
        const auto reference = Run(source, target, motion, options);
        Check(reference.success && reference.analysis.constraints > 0, "Fixture constraints inactive");
        for (unsigned workers : {2u, 4u, 16u, 0u})
        {
            options.workerCount = workers;
            const auto caller = std::this_thread::get_id();
            const auto candidate = Run(source, target, motion, options, nullptr, [&](const Progress&) {
                Check(std::this_thread::get_id() == caller, "Observer invoked on worker");
            });
            Equal(reference, candidate);
        }
    }
}

// Licensed PMX/VMD assets stay external; this mode is opt-in, outside CTest/CI.
void Assets(const std::string& manifest, const std::string& directory, unsigned repeats)
{
    std::ifstream input(manifest);
    std::vector<std::string> paths;
    for (std::string line; std::getline(input, line); )
    { if (!line.empty() && line.back() == '\r') line.pop_back(); paths.push_back(line); }
    Check(paths.size() == 4 && repeats > 0, "Expected four-path UTF-8 manifest and repeats");
    CharacterInput character; libmmd::VMDFile camera;
    auto data = Read(paths[0]); Check(libmmd::ReadPMXFile(&character.source, data.data(), data.size()), "Source parse");
    data = Read(paths[1]); Check(libmmd::ReadPMXFile(&character.target, data.data(), data.size()), "Target parse");
    data = Read(paths[2]); Check(libmmd::ReadVMDFile(&character.motion, data.data(), data.size()), "Motion parse");
    data = Read(paths[3]); Check(libmmd::ReadVMDFile(&camera, data.data(), data.size()), "Camera parse");
    character.options = AllOptions();
    std::filesystem::create_directories(directory);
    std::ofstream receipt(std::filesystem::path(directory) / "timings.tsv");
    receipt << "repeat\tworkers\twall_ms\tphase\tphase_ms\n";
    BatchResult reference;
    for (unsigned repeat = 0; repeat < repeats; ++repeat)
        for (unsigned workers : {1u, 4u})
        {
            character.options.workerCount = workers;
            std::map<ProgressPhase, double> timings;
            auto tick = std::chrono::steady_clock::now();
            ProgressPhase phase = ProgressPhase::Validation;
            const auto started = tick;
            auto result = RunBatch({character}, camera, CameraOptions{true, 5.}, nullptr, [&](const Progress& value) {
                const auto now = std::chrono::steady_clock::now();
                timings[phase] += std::chrono::duration<double, std::milli>(now - tick).count();
                tick = now; phase = value.phase;
            });
            const auto finished = std::chrono::steady_clock::now();
            timings[phase] += std::chrono::duration<double, std::milli>(finished - tick).count();
            const double wall = std::chrono::duration<double, std::milli>(finished - started).count();
            Check(result.success, result.error.c_str());
            if (repeat == 0 && workers == 1) reference = result;
            Equal(reference.characters[0], result.characters[0]);
            Check(Bytes(reference.camera) == Bytes(result.camera), "Camera bytes differ");
            for (const auto& timing : timings)
                receipt << repeat << '\t' << workers << '\t' << wall << '\t' << int(timing.first) << '\t' << timing.second << '\n';
            receipt.flush();
            std::cout << "repeat=" << repeat << " workers=" << workers << " wall_ms=" << wall << " all_stage_bytes_equal=true\n" << std::flush;
            if (repeat == 0)
            {
                for (size_t stage = 0; stage < result.characters[0].stages.size(); ++stage)
                    Check(libmmd::WriteVMDFile(&result.characters[0].stages[stage],
                        (std::filesystem::path(directory) / ("workers-" + std::to_string(workers) + "-stage-" + std::to_string(stage) + ".vmd")).string().c_str()), "Cannot write comparison VMD");
                Check(libmmd::WriteVMDFile(&result.camera,
                    (std::filesystem::path(directory) / ("workers-" + std::to_string(workers) + "-camera.vmd")).string().c_str()), "Cannot write comparison camera");
            }
        }
}
}

int main(int argc, char** argv)
{
    try
    {
        if (argc == 2) { CheckScheduler(); Fixtures(argv[1]); }
        else if (argc == 5 && std::string(argv[1]) == "--assets") Assets(argv[2], argv[3], unsigned(std::stoul(argv[4])));
        else throw std::runtime_error("Expected fixture directory, or --assets manifest output repeats");
        std::cout << "parallel motion sizing passed\n";
        return 0;
    }
    catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
