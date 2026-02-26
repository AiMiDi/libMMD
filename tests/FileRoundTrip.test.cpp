#define _USE_MATH_DEFINES
#include <cmath>

#include <libMMD/Model/MMD/PMXFile.h>
#include <libMMD/Model/MMD/PMXModel.h>
#include <libMMD/Model/MMD/PMDFile.h>
#include <libMMD/Model/MMD/VMDFile.h>
#include <libMMD/Model/MMD/VMDAnimation.h>
#include <libMMD/Model/MMD/VPDFile.h>
#include <libMMD/Base/File.h>

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <vector>
#include <array>
#include <filesystem>

// ---------------------------------------------------------------------------
// Minimal test framework (same as MMDModel.test.cpp)
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

// ===========================================================================
// VMD round-trip test
// ===========================================================================

static libmmd::VMDFile MakeTestVMD()
{
    libmmd::VMDFile vmd;

    vmd.m_header.m_header.Set("Vocaloid Motion Data 0002");
    vmd.m_header.m_modelName.Set("TestModel");

    // Motion keys
    for (int i = 0; i < 3; i++)
    {
        libmmd::VMDMotion m;
        m.m_boneName.Set("bone");
        m.m_frame = i * 10;
        m.m_translate = Eigen::Vector3f(static_cast<float>(i), static_cast<float>(i) * 2.0f, static_cast<float>(i) * 3.0f);
        m.m_quaternion = Eigen::Quaternionf(1.0f, 0.0f, 0.0f, 0.0f);
        m.m_interpolation.fill(20);
        vmd.m_motions.push_back(std::move(m));
    }

    // Morph keys
    {
        libmmd::VMDMorph morph;
        morph.m_blendShapeName.Set("smile");
        morph.m_frame = 0;
        morph.m_weight = 0.5f;
        vmd.m_morphs.push_back(std::move(morph));
    }

    // Camera keys
    {
        libmmd::VMDCamera cam;
        cam.m_frame = 0;
        cam.m_distance = -45.0f;
        cam.m_interest = Eigen::Vector3f(0.0f, 10.0f, 0.0f);
        cam.m_rotate = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
        cam.m_interpolation.fill(20);
        cam.m_viewAngle = 30;
        cam.m_isPerspective = 1;
        vmd.m_cameras.push_back(std::move(cam));
    }

    // Light keys
    {
        libmmd::VMDLight light;
        light.m_frame = 0;
        light.m_color = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
        light.m_position = Eigen::Vector3f(-0.5f, -1.0f, 0.5f);
        vmd.m_lights.push_back(std::move(light));
    }

    // Shadow keys
    {
        libmmd::VMDShadow shadow;
        shadow.m_frame = 0;
        shadow.m_shadowType = 1;
        shadow.m_distance = 0.001f;
        vmd.m_shadows.push_back(std::move(shadow));
    }

    // IK keys
    {
        libmmd::VMDIk ik;
        ik.m_frame = 0;
        ik.m_show = 1;
        libmmd::VMDIkInfo info;
        info.m_name.Set("ik_leg");
        info.m_enable = 1;
        ik.m_ikInfos.push_back(std::move(info));
        vmd.m_iks.push_back(std::move(ik));
    }

    return vmd;
}

static void test_VMD_RoundTrip()
{
    std::cout << "[test] VMD_RoundTrip\n";

    auto original = MakeTestVMD();
    TempFile tmp(".vmd");

    TEST_ASSERT(libmmd::WriteVMDFile(&original, tmp.pathStr().c_str()));

    libmmd::VMDFile loaded;
    TEST_ASSERT(libmmd::ReadVMDFile(&loaded, tmp.pathStr().c_str()));

    // Header
    TEST_ASSERT_EQ(original.m_header.m_header.ToString(), loaded.m_header.m_header.ToString());
    TEST_ASSERT_EQ(original.m_header.m_modelName.ToString(), loaded.m_header.m_modelName.ToString());

    // Motions
    TEST_ASSERT_EQ(original.m_motions.size(), loaded.m_motions.size());
    for (size_t i = 0; i < original.m_motions.size(); i++)
    {
        const auto& a = original.m_motions[i];
        const auto& b = loaded.m_motions[i];
        TEST_ASSERT_EQ(a.m_boneName.ToString(), b.m_boneName.ToString());
        TEST_ASSERT_EQ(a.m_frame, b.m_frame);
        TEST_ASSERT_FLOAT_EQ(a.m_translate.x(), b.m_translate.x());
        TEST_ASSERT_FLOAT_EQ(a.m_translate.y(), b.m_translate.y());
        TEST_ASSERT_FLOAT_EQ(a.m_translate.z(), b.m_translate.z());
        TEST_ASSERT_FLOAT_EQ(a.m_quaternion.w(), b.m_quaternion.w());
        TEST_ASSERT_FLOAT_EQ(a.m_quaternion.x(), b.m_quaternion.x());
        TEST_ASSERT_FLOAT_EQ(a.m_quaternion.y(), b.m_quaternion.y());
        TEST_ASSERT_FLOAT_EQ(a.m_quaternion.z(), b.m_quaternion.z());
        TEST_ASSERT(a.m_interpolation == b.m_interpolation);
    }

    // Morphs
    TEST_ASSERT_EQ(original.m_morphs.size(), loaded.m_morphs.size());
    for (size_t i = 0; i < original.m_morphs.size(); i++)
    {
        TEST_ASSERT_EQ(original.m_morphs[i].m_blendShapeName.ToString(), loaded.m_morphs[i].m_blendShapeName.ToString());
        TEST_ASSERT_EQ(original.m_morphs[i].m_frame, loaded.m_morphs[i].m_frame);
        TEST_ASSERT_FLOAT_EQ(original.m_morphs[i].m_weight, loaded.m_morphs[i].m_weight);
    }

    // Cameras
    TEST_ASSERT_EQ(original.m_cameras.size(), loaded.m_cameras.size());
    for (size_t i = 0; i < original.m_cameras.size(); i++)
    {
        const auto& a = original.m_cameras[i];
        const auto& b = loaded.m_cameras[i];
        TEST_ASSERT_EQ(a.m_frame, b.m_frame);
        TEST_ASSERT_FLOAT_EQ(a.m_distance, b.m_distance);
        TEST_ASSERT_FLOAT_EQ(a.m_interest.x(), b.m_interest.x());
        TEST_ASSERT_FLOAT_EQ(a.m_interest.y(), b.m_interest.y());
        TEST_ASSERT_FLOAT_EQ(a.m_interest.z(), b.m_interest.z());
        TEST_ASSERT_FLOAT_EQ(a.m_rotate.x(), b.m_rotate.x());
        TEST_ASSERT_FLOAT_EQ(a.m_rotate.y(), b.m_rotate.y());
        TEST_ASSERT_FLOAT_EQ(a.m_rotate.z(), b.m_rotate.z());
        TEST_ASSERT_EQ(a.m_viewAngle, b.m_viewAngle);
        TEST_ASSERT_EQ(a.m_isPerspective, b.m_isPerspective);
        TEST_ASSERT(a.m_interpolation == b.m_interpolation);
    }

    // Lights
    TEST_ASSERT_EQ(original.m_lights.size(), loaded.m_lights.size());
    for (size_t i = 0; i < original.m_lights.size(); i++)
    {
        const auto& a = original.m_lights[i];
        const auto& b = loaded.m_lights[i];
        TEST_ASSERT_EQ(a.m_frame, b.m_frame);
        TEST_ASSERT_FLOAT_EQ(a.m_color.x(), b.m_color.x());
        TEST_ASSERT_FLOAT_EQ(a.m_color.y(), b.m_color.y());
        TEST_ASSERT_FLOAT_EQ(a.m_color.z(), b.m_color.z());
        TEST_ASSERT_FLOAT_EQ(a.m_position.x(), b.m_position.x());
        TEST_ASSERT_FLOAT_EQ(a.m_position.y(), b.m_position.y());
        TEST_ASSERT_FLOAT_EQ(a.m_position.z(), b.m_position.z());
    }

    // Shadows
    TEST_ASSERT_EQ(original.m_shadows.size(), loaded.m_shadows.size());
    for (size_t i = 0; i < original.m_shadows.size(); i++)
    {
        TEST_ASSERT_EQ(original.m_shadows[i].m_frame, loaded.m_shadows[i].m_frame);
        TEST_ASSERT_EQ(original.m_shadows[i].m_shadowType, loaded.m_shadows[i].m_shadowType);
        TEST_ASSERT_FLOAT_EQ(original.m_shadows[i].m_distance, loaded.m_shadows[i].m_distance);
    }

    // IK
    TEST_ASSERT_EQ(original.m_iks.size(), loaded.m_iks.size());
    for (size_t i = 0; i < original.m_iks.size(); i++)
    {
        const auto& a = original.m_iks[i];
        const auto& b = loaded.m_iks[i];
        TEST_ASSERT_EQ(a.m_frame, b.m_frame);
        TEST_ASSERT_EQ(a.m_show, b.m_show);
        TEST_ASSERT_EQ(a.m_ikInfos.size(), b.m_ikInfos.size());
        for (size_t j = 0; j < a.m_ikInfos.size(); j++)
        {
            TEST_ASSERT_EQ(a.m_ikInfos[j].m_name.ToString(), b.m_ikInfos[j].m_name.ToString());
            TEST_ASSERT_EQ(a.m_ikInfos[j].m_enable, b.m_ikInfos[j].m_enable);
        }
    }

    std::cout << "    VMD round-trip passed\n";
}

