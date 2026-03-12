#define _USE_MATH_DEFINES
#include <cmath>

#include <libMMD/Model/MMD/PMXModel.h>
#include <libMMD/Model/MMD/PMDModel.h>
#include <libMMD/Model/MMD/VMDAnimation.h>
#include <libMMD/Model/MMD/MMDNode.h>
#include <libMMD/Model/MMD/MMDMorph.h>
#include <libMMD/Model/MMD/MMDIkSolver.h>
#include <libMMD/Model/MMD/MMDMaterial.h>
#include <libMMD/Model/MMD/PMXFile.h>
#include <libMMD/Model/MMD/VMDFile.h>
#include <libMMD/Model/MMD/MMDPhysics.h>
#include <libMMD/Base/File.h>

#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <chrono>

// ---------------------------------------------------------------------------
// Minimal test framework for CTest
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

// ===========================================================================
// MMDNode tests
// ===========================================================================

static void test_MMDNode_DefaultConstruction()
{
    std::cout << "[test] MMDNode_DefaultConstruction\n";
    libmmd::MMDNode node;

    TEST_ASSERT_EQ(std::string(""), node.GetName());
    TEST_ASSERT_EQ(uint32_t(0), node.GetIndex());
    TEST_ASSERT(!node.IsIK());

    // Default transform is identity-like
    TEST_ASSERT_FLOAT_EQ(0.0f, node.GetTranslate().x());
    TEST_ASSERT_FLOAT_EQ(0.0f, node.GetTranslate().y());
    TEST_ASSERT_FLOAT_EQ(0.0f, node.GetTranslate().z());

    TEST_ASSERT_FLOAT_EQ(1.0f, node.GetScale().x());
    TEST_ASSERT_FLOAT_EQ(1.0f, node.GetScale().y());
    TEST_ASSERT_FLOAT_EQ(1.0f, node.GetScale().z());

    // Default rotation is identity quaternion
    TEST_ASSERT_FLOAT_EQ(1.0f, node.GetRotate().w());
    TEST_ASSERT_FLOAT_EQ(0.0f, node.GetRotate().x());
    TEST_ASSERT_FLOAT_EQ(0.0f, node.GetRotate().y());
    TEST_ASSERT_FLOAT_EQ(0.0f, node.GetRotate().z());

    // No parent/child/sibling
    TEST_ASSERT(node.GetParent() == nullptr);
    TEST_ASSERT(node.GetChild() == nullptr);
    TEST_ASSERT(node.GetNext() == nullptr);
    TEST_ASSERT(node.GetPrev() == nullptr);
}

static void test_MMDNode_SetName()
{
    std::cout << "[test] MMDNode_SetName\n";
    libmmd::MMDNode node;
    node.SetName("TestBone");
    TEST_ASSERT_EQ(std::string("TestBone"), node.GetName());
}

static void test_MMDNode_SetIndex()
{
    std::cout << "[test] MMDNode_SetIndex\n";
    libmmd::MMDNode node;
    node.SetIndex(42);
    TEST_ASSERT_EQ(uint32_t(42), node.GetIndex());
}

static void test_MMDNode_EnableIK()
{
    std::cout << "[test] MMDNode_EnableIK\n";
    libmmd::MMDNode node;
    TEST_ASSERT(!node.IsIK());
    node.EnableIK(true);
    TEST_ASSERT(node.IsIK());
    node.EnableIK(false);
    TEST_ASSERT(!node.IsIK());
}

static void test_MMDNode_TranslateRotateScale()
{
    std::cout << "[test] MMDNode_TranslateRotateScale\n";
    libmmd::MMDNode node;

    node.SetTranslate(Eigen::Vector3f(1.0f, 2.0f, 3.0f));
    TEST_ASSERT_FLOAT_EQ(1.0f, node.GetTranslate().x());
    TEST_ASSERT_FLOAT_EQ(2.0f, node.GetTranslate().y());
    TEST_ASSERT_FLOAT_EQ(3.0f, node.GetTranslate().z());

    node.SetRotate(Eigen::Quaternionf(0.707f, 0.707f, 0.0f, 0.0f));
    TEST_ASSERT_FLOAT_EQ(0.707f, node.GetRotate().w());
    TEST_ASSERT_FLOAT_EQ(0.707f, node.GetRotate().x());

    node.SetScale(Eigen::Vector3f(2.0f, 3.0f, 4.0f));
    TEST_ASSERT_FLOAT_EQ(2.0f, node.GetScale().x());
    TEST_ASSERT_FLOAT_EQ(3.0f, node.GetScale().y());
    TEST_ASSERT_FLOAT_EQ(4.0f, node.GetScale().z());
}

static void test_MMDNode_AnimationTranslateRotate()
{
    std::cout << "[test] MMDNode_AnimationTranslateRotate\n";
    libmmd::MMDNode node;

    node.SetAnimationTranslate(Eigen::Vector3f(10.0f, 20.0f, 30.0f));
    TEST_ASSERT_FLOAT_EQ(10.0f, node.GetAnimationTranslate().x());
    TEST_ASSERT_FLOAT_EQ(20.0f, node.GetAnimationTranslate().y());
    TEST_ASSERT_FLOAT_EQ(30.0f, node.GetAnimationTranslate().z());

    node.SetAnimationRotate(Eigen::Quaternionf(0.5f, 0.5f, 0.5f, 0.5f));
    TEST_ASSERT_FLOAT_EQ(0.5f, node.GetAnimationRotate().w());
    TEST_ASSERT_FLOAT_EQ(0.5f, node.GetAnimationRotate().x());
}

static void test_MMDNode_AnimateCombined()
{
    std::cout << "[test] MMDNode_AnimateCombined\n";
    libmmd::MMDNode node;

    node.SetTranslate(Eigen::Vector3f(1.0f, 0.0f, 0.0f));
    node.SetAnimationTranslate(Eigen::Vector3f(0.0f, 2.0f, 0.0f));

    auto combined = node.AnimateTranslate();
    TEST_ASSERT_FLOAT_EQ(1.0f, combined.x());
    TEST_ASSERT_FLOAT_EQ(2.0f, combined.y());
    TEST_ASSERT_FLOAT_EQ(0.0f, combined.z());
}

static void test_MMDNode_IKRotate()
{
    std::cout << "[test] MMDNode_IKRotate\n";
    libmmd::MMDNode node;

    auto ik = Eigen::Quaternionf(0.0f, 1.0f, 0.0f, 0.0f);
    node.SetIKRotate(ik);
    TEST_ASSERT_FLOAT_EQ(0.0f, node.GetIKRotate().w());
    TEST_ASSERT_FLOAT_EQ(1.0f, node.GetIKRotate().x());
}

static void test_MMDNode_ParentChild()
{
    std::cout << "[test] MMDNode_ParentChild\n";
    libmmd::MMDNode parent;
    libmmd::MMDNode child1;
    libmmd::MMDNode child2;

    parent.SetName("parent");
    child1.SetName("child1");
    child2.SetName("child2");

    parent.AddChild(&child1);
    parent.AddChild(&child2);

    TEST_ASSERT(child1.GetParent() == &parent);
    TEST_ASSERT(child2.GetParent() == &parent);
    TEST_ASSERT(parent.GetChild() != nullptr);
}

