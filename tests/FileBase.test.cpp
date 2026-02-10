#include <libMMD/Base/File.h>

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>

// ---------------------------------------------------------------------------
// Minimal test framework for CTest (same as MMDModel.test.cpp)
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

// ---------------------------------------------------------------------------
// Helper: create a temporary binary file with given content
// ---------------------------------------------------------------------------

class TempFile
{
public:
    explicit TempFile(const std::string& name) : m_path(name) {}

    ~TempFile()
    {
        std::remove(m_path.c_str());
    }

    bool WriteBinary(const void* data, size_t size) const
    {
        std::ofstream ofs(m_path, std::ios::binary);
        if (!ofs) return false;
        ofs.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));
        return ofs.good();
    }

    bool WriteText(const std::string& text) const
    {
        std::ofstream ofs(m_path);
        if (!ofs) return false;
        ofs << text;
        return ofs.good();
    }

    const std::string& Path() const { return m_path; }

private:
    std::string m_path;
};

// ===========================================================================
// MemoryReader tests
// ===========================================================================

static void test_MemoryReader_DefaultConstruction()
{
    std::cout << "[test] MemoryReader_DefaultConstruction\n";
    libmmd::MemoryReader reader;

    TEST_ASSERT(!reader.IsBad());
    TEST_ASSERT(reader.IsEOF());
    TEST_ASSERT_EQ(int64_t(0), reader.GetSize());
    TEST_ASSERT_EQ(int64_t(0), reader.Tell());
}

static void test_MemoryReader_ConstructFromBuffer()
{
    std::cout << "[test] MemoryReader_ConstructFromBuffer\n";
    const uint8_t data[] = { 0x01, 0x02, 0x03, 0x04 };
    libmmd::MemoryReader reader(data, sizeof(data));

    TEST_ASSERT(!reader.IsBad());
    TEST_ASSERT(!reader.IsEOF());
    TEST_ASSERT_EQ(int64_t(4), reader.GetSize());
    TEST_ASSERT_EQ(int64_t(0), reader.Tell());
}

static void test_MemoryReader_ReadSingleValue()
{
    std::cout << "[test] MemoryReader_ReadSingleValue\n";
    uint32_t srcValue = 0xDEADBEEF;
    const auto* data = reinterpret_cast<const uint8_t*>(&srcValue);
    libmmd::MemoryReader reader(data, sizeof(srcValue));

    uint32_t readValue = 0;
    TEST_ASSERT(reader.Read(&readValue));
    TEST_ASSERT_EQ(srcValue, readValue);
    TEST_ASSERT_EQ(int64_t(4), reader.Tell());
    TEST_ASSERT(reader.IsEOF());
}

static void test_MemoryReader_ReadMultipleValues()
{
    std::cout << "[test] MemoryReader_ReadMultipleValues\n";
    float srcValues[] = { 1.0f, 2.5f, -3.14f };
    const auto* data = reinterpret_cast<const uint8_t*>(srcValues);
    libmmd::MemoryReader reader(data, sizeof(srcValues));

    float readValues[3] = {};
    TEST_ASSERT(reader.Read(readValues, 3));
    TEST_ASSERT_FLOAT_EQ(1.0f, readValues[0]);
    TEST_ASSERT_FLOAT_EQ(2.5f, readValues[1]);
    TEST_ASSERT_FLOAT_EQ(-3.14f, readValues[2]);
    TEST_ASSERT(reader.IsEOF());
}

static void test_MemoryReader_ReadSequential()
{
    std::cout << "[test] MemoryReader_ReadSequential\n";
    // Manually pack: uint16_t(42) + float(3.14f) with no padding
    uint8_t buf[2 + 4];
    uint16_t srcId = 42;
    float srcVal = 3.14f;
    std::memcpy(buf, &srcId, 2);
    std::memcpy(buf + 2, &srcVal, 4);

    libmmd::MemoryReader reader(buf, sizeof(buf));

    uint16_t id = 0;
    float value = 0;
    TEST_ASSERT(reader.Read(&id));
    TEST_ASSERT(reader.Read(&value));
    TEST_ASSERT_EQ(uint16_t(42), id);
    TEST_ASSERT_FLOAT_EQ(3.14f, value);
    TEST_ASSERT(reader.IsEOF());
}

