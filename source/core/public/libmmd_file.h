/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & libMMD contributors.
Author:			Aimidi
Date:			2023/4/8
File:			file_util.hpp
Description:	file utils 

**************************************************************************/

#pragma once
#include <cassert>
#include <span>
#include "libMMD/libmmd_marco.h"

#if LIBMMD_SYS_WINDOWS
#	define WIN32_LEAN_AND_MEAN 1
#	include <Windows.h>
#else
#	include <cstdio>
#endif

namespace libmmd
{
	class file;

	class path
	{
	public:
		/**
		 * \brief Default constructor.
		 */
		path() = default;

		/**
		 * \brief Constructs the path from a u8string.
		 * \param string The path string.
		 */
		explicit path(const std::u8string& string) :path_(string) {}

		/**
		 * \brief Constructs the path from a localization string.
		 * \param string The path string.
		 */
		explicit path(const std::string& string) :path_(string) {}

		/**
		 * \brief Constructs the path from a wstring.
		 * \param string The path string.
		 */
		explicit path(const std::wstring& string) :path_(string) {}

		/**
		 * \brief Constructs the path from a u8string_view.
		 * \param string The path string.
		 */
		explicit path(const std::u8string_view& string) :path_(string) {}

		/**
		 * \brief Constructs the path from a localization string_view.
		 * \param string The path string.
		 */
		explicit path(const std::string_view& string) :path_(string) {}

		/**
		 * \brief Constructs the path from a wstring_view.
		 * \param string The path string.
		 */
		explicit path(const std::wstring_view& string) :path_(string) {}

		/**
		 * \brief Copy constructor. Creates a path from another path.
		 * \param src The source path.
		 */
		path(const path& src) = default;

		/**
		 * \brief Move constructor. Creates a path from another path.
		 * \param src The source path.
		 */
		path(path&& src) = default;

		/**
		 * \brief Default destructor.
		 */
		~path() = default;


		/**
		 * \brief Checks if the path has been set.
		 * \return If path was set true, Otherwise false.
		 */
		[[nodiscard]] bool is_populated() const
		{
			return !path_.empty();
		}

		/**
		 * \brief Gets the localization wide string of the path.
		 * \return The string with the full path.
		 */
		[[nodiscard]] std::string get_string() const
		{
			return path_.string();
		}

		/**
		 * \brief Gets the localization string of the path.
		 * \return The string with the full path.
		 */
		[[nodiscard]] std::wstring get_wstring() const
		{
			return path_.wstring();
		}

		/**
		 * \brief Sets the path to the passed string.
		 * \param str The utf8 string to use to set the path.
		 */
		void set_string(const std::u8string& str)
		{
			path_.assign(str);
		}

		/**
		 * \brief Sets the path to the passed string.
		 * \param str The localization string to use to set the path.
		 */
		void set_string(const std::string& str)
		{
			path_.assign(str);
		}

		/**
		 * \brief Sets the path to the passed string.
		 * \param str The wstring to use to set the path.
		 */
		void set_string(const std::wstring& str)
		{
			path_.assign(str);
		}


		/**
		 * \brief Gets the directory that the file is located.
		 * \return The directory part for the path.
		 */
		[[nodiscard]] path get_directory() const
		{
			path fn;
			if (is_directory(path_))
				fn.path_ = path_;
			else
				fn.path_ = path_.parent_path();
			return fn;
		}

		/**
		 * \brief Sets the directory for the path. The file part remains unchanged.
		 * \param str The directory to set for the path.
		 */
		void set_directory(const path& str)
		{
			const auto filename = str.path_.filename();
			path_.replace_filename(filename);
		}

		/**
		 * \brief Gets the file part of the path only, without the path.
		 * \return The file part of the path.
		 */
		[[nodiscard]] path get_file() const
		{
			path fn;
			fn.path_ = path_.filename();
			return fn;
		}

		/**
		 * \brief Sets the file part for the path. The directory part remains unchanged.
		 * \param str The file part to set for the path.
		 */
		void set_file(const path& str)
		{
			path_.replace_filename(str.path_.filename());
		}

		/**
		 * \brief Convenience function to directly get the file part string. Identical to GetFile().GetString().
		 * \return The file part of the path as a localization string.
		 */
		[[nodiscard]] std::string get_file_string() const
		{
			return path_.filename().string();
		}