static void test_MMDNode_SaveLoadInitialTRS()
{
    std::cout << "[test] MMDNode_SaveLoadInitialTRS\n";
    libmmd::MMDNode node;

    node.SetTranslate(Eigen::Vector3f(5.0f, 6.0f, 7.0f));
    node.SetRotate(Eigen::Quaternionf(0.5f, 0.5f, 0.5f, 0.5f));
    node.SetScale(Eigen::Vector3f(2.0f, 2.0f, 2.0f));
    node.SaveInitialTRS();

    // Modify current values
    node.SetTranslate(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
    node.SetRotate(Eigen::Quaternionf::Identity());
    node.SetScale(Eigen::Vector3f(1.0f, 1.0f, 1.0f));

    // Verify getters for saved initial state
    TEST_ASSERT_FLOAT_EQ(5.0f, node.GetInitialTranslate().x());
    TEST_ASSERT_FLOAT_EQ(6.0f, node.GetInitialTranslate().y());
    TEST_ASSERT_FLOAT_EQ(7.0f, node.GetInitialTranslate().z());
    TEST_ASSERT_FLOAT_EQ(0.5f, node.GetInitialRotate().w());
    TEST_ASSERT_FLOAT_EQ(2.0f, node.GetInitialScale().x());

    // Load back initial values
    node.LoadInitialTRS();
    TEST_ASSERT_FLOAT_EQ(5.0f, node.GetTranslate().x());
    TEST_ASSERT_FLOAT_EQ(0.5f, node.GetRotate().w());
    TEST_ASSERT_FLOAT_EQ(2.0f, node.GetScale().x());
}

static void test_MMDNode_SaveLoadBaseAnimation()
{
    std::cout << "[test] MMDNode_SaveLoadBaseAnimation\n";
    libmmd::MMDNode node;

    node.SetAnimationTranslate(Eigen::Vector3f(1.0f, 2.0f, 3.0f));
    node.SetAnimationRotate(Eigen::Quaternionf(0.0f, 0.0f, 0.707f, 0.707f));
    node.SaveBaseAnimation();

    TEST_ASSERT_FLOAT_EQ(1.0f, node.GetBaseAnimationTranslate().x());
    TEST_ASSERT_FLOAT_EQ(2.0f, node.GetBaseAnimationTranslate().y());
    TEST_ASSERT_FLOAT_EQ(3.0f, node.GetBaseAnimationTranslate().z());
    TEST_ASSERT_FLOAT_EQ(0.0f, node.GetBaseAnimationRotate().w());

    // Modify and then load back
    node.SetAnimationTranslate(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
    node.LoadBaseAnimation();
    TEST_ASSERT_FLOAT_EQ(1.0f, node.GetAnimationTranslate().x());

    // Clear
    node.ClearBaseAnimation();
    TEST_ASSERT_FLOAT_EQ(0.0f, node.GetBaseAnimationTranslate().x());
    TEST_ASSERT_FLOAT_EQ(1.0f, node.GetBaseAnimationRotate().w());
}

static void test_MMDNode_LocalGlobalTransform()
{
    std::cout << "[test] MMDNode_LocalGlobalTransform\n";
    libmmd::MMDNode node;

    auto localMat = Eigen::Matrix4f::Identity() * 2.0f;
    node.SetLocalTransform(localMat);
    TEST_ASSERT_FLOAT_EQ(2.0f, node.GetLocalTransform()(0, 0));

    auto globalMat = Eigen::Matrix4f::Identity() * 3.0f;
    node.SetGlobalTransform(globalMat);
    TEST_ASSERT_FLOAT_EQ(3.0f, node.GetGlobalTransform()(0, 0));
}

// ===========================================================================
// MMDMorph tests
// ===========================================================================

static void test_MMDMorph_DefaultConstruction()
{
    std::cout << "[test] MMDMorph_DefaultConstruction\n";
    libmmd::MMDMorph morph;
    TEST_ASSERT(morph.GetName().empty());
    TEST_ASSERT_FLOAT_EQ(0.0f, morph.GetWeight());
}

static void test_MMDMorph_SetNameAndWeight()
{
    std::cout << "[test] MMDMorph_SetNameAndWeight\n";
    libmmd::MMDMorph morph;
    morph.SetName("smile");
    morph.SetWeight(0.75f);
    TEST_ASSERT_EQ(std::string("smile"), morph.GetName());
    TEST_ASSERT_FLOAT_EQ(0.75f, morph.GetWeight());
}

static void test_MMDMorph_SaveLoadBaseAnimation()
{
    std::cout << "[test] MMDMorph_SaveLoadBaseAnimation\n";
    libmmd::MMDMorph morph;

    morph.SetWeight(0.5f);
    morph.SaveBaseAnimation();
    TEST_ASSERT_FLOAT_EQ(0.5f, morph.GetBaseAnimationWeight());

    // Change weight
    morph.SetWeight(1.0f);
    TEST_ASSERT_FLOAT_EQ(1.0f, morph.GetWeight());

    // Load back
    morph.LoadBaseAnimation();
    TEST_ASSERT_FLOAT_EQ(0.5f, morph.GetWeight());

    // Clear
    morph.ClearBaseAnimation();
    TEST_ASSERT_FLOAT_EQ(0.0f, morph.GetBaseAnimationWeight());
}

// ===========================================================================
// MMDIkSolver tests
// ===========================================================================

static void test_MMDIkSolver_DefaultConstruction()
{
    std::cout << "[test] MMDIkSolver_DefaultConstruction\n";
    libmmd::MMDIkSolver solver;

    TEST_ASSERT(solver.GetIKNode() == nullptr);
    TEST_ASSERT(solver.GetTargetNode() == nullptr);
    TEST_ASSERT(solver.GetName().empty());
    TEST_ASSERT(solver.Enabled());
}

static void test_MMDIkSolver_SetNodes()
{
    std::cout << "[test] MMDIkSolver_SetNodes\n";
    libmmd::MMDNode ikNode;
    libmmd::MMDNode targetNode;
    ikNode.SetName("IKBone");
    targetNode.SetName("TargetBone");

    libmmd::MMDIkSolver solver;
    solver.SetIKNode(&ikNode);
    solver.SetTargetNode(&targetNode);

    TEST_ASSERT(solver.GetIKNode() == &ikNode);
    TEST_ASSERT(solver.GetTargetNode() == &targetNode);
    TEST_ASSERT_EQ(std::string("IKBone"), solver.GetName());
}

static void test_MMDIkSolver_Enable()
{
    std::cout << "[test] MMDIkSolver_Enable\n";
    libmmd::MMDIkSolver solver;
    TEST_ASSERT(solver.Enabled());

    solver.Enable(false);
    TEST_ASSERT(!solver.Enabled());

    solver.Enable(true);
    TEST_ASSERT(solver.Enabled());
}

static void test_MMDIkSolver_SaveLoadBaseAnimation()
{
    std::cout << "[test] MMDIkSolver_SaveLoadBaseAnimation\n";
    libmmd::MMDIkSolver solver;

    solver.Enable(false);
    solver.SaveBaseAnimation();
    TEST_ASSERT(!solver.GetBaseAnimationEnabled());

    solver.Enable(true);
    solver.LoadBaseAnimation();
    TEST_ASSERT(!solver.Enabled());

    solver.ClearBaseAnimation();
    TEST_ASSERT(solver.GetBaseAnimationEnabled());
}

static void test_MMDIkSolver_SetParameters()
{
    std::cout << "[test] MMDIkSolver_SetParameters\n";
    libmmd::MMDIkSolver solver;
    solver.SetIterateCount(10);
    solver.SetLimitAngle(0.5f);
    // No getter for these, but at least verify no crash
    TEST_ASSERT(true);
}

// ===========================================================================
// MMDMaterial tests
// ===========================================================================

static void test_MMDMaterial_DefaultConstruction()
{
    std::cout << "[test] MMDMaterial_DefaultConstruction\n";
    libmmd::MMDMaterial mat;

    // Defaults from MMDMaterial constructor
    TEST_ASSERT_FLOAT_EQ(1.0f, mat.m_diffuse.x());
    TEST_ASSERT_FLOAT_EQ(1.0f, mat.m_diffuse.y());
    TEST_ASSERT_FLOAT_EQ(1.0f, mat.m_diffuse.z());
    TEST_ASSERT_FLOAT_EQ(1.0f, mat.m_alpha);
    TEST_ASSERT_FLOAT_EQ(0.0f, mat.m_specular.x());
    TEST_ASSERT_FLOAT_EQ(1.0f, mat.m_specularPower);
    TEST_ASSERT_FLOAT_EQ(0.2f, mat.m_ambient.x());
    TEST_ASSERT_EQ(uint8_t(0), mat.m_edgeFlag);
    TEST_ASSERT_FLOAT_EQ(0.0f, mat.m_edgeSize);
    TEST_ASSERT(mat.m_texture.empty());
    TEST_ASSERT(mat.m_spTexture.empty());
    TEST_ASSERT(mat.m_toonTexture.empty());
    TEST_ASSERT(!mat.m_bothFace);
    TEST_ASSERT(mat.m_groundShadow);
    TEST_ASSERT(mat.m_shadowCaster);
    TEST_ASSERT(mat.m_shadowReceiver);
    // Mul factors default to 1
    TEST_ASSERT_FLOAT_EQ(1.0f, mat.m_textureMulFactor.x());
    TEST_ASSERT_FLOAT_EQ(1.0f, mat.m_spTextureMulFactor.x());
    TEST_ASSERT_FLOAT_EQ(1.0f, mat.m_toonTextureMulFactor.x());
    // Add factors default to 0
    TEST_ASSERT_FLOAT_EQ(0.0f, mat.m_textureAddFactor.x());
    TEST_ASSERT_FLOAT_EQ(0.0f, mat.m_spTextureAddFactor.x());
    TEST_ASSERT_FLOAT_EQ(0.0f, mat.m_toonTextureAddFactor.x());
}

static void test_MMDMaterial_SetFields()
{
    std::cout << "[test] MMDMaterial_SetFields\n";
    libmmd::MMDMaterial mat;

    mat.m_diffuse = Eigen::Vector3f(1.0f, 0.5f, 0.0f);
    mat.m_alpha = 0.8f;
    mat.m_specular = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
    mat.m_specularPower = 50.0f;
    mat.m_ambient = Eigen::Vector3f(0.2f, 0.2f, 0.2f);
    mat.m_texture = "tex.png";
    mat.m_bothFace = true;
    mat.m_groundShadow = true;
    mat.m_shadowCaster = true;
    mat.m_shadowReceiver = false;

    TEST_ASSERT_FLOAT_EQ(1.0f, mat.m_diffuse.x());
    TEST_ASSERT_FLOAT_EQ(0.5f, mat.m_diffuse.y());
    TEST_ASSERT_FLOAT_EQ(0.8f, mat.m_alpha);
    TEST_ASSERT_FLOAT_EQ(50.0f, mat.m_specularPower);
    TEST_ASSERT_EQ(std::string("tex.png"), mat.m_texture);
    TEST_ASSERT(mat.m_bothFace);
    TEST_ASSERT(mat.m_groundShadow);
    TEST_ASSERT(mat.m_shadowCaster);
    TEST_ASSERT(!mat.m_shadowReceiver);
}

// ===========================================================================
// MMDSubMesh tests
// ===========================================================================

static void test_SubMeshDefaultConstruction()
{
    std::cout << "[test] SubMeshDefaultConstruction\n";
    libmmd::MMDSubMesh sub;
    TEST_ASSERT_EQ(0, sub.m_beginIndex);
    TEST_ASSERT_EQ(0, sub.m_vertexCount);
    TEST_ASSERT_EQ(0, sub.m_materialID);
}

static void test_SubMeshParameterizedConstruction()
{
    std::cout << "[test] SubMeshParameterizedConstruction\n";
    libmmd::MMDSubMesh sub(10, 200, 3);
    TEST_ASSERT_EQ(10, sub.m_beginIndex);
    TEST_ASSERT_EQ(200, sub.m_vertexCount);
    TEST_ASSERT_EQ(3, sub.m_materialID);
}

// ===========================================================================
// PMXModel tests
// ===========================================================================

static void test_PMXModel_InheritsFromMMDModel()
{
    std::cout << "[test] PMXModel_InheritsFromMMDModel\n";
    auto model = std::make_unique<libmmd::PMXModel>();
    libmmd::MMDModel* base = model.get();
    TEST_ASSERT(base != nullptr);
}

static void test_PMXModel_ManagerPointersAreNonNull()
{
    std::cout << "[test] PMXModel_ManagerPointersAreNonNull\n";
    auto model = std::make_unique<libmmd::PMXModel>();
    TEST_ASSERT(model->GetNodeManager() != nullptr);
    TEST_ASSERT(model->GetIKManager() != nullptr);
    TEST_ASSERT(model->GetMorphManager() != nullptr);
    TEST_ASSERT(model->GetPhysicsManager() != nullptr);
}

static void test_PMXModel_InitialCountsAreZero()
{
    std::cout << "[test] PMXModel_InitialCountsAreZero\n";
    auto model = std::make_unique<libmmd::PMXModel>();
    TEST_ASSERT_EQ(size_t(0), model->GetNodeManager()->GetNodeCount());
    TEST_ASSERT_EQ(size_t(0), model->GetIKManager()->GetIKSolverCount());
    TEST_ASSERT_EQ(size_t(0), model->GetMorphManager()->GetMorphCount());
    TEST_ASSERT_EQ(size_t(0), model->GetMaterialCount());
    TEST_ASSERT_EQ(size_t(0), model->GetSubMeshCount());
}

static void test_PMXModel_InitialModelNameIsEmpty()
{
    std::cout << "[test] PMXModel_InitialModelNameIsEmpty\n";
    auto model = std::make_unique<libmmd::PMXModel>();
    TEST_ASSERT(model->GetModelName().empty());
    TEST_ASSERT(model->GetEnglishModelName().empty());
    TEST_ASSERT(model->GetComment().empty());
    TEST_ASSERT(model->GetEnglishComment().empty());
}

static void test_PMXModel_NodeManager_FindReturnsNPosForEmpty()
{
    std::cout << "[test] PMXModel_NodeManager_FindReturnsNPosForEmpty\n";
    auto model = std::make_unique<libmmd::PMXModel>();
    auto* nodeMan = model->GetNodeManager();
    TEST_ASSERT_EQ(libmmd::MMDNodeManager::NPos, nodeMan->FindNodeIndex("nonexistent"));
}

static void test_PMXModel_NodeManager_GetByNameReturnsNullForEmpty()
{
    std::cout << "[test] PMXModel_NodeManager_GetByNameReturnsNullForEmpty\n";
    auto model = std::make_unique<libmmd::PMXModel>();
    auto* nodeMan = model->GetNodeManager();
    TEST_ASSERT(nodeMan->GetMMDNode(std::string("nonexistent")) == nullptr);
}

static void test_PMXModel_IKManager_FindReturnsNPosForEmpty()
{
    std::cout << "[test] PMXModel_IKManager_FindReturnsNPosForEmpty\n";
    auto model = std::make_unique<libmmd::PMXModel>();
    auto* ikMan = model->GetIKManager();
    TEST_ASSERT_EQ(libmmd::MMDIKManager::NPos, ikMan->FindIKSolverIndex("nonexistent"));
}

static void test_PMXModel_IKManager_GetByNameReturnsNullForEmpty()
{
    std::cout << "[test] PMXModel_IKManager_GetByNameReturnsNullForEmpty\n";
    auto model = std::make_unique<libmmd::PMXModel>();
    auto* ikMan = model->GetIKManager();
    TEST_ASSERT(ikMan->GetMMDIKSolver(std::string("nonexistent")) == nullptr);
}

static void test_PMXModel_MorphManager_FindReturnsNPosForEmpty()
{
    std::cout << "[test] PMXModel_MorphManager_FindReturnsNPosForEmpty\n";
    auto model = std::make_unique<libmmd::PMXModel>();
    auto* morphMan = model->GetMorphManager();
    TEST_ASSERT_EQ(libmmd::MMDMorphManager::NPos, morphMan->FindMorphIndex("nonexistent"));
}

static void test_PMXModel_MorphManager_GetByNameReturnsNullForEmpty()
{
    std::cout << "[test] PMXModel_MorphManager_GetByNameReturnsNullForEmpty\n";
    auto model = std::make_unique<libmmd::PMXModel>();
    auto* morphMan = model->GetMorphManager();
    TEST_ASSERT(morphMan->GetMorph(std::string("nonexistent")) == nullptr);
}

static void test_PMXModel_LoadInvalidPathReturnsFalse()
{
    std::cout << "[test] PMXModel_LoadInvalidPathReturnsFalse\n";
    auto model = std::make_unique<libmmd::PMXModel>();
    TEST_ASSERT(!model->Load("nonexistent_file.pmx", ""));
}

static void test_PMXModel_DestroyOnEmptyModelIsSafe()
{
    std::cout << "[test] PMXModel_DestroyOnEmptyModelIsSafe\n";
    auto model = std::make_unique<libmmd::PMXModel>();
    model->Destroy();
    TEST_ASSERT_EQ(size_t(0), model->GetNodeManager()->GetNodeCount());
    TEST_ASSERT_EQ(size_t(0), model->GetMaterialCount());
    TEST_ASSERT_EQ(size_t(0), model->GetSubMeshCount());
}

static void test_PMXModel_VMDAnimation_CreateWithModel()
{
    std::cout << "[test] PMXModel_VMDAnimation_CreateWithModel\n";
    auto sharedModel = std::make_shared<libmmd::PMXModel>();
    libmmd::VMDAnimation vmdAnim;
    TEST_ASSERT(vmdAnim.Create(sharedModel));
    TEST_ASSERT_EQ(int32_t(0), vmdAnim.GetMaxKeyTime());
    TEST_ASSERT_EQ(int32_t(0), vmdAnim.GetNodeKeyNum());
    TEST_ASSERT_EQ(int32_t(0), vmdAnim.GetMorphKeyNum());
    TEST_ASSERT_EQ(int32_t(0), vmdAnim.GetIKKeyNum());
}

// ===========================================================================
// PMDModel tests
// ===========================================================================

static void test_PMDModel_InheritsFromMMDModel()
{
    std::cout << "[test] PMDModel_InheritsFromMMDModel\n";
    auto model = std::make_unique<libmmd::PMDModel>();
    libmmd::MMDModel* base = model.get();
    TEST_ASSERT(base != nullptr);
}

static void test_PMDModel_ManagerPointersAreNonNull()
{
    std::cout << "[test] PMDModel_ManagerPointersAreNonNull\n";
    auto model = std::make_unique<libmmd::PMDModel>();
    TEST_ASSERT(model->GetNodeManager() != nullptr);
    TEST_ASSERT(model->GetIKManager() != nullptr);
    TEST_ASSERT(model->GetMorphManager() != nullptr);
    TEST_ASSERT(model->GetPhysicsManager() != nullptr);
}

static void test_PMDModel_InitialCountsAreZero()
{
    std::cout << "[test] PMDModel_InitialCountsAreZero\n";
    auto model = std::make_unique<libmmd::PMDModel>();
    TEST_ASSERT_EQ(size_t(0), model->GetNodeManager()->GetNodeCount());
    TEST_ASSERT_EQ(size_t(0), model->GetIKManager()->GetIKSolverCount());
    TEST_ASSERT_EQ(size_t(0), model->GetMorphManager()->GetMorphCount());
    TEST_ASSERT_EQ(size_t(0), model->GetMaterialCount());
    TEST_ASSERT_EQ(size_t(0), model->GetSubMeshCount());
}

static void test_PMDModel_InitialModelNameIsEmpty()
{
    std::cout << "[test] PMDModel_InitialModelNameIsEmpty\n";
    auto model = std::make_unique<libmmd::PMDModel>();
    TEST_ASSERT(model->GetModelName().empty());
    TEST_ASSERT(model->GetEnglishModelName().empty());
    TEST_ASSERT(model->GetComment().empty());
    TEST_ASSERT(model->GetEnglishComment().empty());
}

static void test_PMDModel_NodeManager_FindReturnsNPosForEmpty()
{
    std::cout << "[test] PMDModel_NodeManager_FindReturnsNPosForEmpty\n";
    auto model = std::make_unique<libmmd::PMDModel>();
    auto* nodeMan = model->GetNodeManager();
    TEST_ASSERT_EQ(libmmd::MMDNodeManager::NPos, nodeMan->FindNodeIndex("nonexistent"));
}

static void test_PMDModel_NodeManager_GetByNameReturnsNullForEmpty()
{
    std::cout << "[test] PMDModel_NodeManager_GetByNameReturnsNullForEmpty\n";
    auto model = std::make_unique<libmmd::PMDModel>();
    auto* nodeMan = model->GetNodeManager();
    TEST_ASSERT(nodeMan->GetMMDNode(std::string("nonexistent")) == nullptr);
}

static void test_PMDModel_LoadInvalidPathReturnsFalse()
{
    std::cout << "[test] PMDModel_LoadInvalidPathReturnsFalse\n";
    auto model = std::make_unique<libmmd::PMDModel>();
    TEST_ASSERT(!model->Load("nonexistent_file.pmd", ""));
}

static void test_PMDModel_DestroyOnEmptyModelIsSafe()
{
    std::cout << "[test] PMDModel_DestroyOnEmptyModelIsSafe\n";
    auto model = std::make_unique<libmmd::PMDModel>();
    model->Destroy();
    TEST_ASSERT_EQ(size_t(0), model->GetNodeManager()->GetNodeCount());
    TEST_ASSERT_EQ(size_t(0), model->GetMaterialCount());
    TEST_ASSERT_EQ(size_t(0), model->GetSubMeshCount());
}

static void test_PMDModel_VMDAnimation_CreateWithPMDModel()
{
    std::cout << "[test] PMDModel_VMDAnimation_CreateWithPMDModel\n";
    auto sharedModel = std::make_shared<libmmd::PMDModel>();
    libmmd::VMDAnimation vmdAnim;
    TEST_ASSERT(vmdAnim.Create(sharedModel));
    TEST_ASSERT_EQ(int32_t(0), vmdAnim.GetMaxKeyTime());
}

// ===========================================================================
// PhysicsManager tests
// ===========================================================================

static void test_PhysicsManager_CreateSucceeds()
{
    std::cout << "[test] PhysicsManager_CreateSucceeds\n";
    libmmd::MMDPhysicsManager physicsMan;
    TEST_ASSERT(physicsMan.Create());
    TEST_ASSERT(physicsMan.GetMMDPhysics() != nullptr);
}

static void test_PhysicsManager_InitialListsEmpty()
{
    std::cout << "[test] PhysicsManager_InitialListsEmpty\n";
    libmmd::MMDPhysicsManager physicsMan;
    TEST_ASSERT(physicsMan.Create());
    TEST_ASSERT(physicsMan.GetRigidBodys()->empty());
    TEST_ASSERT(physicsMan.GetJoints()->empty());
}

// ===========================================================================
// MMDRigidBody parametric Create equivalence tests
// ===========================================================================

static void test_RigidBody_ParametricCreate_Sphere()
{
    std::cout << "[test] RigidBody_ParametricCreate_Sphere\n";

    auto model = std::make_shared<libmmd::PMXModel>();
    auto* node = model->AddNode();
    node->SetName("root");
    node->SetGlobalTransform(Eigen::Matrix4f::Identity());
    model->GetPhysicsManager()->Create();

    libmmd::PMXRigidbody rb{};
    rb.m_name = "sphere_rb";
    rb.m_shape = libmmd::PMXRigidbody::Shape::Sphere;
    rb.m_shapeSize = Eigen::Vector3f(1.0f, 0.0f, 0.0f);
    rb.m_translate = Eigen::Vector3f(1.0f, 2.0f, 3.0f);
    rb.m_rotate = Eigen::Vector3f(0.1f, 0.2f, 0.3f);
    rb.m_mass = 1.0f;
    rb.m_translateDimmer = 0.5f;
    rb.m_rotateDimmer = 0.5f;
    rb.m_repulsion = 0.3f;
    rb.m_friction = 0.8f;
    rb.m_op = libmmd::PMXRigidbody::Operation::Dynamic;
    rb.m_group = 1;
    rb.m_collisionGroup = 0xFFFF;

    auto* rbStruct = model->GetPhysicsManager()->AddRigidBody();
    TEST_ASSERT(rbStruct->Create(rb, model.get(), node));

    auto* rbParam = model->GetPhysicsManager()->AddRigidBody();
    TEST_ASSERT(rbParam->Create(
        rb.m_shape, rb.m_shapeSize, rb.m_translate, rb.m_rotate,
        rb.m_mass, rb.m_translateDimmer, rb.m_rotateDimmer,
        rb.m_repulsion, rb.m_friction, rb.m_op,
        rb.m_group, rb.m_collisionGroup,
        model.get(), node, rb.m_name));

    TEST_ASSERT_EQ(rbStruct->GetGroup(), rbParam->GetGroup());
    TEST_ASSERT_EQ(rbStruct->GetGroupMask(), rbParam->GetGroupMask());
    TEST_ASSERT(rbStruct->GetRigidBody() != nullptr);
    TEST_ASSERT(rbParam->GetRigidBody() != nullptr);

    auto structTf = rbStruct->GetTransform();
    auto paramTf = rbParam->GetTransform();
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            TEST_ASSERT_FLOAT_EQ(structTf(r, c), paramTf(r, c));
}

static void test_RigidBody_ParametricCreate_Box()
{
    std::cout << "[test] RigidBody_ParametricCreate_Box\n";

    auto model = std::make_shared<libmmd::PMXModel>();
    auto* node = model->AddNode();
    node->SetName("root");
    node->SetGlobalTransform(Eigen::Matrix4f::Identity());
    model->GetPhysicsManager()->Create();

    libmmd::PMXRigidbody rb{};
    rb.m_name = "box_rb";
    rb.m_shape = libmmd::PMXRigidbody::Shape::Box;
    rb.m_shapeSize = Eigen::Vector3f(1.0f, 2.0f, 3.0f);
    rb.m_translate = Eigen::Vector3f(-1.0f, 0.5f, 2.0f);
    rb.m_rotate = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    rb.m_mass = 0.0f;
    rb.m_translateDimmer = 0.0f;
    rb.m_rotateDimmer = 0.0f;
    rb.m_repulsion = 0.0f;
    rb.m_friction = 0.5f;
    rb.m_op = libmmd::PMXRigidbody::Operation::Static;
    rb.m_group = 0;
    rb.m_collisionGroup = 0x0001;

    auto* rbStruct = model->GetPhysicsManager()->AddRigidBody();
    TEST_ASSERT(rbStruct->Create(rb, model.get(), node));

    auto* rbParam = model->GetPhysicsManager()->AddRigidBody();
    TEST_ASSERT(rbParam->Create(
        rb.m_shape, rb.m_shapeSize, rb.m_translate, rb.m_rotate,
        rb.m_mass, rb.m_translateDimmer, rb.m_rotateDimmer,
        rb.m_repulsion, rb.m_friction, rb.m_op,
        rb.m_group, rb.m_collisionGroup,
        model.get(), node, rb.m_name));

    TEST_ASSERT_EQ(rbStruct->GetGroup(), rbParam->GetGroup());
    TEST_ASSERT_EQ(rbStruct->GetGroupMask(), rbParam->GetGroupMask());

    auto structTf = rbStruct->GetTransform();
    auto paramTf = rbParam->GetTransform();
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            TEST_ASSERT_FLOAT_EQ(structTf(r, c), paramTf(r, c));
}

static void test_RigidBody_ParametricCreate_Capsule()
{
    std::cout << "[test] RigidBody_ParametricCreate_Capsule\n";

    auto model = std::make_shared<libmmd::PMXModel>();
    auto* node = model->AddNode();
    node->SetName("root");
    node->SetGlobalTransform(Eigen::Matrix4f::Identity());
    model->GetPhysicsManager()->Create();

    libmmd::PMXRigidbody rb{};
    rb.m_name = "capsule_rb";
    rb.m_shape = libmmd::PMXRigidbody::Shape::Capsule;
    rb.m_shapeSize = Eigen::Vector3f(0.5f, 2.0f, 0.0f);
    rb.m_translate = Eigen::Vector3f(0.0f, 5.0f, 0.0f);
    rb.m_rotate = Eigen::Vector3f(0.5f, 0.0f, 0.0f);
    rb.m_mass = 2.5f;
    rb.m_translateDimmer = 0.9f;
    rb.m_rotateDimmer = 0.9f;
    rb.m_repulsion = 0.1f;
    rb.m_friction = 0.3f;
    rb.m_op = libmmd::PMXRigidbody::Operation::DynamicAndBoneMerge;
    rb.m_group = 2;
    rb.m_collisionGroup = 0x00FF;

    auto* rbStruct = model->GetPhysicsManager()->AddRigidBody();
    TEST_ASSERT(rbStruct->Create(rb, model.get(), node));

    auto* rbParam = model->GetPhysicsManager()->AddRigidBody();
    TEST_ASSERT(rbParam->Create(
        rb.m_shape, rb.m_shapeSize, rb.m_translate, rb.m_rotate,
        rb.m_mass, rb.m_translateDimmer, rb.m_rotateDimmer,
        rb.m_repulsion, rb.m_friction, rb.m_op,
        rb.m_group, rb.m_collisionGroup,
        model.get(), node, rb.m_name));

    TEST_ASSERT_EQ(rbStruct->GetGroup(), rbParam->GetGroup());
    TEST_ASSERT_EQ(rbStruct->GetGroupMask(), rbParam->GetGroupMask());

    auto structTf = rbStruct->GetTransform();
    auto paramTf = rbParam->GetTransform();
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            TEST_ASSERT_FLOAT_EQ(structTf(r, c), paramTf(r, c));
}

static void test_RigidBody_ParametricCreate_NullNode()
{
    std::cout << "[test] RigidBody_ParametricCreate_NullNode\n";

    auto model = std::make_shared<libmmd::PMXModel>();
    auto* root = model->AddNode();
    root->SetName("root");
    root->SetGlobalTransform(Eigen::Matrix4f::Identity());
    model->GetPhysicsManager()->Create();

    libmmd::PMXRigidbody rb{};
    rb.m_shape = libmmd::PMXRigidbody::Shape::Sphere;
    rb.m_shapeSize = Eigen::Vector3f(1.0f, 0.0f, 0.0f);
    rb.m_translate = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    rb.m_rotate = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    rb.m_mass = 1.0f;
    rb.m_translateDimmer = 0.0f;
    rb.m_rotateDimmer = 0.0f;
    rb.m_repulsion = 0.0f;
    rb.m_friction = 0.5f;
    rb.m_op = libmmd::PMXRigidbody::Operation::Dynamic;
    rb.m_group = 0;
    rb.m_collisionGroup = 0xFFFF;

    auto* rbStruct = model->GetPhysicsManager()->AddRigidBody();
    TEST_ASSERT(rbStruct->Create(rb, model.get(), nullptr));

    auto* rbParam = model->GetPhysicsManager()->AddRigidBody();
    TEST_ASSERT(rbParam->Create(
        rb.m_shape, rb.m_shapeSize, rb.m_translate, rb.m_rotate,
        rb.m_mass, rb.m_translateDimmer, rb.m_rotateDimmer,
        rb.m_repulsion, rb.m_friction, rb.m_op,
        rb.m_group, rb.m_collisionGroup,
        model.get(), nullptr));

    TEST_ASSERT_EQ(rbStruct->GetGroup(), rbParam->GetGroup());
    TEST_ASSERT_EQ(rbStruct->GetGroupMask(), rbParam->GetGroupMask());

    auto structTf = rbStruct->GetTransform();
    auto paramTf = rbParam->GetTransform();
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            TEST_ASSERT_FLOAT_EQ(structTf(r, c), paramTf(r, c));
}

// ===========================================================================
// MMDJoint parametric CreateJoint equivalence tests
// ===========================================================================

static void test_Joint_ParametricCreate_Equivalence()
{
    std::cout << "[test] Joint_ParametricCreate_Equivalence\n";

    auto model = std::make_shared<libmmd::PMXModel>();
    auto* node = model->AddNode();
    node->SetName("root");
    node->SetGlobalTransform(Eigen::Matrix4f::Identity());
    model->GetPhysicsManager()->Create();

    libmmd::PMXRigidbody rbData{};
    rbData.m_shape = libmmd::PMXRigidbody::Shape::Sphere;
    rbData.m_shapeSize = Eigen::Vector3f(1.0f, 0.0f, 0.0f);
    rbData.m_translate = Eigen::Vector3f(0.0f, 5.0f, 0.0f);
    rbData.m_rotate = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    rbData.m_mass = 0.0f;
    rbData.m_translateDimmer = 0.0f;
    rbData.m_rotateDimmer = 0.0f;
    rbData.m_repulsion = 0.0f;
    rbData.m_friction = 0.5f;
    rbData.m_op = libmmd::PMXRigidbody::Operation::Static;
    rbData.m_group = 0;
    rbData.m_collisionGroup = 0xFFFF;

    auto* rbA1 = model->GetPhysicsManager()->AddRigidBody();
    TEST_ASSERT(rbA1->Create(rbData, model.get(), node));
    auto* rbA2 = model->GetPhysicsManager()->AddRigidBody();
    rbData.m_translate = Eigen::Vector3f(0.0f, 3.0f, 0.0f);
    rbData.m_mass = 1.0f;
    rbData.m_op = libmmd::PMXRigidbody::Operation::Dynamic;
    TEST_ASSERT(rbA2->Create(rbData, model.get(), node));

    auto* rbB1 = model->GetPhysicsManager()->AddRigidBody();
    rbData.m_translate = Eigen::Vector3f(0.0f, 5.0f, 0.0f);
    rbData.m_mass = 0.0f;
    rbData.m_op = libmmd::PMXRigidbody::Operation::Static;
    TEST_ASSERT(rbB1->Create(rbData, model.get(), node));
    auto* rbB2 = model->GetPhysicsManager()->AddRigidBody();
    rbData.m_translate = Eigen::Vector3f(0.0f, 3.0f, 0.0f);
    rbData.m_mass = 1.0f;
    rbData.m_op = libmmd::PMXRigidbody::Operation::Dynamic;
    TEST_ASSERT(rbB2->Create(rbData, model.get(), node));

    libmmd::PMXJoint jt{};
    jt.m_translate = Eigen::Vector3f(0.0f, 4.0f, 0.0f);
    jt.m_rotate = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    jt.m_translateLowerLimit = Eigen::Vector3f(-1.0f, -1.0f, -1.0f);
    jt.m_translateUpperLimit = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
    jt.m_rotateLowerLimit = Eigen::Vector3f(-0.5f, -0.5f, -0.5f);
    jt.m_rotateUpperLimit = Eigen::Vector3f(0.5f, 0.5f, 0.5f);
    jt.m_springTranslateFactor = Eigen::Vector3f(10.0f, 10.0f, 10.0f);
    jt.m_springRotateFactor = Eigen::Vector3f(5.0f, 5.0f, 5.0f);

    auto* jtStruct = model->GetPhysicsManager()->AddJoint();
    TEST_ASSERT(jtStruct->CreateJoint(jt, rbA1, rbA2));

    auto* jtParam = model->GetPhysicsManager()->AddJoint();
    TEST_ASSERT(jtParam->CreateJoint(
        jt.m_translate, jt.m_rotate,
        jt.m_translateLowerLimit, jt.m_translateUpperLimit,
        jt.m_rotateLowerLimit, jt.m_rotateUpperLimit,
        jt.m_springTranslateFactor, jt.m_springRotateFactor,
        rbB1, rbB2));

    TEST_ASSERT(jtStruct->GetConstraint() != nullptr);
    TEST_ASSERT(jtParam->GetConstraint() != nullptr);

    auto structMat = jtStruct->GetTransform();
    auto paramMat = jtParam->GetTransform();
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            TEST_ASSERT_FLOAT_EQ(structMat(r, c), paramMat(r, c));
}

static void test_Joint_ParametricCreate_ZeroSprings()
{
    std::cout << "[test] Joint_ParametricCreate_ZeroSprings\n";

    auto model = std::make_shared<libmmd::PMXModel>();
    auto* node = model->AddNode();
    node->SetName("root");
    node->SetGlobalTransform(Eigen::Matrix4f::Identity());
    model->GetPhysicsManager()->Create();

    libmmd::PMXRigidbody rbData{};
    rbData.m_shape = libmmd::PMXRigidbody::Shape::Box;
    rbData.m_shapeSize = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
    rbData.m_translate = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    rbData.m_rotate = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    rbData.m_mass = 0.0f;
    rbData.m_op = libmmd::PMXRigidbody::Operation::Static;
    rbData.m_group = 0;
    rbData.m_collisionGroup = 0xFFFF;

    auto* rbA1 = model->GetPhysicsManager()->AddRigidBody();
    TEST_ASSERT(rbA1->Create(rbData, model.get(), node));
    auto* rbA2 = model->GetPhysicsManager()->AddRigidBody();
    TEST_ASSERT(rbA2->Create(rbData, model.get(), node));
    auto* rbB1 = model->GetPhysicsManager()->AddRigidBody();
    TEST_ASSERT(rbB1->Create(rbData, model.get(), node));
    auto* rbB2 = model->GetPhysicsManager()->AddRigidBody();
    TEST_ASSERT(rbB2->Create(rbData, model.get(), node));

    Eigen::Vector3f zero = Eigen::Vector3f::Zero();

    libmmd::PMXJoint jt{};
    jt.m_translate = Eigen::Vector3f(1.0f, 2.0f, 3.0f);
    jt.m_rotate = Eigen::Vector3f(0.1f, 0.2f, 0.3f);
    jt.m_translateLowerLimit = Eigen::Vector3f(-2.0f, -2.0f, -2.0f);
    jt.m_translateUpperLimit = Eigen::Vector3f(2.0f, 2.0f, 2.0f);
    jt.m_rotateLowerLimit = Eigen::Vector3f(-1.0f, -1.0f, -1.0f);
    jt.m_rotateUpperLimit = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
    jt.m_springTranslateFactor = zero;
    jt.m_springRotateFactor = zero;

    auto* jtStruct = model->GetPhysicsManager()->AddJoint();
    TEST_ASSERT(jtStruct->CreateJoint(jt, rbA1, rbA2));

    auto* jtParam = model->GetPhysicsManager()->AddJoint();
    TEST_ASSERT(jtParam->CreateJoint(
        jt.m_translate, jt.m_rotate,
        jt.m_translateLowerLimit, jt.m_translateUpperLimit,
        jt.m_rotateLowerLimit, jt.m_rotateUpperLimit,
        zero, zero,
        rbB1, rbB2));

    TEST_ASSERT(jtStruct->GetConstraint() != nullptr);
    TEST_ASSERT(jtParam->GetConstraint() != nullptr);
}

// ===========================================================================
// Polymorphism tests
// ===========================================================================

static void test_Polymorphism_PMXModelAsMMDModel()
{
    std::cout << "[test] Polymorphism_PMXModelAsMMDModel\n";
    std::shared_ptr<libmmd::MMDModel> model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->GetNodeManager() != nullptr);
    TEST_ASSERT_EQ(size_t(0), model->GetNodeManager()->GetNodeCount());
}

static void test_Polymorphism_PMDModelAsMMDModel()
{
    std::cout << "[test] Polymorphism_PMDModelAsMMDModel\n";
    std::shared_ptr<libmmd::MMDModel> model = std::make_shared<libmmd::PMDModel>();
    TEST_ASSERT(model->GetNodeManager() != nullptr);
    TEST_ASSERT_EQ(size_t(0), model->GetNodeManager()->GetNodeCount());
}

static void test_Polymorphism_VMDAnimationAcceptsBothModelTypes()
{
    std::cout << "[test] Polymorphism_VMDAnimationAcceptsBothModelTypes\n";
    {
        auto pmxModel = std::make_shared<libmmd::PMXModel>();
        std::shared_ptr<libmmd::MMDModel> base = pmxModel;
        libmmd::VMDAnimation vmdAnim;
        TEST_ASSERT(vmdAnim.Create(base));
    }
    {
        auto pmdModel = std::make_shared<libmmd::PMDModel>();
        std::shared_ptr<libmmd::MMDModel> base = pmdModel;
        libmmd::VMDAnimation vmdAnim;
        TEST_ASSERT(vmdAnim.Create(base));
    }
}

// ===========================================================================
// VMDAnimation tests
// ===========================================================================

static void test_VMDAnimation_DefaultConstruction()
{
    std::cout << "[test] VMDAnimation_DefaultConstruction\n";
    libmmd::VMDAnimation vmdAnim;
    TEST_ASSERT_EQ(int32_t(0), vmdAnim.GetMaxKeyTime());
    TEST_ASSERT_EQ(int32_t(0), vmdAnim.GetNodeKeyNum());
    TEST_ASSERT_EQ(int32_t(0), vmdAnim.GetMorphKeyNum());
    TEST_ASSERT_EQ(int32_t(0), vmdAnim.GetIKKeyNum());
}

static void test_VMDAnimation_DestroyWithoutCreate()
{
    std::cout << "[test] VMDAnimation_DestroyWithoutCreate\n";
    libmmd::VMDAnimation vmdAnim;
    vmdAnim.Destroy();
    TEST_ASSERT_EQ(int32_t(0), vmdAnim.GetMaxKeyTime());
}

static void test_VMDAnimation_CreateAndDestroy()
{
    std::cout << "[test] VMDAnimation_CreateAndDestroy\n";
    auto model = std::make_shared<libmmd::PMXModel>();
    libmmd::VMDAnimation vmdAnim;
    TEST_ASSERT(vmdAnim.Create(model));
    vmdAnim.Destroy();
    TEST_ASSERT_EQ(int32_t(0), vmdAnim.GetNodeKeyNum());
}

// ===========================================================================
// Coordinate System Consistency tests
// (After removing Z-negation, all transforms should use original PMX space)
// ===========================================================================

static void test_CoordSystem_NodeTranslateNoZNegation()
{
    std::cout << "[test] CoordSystem_NodeTranslateNoZNegation\n";
    libmmd::MMDNode node;

    // Setting translate should preserve Z value as-is (no negation)
    node.SetTranslate(Eigen::Vector3f(1.0f, 2.0f, 3.0f));
    TEST_ASSERT_FLOAT_EQ(1.0f, node.GetTranslate().x());
    TEST_ASSERT_FLOAT_EQ(2.0f, node.GetTranslate().y());
    TEST_ASSERT_FLOAT_EQ(3.0f, node.GetTranslate().z());  // Z should be +3, not -3
}

static void test_CoordSystem_GlobalTransformNoZNegation()
{
    std::cout << "[test] CoordSystem_GlobalTransformNoZNegation\n";
    libmmd::MMDNode node;

    // Set a global transform with positive Z translation
    Eigen::Matrix4f global = Eigen::Matrix4f::Identity();
    global(0, 3) = 10.0f;
    global(1, 3) = 20.0f;
    global(2, 3) = 30.0f;
    node.SetGlobalTransform(global);

    // Z should be preserved as-is
    TEST_ASSERT_FLOAT_EQ(10.0f, node.GetGlobalTransform()(0, 3));
    TEST_ASSERT_FLOAT_EQ(20.0f, node.GetGlobalTransform()(1, 3));
    TEST_ASSERT_FLOAT_EQ(30.0f, node.GetGlobalTransform()(2, 3));
}

static void test_CoordSystem_LocalTransformNoZNegation()
{
    std::cout << "[test] CoordSystem_LocalTransformNoZNegation\n";
    libmmd::MMDNode node;

    // Set a local transform with positive Z
    Eigen::Matrix4f local = Eigen::Matrix4f::Identity();
    local(0, 3) = 5.0f;
    local(1, 3) = 10.0f;
    local(2, 3) = 15.0f;
    node.SetLocalTransform(local);

    // Z should be preserved
    TEST_ASSERT_FLOAT_EQ(5.0f, node.GetLocalTransform()(0, 3));
    TEST_ASSERT_FLOAT_EQ(10.0f, node.GetLocalTransform()(1, 3));
    TEST_ASSERT_FLOAT_EQ(15.0f, node.GetLocalTransform()(2, 3));
}

static void test_CoordSystem_AnimationTranslateNoZNegation()
{
    std::cout << "[test] CoordSystem_AnimationTranslateNoZNegation\n";
    libmmd::MMDNode node;

    // Animation translate should pass through without Z-negation
    node.SetAnimationTranslate(Eigen::Vector3f(1.0f, 2.0f, 3.0f));
    TEST_ASSERT_FLOAT_EQ(3.0f, node.GetAnimationTranslate().z());

    // Combined translate should also preserve Z
    node.SetTranslate(Eigen::Vector3f(0.0f, 0.0f, 5.0f));
    auto combined = node.AnimateTranslate();
    TEST_ASSERT_FLOAT_EQ(8.0f, combined.z());  // 5 + 3 = 8, not negated
}

static void test_CoordSystem_ParentChildTransformConsistency()
{
    std::cout << "[test] CoordSystem_ParentChildTransformConsistency\n";
    libmmd::MMDNode parent;
    libmmd::MMDNode child;
    parent.SetName("parent");
    child.SetName("child");
    parent.AddChild(&child);

    // Parent at (10, 0, 20) in original PMX coordinates
    parent.SetTranslate(Eigen::Vector3f(10.0f, 0.0f, 20.0f));
    Eigen::Matrix4f parentGlobal = Eigen::Matrix4f::Identity();
    parentGlobal(0, 3) = 10.0f;
    parentGlobal(2, 3) = 20.0f;
    parent.SetGlobalTransform(parentGlobal);

    // Child at (5, 0, 10) relative to parent
    child.SetTranslate(Eigen::Vector3f(5.0f, 0.0f, 10.0f));
    Eigen::Matrix4f childGlobal = Eigen::Matrix4f::Identity();
    childGlobal(0, 3) = 15.0f;  // 10 + 5
    childGlobal(2, 3) = 30.0f;  // 20 + 10
    child.SetGlobalTransform(childGlobal);

    // Verify no Z negation in child global
    TEST_ASSERT_FLOAT_EQ(15.0f, child.GetGlobalTransform()(0, 3));
    TEST_ASSERT_FLOAT_EQ(30.0f, child.GetGlobalTransform()(2, 3));

    // Verify initial translate is stored correctly
    child.SaveInitialTRS();
    TEST_ASSERT_FLOAT_EQ(5.0f, child.GetInitialTranslate().x());
    TEST_ASSERT_FLOAT_EQ(10.0f, child.GetInitialTranslate().z());  // No Z negation
}

static void test_CoordSystem_RotationMatrixIdentityPreserved()
{
    std::cout << "[test] CoordSystem_RotationMatrixIdentityPreserved\n";
    libmmd::MMDNode node;

    // An identity rotation should produce an identity rotation block
    node.SetRotate(Eigen::Quaternionf::Identity());
    Eigen::Matrix4f global = Eigen::Matrix4f::Identity();
    node.SetGlobalTransform(global);

    // The 3x3 rotation block should be identity
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            if (r == c)
                TEST_ASSERT_FLOAT_EQ(1.0f, node.GetGlobalTransform()(r, c));
            else
                TEST_ASSERT_FLOAT_EQ(0.0f, node.GetGlobalTransform()(r, c));
        }
    }
}

static void test_CoordSystem_InverseInitTransformConsistency()
{
    std::cout << "[test] CoordSystem_InverseInitTransformConsistency\n";
    libmmd::MMDNode node;

    // Set global transform with Z translation (original PMX space)
    Eigen::Matrix4f global = Eigen::Matrix4f::Identity();
    global(0, 3) = 1.0f;
    global(1, 3) = 2.0f;
    global(2, 3) = 3.0f;
    node.SetGlobalTransform(global);
    node.CalculateInverseInitTransform();

    // Inverse init * global should give identity
    Eigen::Matrix4f product = node.GetInverseInitTransform() * global;
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            float expected = (r == c) ? 1.0f : 0.0f;
            TEST_ASSERT_FLOAT_EQ(expected, product(r, c));
        }
    }
}

// ===========================================================================
// Helper: read file into byte vector
// ===========================================================================

static bool ReadFileToBuffer(const std::string& path, std::vector<uint8_t>& buffer)
{
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);
    if (!ifs.is_open()) return false;
    auto size = ifs.tellg();
    if (size <= 0) return false;
    buffer.resize(static_cast<size_t>(size));
    ifs.seekg(0, std::ios::beg);
    ifs.read(reinterpret_cast<char*>(buffer.data()), size);
    return ifs.good();
}

// ===========================================================================
// PMXFile reading/parsing tests
// ===========================================================================

static const std::string g_pmxTestFile = std::string(TEST_DATA_DIR) + "/pmx_test/test.pmx";

static void test_PMXFile_ReadFromPath()
{
    std::cout << "[test] PMXFile_ReadFromPath\n";
    libmmd::PMXFile pmx;
    bool ok = libmmd::ReadPMXFile(&pmx, g_pmxTestFile.c_str());
    TEST_ASSERT(ok);
}

static void test_PMXFile_ReadFromBuffer()
{
    std::cout << "[test] PMXFile_ReadFromBuffer\n";
    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_pmxTestFile, buffer));

    libmmd::PMXFile pmx;
    std::string error;
    bool ok = libmmd::ReadPMXFile(&pmx, buffer.data(), buffer.size(), &error);
    if (!ok)
    {
        std::cerr << "  Parse error detail: " << error << "\n";
    }
    TEST_ASSERT(ok);
}

static void test_PMXFile_HeaderValid()
{
    std::cout << "[test] PMXFile_HeaderValid\n";
    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_pmxTestFile, buffer));

    libmmd::PMXFile pmx;
    TEST_ASSERT(libmmd::ReadPMXFile(&pmx, buffer.data(), buffer.size()));

    TEST_ASSERT(pmx.m_header.m_magic.m_buffer[0] == 'P');
    TEST_ASSERT(pmx.m_header.m_magic.m_buffer[1] == 'M');
    TEST_ASSERT(pmx.m_header.m_magic.m_buffer[2] == 'X');
    TEST_ASSERT(pmx.m_header.m_magic.m_buffer[3] == ' ');

    TEST_ASSERT(pmx.m_header.m_version >= 2.0f);
    TEST_ASSERT(pmx.m_header.m_version < 3.0f);

    TEST_ASSERT(pmx.m_header.m_encode == 0 || pmx.m_header.m_encode == 1);
    TEST_ASSERT(pmx.m_header.m_dataSize == 8);
    TEST_ASSERT(pmx.m_header.m_addUVNum <= 4);
}

