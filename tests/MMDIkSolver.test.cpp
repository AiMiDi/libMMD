#define _USE_MATH_DEFINES
#include <cmath>

#include <libMMD/Model/MMD/MMDIkSolver.h>
#include <libMMD/Model/MMD/MMDNode.h>

#if defined(__has_include)
#  if __has_include(<libMMD/Model/MMD/IMMDNode.h>)
#    include <libMMD/Model/MMD/IMMDNode.h>
#    define LIBMMD_HAS_IMMDNODE 1
#  else
#    define LIBMMD_HAS_IMMDNODE 0
#  endif
#else
#  define LIBMMD_HAS_IMMDNODE 0
#endif

#include <array>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

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

#define TEST_ASSERT_FLOAT_EQ(expected, actual)                             \
    do {                                                                   \
        ++g_totalTests;                                                    \
        if (std::fabs((expected) - (actual)) > 1e-4f) {                    \
            std::cerr << "  FAIL: expected " << (expected)                 \
                      << " but got " << (actual) << "  (" << __FILE__      \
                      << ":" << __LINE__ << ")\n";                         \
            ++g_failedTests;                                               \
        }                                                                  \
    } while (0)

namespace
{
constexpr float kPi = 3.14159265358979323846f;

#if LIBMMD_HAS_IMMDNODE
/**
 * A tiny test-only IMMDNode implementation.
 *
 * The current tree still uses MMDNode directly, but this mock keeps the test
 * source aligned with the future IMMDNode-based solver API.
 */
class MockIMMDNode final : public libmmd::IMMDNode
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    MockIMMDNode() = default;

    void AddChild(MockIMMDNode* child)
    {
        if (child == nullptr)
        {
            return;
        }

        child->m_parent = this;
        if (m_child == nullptr)
        {
            m_child = child;
            m_child->m_prev = m_child;
        }
        else
        {
            MockIMMDNode* last = m_child->m_prev;
            last->m_next = child;
            child->m_prev = last;
            m_child->m_prev = child;
        }
    }

    void SetName(const std::string& name) { m_name = name; }
    const std::string& GetName() const override { return m_name; }

    void SetTranslate(const Eigen::Vector3f& t) { m_translate = t; }
    void SetRotate(const Eigen::Quaternionf& r) { m_rotate = r; }
    void SetScale(const Eigen::Vector3f& s) { m_scale = s; }
    void SetAnimationTranslate(const Eigen::Vector3f& t) { m_animTranslate = t; }
    void SetAnimationRotate(const Eigen::Quaternionf& q) { m_animRotate = q; }
    void EnableIK(const bool enable) { m_enableIK = enable; }

    MockIMMDNode* GetChild() const { return m_child; }
    MockIMMDNode* GetNext() const { return m_next; }
    MockIMMDNode* GetPrev() const { return m_prev; }

    libmmd::IMMDNode* GetParent() const override { return m_parent; }
    const Eigen::Matrix4f& GetLocalTransform() const override { return m_local; }
    const Eigen::Matrix4f& GetGlobalTransform() const override { return m_global; }
    const Eigen::Matrix4f& GetInitialGlobalTransform() const override { return m_initialGlobal; }
    void SetGlobalTransform(const Eigen::Matrix4f& m) override { m_global = m; }
    void SetIKRotate(const Eigen::Quaternionf& q) override { m_ikRotate = q; }
    const Eigen::Quaternionf& GetIKRotate() const override { return m_ikRotate; }
    Eigen::Quaternionf AnimateRotate() const override { return m_animRotate * m_rotate; }

    void UpdateLocalTransform() override
    {
        const Eigen::Quaternionf rot = m_enableIK ? (m_ikRotate * AnimateRotate()) : AnimateRotate();
        m_local.setIdentity();
        m_local.block<3, 3>(0, 0).noalias() = rot.toRotationMatrix() * m_scale.asDiagonal();
        m_local.block<3, 1>(0, 3) = m_animTranslate + m_translate;
    }

