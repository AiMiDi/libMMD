#pragma once
#include <array>
#include <cstdint>
#include <memory>
#include <vector>

namespace libmmd { struct PMXFile; }
namespace libmmd::sizing::detail
{
// Scalar-only boundary: playback nodes use libMMD's Eigen ISA/alignment;
// sizing's private double-precision poses are compiled independently.
using PlaybackChannel = std::array<double, 7>; // translation xyz, quaternion xyzw
class PlaybackIK
{
public:
    virtual ~PlaybackIK() = default;
    virtual void Evaluate(const std::vector<PlaybackChannel>& local, const std::vector<uint8_t>& enabled,
                          std::vector<PlaybackChannel>& world) = 0;
};
std::shared_ptr<PlaybackIK> CreatePlaybackIK(const libmmd::PMXFile& model, const std::vector<int>& parentOrder,
                                          const std::vector<int>& appendOrder);
}