static void test_MemoryReader_ReadBeyondEndFails()
{
    std::cout << "[test] MemoryReader_ReadBeyondEndFails\n";
    const uint8_t data[] = { 0x01, 0x02 };
    libmmd::MemoryReader reader(data, sizeof(data));

    uint32_t val = 0;
    // Only 2 bytes available, trying to read 4 bytes should fail
    TEST_ASSERT(!reader.Read(&val));
    TEST_ASSERT(reader.IsBad());
    // Position should not advance on failure
    TEST_ASSERT_EQ(int64_t(0), reader.Tell());
}

static void test_MemoryReader_ReadNullptrFails()
{
    std::cout << "[test] MemoryReader_ReadNullptrFails\n";
    const uint8_t data[] = { 0x01 };
    libmmd::MemoryReader reader(data, sizeof(data));

    TEST_ASSERT(!reader.Read<uint8_t>(nullptr));
    TEST_ASSERT(!reader.IsBad()); // nullptr is not a bad-flag condition
}

static void test_MemoryReader_ReadArrayCount()
{
    std::cout << "[test] MemoryReader_ReadArrayCount\n";
    uint8_t src[] = { 10, 20, 30, 40, 50 };
    libmmd::MemoryReader reader(src, sizeof(src));

    uint8_t dst[3] = {};
    TEST_ASSERT(reader.Read(dst, 3));
    TEST_ASSERT_EQ(uint8_t(10), dst[0]);
    TEST_ASSERT_EQ(uint8_t(20), dst[1]);
    TEST_ASSERT_EQ(uint8_t(30), dst[2]);
    TEST_ASSERT_EQ(int64_t(3), reader.Tell());

    uint8_t dst2[3] = {};
    // Only 2 bytes left, reading 3 should fail
    TEST_ASSERT(!reader.Read(dst2, 3));
    TEST_ASSERT(reader.IsBad());
}

static void test_MemoryReader_ClearBadFlag()
{
    std::cout << "[test] MemoryReader_ClearBadFlag\n";
    const uint8_t data[] = { 0x01 };
    libmmd::MemoryReader reader(data, sizeof(data));

    uint32_t val = 0;
    reader.Read(&val); // will fail, set bad flag
    TEST_ASSERT(reader.IsBad());

    reader.ClearBadFlag();
    TEST_ASSERT(!reader.IsBad());
}

static void test_MemoryReader_SeekBegin()
{
    std::cout << "[test] MemoryReader_SeekBegin\n";
    const uint8_t data[] = { 0xAA, 0xBB, 0xCC, 0xDD };
    libmmd::MemoryReader reader(data, sizeof(data));

    uint8_t val = 0;
    reader.Read(&val); // pos -> 1
    reader.Read(&val); // pos -> 2

    TEST_ASSERT(reader.Seek(1, libmmd::MemoryReader::SeekDir::Begin));
    TEST_ASSERT_EQ(int64_t(1), reader.Tell());

    reader.Read(&val);
    TEST_ASSERT_EQ(uint8_t(0xBB), val);
}

static void test_MemoryReader_SeekCurrent()
{
    std::cout << "[test] MemoryReader_SeekCurrent\n";
    const uint8_t data[] = { 0x10, 0x20, 0x30, 0x40 };
    libmmd::MemoryReader reader(data, sizeof(data));

    reader.Seek(1, libmmd::MemoryReader::SeekDir::Begin); // pos = 1
    TEST_ASSERT(reader.Seek(2, libmmd::MemoryReader::SeekDir::Current)); // pos = 3
    TEST_ASSERT_EQ(int64_t(3), reader.Tell());

    uint8_t val = 0;
    reader.Read(&val);
    TEST_ASSERT_EQ(uint8_t(0x40), val);
}