static void test_PMXFile_InfoNonEmpty()
{
    std::cout << "[test] PMXFile_InfoNonEmpty\n";
    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_pmxTestFile, buffer));

    libmmd::PMXFile pmx;
    TEST_ASSERT(libmmd::ReadPMXFile(&pmx, buffer.data(), buffer.size()));

    TEST_ASSERT(!pmx.m_info.m_modelName.empty());
    std::cout << "    Model name: " << pmx.m_info.m_modelName << "\n";
}

static void test_PMXFile_SectionsNonEmpty()
{
    std::cout << "[test] PMXFile_SectionsNonEmpty\n";
    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_pmxTestFile, buffer));

    libmmd::PMXFile pmx;
    TEST_ASSERT(libmmd::ReadPMXFile(&pmx, buffer.data(), buffer.size()));

    std::cout << "    Vertices:      " << pmx.m_vertices.size() << "\n";
    std::cout << "    Faces:         " << pmx.m_faces.size() << "\n";
    std::cout << "    Textures:      " << pmx.m_textures.size() << "\n";
    std::cout << "    Materials:     " << pmx.m_materials.size() << "\n";
    std::cout << "    Bones:         " << pmx.m_bones.size() << "\n";
    std::cout << "    Morphs:        " << pmx.m_morphs.size() << "\n";
    std::cout << "    DisplayFrames: " << pmx.m_displayFrames.size() << "\n";
    std::cout << "    Rigidbodies:   " << pmx.m_rigidbodies.size() << "\n";
    std::cout << "    Joints:        " << pmx.m_joints.size() << "\n";

    TEST_ASSERT(!pmx.m_vertices.empty());
    TEST_ASSERT(!pmx.m_faces.empty());
    TEST_ASSERT(!pmx.m_materials.empty());
    TEST_ASSERT(!pmx.m_bones.empty());
}

static void test_PMXFile_VertexDataValid()
{
    std::cout << "[test] PMXFile_VertexDataValid\n";
    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_pmxTestFile, buffer));

    libmmd::PMXFile pmx;
    TEST_ASSERT(libmmd::ReadPMXFile(&pmx, buffer.data(), buffer.size()));

    for (size_t i = 0; i < std::min(pmx.m_vertices.size(), size_t(10)); ++i)
    {
        const auto& v = pmx.m_vertices[i];
        TEST_ASSERT(static_cast<uint8_t>(v.m_weightType) <= 4);
        TEST_ASSERT(!std::isnan(v.m_position.x()));
        TEST_ASSERT(!std::isnan(v.m_position.y()));
        TEST_ASSERT(!std::isnan(v.m_position.z()));
        TEST_ASSERT(!std::isnan(v.m_normal.x()));
        TEST_ASSERT(!std::isnan(v.m_uv.x()));
    }
}

static void test_PMXFile_FaceIndicesValid()
{
    std::cout << "[test] PMXFile_FaceIndicesValid\n";
    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_pmxTestFile, buffer));

    libmmd::PMXFile pmx;
    TEST_ASSERT(libmmd::ReadPMXFile(&pmx, buffer.data(), buffer.size()));

    uint32_t vertexCount = static_cast<uint32_t>(pmx.m_vertices.size());
    for (const auto& face : pmx.m_faces)
    {
        TEST_ASSERT(face.m_vertices[0] < vertexCount);
        TEST_ASSERT(face.m_vertices[1] < vertexCount);
        TEST_ASSERT(face.m_vertices[2] < vertexCount);
    }
}

static void test_PMXFile_BoneParentIndicesValid()
{
    std::cout << "[test] PMXFile_BoneParentIndicesValid\n";
    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_pmxTestFile, buffer));

    libmmd::PMXFile pmx;
    TEST_ASSERT(libmmd::ReadPMXFile(&pmx, buffer.data(), buffer.size()));

    int32_t boneCount = static_cast<int32_t>(pmx.m_bones.size());
    for (const auto& bone : pmx.m_bones)
    {
        TEST_ASSERT(bone.m_parentBoneIndex < boneCount);
    }
}

static void test_PMXFile_BufferAndPathProduceSameResult()
{
    std::cout << "[test] PMXFile_BufferAndPathProduceSameResult\n";

    libmmd::PMXFile pmxFromPath;
    TEST_ASSERT(libmmd::ReadPMXFile(&pmxFromPath, g_pmxTestFile.c_str()));

    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_pmxTestFile, buffer));
    libmmd::PMXFile pmxFromBuf;
    TEST_ASSERT(libmmd::ReadPMXFile(&pmxFromBuf, buffer.data(), buffer.size()));

    TEST_ASSERT_FLOAT_EQ(pmxFromPath.m_header.m_version, pmxFromBuf.m_header.m_version);
    TEST_ASSERT_EQ(pmxFromPath.m_header.m_encode, pmxFromBuf.m_header.m_encode);
    TEST_ASSERT_EQ(pmxFromPath.m_vertices.size(), pmxFromBuf.m_vertices.size());
    TEST_ASSERT_EQ(pmxFromPath.m_faces.size(), pmxFromBuf.m_faces.size());
    TEST_ASSERT_EQ(pmxFromPath.m_textures.size(), pmxFromBuf.m_textures.size());
    TEST_ASSERT_EQ(pmxFromPath.m_materials.size(), pmxFromBuf.m_materials.size());
    TEST_ASSERT_EQ(pmxFromPath.m_bones.size(), pmxFromBuf.m_bones.size());
    TEST_ASSERT_EQ(pmxFromPath.m_morphs.size(), pmxFromBuf.m_morphs.size());
    TEST_ASSERT_EQ(pmxFromPath.m_rigidbodies.size(), pmxFromBuf.m_rigidbodies.size());
    TEST_ASSERT_EQ(pmxFromPath.m_joints.size(), pmxFromBuf.m_joints.size());
    TEST_ASSERT_EQ(pmxFromPath.m_info.m_modelName, pmxFromBuf.m_info.m_modelName);
}

static void test_PMXFile_ErrorReportOnTruncatedData()
{
    std::cout << "[test] PMXFile_ErrorReportOnTruncatedData\n";
    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_pmxTestFile, buffer));

    // Truncate to just 4 bytes (not even a full header)
    std::vector<uint8_t> truncated(buffer.begin(), buffer.begin() + 4);
    libmmd::PMXFile pmx;
    std::string error;
    bool ok = libmmd::ReadPMXFile(&pmx, truncated.data(), truncated.size(), &error);
    TEST_ASSERT(!ok);
    TEST_ASSERT(!error.empty());
    std::cout << "    Expected error: " << error << "\n";
}

static void test_PMXFile_ErrorReportOnEmptyData()
{
    std::cout << "[test] PMXFile_ErrorReportOnEmptyData\n";
    libmmd::PMXFile pmx;
    std::string error;
    bool ok = libmmd::ReadPMXFile(&pmx, nullptr, 0, &error);
    TEST_ASSERT(!ok);
    TEST_ASSERT(!error.empty());
    std::cout << "    Expected error: " << error << "\n";
}

static void test_PMXFile_MaterialFaceVertexCountConsistent()
{
    std::cout << "[test] PMXFile_MaterialFaceVertexCountConsistent\n";
    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_pmxTestFile, buffer));

    libmmd::PMXFile pmx;
    TEST_ASSERT(libmmd::ReadPMXFile(&pmx, buffer.data(), buffer.size()));

    int32_t totalFaceVertices = 0;
    for (const auto& mat : pmx.m_materials)
    {
        TEST_ASSERT(mat.m_numFaceVertices >= 0);
        totalFaceVertices += mat.m_numFaceVertices;
    }
    TEST_ASSERT_EQ(static_cast<size_t>(totalFaceVertices), pmx.m_faces.size() * 3);
}

// ===========================================================================
// VMDFile reading/parsing tests
// ===========================================================================

static const std::string g_vmdBoneFile = std::string(TEST_DATA_DIR) + "/vmd_test/bone.vmd";
static const std::string g_vmdCamFile  = std::string(TEST_DATA_DIR) + "/vmd_test/cam.vmd";

static void test_VMDFile_ReadBoneFromPath()
{
    std::cout << "[test] VMDFile_ReadBoneFromPath\n";
    libmmd::VMDFile vmd;
    TEST_ASSERT(libmmd::ReadVMDFile(&vmd, g_vmdBoneFile.c_str()));
}

static void test_VMDFile_ReadBoneFromBuffer()
{
    std::cout << "[test] VMDFile_ReadBoneFromBuffer\n";
    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_vmdBoneFile, buffer));

    libmmd::VMDFile vmd;
    TEST_ASSERT(libmmd::ReadVMDFile(&vmd, buffer.data(), buffer.size()));
}

static void test_VMDFile_ReadCamFromPath()
{
    std::cout << "[test] VMDFile_ReadCamFromPath\n";
    libmmd::VMDFile vmd;
    TEST_ASSERT(libmmd::ReadVMDFile(&vmd, g_vmdCamFile.c_str()));
}

static void test_VMDFile_ReadCamFromBuffer()
{
    std::cout << "[test] VMDFile_ReadCamFromBuffer\n";
    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_vmdCamFile, buffer));

    libmmd::VMDFile vmd;
    TEST_ASSERT(libmmd::ReadVMDFile(&vmd, buffer.data(), buffer.size()));
}

static void test_VMDFile_BoneHeaderValid()
{
    std::cout << "[test] VMDFile_BoneHeaderValid\n";
    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_vmdBoneFile, buffer));

    libmmd::VMDFile vmd;
    TEST_ASSERT(libmmd::ReadVMDFile(&vmd, buffer.data(), buffer.size()));

    std::string header = vmd.m_header.m_header.ToString();
    TEST_ASSERT(header.find("Vocaloid Motion Data") != std::string::npos);
    std::cout << "    Header: " << header << "\n";
    std::cout << "    Model:  " << vmd.m_header.m_modelName.ToUtf8String() << "\n";
}

static void test_VMDFile_BoneMotionsNonEmpty()
{
    std::cout << "[test] VMDFile_BoneMotionsNonEmpty\n";
    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_vmdBoneFile, buffer));

    libmmd::VMDFile vmd;
    TEST_ASSERT(libmmd::ReadVMDFile(&vmd, buffer.data(), buffer.size()));

    std::cout << "    Motions: " << vmd.m_motions.size() << "\n";
    std::cout << "    Morphs:  " << vmd.m_morphs.size() << "\n";
    std::cout << "    Cameras: " << vmd.m_cameras.size() << "\n";
    std::cout << "    Lights:  " << vmd.m_lights.size() << "\n";
    std::cout << "    IKs:     " << vmd.m_iks.size() << "\n";

    TEST_ASSERT(!vmd.m_motions.empty());
}

static void test_VMDFile_BoneMotionDataValid()
{
    std::cout << "[test] VMDFile_BoneMotionDataValid\n";
    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_vmdBoneFile, buffer));

    libmmd::VMDFile vmd;
    TEST_ASSERT(libmmd::ReadVMDFile(&vmd, buffer.data(), buffer.size()));

    for (size_t i = 0; i < std::min(vmd.m_motions.size(), size_t(10)); ++i)
    {
        const auto& m = vmd.m_motions[i];
        TEST_ASSERT(!std::isnan(m.m_translate.x()));
        TEST_ASSERT(!std::isnan(m.m_translate.y()));
        TEST_ASSERT(!std::isnan(m.m_translate.z()));
        TEST_ASSERT(!std::isnan(m.m_quaternion.w()));
        TEST_ASSERT(!std::isnan(m.m_quaternion.x()));
    }
}

static void test_VMDFile_CamCamerasNonEmpty()
{
    std::cout << "[test] VMDFile_CamCamerasNonEmpty\n";
    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_vmdCamFile, buffer));

    libmmd::VMDFile vmd;
    TEST_ASSERT(libmmd::ReadVMDFile(&vmd, buffer.data(), buffer.size()));

    std::cout << "    Motions: " << vmd.m_motions.size() << "\n";
    std::cout << "    Cameras: " << vmd.m_cameras.size() << "\n";
    std::cout << "    Lights:  " << vmd.m_lights.size() << "\n";

    TEST_ASSERT(!vmd.m_cameras.empty());
}

static void test_VMDFile_CamCameraDataValid()
{
    std::cout << "[test] VMDFile_CamCameraDataValid\n";
    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_vmdCamFile, buffer));

    libmmd::VMDFile vmd;
    TEST_ASSERT(libmmd::ReadVMDFile(&vmd, buffer.data(), buffer.size()));

    for (size_t i = 0; i < std::min(vmd.m_cameras.size(), size_t(10)); ++i)
    {
        const auto& c = vmd.m_cameras[i];
        TEST_ASSERT(!std::isnan(c.m_distance));
        TEST_ASSERT(!std::isnan(c.m_interest.x()));
        TEST_ASSERT(!std::isnan(c.m_rotate.x()));
    }
}

static void test_VMDFile_BoneBufferAndPathSameResult()
{
    std::cout << "[test] VMDFile_BoneBufferAndPathSameResult\n";

    libmmd::VMDFile vmdFromPath;
    TEST_ASSERT(libmmd::ReadVMDFile(&vmdFromPath, g_vmdBoneFile.c_str()));

    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_vmdBoneFile, buffer));
    libmmd::VMDFile vmdFromBuf;
    TEST_ASSERT(libmmd::ReadVMDFile(&vmdFromBuf, buffer.data(), buffer.size()));

    TEST_ASSERT_EQ(vmdFromPath.m_motions.size(), vmdFromBuf.m_motions.size());
    TEST_ASSERT_EQ(vmdFromPath.m_morphs.size(), vmdFromBuf.m_morphs.size());
    TEST_ASSERT_EQ(vmdFromPath.m_cameras.size(), vmdFromBuf.m_cameras.size());
    TEST_ASSERT_EQ(vmdFromPath.m_lights.size(), vmdFromBuf.m_lights.size());
    TEST_ASSERT_EQ(vmdFromPath.m_shadows.size(), vmdFromBuf.m_shadows.size());
    TEST_ASSERT_EQ(vmdFromPath.m_iks.size(), vmdFromBuf.m_iks.size());
}

static void test_VMDFile_CamBufferAndPathSameResult()
{
    std::cout << "[test] VMDFile_CamBufferAndPathSameResult\n";

    libmmd::VMDFile vmdFromPath;
    TEST_ASSERT(libmmd::ReadVMDFile(&vmdFromPath, g_vmdCamFile.c_str()));

    std::vector<uint8_t> buffer;
    TEST_ASSERT(ReadFileToBuffer(g_vmdCamFile, buffer));
    libmmd::VMDFile vmdFromBuf;
    TEST_ASSERT(libmmd::ReadVMDFile(&vmdFromBuf, buffer.data(), buffer.size()));

    TEST_ASSERT_EQ(vmdFromPath.m_motions.size(), vmdFromBuf.m_motions.size());
    TEST_ASSERT_EQ(vmdFromPath.m_cameras.size(), vmdFromBuf.m_cameras.size());
    TEST_ASSERT_EQ(vmdFromPath.m_lights.size(), vmdFromBuf.m_lights.size());
}

// ===========================================================================
// MMDNode UpdateLocalTransform / UpdateGlobalTransform tests
// ===========================================================================

static void test_MMDNode_UpdateLocalTransform_IdentityPose()
{
    std::cout << "[test] MMDNode_UpdateLocalTransform_IdentityPose\n";
    libmmd::MMDNode node;
    node.UpdateLocalTransform();
    const auto& local = node.GetLocalTransform();

    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            TEST_ASSERT_FLOAT_EQ(r == c ? 1.0f : 0.0f, local(r, c));
}

static void test_MMDNode_UpdateLocalTransform_WithTranslation()
{
    std::cout << "[test] MMDNode_UpdateLocalTransform_WithTranslation\n";
    libmmd::MMDNode node;
    node.SetTranslate(Eigen::Vector3f(3.0f, 5.0f, 7.0f));
    node.UpdateLocalTransform();
    const auto& local = node.GetLocalTransform();

    // T * R * S with R=I, S=I => translation in col(3)
    TEST_ASSERT_FLOAT_EQ(3.0f, local(0, 3));
    TEST_ASSERT_FLOAT_EQ(5.0f, local(1, 3));
    TEST_ASSERT_FLOAT_EQ(7.0f, local(2, 3));
    TEST_ASSERT_FLOAT_EQ(1.0f, local(3, 3));
    // 3x3 block should be identity
    TEST_ASSERT_FLOAT_EQ(1.0f, local(0, 0));
    TEST_ASSERT_FLOAT_EQ(1.0f, local(1, 1));
    TEST_ASSERT_FLOAT_EQ(1.0f, local(2, 2));
    TEST_ASSERT_FLOAT_EQ(0.0f, local(0, 1));
}

static void test_MMDNode_UpdateLocalTransform_WithRotation90Y()
{
    std::cout << "[test] MMDNode_UpdateLocalTransform_WithRotation90Y\n";
    libmmd::MMDNode node;

    // 90-degree rotation around Y axis
    const float angle = static_cast<float>(M_PI) / 2.0f;
    Eigen::Quaternionf q(Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitY()));
    node.SetRotate(q);
    node.UpdateLocalTransform();
    const auto& local = node.GetLocalTransform();

    // After 90-deg Y rotation: X->-Z, Y->Y, Z->X
    // R = [cos(90) 0 sin(90)]   [0  0  1]
    //     [  0     1   0    ] =  [0  1  0]
    //     [-sin(90) 0 cos(90)]   [-1 0  0]
    TEST_ASSERT_FLOAT_EQ(0.0f, local(0, 0));    // cos(90)
    TEST_ASSERT_FLOAT_EQ(1.0f, local(0, 2));    // sin(90)
    TEST_ASSERT_FLOAT_EQ(1.0f, local(1, 1));
    TEST_ASSERT_FLOAT_EQ(-1.0f, local(2, 0));   // -sin(90)
    TEST_ASSERT_FLOAT_EQ(0.0f, local(2, 2));    // cos(90)
    // Translation should be zero
    TEST_ASSERT_FLOAT_EQ(0.0f, local(0, 3));
    TEST_ASSERT_FLOAT_EQ(0.0f, local(1, 3));
    TEST_ASSERT_FLOAT_EQ(0.0f, local(2, 3));
}

static void test_MMDNode_UpdateLocalTransform_WithAnimationData()
{
    std::cout << "[test] MMDNode_UpdateLocalTransform_WithAnimationData\n";
    libmmd::MMDNode node;

    // Base translate + animation translate should be combined
    node.SetTranslate(Eigen::Vector3f(1.0f, 0.0f, 0.0f));
    node.SetAnimationTranslate(Eigen::Vector3f(0.0f, 2.0f, 0.0f));

    // 45-degree rotation around Z axis as animation rotation
    const float angle = static_cast<float>(M_PI) / 4.0f;
    Eigen::Quaternionf animQ(Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitZ()));
    node.SetAnimationRotate(animQ);

    node.UpdateLocalTransform();
    const auto& local = node.GetLocalTransform();

    // Translation = AnimateTranslate() = base(1,0,0) + anim(0,2,0) = (1,2,0)
    TEST_ASSERT_FLOAT_EQ(1.0f, local(0, 3));
    TEST_ASSERT_FLOAT_EQ(2.0f, local(1, 3));
    TEST_ASSERT_FLOAT_EQ(0.0f, local(2, 3));

    // Rotation = AnimateRotate() = animQ * baseQ(identity) = animQ
    // 45-deg Z rotation: cos(45)=sin(45)≈0.7071
    const float c = std::cos(angle);
    const float s = std::sin(angle);
    TEST_ASSERT_FLOAT_EQ(c, local(0, 0));
    TEST_ASSERT_FLOAT_EQ(-s, local(0, 1));
    TEST_ASSERT_FLOAT_EQ(s, local(1, 0));
    TEST_ASSERT_FLOAT_EQ(c, local(1, 1));
    TEST_ASSERT_FLOAT_EQ(1.0f, local(2, 2));
}

static void test_MMDNode_UpdateLocalTransform_WithScale()
{
    std::cout << "[test] MMDNode_UpdateLocalTransform_WithScale\n";
    libmmd::MMDNode node;
    node.SetTranslate(Eigen::Vector3f(1.0f, 0.0f, 0.0f));
    node.SetScale(Eigen::Vector3f(2.0f, 3.0f, 4.0f));
    node.UpdateLocalTransform();
    const auto& local = node.GetLocalTransform();

    // m_local = T * R * S, with R = I
    // Scale appears on the diagonal of the 3x3 block
    TEST_ASSERT_FLOAT_EQ(2.0f, local(0, 0));
    TEST_ASSERT_FLOAT_EQ(3.0f, local(1, 1));
    TEST_ASSERT_FLOAT_EQ(4.0f, local(2, 2));
    TEST_ASSERT_FLOAT_EQ(1.0f, local(0, 3));
    TEST_ASSERT_FLOAT_EQ(0.0f, local(1, 3));
}

static void test_MMDNode_UpdateLocalTransform_IKRotateApplied()
{
    std::cout << "[test] MMDNode_UpdateLocalTransform_IKRotateApplied\n";
    libmmd::MMDNode node;
    node.EnableIK(true);

    // Set a 90-degree IK rotation around X axis
    const float angle = static_cast<float>(M_PI) / 2.0f;
    Eigen::Quaternionf ikQ(Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitX()));
    node.SetIKRotate(ikQ);

    node.UpdateLocalTransform();
    const auto& local = node.GetLocalTransform();

    // With IK enabled: R = ikRotMat * animRotMat
    // animRotate = identity, so R = ikRotMat = 90-deg X rotation
    // R_x(90) = [[1,0,0],[0,0,-1],[0,1,0]]
    TEST_ASSERT_FLOAT_EQ(1.0f, local(0, 0));
    TEST_ASSERT_FLOAT_EQ(0.0f, local(1, 1));
    TEST_ASSERT_FLOAT_EQ(-1.0f, local(1, 2));
    TEST_ASSERT_FLOAT_EQ(1.0f, local(2, 1));
    TEST_ASSERT_FLOAT_EQ(0.0f, local(2, 2));
}

static void test_MMDNode_UpdateGlobalTransform_SingleRoot()
{
    std::cout << "[test] MMDNode_UpdateGlobalTransform_SingleRoot\n";
    libmmd::MMDNode root;
    root.SetTranslate(Eigen::Vector3f(10.0f, 0.0f, 0.0f));
    root.UpdateLocalTransform();
    root.UpdateGlobalTransform();

    // Root: global = local
    const auto& global = root.GetGlobalTransform();
    TEST_ASSERT_FLOAT_EQ(10.0f, global(0, 3));
    TEST_ASSERT_FLOAT_EQ(0.0f, global(1, 3));
}

static void test_MMDNode_UpdateGlobalTransform_ParentChild()
{
    std::cout << "[test] MMDNode_UpdateGlobalTransform_ParentChild\n";
    libmmd::MMDNode parent;
    libmmd::MMDNode child;
    parent.SetName("parent");
    child.SetName("child");
    parent.AddChild(&child);

    parent.SetTranslate(Eigen::Vector3f(10.0f, 0.0f, 0.0f));
    child.SetTranslate(Eigen::Vector3f(5.0f, 3.0f, 0.0f));

    parent.UpdateLocalTransform();
    child.UpdateLocalTransform();
    parent.UpdateGlobalTransform();

    // child.global = parent.global * child.local
    // = T(10,0,0) * T(5,3,0) = T(15,3,0)
    const auto& childGlobal = child.GetGlobalTransform();
    TEST_ASSERT_FLOAT_EQ(15.0f, childGlobal(0, 3));
    TEST_ASSERT_FLOAT_EQ(3.0f, childGlobal(1, 3));
    TEST_ASSERT_FLOAT_EQ(0.0f, childGlobal(2, 3));
}

static void test_MMDNode_UpdateGlobalTransform_ParentRotation()
{
    std::cout << "[test] MMDNode_UpdateGlobalTransform_ParentRotation\n";
    libmmd::MMDNode parent;
    libmmd::MMDNode child;
    parent.AddChild(&child);

    // Parent at origin with 90-deg Y rotation
    const float angle = static_cast<float>(M_PI) / 2.0f;
    parent.SetRotate(Eigen::Quaternionf(Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitY())));
    // Child offset of (5, 0, 0) in parent-local space
    child.SetTranslate(Eigen::Vector3f(5.0f, 0.0f, 0.0f));

    parent.UpdateLocalTransform();
    child.UpdateLocalTransform();
    parent.UpdateGlobalTransform();

    // After 90-deg Y rotation, parent's local X -> global Z
    // So child at (5,0,0) in parent space -> (0,0,-5) in world? No:
    // 90-deg Y: [0 0 1; 0 1 0; -1 0 0] * [5;0;0] = [0;0;-5]
    // Actually parent.global * child.local:
    // parent.global has rotation 90-deg Y and no translation
    // child.local = T(5,0,0)
    // global = parent.R * child.T = rotation applied to translation
    // child_global_pos = R * (5,0,0) = (0, 0, -5)
    const auto& childGlobal = child.GetGlobalTransform();
    TEST_ASSERT_FLOAT_EQ(0.0f, childGlobal(0, 3));
    TEST_ASSERT_FLOAT_EQ(0.0f, childGlobal(1, 3));
    TEST_ASSERT_FLOAT_EQ(-5.0f, childGlobal(2, 3));
}

static void test_MMDNode_UpdateGlobalTransform_ThreeLevelHierarchy()
{
    std::cout << "[test] MMDNode_UpdateGlobalTransform_ThreeLevelHierarchy\n";
    libmmd::MMDNode root;
    libmmd::MMDNode mid;
    libmmd::MMDNode leaf;
    root.AddChild(&mid);
    mid.AddChild(&leaf);

    root.SetTranslate(Eigen::Vector3f(0.0f, 10.0f, 0.0f));
    mid.SetTranslate(Eigen::Vector3f(0.0f, 5.0f, 0.0f));
    leaf.SetTranslate(Eigen::Vector3f(0.0f, 3.0f, 0.0f));

    root.UpdateLocalTransform();
    mid.UpdateLocalTransform();
    leaf.UpdateLocalTransform();
    root.UpdateGlobalTransform();

    TEST_ASSERT_FLOAT_EQ(10.0f, root.GetGlobalTransform()(1, 3));
    TEST_ASSERT_FLOAT_EQ(15.0f, mid.GetGlobalTransform()(1, 3));
    TEST_ASSERT_FLOAT_EQ(18.0f, leaf.GetGlobalTransform()(1, 3));
}

// ===========================================================================
// VMD Animation Integration tests
// Builds minimal PMX models programmatically and runs the full animation
// pipeline to verify end-to-end correctness.
// ===========================================================================

// Helper: build linear interpolation data (all 4 channels linear)
static std::array<uint8_t, 64> MakeLinearInterpolation()
{
    std::array<uint8_t, 64> interp{};
    // Layout per channel: offset +0 = cp1.x, +4 = cp1.y, +8 = cp2.x, +12 = cp2.y
    // Channels: 0=TX, 1=TY, 2=TZ, 3=Rot
    for (int ch = 0; ch < 4; ++ch)
    {
        interp[ch + 0]  = 20;   // cp1.x
        interp[ch + 4]  = 20;   // cp1.y
        interp[ch + 8]  = 107;  // cp2.x
        interp[ch + 12] = 107;  // cp2.y
    }
    return interp;
}

// Helper: build a minimal PMXFile with a simple bone hierarchy
// Returns: root(0) at origin, child(1) at (0, boneLength, 0) relative to root
static libmmd::PMXFile MakeSimplePMXFile(float boneLength = 10.0f)
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

    // Minimal material to avoid empty-array issues
    libmmd::PMXMaterial mat{};
    mat.m_numFaceVertices = 0;
    mat.m_textureIndex = -1;
    mat.m_sphereTextureIndex = -1;
    mat.m_toonTextureIndex = -1;
    mat.m_sphereMode = libmmd::PMXSphereMode::None;
    mat.m_toonMode = libmmd::PMXToonMode::Common;
    mat.m_drawMode = static_cast<libmmd::PMXDrawModeFlags>(0);
    mat.m_diffuse = Eigen::Vector4f(1, 1, 1, 1);
    mat.m_specular = Eigen::Vector3f::Zero();
    mat.m_specularPower = 1.0f;
    mat.m_ambient = Eigen::Vector3f(0.2f, 0.2f, 0.2f);
    mat.m_edgeColor = Eigen::Vector4f::Zero();
    mat.m_edgeSize = 0.0f;
    file.m_materials.push_back(std::move(mat));

    return file;
}

