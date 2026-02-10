#include <libMMD/Base/Path.h>

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>
#include <filesystem>

// ---------------------------------------------------------------------------
// Minimal test framework for CTest (same as other test files)
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
            std::cerr << "  FAIL: expected \"" << (expected)               \
                      << "\" but got \"" << (actual) << "\"  ("            \
                      << __FILE__ << ":" << __LINE__ << ")\n";             \
            ++g_failedTests;                                               \
        }                                                                  \
    } while (0)

// Platform-aware expected path separator
#if _WIN32
static const std::string SEP = "\\";
#else
static const std::string SEP = "/";
#endif

// ===========================================================================
// GetCWD tests
// ===========================================================================

static void test_GetCWD_NotEmpty()
{
    std::cout << "[test] GetCWD_NotEmpty\n";
    std::string cwd = libmmd::PathUtil::GetCWD();
    TEST_ASSERT(!cwd.empty());
}

static void test_GetCWD_MatchesFilesystem()
{
    std::cout << "[test] GetCWD_MatchesFilesystem\n";
    std::string cwd = libmmd::PathUtil::GetCWD();
    std::string fsCwd = std::filesystem::current_path().u8string();
    TEST_ASSERT_EQ(fsCwd, cwd);
}

// ===========================================================================
// GetExecutablePath tests
// ===========================================================================

static void test_GetExecutablePath_NotEmpty()
{
    std::cout << "[test] GetExecutablePath_NotEmpty\n";
    std::string exePath = libmmd::PathUtil::GetExecutablePath();
    TEST_ASSERT(!exePath.empty());
}

static void test_GetExecutablePath_ContainsExeName()
{
    std::cout << "[test] GetExecutablePath_ContainsExeName\n";
    std::string exePath = libmmd::PathUtil::GetExecutablePath();
    // The executable name should contain "path_util_test"
    std::string filename = libmmd::PathUtil::GetFilename(exePath);
    TEST_ASSERT(filename.find("path_util_test") != std::string::npos);
}

// ===========================================================================
// Combine tests
// ===========================================================================

static void test_Combine_TwoParts()
{
    std::cout << "[test] Combine_TwoParts\n";
    std::string result = libmmd::PathUtil::Combine("dir", "file.txt");
    TEST_ASSERT_EQ(std::string("dir" + SEP + "file.txt"), result);
}

static void test_Combine_FirstEmpty()
{
    std::cout << "[test] Combine_FirstEmpty\n";
    std::string result = libmmd::PathUtil::Combine("", "file.txt");
    TEST_ASSERT_EQ(std::string("file.txt"), result);
}

static void test_Combine_SecondEmpty()
{
    std::cout << "[test] Combine_SecondEmpty\n";
    std::string result = libmmd::PathUtil::Combine("dir", "");
    TEST_ASSERT_EQ(std::string("dir"), result);
}

static void test_Combine_BothEmpty()
{
    std::cout << "[test] Combine_BothEmpty\n";
    std::string result = libmmd::PathUtil::Combine("", "");
    TEST_ASSERT_EQ(std::string(""), result);
}

static void test_Combine_TrailingDelimiter()
{
    std::cout << "[test] Combine_TrailingDelimiter\n";
    // Trailing delimiter on first part should not produce double delimiter
    std::string result = libmmd::PathUtil::Combine("dir" + SEP, "file.txt");
    TEST_ASSERT_EQ(std::string("dir" + SEP + "file.txt"), result);
}

static void test_Combine_VectorMultipleParts()
{
    std::cout << "[test] Combine_VectorMultipleParts\n";
    std::vector<std::string> parts = { "a", "b", "c.txt" };
    std::string result = libmmd::PathUtil::Combine(parts);
    TEST_ASSERT_EQ(std::string("a" + SEP + "b" + SEP + "c.txt"), result);
}

static void test_Combine_VectorEmptyParts()
{
    std::cout << "[test] Combine_VectorEmptyParts\n";
    std::vector<std::string> parts = { "a", "", "b", "", "c" };
    std::string result = libmmd::PathUtil::Combine(parts);
    TEST_ASSERT_EQ(std::string("a" + SEP + "b" + SEP + "c"), result);
}

static void test_Combine_VectorAllEmpty()
{
    std::cout << "[test] Combine_VectorAllEmpty\n";
    std::vector<std::string> parts = { "", "", "" };
    std::string result = libmmd::PathUtil::Combine(parts);
    TEST_ASSERT_EQ(std::string(""), result);
}

