#include <libMMD/Model/MMD/VPDFile.h>

#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

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

class TempFile
{
public:
    explicit TempFile(const std::string& ext)
    {
        m_pathStr = (std::filesystem::temp_directory_path() /
            ("libmmd_vpd_test_" + std::to_string(reinterpret_cast<uintptr_t>(this)) + ext)).string();
    }

    ~TempFile()
    {
        std::filesystem::remove(m_pathStr);
    }

    const char* path() const { return m_pathStr.c_str(); }

    bool WriteBytes(const std::string& data) const
    {
        std::ofstream ofs(m_pathStr, std::ios::binary);
        if (!ofs)
            return false;
        ofs.write(data.data(), static_cast<std::streamsize>(data.size()));
        return ofs.good();
    }

private:
    std::string m_pathStr;
};

static std::string MakeSjisVpdFixture()
{
    std::string data;
    data += "Vocaloid Pose Data file\r\n";
    data += "\r\n";
    data += "model.osm;\t// parent file name\r\n";
    data += "2;\t// total bones\r\n";
    data += "\r\n";
    data += "Bone0{";
    data += "\x83\x5A\x83\x93\x83\x5E\x81\x5B"; // center
    data += "\r\n";
    data += "  1.250000,-2.500000,3.750000;\r\n";
    data += "  0.100000,0.200000,0.300000,0.923000;\r\n";
    data += "}\r\n";
    data += "\r\n";
    data += "Bone1{";
    data += "\x89\x45\x98\x72"; // right arm
    data += "\r\n";
    data += "  -4.000000,5.500000,6.250000;\r\n";
    data += "  -0.250000,0.000000,0.125000,0.960000;\r\n";
    data += "}\r\n";
    data += "\r\n";
    data += "Morph0{";
    data += "\x8F\xCE\x82\xA2"; // smile
    data += "\r\n";
    data += "  0.750000;\r\n";
    data += "}\r\n";
    return data;
}

static libmmd::VPDFile MakeRoundTripVpd()
{
    libmmd::VPDFile vpd;

    {
        libmmd::VPDBone bone;
        bone.m_boneName = "\xE3\x82\xBB\xE3\x83\xB3\xE3\x82\xBF\xE3\x83\xBC";
        bone.m_translate = Eigen::Vector3f(1.0f, 2.0f, 3.0f);
        bone.m_quaternion = Eigen::Quaternionf(0.9238795f, 0.3826834f, 0.0f, 0.0f);
        vpd.m_bones.push_back(std::move(bone));
    }

    {
        libmmd::VPDBone bone;
        bone.m_boneName = "\xE5\x8F\xB3\xE8\x85\x95";
        bone.m_translate = Eigen::Vector3f(-1.0f, 4.0f, 0.5f);
        bone.m_quaternion = Eigen::Quaternionf(0.9659258f, 0.0f, 0.2588190f, 0.0f);
        vpd.m_bones.push_back(std::move(bone));
    }

    {
        libmmd::VPDMorph morph;
        morph.m_morphName = "\xE7\xAC\x91\xE3\x81\x84";
        morph.m_weight = 0.42f;
        vpd.m_morphs.push_back(std::move(morph));
    }

    return vpd;
}

