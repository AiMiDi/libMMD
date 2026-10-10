// Movement formulae adapted from miu200521358/vmd_sizing (MIT), revision
// e5c3035. See license/vmd_sizing-MIT.txt and docs/MotionSizing.md.
#include "libMMD/Model/MMD/MMDMotionSizing.h"
#include "MMDMotionProgress.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <limits>
#include <set>
#include <stdexcept>
#include <unordered_map>

namespace libmmd::sizing::detail
{
namespace
{
constexpr double Epsilon = 1.e-9;
using Index = std::unordered_map<std::string, size_t>;
using Tracks = std::unordered_map<std::string, std::vector<libmmd::VMDMotion>>;
const std::array<std::string, 9> MoveBones = {
    "全ての親", "センター", "グルーブ", "右足IK親", "左足IK親",
    "右足ＩＫ", "左足ＩＫ", "右つま先ＩＫ", "左つま先ＩＫ"};

void Require(bool condition, const std::string& message)
{
    if (!condition) throw std::runtime_error(message);
}

Index ValidateModel(const libmmd::PMXFile& model)
{
    Index names;
    Require(!model.m_bones.empty(), "Model has no bones");
    for (size_t i = 0; i < model.m_bones.size(); ++i)
    {
        const auto& bone = model.m_bones[i];
        Require(!bone.m_name.empty() && names.emplace(bone.m_name, i).second,
                "Empty or duplicate bone name: " + bone.m_name);
        Require(bone.m_position.allFinite(), "Non-finite bind position: " + bone.m_name);
        Require(bone.m_parentBoneIndex >= -1 && bone.m_parentBoneIndex < static_cast<int>(model.m_bones.size()),
                "Invalid parent: " + bone.m_name);
    }
    // Iterative traversal avoids recursive stack exhaustion on malformed PMX.
    std::vector<unsigned char> state(model.m_bones.size(), 0);
    for (size_t i = 0; i < state.size(); ++i)
    {
        int node = static_cast<int>(i);
        std::vector<size_t> path;
        while (node >= 0 && state[static_cast<size_t>(node)] == 0)
        {
            const size_t index = static_cast<size_t>(node);
            state[index] = 1;
            path.push_back(index);
            node = model.m_bones[index].m_parentBoneIndex;
        }
        Require(node < 0 || state[static_cast<size_t>(node)] != 1, "Cyclic bone hierarchy");
        for (size_t index : path) state[index] = 2;
    }
    return names;
}

Eigen::Vector3d Position(const libmmd::PMXFile& model, const Index& index, const std::string& name)
{
    const auto it = index.find(name);
    Require(it != index.end(), "Required standard bone missing: " + name);
    return model.m_bones[it->second].m_position.cast<double>();
}

Tracks ReadTracks(const libmmd::VMDFile& input)
{
    Require(!input.m_motions.empty(), "VMD contains no bone motion");
    Tracks tracks;
    for (const auto& key : input.m_motions)
    {
        Require(key.m_frame <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()), "Frame exceeds supported range");
        Require(key.m_translate.allFinite() && key.m_quaternion.coeffs().allFinite() &&
                key.m_quaternion.squaredNorm() > 1.e-12f, "Non-finite or zero motion quaternion");
        tracks[key.m_boneName.ToUtf8String()].emplace_back(key);
    }
    for (auto& entry : tracks)
    {
        auto& keys = entry.second;
        std::sort(keys.begin(), keys.end(), [](const auto& a, const auto& b) { return a.m_frame < b.m_frame; });
        for (size_t i = 1; i < keys.size(); ++i)
            Require(keys[i - 1].m_frame != keys[i].m_frame, "Duplicate VMD bone/frame: " + entry.first);
    }
    return tracks;
}

Eigen::Quaterniond SampleRotation(const Tracks& tracks, const std::string& name, int32_t frame)
{
    const auto found = tracks.find(name);
    if (found == tracks.end()) return Eigen::Quaterniond::Identity();
    const auto& keys = found->second;
    const auto next = std::upper_bound(keys.begin(), keys.end(), frame,
        [](int32_t value, const auto& key) { return static_cast<uint32_t>(value) < key.m_frame; });
    if (next == keys.begin()) return keys.front().m_quaternion.cast<double>().normalized();
    if (next == keys.end()) return keys.back().m_quaternion.cast<double>().normalized();
    const auto& previous = *(next - 1);
    if (previous.m_frame == static_cast<uint32_t>(frame)) return previous.m_quaternion.cast<double>().normalized();
    // Upstream VmdData uses the arriving key's fourth rotation-curve replica
    // and MBezierUtils' 15-step bisection. This is intentionally local to sizing:
    // libMMD runtime interpolation has a different segment convention.
    const auto& curve = next->m_interpolation;
    const double x1 = curve[48] / 127., y1 = curve[52] / 127.;
    const double x2 = curve[56] / 127., y2 = curve[60] / 127.;
    const double time = (static_cast<double>(frame) - previous.m_frame) / (next->m_frame - previous.m_frame);
    double t = .5, s = .5;
    for (int i = 0; i < 15; ++i)
    {
        const double difference = 3 * s * s * t * x1 + 3 * s * t * t * x2 + t * t * t - time;
        t += (difference > 0 ? -1. : 1.) / (4 << i);
        s = 1 - t;
    }
    const double weight = 3 * s * s * t * y1 + 3 * s * t * t * y2 + t * t * t;
    return previous.m_quaternion.cast<double>().normalized().slerp(weight, next->m_quaternion.cast<double>().normalized()).normalized();
}

std::vector<size_t> RotationChain(const libmmd::PMXFile& model, const Index& names, size_t index)
{
    // MoveService uses PmxModel.PARENT_BORN_PAIR (is_defined=True), not the
    // actual PMX parent chain. Preserve that choice for these standard tracks.
    std::map<std::string, std::vector<std::string>> parents = {
        {"全ての親", {}}, {"センター", {"全ての親"}}, {"グルーブ", {"センター"}}};
    for (const std::string side : {"左", "右"})
    {
        parents[side + "足IK親"] = {"全ての親"};
        parents[side + "足ＩＫ"] = {side + "足IK親", "全ての親"};
        parents[side + "つま先ＩＫ"] = {side + "足ＩＫ"};
    }
    std::vector<size_t> chain;
    std::string name = model.m_bones[index].m_name;
    while (!name.empty())
    {
        chain.push_back(names.at(name));
        const auto candidates = parents.at(name);
        name.clear();
        for (const auto& parent : candidates)
            if (names.count(parent)) { name = parent; break; }
    }
    std::reverse(chain.begin(), chain.end());
    return chain;
}

bool HasAdvancedAxis(const libmmd::PMXFile& model, const std::vector<size_t>& chain)
{
    // Upstream's append/fixed-axis conventions differ from runtime PMX IK.
    // This legacy offset formula cannot evaluate those channels; keep scaling.
    for (size_t index : chain)
        if ((static_cast<uint16_t>(model.m_bones[index].m_boneFlag) & 0x2700u) != 0) return true;
    return false;
}

double ToeZ(const libmmd::PMXFile& model, const Index& index, Analysis& analysis)
{
    const double ankleY = Position(model, index, "左足首").y();
    std::set<int32_t> candidates;
    for (size_t i = 0; i < model.m_bones.size(); ++i)
    {
        const auto& bone = model.m_bones[i];
        if (bone.m_position.x() > 0 && bone.m_position.y() <= ankleY && bone.m_name.find("左") != std::string::npos)
            candidates.insert(static_cast<int32_t>(i));
    }
    double front = std::numeric_limits<double>::infinity();
    for (const auto& vertex : model.m_vertices)
    {
        const int count = vertex.m_weightType == libmmd::PMXVertexWeight::BDEF1 ? 1 :
            (vertex.m_weightType == libmmd::PMXVertexWeight::BDEF2 || vertex.m_weightType == libmmd::PMXVertexWeight::SDEF ? 2 : 4);
        for (int j = 0; j < count; ++j)
            if (candidates.count(vertex.m_boneIndices[j]) && (count == 1 || vertex.m_boneWeights[j] > 0))
            {
                Require(vertex.m_position.allFinite(), "Non-finite foot vertex");
                front = std::min(front, static_cast<double>(vertex.m_position.z()));
            }
    }
    if (std::isfinite(front)) return front;
    analysis.warnings.emplace_back("Toe reference uses bone fallback (no weighted foot vertices)");
    for (const auto* name : {"左つま先", "左つま先ＩＫ", "左足首"})
        if (index.count(name)) return Position(model, index, name).z();
    throw std::runtime_error("Missing toe reference");
}

void CalculateOffsets(const libmmd::PMXFile& source, const libmmd::PMXFile& target,
                      const Index& si, const Index& ti, const Tracks& tracks,
                      const Options& options, double rawHorizontal, Analysis& analysis)
{
    const auto s = [&](const std::string& name) { return Position(source, si, name); };
    const auto t = [&](const std::string& name) { return Position(target, ti, name); };
    if (options.centerOffsets)
    {
        Eigen::Vector3d offset = Eigen::Vector3d::Zero();
        const Eigen::Vector3d targetAnkle = t("左足首") - Eigen::Vector3d(options.legOffset, 0, 0);
        // Keep upstream's use of target ankle here for reference compatibility.
        const double sourceLength = (s("左ひざ") - s("左足")).norm() + (s("左ひざ") - targetAnkle).norm();
        Require(sourceLength > Epsilon, "Degenerate center Y reference");
        const double ratio = (s("左足") - targetAnkle).y() / sourceLength;
        const double targetLength = (t("左ひざ") - t("左足")).norm() + (t("左ひざ") - t("左足首")).norm();
        offset.y() = std::min(0., ratio * targetLength - (t("左足") - t("左足首")).y());
        if (si.count("左つま先ＩＫ") && ti.count("左つま先ＩＫ"))
        {
            const double sourceFoot = s("左足首").z() - ToeZ(source, si, analysis);
            const double targetFoot = t("左足首").z() - ToeZ(target, ti, analysis);
            if (std::abs(sourceFoot) > Epsilon && std::abs(targetFoot) > Epsilon)
                offset.z() = ((t("左足首").z() - t("左足").z()) / targetFoot -
                              (s("左足首").z() - s("左足").z()) / sourceFoot) * targetFoot / sourceFoot;
            else analysis.warnings.emplace_back("Center Z skipped: zero foot depth");
        }
        else analysis.warnings.emplace_back("Center Z skipped: missing toe IK");
        analysis.localOffsets["センター"] = offset;
    }
    if (options.legOffsets)
        for (const std::string side : {"左", "右"})
        {
            const std::string leg = side + "足", ik = side + "足ＩＫ", parent = side + "足IK親";
            if (!si.count(leg) || !ti.count(leg) || !si.count(ik) || !ti.count(ik))
            {
                analysis.warnings.emplace_back("Leg offset skipped: missing " + leg + "/" + ik);
                continue;
            }
            double x = ((s(ik) - s(leg)) / rawHorizontal - (t(ik) - t(leg))).x();
            const double tx = t(ik).x();
            if (std::abs(x * rawHorizontal) > std::abs(tx))
            {
                const double signX = (x > 0) - (x < 0), signTarget = (tx > 0) - (tx < 0);
                x = (tx - x * rawHorizontal) * rawHorizontal * (signX == signTarget ? 1. : -1.);
            }
            x += options.legOffset * ((tx > 0) - (tx < 0));
            analysis.localOffsets[ti.count(parent) && tracks.count(parent) ? parent : ik] = Eigen::Vector3d(x, 0, 0);
        }
}
}