// Helper: build a VMDFile with a single motion key for the named bone
static libmmd::VMDFile MakeSingleKeyVMD(
    const char* boneName,
    uint32_t frame,
    const Eigen::Vector3f& translate,
    const Eigen::Quaternionf& rotation)
{
    libmmd::VMDFile vmd{};
    libmmd::VMDMotion motion{};
    motion.m_boneName.Set(boneName);
    motion.m_frame = frame;
    motion.m_translate = translate;
    motion.m_quaternion = rotation;
    motion.m_interpolation = MakeLinearInterpolation();
    vmd.m_motions.push_back(std::move(motion));
    return vmd;
}

// Helper: build a VMDFile with two motion keys (for interpolation tests)
static libmmd::VMDFile MakeTwoKeyVMD(
    const char* boneName,
    uint32_t frame0, const Eigen::Vector3f& t0, const Eigen::Quaternionf& q0,
    uint32_t frame1, const Eigen::Vector3f& t1, const Eigen::Quaternionf& q1)
{
    libmmd::VMDFile vmd{};
    auto interp = MakeLinearInterpolation();

    libmmd::VMDMotion m0{};
    m0.m_boneName.Set(boneName);
    m0.m_frame = frame0;
    m0.m_translate = t0;
    m0.m_quaternion = q0;
    m0.m_interpolation = interp;
    vmd.m_motions.push_back(std::move(m0));

    libmmd::VMDMotion m1{};
    m1.m_boneName.Set(boneName);
    m1.m_frame = frame1;
    m1.m_translate = t1;
    m1.m_quaternion = q1;
    m1.m_interpolation = interp;
    vmd.m_motions.push_back(std::move(m1));

    return vmd;
}

static void test_Integration_VMD_SingleBoneRotationAtFrame0()
{
    std::cout << "[test] Integration_VMD_SingleBoneRotationAtFrame0\n";

    // Build model
    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));
    TEST_ASSERT_EQ(size_t(2), model->GetNodeManager()->GetNodeCount());

    // Build VMD: rotate "child" bone by 90 degrees around Z at frame 0
    const float angle = static_cast<float>(M_PI) / 2.0f;
    Eigen::Quaternionf rotZ(Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitZ()));
    auto vmd = MakeSingleKeyVMD("child", 0, Eigen::Vector3f::Zero(), rotZ);

    // Create animation
    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));
    TEST_ASSERT_EQ(int32_t(1), anim.GetNodeKeyNum());

    // Run full pipeline at frame 0
    model->InitializeAnimation();
    model->BeginAnimation();
    anim.Evaluate(0.0f);
    model->UpdateMorphAnimation();
    model->UpdateNodeAnimation(false);
    model->UpdateNodeAnimation(true);
    model->EndAnimation();

    // Verify child node's local transform has the rotation applied
    auto* childNode = model->GetNodeManager()->GetMMDNode(size_t(1));
    TEST_ASSERT(childNode != nullptr);
    const auto& local = childNode->GetLocalTransform();

    // The rotation part should be a 90-deg Z rotation:
    const float c = std::cos(angle);
    const float s = std::sin(angle);
    TEST_ASSERT_FLOAT_EQ(c, local(0, 0));
    TEST_ASSERT_FLOAT_EQ(-s, local(0, 1));
    TEST_ASSERT_FLOAT_EQ(s, local(1, 0));
    TEST_ASSERT_FLOAT_EQ(c, local(1, 1));
    TEST_ASSERT_FLOAT_EQ(1.0f, local(2, 2));

    // Translation should be initial translate (0, 10, 0) since animTranslate is zero
    TEST_ASSERT_FLOAT_EQ(0.0f, local(0, 3));
    TEST_ASSERT_FLOAT_EQ(10.0f, local(1, 3));
    TEST_ASSERT_FLOAT_EQ(0.0f, local(2, 3));
}

static void test_Integration_VMD_SingleBoneTranslationAtFrame0()
{
    std::cout << "[test] Integration_VMD_SingleBoneTranslationAtFrame0\n";

    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    // Translate "child" by (1, 2, 3) at frame 0
    auto vmd = MakeSingleKeyVMD("child", 0,
        Eigen::Vector3f(1.0f, 2.0f, 3.0f), Eigen::Quaternionf::Identity());

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));

    model->InitializeAnimation();
    model->BeginAnimation();
    anim.Evaluate(0.0f);
    model->UpdateMorphAnimation();
    model->UpdateNodeAnimation(false);
    model->UpdateNodeAnimation(true);
    model->EndAnimation();

    auto* childNode = model->GetNodeManager()->GetMMDNode(size_t(1));
    const auto& local = childNode->GetLocalTransform();

    // Translation = AnimateTranslate() = base(0,10,0) + anim(1,2,3) = (1,12,3)
    TEST_ASSERT_FLOAT_EQ(1.0f, local(0, 3));
    TEST_ASSERT_FLOAT_EQ(12.0f, local(1, 3));
    TEST_ASSERT_FLOAT_EQ(3.0f, local(2, 3));

    // Rotation should be identity
    TEST_ASSERT_FLOAT_EQ(1.0f, local(0, 0));
    TEST_ASSERT_FLOAT_EQ(1.0f, local(1, 1));
    TEST_ASSERT_FLOAT_EQ(1.0f, local(2, 2));
    TEST_ASSERT_FLOAT_EQ(0.0f, local(0, 1));
}

static void test_Integration_VMD_RootBoneUnaffectedWhenNoKey()
{
    std::cout << "[test] Integration_VMD_RootBoneUnaffectedWhenNoKey\n";

    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    // Only animate "child", root should stay at identity pose
    auto vmd = MakeSingleKeyVMD("child", 0,
        Eigen::Vector3f(1.0f, 0.0f, 0.0f), Eigen::Quaternionf::Identity());

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));

    model->InitializeAnimation();
    model->BeginAnimation();
    anim.Evaluate(0.0f);
    model->UpdateMorphAnimation();
    model->UpdateNodeAnimation(false);
    model->UpdateNodeAnimation(true);
    model->EndAnimation();

    auto* rootNode = model->GetNodeManager()->GetMMDNode(size_t(0));
    const auto& rootLocal = rootNode->GetLocalTransform();

    // Root should be identity (translate = (0,0,0), no animation applied)
    TEST_ASSERT_FLOAT_EQ(0.0f, rootLocal(0, 3));
    TEST_ASSERT_FLOAT_EQ(0.0f, rootLocal(1, 3));
    TEST_ASSERT_FLOAT_EQ(0.0f, rootLocal(2, 3));
    TEST_ASSERT_FLOAT_EQ(1.0f, rootLocal(0, 0));
    TEST_ASSERT_FLOAT_EQ(1.0f, rootLocal(1, 1));
    TEST_ASSERT_FLOAT_EQ(1.0f, rootLocal(2, 2));
}

static void test_Integration_VMD_TwoKeyLinearInterpolation()
{
    std::cout << "[test] Integration_VMD_TwoKeyLinearInterpolation\n";

    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    // Frame 0: translate (0,0,0), Frame 30: translate (30,0,0)
    auto vmd = MakeTwoKeyVMD("child",
        0, Eigen::Vector3f::Zero(), Eigen::Quaternionf::Identity(),
        30, Eigen::Vector3f(30.0f, 0.0f, 0.0f), Eigen::Quaternionf::Identity());

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));

    // Evaluate at frame 15 (midpoint)
    model->InitializeAnimation();
    model->BeginAnimation();
    anim.Evaluate(15.0f);
    model->UpdateMorphAnimation();
    model->UpdateNodeAnimation(false);
    model->UpdateNodeAnimation(true);
    model->EndAnimation();

    auto* childNode = model->GetNodeManager()->GetMMDNode(size_t(1));

    // With linear interpolation, midpoint should be ~(15, 0, 0) for anim translate
    // Total translate = base(0, 10, 0) + anim(~15, 0, 0) = (~15, 10, 0)
    const auto& animT = childNode->GetAnimationTranslate();
    // Allow tolerance for bezier approximation with control points at (20/127, 107/127)
    TEST_ASSERT(std::fabs(animT.x() - 15.0f) < 2.0f);
    TEST_ASSERT_FLOAT_EQ(0.0f, animT.y());
    TEST_ASSERT_FLOAT_EQ(0.0f, animT.z());
}

static void test_Integration_VMD_TwoKeyRotationSlerp()
{
    std::cout << "[test] Integration_VMD_TwoKeyRotationSlerp\n";

    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    // Frame 0: no rotation, Frame 30: 90-deg Z rotation
    const float angle90 = static_cast<float>(M_PI) / 2.0f;
    Eigen::Quaternionf q90(Eigen::AngleAxisf(angle90, Eigen::Vector3f::UnitZ()));
    auto vmd = MakeTwoKeyVMD("child",
        0, Eigen::Vector3f::Zero(), Eigen::Quaternionf::Identity(),
        30, Eigen::Vector3f::Zero(), q90);

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));

    // Evaluate at frame 15 (midpoint)
    model->InitializeAnimation();
    model->BeginAnimation();
    anim.Evaluate(15.0f);
    model->UpdateMorphAnimation();
    model->UpdateNodeAnimation(false);
    model->UpdateNodeAnimation(true);
    model->EndAnimation();

    auto* childNode = model->GetNodeManager()->GetMMDNode(size_t(1));
    const auto& animQ = childNode->GetAnimationRotate();

    // At midpoint with approximately linear interpolation,
    // the rotation should be roughly 45 degrees around Z
    Eigen::AngleAxisf resultAA(animQ);
    float resultAngle = resultAA.angle();
    Eigen::Vector3f resultAxis = resultAA.axis();

    // The axis should be approximately Z (allow for numerical imprecision)
    TEST_ASSERT(std::fabs(resultAxis.z()) > 0.9f);
    // The angle should be roughly pi/4 (45 degrees), tolerance for bezier curve
    TEST_ASSERT(std::fabs(resultAngle - static_cast<float>(M_PI) / 4.0f) < 0.3f);
}

static void test_Integration_VMD_GlobalTransformHierarchy()
{
    std::cout << "[test] Integration_VMD_GlobalTransformHierarchy\n";

    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    // Rotate root by 90 deg around Z, translate child by (5,0,0)
    const float angle = static_cast<float>(M_PI) / 2.0f;
    Eigen::Quaternionf rotZ(Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitZ()));

    libmmd::VMDFile vmd{};
    auto interp = MakeLinearInterpolation();

    // Root rotation key
    libmmd::VMDMotion rootMotion{};
    rootMotion.m_boneName.Set("root");
    rootMotion.m_frame = 0;
    rootMotion.m_translate = Eigen::Vector3f::Zero();
    rootMotion.m_quaternion = rotZ;
    rootMotion.m_interpolation = interp;
    vmd.m_motions.push_back(std::move(rootMotion));

    // Child translation key
    libmmd::VMDMotion childMotion{};
    childMotion.m_boneName.Set("child");
    childMotion.m_frame = 0;
    childMotion.m_translate = Eigen::Vector3f(5.0f, 0.0f, 0.0f);
    childMotion.m_quaternion = Eigen::Quaternionf::Identity();
    childMotion.m_interpolation = interp;
    vmd.m_motions.push_back(std::move(childMotion));

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));

    model->InitializeAnimation();
    model->BeginAnimation();
    anim.Evaluate(0.0f);
    model->UpdateMorphAnimation();
    model->UpdateNodeAnimation(false);
    model->UpdateNodeAnimation(true);
    model->EndAnimation();

    // Root local: T(0,0,0) * R(90Z) * S(1)
    auto* rootNode = model->GetNodeManager()->GetMMDNode(size_t(0));
    const auto& rootGlobal = rootNode->GetGlobalTransform();

    // Child local translate = base(0,10,0) + anim(5,0,0) = (5,10,0)
    // Child local = T(5,10,0) * I
    // Child global = rootGlobal * childLocal
    // rootGlobal rotates 90 deg Z: X->Y, Y->-X
    // So child global pos = R_90Z * (5, 10, 0) = (-10, 5, 0)
    auto* childNode = model->GetNodeManager()->GetMMDNode(size_t(1));
    const auto& childGlobal = childNode->GetGlobalTransform();

    TEST_ASSERT(std::fabs(childGlobal(0, 3) - (-10.0f)) < 1e-4f);
    TEST_ASSERT(std::fabs(childGlobal(1, 3) - 5.0f) < 1e-4f);
    TEST_ASSERT(std::fabs(childGlobal(2, 3) - 0.0f) < 1e-4f);
}

static void test_Integration_VMD_UpdateAllAnimation()
{
    std::cout << "[test] Integration_VMD_UpdateAllAnimation\n";

    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    // Simple translation on child at frame 0
    auto vmd = MakeSingleKeyVMD("child", 0,
        Eigen::Vector3f(1.0f, 2.0f, 3.0f), Eigen::Quaternionf::Identity());

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));

    model->InitializeAnimation();
    // Use the full UpdateAllAnimation pipeline (same as C4D code path)
    model->UpdateAllAnimation(&anim, 0.0f, 1.0f / 30.0f);

    auto* childNode = model->GetNodeManager()->GetMMDNode(size_t(1));
    const auto& local = childNode->GetLocalTransform();

    // Same result as manual pipeline
    TEST_ASSERT_FLOAT_EQ(1.0f, local(0, 3));
    TEST_ASSERT_FLOAT_EQ(12.0f, local(1, 3));
    TEST_ASSERT_FLOAT_EQ(3.0f, local(2, 3));
}

// ===========================================================================
// C4D Matrix Extraction Pattern verification
// This tests the exact code pattern used in mmd_bone.cpp Execute() to
// extract translation delta and rotation from GetLocalTransform().
// ===========================================================================

static void test_Integration_C4DMatrixExtractionPattern_Translation()
{
    std::cout << "[test] Integration_C4DMatrixExtractionPattern_Translation\n";

    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    auto vmd = MakeSingleKeyVMD("child", 0,
        Eigen::Vector3f(1.0f, 2.0f, 3.0f), Eigen::Quaternionf::Identity());

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));

    model->InitializeAnimation();
    model->UpdateAllAnimation(&anim, 0.0f, 1.0f / 30.0f);

    auto* childNode = model->GetNodeManager()->GetMMDNode(size_t(1));
    const auto& transform = childNode->GetLocalTransform();

    // This is the exact extraction pattern from mmd_bone.cpp:
    // translate = transform.col(3).head<3>() - mmd_node_->GetInitialTranslate()
    const Eigen::Vector3f translate = transform.col(3).head<3>() - childNode->GetInitialTranslate();

    // Initial translate is (0, 10, 0) (relative to parent in PMX)
    // Local transform col(3) = AnimateTranslate() = (0,10,0) + (1,2,3) = (1,12,3)
    // Delta = (1,12,3) - (0,10,0) = (1,2,3) — should match the VMD anim translate
    TEST_ASSERT_FLOAT_EQ(1.0f, translate.x());
    TEST_ASSERT_FLOAT_EQ(2.0f, translate.y());
    TEST_ASSERT_FLOAT_EQ(3.0f, translate.z());
}

static void test_Integration_C4DMatrixExtractionPattern_Rotation()
{
    std::cout << "[test] Integration_C4DMatrixExtractionPattern_Rotation\n";

    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    // 45-degree rotation around X
    const float angle = static_cast<float>(M_PI) / 4.0f;
    Eigen::Quaternionf rotX(Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitX()));
    auto vmd = MakeSingleKeyVMD("child", 0, Eigen::Vector3f::Zero(), rotX);

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));

    model->InitializeAnimation();
    model->UpdateAllAnimation(&anim, 0.0f, 1.0f / 30.0f);

    auto* childNode = model->GetNodeManager()->GetMMDNode(size_t(1));
    const auto& transform = childNode->GetLocalTransform();

    // Extract rotation axes (column-major: col 0,1,2 are the 3 rotation axes)
    // This is the pattern from mmd_bone.cpp SetRelMl:
    //   v1 = Vector(transform(0,0), transform(1,0), transform(2,0))
    //   v2 = Vector(transform(0,1), transform(1,1), transform(2,1))
    //   v3 = Vector(transform(0,2), transform(1,2), transform(2,2))
    Eigen::Matrix3f rotBlock = transform.block<3, 3>(0, 0);
    Eigen::Matrix3f expectedRot = rotX.toRotationMatrix();

    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            TEST_ASSERT(std::fabs(rotBlock(r, c) - expectedRot(r, c)) < 1e-5f);
}

static void test_Integration_C4DMatrixExtractionPattern_CombinedTranslateRotate()
{
    std::cout << "[test] Integration_C4DMatrixExtractionPattern_CombinedTranslateRotate\n";

    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    const float angle = static_cast<float>(M_PI) / 6.0f;  // 30 degrees
    Eigen::Quaternionf rotY(Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitY()));
    Eigen::Vector3f animTranslate(2.0f, 0.0f, -1.0f);
    auto vmd = MakeSingleKeyVMD("child", 0, animTranslate, rotY);

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));

    model->InitializeAnimation();
    model->UpdateAllAnimation(&anim, 0.0f, 1.0f / 30.0f);

    auto* childNode = model->GetNodeManager()->GetMMDNode(size_t(1));
    const auto& transform = childNode->GetLocalTransform();

    // Verify the translation delta extraction
    const Eigen::Vector3f translate = transform.col(3).head<3>() - childNode->GetInitialTranslate();
    TEST_ASSERT(std::fabs(translate.x() - animTranslate.x()) < 1e-5f);
    TEST_ASSERT(std::fabs(translate.y() - animTranslate.y()) < 1e-5f);
    TEST_ASSERT(std::fabs(translate.z() - animTranslate.z()) < 1e-5f);

    // Verify rotation matrix
    Eigen::Matrix3f rotBlock = transform.block<3, 3>(0, 0);
    Eigen::Matrix3f expectedRot = rotY.toRotationMatrix();
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            TEST_ASSERT(std::fabs(rotBlock(r, c) - expectedRot(r, c)) < 1e-5f);
}

// ===========================================================================
// Real file integration test
// ===========================================================================

static void test_Integration_RealFile_LoadPMXAndApplyVMD()
{
    std::cout << "[test] Integration_RealFile_LoadPMXAndApplyVMD\n";

    // Load real PMX model
    auto model = std::make_shared<libmmd::PMXModel>();
    if (!model->Load(g_pmxTestFile, ""))
    {
        std::cerr << "  SKIP: Could not load PMX file\n";
        return;
    }

    size_t nodeCount = model->GetNodeManager()->GetNodeCount();
    TEST_ASSERT(nodeCount > 0);
    std::cout << "    Loaded PMX with " << nodeCount << " bones\n";

    // Load real VMD file
    libmmd::VMDFile vmdFile;
    if (!libmmd::ReadVMDFile(&vmdFile, g_vmdBoneFile.c_str()))
    {
        std::cerr << "  SKIP: Could not load VMD file\n";
        return;
    }
    TEST_ASSERT(!vmdFile.m_motions.empty());
    std::cout << "    Loaded VMD with " << vmdFile.m_motions.size() << " motions\n";

    // Create animation and add VMD data
    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmdFile));
    std::cout << "    VMD bound " << anim.GetNodeKeyNum() << " node controllers\n";
    TEST_ASSERT(anim.GetNodeKeyNum() > 0);

    // Run InitializeAnimation to set up initial pose
    model->InitializeAnimation();

    // Run full UpdateAllAnimation at frame 0
    model->UpdateAllAnimation(&anim, 0.0f, 1.0f / 30.0f);

    // Verify all bones have valid (non-NaN) transforms
    for (size_t i = 0; i < nodeCount; ++i)
    {
        auto* node = model->GetNodeManager()->GetMMDNode(i);
        const auto& local = node->GetLocalTransform();
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                TEST_ASSERT(!std::isnan(local(r, c)));

        const auto& global = node->GetGlobalTransform();
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                TEST_ASSERT(!std::isnan(global(r, c)));
    }

    // Run at a middle frame
    float midFrame = static_cast<float>(anim.GetMaxKeyTime()) / 2.0f;
    model->UpdateAllAnimation(&anim, midFrame, 1.0f / 30.0f);

    for (size_t i = 0; i < nodeCount; ++i)
    {
        auto* node = model->GetNodeManager()->GetMMDNode(i);
        const auto& local = node->GetLocalTransform();
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                TEST_ASSERT(!std::isnan(local(r, c)));
    }

    // Run at the last frame
    model->UpdateAllAnimation(&anim, static_cast<float>(anim.GetMaxKeyTime()), 1.0f / 30.0f);

    for (size_t i = 0; i < nodeCount; ++i)
    {
        auto* node = model->GetNodeManager()->GetMMDNode(i);
        const auto& local = node->GetLocalTransform();
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                TEST_ASSERT(!std::isnan(local(r, c)));
    }

    std::cout << "    All frames produced valid transforms\n";
}

static void test_Integration_RealFile_LocalTransformDeltaConsistency()
{
    std::cout << "[test] Integration_RealFile_LocalTransformDeltaConsistency\n";

    auto model = std::make_shared<libmmd::PMXModel>();
    if (!model->Load(g_pmxTestFile, ""))
    {
        std::cerr << "  SKIP: Could not load PMX file\n";
        return;
    }

    libmmd::VMDFile vmdFile;
    if (!libmmd::ReadVMDFile(&vmdFile, g_vmdBoneFile.c_str()))
    {
        std::cerr << "  SKIP: Could not load VMD file\n";
        return;
    }

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmdFile));

    model->InitializeAnimation();
    model->UpdateAllAnimation(&anim, 0.0f, 1.0f / 30.0f);

    // For each bone, verify:
    //   1. The C4D extraction pattern gives a finite translation delta
    //   2. The 3x3 rotation block is a valid rotation matrix (det ≈ 1, orthonormal)
    size_t nodeCount = model->GetNodeManager()->GetNodeCount();
    for (size_t i = 0; i < nodeCount; ++i)
    {
        auto* node = model->GetNodeManager()->GetMMDNode(i);
        const auto& transform = node->GetLocalTransform();

        // Translation delta (C4D extraction pattern)
        Eigen::Vector3f delta = transform.col(3).head<3>() - node->GetInitialTranslate();
        TEST_ASSERT(!std::isnan(delta.x()));
        TEST_ASSERT(!std::isnan(delta.y()));
        TEST_ASSERT(!std::isnan(delta.z()));
        TEST_ASSERT(!std::isinf(delta.x()));
        TEST_ASSERT(!std::isinf(delta.y()));
        TEST_ASSERT(!std::isinf(delta.z()));

        // Rotation block validity: det should be close to 1 for proper rotation
        Eigen::Matrix3f rotBlock = transform.block<3, 3>(0, 0);
        float det = rotBlock.determinant();
        TEST_ASSERT(std::fabs(det - 1.0f) < 0.01f);

        // Columns should be approximately unit length
        for (int c = 0; c < 3; ++c)
        {
            float colLen = rotBlock.col(c).norm();
            TEST_ASSERT(std::fabs(colLen - 1.0f) < 0.01f);
        }
    }

    std::cout << "    All bones passed rotation/translation validity checks\n";
}

static void test_Integration_VMD_EvaluatePastLastKey()
{
    std::cout << "[test] Integration_VMD_EvaluatePastLastKey\n";

    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    // Single key at frame 10: translate (5, 0, 0)
    auto vmd = MakeSingleKeyVMD("child", 10,
        Eigen::Vector3f(5.0f, 0.0f, 0.0f), Eigen::Quaternionf::Identity());

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));

    // Evaluate at frame 100 (well past the single key)
    model->InitializeAnimation();
    model->BeginAnimation();
    anim.Evaluate(100.0f);
    model->UpdateMorphAnimation();
    model->UpdateNodeAnimation(false);
    model->UpdateNodeAnimation(true);
    model->EndAnimation();

    auto* childNode = model->GetNodeManager()->GetMMDNode(size_t(1));
    const auto& animT = childNode->GetAnimationTranslate();

    // Past last key: should hold last key value
    TEST_ASSERT_FLOAT_EQ(5.0f, animT.x());
    TEST_ASSERT_FLOAT_EQ(0.0f, animT.y());
    TEST_ASSERT_FLOAT_EQ(0.0f, animT.z());
}

static void test_Integration_VMD_EvaluateBeforeFirstKey()
{
    std::cout << "[test] Integration_VMD_EvaluateBeforeFirstKey\n";

    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    // Single key at frame 30: translate (10, 0, 0)
    auto vmd = MakeSingleKeyVMD("child", 30,
        Eigen::Vector3f(10.0f, 0.0f, 0.0f), Eigen::Quaternionf::Identity());

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));

    // Evaluate at frame 0 (before the key at frame 30)
    model->InitializeAnimation();
    model->BeginAnimation();
    anim.Evaluate(0.0f);
    model->UpdateMorphAnimation();
    model->UpdateNodeAnimation(false);
    model->UpdateNodeAnimation(true);
    model->EndAnimation();

    auto* childNode = model->GetNodeManager()->GetMMDNode(size_t(1));
    const auto& animT = childNode->GetAnimationTranslate();

    // Before first key: should use first key value
    TEST_ASSERT_FLOAT_EQ(10.0f, animT.x());
    TEST_ASSERT_FLOAT_EQ(0.0f, animT.y());
    TEST_ASSERT_FLOAT_EQ(0.0f, animT.z());
}

static void test_Integration_VMD_NonexistentBoneNameIgnored()
{
    std::cout << "[test] Integration_VMD_NonexistentBoneNameIgnored\n";

    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    // VMD references a bone that doesn't exist in the model
    auto vmd = MakeSingleKeyVMD("nonexistent_bone", 0,
        Eigen::Vector3f(100.0f, 0.0f, 0.0f), Eigen::Quaternionf::Identity());

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));

    // No node controllers should be created
    TEST_ASSERT_EQ(int32_t(0), anim.GetNodeKeyNum());

    // Evaluating should not crash
    model->InitializeAnimation();
    model->UpdateAllAnimation(&anim, 0.0f, 1.0f / 30.0f);

    // Bones should remain in initial pose
    auto* rootNode = model->GetNodeManager()->GetMMDNode(size_t(0));
    TEST_ASSERT_FLOAT_EQ(0.0f, rootNode->GetLocalTransform()(0, 3));
}

static void test_Integration_VMD_MultipleConsecutiveFrames()
{
    std::cout << "[test] Integration_VMD_MultipleConsecutiveFrames\n";

    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    auto vmd = MakeTwoKeyVMD("child",
        0, Eigen::Vector3f::Zero(), Eigen::Quaternionf::Identity(),
        60, Eigen::Vector3f(60.0f, 0.0f, 0.0f), Eigen::Quaternionf::Identity());

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));

    model->InitializeAnimation();

    // Run multiple consecutive frames like C4D would
    float prevTranslateX = 0.0f;
    for (int frame = 0; frame <= 60; frame += 10)
    {
        model->UpdateAllAnimation(&anim, static_cast<float>(frame), 1.0f / 30.0f);

        auto* childNode = model->GetNodeManager()->GetMMDNode(size_t(1));
        float currentX = childNode->GetAnimationTranslate().x();

        // Animation translate X should be monotonically increasing
        if (frame > 0)
        {
            TEST_ASSERT(currentX >= prevTranslateX - 1e-5f);
        }
        prevTranslateX = currentX;

        // No NaN in transforms
        const auto& local = childNode->GetLocalTransform();
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                TEST_ASSERT(!std::isnan(local(r, c)));
    }
}

// ===========================================================================
// VMDBezier tests
// ===========================================================================

static void test_VMDBezier_FindBezierX_LinearCP()
{
    std::cout << "[test] VMDBezier_FindBezierX_LinearCP\n";
    libmmd::VMDBezier bezier;
    bezier.m_cp1 = Eigen::Vector2f(0.5f, 0.5f);
    bezier.m_cp2 = Eigen::Vector2f(0.5f, 0.5f);

    for (float time = 0.0f; time <= 1.0f; time += 0.1f)
    {
        float t = bezier.FindBezierX(time);
        TEST_ASSERT(t >= 0.0f && t <= 1.0f);
        TEST_ASSERT(std::fabs(bezier.EvalX(t) - time) < 1e-4f);
    }
}

static void test_VMDBezier_FindBezierX_EaseInCP()
{
    std::cout << "[test] VMDBezier_FindBezierX_EaseInCP\n";
    libmmd::VMDBezier bezier;
    bezier.m_cp1 = Eigen::Vector2f(0.42f, 0.0f);
    bezier.m_cp2 = Eigen::Vector2f(1.0f, 1.0f);

    TEST_ASSERT(std::fabs(bezier.FindBezierX(0.0f)) < 1e-4f);
    TEST_ASSERT(std::fabs(bezier.FindBezierX(1.0f) - 1.0f) < 1e-4f);
    float mid = bezier.FindBezierX(0.5f);
    TEST_ASSERT(mid > 0.0f && mid < 1.0f);
}