    void UpdateGlobalTransform() override
    {
        std::vector<MockIMMDNode*> nodeStack;
        nodeStack.push_back(this);

        while (!nodeStack.empty())
        {
            MockIMMDNode* currentNode = nodeStack.back();
            nodeStack.pop_back();

            if (currentNode->m_parent == nullptr)
            {
                currentNode->m_global = currentNode->m_local;
            }
            else
            {
                currentNode->m_global = currentNode->m_parent->m_global * currentNode->m_local;
            }

            for (MockIMMDNode* child = currentNode->m_child; child != nullptr; child = child->m_next)
            {
                nodeStack.push_back(child);
            }
        }
    }

private:
    std::string m_name;
    bool m_enableIK = false;

    MockIMMDNode* m_parent = nullptr;
    MockIMMDNode* m_child = nullptr;
    MockIMMDNode* m_next = nullptr;
    MockIMMDNode* m_prev = nullptr;

    Eigen::Vector3f m_translate = Eigen::Vector3f::Zero();
    Eigen::Quaternionf m_rotate = Eigen::Quaternionf::Identity();
    Eigen::Vector3f m_scale = Eigen::Vector3f::Ones();
    Eigen::Vector3f m_animTranslate = Eigen::Vector3f::Zero();
    Eigen::Quaternionf m_animRotate = Eigen::Quaternionf::Identity();
    Eigen::Quaternionf m_ikRotate = Eigen::Quaternionf::Identity();
    Eigen::Matrix4f m_local = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f m_global = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f m_initialGlobal = Eigen::Matrix4f::Identity();
};

using TestNode = MockIMMDNode;
#else
using TestNode = libmmd::MMDNode;
#endif

template <typename TNode>
static void ResetNodePose(TNode& node, bool enableIK)
{
    node.SetRotate(Eigen::Quaternionf::Identity());
    node.SetScale(Eigen::Vector3f::Ones());
    node.SetAnimationTranslate(Eigen::Vector3f::Zero());
    node.SetAnimationRotate(Eigen::Quaternionf::Identity());
    node.SetIKRotate(Eigen::Quaternionf::Identity());
    node.EnableIK(enableIK);
}

template <typename TNode>
static float GlobalDistance(const TNode& a, const TNode& b)
{
    return (a.GetGlobalTransform().col(3).template head<3>() - b.GetGlobalTransform().col(3).template head<3>()).norm();
}

template <typename TNode>
static void AssertFiniteMatrix(const TNode& node)
{
    const auto& m = node.GetGlobalTransform();
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            TEST_ASSERT(std::isfinite(m(r, c)));
        }
    }
}

template <typename TNode>
struct SimpleIkChainFixture
{
    TNode root;
    TNode mid;
    TNode hinge;
    TNode target;
    TNode ik;

