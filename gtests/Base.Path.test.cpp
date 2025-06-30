#include <gtest/gtest.h>

#include <libMMD/Base/Path.h>

TEST(BaseTest, PathTest)
{
	// Combine
#if _WIN32
	EXPECT_EQ(std::string("abc\\def\\efg"), libmmd::PathUtil::Combine({ "abc", "def\\", "", "efg" }));
	EXPECT_EQ(std::string("abc\\def\\efg"), libmmd::PathUtil::Combine("abc", "def\\efg"));
#else
	EXPECT_EQ(std::string("abc/def/efg"), libmmd::PathUtil::Combine({ "abc", "def/", "", "efg" }));
	EXPECT_EQ(std::string("abc/def/efg"), libmmd::PathUtil::Combine("abc", "def/efg"));
#endif

	// DirectoryName
#if _WIN32
	EXPECT_EQ(std::string("abc\\def"), libmmd::PathUtil::GetDirectoryName("abc\\def/efg.txt"));
#else // _WIN32
	EXPECT_EQ(std::string("abc\\def"), libmmd::PathUtil::GetDirectoryName("abc\\def/efg.txt"));
#endif
	EXPECT_EQ(std::string("abc/def"), libmmd::PathUtil::GetDirectoryName("abc/def/"));
	EXPECT_EQ(std::string(""), libmmd::PathUtil::GetDirectoryName("abc"));
	EXPECT_EQ(std::string(""), libmmd::PathUtil::GetDirectoryName(""));

	// Filename
	EXPECT_EQ(std::string("test.txt"), libmmd::PathUtil::GetFilename("abc/test.txt"));
	EXPECT_EQ(std::string("test.txt"), libmmd::PathUtil::GetFilename("test.txt"));
	EXPECT_EQ(std::string(""), libmmd::PathUtil::GetFilename(""));

	// FilenameWithoutText
	EXPECT_EQ(std::string("test"), libmmd::PathUtil::GetFilenameWithoutExt("abc/test.txt"));
	EXPECT_EQ(std::string("test"), libmmd::PathUtil::GetFilenameWithoutExt("test.txt"));
	EXPECT_EQ(std::string("test"), libmmd::PathUtil::GetFilenameWithoutExt("test"));
	EXPECT_EQ(std::string("abc.test"), libmmd::PathUtil::GetFilenameWithoutExt("abc.test.txt"));
	EXPECT_EQ(std::string(""), libmmd::PathUtil::GetFilenameWithoutExt(""));

	// GetExt
	EXPECT_EQ(std::string("txt"), libmmd::PathUtil::GetExt("abc/test.txt"));
	EXPECT_EQ(std::string("txt"), libmmd::PathUtil::GetExt("test.txt"));
	EXPECT_EQ(std::string("txt"), libmmd::PathUtil::GetExt("abc.test.txt"));
	EXPECT_EQ(std::string(""), libmmd::PathUtil::GetExt("test"));
	EXPECT_EQ(std::string(""), libmmd::PathUtil::GetExt("."));
	EXPECT_EQ(std::string(""), libmmd::PathUtil::GetExt(""));

	// Normalize
#if _WIN32
	EXPECT_EQ(std::string("a\\b\\c\\"), libmmd::PathUtil::Normalize("a/b\\c/"));
#else // _WIN32
	EXPECT_EQ(std::string("a/b/c/"), libmmd::PathUtil::Normalize("a\\b/c\\"));
#endif
}