		/**
		 * \brief Convenience function to directly get the file part string. Identical to GetFile().GetString().
		 * \return The file part of the path as a wstring.
		 */
		[[nodiscard]] std::wstring get_file_wstring() const
		{
			return path_.filename().wstring();
		}

		/**
		 * \brief Gets the suffix of the path.
		 * \note Suffix strings do not contain the dot character @em '.'.
		 * \return The suffix. Only contains the suffix part without dot (e.g. @em "TIF").
		 */
		[[nodiscard]] std::string get_suffix() const
		{
			return path_.extension().string();
		}

		/**
		 * \brief Sets the suffix part of the path.
		 * \note Suffix strings do not contain the dot character @em '.'.
		 * \param str A string with the suffix for the file. Only contains the suffix part and no dot (e.g. @em "TIF").
		 */
		void set_suffix(const std::string& str)
		{
			path_.replace_extension(str);
		}

		/**
		 * \brief Clears the suffix part of the path.
		 */
		void clear_suffix()
		{
			path_.replace_extension();
		}

		/**
		 * \brief Checks if the suffix of the path is the same as its string.
		 * \param str The suffix to check against.
		 * \return If the suffix was checked successfully return true, Otherwise false.
		 */
		[[nodiscard]] bool check_suffix(const std::string& str) const
		{
			auto check_string = str;
			auto extension_string = path_.extension().string();
			std::ranges::transform(check_string, check_string.begin(), tolower);
			std::ranges::transform(extension_string, extension_string.begin(), tolower);
			return check_string == extension_string;
		}

		/**
		 * \brief Checks if the path exists in the file system.
		 * \return If the path exists return true, Otherwise returns false.
		 */
		bool is_exists() const
		{
			return exists(path_);
		}

		/**
		 * \brief Retrieves the size of the file specified by the path.
		 * \return Returns the size of the file in bytes.\n
		 * If the path does not refer to a regular file, the behavior is implementation-defined.
		 */
		uint64_t get_file_size() const
		{
			return file_size(path_);
		}


		/**
		 * \brief Copy assignment operator. Assigns the right-operand path to the left-operand path.
		 * \param path The right-operand path.
		 * \return The left-operand path.
		 */
		path& operator=(const path& path) = default;

		/**
		 * \brief Move assignment operator. Assigns the right-operand path to the left-operand path.
		 * \param path The right-operand path.
		 * \return The left-operand path.
		 */
		path& operator=(path&& path) = default;

		/**
		 * \brief Concatenates the two file paths. It will join directories and files parts.
		 * \param path1 The left-operand path.
		 * \param path2 The right-operand path.
		 * \return The result of the concatenation of the 2 file_paths.
		 */
		friend path operator+(const path& path1, const path& path2)
		{
			path fn = path1;
			fn.path_ += path2.path_;
			return fn;
		}

		/**
		 * \brief Concatenates the file paths and set the left-hand operand to the result.
		 * \param path The right-operand path.
		 * \return The left-operand path.
		 */
		const path& operator+=(const path& path)
		{
			path_ += path.path_;
			return *this;
		}

		/**
		 * \brief Checks if two file paths are equal.
		 * \param path The right-operand path.
		 * \return If file_paths are equal true, Otherwise false.
		 */
		bool operator==(const path& path) const
		{
			return path_ == path.path_;
		}

		/**
		 * \brief Check if two file paths are different.
		 * \param path The right-operand path.
		 * \return If file_paths are different true, Otherwise false.
		 */
		bool operator!=(const path& path) const
		{
			return path_ != path.path_;
		}

		/**
		 * \brief Copies the path to dst.
		 * \param dst The destination path. callerOwnsPointed{path}
		 */
		void copy_to(path* dst) const
		{
			dst->path_ = path_;
		}
	private:
		std::filesystem::path path_{};
	};

	class file
	{
	public:
		enum class open_mode
		{
			APPEND = 0,		/// Open an existing file for writing and set the position to the end of that file.
			READ = 1,		/// Open the file for reading.
			WRITE = 2,		/// Create a new file for writing.
			/// @warning If the file name points to an existing file, it will be overwritten.
			READWRITE = 3,		/// Open the file for both reading and writing.
		};