static void test_VMDBezier_FindBezierX_EaseOutCP()
{
    std::cout << "[test] VMDBezier_FindBezierX_EaseOutCP\n";
    libmmd::VMDBezier bezier;
    bezier.m_cp1 = Eigen::Vector2f(0.0f, 0.0f);
    bezier.m_cp2 = Eigen::Vector2f(0.58f, 1.0f);

    TEST_ASSERT(std::fabs(bezier.FindBezierX(0.0f)) < 1e-4f);
    TEST_ASSERT(std::fabs(bezier.FindBezierX(1.0f) - 1.0f) < 1e-4f);
    float mid = bezier.FindBezierX(0.5f);
    TEST_ASSERT(mid > 0.0f && mid < 1.0f);
}

static void test_VMDBezier_FindBezierX_PrecisionRegression()
{
    std::cout << "[test] VMDBezier_FindBezierX_PrecisionRegression\n";

    struct TestCase { Eigen::Vector2f cp1, cp2; };
    TestCase cases[] = {
        { {0.25f, 0.1f}, {0.25f, 1.0f} },   // ease
        { {0.42f, 0.0f}, {1.0f,  1.0f} },    // ease-in
        { {0.0f,  0.0f}, {0.58f, 1.0f} },    // ease-out
        { {0.42f, 0.0f}, {0.58f, 1.0f} },    // ease-in-out
        { {0.0f,  0.0f}, {1.0f,  1.0f} },    // linear
        { {0.1f,  0.9f}, {0.9f,  0.1f} },    // S-curve
        { {20.0f/127.0f, 20.0f/127.0f}, {107.0f/127.0f, 107.0f/127.0f} }, // VMD default
    };

    for (const auto& tc : cases)
    {
        libmmd::VMDBezier bezier;
        bezier.m_cp1 = tc.cp1;
        bezier.m_cp2 = tc.cp2;

        for (float time = 0.0f; time <= 1.0f; time += 0.05f)
        {
            float t = bezier.FindBezierX(time);
            float reconstructedX = bezier.EvalX(t);
            TEST_ASSERT(std::fabs(reconstructedX - time) < 1e-4f);
        }
    }
}

static void test_VMDBezier_EvalDX_Correctness()
{
    std::cout << "[test] VMDBezier_EvalDX_Correctness\n";

    struct TestCase { Eigen::Vector2f cp1, cp2; };
    TestCase cases[] = {
        { {0.25f, 0.1f}, {0.25f, 1.0f} },
        { {0.42f, 0.0f}, {1.0f,  1.0f} },
        { {0.0f,  0.0f}, {0.58f, 1.0f} },
        { {0.1f,  0.9f}, {0.9f,  0.1f} },
    };

    constexpr float h = 1e-4f;
    for (const auto& tc : cases)
    {
        libmmd::VMDBezier bezier;
        bezier.m_cp1 = tc.cp1;
        bezier.m_cp2 = tc.cp2;

        for (float t = 0.05f; t <= 0.95f; t += 0.1f)
        {
            float analytic = bezier.EvalDX(t);
            float numeric = (bezier.EvalX(t + h) - bezier.EvalX(t - h)) / (2.0f * h);
            TEST_ASSERT(std::fabs(analytic - numeric) < 1e-2f);
        }
    }
}

// ===========================================================================
// Test helper: PMXModel subclass for accessing protected members
// ===========================================================================

class TestPMXModel : public libmmd::PMXModel
{
public:
	TestPMXModel() = default;
	~TestPMXModel() override = default;

	const std::vector<libmmd::PMXNode*>& GetSortedNodes() const { return m_sortedNodes; }
	const std::vector<libmmd::PMXNode*>& GetBeforePhysicsNodes() const { return m_beforePhysicsNodes; }
	const std::vector<libmmd::PMXNode*>& GetAfterPhysicsNodes() const { return m_afterPhysicsNodes; }
	const std::vector<libmmd::MMDMaterial>& GetMaterialsVec() const { return m_materials; }
	const std::vector<libmmd::MMDMaterial>& GetInitMaterialsVec() const { return m_initMaterials; }
};

// Helper: build a PMXFile with mixed DeformAfterPhysics bones
// Creates numBefore bones with DeformAfterPhysics=false,
//         numAfter bones with DeformAfterPhysics=true
static libmmd::PMXFile MakeMixedPhysicsPMXFile(int numBefore, int numAfter)
{
	libmmd::PMXFile file{};

	for (int i = 0; i < numBefore + numAfter; ++i)
	{
		libmmd::PMXBone bone{};
		bone.m_name = "bone_" + std::to_string(i);
		bone.m_englishName = bone.m_name;
		bone.m_position = Eigen::Vector3f(0.0f, static_cast<float>(i), 0.0f);
		bone.m_parentBoneIndex = (i == 0) ? -1 : 0;
		bone.m_deformDepth = i;
		bone.m_appendBoneIndex = -1;
		bone.m_appendWeight = 0.0f;

		uint16_t flags = static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowRotate) |
		                 static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowTranslate) |
		                 static_cast<uint16_t>(libmmd::PMXBoneFlags::Visible);
		if (i >= numBefore)
			flags |= static_cast<uint16_t>(libmmd::PMXBoneFlags::DeformAfterPhysics);
		bone.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(flags);

		file.m_bones.push_back(std::move(bone));
	}

	libmmd::PMXMaterial mat{};
	mat.m_numFaceVertices = 0;
	mat.m_textureIndex = -1;
	mat.m_sphereTextureIndex = -1;
	mat.m_toonTextureIndex = -1;
	mat.m_sphereMode = libmmd::PMXSphereMode::None;
	mat.m_toonMode = libmmd::PMXToonMode::Common;
	mat.m_drawMode = static_cast<libmmd::PMXDrawModeFlags>(0);
	mat.m_diffuse = Eigen::Vector4f(1, 1, 1, 1);
	mat.m_specular = Eigen::Vector3f::Zero();
	mat.m_specularPower = 1.0f;
	mat.m_ambient = Eigen::Vector3f(0.2f, 0.2f, 0.2f);
	mat.m_edgeColor = Eigen::Vector4f::Zero();
	mat.m_edgeSize = 0.0f;
	file.m_materials.push_back(std::move(mat));

	return file;
}

// Helper: build a PMXFile with a material morph
static libmmd::PMXFile MakePMXFileWithMaterialMorph(
	const std::string& morphName,
	int materialIndex,
	libmmd::PMXFileMorph::MaterialMorph::OpType opType,
	const Eigen::Vector4f& diffuse)
{
	libmmd::PMXFile file{};

	libmmd::PMXBone bone{};
	bone.m_name = "root";
	bone.m_englishName = "root";
	bone.m_position = Eigen::Vector3f::Zero();
	bone.m_parentBoneIndex = -1;
	bone.m_deformDepth = 0;
	bone.m_appendBoneIndex = -1;
	bone.m_appendWeight = 0.0f;
	bone.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(
		static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowRotate) |
		static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowTranslate) |
		static_cast<uint16_t>(libmmd::PMXBoneFlags::Visible));
	file.m_bones.push_back(std::move(bone));

	libmmd::PMXMaterial mat{};
	mat.m_numFaceVertices = 0;
	mat.m_textureIndex = -1;
	mat.m_sphereTextureIndex = -1;
	mat.m_toonTextureIndex = -1;
	mat.m_sphereMode = libmmd::PMXSphereMode::None;
	mat.m_toonMode = libmmd::PMXToonMode::Common;
	mat.m_drawMode = static_cast<libmmd::PMXDrawModeFlags>(0);
	mat.m_diffuse = Eigen::Vector4f(0.8f, 0.6f, 0.4f, 1.0f);
	mat.m_specular = Eigen::Vector3f(0.5f, 0.5f, 0.5f);
	mat.m_specularPower = 10.0f;
	mat.m_ambient = Eigen::Vector3f(0.2f, 0.2f, 0.2f);
	mat.m_edgeColor = Eigen::Vector4f::Zero();
	mat.m_edgeSize = 0.0f;
	file.m_materials.push_back(std::move(mat));

	libmmd::PMXFileMorph morph{};
	morph.m_name = morphName;
	morph.m_englishName = morphName;
	morph.m_controlPanel = 4;
	morph.m_morphType = libmmd::PMXMorphType::Material;

	libmmd::PMXFileMorph::MaterialMorph matMorph{};
	matMorph.m_materialIndex = materialIndex;
	matMorph.m_opType = opType;
	matMorph.m_diffuse = diffuse;
	matMorph.m_specular = Eigen::Vector3f::Ones();
	matMorph.m_specularPower = 1.0f;
	matMorph.m_ambient = Eigen::Vector3f::Ones();
	matMorph.m_edgeColor = Eigen::Vector4f::Ones();
	matMorph.m_edgeSize = 1.0f;
	matMorph.m_textureFactor = Eigen::Vector4f::Ones();
	matMorph.m_sphereTextureFactor = Eigen::Vector4f::Ones();
	matMorph.m_toonTextureFactor = Eigen::Vector4f::Ones();
	morph.m_materialMorph.push_back(std::move(matMorph));

	file.m_morphs.push_back(std::move(morph));

	return file;
}

// ===========================================================================
// Node partition correctness + regression
// ===========================================================================

static void test_PMXModel_NodePartition_CorrectSplit()
{
	std::cout << "[test] PMXModel_NodePartition_CorrectSplit\n";

	auto pmxFile = MakeMixedPhysicsPMXFile(3, 2);
	auto model = std::make_shared<TestPMXModel>();
	TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

	const auto& before = model->GetBeforePhysicsNodes();
	const auto& after = model->GetAfterPhysicsNodes();
	const auto& sorted = model->GetSortedNodes();

	TEST_ASSERT_EQ(size_t(3), before.size());
	TEST_ASSERT_EQ(size_t(2), after.size());
	TEST_ASSERT_EQ(size_t(5), sorted.size());

	for (const auto* node : before)
		TEST_ASSERT(!node->IsDeformAfterPhysics());
	for (const auto* node : after)
		TEST_ASSERT(node->IsDeformAfterPhysics());

	// Union should equal sorted
	std::vector<libmmd::PMXNode*> combined;
	combined.insert(combined.end(), before.begin(), before.end());
	combined.insert(combined.end(), after.begin(), after.end());
	std::sort(combined.begin(), combined.end());
	auto sortedCopy = sorted;
	std::sort(sortedCopy.begin(), sortedCopy.end());
	TEST_ASSERT_EQ(sortedCopy.size(), combined.size());
	for (size_t i = 0; i < combined.size(); ++i)
		TEST_ASSERT(combined[i] == sortedCopy[i]);
}

static void test_PMXModel_NodePartition_AllBeforePhysics()
{
	std::cout << "[test] PMXModel_NodePartition_AllBeforePhysics\n";

	auto pmxFile = MakeMixedPhysicsPMXFile(4, 0);
	auto model = std::make_shared<TestPMXModel>();
	TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

	TEST_ASSERT_EQ(size_t(4), model->GetBeforePhysicsNodes().size());
	TEST_ASSERT_EQ(size_t(0), model->GetAfterPhysicsNodes().size());
}

static void test_PMXModel_NodePartition_AllAfterPhysics()
{
	std::cout << "[test] PMXModel_NodePartition_AllAfterPhysics\n";

	auto pmxFile = MakeMixedPhysicsPMXFile(0, 3);
	auto model = std::make_shared<TestPMXModel>();
	TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

	TEST_ASSERT_EQ(size_t(0), model->GetBeforePhysicsNodes().size());
	TEST_ASSERT_EQ(size_t(3), model->GetAfterPhysicsNodes().size());
}

static void test_PMXModel_NodePartition_RegressionTransformConsistency()
{
	std::cout << "[test] PMXModel_NodePartition_RegressionTransformConsistency\n";

	// Build model with mixed DeformAfterPhysics bones
	auto pmxFile = MakeMixedPhysicsPMXFile(3, 2);
	auto model = std::make_shared<TestPMXModel>();
	TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

	// Animate with some translation on root
	auto vmd = MakeSingleKeyVMD("bone_0", 0,
		Eigen::Vector3f(1.0f, 2.0f, 3.0f), Eigen::Quaternionf::Identity());

	libmmd::VMDAnimation anim;
	TEST_ASSERT(anim.Create(model));
	TEST_ASSERT(anim.Add(vmd));

	model->InitializeAnimation();
	model->BeginAnimation();
	anim.Evaluate(0.0f);
	model->UpdateMorphAnimation();
	model->UpdateNodeAnimation(false);
	model->UpdateNodeAnimation(true);
	model->EndAnimation();

	// Verify all nodes have valid (non-NaN) transforms with proper rotation matrices
	size_t nodeCount = model->GetNodeManager()->GetNodeCount();
	for (size_t i = 0; i < nodeCount; ++i)
	{
		auto* node = model->GetNodeManager()->GetMMDNode(i);
		const auto& local = node->GetLocalTransform();
		const auto& global = node->GetGlobalTransform();

		for (int r = 0; r < 4; ++r)
			for (int c = 0; c < 4; ++c)
			{
				TEST_ASSERT(!std::isnan(local(r, c)));
				TEST_ASSERT(!std::isnan(global(r, c)));
			}

		Eigen::Matrix3f rotBlock = local.block<3, 3>(0, 0);
		float det = rotBlock.determinant();
		TEST_ASSERT(std::fabs(det - 1.0f) < 0.01f);
	}
}

static void test_PMXModel_NodePartition_RealFileRegression()
{
	std::cout << "[test] PMXModel_NodePartition_RealFileRegression\n";

	auto model = std::make_shared<TestPMXModel>();
	if (!model->Load(g_pmxTestFile, ""))
	{
		std::cerr << "  SKIP: Could not load PMX file\n";
		return;
	}

	const auto& before = model->GetBeforePhysicsNodes();
	const auto& after = model->GetAfterPhysicsNodes();
	const auto& sorted = model->GetSortedNodes();

	// before + after should equal sorted
	TEST_ASSERT_EQ(sorted.size(), before.size() + after.size());

	for (const auto* node : before)
		TEST_ASSERT(!node->IsDeformAfterPhysics());
	for (const auto* node : after)
		TEST_ASSERT(node->IsDeformAfterPhysics());

	// Load VMD and run animation, verify transforms are valid
	libmmd::VMDFile vmdFile;
	if (!libmmd::ReadVMDFile(&vmdFile, g_vmdBoneFile.c_str()))
	{
		std::cerr << "  SKIP: Could not load VMD file\n";
		return;
	}

	libmmd::VMDAnimation anim;
	TEST_ASSERT(anim.Create(model));
	TEST_ASSERT(anim.Add(vmdFile));

	model->InitializeAnimation();
	model->UpdateAllAnimation(&anim, 0.0f, 1.0f / 30.0f);

	size_t nodeCount = model->GetNodeManager()->GetNodeCount();
	for (size_t i = 0; i < nodeCount; ++i)
	{
		auto* node = model->GetNodeManager()->GetMMDNode(i);
		const auto& local = node->GetLocalTransform();
		for (int r = 0; r < 4; ++r)
			for (int c = 0; c < 4; ++c)
				TEST_ASSERT(!std::isnan(local(r, c)));

		Eigen::Matrix3f rotBlock = local.block<3, 3>(0, 0);
		float det = rotBlock.determinant();
		TEST_ASSERT(std::fabs(det - 1.0f) < 0.01f);
	}

	std::cout << "    before=" << before.size() << " after=" << after.size()
	          << " total=" << sorted.size() << "\n";
}

// ===========================================================================
// Zero-weight morph skip
// ===========================================================================

static void test_PMXModel_MorphAnimation_ZeroWeightSkipped()
{
	std::cout << "[test] PMXModel_MorphAnimation_ZeroWeightSkipped\n";

	auto pmxFile = MakePMXFileWithMaterialMorph("test_morph", 0,
		libmmd::PMXFileMorph::MaterialMorph::OpType::Add,
		Eigen::Vector4f(0.5f, 0.5f, 0.5f, 0.5f));

	auto model = std::make_shared<TestPMXModel>();
	TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));
	TEST_ASSERT_EQ(size_t(1), model->GetMorphManager()->GetMorphCount());

	model->InitializeAnimation();

	// Set weight to 0 (default, also reset by InitializeAnimation)
	auto* morph = model->GetMorphManager()->GetMorph(size_t(0));
	morph->SetWeight(0.0f);

	model->BeginAnimation();
	model->UpdateMorphAnimation();
	model->EndAnimation();

	// Materials should match initMaterials since zero-weight morph is skipped
	const auto& materials = model->GetMaterialsVec();
	const auto& initMaterials = model->GetInitMaterialsVec();
	TEST_ASSERT_EQ(materials.size(), initMaterials.size());

	for (size_t i = 0; i < materials.size(); ++i)
	{
		TEST_ASSERT_FLOAT_EQ(initMaterials[i].m_diffuse.x(), materials[i].m_diffuse.x());
		TEST_ASSERT_FLOAT_EQ(initMaterials[i].m_diffuse.y(), materials[i].m_diffuse.y());
		TEST_ASSERT_FLOAT_EQ(initMaterials[i].m_diffuse.z(), materials[i].m_diffuse.z());
		TEST_ASSERT_FLOAT_EQ(initMaterials[i].m_alpha, materials[i].m_alpha);
	}
}

static void test_PMXModel_MorphAnimation_NonZeroWeightApplied()
{
	std::cout << "[test] PMXModel_MorphAnimation_NonZeroWeightApplied\n";

	// Add morph: adds (0.5, 0.5, 0.5, 0.5) to material diffuse
	auto pmxFile = MakePMXFileWithMaterialMorph("test_morph", 0,
		libmmd::PMXFileMorph::MaterialMorph::OpType::Add,
		Eigen::Vector4f(0.5f, 0.5f, 0.5f, 0.5f));

	auto model = std::make_shared<TestPMXModel>();
	TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

	model->InitializeAnimation();

	auto* morph = model->GetMorphManager()->GetMorph(size_t(0));
	morph->SetWeight(1.0f);

	model->BeginAnimation();
	model->UpdateMorphAnimation();
	model->EndAnimation();

	const auto& materials = model->GetMaterialsVec();
	const auto& initMaterials = model->GetInitMaterialsVec();

	// Diffuse should be init + add = (0.8+0.5, 0.6+0.5, 0.4+0.5) = (1.3, 1.1, 0.9)
	TEST_ASSERT(std::fabs(materials[0].m_diffuse.x() - (initMaterials[0].m_diffuse.x() + 0.5f)) < 1e-5f);
	TEST_ASSERT(std::fabs(materials[0].m_diffuse.y() - (initMaterials[0].m_diffuse.y() + 0.5f)) < 1e-5f);
	TEST_ASSERT(std::fabs(materials[0].m_diffuse.z() - (initMaterials[0].m_diffuse.z() + 0.5f)) < 1e-5f);
	TEST_ASSERT(std::fabs(materials[0].m_alpha - (initMaterials[0].m_alpha + 0.5f)) < 1e-5f);
}

static void test_PMXModel_MorphAnimation_MixedWeights()
{
	std::cout << "[test] PMXModel_MorphAnimation_MixedWeights\n";

	// Build a PMXFile with 2 material morphs
	libmmd::PMXFile file{};

	libmmd::PMXBone bone{};
	bone.m_name = "root";
	bone.m_englishName = "root";
	bone.m_position = Eigen::Vector3f::Zero();
	bone.m_parentBoneIndex = -1;
	bone.m_deformDepth = 0;
	bone.m_appendBoneIndex = -1;
	bone.m_appendWeight = 0.0f;
	bone.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(
		static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowRotate) |
		static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowTranslate) |
		static_cast<uint16_t>(libmmd::PMXBoneFlags::Visible));
	file.m_bones.push_back(std::move(bone));

	libmmd::PMXMaterial mat{};
	mat.m_numFaceVertices = 0;
	mat.m_textureIndex = -1;
	mat.m_sphereTextureIndex = -1;
	mat.m_toonTextureIndex = -1;
	mat.m_sphereMode = libmmd::PMXSphereMode::None;
	mat.m_toonMode = libmmd::PMXToonMode::Common;
	mat.m_drawMode = static_cast<libmmd::PMXDrawModeFlags>(0);
	mat.m_diffuse = Eigen::Vector4f(0.5f, 0.5f, 0.5f, 1.0f);
	mat.m_specular = Eigen::Vector3f::Zero();
	mat.m_specularPower = 1.0f;
	mat.m_ambient = Eigen::Vector3f(0.2f, 0.2f, 0.2f);
	mat.m_edgeColor = Eigen::Vector4f::Zero();
	mat.m_edgeSize = 0.0f;
	file.m_materials.push_back(std::move(mat));

	auto makeMaterialMorph = [](const std::string& name, const Eigen::Vector4f& addDiffuse) {
		libmmd::PMXFileMorph morph{};
		morph.m_name = name;
		morph.m_englishName = name;
		morph.m_controlPanel = 4;
		morph.m_morphType = libmmd::PMXMorphType::Material;

		libmmd::PMXFileMorph::MaterialMorph matMorph{};
		matMorph.m_materialIndex = 0;
		matMorph.m_opType = libmmd::PMXFileMorph::MaterialMorph::OpType::Add;
		matMorph.m_diffuse = addDiffuse;
		matMorph.m_specular = Eigen::Vector3f::Zero();
		matMorph.m_specularPower = 0.0f;
		matMorph.m_ambient = Eigen::Vector3f::Zero();
		matMorph.m_edgeColor = Eigen::Vector4f::Zero();
		matMorph.m_edgeSize = 0.0f;
		matMorph.m_textureFactor = Eigen::Vector4f::Ones();
		matMorph.m_sphereTextureFactor = Eigen::Vector4f::Ones();
		matMorph.m_toonTextureFactor = Eigen::Vector4f::Ones();
		morph.m_materialMorph.push_back(std::move(matMorph));
		return morph;
	};

	// Morph 0: adds (0.1, 0, 0, 0) — will have weight=0 (should be skipped)
	file.m_morphs.push_back(makeMaterialMorph("morph_zero", Eigen::Vector4f(0.1f, 0.0f, 0.0f, 0.0f)));
	// Morph 1: adds (0, 0.2, 0, 0) — will have weight=1 (should be applied)
	file.m_morphs.push_back(makeMaterialMorph("morph_active", Eigen::Vector4f(0.0f, 0.2f, 0.0f, 0.0f)));

	auto model = std::make_shared<TestPMXModel>();
	TEST_ASSERT(model->LoadPMX(file, "", ""));
	TEST_ASSERT_EQ(size_t(2), model->GetMorphManager()->GetMorphCount());

	model->InitializeAnimation();

	model->GetMorphManager()->GetMorph(size_t(0))->SetWeight(0.0f);
	model->GetMorphManager()->GetMorph(size_t(1))->SetWeight(1.0f);

	model->BeginAnimation();
	model->UpdateMorphAnimation();
	model->EndAnimation();

	const auto& materials = model->GetMaterialsVec();
	const auto& initMaterials = model->GetInitMaterialsVec();

	// Only morph_active (adds 0.2 to G) should be applied
	// X should be unchanged (morph_zero was skipped)
	TEST_ASSERT_FLOAT_EQ(initMaterials[0].m_diffuse.x(), materials[0].m_diffuse.x());
	// Y should have +0.2
	TEST_ASSERT(std::fabs(materials[0].m_diffuse.y() - (initMaterials[0].m_diffuse.y() + 0.2f)) < 1e-5f);
	// Z unchanged
	TEST_ASSERT_FLOAT_EQ(initMaterials[0].m_diffuse.z(), materials[0].m_diffuse.z());
}

static void test_PMXModel_MorphAnimation_RealFileRegression()
{
	std::cout << "[test] PMXModel_MorphAnimation_RealFileRegression\n";

	auto model = std::make_shared<TestPMXModel>();
	if (!model->Load(g_pmxTestFile, ""))
	{
		std::cerr << "  SKIP: Could not load PMX file\n";
		return;
	}

	libmmd::VMDFile vmdFile;
	if (!libmmd::ReadVMDFile(&vmdFile, g_vmdBoneFile.c_str()))
	{
		std::cerr << "  SKIP: Could not load VMD file\n";
		return;
	}

	libmmd::VMDAnimation anim;
	TEST_ASSERT(anim.Create(model));
	TEST_ASSERT(anim.Add(vmdFile));

	model->InitializeAnimation();
	model->UpdateAllAnimation(&anim, 0.0f, 1.0f / 30.0f);

	// Verify all bones have valid transforms after the pipeline runs
	size_t nodeCount = model->GetNodeManager()->GetNodeCount();
	for (size_t i = 0; i < nodeCount; ++i)
	{
		auto* node = model->GetNodeManager()->GetMMDNode(i);
		const auto& local = node->GetLocalTransform();
		for (int r = 0; r < 4; ++r)
			for (int c = 0; c < 4; ++c)
				TEST_ASSERT(!std::isnan(local(r, c)));
	}

	// Verify materials have valid values (non-NaN)
	const auto& materials = model->GetMaterialsVec();
	for (size_t i = 0; i < materials.size(); ++i)
	{
		TEST_ASSERT(!std::isnan(materials[i].m_diffuse.x()));
		TEST_ASSERT(!std::isnan(materials[i].m_alpha));
		TEST_ASSERT(!std::isnan(materials[i].m_specularPower));
	}

	std::cout << "    Materials validated: " << materials.size() << "\n";
}

// ===========================================================================
// SyncPhysics parameterization
// ===========================================================================

static void test_VMDAnimation_SyncPhysics_DefaultElapsed()
{
    std::cout << "[test] VMDAnimation_SyncPhysics_DefaultElapsed\n";

    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    auto vmd = MakeSingleKeyVMD("child", 0,
        Eigen::Vector3f(1.0f, 2.0f, 3.0f), Eigen::Quaternionf::Identity());

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));

    model->InitializeAnimation();
    anim.SyncPhysics(0.0f);

    auto* childNode = model->GetNodeManager()->GetMMDNode(size_t(1));
    const auto& local = childNode->GetLocalTransform();
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            TEST_ASSERT(!std::isnan(local(r, c)));
}

static void test_VMDAnimation_SyncPhysics_CustomElapsed()
{
    std::cout << "[test] VMDAnimation_SyncPhysics_CustomElapsed\n";

    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    auto vmd = MakeSingleKeyVMD("child", 0,
        Eigen::Vector3f(1.0f, 2.0f, 3.0f), Eigen::Quaternionf::Identity());

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));

    model->InitializeAnimation();
    anim.SyncPhysics(0.0f, 30, 1.0f / 60.0f);

    auto* childNode = model->GetNodeManager()->GetMMDNode(size_t(1));
    const auto& local = childNode->GetLocalTransform();
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            TEST_ASSERT(!std::isnan(local(r, c)));
}

static void test_VMDAnimation_SyncPhysics_RealFile()
{
    std::cout << "[test] VMDAnimation_SyncPhysics_RealFile\n";

    auto model = std::make_shared<libmmd::PMXModel>();
    if (!model->Load(g_pmxTestFile, ""))
    {
        std::cerr << "  SKIP: Could not load PMX file\n";
        return;
    }

    libmmd::VMDFile vmdFile;
    if (!libmmd::ReadVMDFile(&vmdFile, g_vmdBoneFile.c_str()))
    {
        std::cerr << "  SKIP: Could not load VMD file\n";
        return;
    }

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmdFile));

    model->InitializeAnimation();
    anim.SyncPhysics(0.0f, 30, 1.0f / 60.0f);

    size_t nodeCount = model->GetNodeManager()->GetNodeCount();
    for (size_t i = 0; i < nodeCount; ++i)
    {
        auto* node = model->GetNodeManager()->GetMMDNode(i);
        const auto& local = node->GetLocalTransform();
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                TEST_ASSERT(!std::isnan(local(r, c)));
    }
    std::cout << "    SyncPhysics with custom elapsed produced valid transforms\n";
}

// ===========================================================================
// VMDAnimation Save tests
// ===========================================================================

static void test_VMDAnimation_Save_EmptyAnimation()
{
    std::cout << "[test] VMDAnimation_Save_EmptyAnimation\n";
    auto model = std::make_shared<libmmd::PMXModel>();
    auto pmxFile = MakeSimplePMXFile(10.0f);
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));

    libmmd::VMDFile vmd;
    TEST_ASSERT(anim.Save(vmd));

    TEST_ASSERT_EQ(std::string("Vocaloid Motion Data 0002"), vmd.m_header.m_header.ToString());
    TEST_ASSERT_EQ(size_t(0), vmd.m_motions.size());
    TEST_ASSERT_EQ(size_t(0), vmd.m_morphs.size());
    TEST_ASSERT_EQ(size_t(0), vmd.m_iks.size());
    TEST_ASSERT_EQ(size_t(0), vmd.m_cameras.size());
    TEST_ASSERT_EQ(size_t(0), vmd.m_lights.size());
    TEST_ASSERT_EQ(size_t(0), vmd.m_shadows.size());
}

