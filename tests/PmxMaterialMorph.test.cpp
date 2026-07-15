#include <libMMD/Model/MMD/PMXFile.h>

#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>

namespace
{
int g_assertions = 0;
int g_failures = 0;

#define TEST_ASSERT(expr)                                                   \
    do                                                                      \
    {                                                                       \
        ++g_assertions;                                                     \
        if (!(expr))                                                        \
        {                                                                   \
            std::cerr << "FAIL: " #expr << " (" << __FILE__ << ':'        \
                      << __LINE__ << ")\n";                                  \
            ++g_failures;                                                   \
        }                                                                   \
    } while (false)

#define TEST_ASSERT_FLOAT(expected, actual)                                 \
    TEST_ASSERT(std::fabs((expected) - (actual)) <= 1.0e-5f)

class TempFile
{
public:
    TempFile()
        : path_((std::filesystem::temp_directory_path() /
                 ("libmmd_material_morph_" +
                  std::to_string(reinterpret_cast<std::uintptr_t>(this)) +
                  ".pmx"))
                    .string())
    {
    }

    ~TempFile()
    {
        std::error_code error;
        std::filesystem::remove(path_, error);
    }

    const std::string& path() const { return path_; }

private:
    std::string path_;
};

libmmd::PMXFile MakeFile()
{
    libmmd::PMXFile file{};
    auto& header = file.m_header;
    header.m_magic.Set("PMX ");
    header.m_version = 2.0f;
    header.m_dataSize = 8;
    header.m_encode = 1;
    header.m_addUVNum = 0;
    header.m_vertexIndexSize = 1;
    header.m_textureIndexSize = 1;
    header.m_materialIndexSize = 1;
    header.m_boneIndexSize = 1;
    header.m_morphIndexSize = 1;
    header.m_rigidbodyIndexSize = 1;

    file.m_info.m_modelName = "material_morph_test";
    file.m_info.m_englishModelName = "material_morph_test";

    auto addMorph = [&file](const char* name,
                            libmmd::PMXFileMorph::MaterialMorph::OpType op,
                            float seed)
    {
        libmmd::PMXFileMorph morph{};
        morph.m_name = name;
        morph.m_englishName = name;
        morph.m_controlPanel = 4;
        morph.m_morphType = libmmd::PMXMorphType::Material;

        libmmd::PMXFileMorph::MaterialMorph value{};
        value.m_materialIndex = -1;
        value.m_opType = op;
        value.m_diffuse = Eigen::Vector4f(seed + 0.01f, seed + 0.02f,
                                         seed + 0.03f, seed + 0.04f);
        value.m_specular = Eigen::Vector3f(seed + 0.05f, seed + 0.06f,
                                          seed + 0.07f);
        value.m_specularPower = seed + 0.08f;
        value.m_ambient = Eigen::Vector3f(seed + 0.09f, seed + 0.10f,
                                         seed + 0.11f);
        value.m_edgeColor = Eigen::Vector4f(seed + 0.12f, seed + 0.13f,
                                           seed + 0.14f, seed + 0.15f);
        value.m_edgeSize = seed + 0.16f;
        value.m_textureFactor = Eigen::Vector4f(seed + 0.17f, seed + 0.18f,
                                               seed + 0.19f, seed + 0.20f);
        value.m_sphereTextureFactor =
            Eigen::Vector4f(seed + 0.21f, seed + 0.22f,
                            seed + 0.23f, seed + 0.24f);
        value.m_toonTextureFactor =
            Eigen::Vector4f(seed + 0.25f, seed + 0.26f,
                            seed + 0.27f, seed + 0.28f);
        morph.m_materialMorph.push_back(value);
        file.m_morphs.push_back(std::move(morph));
    };

    addMorph("multiply", libmmd::PMXFileMorph::MaterialMorph::OpType::Mul,
             0.5f);
    addMorph("add", libmmd::PMXFileMorph::MaterialMorph::OpType::Add, 1.5f);
    return file;
}

void AssertVector4(const Eigen::Vector4f& expected,
                   const Eigen::Vector4f& actual)
{
    for (Eigen::Index index = 0; index < 4; ++index)
        TEST_ASSERT_FLOAT(expected[index], actual[index]);
}

void AssertVector3(const Eigen::Vector3f& expected,
                   const Eigen::Vector3f& actual)
{
    for (Eigen::Index index = 0; index < 3; ++index)
        TEST_ASSERT_FLOAT(expected[index], actual[index]);
}

void AssertMaterialMorph(
    const libmmd::PMXFileMorph::MaterialMorph& expected,
    const libmmd::PMXFileMorph::MaterialMorph& actual)
{
    TEST_ASSERT(expected.m_materialIndex == actual.m_materialIndex);
    TEST_ASSERT(expected.m_opType == actual.m_opType);
    AssertVector4(expected.m_diffuse, actual.m_diffuse);
    AssertVector3(expected.m_specular, actual.m_specular);
    TEST_ASSERT_FLOAT(expected.m_specularPower, actual.m_specularPower);
    AssertVector3(expected.m_ambient, actual.m_ambient);
    AssertVector4(expected.m_edgeColor, actual.m_edgeColor);
    TEST_ASSERT_FLOAT(expected.m_edgeSize, actual.m_edgeSize);
    AssertVector4(expected.m_textureFactor, actual.m_textureFactor);
    AssertVector4(expected.m_sphereTextureFactor,
                  actual.m_sphereTextureFactor);
    AssertVector4(expected.m_toonTextureFactor,
                  actual.m_toonTextureFactor);
}
} // namespace

int main()
{
    const libmmd::PMXFile source = MakeFile();
    TempFile temp;
    TEST_ASSERT(libmmd::WritePMXFile(&source, temp.path().c_str()));

    libmmd::PMXFile loaded{};
    TEST_ASSERT(libmmd::ReadPMXFile(&loaded, temp.path().c_str()));
    TEST_ASSERT(source.m_morphs.size() == loaded.m_morphs.size());

    if (source.m_morphs.size() == loaded.m_morphs.size())
    {
        for (std::size_t index = 0; index < source.m_morphs.size(); ++index)
        {
            const auto& expected = source.m_morphs[index];
            const auto& actual = loaded.m_morphs[index];
            TEST_ASSERT(expected.m_name == actual.m_name);
            TEST_ASSERT(expected.m_morphType == actual.m_morphType);
            TEST_ASSERT(actual.m_materialMorph.size() == 1);
            if (actual.m_materialMorph.size() == 1)
                AssertMaterialMorph(expected.m_materialMorph.front(),
                                    actual.m_materialMorph.front());
        }
    }

    std::cout << "PMX material morph: " << g_assertions << " assertions, "
              << g_failures << " failures\n";
    return g_failures == 0 ? 0 : 1;
}