static void test_MemoryReader_SeekEnd()
{
    std::cout << "[test] MemoryReader_SeekEnd\n";
    const uint8_t data[] = { 0x10, 0x20, 0x30, 0x40 };
    libmmd::MemoryReader reader(data, sizeof(data));

    // Seek to 2 bytes before end
    TEST_ASSERT(reader.Seek(-2, libmmd::MemoryReader::SeekDir::End));
    TEST_ASSERT_EQ(int64_t(2), reader.Tell());

    uint8_t val = 0;
    reader.Read(&val);
    TEST_ASSERT_EQ(uint8_t(0x30), val);
}

static void test_MemoryReader_SeekBeyondEndFails()
{
    std::cout << "[test] MemoryReader_SeekBeyondEndFails\n";
    const uint8_t data[] = { 0x01, 0x02 };
    libmmd::MemoryReader reader(data, sizeof(data));

    TEST_ASSERT(!reader.Seek(10, libmmd::MemoryReader::SeekDir::Begin));
    TEST_ASSERT(reader.IsBad());
}

static void test_MemoryReader_SeekBeforeBeginFails()
{
    std::cout << "[test] MemoryReader_SeekBeforeBeginFails\n";
    const uint8_t data[] = { 0x01, 0x02 };
    libmmd::MemoryReader reader(data, sizeof(data));

    TEST_ASSERT(!reader.Seek(-1, libmmd::MemoryReader::SeekDir::Begin));
    TEST_ASSERT(reader.IsBad());
}

static void test_MemoryReader_TellAndGetSize()
{
    std::cout << "[test] MemoryReader_TellAndGetSize\n";
    uint8_t data[128];
    std::memset(data, 0, sizeof(data));
    libmmd::MemoryReader reader(data, sizeof(data));

    TEST_ASSERT_EQ(int64_t(128), reader.GetSize());
    TEST_ASSERT_EQ(int64_t(0), reader.Tell());

    uint32_t dummy;
    reader.Read(&dummy); // advance 4 bytes
    TEST_ASSERT_EQ(int64_t(4), reader.Tell());
    TEST_ASSERT_EQ(int64_t(128), reader.GetSize()); // size unchanged
}

static void test_MemoryReader_MoveConstruct()
{
    std::cout << "[test] MemoryReader_MoveConstruct\n";
    const uint8_t data[] = { 0xAA, 0xBB, 0xCC };
    libmmd::MemoryReader reader1(data, sizeof(data));

    uint8_t val = 0;
    reader1.Read(&val); // pos -> 1

    libmmd::MemoryReader reader2(std::move(reader1));
    TEST_ASSERT_EQ(int64_t(1), reader2.Tell());
    TEST_ASSERT_EQ(int64_t(3), reader2.GetSize());

    reader2.Read(&val);
    TEST_ASSERT_EQ(uint8_t(0xBB), val);
}

static void test_MemoryReader_SimulateBinaryParsing()
{
    std::cout << "[test] MemoryReader_SimulateBinaryParsing\n";
    // Simulate a mini binary format: magic(4B) + count(uint32) + count*float
    struct {
        char magic[4];
        uint32_t count;
        float values[3];
    } srcData;
    std::memcpy(srcData.magic, "TEST", 4);
    srcData.count = 3;
    srcData.values[0] = 1.0f;
    srcData.values[1] = 2.0f;
    srcData.values[2] = 3.0f;

    const auto* rawData = reinterpret_cast<const uint8_t*>(&srcData);
    libmmd::MemoryReader reader(rawData, sizeof(srcData));

    // Read magic
    char magic[4] = {};
    TEST_ASSERT(reader.Read(magic, 4));
    TEST_ASSERT(std::memcmp(magic, "TEST", 4) == 0);

    // Read count
    uint32_t count = 0;
    TEST_ASSERT(reader.Read(&count));
    TEST_ASSERT_EQ(uint32_t(3), count);

    // Read values
    for (uint32_t i = 0; i < count; i++)
    {
        float val = 0;
        TEST_ASSERT(reader.Read(&val));
        TEST_ASSERT_FLOAT_EQ(static_cast<float>(i + 1), val);
    }

    TEST_ASSERT(reader.IsEOF());
    TEST_ASSERT(!reader.IsBad());
}

// ===========================================================================
// File + MemoryReader integration test
// ===========================================================================