		enum class seek_mode
		{
			START = 0,			/// The position is given relative to the start of the file.
			RELATIVE_ = 1		/// The position is given relative to the current position.
		};


		/**
		 * \brief Default constructor.
		 */
		file() = default;

		/**
		 * \brief Default destructor.
		 */
		~file()
		{
			if (file_)
				close();
		}

		file(const file&) = delete;
		file(file&&) = delete;
		bool operator==(const file&) const = delete;
		bool operator==(file&&) const = delete;


		/**
		 * \brief Opens a file.
		 * \param path The name of the file to open.
		 * \param mode The file access mode: @enumerateEnum{open_mode}
		 * \return If the file was opened without any problem true, Otherwise false.
		 */
		bool open(const path& path, const open_mode mode = open_mode::READ)
		{
			open_mode_ = mode;

			if (path.is_exists())
			{
				length_ = path.get_file_size();
			}

			if (mode == open_mode::READ && length_ == 0)
			{
				return false;
			}
#if LIBMMD_SYS_WINDOWS
			const auto path_str = path.get_wstring();

			static constexpr std::tuple<DWORD, DWORD> win_modes[] = {
				{FILE_APPEND_DATA, OPEN_ALWAYS},
				{GENERIC_READ , OPEN_EXISTING},
				{GENERIC_WRITE, CREATE_ALWAYS},
				{GENERIC_READ | GENERIC_WRITE, OPEN_ALWAYS}
		};

			const auto& [file_access, file_disposition] = win_modes[static_cast<int>(mode)];

			if (file_ = CreateFileW(path_str.c_str(), file_access, 0, nullptr, file_disposition, FILE_ATTRIBUTE_NORMAL, nullptr); file_ == nullptr)
			{
				return false;
			}
#else
			const char* path_str = path.get_string().c_str();

			static const char* mode_string[] = { "ab", "rb", "wb", "w+b" };
			if (file_ = fopen(path_str, mode_string[static_cast<int>(mode)]); file_ == nullptr)
			{
				return false;
			}
#endif
			return true;
	}

		/**
		 * \brief Closes the file.\n
		 * Automatically called when a file object is destroyed.
		 * \return If the file was successfully closed true, Otherwise false.
		 */
		bool close()
		{
			if(file_)
			{
#if LIBMMD_SYS_WINDOWS
				CloseHandle(file_);
#else
				fclose(file_);
#endif
			}
			length_ = 0;
			file_ = nullptr;

			return true;
		}

		/**
		 * \brief Read a element{T} from file.
		 * \tparam T Element type.
		 * \param data Element to read.
		 * \return If the file was successfully read true, Otherwise false.
		 */
		template<typename T>
		bool read_element(T& data) const
		{
			if(!file_)
				return false;
			constexpr auto elements_size = sizeof(T);
#if LIBMMD_SYS_WINDOWS
			if (DWORD bytesRead; !ReadFile(file_, &data, elements_size, &bytesRead, nullptr) || bytesRead != elements_size)
			{
				return false;
			}
			return true;
#else
			return fread(&data, elements_size, 1, file_) != 0;
#endif
		}

		/**
		 * \brief Read {num} of elements{T} from file.
		 * \tparam T Elements type.
		 * \param data Elements to read.
		 * \param num Elements number.
		 * \return If the file was successfully read true, Otherwise false.
		 */
		template<typename T, size_t N>
		bool read_elements(const std::span<T, N>& data, const UInt64 num = 1) const
		{
			if (!file_)
				return false;
			constexpr auto elements_size = sizeof(T);
			assert(N >= num);
#if LIBMMD_SYS_WINDOWS
			if (DWORD bytesRead; !ReadFile(file_, data.data(), elements_size * num, &bytesRead, nullptr)
				|| bytesRead != elements_size * num)
			{
				return false;
			}
			return true;
#else
			return fread(data.data(), elements_size, num, file_) != 0;
#endif
		}

		/**
		 * \brief Write a element{T} to file.
		 * \tparam T Element type.
		 * \param data Element to write.
		 * \return If the file was successfully write true, Otherwise false.
		 */
		template<typename T>
		bool write_element(const T& data) const
		{
			if (!file_)
				return false;
			constexpr auto elements_size = sizeof(T);
#if LIBMMD_SYS_WINDOWS
			if (DWORD bytesWritten; !WriteFile(file_, &data, elements_size, &bytesWritten, nullptr)
				 || bytesWritten != elements_size)
			{
				return false;
			}
			return true;
#else
			return fwrite(&data, elements_size, 1, file_) != 0;
#endif
		}