static void test_ReadVPDFile_ParsesSjisBoneAndMorphData()
{
    std::cout << "[test] ReadVPDFile_ParsesSjisBoneAndMorphData\n";
    TempFile file(".vpd");
    TEST_ASSERT(file.WriteBytes(MakeSjisVpdFixture()));

    libmmd::VPDFile vpd;
    TEST_ASSERT(libmmd::ReadVPDFile(&vpd, file.path()));

    TEST_ASSERT_EQ(size_t(2), vpd.m_bones.size());
    TEST_ASSERT_EQ(size_t(1), vpd.m_morphs.size());

    TEST_ASSERT_EQ(std::string("\xE3\x82\xBB\xE3\x83\xB3\xE3\x82\xBF\xE3\x83\xBC"), vpd.m_bones[0].m_boneName);
    TEST_ASSERT_FLOAT_EQ(1.25f, vpd.m_bones[0].m_translate.x());
    TEST_ASSERT_FLOAT_EQ(-2.5f, vpd.m_bones[0].m_translate.y());
    TEST_ASSERT_FLOAT_EQ(3.75f, vpd.m_bones[0].m_translate.z());
    TEST_ASSERT_FLOAT_EQ(0.1f, vpd.m_bones[0].m_quaternion.x());
    TEST_ASSERT_FLOAT_EQ(0.2f, vpd.m_bones[0].m_quaternion.y());
    TEST_ASSERT_FLOAT_EQ(0.3f, vpd.m_bones[0].m_quaternion.z());
    TEST_ASSERT_FLOAT_EQ(0.923f, vpd.m_bones[0].m_quaternion.w());

    TEST_ASSERT_EQ(std::string("\xE5\x8F\xB3\xE8\x85\x95"), vpd.m_bones[1].m_boneName);
    TEST_ASSERT_FLOAT_EQ(-4.0f, vpd.m_bones[1].m_translate.x());
    TEST_ASSERT_FLOAT_EQ(5.5f, vpd.m_bones[1].m_translate.y());
    TEST_ASSERT_FLOAT_EQ(6.25f, vpd.m_bones[1].m_translate.z());

    TEST_ASSERT_EQ(std::string("\xE7\xAC\x91\xE3\x81\x84"), vpd.m_morphs[0].m_morphName);
    TEST_ASSERT_FLOAT_EQ(0.75f, vpd.m_morphs[0].m_weight);
}

static void test_WriteVPDFile_RoundTripsReadableVpdData()
{
    std::cout << "[test] WriteVPDFile_RoundTripsReadableVpdData\n";
    const libmmd::VPDFile source = MakeRoundTripVpd();
    TempFile file(".vpd");

    TEST_ASSERT(libmmd::WriteVPDFile(&source, file.path()));

    libmmd::VPDFile readback;
    TEST_ASSERT(libmmd::ReadVPDFile(&readback, file.path()));
    TEST_ASSERT_EQ(source.m_bones.size(), readback.m_bones.size());
    TEST_ASSERT_EQ(source.m_morphs.size(), readback.m_morphs.size());

    for (size_t i = 0; i < source.m_bones.size(); ++i)
    {
        TEST_ASSERT_EQ(source.m_bones[i].m_boneName, readback.m_bones[i].m_boneName);
        TEST_ASSERT_FLOAT_EQ(source.m_bones[i].m_translate.x(), readback.m_bones[i].m_translate.x());
        TEST_ASSERT_FLOAT_EQ(source.m_bones[i].m_translate.y(), readback.m_bones[i].m_translate.y());
        TEST_ASSERT_FLOAT_EQ(source.m_bones[i].m_translate.z(), readback.m_bones[i].m_translate.z());
        TEST_ASSERT_FLOAT_EQ(source.m_bones[i].m_quaternion.x(), readback.m_bones[i].m_quaternion.x());
        TEST_ASSERT_FLOAT_EQ(source.m_bones[i].m_quaternion.y(), readback.m_bones[i].m_quaternion.y());
        TEST_ASSERT_FLOAT_EQ(source.m_bones[i].m_quaternion.z(), readback.m_bones[i].m_quaternion.z());
        TEST_ASSERT_FLOAT_EQ(source.m_bones[i].m_quaternion.w(), readback.m_bones[i].m_quaternion.w());
    }

    for (size_t i = 0; i < source.m_morphs.size(); ++i)
    {
        TEST_ASSERT_EQ(source.m_morphs[i].m_morphName, readback.m_morphs[i].m_morphName);
        TEST_ASSERT_FLOAT_EQ(source.m_morphs[i].m_weight, readback.m_morphs[i].m_weight);
    }
}

int main()
{
    test_ReadVPDFile_ParsesSjisBoneAndMorphData();
    test_WriteVPDFile_RoundTripsReadableVpdData();

    if (g_failedTests == 0)
        std::cout << "[summary] All " << g_totalTests << " assertions passed.\n";
    else
        std::cerr << "[summary] " << g_failedTests << " / " << g_totalTests << " assertions failed.\n";

    return g_failedTests == 0 ? 0 : 1;
}