Result RunMovement(const libmmd::PMXFile& source, const libmmd::PMXFile& target,
           const libmmd::VMDFile& input, const Options& options, const std::atomic_bool* cancel, const ProgressCallback& progress)
{
    const auto started = std::chrono::steady_clock::now();
    Result result;
    const auto cancelled = [&]() { return cancel && cancel->load(std::memory_order_relaxed); };
    try
    {
        if (cancelled()) { result.cancelled = true; return result; }
        ReportProgress(progress, ProgressPhase::Validation);
        Require(std::isfinite(options.movementMultiplier) && options.movementMultiplier > 0 &&
                std::isfinite(options.legOffset), "Invalid sizing options");
        const Index si = ValidateModel(source), ti = ValidateModel(target);
        const Tracks tracks = ReadTracks(input);
        for (const auto* name : {"左足", "左ひざ", "左足首", "センター"})
        {
            Position(source, si, name);
            Position(target, ti, name);
        }
        const Eigen::Vector3d sourceLeg = Position(source, si, "左足首") - Position(source, si, "左足");
        const Eigen::Vector3d targetLeg = Position(target, ti, "左足首") - Position(target, ti, "左足");
        Require(sourceLeg.norm() > Epsilon && std::abs(sourceLeg.y()) > Epsilon && targetLeg.norm() > Epsilon,
                "Degenerate leg proportions");
        const double rawHorizontal = targetLeg.norm() / sourceLeg.norm();
        result.analysis.horizontalRatio = rawHorizontal * options.movementMultiplier;
        result.analysis.verticalRatio = targetLeg.y() / sourceLeg.y();
        Require(result.analysis.verticalRatio > 0, "Opposite or zero vertical leg proportions");
        CalculateOffsets(source, target, si, ti, tracks, options, rawHorizontal, result.analysis);
        for (const auto& entry : tracks)
        {
            if (ti.count(entry.first)) ++result.analysis.matchedTracks;
            else result.analysis.warnings.emplace_back("Unmatched target motion track: " + entry.first);
        }
        result.stages[0] = input;
        result.stages[1] = input;
        result.stages[2] = input;
        std::map<std::string, std::vector<size_t>> chains;
        for (const auto& name : MoveBones)
            if (ti.count(name) && tracks.count(name))
            {
                chains[name] = RotationChain(target, ti, ti.at(name));
                if (HasAdvancedAxis(target, chains.at(name)))
                {
                    result.analysis.localOffsets.erase(name);
                    result.analysis.warnings.emplace_back("Offset skipped for append/fixed-axis/outer-parent chain: " + name);
                }
            }
        Require(!chains.empty(), "No supported movement tracks match the target");
        for (size_t i = 0; i < input.m_motions.size(); ++i)
        {
            if (cancelled()) { result.cancelled = true; for (auto& stage : result.stages) stage = libmmd::VMDFile(); return result; }
            ReportProgress(progress, ProgressPhase::Movement, i, input.m_motions.size());
            const auto& key = input.m_motions[i];
            const std::string name = key.m_boneName.ToUtf8String();
            const auto chain = chains.find(name);
            if (chain == chains.end()) continue;
            Eigen::Vector3d position = key.m_translate.cast<double>();
            position.x() *= result.analysis.horizontalRatio;
            position.z() *= result.analysis.horizontalRatio;
            position.y() *= result.analysis.verticalRatio;
            Require(position.cast<float>().allFinite(), "Scaled motion exceeds float range");
            result.stages[1].m_motions[i].m_translate = position.cast<float>();
            const auto offset = result.analysis.localOffsets.find(name);
            if (offset != result.analysis.localOffsets.end())
            {
                Eigen::Quaterniond rotation = Eigen::Quaterniond::Identity();
                for (size_t bone : chain->second)
                    rotation = rotation * SampleRotation(tracks, target.m_bones[bone].m_name, static_cast<int32_t>(key.m_frame));
                position += rotation * offset->second;
            }
            Require(position.cast<float>().allFinite(), "Offset motion exceeds float range");
            result.stages[2].m_motions[i].m_translate = position.cast<float>();
            ++result.analysis.modifiedKeys;
        }
        if (cancelled()) { result.cancelled = true; for (auto& stage : result.stages) stage = libmmd::VMDFile(); return result; }
        ReportProgress(progress, ProgressPhase::Movement, input.m_motions.size(), input.m_motions.size());
        result.success = true;
    }
    catch (const std::exception& error)
    {
        result.error = error.what();
        for (auto& stage : result.stages) stage = libmmd::VMDFile();
    }
    result.elapsedMilliseconds = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - started).count();
    return result;
}
}