static void test_Combine_VectorSinglePart()
{
    std::cout << "[test] Combine_VectorSinglePart\n";
    std::vector<std::string> parts = { "only" };
    std::string result = libmmd::PathUtil::Combine(parts);
    TEST_ASSERT_EQ(std::string("only"), result);
}

static void test_Combine_NestedSubdir()
{
    std::cout << "[test] Combine_NestedSubdir\n";
    std::string result = libmmd::PathUtil::Combine("models" + SEP + "mmd", "texture.bmp");
    TEST_ASSERT_EQ(std::string("models" + SEP + "mmd" + SEP + "texture.bmp"), result);
}

// ===========================================================================
// GetDirectoryName tests
// ===========================================================================

static void test_GetDirectoryName_WithFile()
{
    std::cout << "[test] GetDirectoryName_WithFile\n";
    std::string result = libmmd::PathUtil::GetDirectoryName("a" + SEP + "b" + SEP + "c.txt");
    TEST_ASSERT_EQ(std::string("a" + SEP + "b"), result);
}

static void test_GetDirectoryName_NoDirectory()
{
    std::cout << "[test] GetDirectoryName_NoDirectory\n";
    std::string result = libmmd::PathUtil::GetDirectoryName("c.txt");
    TEST_ASSERT_EQ(std::string(""), result);
}

static void test_GetDirectoryName_Empty()
{
    std::cout << "[test] GetDirectoryName_Empty\n";
    std::string result = libmmd::PathUtil::GetDirectoryName("");
    TEST_ASSERT_EQ(std::string(""), result);
}

static void test_GetDirectoryName_DeepPath()
{
    std::cout << "[test] GetDirectoryName_DeepPath\n";
    std::string path = "a" + SEP + "b" + SEP + "c" + SEP + "d.pmx";
    std::string result = libmmd::PathUtil::GetDirectoryName(path);
    TEST_ASSERT_EQ(std::string("a" + SEP + "b" + SEP + "c"), result);
}

// ===========================================================================
// GetFilename tests
// ===========================================================================

static void test_GetFilename_WithDirectory()
{
    std::cout << "[test] GetFilename_WithDirectory\n";
    std::string result = libmmd::PathUtil::GetFilename("a" + SEP + "b" + SEP + "model.pmx");
    TEST_ASSERT_EQ(std::string("model.pmx"), result);
}

static void test_GetFilename_NoDirectory()
{
    std::cout << "[test] GetFilename_NoDirectory\n";
    std::string result = libmmd::PathUtil::GetFilename("model.pmx");
    TEST_ASSERT_EQ(std::string("model.pmx"), result);
}

static void test_GetFilename_Empty()
{
    std::cout << "[test] GetFilename_Empty\n";
    std::string result = libmmd::PathUtil::GetFilename("");
    TEST_ASSERT_EQ(std::string(""), result);
}

// ===========================================================================
// GetFilenameWithoutExt tests
// ===========================================================================

static void test_GetFilenameWithoutExt_Normal()
{
    std::cout << "[test] GetFilenameWithoutExt_Normal\n";
    std::string result = libmmd::PathUtil::GetFilenameWithoutExt("a" + SEP + "model.pmx");
    TEST_ASSERT_EQ(std::string("model"), result);
}

static void test_GetFilenameWithoutExt_NoExt()
{
    std::cout << "[test] GetFilenameWithoutExt_NoExt\n";
    std::string result = libmmd::PathUtil::GetFilenameWithoutExt("a" + SEP + "model");
    TEST_ASSERT_EQ(std::string("model"), result);
}

static void test_GetFilenameWithoutExt_MultiDot()
{
    std::cout << "[test] GetFilenameWithoutExt_MultiDot\n";
    std::string result = libmmd::PathUtil::GetFilenameWithoutExt("archive.tar.gz");
    TEST_ASSERT_EQ(std::string("archive.tar"), result);
}

static void test_GetFilenameWithoutExt_JustFilename()
{
    std::cout << "[test] GetFilenameWithoutExt_JustFilename\n";
    std::string result = libmmd::PathUtil::GetFilenameWithoutExt("texture.bmp");
    TEST_ASSERT_EQ(std::string("texture"), result);
}

// ===========================================================================
// GetExt tests
// ===========================================================================

static void test_GetExt_Normal()
{
    std::cout << "[test] GetExt_Normal\n";
    std::string result = libmmd::PathUtil::GetExt("model.PMX");
    TEST_ASSERT_EQ(std::string("pmx"), result);
}

