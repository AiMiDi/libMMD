#include "libMMD/Model/MMD/PMXFile.h"

#include <chrono>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace
{
void Check(bool condition, const char* message)
{
    if (!condition) throw std::runtime_error(message);
}

class TemporaryModel
{
public:
    TemporaryModel()
        : path_(std::filesystem::temp_directory_path() /
            ("libmmd_vertex_weights_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".pmx")) {}
    ~TemporaryModel() { std::error_code error; std::filesystem::remove(path_, error); }
    std::string Path() const { return path_.string(); }
private:
    std::filesystem::path path_;
};

void CheckDefaultsAndInfluences()
{
    libmmd::PMXVertex vertex; // Default construction, intentionally without {}.
    Check(vertex.m_position.isZero() && vertex.m_normal.isZero() && vertex.m_uv.isZero(), "Uninitialized vertex vectors");
    for (const auto& uv : vertex.m_addUV) Check(uv.isZero(), "Uninitialized additional UV");
    Check(vertex.m_sdefC.isZero() && vertex.m_sdefR0.isZero() && vertex.m_sdefR1.isZero(), "Uninitialized SDEF vectors");
    Check(vertex.m_weightType == libmmd::PMXVertexWeight::BDEF1 && vertex.m_edgeMag == 1.F, "Invalid default vertex metadata");
    for (int i = 0; i < 4; ++i)
    {
        Check(vertex.m_boneIndices[i] == 0, "Uninitialized bone index");
        Check(vertex.m_boneWeights[i] == (i == 0 ? 1.F : 0.F), "Uninitialized bone weight");
    }
    const float unused = std::numeric_limits<float>::quiet_NaN();
    for (float& weight : vertex.m_boneWeights) weight = unused;
    Check(libmmd::GetPMXVertexInfluenceCount(vertex.m_weightType) == 1 &&
          libmmd::GetPMXVertexInfluenceWeight(vertex, 0) == 1.F, "BDEF1 read an unstored weight");
    for (const auto type : {libmmd::PMXVertexWeight::BDEF2, libmmd::PMXVertexWeight::SDEF})
    {
        vertex.m_weightType = type; vertex.m_boneWeights[0] = .25F;
        Check(libmmd::GetPMXVertexInfluenceCount(type) == 2, "Two-bone influence count");
        Check(libmmd::GetPMXVertexInfluenceWeight(vertex, 0) == .25F &&
              libmmd::GetPMXVertexInfluenceWeight(vertex, 1) == .75F, "Implicit complementary weight ignored");
        vertex.m_boneWeights[0] = 1.F;
        Check(libmmd::GetPMXVertexInfluenceWeight(vertex, 1) == 0.F, "Zero second influence became active");
        Check(libmmd::GetPMXVertexInfluenceWeight(vertex, -1) == 0.F &&
              libmmd::GetPMXVertexInfluenceWeight(vertex, 2) == 0.F, "Invalid influence accessed a slot");
    }
    for (const auto type : {libmmd::PMXVertexWeight::BDEF4, libmmd::PMXVertexWeight::QDEF})
    {
        vertex.m_weightType = type;
        Check(libmmd::GetPMXVertexInfluenceCount(type) == 4, "Four-bone influence count");
        for (int i = 0; i < 4; ++i)
        {
            vertex.m_boneWeights[i] = (i + 1) * .1F;
            Check(libmmd::GetPMXVertexInfluenceWeight(vertex, i) == vertex.m_boneWeights[i], "Explicit weight changed");
        }
    }
    vertex.m_weightType = static_cast<libmmd::PMXVertexWeight>(255);
    Check(libmmd::GetPMXVertexInfluenceCount(vertex.m_weightType) == 0 &&
          libmmd::GetPMXVertexInfluenceWeight(vertex, 0) == 0.F, "Unknown weight type accepted");
}

void CheckWeightRoundTrip()
{
    libmmd::PMXFile model{};
    model.m_header.m_magic.Set("PMX ");
    model.m_header.m_version = 2.1F; model.m_header.m_dataSize = 8;
    model.m_header.m_encode = 1;
    model.m_header.m_vertexIndexSize = model.m_header.m_textureIndexSize = model.m_header.m_materialIndexSize = 4;
    model.m_header.m_boneIndexSize = model.m_header.m_morphIndexSize = model.m_header.m_rigidbodyIndexSize = 4;
    libmmd::PMXVertex qdef;
    qdef.m_weightType = libmmd::PMXVertexWeight::QDEF;
    for (int i = 0; i < 4; ++i) { qdef.m_boneIndices[i] = i; qdef.m_boneWeights[i] = (i + 1) * .1F; }
    qdef.m_sdefC.x() = 19.F; // Former weights[4] overrun touched this adjacent field.
    qdef.m_edgeMag = .375F;
    model.m_vertices.push_back(qdef);
    for (const auto type : {libmmd::PMXVertexWeight::BDEF2, libmmd::PMXVertexWeight::SDEF})
    {
        libmmd::PMXVertex vertex;
        vertex.m_weightType = type; vertex.m_boneIndices[0] = 1; vertex.m_boneIndices[1] = 2;
        vertex.m_boneWeights[0] = .25F;
        vertex.m_boneWeights[1] = std::numeric_limits<float>::quiet_NaN(); // Not part of the file.
        model.m_vertices.push_back(vertex);
    }
    TemporaryModel file;
    Check(libmmd::WritePMXFile(&model, file.Path().c_str()), "Write vertex weights");
    std::ifstream stream(file.Path(), std::ios::binary);
    std::vector<uint8_t> bytes{std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
    stream.close();
    // Independently inspect the first vertex's disk weights. A writer and
    // reader with the same shifted index bug must not validate each other.
    const size_t weightsOffset = 17 + 4 * sizeof(int32_t) + sizeof(int32_t) + 8 * sizeof(float) + 1 + 4 * sizeof(int32_t);
    Check(bytes.size() >= weightsOffset + 5 * sizeof(float), "Truncated QDEF vertex");
    for (int i = 0; i < 4; ++i)
    {
        float value = 0.F;
        std::memcpy(&value, bytes.data() + weightsOffset + i * sizeof(float), sizeof(float));
        Check(value == qdef.m_boneWeights[i], "QDEF disk weight index is wrong");
    }
    libmmd::PMXFile loaded;
    Check(libmmd::ReadPMXFile(&loaded, bytes.data(), bytes.size()) && loaded.m_vertices.size() == 3, "Read vertex weights");
    for (int i = 0; i < 4; ++i)
        Check(loaded.m_vertices[0].m_boneIndices[i] == i &&
              loaded.m_vertices[0].m_boneWeights[i] == qdef.m_boneWeights[i], "QDEF roundtrip lost a weight");
    Check(loaded.m_vertices[0].m_sdefC.isZero() && loaded.m_vertices[0].m_edgeMag == qdef.m_edgeMag,
          "QDEF read overwrote adjacent storage or changed record layout");
    for (size_t i = 1; i < loaded.m_vertices.size(); ++i)
        Check(libmmd::GetPMXVertexInfluenceWeight(loaded.m_vertices[i], 1) == .75F,
              "Parsed two-bone vertex lost its implicit influence");
}
}

int main()
{
    try
    {
        CheckDefaultsAndInfluences();
        CheckWeightRoundTrip();
        std::cout << "PMX vertex weight regression passed\n";
        return 0;
    }
    catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