// ===========================================================================
// PMX round-trip test
// ===========================================================================

static libmmd::PMXFile MakeTestPMX()
{
    libmmd::PMXFile pmx;

    auto& h = pmx.m_header;
    h.m_magic.Set("PMX ");
    h.m_version = 2.0f;
    h.m_dataSize = 8;
    h.m_encode = 1; // UTF-8
    h.m_addUVNum = 0;
    h.m_vertexIndexSize = 2;
    h.m_textureIndexSize = 2;
    h.m_materialIndexSize = 1;
    h.m_boneIndexSize = 2;
    h.m_morphIndexSize = 2;
    h.m_rigidbodyIndexSize = 2;

    pmx.m_info.m_modelName = "TestModel";
    pmx.m_info.m_englishModelName = "TestModelEn";
    pmx.m_info.m_comment = "Test comment";
    pmx.m_info.m_englishComment = "Test comment en";

    // Vertices (3 for a triangle)
    for (int i = 0; i < 3; i++)
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

    // Face
    {
        libmmd::PMXFace f{};
        f.m_vertices[0] = 0;
        f.m_vertices[1] = 1;
        f.m_vertices[2] = 2;
        pmx.m_faces.push_back(std::move(f));
    }

    // Texture
    {
        libmmd::PMXTexture tex;
        tex.m_textureName = "tex.png";
        pmx.m_textures.push_back(std::move(tex));
    }

    // Material
    {
        libmmd::PMXMaterial mat{};
        mat.m_name = "mat0";
        mat.m_englishName = "mat0_en";
        mat.m_diffuse = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        mat.m_specular = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
        mat.m_specularPower = 1.0f;
        mat.m_ambient = Eigen::Vector3f(0.2f, 0.2f, 0.2f);
        mat.m_drawMode = static_cast<libmmd::PMXDrawModeFlags>(0);
        mat.m_edgeColor = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
        mat.m_edgeSize = 1.0f;
        mat.m_textureIndex = 0;
        mat.m_sphereTextureIndex = -1;
        mat.m_sphereMode = libmmd::PMXSphereMode::None;
        mat.m_toonMode = libmmd::PMXToonMode::Common;
        mat.m_toonTextureIndex = 0;
        mat.m_memo = "";
        mat.m_numFaceVertices = 3;
        pmx.m_materials.push_back(std::move(mat));
    }

    // Bones
    {
        libmmd::PMXBone bone{};
        bone.m_name = "root";
        bone.m_englishName = "root";
        bone.m_position = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
        bone.m_parentBoneIndex = -1;
        bone.m_deformDepth = 0;
        bone.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(
            static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowRotate) |
            static_cast<uint16_t>(libmmd::PMXBoneFlags::Visible));
        bone.m_positionOffset = Eigen::Vector3f(0.0f, 5.0f, 0.0f);
        pmx.m_bones.push_back(std::move(bone));
    }
    {
        libmmd::PMXBone bone{};
        bone.m_name = "child";
        bone.m_englishName = "child";
        bone.m_position = Eigen::Vector3f(0.0f, 10.0f, 0.0f);
        bone.m_parentBoneIndex = 0;
        bone.m_deformDepth = 1;
        bone.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(
            static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowRotate) |
            static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowTranslate) |
            static_cast<uint16_t>(libmmd::PMXBoneFlags::Visible));
        bone.m_positionOffset = Eigen::Vector3f(0.0f, 5.0f, 0.0f);
        pmx.m_bones.push_back(std::move(bone));
    }

    // Position morph
    {
        libmmd::PMXFileMorph morph{};
        morph.m_name = "morph0";
        morph.m_englishName = "morph0_en";
        morph.m_controlPanel = 4;
        morph.m_morphType = libmmd::PMXMorphType::Position;
        libmmd::PMXFileMorph::PositionMorph pm{};
        pm.m_vertexIndex = 0;
        pm.m_position = Eigen::Vector3f(0.0f, 1.0f, 0.0f);
        morph.m_positionMorph.push_back(pm);
        pmx.m_morphs.push_back(std::move(morph));
    }

    // Group morph
    {
        libmmd::PMXFileMorph morph{};
        morph.m_name = "group_morph";
        morph.m_englishName = "group_morph_en";
        morph.m_controlPanel = 4;
        morph.m_morphType = libmmd::PMXMorphType::Group;
        libmmd::PMXFileMorph::GroupMorph gm{};
        gm.m_morphIndex = 0;
        gm.m_weight = 1.0f;
        morph.m_groupMorph.push_back(gm);
        pmx.m_morphs.push_back(std::move(morph));
    }

    // Display frame
    {
        libmmd::PMXDisplayFrame df{};
        df.m_name = "Root";
        df.m_englishName = "Root";
        df.m_flag = libmmd::PMXDisplayFrame::FrameType::SpecialFrame;
        libmmd::PMXDisplayFrame::Target t{};
        t.m_type = libmmd::PMXDisplayFrame::TargetType::BoneIndex;
        t.m_index = 0;
        df.m_targets.push_back(t);
        pmx.m_displayFrames.push_back(std::move(df));
    }

    // Rigidbody
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
        rb.m_rotate = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
        rb.m_mass = 1.0f;
        rb.m_translateDimmer = 0.5f;
        rb.m_rotateDimmer = 0.5f;
        rb.m_repulsion = 0.0f;
        rb.m_friction = 0.5f;
        rb.m_op = libmmd::PMXRigidbody::Operation::Static;
        pmx.m_rigidbodies.push_back(std::move(rb));
    }

    // Joint
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