static void test_GetExt_Lowercase()
{
    std::cout << "[test] GetExt_Lowercase\n";
    std::string result = libmmd::PathUtil::GetExt("texture.bmp");
    TEST_ASSERT_EQ(std::string("bmp"), result);
}

static void test_GetExt_MixedCase()
{
    std::cout << "[test] GetExt_MixedCase\n";
    std::string result = libmmd::PathUtil::GetExt("model.Vmd");
    TEST_ASSERT_EQ(std::string("vmd"), result);
}

static void test_GetExt_NoExtension()
{
    std::cout << "[test] GetExt_NoExtension\n";
    std::string result = libmmd::PathUtil::GetExt("README");
    TEST_ASSERT_EQ(std::string(""), result);
}

static void test_GetExt_MultiDot()
{
    std::cout << "[test] GetExt_MultiDot\n";
    std::string result = libmmd::PathUtil::GetExt("archive.tar.GZ");
    TEST_ASSERT_EQ(std::string("gz"), result);
}

static void test_GetExt_WithPath()
{
    std::cout << "[test] GetExt_WithPath\n";
    std::string result = libmmd::PathUtil::GetExt("models" + SEP + "miku" + SEP + "texture.SPH");
    TEST_ASSERT_EQ(std::string("sph"), result);
}

static void test_GetExt_Empty()
{
    std::cout << "[test] GetExt_Empty\n";
    std::string result = libmmd::PathUtil::GetExt("");
    TEST_ASSERT_EQ(std::string(""), result);
}

// ===========================================================================
// GetDelimiter tests
// ===========================================================================

static void test_GetDelimiter_Platform()
{
    std::cout << "[test] GetDelimiter_Platform\n";
    std::string delim = libmmd::PathUtil::GetDelimiter();
    TEST_ASSERT_EQ(SEP, delim);
}

// ===========================================================================
// Normalize tests
// ===========================================================================

static void test_Normalize_ForwardSlash()
{
    std::cout << "[test] Normalize_ForwardSlash\n";
    std::string result = libmmd::PathUtil::Normalize("a/b/c.txt");
#if _WIN32
    TEST_ASSERT_EQ(std::string("a\\b\\c.txt"), result);
#else
    TEST_ASSERT_EQ(std::string("a/b/c.txt"), result);
#endif
}

static void test_Normalize_BackSlash()
{
    std::cout << "[test] Normalize_BackSlash\n";
    std::string result = libmmd::PathUtil::Normalize("a\\b\\c.txt");
#if _WIN32
    TEST_ASSERT_EQ(std::string("a\\b\\c.txt"), result);
#else
    TEST_ASSERT_EQ(std::string("a/b/c.txt"), result);
#endif
}

static void test_Normalize_MixedSlash()
{
    std::cout << "[test] Normalize_MixedSlash\n";
    std::string result = libmmd::PathUtil::Normalize("models/mmd\\texture.bmp");
#if _WIN32
    TEST_ASSERT_EQ(std::string("models\\mmd\\texture.bmp"), result);
#else
    TEST_ASSERT_EQ(std::string("models/mmd/texture.bmp"), result);
#endif
}

static void test_Normalize_AlreadyNormalized()
{
    std::cout << "[test] Normalize_AlreadyNormalized\n";
    std::string input = "a" + SEP + "b" + SEP + "c";
    std::string result = libmmd::PathUtil::Normalize(input);
    TEST_ASSERT_EQ(input, result);
}

static void test_Normalize_Empty()
{
    std::cout << "[test] Normalize_Empty\n";
    std::string result = libmmd::PathUtil::Normalize("");
    TEST_ASSERT_EQ(std::string(""), result);
}

// ===========================================================================
// Realistic MMD workflow tests
// ===========================================================================

static void test_Workflow_PMXTexturePath()
{
    std::cout << "[test] Workflow_PMXTexturePath\n";
    // Simulate: get directory of pmx file, combine with texture name, normalize
    std::string pmxPath = "D:" + SEP + "models" + SEP + "miku" + SEP + "miku.pmx";
    std::string dirName = libmmd::PathUtil::GetDirectoryName(pmxPath);
    std::string texPath = libmmd::PathUtil::Combine(dirName, "tex" + SEP + "body.bmp");
    std::string normalized = libmmd::PathUtil::Normalize(texPath);

    TEST_ASSERT(normalized.find("body.bmp") != std::string::npos);
    TEST_ASSERT(normalized.find("miku") != std::string::npos);

    std::string ext = libmmd::PathUtil::GetExt(normalized);
    TEST_ASSERT_EQ(std::string("bmp"), ext);
}

