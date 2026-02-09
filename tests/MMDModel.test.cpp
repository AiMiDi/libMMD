#include <libMMD/Model/MMD/PMXModel.h>
#include <libMMD/Model/MMD/PMDModel.h>
#include <libMMD/Model/MMD/VMDAnimation.h>
#include <libMMD/Model/MMD/MMDNode.h>
#include <libMMD/Model/MMD/MMDMorph.h>
#include <libMMD/Model/MMD/MMDIkSolver.h>
#include <libMMD/Model/MMD/MMDMaterial.h>

#include <iostream>
#include <memory>
#include <string>
#include <cmath>
#include <cstdlib>

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