static void test_PMX_RoundTrip()
{
    std::cout << "[test] PMX_RoundTrip\n";

    auto original = MakeTestPMX();
    TempFile tmp(".pmx");

    TEST_ASSERT(libmmd::WritePMXFile(&original, tmp.pathStr().c_str()));

    libmmd::PMXFile loaded;
    TEST_ASSERT(libmmd::ReadPMXFile(&loaded, tmp.pathStr().c_str()));

    // Header
    TEST_ASSERT_FLOAT_EQ(original.m_header.m_version, loaded.m_header.m_version);
    TEST_ASSERT_EQ(original.m_header.m_encode, loaded.m_header.m_encode);
    TEST_ASSERT_EQ(original.m_header.m_addUVNum, loaded.m_header.m_addUVNum);
    TEST_ASSERT_EQ(original.m_header.m_vertexIndexSize, loaded.m_header.m_vertexIndexSize);
    TEST_ASSERT_EQ(original.m_header.m_boneIndexSize, loaded.m_header.m_boneIndexSize);

    // Info
    TEST_ASSERT_EQ(original.m_info.m_modelName, loaded.m_info.m_modelName);
    TEST_ASSERT_EQ(original.m_info.m_englishModelName, loaded.m_info.m_englishModelName);
    TEST_ASSERT_EQ(original.m_info.m_comment, loaded.m_info.m_comment);
    TEST_ASSERT_EQ(original.m_info.m_englishComment, loaded.m_info.m_englishComment);

    // Vertices
    TEST_ASSERT_EQ(original.m_vertices.size(), loaded.m_vertices.size());
    for (size_t i = 0; i < original.m_vertices.size(); i++)
    {
        const auto& a = original.m_vertices[i];
        const auto& b = loaded.m_vertices[i];
        TEST_ASSERT_FLOAT_EQ(a.m_position.x(), b.m_position.x());
        TEST_ASSERT_FLOAT_EQ(a.m_position.y(), b.m_position.y());
        TEST_ASSERT_FLOAT_EQ(a.m_position.z(), b.m_position.z());
        TEST_ASSERT_FLOAT_EQ(a.m_normal.x(), b.m_normal.x());
        TEST_ASSERT_FLOAT_EQ(a.m_normal.y(), b.m_normal.y());
        TEST_ASSERT_FLOAT_EQ(a.m_normal.z(), b.m_normal.z());
        TEST_ASSERT_FLOAT_EQ(a.m_uv.x(), b.m_uv.x());
        TEST_ASSERT_FLOAT_EQ(a.m_uv.y(), b.m_uv.y());
        TEST_ASSERT(a.m_weightType == b.m_weightType);
        TEST_ASSERT_EQ(a.m_boneIndices[0], b.m_boneIndices[0]);
        TEST_ASSERT_FLOAT_EQ(a.m_edgeMag, b.m_edgeMag);
    }

    // Faces
    TEST_ASSERT_EQ(original.m_faces.size(), loaded.m_faces.size());
    for (size_t i = 0; i < original.m_faces.size(); i++)
    {
        TEST_ASSERT_EQ(original.m_faces[i].m_vertices[0], loaded.m_faces[i].m_vertices[0]);
        TEST_ASSERT_EQ(original.m_faces[i].m_vertices[1], loaded.m_faces[i].m_vertices[1]);
        TEST_ASSERT_EQ(original.m_faces[i].m_vertices[2], loaded.m_faces[i].m_vertices[2]);
    }

    // Textures
    TEST_ASSERT_EQ(original.m_textures.size(), loaded.m_textures.size());
    for (size_t i = 0; i < original.m_textures.size(); i++)
    {
        TEST_ASSERT_EQ(original.m_textures[i].m_textureName, loaded.m_textures[i].m_textureName);
    }

    // Materials
    TEST_ASSERT_EQ(original.m_materials.size(), loaded.m_materials.size());
    for (size_t i = 0; i < original.m_materials.size(); i++)
    {
        const auto& a = original.m_materials[i];
        const auto& b = loaded.m_materials[i];
        TEST_ASSERT_EQ(a.m_name, b.m_name);
        TEST_ASSERT_EQ(a.m_englishName, b.m_englishName);
        TEST_ASSERT_FLOAT_EQ(a.m_diffuse.x(), b.m_diffuse.x());
        TEST_ASSERT_FLOAT_EQ(a.m_specularPower, b.m_specularPower);
        TEST_ASSERT_EQ(a.m_textureIndex, b.m_textureIndex);
        TEST_ASSERT_EQ(a.m_numFaceVertices, b.m_numFaceVertices);
    }

    // Bones
    TEST_ASSERT_EQ(original.m_bones.size(), loaded.m_bones.size());
    for (size_t i = 0; i < original.m_bones.size(); i++)
    {
        const auto& a = original.m_bones[i];
        const auto& b = loaded.m_bones[i];
        TEST_ASSERT_EQ(a.m_name, b.m_name);
        TEST_ASSERT_EQ(a.m_englishName, b.m_englishName);
        TEST_ASSERT_FLOAT_EQ(a.m_position.x(), b.m_position.x());
        TEST_ASSERT_FLOAT_EQ(a.m_position.y(), b.m_position.y());
        TEST_ASSERT_FLOAT_EQ(a.m_position.z(), b.m_position.z());
        TEST_ASSERT_EQ(a.m_parentBoneIndex, b.m_parentBoneIndex);
        TEST_ASSERT_EQ(a.m_deformDepth, b.m_deformDepth);
        TEST_ASSERT(static_cast<uint16_t>(a.m_boneFlag) == static_cast<uint16_t>(b.m_boneFlag));
    }

    // Morphs
    TEST_ASSERT_EQ(original.m_morphs.size(), loaded.m_morphs.size());
    for (size_t i = 0; i < original.m_morphs.size(); i++)
    {
        const auto& a = original.m_morphs[i];
        const auto& b = loaded.m_morphs[i];
        TEST_ASSERT_EQ(a.m_name, b.m_name);
        TEST_ASSERT_EQ(a.m_englishName, b.m_englishName);
        TEST_ASSERT_EQ(a.m_controlPanel, b.m_controlPanel);
        TEST_ASSERT(a.m_morphType == b.m_morphType);
        TEST_ASSERT_EQ(a.m_positionMorph.size(), b.m_positionMorph.size());
        TEST_ASSERT_EQ(a.m_groupMorph.size(), b.m_groupMorph.size());
    }

    // Display frames
    TEST_ASSERT_EQ(original.m_displayFrames.size(), loaded.m_displayFrames.size());

    // Rigidbodies
    TEST_ASSERT_EQ(original.m_rigidbodies.size(), loaded.m_rigidbodies.size());
    for (size_t i = 0; i < original.m_rigidbodies.size(); i++)
    {
        const auto& a = original.m_rigidbodies[i];
        const auto& b = loaded.m_rigidbodies[i];
        TEST_ASSERT_EQ(a.m_name, b.m_name);
        TEST_ASSERT_EQ(a.m_boneIndex, b.m_boneIndex);
        TEST_ASSERT_FLOAT_EQ(a.m_mass, b.m_mass);
    }

    // Joints
    TEST_ASSERT_EQ(original.m_joints.size(), loaded.m_joints.size());
    for (size_t i = 0; i < original.m_joints.size(); i++)
    {
        const auto& a = original.m_joints[i];
        const auto& b = loaded.m_joints[i];
        TEST_ASSERT_EQ(a.m_name, b.m_name);
        TEST_ASSERT_EQ(a.m_rigidbodyAIndex, b.m_rigidbodyAIndex);
    }

    // Softbodies (empty in this test)
    TEST_ASSERT_EQ(original.m_softbodies.size(), loaded.m_softbodies.size());

    std::cout << "    PMX round-trip passed\n";
}

