#define _USE_MATH_DEFINES
#include <cmath>

#include <libMMD/Model/MMD/IMMDNode.h>
#include <libMMD/Model/MMD/MMDModel.h>
#include <libMMD/Model/MMD/MMDNode.h>
#include <libMMD/Model/MMD/MMDPhysics.h>

#include <btBulletDynamicsCommon.h>

#include <iostream>
#include <memory>
#include <cstdlib>

// ---------------------------------------------------------------------------
// Minimal test framework (same as other libMMD tests)
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
        if (std::fabs((expected) - (actual)) > 1e-3f) {                    \
            std::cerr << "  FAIL: expected " << (expected)                 \
                      << " but got " << (actual) << "  (" << __FILE__      \
                      << ":" << __LINE__ << ")\n";                         \
            ++g_failedTests;                                               \
        }                                                                  \
    } while (0)

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

struct TestRigidBody
{
    std::unique_ptr<btCollisionShape> shape;
    std::unique_ptr<btDefaultMotionState> motionState;
    std::unique_ptr<btRigidBody> body;

    static TestRigidBody CreateDynamic(float mass, const btVector3& startPos, float radius = 0.5f)
    {
        TestRigidBody rb;
        rb.shape = std::make_unique<btSphereShape>(radius);

        btVector3 localInertia(0, 0, 0);
        rb.shape->calculateLocalInertia(mass, localInertia);

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(startPos);
        rb.motionState = std::make_unique<btDefaultMotionState>(startTransform);

        btRigidBody::btRigidBodyConstructionInfo info(mass, rb.motionState.get(), rb.shape.get(), localInertia);
        rb.body = std::make_unique<btRigidBody>(info);
        rb.body->setActivationState(DISABLE_DEACTIVATION);
        return rb;
    }

    btVector3 GetPosition() const
    {
        btTransform t;
        body->getMotionState()->getWorldTransform(t);
        return t.getOrigin();
    }
};

struct MockIMMDNode final : libmmd::IMMDNode
{
    std::string name = "mock";
    Eigen::Matrix4f global = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f initialGlobal = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f local = Eigen::Matrix4f::Identity();
    Eigen::Quaternionf ikRotate = Eigen::Quaternionf::Identity();
    libmmd::IMMDNode* parent = nullptr;
    int setGlobalCalls = 0;
    Eigen::Matrix4f lastSetGlobal = Eigen::Matrix4f::Identity();

    const std::string& GetName() const override
    {
        return name;
    }

    void SetGlobalTransform(const Eigen::Matrix4f& m) override
    {
        global = m;
        lastSetGlobal = m;
        ++setGlobalCalls;
    }

    const Eigen::Matrix4f& GetGlobalTransform() const override
    {
        return global;
    }

    const Eigen::Matrix4f& GetInitialGlobalTransform() const override
    {
        return initialGlobal;
    }

    const Eigen::Matrix4f& GetLocalTransform() const override
    {
        return local;
    }

    void SetIKRotate(const Eigen::Quaternionf& ikr) override
    {
        ikRotate = ikr;
    }

    const Eigen::Quaternionf& GetIKRotate() const override
    {
        return ikRotate;
    }

    Eigen::Quaternionf AnimateRotate() const override
    {
        return Eigen::Quaternionf::Identity();
    }

    void UpdateLocalTransform() override
    {
    }

    void UpdateGlobalTransform() override
    {
    }

    libmmd::IMMDNode* GetParent() const override
    {
        return parent;
    }
};

static Eigen::Matrix4f MakeTranslate(float x, float y, float z)
{
    Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();
    mat(0, 3) = x;
    mat(1, 3) = y;
    mat(2, 3) = z;
    return mat;
}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

static void test_MMDPhysics_CreateAndDestroy()
{
    std::cout << "[test] MMDPhysics_CreateAndDestroy\n";

    libmmd::MMDPhysics physics;
    bool ok = physics.Create();
    TEST_ASSERT(ok);

    auto* world = physics.GetDynamicsWorld();
    TEST_ASSERT(world != nullptr);

    physics.Destroy();
    TEST_ASSERT(physics.GetDynamicsWorld() == nullptr);
}

