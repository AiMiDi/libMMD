#include "MMDMotionPose.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace libmmd::sizing::detail
{
void Require(bool condition, const std::string& error)
{
    if (!condition) throw std::runtime_error(error);
}

void CheckCancel(const std::atomic_bool* cancel)
{
    if (cancel && cancel->load(std::memory_order_relaxed)) throw std::runtime_error("Sizing cancelled");
}

void Record(Analysis& analysis, const Options& options, Stage stage, uint32_t frame,
            const std::string& bone, const Vector& target, const Vector& actual)
{
    const double error = (target - actual).norm();
    Require(std::isfinite(error), "Non-finite constraint residual");
    ++analysis.constraints;
    if (error > options.tolerance) ++analysis.unresolved;
    analysis.maxResidual = std::max(analysis.maxResidual, error);
    if (analysis.samples.size() < options.maxDiagnostics)
        analysis.samples.push_back({stage, frame, bone, target, actual, error});
}

}