// ===========================================================================
// PMD round-trip test
// ===========================================================================

static libmmd::PMDFile MakeTestPMD()
{
    libmmd::PMDFile pmd;

    pmd.m_header.m_magic.Set("Pmd");
    pmd.m_header.m_version = 1.0f;
    pmd.m_header.m_modelName.Set("TestPMD");
    pmd.m_header.m_comment.Set("Test comment");
    pmd.m_header.m_haveEnglishNameExt = 0;

    // Vertices
    for (int i = 0; i < 3; i++)
    {
        libmmd::PMDVertex v{};
        v.m_position = Eigen::Vector3f(static_cast<float>(i), 0.0f, 0.0f);
        v.m_normal = Eigen::Vector3f(0.0f, 1.0f, 0.0f);
        v.m_uv = Eigen::Vector2f(0.0f, 0.0f);
        v.m_bone[0] = 0;
        v.m_bone[1] = 0;
        v.m_boneWeight = 100;
        v.m_edge = 0;
        pmd.m_vertices.push_back(std::move(v));
    }

    // Face
    {
        libmmd::PMDFace f{};
        f.m_vertices[0] = 0;
        f.m_vertices[1] = 1;
        f.m_vertices[2] = 2;
        pmd.m_faces.push_back(std::move(f));
    }

    // Material
    {
        libmmd::PMDMaterial mat{};
        mat.m_diffuse = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
        mat.m_alpha = 1.0f;
        mat.m_specularPower = 1.0f;
        mat.m_specular = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
        mat.m_ambient = Eigen::Vector3f(0.2f, 0.2f, 0.2f);
        mat.m_toonIndex = 0;
        mat.m_edgeFlag = 0;
        mat.m_faceVertexCount = 3;
        mat.m_textureName.Set("tex.bmp");
        pmd.m_materials.push_back(std::move(mat));
    }

    // Bones
    {
        libmmd::PMDBone bone{};
        bone.m_boneName.Set("center");
        bone.m_parent = 0xFFFF;
        bone.m_tail = 1;
        bone.m_boneType = 0;
        bone.m_ikParent = 0;
        bone.m_position = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
        pmd.m_bones.push_back(std::move(bone));
    }
    {
        libmmd::PMDBone bone{};
        bone.m_boneName.Set("child");
        bone.m_parent = 0;
        bone.m_tail = 0xFFFF;
        bone.m_boneType = 0;
        bone.m_ikParent = 0;
        bone.m_position = Eigen::Vector3f(0.0f, 10.0f, 0.0f);
        pmd.m_bones.push_back(std::move(bone));
    }

    // Morphs (base + one actual)
    {
        libmmd::PMDMorph base{};
        base.m_morphName.Set("base");
        base.m_morphType = libmmd::PMDMorph::Base;
        libmmd::PMDMorph::Vertex v{};
        v.m_vertexIndex = 0;
        v.m_position = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
        base.m_vertices.push_back(v);
        pmd.m_morphs.push_back(std::move(base));
    }
    {
        libmmd::PMDMorph morph{};
        morph.m_morphName.Set("smile");
        morph.m_morphType = libmmd::PMDMorph::Other;
        libmmd::PMDMorph::Vertex v{};
        v.m_vertexIndex = 0;
        v.m_position = Eigen::Vector3f(0.0f, 1.0f, 0.0f);
        morph.m_vertices.push_back(v);
        pmd.m_morphs.push_back(std::move(morph));
    }

    // Morph display list
    pmd.m_morphDisplayList.m_displayList.push_back(1);

    // Bone display lists (first is always center frame)
    {
        libmmd::PMDBoneDisplayList center{};
        center.m_displayList.push_back(0);
        pmd.m_boneDisplayLists.push_back(std::move(center));
    }
    {
        libmmd::PMDBoneDisplayList frame{};
        frame.m_name.Set("Body");
        frame.m_displayList.push_back(1);
        pmd.m_boneDisplayLists.push_back(std::move(frame));
    }

    // Toon textures
    for (int i = 0; i < 10; i++)
    {
        char buf[16];
        std::snprintf(buf, sizeof(buf), "toon%02d.bmp", i + 1);
        pmd.m_toonTextureNames[i].Set(buf);
    }

    return pmd;
}

static void test_PMD_RoundTrip()
{
    std::cout << "[test] PMD_RoundTrip\n";

    auto original = MakeTestPMD();
    TempFile tmp(".pmd");

    TEST_ASSERT(libmmd::WritePMDFile(&original, tmp.pathStr().c_str()));

    libmmd::PMDFile loaded;
    TEST_ASSERT(libmmd::ReadPMDFile(&loaded, tmp.pathStr().c_str()));

    // Header
    TEST_ASSERT_EQ(original.m_header.m_magic.ToString(), loaded.m_header.m_magic.ToString());
    TEST_ASSERT_FLOAT_EQ(original.m_header.m_version, loaded.m_header.m_version);
    TEST_ASSERT_EQ(original.m_header.m_modelName.ToString(), loaded.m_header.m_modelName.ToString());

    // Vertices
    TEST_ASSERT_EQ(original.m_vertices.size(), loaded.m_vertices.size());
    for (size_t i = 0; i < original.m_vertices.size(); i++)
    {
        const auto& a = original.m_vertices[i];
        const auto& b = loaded.m_vertices[i];
        TEST_ASSERT_FLOAT_EQ(a.m_position.x(), b.m_position.x());
        TEST_ASSERT_FLOAT_EQ(a.m_position.y(), b.m_position.y());
        TEST_ASSERT_FLOAT_EQ(a.m_position.z(), b.m_position.z());
        TEST_ASSERT_EQ(a.m_bone[0], b.m_bone[0]);
        TEST_ASSERT_EQ(a.m_bone[1], b.m_bone[1]);
        TEST_ASSERT_EQ(a.m_boneWeight, b.m_boneWeight);
        TEST_ASSERT_EQ(a.m_edge, b.m_edge);
    }

    // Faces
    TEST_ASSERT_EQ(original.m_faces.size(), loaded.m_faces.size());
    for (size_t i = 0; i < original.m_faces.size(); i++)
    {
        TEST_ASSERT_EQ(original.m_faces[i].m_vertices[0], loaded.m_faces[i].m_vertices[0]);
        TEST_ASSERT_EQ(original.m_faces[i].m_vertices[1], loaded.m_faces[i].m_vertices[1]);
        TEST_ASSERT_EQ(original.m_faces[i].m_vertices[2], loaded.m_faces[i].m_vertices[2]);
    }

    // Materials
    TEST_ASSERT_EQ(original.m_materials.size(), loaded.m_materials.size());
    for (size_t i = 0; i < original.m_materials.size(); i++)
    {
        const auto& a = original.m_materials[i];
        const auto& b = loaded.m_materials[i];
        TEST_ASSERT_FLOAT_EQ(a.m_diffuse.x(), b.m_diffuse.x());
        TEST_ASSERT_FLOAT_EQ(a.m_alpha, b.m_alpha);
        TEST_ASSERT_EQ(a.m_faceVertexCount, b.m_faceVertexCount);
        TEST_ASSERT_EQ(a.m_textureName.ToString(), b.m_textureName.ToString());
    }

    // Bones
    TEST_ASSERT_EQ(original.m_bones.size(), loaded.m_bones.size());
    for (size_t i = 0; i < original.m_bones.size(); i++)
    {
        const auto& a = original.m_bones[i];
        const auto& b = loaded.m_bones[i];
        TEST_ASSERT_EQ(a.m_boneName.ToString(), b.m_boneName.ToString());
        TEST_ASSERT_EQ(a.m_parent, b.m_parent);
        TEST_ASSERT_EQ(a.m_tail, b.m_tail);
        TEST_ASSERT_FLOAT_EQ(a.m_position.x(), b.m_position.x());
        TEST_ASSERT_FLOAT_EQ(a.m_position.y(), b.m_position.y());
        TEST_ASSERT_FLOAT_EQ(a.m_position.z(), b.m_position.z());
    }

    // Morphs
    TEST_ASSERT_EQ(original.m_morphs.size(), loaded.m_morphs.size());
    for (size_t i = 0; i < original.m_morphs.size(); i++)
    {
        const auto& a = original.m_morphs[i];
        const auto& b = loaded.m_morphs[i];
        TEST_ASSERT_EQ(a.m_morphName.ToString(), b.m_morphName.ToString());
        TEST_ASSERT(a.m_morphType == b.m_morphType);
        TEST_ASSERT_EQ(a.m_vertices.size(), b.m_vertices.size());
    }

    // Bone display lists
    TEST_ASSERT_EQ(original.m_boneDisplayLists.size(), loaded.m_boneDisplayLists.size());
    for (size_t i = 0; i < original.m_boneDisplayLists.size(); i++)
    {
        TEST_ASSERT_EQ(original.m_boneDisplayLists[i].m_displayList.size(),
                        loaded.m_boneDisplayLists[i].m_displayList.size());
        for (size_t j = 0; j < original.m_boneDisplayLists[i].m_displayList.size(); j++)
        {
            TEST_ASSERT_EQ(original.m_boneDisplayLists[i].m_displayList[j],
                            loaded.m_boneDisplayLists[i].m_displayList[j]);
        }
    }

    // Toon textures
    for (int i = 0; i < 10; i++)
    {
        TEST_ASSERT_EQ(original.m_toonTextureNames[i].ToString(), loaded.m_toonTextureNames[i].ToString());
    }

    std::cout << "    PMD round-trip passed\n";
}