static void test_MMDPhysics_GravitySimulation()
{
    std::cout << "[test] MMDPhysics_GravitySimulation\n";

    libmmd::MMDPhysics physics;
    TEST_ASSERT(physics.Create());

    auto* world = physics.GetDynamicsWorld();
    TEST_ASSERT(world != nullptr);

    auto rb = TestRigidBody::CreateDynamic(1.0f, btVector3(0, 10, 0));
    world->addRigidBody(rb.body.get());

    float initialY = rb.GetPosition().y();
    TEST_ASSERT_FLOAT_EQ(10.0f, initialY);

    for (int i = 0; i < 60; ++i)
    {
        physics.Update(1.0f / 60.0f);
    }

    float finalY = rb.GetPosition().y();
    TEST_ASSERT(finalY < initialY);
    TEST_ASSERT(finalY >= 0.0f);

    world->removeRigidBody(rb.body.get());
    physics.Destroy();
}

static void test_MMDPhysics_SetFPS()
{
    std::cout << "[test] MMDPhysics_SetFPS\n";

    libmmd::MMDPhysics physics;
    TEST_ASSERT(physics.Create());

    TEST_ASSERT_FLOAT_EQ(120.0f, physics.GetFPS());

    physics.SetFPS(60.0f);
    TEST_ASSERT_FLOAT_EQ(60.0f, physics.GetFPS());

    physics.SetFPS(240.0f);
    TEST_ASSERT_FLOAT_EQ(240.0f, physics.GetFPS());

    physics.SetMaxSubStepCount(20);
    TEST_ASSERT(physics.GetMaxSubStepCount() == 20);

    physics.Destroy();
}

static void test_MMDPhysics_MultipleRigidBodies()
{
    std::cout << "[test] MMDPhysics_MultipleRigidBodies\n";

    libmmd::MMDPhysics physics;
    TEST_ASSERT(physics.Create());

    auto* world = physics.GetDynamicsWorld();

    constexpr int NUM_BODIES = 10;
    std::vector<TestRigidBody> bodies;
    bodies.reserve(NUM_BODIES);
    for (int i = 0; i < NUM_BODIES; ++i)
    {
        bodies.push_back(TestRigidBody::CreateDynamic(
            1.0f, btVector3(static_cast<float>(i) * 2.0f, 5.0f + static_cast<float>(i), 0)));
        world->addRigidBody(bodies.back().body.get());
    }

    for (int step = 0; step < 120; ++step)
    {
        physics.Update(1.0f / 60.0f);
    }

    for (int i = 0; i < NUM_BODIES; ++i)
    {
        float y = bodies[i].GetPosition().y();
        TEST_ASSERT(y < 5.0f + static_cast<float>(i));
        TEST_ASSERT(y >= -1.0f);
    }

    for (auto& rb : bodies)
    {
        world->removeRigidBody(rb.body.get());
    }
    bodies.clear();
    physics.Destroy();
}

static void test_MMDPhysics_RepeatedCreateDestroy()
{
    std::cout << "[test] MMDPhysics_RepeatedCreateDestroy\n";

    for (int cycle = 0; cycle < 5; ++cycle)
    {
        libmmd::MMDPhysics physics;
        TEST_ASSERT(physics.Create());

        auto* world = physics.GetDynamicsWorld();
        TEST_ASSERT(world != nullptr);

        auto rb = TestRigidBody::CreateDynamic(1.0f, btVector3(0, 5, 0));
        world->addRigidBody(rb.body.get());

        for (int step = 0; step < 30; ++step)
        {
            physics.Update(1.0f / 60.0f);
        }

        TEST_ASSERT(rb.GetPosition().y() < 5.0f);

        world->removeRigidBody(rb.body.get());
        physics.Destroy();
    }
}