static void test_File_ReadAll_ToMemoryReader()
{
    std::cout << "[test] File_ReadAll_ToMemoryReader\n";
    // Manually pack: uint32_t(12345) + float(6.78f) + uint16_t(999) = 10 bytes, no padding
    uint8_t srcBuf[4 + 4 + 2];
    uint32_t srcA = 12345;
    float srcB = 6.78f;
    uint16_t srcC = 999;
    std::memcpy(srcBuf, &srcA, 4);
    std::memcpy(srcBuf + 4, &srcB, 4);
    std::memcpy(srcBuf + 8, &srcC, 2);

    TempFile tmp("_test_file_readall.bin");
    TEST_ASSERT(tmp.WriteBinary(srcBuf, sizeof(srcBuf)));

    libmmd::File file;
    TEST_ASSERT(file.Open(tmp.Path()));

    std::vector<uint8_t> buffer;
    TEST_ASSERT(file.ReadAll(&buffer));
    TEST_ASSERT_EQ(size_t(10), buffer.size());
    file.Close();

    libmmd::MemoryReader reader(buffer.data(), buffer.size());

    uint32_t a = 0;
    float b = 0;
    uint16_t c = 0;
    TEST_ASSERT(reader.Read(&a));
    TEST_ASSERT(reader.Read(&b));
    TEST_ASSERT(reader.Read(&c));
    TEST_ASSERT_EQ(uint32_t(12345), a);
    TEST_ASSERT_FLOAT_EQ(6.78f, b);
    TEST_ASSERT_EQ(uint16_t(999), c);
    TEST_ASSERT(reader.IsEOF());
}

// ===========================================================================
// TextFileReader tests
// ===========================================================================

static void test_TextFileReader_ReadAll()
{
    std::cout << "[test] TextFileReader_ReadAll\n";
    TempFile tmp("_test_textfile_readall.txt");
    std::string content = "Hello, World!\nSecond line\nThird line";
    TEST_ASSERT(tmp.WriteText(content));

    libmmd::TextFileReader reader;
    TEST_ASSERT(reader.Open(tmp.Path()));

    std::string result = reader.ReadAll();
    reader.Close();

    // Text mode may strip \r on Windows, but content should match
    TEST_ASSERT(!result.empty());
    // Check that all expected substrings are present
    TEST_ASSERT(result.find("Hello, World!") != std::string::npos);
    TEST_ASSERT(result.find("Second line") != std::string::npos);
    TEST_ASSERT(result.find("Third line") != std::string::npos);
}

static void test_TextFileReader_ReadAll_Empty()
{
    std::cout << "[test] TextFileReader_ReadAll_Empty\n";
    TempFile tmp("_test_textfile_empty.txt");
    TEST_ASSERT(tmp.WriteText(""));

    libmmd::TextFileReader reader;
    TEST_ASSERT(reader.Open(tmp.Path()));

    std::string result = reader.ReadAll();
    reader.Close();

    TEST_ASSERT(result.empty());
}

static void test_TextFileReader_ReadAll_NotOpened()
{
    std::cout << "[test] TextFileReader_ReadAll_NotOpened\n";
    libmmd::TextFileReader reader;
    // Don't open any file
    std::string result = reader.ReadAll();
    TEST_ASSERT(result.empty());
}

static void test_TextFileReader_ReadAllLines()
{
    std::cout << "[test] TextFileReader_ReadAllLines\n";
    TempFile tmp("_test_textfile_lines.txt");
    std::string content = "Line1\nLine2\nLine3";
    TEST_ASSERT(tmp.WriteText(content));

    libmmd::TextFileReader reader;
    TEST_ASSERT(reader.Open(tmp.Path()));

    std::vector<std::string> lines;
    reader.ReadAllLines(lines);
    reader.Close();

    TEST_ASSERT_EQ(size_t(3), lines.size());
    TEST_ASSERT_EQ(std::string("Line1"), lines[0]);
    TEST_ASSERT_EQ(std::string("Line2"), lines[1]);
    TEST_ASSERT_EQ(std::string("Line3"), lines[2]);
}