    void Build()
    {
        root.SetName("root");
        mid.SetName("mid");
        hinge.SetName("hinge");
        target.SetName("target");
        ik.SetName("ik");

        root.AddChild(&mid);
        mid.AddChild(&hinge);
        mid.AddChild(&target);
        hinge.AddChild(&ik);

        root.SetTranslate(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
        mid.SetTranslate(Eigen::Vector3f(0.0f, 10.0f, 0.0f));
        hinge.SetTranslate(Eigen::Vector3f(0.0f, 10.0f, 0.0f));
        target.SetTranslate(Eigen::Vector3f(10.0f, 0.0f, 0.0f));
        ik.SetTranslate(Eigen::Vector3f(0.0f, 10.0f, 0.0f));

        ResetNodePose(root, true);
        ResetNodePose(mid, true);
        ResetNodePose(hinge, true);
        ResetNodePose(target, false);
        ResetNodePose(ik, false);

        root.SetTranslate(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
        mid.SetTranslate(Eigen::Vector3f(0.0f, 10.0f, 0.0f));
        hinge.SetTranslate(Eigen::Vector3f(0.0f, 10.0f, 0.0f));
        target.SetTranslate(Eigen::Vector3f(10.0f, 0.0f, 0.0f));
        ik.SetTranslate(Eigen::Vector3f(0.0f, 10.0f, 0.0f));

        root.UpdateLocalTransform();
        mid.UpdateLocalTransform();
        hinge.UpdateLocalTransform();
        target.UpdateLocalTransform();
        ik.UpdateLocalTransform();
        root.UpdateGlobalTransform();
    }

    void ConfigureSolver(libmmd::MMDIkSolver& solver)
    {
        solver.ClearIKChains();
        solver.SetIKNode(&ik);
        solver.SetTargetNode(&target);
        solver.SetIterateCount(96);
        solver.SetLimitAngle(2.0f * kPi);
        solver.Enable(true);
        solver.AddIKChain(&root);
        solver.AddIKChain(&mid);
        solver.AddIKChain(&hinge);
        solver.BuildChainPath();
    }

    std::array<TNode*, 5> Nodes()
    {
        return { &root, &mid, &hinge, &target, &ik };
    }

    std::array<const TNode*, 5> Nodes() const
    {
        return { &root, &mid, &hinge, &target, &ik };
    }
};

template <typename TNode>
static void ValidateHierarchy(const SimpleIkChainFixture<TNode>& fixture)
{
    TEST_ASSERT(fixture.mid.GetParent() == &fixture.root);
    TEST_ASSERT(fixture.hinge.GetParent() == &fixture.mid);
    TEST_ASSERT(fixture.target.GetParent() == &fixture.mid);
    TEST_ASSERT(fixture.ik.GetParent() == &fixture.hinge);
}

template <typename TNode>
static void ValidateFiniteTransforms(const SimpleIkChainFixture<TNode>& fixture)
{
    for (auto* node : fixture.Nodes())
    {
        AssertFiniteMatrix(*node);
    }
}

} // namespace

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

static void test_MMDIkSolver_SolveConverges_SimpleChain()
{
    std::cout << "[test] MMDIkSolver_SolveConverges_SimpleChain\n";

    SimpleIkChainFixture<TestNode> fixture;
    fixture.Build();
    ValidateHierarchy(fixture);

    libmmd::MMDIkSolver solver;
    fixture.ConfigureSolver(solver);

    TEST_ASSERT(solver.GetName() == std::string("ik"));
    TEST_ASSERT(solver.GetIKNode() == &fixture.ik);
    TEST_ASSERT(solver.GetTargetNode() == &fixture.target);

    const float before = GlobalDistance(fixture.target, fixture.ik);
    TEST_ASSERT(before > 1.0f);

    solver.Solve();

    const float after = GlobalDistance(fixture.target, fixture.ik);
    TEST_ASSERT(after < before);
    TEST_ASSERT(after < before * 0.5f);
    ValidateFiniteTransforms(fixture);
}

static void test_MMDIkSolver_RealMMDNode_Regression()
{
    std::cout << "[test] MMDIkSolver_RealMMDNode_Regression\n";

    SimpleIkChainFixture<libmmd::MMDNode> fixtureA;
    fixtureA.Build();
    ValidateHierarchy(fixtureA);

    libmmd::MMDIkSolver solverA;
    fixtureA.ConfigureSolver(solverA);
    const float before = GlobalDistance(fixtureA.target, fixtureA.ik);
    solverA.Solve();
    const float afterFirstSolve = GlobalDistance(fixtureA.target, fixtureA.ik);

    SimpleIkChainFixture<libmmd::MMDNode> fixtureB;
    fixtureB.Build();
    ValidateHierarchy(fixtureB);

    libmmd::MMDIkSolver solverB;
    fixtureB.ConfigureSolver(solverB);
    solverB.Solve();
    const float afterSecondSolve = GlobalDistance(fixtureB.target, fixtureB.ik);

    TEST_ASSERT(afterFirstSolve < before);
    TEST_ASSERT(afterSecondSolve < before);
    TEST_ASSERT(std::fabs(afterSecondSolve - afterFirstSolve) < 1e-4f);
    ValidateFiniteTransforms(fixtureA);
    ValidateFiniteTransforms(fixtureB);
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

int main()
{
    std::cout << "=== MMDIkSolver Tests ===\n";

    test_MMDIkSolver_SolveConverges_SimpleChain();
    test_MMDIkSolver_RealMMDNode_Regression();

    std::cout << "\nResults: " << (g_totalTests - g_failedTests) << " / "
              << g_totalTests << " passed\n";
    if (g_failedTests > 0)
    {
        std::cerr << g_failedTests << " test(s) FAILED\n";
        return EXIT_FAILURE;
    }
    std::cout << "All tests passed.\n";
    return EXIT_SUCCESS;
}