// ===========================================================================
// VPD round-trip test
// ===========================================================================

static libmmd::VPDFile MakeTestVPD()
{
    libmmd::VPDFile vpd;

    // Use ASCII bone names to ensure SJIS round-trip works cleanly
    {
        libmmd::VPDBone bone;
        bone.m_boneName = "center";
        bone.m_translate = Eigen::Vector3f(1.0f, 2.0f, 3.0f);
        bone.m_quaternion = Eigen::Quaternionf(1.0f, 0.0f, 0.0f, 0.0f);
        vpd.m_bones.push_back(std::move(bone));
    }
    {
        libmmd::VPDBone bone;
        bone.m_boneName = "arm_R";
        bone.m_translate = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
        bone.m_quaternion = Eigen::Quaternionf(0.707f, 0.707f, 0.0f, 0.0f);
        vpd.m_bones.push_back(std::move(bone));
    }

    {
        libmmd::VPDMorph morph;
        morph.m_morphName = "smile";
        morph.m_weight = 0.5f;
        vpd.m_morphs.push_back(std::move(morph));
    }

    return vpd;
}

static void test_VPD_RoundTrip()
{
    std::cout << "[test] VPD_RoundTrip\n";

    auto original = MakeTestVPD();
    TempFile tmp(".vpd");

    TEST_ASSERT(libmmd::WriteVPDFile(&original, tmp.pathStr().c_str()));

    libmmd::VPDFile loaded;
    TEST_ASSERT(libmmd::ReadVPDFile(&loaded, tmp.pathStr().c_str()));

    // Bones
    TEST_ASSERT_EQ(original.m_bones.size(), loaded.m_bones.size());
    for (size_t i = 0; i < original.m_bones.size(); i++)
    {
        const auto& a = original.m_bones[i];
        const auto& b = loaded.m_bones[i];
        TEST_ASSERT_EQ(a.m_boneName, b.m_boneName);
        TEST_ASSERT(std::fabs(a.m_translate.x() - b.m_translate.x()) < 1e-4f);
        TEST_ASSERT(std::fabs(a.m_translate.y() - b.m_translate.y()) < 1e-4f);
        TEST_ASSERT(std::fabs(a.m_translate.z() - b.m_translate.z()) < 1e-4f);
        TEST_ASSERT(std::fabs(a.m_quaternion.w() - b.m_quaternion.w()) < 1e-3f);
        TEST_ASSERT(std::fabs(a.m_quaternion.x() - b.m_quaternion.x()) < 1e-3f);
        TEST_ASSERT(std::fabs(a.m_quaternion.y() - b.m_quaternion.y()) < 1e-3f);
        TEST_ASSERT(std::fabs(a.m_quaternion.z() - b.m_quaternion.z()) < 1e-3f);
    }

    // Morphs
    TEST_ASSERT_EQ(original.m_morphs.size(), loaded.m_morphs.size());
    for (size_t i = 0; i < original.m_morphs.size(); i++)
    {
        const auto& a = original.m_morphs[i];
        const auto& b = loaded.m_morphs[i];
        TEST_ASSERT_EQ(a.m_morphName, b.m_morphName);
        TEST_ASSERT(std::fabs(a.m_weight - b.m_weight) < 1e-4f);
    }

    std::cout << "    VPD round-trip passed\n";
}

// ===========================================================================
// PMX round-trip with UTF-16 encoding
// ===========================================================================

static void test_PMX_RoundTrip_UTF16()
{
    std::cout << "[test] PMX_RoundTrip_UTF16\n";

    auto original = MakeTestPMX();
    original.m_header.m_encode = 0; // UTF-16

    TempFile tmp(".pmx");

    TEST_ASSERT(libmmd::WritePMXFile(&original, tmp.pathStr().c_str()));

    libmmd::PMXFile loaded;
    TEST_ASSERT(libmmd::ReadPMXFile(&loaded, tmp.pathStr().c_str()));

    TEST_ASSERT_EQ(original.m_info.m_modelName, loaded.m_info.m_modelName);
    TEST_ASSERT_EQ(original.m_info.m_englishModelName, loaded.m_info.m_englishModelName);
    TEST_ASSERT_EQ(original.m_info.m_comment, loaded.m_info.m_comment);
    TEST_ASSERT_EQ(original.m_vertices.size(), loaded.m_vertices.size());
    TEST_ASSERT_EQ(original.m_bones.size(), loaded.m_bones.size());
    for (size_t i = 0; i < original.m_bones.size(); i++)
    {
        TEST_ASSERT_EQ(original.m_bones[i].m_name, loaded.m_bones[i].m_name);
    }
    TEST_ASSERT_EQ(original.m_morphs.size(), loaded.m_morphs.size());
    for (size_t i = 0; i < original.m_morphs.size(); i++)
    {
        TEST_ASSERT_EQ(original.m_morphs[i].m_name, loaded.m_morphs[i].m_name);
        TEST_ASSERT(original.m_morphs[i].m_morphType == loaded.m_morphs[i].m_morphType);
    }

    std::cout << "    PMX UTF-16 round-trip passed\n";
}

// ===========================================================================
// VMD empty sections round-trip
// ===========================================================================

