#include <cmath>

#include <libMMD/Model/MMD/VMDInterpolation.h>
#include <libMMD/Model/MMD/VMDFile.h>
#include <libMMD/Model/MMD/VMDCameraAnimation.h>

#include <array>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdlib>

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

#define TEST_ASSERT_NEAR(expected, actual, epsilon)                        \
	do {                                                                   \
		++g_totalTests;                                                    \
		if (std::fabs((expected) - (actual)) > (epsilon)) {                \
			std::cerr << "  FAIL: expected " << (expected)                 \
			          << " but got " << (actual) << "  (" << __FILE__      \
			          << ":" << __LINE__ << ")\n";                         \
			++g_failedTests;                                               \
		}                                                                  \
	} while (0)

namespace
{
	constexpr float kPi = 3.14159265358979323846f;

	libmmd::VMDBezier MakeLinearBezier()
	{
		libmmd::VMDBezier bezier;
		bezier.m_cp1 = Eigen::Vector2f(1.0f / 3.0f, 1.0f / 3.0f);
		bezier.m_cp2 = Eigen::Vector2f(2.0f / 3.0f, 2.0f / 3.0f);
		return bezier;
	}

	std::array<uint8_t, 24> MakeCameraInterpolation(
		const uint8_t x0,
		const uint8_t x1,
		const uint8_t y0,
		const uint8_t y1
	)
	{
		std::array<uint8_t, 24> interpolation{};
		for (size_t channel = 0; channel < 6; ++channel)
		{
			const size_t offset = channel * 4;
			interpolation[offset + 0] = x0;
			interpolation[offset + 1] = x1;
			interpolation[offset + 2] = y0;
			interpolation[offset + 3] = y1;
		}
		return interpolation;
	}

	libmmd::VMDBezier MakeCameraBezier(
		const uint8_t x0,
		const uint8_t x1,
		const uint8_t y0,
		const uint8_t y1
	)
	{
		libmmd::VMDBezier bezier;
		bezier.m_cp1 = Eigen::Vector2f(static_cast<float>(x0) / 127.0f, static_cast<float>(y0) / 127.0f);
		bezier.m_cp2 = Eigen::Vector2f(static_cast<float>(x1) / 127.0f, static_cast<float>(y1) / 127.0f);
		return bezier;
	}

	void ExpectQuaternionNear(const Eigen::Quaternionf& expected, const Eigen::Quaternionf& actual, float epsilon)
	{
		++g_totalTests;
		const float dot = std::fabs(expected.normalized().dot(actual.normalized()));
		if (std::fabs(1.0f - dot) > epsilon)
		{
			std::cerr << "  FAIL: quaternion mismatch  (" << __FILE__
			          << ":" << __LINE__ << ")\n";
			++g_failedTests;
		}
	}
}

static void test_VMDBezier_DecodeAndEncodeRoundTrip()
{
	std::cout << "[test] VMDBezier_DecodeAndEncodeRoundTrip\n";

	uint8_t bytes[16] = {};
	bytes[0] = 20;
	bytes[4] = 20;
	bytes[8] = 107;
	bytes[12] = 107;

	libmmd::VMDBezier bezier;
	libmmd::SetVMDBezier(bezier, bytes);

	TEST_ASSERT_NEAR(20.0f / 127.0f, bezier.m_cp1.x(), 1e-6f);
	TEST_ASSERT_NEAR(20.0f / 127.0f, bezier.m_cp1.y(), 1e-6f);
	TEST_ASSERT_NEAR(107.0f / 127.0f, bezier.m_cp2.x(), 1e-6f);
	TEST_ASSERT_NEAR(107.0f / 127.0f, bezier.m_cp2.y(), 1e-6f);

	uint8_t encoded[16] = {};
	libmmd::GetVMDBezier(bezier, encoded);
	TEST_ASSERT(encoded[0] == 20);
	TEST_ASSERT(encoded[4] == 20);
	TEST_ASSERT(encoded[8] == 107);
	TEST_ASSERT(encoded[12] == 107);
}

