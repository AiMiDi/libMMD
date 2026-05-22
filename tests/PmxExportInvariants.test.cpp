#define _USE_MATH_DEFINES
#include <cmath>

#include <libMMD/Model/MMD/PMXFile.h>
#include <libMMD/Base/File.h>

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

// ---------------------------------------------------------------------------
// Minimal test framework (same as FileRoundTrip.test.cpp)
// ---------------------------------------------------------------------------

static int g_totalTests = 0;
static int g_failedTests = 0;

#define TEST_ASSERT(expr)                                                  \
    do {                                                                   \
        ++g_totalTests;                                                    \
        if (!(expr)) {                                                     \
            std::cerr << "  FAIL: " #expr << "  (" << __FILE__             \
                      << ":" << __LINE__ << ")\n";                         \
            ++g_failedTests;                                               \
        }                                                                  \
    } while (0)

#define TEST_ASSERT_EQ(expected, actual)                                   \
    do {                                                                   \
        ++g_totalTests;                                                    \
        if ((expected) != (actual)) {                                      \
            std::cerr << "  FAIL: expected " << (expected)                 \
                      << " but got " << (actual) << "  (" << __FILE__      \
                      << ":" << __LINE__ << ")\n";                         \
            ++g_failedTests;                                               \
        }                                                                  \
    } while (0)

#define TEST_ASSERT_FLOAT_EQ(expected, actual)                             \
    do {                                                                   \
        ++g_totalTests;                                                    \
        if (std::fabs((expected) - (actual)) > 1e-5f) {                    \
            std::cerr << "  FAIL: expected " << (expected)                 \
                      << " but got " << (actual) << "  (" << __FILE__      \
                      << ":" << __LINE__ << ")\n";                         \
            ++g_failedTests;                                               \
        }                                                                  \
    } while (0)

// ---------------------------------------------------------------------------
// Temp file helper
// ---------------------------------------------------------------------------

class TempFile
{
public:
    explicit TempFile(const std::string& ext)
    {
        m_pathStr = (std::filesystem::temp_directory_path() / ("libmmd_test_" + std::to_string(reinterpret_cast<uintptr_t>(this)) + ext)).string();
    }
    ~TempFile()
    {
        std::filesystem::remove(m_pathStr);
    }
    const char* path() const { return m_pathStr.c_str(); }
    const std::string& pathStr() const { return m_pathStr; }
private:
    std::string m_pathStr;
};

// ---------------------------------------------------------------------------
// PMX export invariant validation (mirrors plugin export consistency rules)
// ---------------------------------------------------------------------------

static bool BoneHasIkFlag(const libmmd::PMXBone& bone)
{
    return (static_cast<uint16_t>(bone.m_boneFlag) &
            static_cast<uint16_t>(libmmd::PMXBoneFlags::IK)) != 0;
}

static bool IsValidBoneIndex(int32_t idx, size_t boneCount)
{
    return idx == -1 || (idx >= 0 && static_cast<size_t>(idx) < boneCount);
}

static bool IsValidRigidIndex(int32_t idx, size_t rigidCount)
{
    return idx == -1 || (idx >= 0 && static_cast<size_t>(idx) < rigidCount);
}

static bool IsValidTextureIndex(int32_t idx, size_t textureCount)
{
    return idx == -1 || (idx >= 0 && static_cast<size_t>(idx) < textureCount);
}

static bool IsValidMaterialToonIndex(const libmmd::PMXMaterial& mat, size_t textureCount)
{
    if (mat.m_toonMode == libmmd::PMXToonMode::Separate)
        return IsValidTextureIndex(mat.m_toonTextureIndex, textureCount);
    if (mat.m_toonMode == libmmd::PMXToonMode::Common)
        return mat.m_toonTextureIndex >= 0 && mat.m_toonTextureIndex <= 9;
    return false;
}