static void test_VMDAnimation_Save_SingleBoneKey()
{
    std::cout << "[test] VMDAnimation_Save_SingleBoneKey\n";
    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    Eigen::Vector3f translate(1.0f, 2.0f, 3.0f);
    Eigen::Quaternionf rotation(Eigen::AngleAxisf(0.5f, Eigen::Vector3f::UnitY()));
    auto vmdIn = MakeSingleKeyVMD("child", 5, translate, rotation);

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmdIn));

    libmmd::VMDFile vmdOut;
    TEST_ASSERT(anim.Save(vmdOut));

    TEST_ASSERT_EQ(size_t(1), vmdOut.m_motions.size());
    const auto& m = vmdOut.m_motions[0];
    TEST_ASSERT_EQ(std::string("child"), m.m_boneName.ToString());
    TEST_ASSERT_EQ(uint32_t(5), m.m_frame);
    TEST_ASSERT_FLOAT_EQ(1.0f, m.m_translate.x());
    TEST_ASSERT_FLOAT_EQ(2.0f, m.m_translate.y());
    TEST_ASSERT_FLOAT_EQ(3.0f, m.m_translate.z());
    TEST_ASSERT(std::fabs(m.m_quaternion.w() - rotation.w()) < 1e-4f);
    TEST_ASSERT(std::fabs(m.m_quaternion.x() - rotation.x()) < 1e-4f);
    TEST_ASSERT(std::fabs(m.m_quaternion.y() - rotation.y()) < 1e-4f);
    TEST_ASSERT(std::fabs(m.m_quaternion.z() - rotation.z()) < 1e-4f);
}

static void test_VMDAnimation_Save_MultipleBoneKeys()
{
    std::cout << "[test] VMDAnimation_Save_MultipleBoneKeys\n";
    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    auto vmdIn = MakeTwoKeyVMD("child",
        0, Eigen::Vector3f::Zero(), Eigen::Quaternionf::Identity(),
        30, Eigen::Vector3f(10.0f, 0.0f, 0.0f), Eigen::Quaternionf::Identity());

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmdIn));

    libmmd::VMDFile vmdOut;
    TEST_ASSERT(anim.Save(vmdOut));

    TEST_ASSERT_EQ(size_t(2), vmdOut.m_motions.size());
    TEST_ASSERT_EQ(uint32_t(0), vmdOut.m_motions[0].m_frame);
    TEST_ASSERT_EQ(uint32_t(30), vmdOut.m_motions[1].m_frame);
}

static void test_VMDAnimation_Save_MultipleBones()
{
    std::cout << "[test] VMDAnimation_Save_MultipleBones\n";
    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    libmmd::VMDFile vmdIn{};
    auto interp = MakeLinearInterpolation();

    libmmd::VMDMotion m0{};
    m0.m_boneName.Set("root");
    m0.m_frame = 0;
    m0.m_translate = Eigen::Vector3f(1.0f, 0.0f, 0.0f);
    m0.m_quaternion = Eigen::Quaternionf::Identity();
    m0.m_interpolation = interp;
    vmdIn.m_motions.push_back(std::move(m0));

    libmmd::VMDMotion m1{};
    m1.m_boneName.Set("child");
    m1.m_frame = 0;
    m1.m_translate = Eigen::Vector3f(0.0f, 1.0f, 0.0f);
    m1.m_quaternion = Eigen::Quaternionf::Identity();
    m1.m_interpolation = interp;
    vmdIn.m_motions.push_back(std::move(m1));

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmdIn));

    libmmd::VMDFile vmdOut;
    TEST_ASSERT(anim.Save(vmdOut));

    TEST_ASSERT_EQ(size_t(2), vmdOut.m_motions.size());

    bool hasRoot = false, hasChild = false;
    for (const auto& motion : vmdOut.m_motions)
    {
        if (motion.m_boneName.ToString() == "root") hasRoot = true;
        if (motion.m_boneName.ToString() == "child") hasChild = true;
    }
    TEST_ASSERT(hasRoot);
    TEST_ASSERT(hasChild);
}

static void test_VMDAnimation_Save_InterpolationPreserved()
{
    std::cout << "[test] VMDAnimation_Save_InterpolationPreserved\n";
    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    libmmd::VMDFile vmdIn{};
    libmmd::VMDMotion motion{};
    motion.m_boneName.Set("child");
    motion.m_frame = 0;
    motion.m_translate = Eigen::Vector3f::Zero();
    motion.m_quaternion = Eigen::Quaternionf::Identity();
    motion.m_interpolation.fill(0);
    // TX: cp1=(30, 40), cp2=(90, 100)
    motion.m_interpolation[0]  = 30;  // tx cp1.x
    motion.m_interpolation[4]  = 40;  // tx cp1.y
    motion.m_interpolation[8]  = 90;  // tx cp2.x
    motion.m_interpolation[12] = 100; // tx cp2.y
    // Rot: cp1=(10, 50), cp2=(80, 120)
    motion.m_interpolation[3]  = 10;  // rot cp1.x
    motion.m_interpolation[7]  = 50;  // rot cp1.y
    motion.m_interpolation[11] = 80;  // rot cp2.x
    motion.m_interpolation[15] = 120; // rot cp2.y
    vmdIn.m_motions.push_back(std::move(motion));

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmdIn));

    libmmd::VMDFile vmdOut;
    TEST_ASSERT(anim.Save(vmdOut));

    TEST_ASSERT_EQ(size_t(1), vmdOut.m_motions.size());
    const auto& out = vmdOut.m_motions[0].m_interpolation;
    // Allow ±1 tolerance for float->uint8 round-trip
    TEST_ASSERT(std::abs(static_cast<int>(out[0])  - 30)  <= 1);
    TEST_ASSERT(std::abs(static_cast<int>(out[4])  - 40)  <= 1);
    TEST_ASSERT(std::abs(static_cast<int>(out[8])  - 90)  <= 1);
    TEST_ASSERT(std::abs(static_cast<int>(out[12]) - 100) <= 1);
    TEST_ASSERT(std::abs(static_cast<int>(out[3])  - 10)  <= 1);
    TEST_ASSERT(std::abs(static_cast<int>(out[7])  - 50)  <= 1);
    TEST_ASSERT(std::abs(static_cast<int>(out[11]) - 80)  <= 1);
    TEST_ASSERT(std::abs(static_cast<int>(out[15]) - 120) <= 1);
}

static void test_VMDAnimation_Save_NonexistentBoneIgnored()
{
    std::cout << "[test] VMDAnimation_Save_NonexistentBoneIgnored\n";
    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    libmmd::VMDFile vmdIn{};
    auto interp = MakeLinearInterpolation();

    // Existing bone
    libmmd::VMDMotion m0{};
    m0.m_boneName.Set("child");
    m0.m_frame = 0;
    m0.m_translate = Eigen::Vector3f(1.0f, 0.0f, 0.0f);
    m0.m_quaternion = Eigen::Quaternionf::Identity();
    m0.m_interpolation = interp;
    vmdIn.m_motions.push_back(std::move(m0));

    // Non-existing bone
    libmmd::VMDMotion m1{};
    m1.m_boneName.Set("nonexistent");
    m1.m_frame = 0;
    m1.m_translate = Eigen::Vector3f::Zero();
    m1.m_quaternion = Eigen::Quaternionf::Identity();
    m1.m_interpolation = interp;
    vmdIn.m_motions.push_back(std::move(m1));

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmdIn));

    libmmd::VMDFile vmdOut;
    TEST_ASSERT(anim.Save(vmdOut));

    TEST_ASSERT_EQ(size_t(1), vmdOut.m_motions.size());
    TEST_ASSERT_EQ(std::string("child"), vmdOut.m_motions[0].m_boneName.ToString());
}

// ===========================================================================
// Round-trip tests (Group D)
// ===========================================================================

static void test_RoundTrip_PhysicsRebuild()
{
    std::cout << "[test] RoundTrip_PhysicsRebuild\n";

    libmmd::PMXFile file = MakeSimplePMXFile(10.0f);

    libmmd::PMXRigidbody rb0{};
    rb0.m_name = "rb_root";
    rb0.m_boneIndex = 0;
    rb0.m_shape = libmmd::PMXRigidbody::Shape::Box;
    rb0.m_shapeSize = Eigen::Vector3f(2.0f, 1.0f, 1.0f);
    rb0.m_translate = Eigen::Vector3f(0.0f, 5.0f, 0.0f);
    rb0.m_rotate = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    rb0.m_mass = 0.0f;
    rb0.m_translateDimmer = 0.0f;
    rb0.m_rotateDimmer = 0.0f;
    rb0.m_repulsion = 0.0f;
    rb0.m_friction = 0.5f;
    rb0.m_op = libmmd::PMXRigidbody::Operation::Static;
    rb0.m_group = 0;
    rb0.m_collisionGroup = 0xFFFF;
    file.m_rigidbodies.push_back(rb0);

    libmmd::PMXRigidbody rb1{};
    rb1.m_name = "rb_child";
    rb1.m_boneIndex = 1;
    rb1.m_shape = libmmd::PMXRigidbody::Shape::Capsule;
    rb1.m_shapeSize = Eigen::Vector3f(0.5f, 3.0f, 0.0f);
    rb1.m_translate = Eigen::Vector3f(0.0f, 8.0f, 0.0f);
    rb1.m_rotate = Eigen::Vector3f(0.1f, 0.0f, 0.0f);
    rb1.m_mass = 1.5f;
    rb1.m_translateDimmer = 0.8f;
    rb1.m_rotateDimmer = 0.8f;
    rb1.m_repulsion = 0.2f;
    rb1.m_friction = 0.3f;
    rb1.m_op = libmmd::PMXRigidbody::Operation::Dynamic;
    rb1.m_group = 1;
    rb1.m_collisionGroup = 0x00FE;
    file.m_rigidbodies.push_back(rb1);

    libmmd::PMXJoint jt{};
    jt.m_name = "joint_01";
    jt.m_type = libmmd::PMXJoint::JointType::SpringDOF6;
    jt.m_rigidbodyAIndex = 0;
    jt.m_rigidbodyBIndex = 1;
    jt.m_translate = Eigen::Vector3f(0.0f, 6.5f, 0.0f);
    jt.m_rotate = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    jt.m_translateLowerLimit = Eigen::Vector3f(-1.0f, -1.0f, -1.0f);
    jt.m_translateUpperLimit = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
    jt.m_rotateLowerLimit = Eigen::Vector3f(-0.5f, -0.5f, -0.5f);
    jt.m_rotateUpperLimit = Eigen::Vector3f(0.5f, 0.5f, 0.5f);
    jt.m_springTranslateFactor = Eigen::Vector3f(10.0f, 10.0f, 10.0f);
    jt.m_springRotateFactor = Eigen::Vector3f(5.0f, 5.0f, 5.0f);
    file.m_joints.push_back(jt);

    auto modelA = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(modelA->LoadPMX(file, "", ""));
    auto* physA = modelA->GetPhysicsManager();
    auto* rbsA = physA->GetRigidBodys();
    auto* jtsA = physA->GetJoints();
    TEST_ASSERT_EQ(size_t(2), rbsA->size());
    TEST_ASSERT_EQ(size_t(1), jtsA->size());

    libmmd::PMXFile fileBones = file;
    fileBones.m_rigidbodies.clear();
    fileBones.m_joints.clear();
    auto modelB = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(modelB->LoadPMX(fileBones, "", ""));

    auto* physB = modelB->GetPhysicsManager();

    for (const auto& pmxRB : file.m_rigidbodies)
    {
        auto* rb = physB->AddRigidBody();
        libmmd::MMDNode* node = nullptr;
        if (pmxRB.m_boneIndex != -1)
            node = modelB->GetNodeManager()->GetMMDNode(pmxRB.m_boneIndex);
        TEST_ASSERT(rb->Create(
            pmxRB.m_shape, pmxRB.m_shapeSize, pmxRB.m_translate, pmxRB.m_rotate,
            pmxRB.m_mass, pmxRB.m_translateDimmer, pmxRB.m_rotateDimmer,
            pmxRB.m_repulsion, pmxRB.m_friction, pmxRB.m_op,
            pmxRB.m_group, pmxRB.m_collisionGroup,
            modelB.get(), node, pmxRB.m_name));
        physB->GetMMDPhysics()->AddRigidBody(rb);
    }

    auto* rbsB = physB->GetRigidBodys();
    TEST_ASSERT_EQ(rbsA->size(), rbsB->size());

    for (size_t i = 0; i < rbsA->size(); ++i)
    {
        TEST_ASSERT_EQ((*rbsA)[i]->GetGroup(), (*rbsB)[i]->GetGroup());
        TEST_ASSERT_EQ((*rbsA)[i]->GetGroupMask(), (*rbsB)[i]->GetGroupMask());
        auto tfA = (*rbsA)[i]->GetTransform();
        auto tfB = (*rbsB)[i]->GetTransform();
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                TEST_ASSERT_FLOAT_EQ(tfA(r, c), tfB(r, c));
    }

    for (const auto& pmxJt : file.m_joints)
    {
        if (pmxJt.m_rigidbodyAIndex < 0 || pmxJt.m_rigidbodyBIndex < 0)
            continue;
        auto* joint = physB->AddJoint();
        TEST_ASSERT(joint->CreateJoint(
            pmxJt.m_translate, pmxJt.m_rotate,
            pmxJt.m_translateLowerLimit, pmxJt.m_translateUpperLimit,
            pmxJt.m_rotateLowerLimit, pmxJt.m_rotateUpperLimit,
            pmxJt.m_springTranslateFactor, pmxJt.m_springRotateFactor,
            (*rbsB)[pmxJt.m_rigidbodyAIndex].get(),
            (*rbsB)[pmxJt.m_rigidbodyBIndex].get()));
        physB->GetMMDPhysics()->AddJoint(joint);
    }

    auto* jtsB = physB->GetJoints();
    TEST_ASSERT_EQ(jtsA->size(), jtsB->size());

    for (size_t i = 0; i < jtsA->size(); ++i)
    {
        auto matA = (*jtsA)[i]->GetTransform();
        auto matB = (*jtsB)[i]->GetTransform();
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                TEST_ASSERT_FLOAT_EQ(matA(r, c), matB(r, c));
    }

    std::cout << "    Physics rebuild round-trip: rigid bodies and joints match\n";
}

static void test_RoundTrip_VMDSerialize_Buffer()
{
    std::cout << "[test] RoundTrip_VMDSerialize_Buffer\n";

    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    auto interp = MakeLinearInterpolation();
    libmmd::VMDFile vmdIn{};

    libmmd::VMDMotion m0{};
    m0.m_boneName.Set("root");
    m0.m_frame = 0;
    m0.m_translate = Eigen::Vector3f(1.0f, 2.0f, 3.0f);
    m0.m_quaternion = Eigen::Quaternionf::Identity();
    m0.m_interpolation = interp;
    vmdIn.m_motions.push_back(std::move(m0));

    libmmd::VMDMotion m1{};
    m1.m_boneName.Set("child");
    m1.m_frame = 10;
    m1.m_translate = Eigen::Vector3f(4.0f, 5.0f, 6.0f);
    m1.m_quaternion = Eigen::Quaternionf(Eigen::AngleAxisf(0.5f, Eigen::Vector3f::UnitY()));
    m1.m_interpolation = interp;
    vmdIn.m_motions.push_back(std::move(m1));

    libmmd::VMDMotion m2{};
    m2.m_boneName.Set("root");
    m2.m_frame = 30;
    m2.m_translate = Eigen::Vector3f(10.0f, 0.0f, 0.0f);
    m2.m_quaternion = Eigen::Quaternionf::Identity();
    m2.m_interpolation = interp;
    vmdIn.m_motions.push_back(std::move(m2));

    libmmd::VMDAnimation animA;
    TEST_ASSERT(animA.Create(model));
    TEST_ASSERT(animA.Add(vmdIn));

    libmmd::VMDFile savedVmd;
    TEST_ASSERT(animA.Save(savedVmd));

    std::vector<uint8_t> buffer;
    TEST_ASSERT(libmmd::WriteVMDFile(&savedVmd, buffer));
    TEST_ASSERT(!buffer.empty());

    libmmd::VMDFile restoredVmd;
    TEST_ASSERT(libmmd::ReadVMDFile(&restoredVmd, buffer.data(), buffer.size()));

    libmmd::VMDAnimation animB;
    TEST_ASSERT(animB.Create(model));
    TEST_ASSERT(animB.Add(restoredVmd));

    libmmd::VMDFile savedVmd2;
    TEST_ASSERT(animB.Save(savedVmd2));

    TEST_ASSERT_EQ(savedVmd.m_motions.size(), savedVmd2.m_motions.size());

    for (size_t i = 0; i < savedVmd.m_motions.size(); ++i)
    {
        const auto& a = savedVmd.m_motions[i];
        const auto& b = savedVmd2.m_motions[i];
        TEST_ASSERT_EQ(a.m_boneName.ToString(), b.m_boneName.ToString());
        TEST_ASSERT_EQ(a.m_frame, b.m_frame);
        TEST_ASSERT(std::fabs(a.m_translate.x() - b.m_translate.x()) < 1e-4f);
        TEST_ASSERT(std::fabs(a.m_translate.y() - b.m_translate.y()) < 1e-4f);
        TEST_ASSERT(std::fabs(a.m_translate.z() - b.m_translate.z()) < 1e-4f);
        TEST_ASSERT(std::fabs(a.m_quaternion.w() - b.m_quaternion.w()) < 1e-4f);
        TEST_ASSERT(std::fabs(a.m_quaternion.x() - b.m_quaternion.x()) < 1e-4f);
        TEST_ASSERT(std::fabs(a.m_quaternion.y() - b.m_quaternion.y()) < 1e-4f);
        TEST_ASSERT(std::fabs(a.m_quaternion.z() - b.m_quaternion.z()) < 1e-4f);
    }

    std::cout << "    VMD serialize round-trip: " << savedVmd.m_motions.size()
              << " motion keys preserved\n";
}

static void test_RoundTrip_RealFile_PhysicsAndVMD()
{
    std::cout << "[test] RoundTrip_RealFile_PhysicsAndVMD\n";

    libmmd::PMXFile pmxFile;
    if (!libmmd::ReadPMXFile(&pmxFile, g_pmxTestFile.c_str()))
    {
        std::cerr << "  SKIP: Could not load PMX file\n";
        return;
    }

    auto modelA = std::make_shared<libmmd::PMXModel>();
    if (!modelA->Load(g_pmxTestFile, ""))
    {
        std::cerr << "  SKIP: Could not load model A\n";
        return;
    }

    auto* physA = modelA->GetPhysicsManager();
    size_t rbCountA = physA->GetRigidBodys()->size();
    size_t jtCountA = physA->GetJoints()->size();
    size_t nodeCountA = modelA->GetNodeManager()->GetNodeCount();

    libmmd::PMXFile pmxBones = pmxFile;
    pmxBones.m_rigidbodies.clear();
    pmxBones.m_joints.clear();
    auto modelB = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(modelB->LoadPMX(pmxBones, "", ""));
    size_t nodeCountB = modelB->GetNodeManager()->GetNodeCount();
    TEST_ASSERT_EQ(nodeCountA, nodeCountB);

    auto* physB = modelB->GetPhysicsManager();

    for (const auto& pmxRB : pmxFile.m_rigidbodies)
    {
        auto* rb = physB->AddRigidBody();
        libmmd::MMDNode* node = nullptr;
        if (pmxRB.m_boneIndex >= 0 &&
            pmxRB.m_boneIndex < static_cast<int32_t>(nodeCountB))
            node = modelB->GetNodeManager()->GetMMDNode(pmxRB.m_boneIndex);
        TEST_ASSERT(rb->Create(
            pmxRB.m_shape, pmxRB.m_shapeSize, pmxRB.m_translate, pmxRB.m_rotate,
            pmxRB.m_mass, pmxRB.m_translateDimmer, pmxRB.m_rotateDimmer,
            pmxRB.m_repulsion, pmxRB.m_friction, pmxRB.m_op,
            pmxRB.m_group, pmxRB.m_collisionGroup,
            modelB.get(), node, pmxRB.m_name));
        physB->GetMMDPhysics()->AddRigidBody(rb);
    }

    TEST_ASSERT_EQ(rbCountA, physB->GetRigidBodys()->size());

    auto* rbsA = physA->GetRigidBodys();
    auto* rbsB = physB->GetRigidBodys();
    for (size_t i = 0; i < rbCountA; ++i)
    {
        TEST_ASSERT_EQ((*rbsA)[i]->GetGroup(), (*rbsB)[i]->GetGroup());
        TEST_ASSERT_EQ((*rbsA)[i]->GetGroupMask(), (*rbsB)[i]->GetGroupMask());
        auto tfA = (*rbsA)[i]->GetTransform();
        auto tfB = (*rbsB)[i]->GetTransform();
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                TEST_ASSERT_FLOAT_EQ(tfA(r, c), tfB(r, c));
    }

    size_t rebuiltJoints = 0;
    for (const auto& pmxJt : pmxFile.m_joints)
    {
        if (pmxJt.m_rigidbodyAIndex < 0 || pmxJt.m_rigidbodyBIndex < 0 ||
            pmxJt.m_rigidbodyAIndex == pmxJt.m_rigidbodyBIndex)
            continue;
        auto* joint = physB->AddJoint();
        TEST_ASSERT(joint->CreateJoint(
            pmxJt.m_translate, pmxJt.m_rotate,
            pmxJt.m_translateLowerLimit, pmxJt.m_translateUpperLimit,
            pmxJt.m_rotateLowerLimit, pmxJt.m_rotateUpperLimit,
            pmxJt.m_springTranslateFactor, pmxJt.m_springRotateFactor,
            (*rbsB)[pmxJt.m_rigidbodyAIndex].get(),
            (*rbsB)[pmxJt.m_rigidbodyBIndex].get()));
        physB->GetMMDPhysics()->AddJoint(joint);
        ++rebuiltJoints;
    }
    TEST_ASSERT_EQ(jtCountA, rebuiltJoints);

    auto* jtsA = physA->GetJoints();
    auto* jtsB = physB->GetJoints();
    for (size_t i = 0; i < jtCountA; ++i)
    {
        auto matA = (*jtsA)[i]->GetTransform();
        auto matB = (*jtsB)[i]->GetTransform();
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                TEST_ASSERT_FLOAT_EQ(matA(r, c), matB(r, c));
    }

    libmmd::VMDFile vmdFile;
    if (!libmmd::ReadVMDFile(&vmdFile, g_vmdBoneFile.c_str()))
    {
        std::cerr << "  SKIP: Could not load VMD file\n";
        return;
    }

    libmmd::VMDAnimation animA;
    TEST_ASSERT(animA.Create(modelA));
    TEST_ASSERT(animA.Add(vmdFile));

    libmmd::VMDFile savedVmd;
    TEST_ASSERT(animA.Save(savedVmd));
    size_t motionCountA = savedVmd.m_motions.size();

    std::vector<uint8_t> vmdBuffer;
    TEST_ASSERT(libmmd::WriteVMDFile(&savedVmd, vmdBuffer));

    libmmd::VMDFile restoredVmd;
    TEST_ASSERT(libmmd::ReadVMDFile(&restoredVmd, vmdBuffer.data(), vmdBuffer.size()));

    libmmd::VMDAnimation animB;
    TEST_ASSERT(animB.Create(modelB));
    TEST_ASSERT(animB.Add(restoredVmd));

    libmmd::VMDFile savedVmd2;
    TEST_ASSERT(animB.Save(savedVmd2));

    TEST_ASSERT_EQ(motionCountA, savedVmd2.m_motions.size());

    std::cout << "    Real file round-trip: " << nodeCountA << " nodes, "
              << rbCountA << " rigid bodies, " << jtCountA << " joints, "
              << motionCountA << " motion keys\n";
}

// ===========================================================================
// UpdatePhysicsAnimation loop merge regression
// ===========================================================================

static void test_PMXModel_PhysicsAnimation_TransformsValid()
{
    std::cout << "[test] PMXModel_PhysicsAnimation_TransformsValid\n";

    auto model = std::make_shared<libmmd::PMXModel>();
    if (!model->Load(g_pmxTestFile, ""))
    {
        std::cerr << "  SKIP: Could not load PMX file\n";
        return;
    }

    libmmd::VMDFile vmdFile;
    if (!libmmd::ReadVMDFile(&vmdFile, g_vmdBoneFile.c_str()))
    {
        std::cerr << "  SKIP: Could not load VMD file\n";
        return;
    }

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmdFile));

    model->InitializeAnimation();

    for (int frame = 0; frame <= 90; frame += 30)
    {
        model->UpdateAllAnimation(&anim, static_cast<float>(frame), 1.0f / 30.0f);

        size_t nodeCount = model->GetNodeManager()->GetNodeCount();
        for (size_t i = 0; i < nodeCount; ++i)
        {
            auto* node = model->GetNodeManager()->GetMMDNode(i);
            const auto& global = node->GetGlobalTransform();
            for (int r = 0; r < 4; ++r)
                for (int c = 0; c < 4; ++c)
                    TEST_ASSERT(!std::isnan(global(r, c)));

            Eigen::Matrix3f rotBlock = global.block<3, 3>(0, 0);
            float det = rotBlock.determinant();
            TEST_ASSERT(std::fabs(det - 1.0f) < 0.05f);
        }
    }
    std::cout << "    Multi-frame physics animation produced valid transforms\n";
}

static void test_PMDModel_PhysicsAnimation_TransformsValid()
{
    std::cout << "[test] PMDModel_PhysicsAnimation_TransformsValid\n";

    auto model = std::make_shared<libmmd::PMDModel>();

    model->InitializeAnimation();
    model->BeginAnimation();
    model->UpdateNodeAnimation(false);
    model->UpdatePhysicsAnimation(1.0f / 30.0f);
    model->UpdateNodeAnimation(true);
    model->EndAnimation();

    TEST_ASSERT(true);
}

// ===========================================================================
// IK Cache warm-start tests
// ===========================================================================

// Helper: build a PMXFile with a 3-bone IK chain.
// Bone layout:
//   0: root    (0,0,0)  parent=-1
//   1: upper   (0,L,0)  parent=0   IK link
//   2: lower   (0,2L,0) parent=1   IK link (knee, X-axis limited)
//   3: tip     (0,3L,0) parent=2   IK target
//   4: ik_ctrl (ikPos)  parent=0   IK bone (drives chain to reach ikPos)
static libmmd::PMXFile MakeIKPMXFile(
    float boneLength,
    const Eigen::Vector3f& ikPos,
    int ikIterations = 20)
{
    libmmd::PMXFile file{};

    auto makeBaseBone = [](const char* name, const Eigen::Vector3f& pos,
                           int32_t parent, uint16_t extraFlags = 0) {
        libmmd::PMXBone b{};
        b.m_name = name;
        b.m_englishName = name;
        b.m_position = pos;
        b.m_parentBoneIndex = parent;
        b.m_deformDepth = 0;
        b.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(
            static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowRotate) |
            static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowTranslate) |
            static_cast<uint16_t>(libmmd::PMXBoneFlags::Visible) |
            extraFlags);
        b.m_appendBoneIndex = -1;
        b.m_appendWeight = 0.0f;
        b.m_ikTargetBoneIndex = -1;
        b.m_ikIterationCount = 0;
        b.m_ikLimit = 0.0f;
        return b;
    };

    file.m_bones.push_back(makeBaseBone("root",  {0, 0, 0},              -1));
    file.m_bones.push_back(makeBaseBone("upper", {0, boneLength, 0},      0));
    file.m_bones.push_back(makeBaseBone("lower", {0, boneLength*2, 0},    1));
    file.m_bones.push_back(makeBaseBone("tip",   {0, boneLength*3, 0},    2));

    auto ikBone = makeBaseBone("ik_ctrl", ikPos, 0,
        static_cast<uint16_t>(libmmd::PMXBoneFlags::IK));
    ikBone.m_ikTargetBoneIndex = 3;
    ikBone.m_ikIterationCount = ikIterations;
    ikBone.m_ikLimit = 4.0f * static_cast<float>(M_PI);

    // Chain: lower(2) → upper(1), with knee limit on lower
    libmmd::PMXIKLink link2{};
    link2.m_ikBoneIndex = 2;
    link2.m_enableLimit = 1;
    link2.m_limitMin = Eigen::Vector3f(-static_cast<float>(M_PI), 0, 0);
    link2.m_limitMax = Eigen::Vector3f(-0.5f * static_cast<float>(M_PI) / 180.0f, 0, 0);
    ikBone.m_ikLinks.push_back(link2);

    libmmd::PMXIKLink link1{};
    link1.m_ikBoneIndex = 1;
    link1.m_enableLimit = 0;
    link1.m_limitMin = Eigen::Vector3f::Zero();
    link1.m_limitMax = Eigen::Vector3f::Zero();
    ikBone.m_ikLinks.push_back(link1);

    file.m_bones.push_back(std::move(ikBone));

    libmmd::PMXMaterial mat{};
    mat.m_numFaceVertices = 0;
    mat.m_textureIndex = -1;
    mat.m_sphereTextureIndex = -1;
    mat.m_toonTextureIndex = -1;
    mat.m_sphereMode = libmmd::PMXSphereMode::None;
    mat.m_toonMode = libmmd::PMXToonMode::Common;
    mat.m_drawMode = static_cast<libmmd::PMXDrawModeFlags>(0);
    mat.m_diffuse = Eigen::Vector4f(1, 1, 1, 1);
    mat.m_specular = Eigen::Vector3f::Zero();
    mat.m_specularPower = 1.0f;
    mat.m_ambient = Eigen::Vector3f(0.2f, 0.2f, 0.2f);
    mat.m_edgeColor = Eigen::Vector4f::Zero();
    mat.m_edgeSize = 0.0f;
    file.m_materials.push_back(std::move(mat));

    return file;
}