static void test_VMDBoneKeyframe_ConstructFromVMDMotion()
{
	std::cout << "[test] VMDBoneKeyframe_ConstructFromVMDMotion\n";

	libmmd::VMDMotion motion;
	motion.m_frame = 42;
	motion.m_translate = Eigen::Vector3f(1.0f, 2.0f, 3.0f);
	motion.m_quaternion = Eigen::Quaternionf(Eigen::AngleAxisf(0.25f * kPi, Eigen::Vector3f::UnitZ()));
	motion.m_interpolation.fill(0);
	motion.m_interpolation[0] = 20;
	motion.m_interpolation[4] = 21;
	motion.m_interpolation[8] = 100;
	motion.m_interpolation[12] = 101;
	motion.m_interpolation[1] = 30;
	motion.m_interpolation[5] = 31;
	motion.m_interpolation[9] = 90;
	motion.m_interpolation[13] = 91;

	const libmmd::VMDBoneKeyframe key(motion);

	TEST_ASSERT(key.frame == 42);
	TEST_ASSERT_NEAR(1.0f, key.translate.x(), 1e-6f);
	TEST_ASSERT_NEAR(2.0f, key.translate.y(), 1e-6f);
	TEST_ASSERT_NEAR(3.0f, key.translate.z(), 1e-6f);
	ExpectQuaternionNear(motion.m_quaternion, key.rotate, 1e-5f);
	TEST_ASSERT_NEAR(20.0f / 127.0f, key.txBezier.m_cp1.x(), 1e-6f);
	TEST_ASSERT_NEAR(21.0f / 127.0f, key.txBezier.m_cp1.y(), 1e-6f);
	TEST_ASSERT_NEAR(90.0f / 127.0f, key.tyBezier.m_cp2.x(), 1e-6f);
	TEST_ASSERT_NEAR(91.0f / 127.0f, key.tyBezier.m_cp2.y(), 1e-6f);
}

static void test_InterpolateBoneKeys_Linear()
{
	std::cout << "[test] InterpolateBoneKeys_Linear\n";

	libmmd::VMDBoneKeyframe key0;
	key0.frame = 0;
	key0.translate = Eigen::Vector3f::Zero();
	key0.rotate = Eigen::Quaternionf::Identity();
	key0.txBezier = MakeLinearBezier();
	key0.tyBezier = MakeLinearBezier();
	key0.tzBezier = MakeLinearBezier();
	key0.rotBezier = MakeLinearBezier();

	libmmd::VMDBoneKeyframe key1;
	key1.frame = 30;
	key1.translate = Eigen::Vector3f(10.0f, 0.0f, 0.0f);
	key1.rotate = Eigen::Quaternionf(Eigen::AngleAxisf(0.5f * kPi, Eigen::Vector3f::UnitY()));

	const auto result = libmmd::InterpolateBoneKeys(key0, key1, 15.0f);
	const Eigen::Quaternionf expected = Eigen::Quaternionf::Identity().slerp(0.5f, key1.rotate);

	TEST_ASSERT_NEAR(5.0f, result.translate.x(), 1e-3f);
	TEST_ASSERT_NEAR(0.0f, result.translate.y(), 1e-3f);
	TEST_ASSERT_NEAR(0.0f, result.translate.z(), 1e-3f);
	ExpectQuaternionNear(expected, result.rotate, 1e-4f);
}

static void test_InterpolateBoneKeys_NonLinearPerAxis()
{
	std::cout << "[test] InterpolateBoneKeys_NonLinearPerAxis\n";

	libmmd::VMDBoneKeyframe key0;
	key0.frame = 0;
	key0.translate = Eigen::Vector3f::Zero();
	key0.rotate = Eigen::Quaternionf::Identity();
	key0.txBezier.m_cp1 = Eigen::Vector2f(0.2f, 0.05f);
	key0.txBezier.m_cp2 = Eigen::Vector2f(0.8f, 0.35f);
	key0.tyBezier = MakeLinearBezier();
	key0.tzBezier = MakeLinearBezier();
	key0.rotBezier = MakeLinearBezier();

	libmmd::VMDBoneKeyframe key1;
	key1.frame = 30;
	key1.translate = Eigen::Vector3f(10.0f, 10.0f, 10.0f);
	key1.rotate = Eigen::Quaternionf(Eigen::AngleAxisf(0.5f * kPi, Eigen::Vector3f::UnitX()));

	const auto result = libmmd::InterpolateBoneKeys(key0, key1, 15.0f);
	const float txParam = key0.txBezier.FindBezierX(0.5f);
	const float txFactor = key0.txBezier.EvalY(txParam);

	TEST_ASSERT(result.translate.x() < 5.0f);
	TEST_ASSERT_NEAR(10.0f * txFactor, result.translate.x(), 1e-3f);
	TEST_ASSERT_NEAR(5.0f, result.translate.y(), 1e-3f);
	TEST_ASSERT_NEAR(5.0f, result.translate.z(), 1e-3f);
}

