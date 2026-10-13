#pragma once

#include "MMDMotionPose.h"
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <exception>
#include <mutex>
#include <optional>
#include <thread>

namespace libmmd::sizing::detail
{
// Only independent frame work belongs here. Each lane owns its IK context;
// at most one block per lane is in flight. The calling thread consumes blocks
// in frame order and is the only thread allowed to publish progress/results.
// A small fixed block bounds temporary key/diagnostic memory and cancellation
// latency without changing arithmetic within a frame.
constexpr uint32_t FrameBlockSize = 64;

inline unsigned FrameWorkerCount(uint32_t frames, unsigned requested)
{
    const unsigned available = std::max(1u, std::thread::hardware_concurrency());
    const unsigned limit = requested == 0 ? std::min(available, 4u) : std::min(requested, 16u);
    return std::max(1u, std::min(limit, unsigned((uint64_t(frames) + FrameBlockSize - 1) / FrameBlockSize)));
}

template<class Factory, class Compute, class Consume>
void ProcessFrameBlocks(uint32_t frames, unsigned requested, const std::atomic_bool* cancel,
                        Factory factory, Compute compute, Consume consume)
{
    using Context = decltype(factory());
    using Block = decltype(compute(factory().get(), uint32_t{}, uint32_t{}, cancel));
    const unsigned workers = FrameWorkerCount(frames, requested);
    if (workers == 1)
    {
        // Preserve the original serial context and full-range execution path.
        consume(0, frames, compute(nullptr, 0, frames, cancel));
        return;
    }
    struct Lane
    {
        Context context;
        std::optional<Block> block;
        std::exception_ptr failure;
        bool ready = false;
    };
    std::vector<Lane> lanes(workers);
    for (auto& lane : lanes)
    {
        CheckCancel(cancel);
        lane.context = factory();
    }
    std::atomic_bool stop{false};
    std::mutex mutex;
    std::condition_variable condition;
    std::vector<std::jthread> threads;
    threads.reserve(workers);
    try
    {
        for (unsigned index = 0; index < workers; ++index)
            threads.emplace_back([&, index](std::stop_token token) {
                // jthread's stop request also wakes a lane waiting for the
                // coordinator to consume its previous block.
                std::stop_callback wake(token, [&] { condition.notify_all(); });
                auto& lane = lanes[index];
                for (uint64_t first = uint64_t(index) * FrameBlockSize; first < frames;
                     first += uint64_t(workers) * FrameBlockSize)
                {
                    if (token.stop_requested() || stop.load(std::memory_order_relaxed)) return;
                    try
                    {
                        CheckCancel(cancel);
                        auto block = compute(lane.context.get(), uint32_t(first),
                            uint32_t(std::min(uint64_t(frames), first + FrameBlockSize)), &stop);
                        std::unique_lock lock(mutex);
                        lane.block.emplace(std::move(block));
                        lane.ready = true;
                        condition.notify_all();
                        condition.wait(lock, [&] {
                            return !lane.ready || token.stop_requested() || stop.load(std::memory_order_relaxed);
                        });
                    }
                    catch (...)
                    {
                        std::lock_guard lock(mutex);
                        lane.failure = std::current_exception();
                        lane.ready = true;
                        condition.notify_all();
                        return;
                    }
                }
            });
        for (uint32_t first = 0, index = 0; first < frames; first += FrameBlockSize, ++index)
        {
            auto& lane = lanes[index % workers];
            std::unique_lock lock(mutex);
            while (!lane.ready)
            {
                condition.wait_for(lock, std::chrono::milliseconds(10));
                CheckCancel(cancel);
            }
            CheckCancel(cancel);
            if (lane.failure) std::rethrow_exception(lane.failure);
            auto block = std::move(*lane.block);
            lane.block.reset();
            lane.ready = false;
            lock.unlock();
            condition.notify_all();
            consume(first, std::min(frames, first + FrameBlockSize), std::move(block));
        }
    }
    catch (...)
    {
        stop.store(true, std::memory_order_relaxed);
        condition.notify_all();
        // Join before destroying contexts or propagating observer/worker errors.
        threads.clear();
        throw;
    }

}

struct FrameBlockResult
{
    Baker baker;
    Analysis analysis;
};

inline void MergeFrameAnalysis(Analysis& output, const Analysis& block, const Options& options)
{
    output.constraints += block.constraints;
    output.unresolved += block.unresolved;
    output.maxResidual = std::max(output.maxResidual, block.maxResidual);
    output.warnings.insert(output.warnings.end(), block.warnings.begin(), block.warnings.end());
    const size_t remaining = options.maxDiagnostics > output.samples.size()
        ? options.maxDiagnostics - output.samples.size() : 0;
    const size_t count = std::min(remaining, block.samples.size());
    output.samples.insert(output.samples.end(), block.samples.begin(), block.samples.begin() + count);
}
}