static bool ValidatePmxExportInvariants(const libmmd::PMXFile& pmx)
{
    const size_t boneCount = pmx.m_bones.size();
    const size_t morphCount = pmx.m_morphs.size();
    const size_t rigidCount = pmx.m_rigidbodies.size();
    const size_t textureCount = pmx.m_textures.size();

    const int64_t expectedFaceVertices =
        static_cast<int64_t>(pmx.m_faces.size()) * 3;
    int64_t materialFaceSum = 0;
    for (const auto& mat : pmx.m_materials)
    {
        materialFaceSum += mat.m_numFaceVertices;
        if (!IsValidTextureIndex(mat.m_textureIndex, textureCount))
            return false;
        if (!IsValidTextureIndex(mat.m_sphereTextureIndex, textureCount))
            return false;
        if (!IsValidMaterialToonIndex(mat, textureCount))
            return false;
    }
    if (materialFaceSum != expectedFaceVertices)
        return false;

    for (const auto& bone : pmx.m_bones)
    {
        if (!IsValidBoneIndex(bone.m_parentBoneIndex, boneCount))
            return false;

        if (BoneHasIkFlag(bone))
        {
            if (!IsValidBoneIndex(bone.m_ikTargetBoneIndex, boneCount) ||
                bone.m_ikTargetBoneIndex < 0)
                return false;
            for (const auto& link : bone.m_ikLinks)
            {
                if (!IsValidBoneIndex(link.m_ikBoneIndex, boneCount) ||
                    link.m_ikBoneIndex < 0)
                    return false;
            }
        }
    }

    for (const auto& rb : pmx.m_rigidbodies)
    {
        if (!IsValidBoneIndex(rb.m_boneIndex, boneCount))
            return false;
    }

    for (const auto& joint : pmx.m_joints)
    {
        if (!IsValidRigidIndex(joint.m_rigidbodyAIndex, rigidCount))
            return false;
        if (!IsValidRigidIndex(joint.m_rigidbodyBIndex, rigidCount))
            return false;
    }

    for (const auto& frame : pmx.m_displayFrames)
    {
        for (const auto& target : frame.m_targets)
        {
            if (target.m_type == libmmd::PMXDisplayFrame::TargetType::BoneIndex)
            {
                if (target.m_index < 0 ||
                    static_cast<size_t>(target.m_index) >= boneCount)
                    return false;
            }
            else if (target.m_type == libmmd::PMXDisplayFrame::TargetType::MorphIndex)
            {
                if (target.m_index < 0 ||
                    static_cast<size_t>(target.m_index) >= morphCount)
                    return false;
            }
        }
    }

    return true;
}

// ---------------------------------------------------------------------------
// Plugin-style export fixture (mesh + 2 materials + IK + physics + morph)
// ---------------------------------------------------------------------------