static void test_MMDRigidBody_ExternalCreate_NullNodeKeepsPmxWorldTransform()
{
    std::cout << "[test] MMDRigidBody_ExternalCreate_NullNodeKeepsPmxWorldTransform\n";

    libmmd::PMXRigidbody rb;
    rb.m_shape = libmmd::PMXRigidbody::Shape::Sphere;
    rb.m_shapeSize = Eigen::Vector3f(0.5f, 0.0f, 0.0f);
    rb.m_translate = Eigen::Vector3f(12.0f, 3.0f, -4.0f);
    rb.m_op = libmmd::PMXRigidbody::Operation::Static;

    libmmd::MMDRigidBody body;
    TEST_ASSERT(body.Create(rb, static_cast<libmmd::IMMDNode*>(nullptr)));
    TEST_ASSERT(body.GetRigidBody() != nullptr);

    btTransform world;
    body.GetRigidBody()->getMotionState()->getWorldTransform(world);
    TEST_ASSERT_FLOAT_EQ(12.0f, world.getOrigin().x());
    TEST_ASSERT_FLOAT_EQ(3.0f, world.getOrigin().y());
    TEST_ASSERT_FLOAT_EQ(-4.0f, world.getOrigin().z());
}

static void test_MMDRigidBody_ExternalCreate_RealMMDNodeUsesCapturedInitialGlobal()
{
    std::cout << "[test] MMDRigidBody_ExternalCreate_RealMMDNodeUsesCapturedInitialGlobal\n";

    libmmd::PMXRigidbody rb;
    rb.m_shape = libmmd::PMXRigidbody::Shape::Sphere;
    rb.m_shapeSize = Eigen::Vector3f(0.5f, 0.0f, 0.0f);
    rb.m_translate = Eigen::Vector3f(12.0f, 0.0f, 0.0f);
    rb.m_rotate = Eigen::Vector3f::Zero();
    rb.m_op = libmmd::PMXRigidbody::Operation::Static;

    libmmd::MMDNode node;
    node.SetGlobalTransform(MakeTranslate(10.0f, 0.0f, 0.0f));
    node.CalculateInverseInitTransform();
    node.SetGlobalTransform(MakeTranslate(20.0f, 0.0f, 0.0f));

    libmmd::MMDRigidBody body;
    TEST_ASSERT(body.Create(rb, static_cast<libmmd::IMMDNode*>(&node)));

    btTransform world;
    body.GetRigidBody()->getMotionState()->getWorldTransform(world);
    TEST_ASSERT_FLOAT_EQ(22.0f, world.getOrigin().x());
    TEST_ASSERT_FLOAT_EQ(0.0f, world.getOrigin().y());
    TEST_ASSERT_FLOAT_EQ(0.0f, world.getOrigin().z());
}

static void test_MMDRigidBody_ExternalKinematic_UsesCurrentGlobalAndInitialOffset()
{
    std::cout << "[test] MMDRigidBody_ExternalKinematic_UsesCurrentGlobalAndInitialOffset\n";

    libmmd::PMXRigidbody rb;
    rb.m_shape = libmmd::PMXRigidbody::Shape::Sphere;
    rb.m_shapeSize = Eigen::Vector3f(0.5f, 0.0f, 0.0f);
    rb.m_translate = Eigen::Vector3f(12.0f, 0.0f, 0.0f);
    rb.m_rotate = Eigen::Vector3f::Zero();
    rb.m_op = libmmd::PMXRigidbody::Operation::Static;

    MockIMMDNode node;
    node.initialGlobal = MakeTranslate(10.0f, 0.0f, 0.0f);
    node.global = node.initialGlobal;

    libmmd::MMDRigidBody body;
    TEST_ASSERT(body.Create(rb, &node));

    node.global = MakeTranslate(20.0f, 0.0f, 0.0f);

    btTransform world;
    body.GetRigidBody()->getMotionState()->getWorldTransform(world);
    TEST_ASSERT_FLOAT_EQ(22.0f, world.getOrigin().x());
    TEST_ASSERT_FLOAT_EQ(0.0f, world.getOrigin().y());
    TEST_ASSERT_FLOAT_EQ(0.0f, world.getOrigin().z());

    body.CalcLocalTransform();
    TEST_ASSERT_FLOAT_EQ(20.0f, node.global(0, 3));
}