static void test_VMD_EmptySections_RoundTrip()
{
    std::cout << "[test] VMD_EmptySections_RoundTrip\n";

    libmmd::VMDFile original;
    original.m_header.m_header.Set("Vocaloid Motion Data 0002");
    original.m_header.m_modelName.Set("Empty");

    TempFile tmp(".vmd");

    TEST_ASSERT(libmmd::WriteVMDFile(&original, tmp.pathStr().c_str()));

    libmmd::VMDFile loaded;
    TEST_ASSERT(libmmd::ReadVMDFile(&loaded, tmp.pathStr().c_str()));

    TEST_ASSERT_EQ(size_t(0), loaded.m_motions.size());
    TEST_ASSERT_EQ(size_t(0), loaded.m_morphs.size());
    TEST_ASSERT_EQ(size_t(0), loaded.m_cameras.size());
    TEST_ASSERT_EQ(size_t(0), loaded.m_lights.size());
    TEST_ASSERT_EQ(size_t(0), loaded.m_shadows.size());
    TEST_ASSERT_EQ(size_t(0), loaded.m_iks.size());

    std::cout << "    VMD empty sections round-trip passed\n";
}

// ===========================================================================
// PMX morph types round-trip (test all morph type variants)
// ===========================================================================

static void test_PMX_MorphTypes_RoundTrip()
{
    std::cout << "[test] PMX_MorphTypes_RoundTrip\n";

    auto pmx = MakeTestPMX();
    pmx.m_morphs.clear();

    // UV morph
    {
        libmmd::PMXFileMorph morph{};
        morph.m_name = "uv_morph";
        morph.m_englishName = "uv_morph_en";
        morph.m_controlPanel = 4;
        morph.m_morphType = libmmd::PMXMorphType::UV;
        libmmd::PMXFileMorph::UVMorph um{};
        um.m_vertexIndex = 0;
        um.m_uv = Eigen::Vector4f(0.1f, 0.2f, 0.0f, 0.0f);
        morph.m_uvMorph.push_back(um);
        pmx.m_morphs.push_back(std::move(morph));
    }

    // Bone morph
    {
        libmmd::PMXFileMorph morph{};
        morph.m_name = "bone_morph";
        morph.m_englishName = "bone_morph_en";
        morph.m_controlPanel = 4;
        morph.m_morphType = libmmd::PMXMorphType::Bone;
        libmmd::PMXFileMorph::BoneMorph bm{};
        bm.m_boneIndex = 0;
        bm.m_position = Eigen::Vector3f(1.0f, 0.0f, 0.0f);
        bm.m_quaternion = Eigen::Quaternionf::Identity();
        morph.m_boneMorph.push_back(bm);
        pmx.m_morphs.push_back(std::move(morph));
    }

    // Material morph
    {
        libmmd::PMXFileMorph morph{};
        morph.m_name = "mat_morph";
        morph.m_englishName = "mat_morph_en";
        morph.m_controlPanel = 4;
        morph.m_morphType = libmmd::PMXMorphType::Material;
        libmmd::PMXFileMorph::MaterialMorph mm{};
        mm.m_materialIndex = 0;
        mm.m_opType = libmmd::PMXFileMorph::MaterialMorph::OpType::Mul;
        mm.m_diffuse = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        mm.m_specular = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
        mm.m_specularPower = 1.0f;
        mm.m_ambient = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
        mm.m_edgeColor = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        mm.m_edgeSize = 1.0f;
        mm.m_textureFactor = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        mm.m_sphereTextureFactor = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        mm.m_toonTextureFactor = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        morph.m_materialMorph.push_back(mm);
        pmx.m_morphs.push_back(std::move(morph));
    }

    TempFile tmp(".pmx");
    TEST_ASSERT(libmmd::WritePMXFile(&pmx, tmp.pathStr().c_str()));

    libmmd::PMXFile loaded;
    TEST_ASSERT(libmmd::ReadPMXFile(&loaded, tmp.pathStr().c_str()));

    TEST_ASSERT_EQ(pmx.m_morphs.size(), loaded.m_morphs.size());

    // UV morph check
    TEST_ASSERT(loaded.m_morphs[0].m_morphType == libmmd::PMXMorphType::UV);
    TEST_ASSERT_EQ(size_t(1), loaded.m_morphs[0].m_uvMorph.size());
    TEST_ASSERT_FLOAT_EQ(0.1f, loaded.m_morphs[0].m_uvMorph[0].m_uv.x());

    // Bone morph check
    TEST_ASSERT(loaded.m_morphs[1].m_morphType == libmmd::PMXMorphType::Bone);
    TEST_ASSERT_EQ(size_t(1), loaded.m_morphs[1].m_boneMorph.size());
    TEST_ASSERT_FLOAT_EQ(1.0f, loaded.m_morphs[1].m_boneMorph[0].m_position.x());

    // Material morph check
    TEST_ASSERT(loaded.m_morphs[2].m_morphType == libmmd::PMXMorphType::Material);
    TEST_ASSERT_EQ(size_t(1), loaded.m_morphs[2].m_materialMorph.size());
    TEST_ASSERT_FLOAT_EQ(1.0f, loaded.m_morphs[2].m_materialMorph[0].m_edgeSize);

    std::cout << "    PMX morph types round-trip passed\n";
}

// ===========================================================================
// VMDAnimation Add->Save round-trip helpers
// ===========================================================================

static libmmd::PMXFile MakeSimplePMXFileForAnim(float boneLength = 10.0f)
{
    libmmd::PMXFile file{};

    libmmd::PMXBone root{};
    root.m_name = "root";
    root.m_englishName = "root";
    root.m_position = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    root.m_parentBoneIndex = -1;
    root.m_deformDepth = 0;
    root.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(
        static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowRotate) |
        static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowTranslate) |
        static_cast<uint16_t>(libmmd::PMXBoneFlags::Visible)
    );
    root.m_appendBoneIndex = -1;
    root.m_appendWeight = 0.0f;

    libmmd::PMXBone child{};
    child.m_name = "child";
    child.m_englishName = "child";
    child.m_position = Eigen::Vector3f(0.0f, boneLength, 0.0f);
    child.m_parentBoneIndex = 0;
    child.m_deformDepth = 0;
    child.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(
        static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowRotate) |
        static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowTranslate) |
        static_cast<uint16_t>(libmmd::PMXBoneFlags::Visible)
    );
    child.m_appendBoneIndex = -1;
    child.m_appendWeight = 0.0f;

    file.m_bones.push_back(std::move(root));
    file.m_bones.push_back(std::move(child));

    libmmd::PMXMaterial mat{};
    mat.m_numFaceVertices = 0;
    mat.m_textureIndex = -1;
    mat.m_sphereTextureIndex = -1;
    mat.m_toonTextureIndex = -1;
    mat.m_sphereMode = libmmd::PMXSphereMode::None;
    mat.m_toonMode = libmmd::PMXToonMode::Common;
    file.m_materials.push_back(std::move(mat));

    return file;
}

static libmmd::PMXFile MakePMXFileWithMorphForAnim()
{
    auto file = MakeSimplePMXFileForAnim();

    libmmd::PMXFileMorph morph{};
    morph.m_name = "smile";
    morph.m_englishName = "smile";
    morph.m_controlPanel = 4;
    morph.m_morphType = libmmd::PMXMorphType::Position;
    file.m_morphs.push_back(std::move(morph));

    return file;
}

