#pragma once
#include "libMMD/Model/MMD/MMDMotionSizing.h"

namespace libmmd::sizing::detail
{
// Bound observer overhead without using wall-clock time in the computation.
// Always publish phase boundaries, including empty/indeterminate work.
inline void ReportProgress(const ProgressCallback& callback, ProgressPhase phase,
                           uint64_t completed = 0, uint64_t total = 0)
{
    if (callback && (completed == 0 || completed == total || completed % 64 == 0))
        callback(Progress{phase, completed, total});
}
}