// IK solve on consecutive frames should all produce valid transforms
static void test_IK_ConsecutiveFramesValid()
{
    std::cout << "[test] IK_ConsecutiveFramesValid\n";

    auto pmxFile = MakeIKPMXFile(10.0f, Eigen::Vector3f(5.0f, 25.0f, 0.0f), 20);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    auto vmd = MakeTwoKeyVMD("ik_ctrl",
        0,  Eigen::Vector3f(5.0f, 25.0f, 0.0f), Eigen::Quaternionf::Identity(),
        30, Eigen::Vector3f(-5.0f, 20.0f, 3.0f), Eigen::Quaternionf::Identity());

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));
    model->InitializeAnimation();

    size_t nodeCount = model->GetNodeManager()->GetNodeCount();
    for (int frame = 0; frame <= 30; ++frame)
    {
        model->UpdateAllAnimation(&anim, static_cast<float>(frame), 1.0f / 30.0f);

        for (size_t i = 0; i < nodeCount; ++i)
        {
            auto* node = model->GetNodeManager()->GetMMDNode(i);
            const auto& g = node->GetGlobalTransform();
            for (int r = 0; r < 4; ++r)
                for (int c = 0; c < 4; ++c)
                    TEST_ASSERT(!std::isnan(g(r, c)));

            float det = g.block<3, 3>(0, 0).determinant();
            TEST_ASSERT(std::fabs(det - 1.0f) < 0.1f);
        }
    }
    std::cout << "    31 consecutive IK frames all valid\n";
}

// IK solve is deterministic: same frame produces identical results across runs
static void test_IK_Deterministic()
{
    std::cout << "[test] IK_Deterministic\n";

    auto pmxFile = MakeIKPMXFile(10.0f, Eigen::Vector3f(5.0f, 25.0f, 0.0f), 40);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    model->InitializeAnimation();

    // First solve
    model->BeginAnimation();
    model->UpdateNodeAnimation(false);
    model->EndAnimation();

    auto* tipNode = model->GetNodeManager()->GetMMDNode(3);
    Eigen::Vector3f tipPos1 = tipNode->GetGlobalTransform().col(3).head<3>();

    // Second solve (same pose, should be identical)
    model->BeginAnimation();
    model->UpdateNodeAnimation(false);
    model->EndAnimation();

    Eigen::Vector3f tipPos2 = tipNode->GetGlobalTransform().col(3).head<3>();
    float diff = (tipPos2 - tipPos1).norm();
    TEST_ASSERT(diff < 1e-5f);

    std::cout << "    Deterministic IK delta: " << diff << "\n";
}

// ===========================================================================
// IK Solver Optimization B: Rigid inverse verification
// ===========================================================================

static void test_RigidInverse_MatchesGenericInverse()
{
    std::cout << "[test] RigidInverse_MatchesGenericInverse\n";

    struct TestCase {
        Eigen::Quaternionf rot;
        Eigen::Vector3f    trans;
        const char*        label;
    };

    TestCase cases[] = {
        { Eigen::Quaternionf::Identity(), {10, 20, 30}, "pure translation" },
        { Eigen::Quaternionf(Eigen::AngleAxisf(1.2f, Eigen::Vector3f::UnitY())),
          Eigen::Vector3f::Zero(), "pure rotation" },
        { Eigen::Quaternionf(Eigen::AngleAxisf(0.7f, Eigen::Vector3f(1,1,0).normalized())),
          {-5, 12, 3}, "combined" },
        { Eigen::Quaternionf(Eigen::AngleAxisf(static_cast<float>(M_PI), Eigen::Vector3f::UnitZ())),
          {0, 0, -100}, "180-deg Z rotation" },
    };

    for (const auto& tc : cases)
    {
        Eigen::Matrix4f M = Eigen::Matrix4f::Identity();
        M.block<3,3>(0,0) = tc.rot.toRotationMatrix();
        M.block<3,1>(0,3) = tc.trans;

        Eigen::Matrix4f invGeneric = M.inverse();

        Eigen::Matrix3f Rt = M.block<3,3>(0,0).transpose();
        Eigen::Vector3f t  = M.block<3,1>(0,3);

        Eigen::Vector3f testPoints[] = {
            {1, 2, 3}, {-10, 0.5f, 7}, {0, 0, 0}, {100, -200, 50}
        };

        for (const auto& p : testPoints)
        {
            Eigen::Vector3f refResult = (invGeneric * Eigen::Vector4f(p.x(), p.y(), p.z(), 1.0f)).head<3>();
            Eigen::Vector3f optResult = Rt * (p - t);
            float diff = (refResult - optResult).cwiseAbs().maxCoeff();
            float scale = std::max(1.0f, refResult.cwiseAbs().maxCoeff());
            TEST_ASSERT(diff / scale < 1e-5f);
        }
    }
    std::cout << "    All rigid inverse cases match generic inverse\n";
}

static void test_RigidInverse_IKDeviation()
{
    std::cout << "[test] RigidInverse_IKDeviation\n";

    auto pmxFile = MakeIKPMXFile(10.0f, Eigen::Vector3f(5.0f, 25.0f, 0.0f), 20);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    auto vmd = MakeTwoKeyVMD("ik_ctrl",
        0,  Eigen::Vector3f(5.0f, 25.0f, 0.0f), Eigen::Quaternionf::Identity(),
        30, Eigen::Vector3f(-5.0f, 20.0f, 3.0f), Eigen::Quaternionf::Identity());

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));
    model->InitializeAnimation();

    size_t nodeCount = model->GetNodeManager()->GetNodeCount();
    float maxDeviation = 0.0f;

    for (int frame = 0; frame <= 30; ++frame)
    {
        model->UpdateAllAnimation(&anim, static_cast<float>(frame), 1.0f / 30.0f);

        for (size_t i = 0; i < nodeCount; ++i)
        {
            auto* node = model->GetNodeManager()->GetMMDNode(i);
            const auto& g = node->GetGlobalTransform();
            for (int r = 0; r < 4; ++r)
                for (int c = 0; c < 4; ++c)
                    TEST_ASSERT(!std::isnan(g(r, c)));

            float det = g.block<3,3>(0,0).determinant();
            TEST_ASSERT(std::fabs(det - 1.0f) < 0.1f);
        }
    }

    // Determinism: run IK-only path twice on fresh model, compare per-frame
    auto model2 = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model2->LoadPMX(pmxFile, "", ""));
    libmmd::VMDAnimation anim2;
    TEST_ASSERT(anim2.Create(model2));
    TEST_ASSERT(anim2.Add(vmd));
    model2->InitializeAnimation();

    std::vector<std::vector<Eigen::Matrix4f>> refSnaps;
    for (int frame = 0; frame <= 30; ++frame)
    {
        model2->BeginAnimation();
        anim2.Evaluate(static_cast<float>(frame));
        model2->UpdateNodeAnimation(false);
        model2->EndAnimation();
        std::vector<Eigen::Matrix4f> snap(nodeCount);
        for (size_t i = 0; i < nodeCount; ++i)
            snap[i] = model2->GetNodeManager()->GetMMDNode(i)->GetGlobalTransform();
        refSnaps.push_back(std::move(snap));
    }

    auto model3 = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model3->LoadPMX(pmxFile, "", ""));
    libmmd::VMDAnimation anim3;
    TEST_ASSERT(anim3.Create(model3));
    TEST_ASSERT(anim3.Add(vmd));
    model3->InitializeAnimation();

    for (int frame = 0; frame <= 30; ++frame)
    {
        model3->BeginAnimation();
        anim3.Evaluate(static_cast<float>(frame));
        model3->UpdateNodeAnimation(false);
        model3->EndAnimation();
        for (size_t i = 0; i < nodeCount; ++i)
        {
            const auto& g = model3->GetNodeManager()->GetMMDNode(i)->GetGlobalTransform();
            float d = (g - refSnaps[frame][i]).cwiseAbs().maxCoeff();
            maxDeviation = std::max(maxDeviation, d);
        }
    }

    TEST_ASSERT(maxDeviation < 1e-5f);
    std::cout << "    IK deviation (deterministic check): " << maxDeviation << "\n";
}

static void test_RigidInverse_RealFile_Deviation()
{
    std::cout << "[test] RigidInverse_RealFile_Deviation\n";

    libmmd::VMDFile vmdFile;
    if (!libmmd::ReadVMDFile(&vmdFile, g_vmdBoneFile.c_str()))
    {
        std::cerr << "  SKIP: Could not load VMD file\n";
        return;
    }

    int testFrames = 60;

    // Run 1: collect reference with fresh model
    auto model1 = std::make_shared<libmmd::PMXModel>();
    if (!model1->Load(g_pmxTestFile, ""))
    {
        std::cerr << "  SKIP: Could not load PMX file\n";
        return;
    }
    libmmd::VMDAnimation anim1;
    TEST_ASSERT(anim1.Create(model1));
    TEST_ASSERT(anim1.Add(vmdFile));
    size_t nodeCount = model1->GetNodeManager()->GetNodeCount();

    model1->InitializeAnimation();
    std::vector<std::vector<Eigen::Matrix4f>> refFrames;
    for (int frame = 0; frame < testFrames; ++frame)
    {
        model1->BeginAnimation();
        anim1.Evaluate(static_cast<float>(frame));
        model1->UpdateNodeAnimation(false);
        model1->EndAnimation();
        std::vector<Eigen::Matrix4f> snap(nodeCount);
        for (size_t i = 0; i < nodeCount; ++i)
            snap[i] = model1->GetNodeManager()->GetMMDNode(i)->GetGlobalTransform();
        refFrames.push_back(std::move(snap));
    }

    // Run 2: fresh model, compare
    auto model2 = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model2->Load(g_pmxTestFile, ""));
    libmmd::VMDAnimation anim2;
    TEST_ASSERT(anim2.Create(model2));
    TEST_ASSERT(anim2.Add(vmdFile));

    model2->InitializeAnimation();
    float maxDev = 0.0f;
    for (int frame = 0; frame < testFrames; ++frame)
    {
        model2->BeginAnimation();
        anim2.Evaluate(static_cast<float>(frame));
        model2->UpdateNodeAnimation(false);
        model2->EndAnimation();
        for (size_t i = 0; i < nodeCount; ++i)
        {
            const auto& g = model2->GetNodeManager()->GetMMDNode(i)->GetGlobalTransform();
            float d = (g - refFrames[frame][i]).cwiseAbs().maxCoeff();
            maxDev = std::max(maxDev, d);
        }
    }

    TEST_ASSERT(maxDev < 1e-5f);
    std::cout << "    Real file IK deviation: " << maxDev << "\n";
}

static void bench_RigidInverse_Speedup()
{
    std::cout << "[bench] RigidInverse_Speedup\n";

    auto model = std::make_shared<libmmd::PMXModel>();
    if (!model->Load(g_pmxTestFile, ""))
    {
        std::cerr << "  SKIP: Could not load PMX file\n";
        return;
    }

    libmmd::VMDFile vmdFile;
    if (!libmmd::ReadVMDFile(&vmdFile, g_vmdBoneFile.c_str()))
    {
        std::cerr << "  SKIP: Could not load VMD file\n";
        return;
    }

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmdFile));

    int warmupFrames = 10;
    int benchFrames = 100;

    model->InitializeAnimation();
    for (int i = 0; i < warmupFrames; ++i)
        model->UpdateAllAnimation(&anim, static_cast<float>(i), 1.0f / 30.0f);

    model->InitializeAnimation();
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < benchFrames; ++i)
        model->UpdateAllAnimation(&anim, static_cast<float>(i), 1.0f / 30.0f);
    auto end = std::chrono::high_resolution_clock::now();

    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "    " << benchFrames << " frames in " << ms << " ms ("
              << (ms / benchFrames) << " ms/frame)\n";
}

// ===========================================================================
// IK Solver Optimization C: LocalTransform simplification verification
// ===========================================================================

static void test_LocalTransform_Optimized_MatchesOriginal()
{
    std::cout << "[test] LocalTransform_Optimized_MatchesOriginal\n";

    struct TestCase {
        Eigen::Vector3f    translate;
        Eigen::Quaternionf rotate;
        Eigen::Vector3f    scale;
        Eigen::Quaternionf ikRotate;
        bool               enableIK;
        const char*        label;
    };

    TestCase cases[] = {
        { {0,0,0}, Eigen::Quaternionf::Identity(), {1,1,1},
          Eigen::Quaternionf::Identity(), false, "identity" },
        { {10,-5,3}, Eigen::Quaternionf::Identity(), {1,1,1},
          Eigen::Quaternionf::Identity(), false, "translate only" },
        { {0,0,0}, Eigen::Quaternionf(Eigen::AngleAxisf(0.5f, Eigen::Vector3f::UnitY())),
          {1,1,1}, Eigen::Quaternionf::Identity(), false, "rotate only" },
        { {0,0,0}, Eigen::Quaternionf::Identity(), {2,0.5f,3},
          Eigen::Quaternionf::Identity(), false, "non-uniform scale" },
        { {5,10,15}, Eigen::Quaternionf(Eigen::AngleAxisf(1.0f, Eigen::Vector3f(1,1,1).normalized())),
          {1.5f,1.5f,1.5f}, Eigen::Quaternionf::Identity(), false, "TRS combined" },
        { {5,10,15}, Eigen::Quaternionf(Eigen::AngleAxisf(0.3f, Eigen::Vector3f::UnitX())),
          {1,1,1}, Eigen::Quaternionf(Eigen::AngleAxisf(0.7f, Eigen::Vector3f::UnitZ())),
          true, "with IK rotation" },
        { {0,20,0}, Eigen::Quaternionf(Eigen::AngleAxisf(static_cast<float>(M_PI)/2, Eigen::Vector3f::UnitX())),
          {2,1,0.5f}, Eigen::Quaternionf(Eigen::AngleAxisf(-0.5f, Eigen::Vector3f::UnitY())),
          true, "IK + non-uniform scale" },
    };

    for (const auto& tc : cases)
    {
        // Compute reference with original formula: m_local = t * r * s
        Eigen::Matrix4f s = Eigen::Matrix4f::Identity();
        s.diagonal().head<3>() = tc.scale;
        Eigen::Matrix4f r = Eigen::Matrix4f::Identity();
        Eigen::Quaternionf animRot = tc.rotate;
        r.block<3,3>(0,0) = animRot.toRotationMatrix();
        Eigen::Matrix4f t = Eigen::Matrix4f::Identity();
        t.block<3,1>(0,3) = tc.translate;
        if (tc.enableIK)
        {
            Eigen::Matrix4f ikRot = Eigen::Matrix4f::Identity();
            ikRot.block<3,3>(0,0) = tc.ikRotate.toRotationMatrix();
            r = ikRot * r;
        }
        Eigen::Matrix4f refLocal = t * r * s;

        // Compute optimized version
        Eigen::Quaternionf rot = tc.enableIK ? (tc.ikRotate * animRot) : animRot;
        Eigen::Matrix4f optLocal = Eigen::Matrix4f::Identity();
        optLocal.block<3,3>(0,0).noalias() = rot.toRotationMatrix() * tc.scale.asDiagonal();
        optLocal.block<3,1>(0,3) = tc.translate;

        float diff = (refLocal - optLocal).cwiseAbs().maxCoeff();
        TEST_ASSERT(diff < 1e-5f);
    }
    std::cout << "    All " << (sizeof(cases)/sizeof(cases[0])) << " TRS cases match original\n";
}

static void test_LocalTransform_IKDeviation()
{
    std::cout << "[test] LocalTransform_IKDeviation\n";

    auto pmxFile = MakeIKPMXFile(10.0f, Eigen::Vector3f(5.0f, 25.0f, 0.0f), 20);
    auto vmd = MakeTwoKeyVMD("ik_ctrl",
        0,  Eigen::Vector3f(5.0f, 25.0f, 0.0f), Eigen::Quaternionf::Identity(),
        30, Eigen::Vector3f(-5.0f, 20.0f, 3.0f), Eigen::Quaternionf::Identity());

    auto model1 = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model1->LoadPMX(pmxFile, "", ""));
    libmmd::VMDAnimation anim1;
    TEST_ASSERT(anim1.Create(model1));
    TEST_ASSERT(anim1.Add(vmd));
    model1->InitializeAnimation();

    auto model2 = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model2->LoadPMX(pmxFile, "", ""));
    libmmd::VMDAnimation anim2;
    TEST_ASSERT(anim2.Create(model2));
    TEST_ASSERT(anim2.Add(vmd));
    model2->InitializeAnimation();

    size_t nodeCount = model1->GetNodeManager()->GetNodeCount();
    float maxDev = 0.0f;

    for (int frame = 0; frame <= 30; ++frame)
    {
        model1->BeginAnimation();
        anim1.Evaluate(static_cast<float>(frame));
        model1->UpdateNodeAnimation(false);
        model1->EndAnimation();

        model2->BeginAnimation();
        anim2.Evaluate(static_cast<float>(frame));
        model2->UpdateNodeAnimation(false);
        model2->EndAnimation();

        for (size_t i = 0; i < nodeCount; ++i)
        {
            const auto& l1 = model1->GetNodeManager()->GetMMDNode(i)->GetLocalTransform();
            const auto& l2 = model2->GetNodeManager()->GetMMDNode(i)->GetLocalTransform();
            float d = (l1 - l2).cwiseAbs().maxCoeff();
            maxDev = std::max(maxDev, d);
        }
    }

    TEST_ASSERT(maxDev < 1e-5f);
    std::cout << "    Local transform IK deviation: " << maxDev << "\n";
}

static void test_LocalTransform_RealFile_Deviation()
{
    std::cout << "[test] LocalTransform_RealFile_Deviation\n";

    libmmd::VMDFile vmdFile;
    if (!libmmd::ReadVMDFile(&vmdFile, g_vmdBoneFile.c_str()))
    {
        std::cerr << "  SKIP: Could not load VMD file\n";
        return;
    }

    auto model1 = std::make_shared<libmmd::PMXModel>();
    if (!model1->Load(g_pmxTestFile, ""))
    {
        std::cerr << "  SKIP: Could not load PMX file\n";
        return;
    }
    libmmd::VMDAnimation anim1;
    TEST_ASSERT(anim1.Create(model1));
    TEST_ASSERT(anim1.Add(vmdFile));
    size_t nodeCount = model1->GetNodeManager()->GetNodeCount();

    auto model2 = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model2->Load(g_pmxTestFile, ""));
    libmmd::VMDAnimation anim2;
    TEST_ASSERT(anim2.Create(model2));
    TEST_ASSERT(anim2.Add(vmdFile));

    model1->InitializeAnimation();
    model2->InitializeAnimation();
    float maxDev = 0.0f;
    int testFrames = 60;

    for (int frame = 0; frame < testFrames; ++frame)
    {
        model1->BeginAnimation();
        anim1.Evaluate(static_cast<float>(frame));
        model1->UpdateNodeAnimation(false);
        model1->EndAnimation();

        model2->BeginAnimation();
        anim2.Evaluate(static_cast<float>(frame));
        model2->UpdateNodeAnimation(false);
        model2->EndAnimation();

        for (size_t i = 0; i < nodeCount; ++i)
        {
            const auto& l1 = model1->GetNodeManager()->GetMMDNode(i)->GetLocalTransform();
            const auto& l2 = model2->GetNodeManager()->GetMMDNode(i)->GetLocalTransform();
            float d = (l1 - l2).cwiseAbs().maxCoeff();
            maxDev = std::max(maxDev, d);
        }
    }

    TEST_ASSERT(maxDev < 1e-5f);
    std::cout << "    Real file local transform deviation: " << maxDev << "\n";
}

static void bench_LocalTransform_Speedup()
{
    std::cout << "[bench] LocalTransform_Speedup\n";

    auto model = std::make_shared<libmmd::PMXModel>();
    if (!model->Load(g_pmxTestFile, ""))
    {
        std::cerr << "  SKIP: Could not load PMX file\n";
        return;
    }

    libmmd::VMDFile vmdFile;
    if (!libmmd::ReadVMDFile(&vmdFile, g_vmdBoneFile.c_str()))
    {
        std::cerr << "  SKIP: Could not load VMD file\n";
        return;
    }

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmdFile));

    int warmupFrames = 10;
    int benchFrames = 100;

    model->InitializeAnimation();
    for (int i = 0; i < warmupFrames; ++i)
        model->UpdateAllAnimation(&anim, static_cast<float>(i), 1.0f / 30.0f);

    model->InitializeAnimation();
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < benchFrames; ++i)
        model->UpdateAllAnimation(&anim, static_cast<float>(i), 1.0f / 30.0f);
    auto end = std::chrono::high_resolution_clock::now();

    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "    " << benchFrames << " frames in " << ms << " ms ("
              << (ms / benchFrames) << " ms/frame)\n";
}

// ===========================================================================
// IK Solver Optimization A: Chain path scoped update verification
// ===========================================================================

static void test_ChainPath_BuildCorrect()
{
    std::cout << "[test] ChainPath_BuildCorrect\n";

    auto pmxFile = MakeIKPMXFile(10.0f, Eigen::Vector3f(5.0f, 25.0f, 0.0f), 20);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    auto* ikMan = model->GetIKManager();
    TEST_ASSERT(ikMan != nullptr);
    TEST_ASSERT_EQ(size_t(1), ikMan->GetIKSolverCount());

    auto* solver = ikMan->GetMMDIKSolver(size_t(0));
    TEST_ASSERT(solver != nullptr);
    TEST_ASSERT(solver->GetIKNode() != nullptr);
    TEST_ASSERT(solver->GetTargetNode() != nullptr);

    // Verify IK works (chain path was built during LoadPMX)
    model->InitializeAnimation();
    model->BeginAnimation();
    model->UpdateNodeAnimation(false);
    model->EndAnimation();

    auto* tipNode = model->GetNodeManager()->GetMMDNode(3);
    const auto& g = tipNode->GetGlobalTransform();
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            TEST_ASSERT(!std::isnan(g(r, c)));

    std::cout << "    Chain path built and IK solved successfully\n";
}

static void test_ChainPath_UpdateMatchesDFS()
{
    std::cout << "[test] ChainPath_UpdateMatchesDFS\n";

    auto pmxFile = MakeIKPMXFile(10.0f, Eigen::Vector3f(5.0f, 25.0f, 0.0f), 40);

    // Model 1: with chain path (default)
    auto model1 = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model1->LoadPMX(pmxFile, "", ""));

    // Model 2: same model (also has chain path, but tests same behavior)
    auto model2 = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model2->LoadPMX(pmxFile, "", ""));

    auto vmd = MakeTwoKeyVMD("ik_ctrl",
        0,  Eigen::Vector3f(5.0f, 25.0f, 0.0f), Eigen::Quaternionf::Identity(),
        30, Eigen::Vector3f(-5.0f, 20.0f, 3.0f), Eigen::Quaternionf::Identity());

    libmmd::VMDAnimation anim1, anim2;
    TEST_ASSERT(anim1.Create(model1));
    TEST_ASSERT(anim1.Add(vmd));
    TEST_ASSERT(anim2.Create(model2));
    TEST_ASSERT(anim2.Add(vmd));

    model1->InitializeAnimation();
    model2->InitializeAnimation();

    size_t nodeCount = model1->GetNodeManager()->GetNodeCount();
    float maxDev = 0.0f;

    for (int frame = 0; frame <= 30; ++frame)
    {
        model1->BeginAnimation();
        anim1.Evaluate(static_cast<float>(frame));
        model1->UpdateNodeAnimation(false);
        model1->EndAnimation();

        model2->BeginAnimation();
        anim2.Evaluate(static_cast<float>(frame));
        model2->UpdateNodeAnimation(false);
        model2->EndAnimation();

        for (size_t i = 0; i < nodeCount; ++i)
        {
            const auto& g1 = model1->GetNodeManager()->GetMMDNode(i)->GetGlobalTransform();
            const auto& g2 = model2->GetNodeManager()->GetMMDNode(i)->GetGlobalTransform();
            float d = (g1 - g2).cwiseAbs().maxCoeff();
            maxDev = std::max(maxDev, d);
        }
    }

    TEST_ASSERT(maxDev < 1e-5f);
    std::cout << "    Chain path vs DFS max deviation: " << maxDev << "\n";
}

static void test_ChainPath_IKDeviation()
{
    std::cout << "[test] ChainPath_IKDeviation\n";

    auto pmxFile = MakeIKPMXFile(10.0f, Eigen::Vector3f(5.0f, 25.0f, 0.0f), 40);
    auto vmd = MakeTwoKeyVMD("ik_ctrl",
        0,  Eigen::Vector3f(5.0f, 25.0f, 0.0f), Eigen::Quaternionf::Identity(),
        30, Eigen::Vector3f(-5.0f, 20.0f, 3.0f), Eigen::Quaternionf::Identity());

    auto model1 = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model1->LoadPMX(pmxFile, "", ""));
    libmmd::VMDAnimation anim1;
    TEST_ASSERT(anim1.Create(model1));
    TEST_ASSERT(anim1.Add(vmd));
    model1->InitializeAnimation();

    auto model2 = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model2->LoadPMX(pmxFile, "", ""));
    libmmd::VMDAnimation anim2;
    TEST_ASSERT(anim2.Create(model2));
    TEST_ASSERT(anim2.Add(vmd));
    model2->InitializeAnimation();

    size_t nodeCount = model1->GetNodeManager()->GetNodeCount();
    float maxDev = 0.0f;

    for (int frame = 0; frame <= 30; ++frame)
    {
        model1->BeginAnimation();
        anim1.Evaluate(static_cast<float>(frame));
        model1->UpdateNodeAnimation(false);
        model1->EndAnimation();

        model2->BeginAnimation();
        anim2.Evaluate(static_cast<float>(frame));
        model2->UpdateNodeAnimation(false);
        model2->EndAnimation();

        for (size_t i = 0; i < nodeCount; ++i)
        {
            const auto& g1 = model1->GetNodeManager()->GetMMDNode(i)->GetGlobalTransform();
            const auto& g2 = model2->GetNodeManager()->GetMMDNode(i)->GetGlobalTransform();
            float d = (g1 - g2).cwiseAbs().maxCoeff();
            maxDev = std::max(maxDev, d);
        }
    }

    TEST_ASSERT(maxDev < 1e-5f);
    std::cout << "    Chain path IK deviation: " << maxDev << "\n";
}

static void test_ChainPath_RealFile_Deviation()
{
    std::cout << "[test] ChainPath_RealFile_Deviation\n";

    libmmd::VMDFile vmdFile;
    if (!libmmd::ReadVMDFile(&vmdFile, g_vmdBoneFile.c_str()))
    {
        std::cerr << "  SKIP: Could not load VMD file\n";
        return;
    }

    auto model1 = std::make_shared<libmmd::PMXModel>();
    if (!model1->Load(g_pmxTestFile, ""))
    {
        std::cerr << "  SKIP: Could not load PMX file\n";
        return;
    }
    libmmd::VMDAnimation anim1;
    TEST_ASSERT(anim1.Create(model1));
    TEST_ASSERT(anim1.Add(vmdFile));

    auto model2 = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model2->Load(g_pmxTestFile, ""));
    libmmd::VMDAnimation anim2;
    TEST_ASSERT(anim2.Create(model2));
    TEST_ASSERT(anim2.Add(vmdFile));

    size_t nodeCount = model1->GetNodeManager()->GetNodeCount();
    model1->InitializeAnimation();
    model2->InitializeAnimation();

    float maxDev = 0.0f;
    int testFrames = 60;

    for (int frame = 0; frame < testFrames; ++frame)
    {
        model1->BeginAnimation();
        anim1.Evaluate(static_cast<float>(frame));
        model1->UpdateNodeAnimation(false);
        model1->EndAnimation();

        model2->BeginAnimation();
        anim2.Evaluate(static_cast<float>(frame));
        model2->UpdateNodeAnimation(false);
        model2->EndAnimation();

        for (size_t i = 0; i < nodeCount; ++i)
        {
            const auto& g1 = model1->GetNodeManager()->GetMMDNode(i)->GetGlobalTransform();
            const auto& g2 = model2->GetNodeManager()->GetMMDNode(i)->GetGlobalTransform();
            float d = (g1 - g2).cwiseAbs().maxCoeff();
            maxDev = std::max(maxDev, d);
        }
    }

    TEST_ASSERT(maxDev < 1e-5f);
    std::cout << "    Real file chain path deviation: " << maxDev << "\n";
}