static libmmd::PMXFile MakePMXFileWithIKForAnim()
{
    libmmd::PMXFile file{};

    // Bone 0: root
    libmmd::PMXBone root{};
    root.m_name = "root";
    root.m_englishName = "root";
    root.m_position = Eigen::Vector3f::Zero();
    root.m_parentBoneIndex = -1;
    root.m_deformDepth = 0;
    root.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(
        static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowRotate) |
        static_cast<uint16_t>(libmmd::PMXBoneFlags::Visible)
    );
    root.m_appendBoneIndex = -1;
    file.m_bones.push_back(std::move(root));

    // Bone 1: ik_target (the bone that IK tries to reach)
    libmmd::PMXBone target{};
    target.m_name = "ik_target";
    target.m_englishName = "ik_target";
    target.m_position = Eigen::Vector3f(0.0f, 0.0f, -5.0f);
    target.m_parentBoneIndex = 0;
    target.m_deformDepth = 0;
    target.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(
        static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowRotate) |
        static_cast<uint16_t>(libmmd::PMXBoneFlags::Visible)
    );
    target.m_appendBoneIndex = -1;
    file.m_bones.push_back(std::move(target));

    // Bone 2: ik_bone (the IK solver bone)
    libmmd::PMXBone ikBone{};
    ikBone.m_name = "ik_leg";
    ikBone.m_englishName = "ik_leg";
    ikBone.m_position = Eigen::Vector3f(0.0f, 0.0f, -10.0f);
    ikBone.m_parentBoneIndex = 0;
    ikBone.m_deformDepth = 0;
    ikBone.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(
        static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowRotate) |
        static_cast<uint16_t>(libmmd::PMXBoneFlags::Visible) |
        static_cast<uint16_t>(libmmd::PMXBoneFlags::IK)
    );
    ikBone.m_appendBoneIndex = -1;
    ikBone.m_ikTargetBoneIndex = 1;
    ikBone.m_ikIterationCount = 10;
    ikBone.m_ikLimit = 2.0f;
    libmmd::PMXIKLink link{};
    link.m_ikBoneIndex = 0;
    link.m_enableLimit = 0;
    ikBone.m_ikLinks.push_back(link);
    file.m_bones.push_back(std::move(ikBone));

    libmmd::PMXMaterial mat{};
    mat.m_numFaceVertices = 0;
    mat.m_textureIndex = -1;
    mat.m_sphereTextureIndex = -1;
    mat.m_toonTextureIndex = -1;
    mat.m_sphereMode = libmmd::PMXSphereMode::None;
    mat.m_toonMode = libmmd::PMXToonMode::Common;
    file.m_materials.push_back(std::move(mat));

    return file;
}

// ===========================================================================
// VMDAnimation Add->Save round-trip tests
// ===========================================================================

static void test_VMDAnimation_AddSave_MotionRoundTrip()
{
    std::cout << "[test] VMDAnimation_AddSave_MotionRoundTrip\n";

    auto pmxFile = MakeSimplePMXFileForAnim();
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    // Build input VMD with multiple bones and frames
    libmmd::VMDFile vmdIn;
    vmdIn.m_header.m_header.Set("Vocaloid Motion Data 0002");
    vmdIn.m_header.m_modelName.Set("TestModel");

    auto makeMotion = [](const char* name, uint32_t frame,
        const Eigen::Vector3f& t, const Eigen::Quaternionf& q,
        uint8_t interpVal) -> libmmd::VMDMotion
    {
        libmmd::VMDMotion m;
        m.m_boneName.Set(name);
        m.m_frame = frame;
        m.m_translate = t;
        m.m_quaternion = q;
        m.m_interpolation.fill(0);
        for (int ch = 0; ch < 4; ++ch)
        {
            m.m_interpolation[ch + 0]  = interpVal;
            m.m_interpolation[ch + 4]  = interpVal;
            m.m_interpolation[ch + 8]  = static_cast<uint8_t>(127 - interpVal);
            m.m_interpolation[ch + 12] = static_cast<uint8_t>(127 - interpVal);
        }
        return m;
    };

    vmdIn.m_motions.push_back(makeMotion("root", 0,
        Eigen::Vector3f(1.0f, 2.0f, 3.0f), Eigen::Quaternionf::Identity(), 20));
    vmdIn.m_motions.push_back(makeMotion("root", 30,
        Eigen::Vector3f(4.0f, 5.0f, 6.0f), Eigen::Quaternionf::Identity(), 20));
    vmdIn.m_motions.push_back(makeMotion("child", 0,
        Eigen::Vector3f::Zero(),
        Eigen::Quaternionf(Eigen::AngleAxisf(0.5f, Eigen::Vector3f::UnitZ())), 40));
    vmdIn.m_motions.push_back(makeMotion("child", 15,
        Eigen::Vector3f(7.0f, 8.0f, 9.0f), Eigen::Quaternionf::Identity(), 40));

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmdIn));

    libmmd::VMDFile vmdOut;
    TEST_ASSERT(anim.Save(vmdOut));

    TEST_ASSERT_EQ(vmdIn.m_motions.size(), vmdOut.m_motions.size());

    // Build lookup: (boneName, frame) -> motion for both input and output
    auto findMotion = [](const std::vector<libmmd::VMDMotion>& motions,
        const std::string& name, uint32_t frame) -> const libmmd::VMDMotion*
    {
        for (const auto& m : motions)
            if (m.m_boneName.ToString() == name && m.m_frame == frame)
                return &m;
        return nullptr;
    };

    for (const auto& orig : vmdIn.m_motions)
    {
        const auto* saved = findMotion(vmdOut.m_motions,
            orig.m_boneName.ToString(), orig.m_frame);
        TEST_ASSERT(saved != nullptr);
        if (!saved) continue;

        TEST_ASSERT_FLOAT_EQ(orig.m_translate.x(), saved->m_translate.x());
        TEST_ASSERT_FLOAT_EQ(orig.m_translate.y(), saved->m_translate.y());
        TEST_ASSERT_FLOAT_EQ(orig.m_translate.z(), saved->m_translate.z());
        TEST_ASSERT(std::fabs(orig.m_quaternion.w() - saved->m_quaternion.w()) < 1e-4f);
        TEST_ASSERT(std::fabs(orig.m_quaternion.x() - saved->m_quaternion.x()) < 1e-4f);
        TEST_ASSERT(std::fabs(orig.m_quaternion.y() - saved->m_quaternion.y()) < 1e-4f);
        TEST_ASSERT(std::fabs(orig.m_quaternion.z() - saved->m_quaternion.z()) < 1e-4f);

        // Check interpolation control points (allow ±1 for float->uint8 precision)
        for (int i = 0; i < 16; ++i)
        {
            TEST_ASSERT(std::abs(static_cast<int>(orig.m_interpolation[i])
                - static_cast<int>(saved->m_interpolation[i])) <= 1);
        }
    }

    std::cout << "    VMDAnimation motion Add->Save round-trip passed\n";
}