static libmmd::PMXFile MakePluginStyleExportPMX()
{
    libmmd::PMXFile pmx;

    auto& h = pmx.m_header;
    h.m_magic.Set("PMX ");
    h.m_version = 2.0f;
    h.m_dataSize = 8;
    h.m_encode = 1;
    h.m_addUVNum = 0;
    h.m_vertexIndexSize = 2;
    h.m_textureIndexSize = 2;
    h.m_materialIndexSize = 1;
    h.m_boneIndexSize = 2;
    h.m_morphIndexSize = 2;
    h.m_rigidbodyIndexSize = 2;

    pmx.m_info.m_modelName = "ExportFixture";
    pmx.m_info.m_englishModelName = "ExportFixtureEn";
    pmx.m_info.m_comment = "PMX export invariant fixture";
    pmx.m_info.m_englishComment = "PMX export invariant fixture en";

    for (int i = 0; i < 3; ++i)
    {
        libmmd::PMXVertex v{};
        v.m_position = Eigen::Vector3f(static_cast<float>(i), 0.0f, 0.0f);
        v.m_normal = Eigen::Vector3f(0.0f, 1.0f, 0.0f);
        v.m_uv = Eigen::Vector2f(0.0f, 0.0f);
        v.m_weightType = libmmd::PMXVertexWeight::BDEF1;
        v.m_boneIndices[0] = 0;
        v.m_edgeMag = 1.0f;
        pmx.m_vertices.push_back(std::move(v));
    }

    {
        libmmd::PMXFace f{};
        f.m_vertices[0] = 0;
        f.m_vertices[1] = 1;
        f.m_vertices[2] = 2;
        pmx.m_faces.push_back(std::move(f));
    }

    {
        libmmd::PMXTexture tex;
        tex.m_textureName = "shared.png";
        pmx.m_textures.push_back(std::move(tex));
    }

    auto makeMaterial = [](const char* name, int32_t faceVerts, int32_t texIndex) {
        libmmd::PMXMaterial mat{};
        mat.m_name = name;
        mat.m_englishName = name;
        mat.m_diffuse = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        mat.m_specular = Eigen::Vector3f::Zero();
        mat.m_specularPower = 1.0f;
        mat.m_ambient = Eigen::Vector3f(0.2f, 0.2f, 0.2f);
        mat.m_drawMode = static_cast<libmmd::PMXDrawModeFlags>(0);
        mat.m_edgeColor = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
        mat.m_edgeSize = 1.0f;
        mat.m_textureIndex = texIndex;
        mat.m_sphereTextureIndex = -1;
        mat.m_sphereMode = libmmd::PMXSphereMode::None;
        mat.m_toonMode = libmmd::PMXToonMode::Common;
        mat.m_toonTextureIndex = 0; // shared toon slot (PMX Common mode uses 0-9, not texture table index)
        mat.m_numFaceVertices = faceVerts;
        return mat;
    };

    pmx.m_materials.push_back(makeMaterial("mat_skin", 3, 0));
    pmx.m_materials.push_back(makeMaterial("mat_detail", 0, 0));

    {
        libmmd::PMXBone root{};
        root.m_name = "root";
        root.m_englishName = "root";
        root.m_position = Eigen::Vector3f::Zero();
        root.m_parentBoneIndex = -1;
        root.m_deformDepth = 0;
        root.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(
            static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowRotate) |
            static_cast<uint16_t>(libmmd::PMXBoneFlags::Visible));
        root.m_positionOffset = Eigen::Vector3f(0.0f, 5.0f, 0.0f);
        pmx.m_bones.push_back(std::move(root));
    }
    {
        libmmd::PMXBone child{};
        child.m_name = "child";
        child.m_englishName = "child";
        child.m_position = Eigen::Vector3f(0.0f, 10.0f, 0.0f);
        child.m_parentBoneIndex = 0;
        child.m_deformDepth = 1;
        child.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(
            static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowRotate) |
            static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowTranslate) |
            static_cast<uint16_t>(libmmd::PMXBoneFlags::Visible));
        child.m_positionOffset = Eigen::Vector3f(0.0f, 5.0f, 0.0f);
        pmx.m_bones.push_back(std::move(child));
    }
    {
        libmmd::PMXBone ikTarget{};
        ikTarget.m_name = "ik_target";
        ikTarget.m_englishName = "ik_target";
        ikTarget.m_position = Eigen::Vector3f(0.0f, 0.0f, -5.0f);
        ikTarget.m_parentBoneIndex = 0;
        ikTarget.m_deformDepth = 1;
        ikTarget.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(
            static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowRotate) |
            static_cast<uint16_t>(libmmd::PMXBoneFlags::Visible));
        pmx.m_bones.push_back(std::move(ikTarget));
    }
    {
        libmmd::PMXBone ikBone{};
        ikBone.m_name = "ik_leg";
        ikBone.m_englishName = "ik_leg";
        ikBone.m_position = Eigen::Vector3f(0.0f, 0.0f, -10.0f);
        ikBone.m_parentBoneIndex = 0;
        ikBone.m_deformDepth = 1;
        ikBone.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(
            static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowRotate) |
            static_cast<uint16_t>(libmmd::PMXBoneFlags::Visible) |
            static_cast<uint16_t>(libmmd::PMXBoneFlags::IK));
        ikBone.m_ikTargetBoneIndex = 2;
        ikBone.m_ikIterationCount = 10;
        ikBone.m_ikLimit = 2.0f;
        libmmd::PMXIKLink linkRoot{};
        linkRoot.m_ikBoneIndex = 0;
        linkRoot.m_enableLimit = 0;
        ikBone.m_ikLinks.push_back(linkRoot);
        libmmd::PMXIKLink linkChild{};
        linkChild.m_ikBoneIndex = 1;
        linkChild.m_enableLimit = 0;
        ikBone.m_ikLinks.push_back(linkChild);
        pmx.m_bones.push_back(std::move(ikBone));
    }

    {
        libmmd::PMXFileMorph morph{};
        morph.m_name = "morph_stub";
        morph.m_englishName = "morph_stub_en";
        morph.m_controlPanel = 4;
        morph.m_morphType = libmmd::PMXMorphType::Position;
        libmmd::PMXFileMorph::PositionMorph pm{};
        pm.m_vertexIndex = 0;
        pm.m_position = Eigen::Vector3f(0.0f, 1.0f, 0.0f);
        morph.m_positionMorph.push_back(pm);
        pmx.m_morphs.push_back(std::move(morph));
    }

    {
        libmmd::PMXDisplayFrame df{};
        df.m_name = "Body";
        df.m_englishName = "Body";
        df.m_flag = libmmd::PMXDisplayFrame::FrameType::SpecialFrame;
        libmmd::PMXDisplayFrame::Target boneTarget{};
        boneTarget.m_type = libmmd::PMXDisplayFrame::TargetType::BoneIndex;
        boneTarget.m_index = 0;
        df.m_targets.push_back(boneTarget);
        libmmd::PMXDisplayFrame::Target morphTarget{};
        morphTarget.m_type = libmmd::PMXDisplayFrame::TargetType::MorphIndex;
        morphTarget.m_index = 0;
        df.m_targets.push_back(morphTarget);
        pmx.m_displayFrames.push_back(std::move(df));
    }

    {
        libmmd::PMXRigidbody rb{};
        rb.m_name = "rb0";
        rb.m_englishName = "rb0_en";
        rb.m_boneIndex = 0;
        rb.m_group = 0;
        rb.m_collisionGroup = 0xFFFF;
        rb.m_shape = libmmd::PMXRigidbody::Shape::Sphere;
        rb.m_shapeSize = Eigen::Vector3f(1.0f, 0.0f, 0.0f);
        rb.m_translate = Eigen::Vector3f(0.0f, 5.0f, 0.0f);
        rb.m_rotate = Eigen::Vector3f::Zero();
        rb.m_mass = 1.0f;
        rb.m_translateDimmer = 0.5f;
        rb.m_rotateDimmer = 0.5f;
        rb.m_repulsion = 0.0f;
        rb.m_friction = 0.5f;
        rb.m_op = libmmd::PMXRigidbody::Operation::Static;
        pmx.m_rigidbodies.push_back(std::move(rb));
    }

    {
        libmmd::PMXJoint jt{};
        jt.m_name = "jt0";
        jt.m_englishName = "jt0_en";
        jt.m_type = libmmd::PMXJoint::JointType::SpringDOF6;
        jt.m_rigidbodyAIndex = 0;
        jt.m_rigidbodyBIndex = -1;
        jt.m_translate = Eigen::Vector3f::Zero();
        jt.m_rotate = Eigen::Vector3f::Zero();
        jt.m_translateLowerLimit = Eigen::Vector3f::Zero();
        jt.m_translateUpperLimit = Eigen::Vector3f::Zero();
        jt.m_rotateLowerLimit = Eigen::Vector3f::Zero();
        jt.m_rotateUpperLimit = Eigen::Vector3f::Zero();
        jt.m_springTranslateFactor = Eigen::Vector3f::Zero();
        jt.m_springRotateFactor = Eigen::Vector3f::Zero();
        pmx.m_joints.push_back(std::move(jt));
    }

    return pmx;
}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