static void test_MMDRigidBody_ExternalDynamic_ReflectWritesBackToNode()
{
    std::cout << "[test] MMDRigidBody_ExternalDynamic_ReflectWritesBackToNode\n";

    libmmd::PMXRigidbody rb;
    rb.m_shape = libmmd::PMXRigidbody::Shape::Sphere;
    rb.m_shapeSize = Eigen::Vector3f(0.5f, 0.0f, 0.0f);
    rb.m_translate = Eigen::Vector3f(0.0f, 10.0f, 0.0f);
    rb.m_rotate = Eigen::Vector3f::Zero();
    rb.m_mass = 1.0f;
    rb.m_op = libmmd::PMXRigidbody::Operation::Dynamic;

    MockIMMDNode node;
    node.initialGlobal = MakeTranslate(0.0f, 10.0f, 0.0f);
    node.global = node.initialGlobal;

    libmmd::MMDRigidBody body;
    TEST_ASSERT(body.Create(rb, &node));

    btTransform forced;
    forced.setIdentity();
    forced.setOrigin(btVector3(3.0f, 4.0f, 5.0f));
    body.GetRigidBody()->setCenterOfMassTransform(forced);
    body.GetRigidBody()->getMotionState()->setWorldTransform(forced);

    body.ReflectGlobalTransform();

    TEST_ASSERT(node.setGlobalCalls > 0);
    TEST_ASSERT_FLOAT_EQ(3.0f, node.lastSetGlobal(0, 3));
    TEST_ASSERT_FLOAT_EQ(4.0f, node.lastSetGlobal(1, 3));
    TEST_ASSERT_FLOAT_EQ(5.0f, node.lastSetGlobal(2, 3));
}

static void test_MMDRigidBody_ExternalDynamicAndBoneMerge_SyncsPositionAndVelocity()
{
    std::cout << "[test] MMDRigidBody_ExternalDynamicAndBoneMerge_SyncsPositionAndVelocity\n";

    libmmd::PMXRigidbody rb;
    rb.m_shape = libmmd::PMXRigidbody::Shape::Sphere;
    rb.m_shapeSize = Eigen::Vector3f(0.5f, 0.0f, 0.0f);
    rb.m_translate = Eigen::Vector3f(3.0f, 0.0f, 0.0f);
    rb.m_rotate = Eigen::Vector3f::Zero();
    rb.m_mass = 1.0f;
    rb.m_op = libmmd::PMXRigidbody::Operation::DynamicAndBoneMerge;

    MockIMMDNode node;
    node.initialGlobal = MakeTranslate(3.0f, 0.0f, 0.0f);
    node.global = node.initialGlobal;

    libmmd::MMDRigidBody body;
    TEST_ASSERT(body.Create(rb, &node));

    node.global = MakeTranslate(5.0f, 0.0f, 0.0f);
    body.SyncBonePositionToPhysics(0.5f);

    const Eigen::Matrix4f transform = body.GetTransform();
    TEST_ASSERT_FLOAT_EQ(5.0f, transform(0, 3));
    TEST_ASSERT_FLOAT_EQ(0.0f, transform(1, 3));
    TEST_ASSERT_FLOAT_EQ(0.0f, transform(2, 3));
    TEST_ASSERT_FLOAT_EQ(4.0f, body.GetRigidBody()->getLinearVelocity().x());
}

static void test_MMDPhysicsManager_DestroyWithoutCreate()
{
    std::cout << "[test] MMDPhysicsManager_DestroyWithoutCreate\n";

    auto manager = std::make_unique<libmmd::MMDPhysicsManager>();
    manager.reset();

    TEST_ASSERT(true);
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

int main()
{
    std::cout << "=== MMDPhysics Tests ===\n";

    test_MMDPhysics_CreateAndDestroy();
    test_MMDPhysics_GravitySimulation();
    test_MMDPhysics_SetFPS();
    test_MMDPhysics_MultipleRigidBodies();
    test_MMDPhysics_RepeatedCreateDestroy();
    test_MMDRigidBody_ExternalCreate_NullNodeKeepsPmxWorldTransform();
    test_MMDRigidBody_ExternalCreate_RealMMDNodeUsesCapturedInitialGlobal();
    test_MMDRigidBody_ExternalKinematic_UsesCurrentGlobalAndInitialOffset();
    test_MMDRigidBody_ExternalDynamic_ReflectWritesBackToNode();
    test_MMDRigidBody_ExternalDynamicAndBoneMerge_SyncsPositionAndVelocity();
    test_MMDPhysicsManager_DestroyWithoutCreate();

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
