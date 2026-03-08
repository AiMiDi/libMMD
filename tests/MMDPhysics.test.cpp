#define _USE_MATH_DEFINES
#include <cmath>

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