static void test_PMX_Export_Invariants_Valid()
{
    std::cout << "[test] PMX_Export_Invariants_Valid\n";

    const auto pmx = MakePluginStyleExportPMX();
    TEST_ASSERT(ValidatePmxExportInvariants(pmx));

    std::cout << "    PMX export invariants (valid fixture) passed\n";
}

static void test_PMX_Export_RoundTrip()
{
    std::cout << "[test] PMX_Export_RoundTrip\n";

    const auto original = MakePluginStyleExportPMX();
    TEST_ASSERT(ValidatePmxExportInvariants(original));

    TempFile tmp(".pmx");
    TEST_ASSERT(libmmd::WritePMXFile(&original, tmp.pathStr().c_str()));

    libmmd::PMXFile loaded;
    TEST_ASSERT(libmmd::ReadPMXFile(&loaded, tmp.pathStr().c_str()));
    TEST_ASSERT(ValidatePmxExportInvariants(loaded));

    TEST_ASSERT_FLOAT_EQ(original.m_header.m_version, loaded.m_header.m_version);
    TEST_ASSERT_EQ(original.m_header.m_encode, loaded.m_header.m_encode);
    TEST_ASSERT_EQ(original.m_info.m_modelName, loaded.m_info.m_modelName);
    TEST_ASSERT_EQ(original.m_info.m_englishModelName, loaded.m_info.m_englishModelName);
    TEST_ASSERT_EQ(original.m_info.m_comment, loaded.m_info.m_comment);
    TEST_ASSERT_EQ(original.m_info.m_englishComment, loaded.m_info.m_englishComment);

    TEST_ASSERT_EQ(original.m_vertices.size(), loaded.m_vertices.size());
    TEST_ASSERT_EQ(original.m_faces.size(), loaded.m_faces.size());
    TEST_ASSERT_EQ(original.m_textures.size(), loaded.m_textures.size());
    TEST_ASSERT_EQ(original.m_materials.size(), loaded.m_materials.size());
    TEST_ASSERT_EQ(original.m_bones.size(), loaded.m_bones.size());
    TEST_ASSERT_EQ(original.m_morphs.size(), loaded.m_morphs.size());
    TEST_ASSERT_EQ(original.m_displayFrames.size(), loaded.m_displayFrames.size());
    TEST_ASSERT_EQ(original.m_rigidbodies.size(), loaded.m_rigidbodies.size());
    TEST_ASSERT_EQ(original.m_joints.size(), loaded.m_joints.size());

    int64_t loadedFaceSum = 0;
    for (const auto& mat : loaded.m_materials)
        loadedFaceSum += mat.m_numFaceVertices;
    TEST_ASSERT_EQ(static_cast<int64_t>(loaded.m_faces.size()) * 3, loadedFaceSum);

    std::cout << "    PMX export round-trip passed\n";
}