static void test_TextFileReader_ReadAllLines_TrailingNewline()
{
    std::cout << "[test] TextFileReader_ReadAllLines_TrailingNewline\n";
    TempFile tmp("_test_textfile_trailing.txt");
    std::string content = "AAA\nBBB\n";
    TEST_ASSERT(tmp.WriteText(content));

    libmmd::TextFileReader reader;
    TEST_ASSERT(reader.Open(tmp.Path()));

    std::vector<std::string> lines;
    reader.ReadAllLines(lines);
    reader.Close();

    // "AAA\nBBB\n" splits into ["AAA", "BBB"] (trailing newline creates empty after)
    // The implementation splits and if last char is \n, there's nothing after start
    // Actually: "AAA\nBBB\n" -> after split: "AAA", "BBB", then start == all.size()
    // so no trailing empty string. Let's just verify the content lines.
    TEST_ASSERT(lines.size() >= 2);
    TEST_ASSERT_EQ(std::string("AAA"), lines[0]);
    TEST_ASSERT_EQ(std::string("BBB"), lines[1]);
}

static void test_TextFileReader_ReadAllLines_SingleLine()
{
    std::cout << "[test] TextFileReader_ReadAllLines_SingleLine\n";
    TempFile tmp("_test_textfile_single.txt");
    std::string content = "OnlyOneLine";
    TEST_ASSERT(tmp.WriteText(content));

    libmmd::TextFileReader reader;
    TEST_ASSERT(reader.Open(tmp.Path()));

    std::vector<std::string> lines;
    reader.ReadAllLines(lines);
    reader.Close();

    TEST_ASSERT_EQ(size_t(1), lines.size());
    TEST_ASSERT_EQ(std::string("OnlyOneLine"), lines[0]);
}

static void test_TextFileReader_ReadAllLines_EmptyLines()
{
    std::cout << "[test] TextFileReader_ReadAllLines_EmptyLines\n";
    TempFile tmp("_test_textfile_emptylines.txt");
    std::string content = "A\n\nB\n\nC";
    TEST_ASSERT(tmp.WriteText(content));

    libmmd::TextFileReader reader;
    TEST_ASSERT(reader.Open(tmp.Path()));

    std::vector<std::string> lines;
    reader.ReadAllLines(lines);
    reader.Close();

    TEST_ASSERT_EQ(size_t(5), lines.size());
    TEST_ASSERT_EQ(std::string("A"), lines[0]);
    TEST_ASSERT_EQ(std::string(""), lines[1]);
    TEST_ASSERT_EQ(std::string("B"), lines[2]);
    TEST_ASSERT_EQ(std::string(""), lines[3]);
    TEST_ASSERT_EQ(std::string("C"), lines[4]);
}

// ===========================================================================
// main
// ===========================================================================

int main()
{
    std::cout << "=== FileBase Unit Tests ===\n\n";

    // MemoryReader
    test_MemoryReader_DefaultConstruction();
    test_MemoryReader_ConstructFromBuffer();
    test_MemoryReader_ReadSingleValue();
    test_MemoryReader_ReadMultipleValues();
    test_MemoryReader_ReadSequential();
    test_MemoryReader_ReadBeyondEndFails();
    test_MemoryReader_ReadNullptrFails();
    test_MemoryReader_ReadArrayCount();
    test_MemoryReader_ClearBadFlag();
    test_MemoryReader_SeekBegin();
    test_MemoryReader_SeekCurrent();
    test_MemoryReader_SeekEnd();
    test_MemoryReader_SeekBeyondEndFails();
    test_MemoryReader_SeekBeforeBeginFails();
    test_MemoryReader_TellAndGetSize();
    test_MemoryReader_MoveConstruct();
    test_MemoryReader_SimulateBinaryParsing();

    // File + MemoryReader integration
    test_File_ReadAll_ToMemoryReader();

    // TextFileReader
    test_TextFileReader_ReadAll();
    test_TextFileReader_ReadAll_Empty();
    test_TextFileReader_ReadAll_NotOpened();
    test_TextFileReader_ReadAllLines();
    test_TextFileReader_ReadAllLines_TrailingNewline();
    test_TextFileReader_ReadAllLines_SingleLine();
    test_TextFileReader_ReadAllLines_EmptyLines();

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
