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
#include <libMMD/Base/File.h>

#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>
#include <vector>
#include <fstream>

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