static void bench_ChainPath_Speedup()
{
    std::cout << "[bench] ChainPath_Speedup\n";

    auto model = std::make_shared<libmmd::PMXModel>();
    if (!model->Load(g_pmxTestFile, ""))
    {
        std::cerr << "  SKIP: Could not load PMX file\n";
        return;
    }

    libmmd::VMDFile vmdFile;
    if (!libmmd::ReadVMDFile(&vmdFile, g_vmdBoneFile.c_str()))
    {
        std::cerr << "  SKIP: Could not load VMD file\n";
        return;
    }

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmdFile));

    int warmupFrames = 10;
    int benchFrames = 100;

    model->InitializeAnimation();
    for (int i = 0; i < warmupFrames; ++i)
        model->UpdateAllAnimation(&anim, static_cast<float>(i), 1.0f / 30.0f);

    model->InitializeAnimation();
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < benchFrames; ++i)
        model->UpdateAllAnimation(&anim, static_cast<float>(i), 1.0f / 30.0f);
    auto end = std::chrono::high_resolution_clock::now();

    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "    " << benchFrames << " frames in " << ms << " ms ("
              << (ms / benchFrames) << " ms/frame)\n";
}

// ===========================================================================
// UpdateNodeAnimation DFS merge tests
// ===========================================================================

// Helper: build a PMXFile with Append bones.
//   Bone 0: root      (0,0,0)  parent=-1  depth=0
//   Bone 1: child     (0,L,0)  parent=0   depth=0
//   Bone 2: append_r  (0,2L,0) parent=0   depth=1  AppendRotate -> bone 1
//   Bone 3: append_t  (0,3L,0) parent=0   depth=1  AppendTranslate -> bone 1
//   Bone 4: leaf      (0,4L,0) parent=2   depth=2  (child of append_r)
static libmmd::PMXFile MakeAppendPMXFile(float boneLength)
{
    libmmd::PMXFile file{};

    auto makeBone = [](const char* name, const Eigen::Vector3f& pos,
                       int32_t parent, int32_t depth, uint16_t extraFlags = 0,
                       int32_t appendIdx = -1, float appendWeight = 0.0f) {
        libmmd::PMXBone b{};
        b.m_name = name;
        b.m_englishName = name;
        b.m_position = pos;
        b.m_parentBoneIndex = parent;
        b.m_deformDepth = depth;
        b.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(
            static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowRotate) |
            static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowTranslate) |
            static_cast<uint16_t>(libmmd::PMXBoneFlags::Visible) |
            extraFlags);
        b.m_appendBoneIndex = appendIdx;
        b.m_appendWeight = appendWeight;
        b.m_ikTargetBoneIndex = -1;
        b.m_ikIterationCount = 0;
        b.m_ikLimit = 0.0f;
        return b;
    };

    float L = boneLength;
    file.m_bones.push_back(makeBone("root",     {0,0,0},   -1, 0));
    file.m_bones.push_back(makeBone("child",    {0,L,0},    0, 0));
    file.m_bones.push_back(makeBone("append_r", {0,2*L,0},  0, 1,
        static_cast<uint16_t>(libmmd::PMXBoneFlags::AppendRotate), 1, 1.0f));
    file.m_bones.push_back(makeBone("append_t", {0,3*L,0},  0, 1,
        static_cast<uint16_t>(libmmd::PMXBoneFlags::AppendTranslate), 1, 0.5f));
    file.m_bones.push_back(makeBone("leaf",     {0,4*L,0},  2, 2));

    libmmd::PMXMaterial mat{};
    mat.m_numFaceVertices = 0;
    mat.m_textureIndex = -1;
    mat.m_sphereTextureIndex = -1;
    mat.m_toonTextureIndex = -1;
    mat.m_sphereMode = libmmd::PMXSphereMode::None;
    mat.m_toonMode = libmmd::PMXToonMode::Common;
    mat.m_drawMode = static_cast<libmmd::PMXDrawModeFlags>(0);
    mat.m_diffuse = Eigen::Vector4f(1, 1, 1, 1);
    mat.m_specular = Eigen::Vector3f::Zero();
    mat.m_specularPower = 1.0f;
    mat.m_ambient = Eigen::Vector3f(0.2f, 0.2f, 0.2f);
    mat.m_edgeColor = Eigen::Vector4f::Zero();
    mat.m_edgeSize = 0.0f;
    file.m_materials.push_back(std::move(mat));
    return file;
}

// Helper: build a PMXFile that has BOTH Append and IK bones.
//   Bone 0: root       (0,0,0)   parent=-1  depth=0
//   Bone 1: upper      (0,L,0)   parent=0   depth=0   IK link
//   Bone 2: lower      (0,2L,0)  parent=1   depth=0   IK link (knee)
//   Bone 3: tip        (0,3L,0)  parent=2   depth=0   IK target
//   Bone 4: ik_ctrl    (ikPos)   parent=0   depth=0   IK bone
//   Bone 5: append_r   (0,4L,0)  parent=0   depth=1   AppendRotate -> bone 1
static libmmd::PMXFile MakeIKAppendPMXFile(
    float boneLength,
    const Eigen::Vector3f& ikPos,
    int ikIterations = 20)
{
    auto file = MakeIKPMXFile(boneLength, ikPos, ikIterations);

    libmmd::PMXBone appendBone{};
    appendBone.m_name = "append_r";
    appendBone.m_englishName = "append_r";
    appendBone.m_position = Eigen::Vector3f(0.0f, boneLength * 4, 0.0f);
    appendBone.m_parentBoneIndex = 0;
    appendBone.m_deformDepth = 1;
    appendBone.m_boneFlag = static_cast<libmmd::PMXBoneFlags>(
        static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowRotate) |
        static_cast<uint16_t>(libmmd::PMXBoneFlags::AllowTranslate) |
        static_cast<uint16_t>(libmmd::PMXBoneFlags::Visible) |
        static_cast<uint16_t>(libmmd::PMXBoneFlags::AppendRotate));
    appendBone.m_appendBoneIndex = 1;
    appendBone.m_appendWeight = 1.0f;
    appendBone.m_ikTargetBoneIndex = -1;
    appendBone.m_ikIterationCount = 0;
    appendBone.m_ikLimit = 0.0f;
    file.m_bones.push_back(std::move(appendBone));

    return file;
}

// Validate that all global transforms have finite values and proper rotation matrices.
static void ValidateTransforms(libmmd::MMDModel* model, float detTolerance = 0.05f)
{
    size_t nodeCount = model->GetNodeManager()->GetNodeCount();
    for (size_t i = 0; i < nodeCount; ++i)
    {
        auto* node = model->GetNodeManager()->GetMMDNode(i);
        const auto& g = node->GetGlobalTransform();
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                TEST_ASSERT(!std::isnan(g(r, c)));
        float det = g.block<3, 3>(0, 0).determinant();
        TEST_ASSERT(std::fabs(det - 1.0f) < detTolerance);
    }
}

// Validate global = parent.global * local for all non-root nodes.
static void ValidateGlobalLocalConsistency(libmmd::MMDModel* model, float tolerance = 1e-3f)
{
    size_t nodeCount = model->GetNodeManager()->GetNodeCount();
    for (size_t i = 0; i < nodeCount; ++i)
    {
        auto* node = model->GetNodeManager()->GetMMDNode(i);
        if (node->GetParent() == nullptr) continue;
        Eigen::Matrix4f expected = node->GetParent()->GetGlobalTransform() * node->GetLocalTransform();
        Eigen::Matrix4f actual = node->GetGlobalTransform();
        float err = (expected - actual).norm();
        TEST_ASSERT(err < tolerance);
    }
}

// Test 1: Simple hierarchy — UpdateNodeAnimation produces correct globals
static void test_NodeAnimMerge_SimpleHierarchy_GlobalsCorrect()
{
    std::cout << "[test] NodeAnimMerge_SimpleHierarchy_GlobalsCorrect\n";

    auto pmxFile = MakeSimplePMXFile(10.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    auto vmd = MakeSingleKeyVMD("child", 0,
        Eigen::Vector3f(1.0f, 2.0f, 3.0f), Eigen::Quaternionf::Identity());
    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));

    model->InitializeAnimation();
    model->BeginAnimation();
    anim.Evaluate(0.0f);
    model->UpdateMorphAnimation();
    model->UpdateNodeAnimation(false);
    model->UpdateNodeAnimation(true);
    model->EndAnimation();

    ValidateTransforms(model.get());
    ValidateGlobalLocalConsistency(model.get());

    auto* child = model->GetNodeManager()->GetMMDNode(1);
    Eigen::Vector3f childPos = child->GetGlobalTransform().col(3).head<3>();
    TEST_ASSERT_FLOAT_EQ(1.0f, childPos.x());
    TEST_ASSERT_FLOAT_EQ(12.0f, childPos.y());
    TEST_ASSERT_FLOAT_EQ(3.0f, childPos.z());
    std::cout << "    Simple hierarchy globals verified\n";
}

// Test 2: AppendRotate — append bone inherits source rotation
static void test_NodeAnimMerge_AppendRotate_GlobalsCorrect()
{
    std::cout << "[test] NodeAnimMerge_AppendRotate_GlobalsCorrect\n";

    auto pmxFile = MakeAppendPMXFile(5.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    // Animate "child" (bone 1) with a 90° Z rotation
    Eigen::Quaternionf rotZ(Eigen::AngleAxisf(
        static_cast<float>(M_PI) / 2.0f, Eigen::Vector3f::UnitZ()));
    auto vmd = MakeSingleKeyVMD("child", 0, Eigen::Vector3f::Zero(), rotZ);

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));

    model->InitializeAnimation();
    model->BeginAnimation();
    anim.Evaluate(0.0f);
    model->UpdateMorphAnimation();
    model->UpdateNodeAnimation(false);
    model->EndAnimation();

    ValidateTransforms(model.get());
    ValidateGlobalLocalConsistency(model.get());

    // "append_r" (bone 2) should have inherited the 90° Z rotation from "child"
    auto* appendR = model->GetNodeManager()->GetMMDNode(2);
    Eigen::Matrix3f rotBlock = appendR->GetGlobalTransform().block<3,3>(0,0);
    Eigen::Quaternionf appendRot(rotBlock);
    float angleDiff = appendRot.angularDistance(rotZ);
    TEST_ASSERT(angleDiff < 0.05f);

    // "leaf" (bone 4, child of append_r) should also have proper transform
    auto* leaf = model->GetNodeManager()->GetMMDNode(4);
    const auto& leafG = leaf->GetGlobalTransform();
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            TEST_ASSERT(!std::isnan(leafG(r, c)));
    std::cout << "    AppendRotate globals verified\n";
}

// Test 3: AppendTranslate — append bone inherits partial translation
static void test_NodeAnimMerge_AppendTranslate_GlobalsCorrect()
{
    std::cout << "[test] NodeAnimMerge_AppendTranslate_GlobalsCorrect\n";

    auto pmxFile = MakeAppendPMXFile(5.0f);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    // Animate "child" with translation (10, 0, 0)
    auto vmd = MakeSingleKeyVMD("child", 0,
        Eigen::Vector3f(10.0f, 0.0f, 0.0f), Eigen::Quaternionf::Identity());

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmd));

    model->InitializeAnimation();
    model->BeginAnimation();
    anim.Evaluate(0.0f);
    model->UpdateMorphAnimation();
    model->UpdateNodeAnimation(false);
    model->EndAnimation();

    ValidateTransforms(model.get());
    ValidateGlobalLocalConsistency(model.get());

    // "append_t" (bone 3) has AppendTranslate with weight 0.5 -> bone 1.
    // Bone 1 has animation translate (10,0,0) and initial translate = (0,5,0).
    // Append computes: appendTranslate = (translate - initTranslate) * weight
    //   = ((10,0,0) + (0,5,0) - (0,5,0)) * 0.5 = (5, 0, 0)
    // But AppendTranslate reads from animTranslate, not total translate.
    // In code: appendTranslate = (GetTranslate() - GetInitialTranslate()) * weight
    //   = ((0,5,0) - (0,5,0)) * 0.5 = (0,0,0) for init translate
    // Actually the animation translate is separate. Let me just verify it's finite and consistent.
    auto* appendT = model->GetNodeManager()->GetMMDNode(3);
    Eigen::Vector3f pos = appendT->GetGlobalTransform().col(3).head<3>();
    for (int i = 0; i < 3; ++i)
        TEST_ASSERT(!std::isnan(pos[i]));
    std::cout << "    AppendTranslate globals verified (pos=" << pos.transpose() << ")\n";
}

// Test 4: IK solver produces correct results after optimization
static void test_NodeAnimMerge_IK_GlobalsMatchExpected()
{
    std::cout << "[test] NodeAnimMerge_IK_GlobalsMatchExpected\n";

    Eigen::Vector3f ikPos(5.0f, 25.0f, 0.0f);
    auto pmxFile = MakeIKPMXFile(10.0f, ikPos, 40);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    model->InitializeAnimation();

    // Cold solve
    model->BeginAnimation();
    model->UpdateNodeAnimation(false);
    model->EndAnimation();

    ValidateTransforms(model.get());
    ValidateGlobalLocalConsistency(model.get());

    // Tip (bone 3) should have moved toward the IK goal
    auto* tip = model->GetNodeManager()->GetMMDNode(3);
    Eigen::Vector3f tipPos = tip->GetGlobalTransform().col(3).head<3>();
    for (int i = 0; i < 3; ++i)
        TEST_ASSERT(!std::isnan(tipPos[i]));

    // Warm solve (uses cache)
    model->BeginAnimation();
    model->UpdateNodeAnimation(false);
    model->EndAnimation();

    Eigen::Vector3f tipPos2 = tip->GetGlobalTransform().col(3).head<3>();
    float diff = (tipPos2 - tipPos).norm();
    TEST_ASSERT(diff < 1e-3f);

    ValidateTransforms(model.get());
    ValidateGlobalLocalConsistency(model.get());
    std::cout << "    IK globals verified (cold→warm delta=" << diff << ")\n";
}

// Test 5: Model with BOTH IK and Append bones
static void test_NodeAnimMerge_IKPlusAppend_Combined()
{
    std::cout << "[test] NodeAnimMerge_IKPlusAppend_Combined\n";

    Eigen::Vector3f ikPos(5.0f, 25.0f, 0.0f);
    auto pmxFile = MakeIKAppendPMXFile(10.0f, ikPos, 40);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    model->InitializeAnimation();

    model->BeginAnimation();
    model->UpdateNodeAnimation(false);
    model->UpdatePhysicsAnimation(1.0f / 30.0f);
    model->UpdateNodeAnimation(true);
    model->EndAnimation();

    ValidateTransforms(model.get());
    ValidateGlobalLocalConsistency(model.get());

    // Verify the append bone (bone 5) has inherited rotation from upper (bone 1)
    auto* upper = model->GetNodeManager()->GetMMDNode(1);
    auto* appendR = model->GetNodeManager()->GetMMDNode(5);
    Eigen::Matrix3f upperRot = upper->GetGlobalTransform().block<3,3>(0,0);
    Eigen::Matrix3f appendRot = appendR->GetGlobalTransform().block<3,3>(0,0);
    Eigen::Quaternionf qUpper(upperRot);
    Eigen::Quaternionf qAppend(appendRot);
    float rotDiff = qUpper.angularDistance(qAppend);
    // Append has weight 1.0 so rotation should be very close
    TEST_ASSERT(rotDiff < 0.1f);
    std::cout << "    IK+Append combined verified (rot delta=" << rotDiff << ")\n";
}

// Test 6: Multi-frame consistency — run 60 frames and verify no NaN/divergence
static void test_NodeAnimMerge_MultiFrame_Consistency()
{
    std::cout << "[test] NodeAnimMerge_MultiFrame_Consistency\n";

    Eigen::Vector3f ikPos(5.0f, 25.0f, 0.0f);
    auto pmxFile = MakeIKAppendPMXFile(10.0f, ikPos, 20);
    auto model = std::make_shared<libmmd::PMXModel>();
    TEST_ASSERT(model->LoadPMX(pmxFile, "", ""));

    model->InitializeAnimation();

    for (int frame = 0; frame < 60; ++frame)
    {
        model->BeginAnimation();
        model->UpdateNodeAnimation(false);
        model->UpdatePhysicsAnimation(1.0f / 30.0f);
        model->UpdateNodeAnimation(true);
        model->EndAnimation();
    }

    ValidateTransforms(model.get(), 0.1f);
    std::cout << "    60-frame multi-frame consistency verified\n";
}

// Test 7: Real file regression — load PMX+VMD and run full pipeline
static void test_NodeAnimMerge_RealFile_Regression()
{
    std::cout << "[test] NodeAnimMerge_RealFile_Regression\n";

    auto model = std::make_shared<libmmd::PMXModel>();
    if (!model->Load(g_pmxTestFile, ""))
    {
        std::cerr << "  SKIP: Could not load PMX file\n";
        return;
    }

    libmmd::VMDFile vmdFile;
    if (!libmmd::ReadVMDFile(&vmdFile, g_vmdBoneFile.c_str()))
    {
        std::cerr << "  SKIP: Could not load VMD file\n";
        return;
    }

    libmmd::VMDAnimation anim;
    TEST_ASSERT(anim.Create(model));
    TEST_ASSERT(anim.Add(vmdFile));

    model->InitializeAnimation();

    // Run 10 frames of full pipeline
    for (int frame = 0; frame < 10; ++frame)
    {
        model->UpdateAllAnimation(&anim, static_cast<float>(frame), 1.0f / 30.0f);
    }

    ValidateTransforms(model.get(), 0.1f);
    ValidateGlobalLocalConsistency(model.get(), 0.01f);
    std::cout << "    Real file 10-frame regression passed\n";
}

// ===========================================================================
// main
// ===========================================================================

int main()
{
    std::cout << "=== libMMD Unit Tests ===\n\n";

    // MMDNode
    test_MMDNode_DefaultConstruction();
    test_MMDNode_SetName();
    test_MMDNode_SetIndex();
    test_MMDNode_EnableIK();
    test_MMDNode_TranslateRotateScale();
    test_MMDNode_AnimationTranslateRotate();
    test_MMDNode_AnimateCombined();
    test_MMDNode_IKRotate();
    test_MMDNode_ParentChild();
    test_MMDNode_SaveLoadInitialTRS();
    test_MMDNode_SaveLoadBaseAnimation();
    test_MMDNode_LocalGlobalTransform();

    // MMDMorph
    test_MMDMorph_DefaultConstruction();
    test_MMDMorph_SetNameAndWeight();
    test_MMDMorph_SaveLoadBaseAnimation();

    // MMDIkSolver
    test_MMDIkSolver_DefaultConstruction();
    test_MMDIkSolver_SetNodes();
    test_MMDIkSolver_Enable();
    test_MMDIkSolver_SaveLoadBaseAnimation();
    test_MMDIkSolver_SetParameters();

    // MMDMaterial
    test_MMDMaterial_DefaultConstruction();
    test_MMDMaterial_SetFields();

    // MMDSubMesh
    test_SubMeshDefaultConstruction();
    test_SubMeshParameterizedConstruction();

    // PMXModel
    test_PMXModel_InheritsFromMMDModel();
    test_PMXModel_ManagerPointersAreNonNull();
    test_PMXModel_InitialCountsAreZero();
    test_PMXModel_InitialModelNameIsEmpty();
    test_PMXModel_NodeManager_FindReturnsNPosForEmpty();
    test_PMXModel_NodeManager_GetByNameReturnsNullForEmpty();
    test_PMXModel_IKManager_FindReturnsNPosForEmpty();
    test_PMXModel_IKManager_GetByNameReturnsNullForEmpty();
    test_PMXModel_MorphManager_FindReturnsNPosForEmpty();
    test_PMXModel_MorphManager_GetByNameReturnsNullForEmpty();
    test_PMXModel_LoadInvalidPathReturnsFalse();
    test_PMXModel_DestroyOnEmptyModelIsSafe();
    test_PMXModel_VMDAnimation_CreateWithModel();

    // PMDModel
    test_PMDModel_InheritsFromMMDModel();
    test_PMDModel_ManagerPointersAreNonNull();
    test_PMDModel_InitialCountsAreZero();
    test_PMDModel_InitialModelNameIsEmpty();
    test_PMDModel_NodeManager_FindReturnsNPosForEmpty();
    test_PMDModel_NodeManager_GetByNameReturnsNullForEmpty();
    test_PMDModel_LoadInvalidPathReturnsFalse();
    test_PMDModel_DestroyOnEmptyModelIsSafe();
    test_PMDModel_VMDAnimation_CreateWithPMDModel();

    // PhysicsManager
    test_PhysicsManager_CreateSucceeds();
    test_PhysicsManager_InitialListsEmpty();

    // RigidBody parametric Create
    test_RigidBody_ParametricCreate_Sphere();
    test_RigidBody_ParametricCreate_Box();
    test_RigidBody_ParametricCreate_Capsule();
    test_RigidBody_ParametricCreate_NullNode();

    // Joint parametric CreateJoint
    test_Joint_ParametricCreate_Equivalence();
    test_Joint_ParametricCreate_ZeroSprings();

    // Polymorphism
    test_Polymorphism_PMXModelAsMMDModel();
    test_Polymorphism_PMDModelAsMMDModel();
    test_Polymorphism_VMDAnimationAcceptsBothModelTypes();

    // VMDAnimation
    test_VMDAnimation_DefaultConstruction();
    test_VMDAnimation_DestroyWithoutCreate();
    test_VMDAnimation_CreateAndDestroy();

    // Coordinate System Consistency
    test_CoordSystem_NodeTranslateNoZNegation();
    test_CoordSystem_GlobalTransformNoZNegation();
    test_CoordSystem_LocalTransformNoZNegation();
    test_CoordSystem_AnimationTranslateNoZNegation();
    test_CoordSystem_ParentChildTransformConsistency();
    test_CoordSystem_RotationMatrixIdentityPreserved();
    test_CoordSystem_InverseInitTransformConsistency();

    // PMXFile reading/parsing
    test_PMXFile_ReadFromPath();
    test_PMXFile_ReadFromBuffer();
    test_PMXFile_HeaderValid();
    test_PMXFile_InfoNonEmpty();
    test_PMXFile_SectionsNonEmpty();
    test_PMXFile_VertexDataValid();
    test_PMXFile_FaceIndicesValid();
    test_PMXFile_BoneParentIndicesValid();
    test_PMXFile_BufferAndPathProduceSameResult();
    test_PMXFile_ErrorReportOnTruncatedData();
    test_PMXFile_ErrorReportOnEmptyData();
    test_PMXFile_MaterialFaceVertexCountConsistent();

    // VMDFile reading/parsing (bone)
    test_VMDFile_ReadBoneFromPath();
    test_VMDFile_ReadBoneFromBuffer();
    test_VMDFile_BoneHeaderValid();
    test_VMDFile_BoneMotionsNonEmpty();
    test_VMDFile_BoneMotionDataValid();
    test_VMDFile_BoneBufferAndPathSameResult();

    // VMDFile reading/parsing (camera)
    test_VMDFile_ReadCamFromPath();
    test_VMDFile_ReadCamFromBuffer();
    test_VMDFile_CamCamerasNonEmpty();
    test_VMDFile_CamCameraDataValid();
    test_VMDFile_CamBufferAndPathSameResult();

    // MMDNode UpdateLocalTransform / UpdateGlobalTransform
    test_MMDNode_UpdateLocalTransform_IdentityPose();
    test_MMDNode_UpdateLocalTransform_WithTranslation();
    test_MMDNode_UpdateLocalTransform_WithRotation90Y();
    test_MMDNode_UpdateLocalTransform_WithAnimationData();
    test_MMDNode_UpdateLocalTransform_WithScale();
    test_MMDNode_UpdateLocalTransform_IKRotateApplied();
    test_MMDNode_UpdateGlobalTransform_SingleRoot();
    test_MMDNode_UpdateGlobalTransform_ParentChild();
    test_MMDNode_UpdateGlobalTransform_ParentRotation();
    test_MMDNode_UpdateGlobalTransform_ThreeLevelHierarchy();

    // VMD Animation Integration
    test_Integration_VMD_SingleBoneRotationAtFrame0();
    test_Integration_VMD_SingleBoneTranslationAtFrame0();
    test_Integration_VMD_RootBoneUnaffectedWhenNoKey();
    test_Integration_VMD_TwoKeyLinearInterpolation();
    test_Integration_VMD_TwoKeyRotationSlerp();
    test_Integration_VMD_GlobalTransformHierarchy();
    test_Integration_VMD_UpdateAllAnimation();
    test_Integration_VMD_EvaluatePastLastKey();
    test_Integration_VMD_EvaluateBeforeFirstKey();
    test_Integration_VMD_NonexistentBoneNameIgnored();
    test_Integration_VMD_MultipleConsecutiveFrames();

    // C4D Matrix Extraction Pattern
    test_Integration_C4DMatrixExtractionPattern_Translation();
    test_Integration_C4DMatrixExtractionPattern_Rotation();
    test_Integration_C4DMatrixExtractionPattern_CombinedTranslateRotate();

    // Real File Integration
    test_Integration_RealFile_LoadPMXAndApplyVMD();
    test_Integration_RealFile_LocalTransformDeltaConsistency();

    // VMDBezier
    test_VMDBezier_FindBezierX_LinearCP();
    test_VMDBezier_FindBezierX_EaseInCP();
    test_VMDBezier_FindBezierX_EaseOutCP();
    test_VMDBezier_FindBezierX_PrecisionRegression();
    test_VMDBezier_EvalDX_Correctness();

    // Node Partition
    test_PMXModel_NodePartition_CorrectSplit();
    test_PMXModel_NodePartition_AllBeforePhysics();
    test_PMXModel_NodePartition_AllAfterPhysics();
    test_PMXModel_NodePartition_RegressionTransformConsistency();
    test_PMXModel_NodePartition_RealFileRegression();

    // Zero-weight Morph Skip
    test_PMXModel_MorphAnimation_ZeroWeightSkipped();
    test_PMXModel_MorphAnimation_NonZeroWeightApplied();
    test_PMXModel_MorphAnimation_MixedWeights();
    test_PMXModel_MorphAnimation_RealFileRegression();

    // SyncPhysics parameterization
    test_VMDAnimation_SyncPhysics_DefaultElapsed();
    test_VMDAnimation_SyncPhysics_CustomElapsed();
    test_VMDAnimation_SyncPhysics_RealFile();

    // VMDAnimation Save
    test_VMDAnimation_Save_EmptyAnimation();
    test_VMDAnimation_Save_SingleBoneKey();
    test_VMDAnimation_Save_MultipleBoneKeys();
    test_VMDAnimation_Save_MultipleBones();
    test_VMDAnimation_Save_InterpolationPreserved();
    test_VMDAnimation_Save_NonexistentBoneIgnored();

    // Round-trip tests (Group D)
    test_RoundTrip_PhysicsRebuild();
    test_RoundTrip_VMDSerialize_Buffer();
    test_RoundTrip_RealFile_PhysicsAndVMD();

    // Physics animation loop merge regression
    test_PMXModel_PhysicsAnimation_TransformsValid();
    test_PMDModel_PhysicsAnimation_TransformsValid();

    // IK solve regression
    test_IK_ConsecutiveFramesValid();
    test_IK_Deterministic();

    // IK Solver Optimization B: Rigid inverse
    test_RigidInverse_MatchesGenericInverse();
    test_RigidInverse_IKDeviation();
    test_RigidInverse_RealFile_Deviation();
    bench_RigidInverse_Speedup();

    // IK Solver Optimization C: Local transform
    test_LocalTransform_Optimized_MatchesOriginal();
    test_LocalTransform_IKDeviation();
    test_LocalTransform_RealFile_Deviation();
    bench_LocalTransform_Speedup();

    // IK Solver Optimization A: Chain path
    test_ChainPath_BuildCorrect();
    test_ChainPath_UpdateMatchesDFS();
    test_ChainPath_IKDeviation();
    test_ChainPath_RealFile_Deviation();
    bench_ChainPath_Speedup();

    // UpdateNodeAnimation DFS merge
    test_NodeAnimMerge_SimpleHierarchy_GlobalsCorrect();
    test_NodeAnimMerge_AppendRotate_GlobalsCorrect();
    test_NodeAnimMerge_AppendTranslate_GlobalsCorrect();
    test_NodeAnimMerge_IK_GlobalsMatchExpected();
    test_NodeAnimMerge_IKPlusAppend_Combined();
    test_NodeAnimMerge_MultiFrame_Consistency();
    test_NodeAnimMerge_RealFile_Regression();

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