static void test_Workflow_OBJMaterialPath()
{
    std::cout << "[test] Workflow_OBJMaterialPath\n";
    // Simulate OBJModel.cpp pattern: GetDirectoryName + GetDelimiter + Combine
    std::string objPath = "models" + SEP + "scene.obj";
    std::string fileDir = libmmd::PathUtil::GetDirectoryName(objPath);
    fileDir += libmmd::PathUtil::GetDelimiter();
    std::string mtlPath = libmmd::PathUtil::Combine(fileDir, "material.mtl");

    TEST_ASSERT(mtlPath.find("material.mtl") != std::string::npos);
    TEST_ASSERT(mtlPath.find("models") != std::string::npos);
}

static void test_Workflow_FilenameWithoutExtForMtl()
{
    std::cout << "[test] Workflow_FilenameWithoutExtForMtl\n";
    // Simulate OBJModel.cpp: GetFilenameWithoutExt + ".mtl"
    std::string objPath = "scenes" + SEP + "my_model.obj";
    std::string baseName = libmmd::PathUtil::GetFilenameWithoutExt(objPath);
    std::string mtlName = baseName + ".mtl";

    TEST_ASSERT_EQ(std::string("my_model"), baseName);
    TEST_ASSERT_EQ(std::string("my_model.mtl"), mtlName);
}

static void test_Workflow_TextureExtCheck()
{
    std::cout << "[test] Workflow_TextureExtCheck\n";
    // Simulate PMDModel.cpp pattern: check ext for sph/spa
    TEST_ASSERT_EQ(std::string("sph"), libmmd::PathUtil::GetExt("env.SPH"));
    TEST_ASSERT_EQ(std::string("spa"), libmmd::PathUtil::GetExt("env.SPA"));
    TEST_ASSERT_EQ(std::string("bmp"), libmmd::PathUtil::GetExt("body.BMP"));
    TEST_ASSERT_EQ(std::string("png"), libmmd::PathUtil::GetExt("face.PNG"));
    TEST_ASSERT_EQ(std::string("tga"), libmmd::PathUtil::GetExt("hair.TGA"));
}

// ===========================================================================
// main
// ===========================================================================

int main()
{
    std::cout << "=== PathUtil Unit Tests ===\n\n";

    // GetCWD
    test_GetCWD_NotEmpty();
    test_GetCWD_MatchesFilesystem();

    // GetExecutablePath
    test_GetExecutablePath_NotEmpty();
    test_GetExecutablePath_ContainsExeName();

    // Combine
    test_Combine_TwoParts();
    test_Combine_FirstEmpty();
    test_Combine_SecondEmpty();
    test_Combine_BothEmpty();
    test_Combine_TrailingDelimiter();
    test_Combine_VectorMultipleParts();
    test_Combine_VectorEmptyParts();
    test_Combine_VectorAllEmpty();
    test_Combine_VectorSinglePart();
    test_Combine_NestedSubdir();

    // GetDirectoryName
    test_GetDirectoryName_WithFile();
    test_GetDirectoryName_NoDirectory();
    test_GetDirectoryName_Empty();
    test_GetDirectoryName_DeepPath();

    // GetFilename
    test_GetFilename_WithDirectory();
    test_GetFilename_NoDirectory();
    test_GetFilename_Empty();

    // GetFilenameWithoutExt
    test_GetFilenameWithoutExt_Normal();
    test_GetFilenameWithoutExt_NoExt();
    test_GetFilenameWithoutExt_MultiDot();
    test_GetFilenameWithoutExt_JustFilename();

    // GetExt
    test_GetExt_Normal();
    test_GetExt_Lowercase();
    test_GetExt_MixedCase();
    test_GetExt_NoExtension();
    test_GetExt_MultiDot();
    test_GetExt_WithPath();
    test_GetExt_Empty();

    // GetDelimiter
    test_GetDelimiter_Platform();

    // Normalize
    test_Normalize_ForwardSlash();
    test_Normalize_BackSlash();
    test_Normalize_MixedSlash();
    test_Normalize_AlreadyNormalized();
    test_Normalize_Empty();

    // Realistic MMD workflows
    test_Workflow_PMXTexturePath();
    test_Workflow_OBJMaterialPath();
    test_Workflow_FilenameWithoutExtForMtl();
    test_Workflow_TextureExtCheck();

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