static void test_VMDAnimation_AddSave_MorphRoundTrip()
{
    std::cout << "[test] VMDAnimation_AddSave_MorphRoundTrip\n";

    auto pmxFile = MakePMXFileWithMorphForAnim();
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    libmmd::VMDFile vmdIn;
    vmdIn.m_header.m_header.Set("Vocaloid Motion Data 0002");

    {
        libmmd::VMDMorph m;
        m.m_blendShapeName.Set("smile");
        m.m_frame = 0;
        m.m_weight = 0.0f;
        vmdIn.m_morphs.push_back(std::move(m));
    }
    {
        libmmd::VMDMorph m;
        m.m_blendShapeName.Set("smile");
        m.m_frame = 15;
        m.m_weight = 0.75f;
        vmdIn.m_morphs.push_back(std::move(m));
    }
    {
        libmmd::VMDMorph m;
        m.m_blendShapeName.Set("smile");
        m.m_frame = 30;
        m.m_weight = 1.0f;
        vmdIn.m_morphs.push_back(std::move(m));
    }

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmdIn));

    libmmd::VMDFile vmdOut;
    TEST_ASSERT(anim.Save(vmdOut));

    TEST_ASSERT_EQ(vmdIn.m_morphs.size(), vmdOut.m_morphs.size());

    for (size_t i = 0; i < vmdIn.m_morphs.size(); ++i)
    {
        TEST_ASSERT_EQ(vmdIn.m_morphs[i].m_blendShapeName.ToString(),
                        vmdOut.m_morphs[i].m_blendShapeName.ToString());
        TEST_ASSERT_EQ(vmdIn.m_morphs[i].m_frame, vmdOut.m_morphs[i].m_frame);
        TEST_ASSERT_FLOAT_EQ(vmdIn.m_morphs[i].m_weight, vmdOut.m_morphs[i].m_weight);
    }

    std::cout << "    VMDAnimation morph Add->Save round-trip passed\n";
}

static void test_VMDAnimation_AddSave_IKRoundTrip()
{
    std::cout << "[test] VMDAnimation_AddSave_IKRoundTrip\n";

    auto pmxFile = MakePMXFileWithIKForAnim();
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));
    TEST_ASSERT(model->GetIKManager()->GetIKSolverCount() > 0);

    libmmd::VMDFile vmdIn;
    vmdIn.m_header.m_header.Set("Vocaloid Motion Data 0002");

    {
        libmmd::VMDIk ik;
        ik.m_frame = 0;
        ik.m_show = 1;
        libmmd::VMDIkInfo info;
        info.m_name.Set("ik_leg");
        info.m_enable = 1;
        ik.m_ikInfos.push_back(std::move(info));
        vmdIn.m_iks.push_back(std::move(ik));
    }
    {
        libmmd::VMDIk ik;
        ik.m_frame = 20;
        ik.m_show = 1;
        libmmd::VMDIkInfo info;
        info.m_name.Set("ik_leg");
        info.m_enable = 0;
        ik.m_ikInfos.push_back(std::move(info));
        vmdIn.m_iks.push_back(std::move(ik));
    }

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmdIn));

    libmmd::VMDFile vmdOut;
    TEST_ASSERT(anim.Save(vmdOut));

    TEST_ASSERT_EQ(vmdIn.m_iks.size(), vmdOut.m_iks.size());

    for (size_t i = 0; i < vmdIn.m_iks.size(); ++i)
    {
        TEST_ASSERT_EQ(vmdIn.m_iks[i].m_frame, vmdOut.m_iks[i].m_frame);
        TEST_ASSERT_EQ(vmdIn.m_iks[i].m_ikInfos.size(), vmdOut.m_iks[i].m_ikInfos.size());
        for (size_t j = 0; j < vmdIn.m_iks[i].m_ikInfos.size(); ++j)
        {
            TEST_ASSERT_EQ(vmdIn.m_iks[i].m_ikInfos[j].m_name.ToString(),
                            vmdOut.m_iks[i].m_ikInfos[j].m_name.ToString());
            TEST_ASSERT_EQ(vmdIn.m_iks[i].m_ikInfos[j].m_enable,
                            vmdOut.m_iks[i].m_ikInfos[j].m_enable);
        }
    }

    std::cout << "    VMDAnimation IK Add->Save round-trip passed\n";
}

static void test_VMDAnimation_SaveToFile_RoundTrip()
{
    std::cout << "[test] VMDAnimation_SaveToFile_RoundTrip\n";

    auto pmxFile = MakeSimplePMXFileForAnim();
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    // Build a VMD with motion data
    libmmd::VMDFile vmdIn;
    vmdIn.m_header.m_header.Set("Vocaloid Motion Data 0002");
    {
        libmmd::VMDMotion m;
        m.m_boneName.Set("child");
        m.m_frame = 0;
        m.m_translate = Eigen::Vector3f(1.0f, 2.0f, 3.0f);
        m.m_quaternion = Eigen::Quaternionf::Identity();
        m.m_interpolation.fill(20);
        vmdIn.m_motions.push_back(std::move(m));
    }
    {
        libmmd::VMDMotion m;
        m.m_boneName.Set("child");
        m.m_frame = 30;
        m.m_translate = Eigen::Vector3f(4.0f, 5.0f, 6.0f);
        m.m_quaternion = Eigen::Quaternionf::Identity();
        m.m_interpolation.fill(20);
        vmdIn.m_motions.push_back(std::move(m));
    }

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmdIn));

    // Save to file
    TempFile tmp(".vmd");
    TEST_ASSERT(anim.Save(tmp.path()));

    // Read back
    libmmd::VMDFile vmdLoaded;
    TEST_ASSERT(libmmd::ReadVMDFile(&vmdLoaded, tmp.path()));

    // Compare with in-memory Save result
    libmmd::VMDFile vmdMem;
    TEST_ASSERT(anim.Save(vmdMem));

    TEST_ASSERT_EQ(vmdMem.m_header.m_header.ToString(), vmdLoaded.m_header.m_header.ToString());
    TEST_ASSERT_EQ(vmdMem.m_motions.size(), vmdLoaded.m_motions.size());

    for (size_t i = 0; i < vmdMem.m_motions.size(); ++i)
    {
        const auto& a = vmdMem.m_motions[i];
        const auto& b = vmdLoaded.m_motions[i];
        TEST_ASSERT_EQ(a.m_boneName.ToString(), b.m_boneName.ToString());
        TEST_ASSERT_EQ(a.m_frame, b.m_frame);
        TEST_ASSERT_FLOAT_EQ(a.m_translate.x(), b.m_translate.x());
        TEST_ASSERT_FLOAT_EQ(a.m_translate.y(), b.m_translate.y());
        TEST_ASSERT_FLOAT_EQ(a.m_translate.z(), b.m_translate.z());
        TEST_ASSERT_FLOAT_EQ(a.m_quaternion.w(), b.m_quaternion.w());
        TEST_ASSERT_FLOAT_EQ(a.m_quaternion.x(), b.m_quaternion.x());
        TEST_ASSERT_FLOAT_EQ(a.m_quaternion.y(), b.m_quaternion.y());
        TEST_ASSERT_FLOAT_EQ(a.m_quaternion.z(), b.m_quaternion.z());
        TEST_ASSERT(a.m_interpolation == b.m_interpolation);
    }

    TEST_ASSERT_EQ(vmdMem.m_morphs.size(), vmdLoaded.m_morphs.size());
    TEST_ASSERT_EQ(vmdMem.m_iks.size(), vmdLoaded.m_iks.size());

    std::cout << "    VMDAnimation Save-to-file round-trip passed\n";
}

// ===========================================================================
// main
// ===========================================================================

int main()
{
    std::cout << "=== libMMD File Round-Trip Tests ===\n\n";

    test_VMD_RoundTrip();
    test_VMD_EmptySections_RoundTrip();
    test_PMX_RoundTrip();
    test_PMX_RoundTrip_UTF16();
    test_PMX_MorphTypes_RoundTrip();
    test_PMD_RoundTrip();
    test_VPD_RoundTrip();

    // VMDAnimation Add->Save round-trips
    test_VMDAnimation_AddSave_MotionRoundTrip();
    test_VMDAnimation_AddSave_MorphRoundTrip();
    test_VMDAnimation_AddSave_IKRoundTrip();
    test_VMDAnimation_SaveToFile_RoundTrip();

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