static void test_PMX_Export_Invariants_InvalidMaterialFaces()
{
    std::cout << "[test] PMX_Export_Invariants_InvalidMaterialFaces\n";

    auto pmx = MakePluginStyleExportPMX();
    TEST_ASSERT(ValidatePmxExportInvariants(pmx));

    pmx.m_materials[0].m_numFaceVertices = 1;
    TEST_ASSERT(!ValidatePmxExportInvariants(pmx));

    std::cout << "    PMX export invariants (invalid material faces) passed\n";
}

static void test_PMX_WriteFailure_InvalidPath()
{
    std::cout << "[test] PMX_WriteFailure_InvalidPath\n";

    const auto pmx = MakePluginStyleExportPMX();
    const char* badPath = "\\\\?\\Z:\\no_such_drive\\out.pmx";
    TEST_ASSERT(!libmmd::WritePMXFile(&pmx, badPath));

    std::cout << "    PMX write failure (invalid path) passed\n";
}

static void test_PMX_ReadFailure_MissingFile()
{
    std::cout << "[test] PMX_ReadFailure_MissingFile\n";

    libmmd::PMXFile pmx;
    const char* missingPath = "\\\\?\\Z:\\no_such_drive\\missing.pmx";
    TEST_ASSERT(!libmmd::ReadPMXFile(&pmx, missingPath));

    std::cout << "    PMX read failure (missing file) passed\n";
}

static void test_PMX_ReadFailure_InvalidFile()
{
    std::cout << "[test] PMX_ReadFailure_InvalidFile\n";

    TempFile tmp(".pmx");
    {
        std::ofstream out(tmp.pathStr(), std::ios::binary);
        TEST_ASSERT(out.good());
    }

    libmmd::PMXFile pmx;
    TEST_ASSERT(!libmmd::ReadPMXFile(&pmx, tmp.pathStr().c_str()));

    std::cout << "    PMX read failure (invalid file) passed\n";
}

// C4D plugin selection errors (no selection, non-MMD object, cancelled save dialog)
// are covered by manual verification; see docs/dev/export-flow.md (task 4.3).

// ===========================================================================
// main
// ===========================================================================

int main()
{
    std::cout << "=== libMMD PMX Export Invariant Tests ===\n\n";

    test_PMX_Export_Invariants_Valid();
    test_PMX_Export_RoundTrip();
    test_PMX_Export_Invariants_InvalidMaterialFaces();
    test_PMX_WriteFailure_InvalidPath();
    test_PMX_ReadFailure_MissingFile();
    test_PMX_ReadFailure_InvalidFile();

    std::cout << "\n=== Results: " << (g_totalTests - g_failedTests)
              << " / " << g_totalTests << " passed ===\n";

    if (g_failedTests > 0)
    {
        std::cerr << g_failedTests << " test(s) FAILED.\n";
        return EXIT_FAILURE;
    }

    std::cout << "All tests passed.\n";
    return EXIT_SUCCESS;
}
