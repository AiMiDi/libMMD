//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "Path.h"

#include <algorithm>
#include <filesystem>
#include <cctype>

#if _WIN32
#include <Windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#include <climits>
#elif __linux
#include <unistd.h>
#endif

namespace fs = std::filesystem;

namespace libmmd
{
	namespace
	{
		// C++20 u8string uses char8_t; the public API retains UTF-8 bytes
		// in std::string for C++17 callers, without locale conversion.
		std::string Utf8Bytes(const fs::path& path)
		{
			const auto encoded = path.u8string();
			return std::string(reinterpret_cast<const char*>(encoded.data()), encoded.size());
		}
	}

	std::string PathUtil::GetCWD()
	{
		return Utf8Bytes(fs::current_path());
	}

	std::string PathUtil::GetExecutablePath()
	{
#if _WIN32
		std::vector<wchar_t> modulePath(MAX_PATH);
		if (GetModuleFileNameW(nullptr, modulePath.data(), static_cast<DWORD>(modulePath.size())) == 0)
		{
			return "";
		}
		return Utf8Bytes(fs::path(modulePath.data()));
#elif __APPLE__
		char pathbuf[PATH_MAX + 1];
		uint32_t bufsize = sizeof(pathbuf);
		if (_NSGetExecutablePath(pathbuf, &bufsize) != 0)
		{
			return "";
		}
		return pathbuf;
#elif __linux
		char pathbuf[1024 + 1] = {};
		ssize_t sz = readlink("/proc/self/exe", pathbuf, sizeof(pathbuf) - 1);
		if (sz == -1)
		{
			return "";
		}
		pathbuf[sz] = '\0';
		return pathbuf;
#else
		return "";
#endif
	}

	std::string PathUtil::Combine(const std::vector<std::string>& parts)
	{
		fs::path result;
		for (const auto& part : parts)
		{
			if (!part.empty())
			{
				if (result.empty())
				{
					result = fs::u8path(part);
				}
				else
				{
					result /= fs::u8path(part);
				}
			}
		}
		return Utf8Bytes(result);
	}

	std::string PathUtil::Combine(const std::string& a, const std::string& b)
	{
		if (a.empty()) return b;
		if (b.empty()) return a;
		return Utf8Bytes(fs::u8path(a) / fs::u8path(b));
	}

	std::string PathUtil::GetDirectoryName(const std::string& path)
	{
		return Utf8Bytes(fs::u8path(path).parent_path());
	}

	std::string PathUtil::GetFilename(const std::string& path)
	{
		return Utf8Bytes(fs::u8path(path).filename());
	}

	std::string PathUtil::GetFilenameWithoutExt(const std::string& path)
	{
		return Utf8Bytes(fs::u8path(path).stem());
	}

	std::string PathUtil::GetExt(const std::string& path)
	{
		std::string ext = Utf8Bytes(fs::u8path(path).extension());
		// extension() returns with leading dot (e.g. ".txt"), strip it
		if (!ext.empty() && ext[0] == '.')
		{
			ext = ext.substr(1);
		}
		for (auto& ch : ext)
		{
			ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
		}
		return ext;
	}

	std::string PathUtil::GetDelimiter()
	{
		return std::string(1, static_cast<char>(fs::path::preferred_separator));
	}

	std::string PathUtil::Normalize(const std::string& path)
	{
		std::string result = path;
#if _WIN32
		std::replace(result.begin(), result.end(), '/', '\\');
#else
		std::replace(result.begin(), result.end(), '\\', '/');
#endif
		return result;
	}

}