static void test_InterpolateBoneKeys_BoundaryFrames()
{
	std::cout << "[test] InterpolateBoneKeys_BoundaryFrames\n";

	libmmd::VMDBoneKeyframe key0;
	key0.frame = 10;
	key0.translate = Eigen::Vector3f(1.0f, 2.0f, 3.0f);
	key0.rotate = Eigen::Quaternionf::Identity();
	key0.txBezier = MakeLinearBezier();
	key0.tyBezier = MakeLinearBezier();
	key0.tzBezier = MakeLinearBezier();
	key0.rotBezier = MakeLinearBezier();

	libmmd::VMDBoneKeyframe key1;
	key1.frame = 20;
	key1.translate = Eigen::Vector3f(4.0f, 5.0f, 6.0f);
	key1.rotate = Eigen::Quaternionf(Eigen::AngleAxisf(0.25f * kPi, Eigen::Vector3f::UnitY()));

	const auto before = libmmd::InterpolateBoneKeys(key0, key1, 5.0f);
	const auto after = libmmd::InterpolateBoneKeys(key0, key1, 30.0f);

	TEST_ASSERT_NEAR(1.0f, before.translate.x(), 1e-6f);
	TEST_ASSERT_NEAR(2.0f, before.translate.y(), 1e-6f);
	TEST_ASSERT_NEAR(3.0f, before.translate.z(), 1e-6f);
	ExpectQuaternionNear(key0.rotate, before.rotate, 1e-5f);

	TEST_ASSERT_NEAR(4.0f, after.translate.x(), 1e-6f);
	TEST_ASSERT_NEAR(5.0f, after.translate.y(), 1e-6f);
	TEST_ASSERT_NEAR(6.0f, after.translate.z(), 1e-6f);
	ExpectQuaternionNear(key1.rotate, after.rotate, 1e-5f);
}

static void test_FindBoundKey_WithSequentialHint()
{
	std::cout << "[test] FindBoundKey_WithSequentialHint\n";

	std::vector<libmmd::VMDBoneKeyframe> keys(3);
	keys[0].frame = 0;
	keys[1].frame = 10;
	keys[2].frame = 20;

	const auto first = libmmd::FindBoundKey(keys, 5, 0);
	TEST_ASSERT(first != keys.end());
	TEST_ASSERT(first->frame == 10);

	const auto second = libmmd::FindBoundKey(keys, 15, 1);
	TEST_ASSERT(second != keys.end());
	TEST_ASSERT(second->frame == 20);

	const auto pastEnd = libmmd::FindBoundKey(keys, 25, 2);
	TEST_ASSERT(pastEnd == keys.end());
}

static void test_VMDCameraAnimation_UsesUpperKeyInterpolation()
{
	std::cout << "[test] VMDCameraAnimation_UsesUpperKeyInterpolation\n";

	libmmd::VMDFile vmd;
	vmd.m_cameras.emplace_back(
		0,
		0.0f,
		Eigen::Vector3f::Zero(),
		Eigen::Vector3f::Zero(),
		45,
		0,
		MakeCameraInterpolation(20, 107, 20, 107));
	vmd.m_cameras.emplace_back(
		10,
		0.0f,
		Eigen::Vector3f(10.0f, 0.0f, 0.0f),
		Eigen::Vector3f::Zero(),
		45,
		0,
		MakeCameraInterpolation(20, 107, 0, 0));

	libmmd::VMDCameraAnimation animation;
	TEST_ASSERT(animation.Create(vmd));

	animation.Evaluate(5.0f);

	const auto upperKeyBezier = MakeCameraBezier(20, 107, 0, 0);
	const float interpolationTime = upperKeyBezier.FindBezierX(0.5f);
	const float expectedFactor = upperKeyBezier.EvalY(interpolationTime);

	TEST_ASSERT(expectedFactor < 0.5f);
	TEST_ASSERT_NEAR(10.0f * expectedFactor, animation.GetCamera().m_interest.x(), 1e-3f);
	TEST_ASSERT_NEAR(0.0f, animation.GetCamera().m_interest.y(), 1e-6f);
	TEST_ASSERT_NEAR(0.0f, animation.GetCamera().m_interest.z(), 1e-6f);
}

int main()
{
	std::cout << "=== VMDInterpolation Tests ===\n";

	test_VMDBezier_DecodeAndEncodeRoundTrip();
	test_VMDBoneKeyframe_ConstructFromVMDMotion();
	test_InterpolateBoneKeys_Linear();
	test_InterpolateBoneKeys_NonLinearPerAxis();
	test_InterpolateBoneKeys_BoundaryFrames();
	test_FindBoundKey_WithSequentialHint();
	test_VMDCameraAnimation_UsesUpperKeyInterpolation();

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