		/**
		 * \brief Write {num} of elements{T} to file.
		 * \tparam T Elements type.
		 * \param data Elements to write.
		 * \param num Elements number.
		 * \return If the file was successfully write true, Otherwise false.
		 */
		template<typename T, size_t N>
		bool write_elements(const std::span<T, N> data, const UInt64 num = 1) const
		{
			if (!file_)
				return false;
			assert(N >= num);
			constexpr auto elements_size = sizeof(T);
#if LIBMMD_SYS_WINDOWS
			if (DWORD bytesWritten; !WriteFile(file_, data.data(), elements_size * num, &bytesWritten, nullptr)
				 || bytesWritten != elements_size * num)
			{
				return false;
			}
			return true;
#else
			return fwrite(data.data(), elements_size, num, file_) != 0;
#endif
		}

		/**
		 * \brief Sets the file pointer within the file.
		 * \param pos The position within the file to place the file pointer.
		 * \param mode Sets how the position relates to the file.
		 * \return If the file pointer position was successfully changed true, Otherwise false.
		 */
		bool seek(const Int64 pos, seek_mode mode = seek_mode::RELATIVE_) const
		{
			if (!file_)
				return false;
#if LIBMMD_SYS_WINDOWS
			LARGE_INTEGER liDistanceToMove;
			liDistanceToMove.QuadPart = pos;

			DWORD dwMoveMethod;
			switch (mode)
			{
			case seek_mode::START:
				dwMoveMethod = FILE_BEGIN;
				break;
			case seek_mode::RELATIVE_:
				dwMoveMethod = FILE_CURRENT;
				break;
			default:
				// Handle unexpected mode value.
				return false;
			}

			LARGE_INTEGER liNewFilePointer;
			if (!SetFilePointerEx(file_, liDistanceToMove, &liNewFilePointer, dwMoveMethod))
			{
				return false;
			}

			return true;
#else
			int c_mode;
			switch (mode)
			{
			case seek_mode::START:
				c_mode = SEEK_SET;
				break;
			case seek_mode::RELATIVE_:
				c_mode = SEEK_CUR;
				break;
			default:
				// Handle unexpected mode value.
				return false;
			}
			return _fseeki64(file_, pos, c_mode) == 0;
#endif
		}


		/**
		 * \brief Get file read/write 
		 * \return 
		 */
		UInt64 get_position() const
		{
			if (!file_)
				return false;
#if LIBMMD_SYS_WINDOWS
			constexpr LARGE_INTEGER liZero = {};
			LARGE_INTEGER liNewFilePointer;
			if (!SetFilePointerEx(file_, liZero, &liNewFilePointer, FILE_CURRENT))
			{
				// Handle error or return some sentinel/error value.
				return static_cast<UInt64>(-1);
			}
			return liNewFilePointer.QuadPart;
#else
			return _ftelli64(file_);
#endif
		}

		/**
		 * \brief Gets the length of the file in bytes.
		 * \return The byte length of the file.
		 */
		UInt64 get_length() const
		{
			return length_;
		}

		/**
		 * \brief Gets the last error during read or write.
		 * \return The last error
		 */
		int get_error() const
		{
			if (!file_)
				return false;
#if LIBMMD_SYS_WINDOWS
			return GetLastError();
#else
			return ferror(file_);
#endif
		}

	private:
#if LIBMMD_SYS_WINDOWS
		HANDLE file_ = nullptr;
#else
		FILE* file_ = nullptr;
#endif

		open_mode open_mode_ = open_mode::READ;
		UInt64 length_ = 0;
	};
}

#if LIBMMD_SYS_WINDOWS
#undef WIN32_LEAN_AND_MEAN 1

// Undef potentially problematic macros
#undef min
#undef max
#undef small
#undef DELETE
#undef ERROR
#undef NO_ERROR
#undef CreateWindow
#undef LoadImage
#undef TextOut
#undef DrawText
#undef TranslateMessage
#undef DispatchMessage
#undef IN
#undef OUT
#undef FAR

#endif
