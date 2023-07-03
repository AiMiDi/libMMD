/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & libMMD contributors.
Author:			Aimidi
Date:			2023/4/8
File:			file_util.hpp
Description:	file utils 

**************************************************************************/

#pragma once

namespace libmmd
{
	class path
	{
	public:
		/// Constructor & Destructor

		//---------------------------------------------------------------------------------------
		/// Default constructor.
		//---------------------------------------------------------------------------------------
		path() = default;

		//----------------------------------------------------------------------------------------
		/// Constructs the path from a character array.
		/// @param[in] string							The path character array.
		//----------------------------------------------------------------------------------------
		explicit path(const char* string) :path_(string) {}

		explicit path(const wchar_t* wide_string) :path_(wide_string) {}


		//----------------------------------------------------------------------------------------
		/// Constructs the path from a string.
		/// @param[in] string							The path string.
		//----------------------------------------------------------------------------------------
		explicit path(const std::string& string) :path_(string) {}

		explicit path(const std::wstring& wide_string) :path_(wide_string) {}

		//----------------------------------------------------------------------------------------
		/// Copy constructor. Creates a path from another path.
		/// @param[in] src								The source path.
		//----------------------------------------------------------------------------------------
		path(const path& src) = default;

		path(path&& src) = default;

		//---------------------------------------------------------------------------------------
		/// Default destructor.
		//---------------------------------------------------------------------------------------
		~path() = default;



		/// @name String Content
		/// @{

		//----------------------------------------------------------------------------------------
		/// Checks if the path has been set.
		/// @return												@trueIfOtherwiseFalse{path was set}
		//----------------------------------------------------------------------------------------
		[[nodiscard]] bool is_populated() const
		{
			return !path_.empty();
		}

		//----------------------------------------------------------------------------------------
		/// Gets the String of the path.
		/// @return												The string with the full path.
		//----------------------------------------------------------------------------------------
		[[nodiscard]] std::string get_string() const
		{
			return path_.string();
		}

		[[nodiscard]] std::wstring get_wide_string() const
		{
			return path_.wstring();
		}

		//----------------------------------------------------------------------------------------
		/// Sets the path to the passed string.
		/// @param[in] str								The string to use to set the path.
		//----------------------------------------------------------------------------------------
		void set_string(const std::string& str)
		{
			path_.assign(str);
		}

		void set_wide_string(const std::wstring& str)
		{
			path_.assign(str);
		}

		/// @}

		/// @name Directory/File Parts
		/// @{

		//----------------------------------------------------------------------------------------
		/// Gets the directory that the file is located.
		/// @return												The directory part for the path.
		//----------------------------------------------------------------------------------------
		[[nodiscard]] path get_directory() const
		{
			path fn;
			if (is_directory(path_))
				fn.path_ = path_;
			else
				fn.path_ = path_.parent_path();
			return fn;
		}

		//----------------------------------------------------------------------------------------
		/// Sets the directory for the path. The file part remains unchanged.
		/// @param[in] str								The directory to set for the path.
		//----------------------------------------------------------------------------------------
		void set_directory(const path& str)
		{
			const auto filename = path_.filename();
			path_ = str.path_;
			path_.replace_filename(filename);
		}

		//----------------------------------------------------------------------------------------
		/// Gets the file part of the path only, without the path.
		/// @return												The file part of the path.
		//----------------------------------------------------------------------------------------
		[[nodiscard]] path get_file() const
		{
			path fn;
			fn.path_ = path_.filename();
			return fn;
		}

		//----------------------------------------------------------------------------------------
		/// Sets the file part for the path. The directory part remains unchanged.
		/// @param[in] str								The file part to set for the path.
		//----------------------------------------------------------------------------------------
		void set_file(const path& str)
		{
			path_.replace_filename(str.path_.filename());
		}

		//----------------------------------------------------------------------------------------
		/// Convenience function to directly get the file part string. Identical to GetFile().GetString().
		/// @return												The file part of the path as a string.
		//----------------------------------------------------------------------------------------
		[[nodiscard]] std::string get_file_string() const
		{
			return path_.filename().string();
		}

		[[nodiscard]] std::wstring get_file_wide_string() const
		{
			return path_.filename().wstring();
		}

		/// @}

		/// @name Suffix Part
		/// @{

		//----------------------------------------------------------------------------------------
		/// Gets the suffix of the path.
		/// @note Suffix strings do not contain the dot character @em '.'.
		/// @return												The suffix. Only contains the suffix part without dot (e.g. @em "TIF").
		//----------------------------------------------------------------------------------------
		[[nodiscard]] std::string get_suffix() const
		{
			return path_.extension().string();
		}

		//----------------------------------------------------------------------------------------
		/// Sets the suffix part of the path.
		/// @note Suffix strings do not contain the dot character @em '.'.
		/// @param[in] str								A string with the suffix for the file. Only contains the suffix part and no dot (e.g. @em "TIF").
		//----------------------------------------------------------------------------------------
		void set_suffix(const std::string& str)
		{
			path_.replace_extension(str);
		}

		//---------------------------------------------------------------------------------------
		/// Clears the suffix part of the path.
		//---------------------------------------------------------------------------------------
		void clear_suffix()
		{
			path_.replace_extension();
		}

		//----------------------------------------------------------------------------------------
		/// Checks if the suffix of the path is the same as its string.
		/// @param[in] str								The suffix to check against.
		/// @return												@trueIfOtherwiseFalse{the suffix was checked successfully}
		//----------------------------------------------------------------------------------------
		[[nodiscard]] bool check_suffix(const std::string& str) const
		{
			auto check_string = str;
			auto extension_string = path_.extension().string();
			std::ranges::transform(check_string, check_string.begin(), tolower);
			std::ranges::transform(extension_string, extension_string.begin(), tolower);
			return check_string == extension_string;
		}

		/// @}

		/// @name Operators
		/// @{

		//----------------------------------------------------------------------------------------
		/// Assignment operator. Assigns the right-operand path to the left-operand path.
		/// @param[in] path							The right-operand path.
		/// @return												The left-operand path.
		//----------------------------------------------------------------------------------------
		path& operator=(const path& path) = default;

		path& operator=(path&& path) = default;

		//----------------------------------------------------------------------------------------
		/// Concatenates the two file paths. It will join directories and files parts.
		/// @param[in] path1							The left-operand path.
		/// @param[in] path2							The right-operand path.
		/// @return												The result of the concatenation of the 2 file_paths.
		//----------------------------------------------------------------------------------------
		friend path operator+(const path& path1, const path& path2)
		{
			path fn = path1;
			fn.path_ += path2.path_;
			return fn;
		}

		//----------------------------------------------------------------------------------------
		/// Concatenates the file paths and set the left-hand operand to the result.
		/// @param[in] path							The right-operand path.
		/// @return												The left-operand path.
		//----------------------------------------------------------------------------------------
		const path& operator+=(const path& path)
		{
			path_ += path.path_;
			return *this;
		}

		//----------------------------------------------------------------------------------------
		/// Checks if two file paths are equal.
		/// @param[in] path							The right-operand path.
		/// @return									@trueIfOtherwiseFalse{file_paths are equal}
		//----------------------------------------------------------------------------------------
		bool operator==(const path& path) const
		{
			return path_ == path.path_;
		}

		//----------------------------------------------------------------------------------------
		/// Check if two file paths are different.
		/// @param[in] path							The right-operand path.
		/// @return									@trueIfOtherwiseFalse{file_paths are different}
		//----------------------------------------------------------------------------------------
		bool operator!=(const path& path) const
		{
			return path_ != path.path_;
		}

		/// @}

		//----------------------------------------------------------------------------------------
		/// Copies the path to @formatParam{dst}.
		/// @param[out] dst								The destination path. @callerOwnsPointed{path}
		//----------------------------------------------------------------------------------------
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
			APPEND = 0,		///< Open an existing file for writing and set the position to the end of that file.
			READ = 1,		///< Open the file for reading.
			WRITE = 2,		///< Create a new file for writing.
			///< @warning If the file name points to an existing file, it will be overwritten.
			READWRITE = 3,		///< Open the file for both reading and writing.
		};

		enum class seek_mode
		{
			START = 0,			///< The position is given relative to the start of the file.
			RELATIVE_ = 1				///< The position is given relative to the current position.
		};

	private:
		FILE* file_ = nullptr;
		open_mode open_mode_ = open_mode::READ;
		UInt64 length_ = 0;
	public:
		file() = default;
		~file()
		{
			if (file_)
			{
				length_ = 0;
				fclose(file_);
				file_ = nullptr;
			}
		}


		/// @name Open/Close
		/// @{

		//----------------------------------------------------------------------------------------
		/// Opens a file.
		/// @note If a plugin has to be cross platform from a PC to a Mac (generally advised) then it must correctly fill in the @formatParam{type} and @formatParam{creator} parameters which are needed on a Mac.
		/// @param[in] path								The name of the file to open.
		/// @param[in] mode								The file access mode: @enumerateEnum{open_mode}
		/// @return												@trueIfOtherwiseFalse{the file was opened without any problem}
		//----------------------------------------------------------------------------------------
		bool open(const path& path, const open_mode mode = open_mode::READ)
		{
			open_mode_ = mode;
#ifdef _WIN32
			const wchar_t* path_str = path.get_wide_string().c_str();
			if (length_ = get_file_size(path_str); length_ == 0)
			{
				return false;
			}

			static const wchar_t* mode_string[] = { L"ab", L"rb", L"wb", L"w+b" };
			if(const auto errno_code = _wfopen_s(&file_, path_str, mode_string[static_cast<int>(mode)]); errno_code != 0)
			{
				return false;
			}
#else
			const char* path_str = path.get_string().c_str();
			if (length_ = get_file_size(path_str); length_ == 0)
			{
				return false;
			}
			static const char* mode_string[] = { "ab", "rb", "wb", "w+b" };
			if (const auto errno_code = fopen_s(&file_, path_str, mode_string[static_cast<int>(mode)]); errno_code != 0)
			{
				return false;
			}
#endif
			return true;
		}

		//----------------------------------------------------------------------------------------
		/// Closes the file.\n
		/// Automatically called when a file object is destroyed.
		/// @return												@trueIfOtherwiseFalse{the file was successfully closed}
		//----------------------------------------------------------------------------------------
		bool close()
		{
			if(file_)
			{
				length_ = 0;
				fclose(file_);
				file_ = nullptr;
			}

			return true;
		}

		template<typename T>
		UInt64 read_elements(T& data, const UInt64 length = 1) const
		{
			constexpr auto elements_size = sizeof(T);
			return fread_s(&data, length * elements_size, elements_size, length, file_) == 0;
		}

		template<typename T>
		UInt64 read_elements(T* data, const UInt64 length = 1) const
		{
			constexpr auto elements_size = sizeof(T);
			return fread_s(data, length * elements_size, elements_size, length, file_) == 0;
		}

		template<typename T>
		UInt64 write_elements(const T& data, const UInt64 length = 1) const
		{
			return fwrite(&data, sizeof(T), length, file_) == 0;
		}

		template<typename T>
		UInt64 write_elements(const T* data, const UInt64 length = 1) const
		{
			return fwrite(data, sizeof(T), length, file_) == 0;
		}

		
		/// @name File Pointer/Information/Error
		/// @{

		//----------------------------------------------------------------------------------------
		/// Sets the file pointer within the file.
		/// @param[in] pos								The position within the file to place the file pointer.
		/// @param[in] mode								Sets how the position relates to the file.
		/// @return												@trueIfOtherwiseFalse{the file pointer position was successfully changed}
		//----------------------------------------------------------------------------------------
		bool seek(const Int64 pos, seek_mode mode = seek_mode::RELATIVE_) const
		{
			if(_fseeki64(file_, pos, static_cast<int>(mode)) != 0)
			{
				return false;
			}
			return true;
		}

		//----------------------------------------------------------------------------------------
		/// Gets the file pointer.
		/// @return												The position of the file pointer.
		//----------------------------------------------------------------------------------------
		UInt64 get_position() const
		{
			return _ftelli64(file_);
		}

		//----------------------------------------------------------------------------------------
		/// Gets the length of the file in bytes.
		/// @return												The byte length of the file.
		//----------------------------------------------------------------------------------------
		UInt64 get_length() const
		{
			return length_;
		}

		//----------------------------------------------------------------------------------------
		/// Gets the last error during read or write.
		/// @return												The last error: @enumerateEnum{FILEERROR}
		//----------------------------------------------------------------------------------------
		int get_error() const
		{
			return ferror(file_);
		}

	private:
#ifdef _WIN32
		static UInt64 get_file_size(const wchar_t* path)
		{
			if(struct _stat64 stat_buf; path == nullptr || _wstat64(path, &stat_buf) != 0)
			{
				return 0;
			}
			else
			{
				return stat_buf.st_size;
			}
		}
#else
		static UInt64 get_file_size(const char* path)
		{
			if (struct _stat64 stat_buf; path == nullptr || _stat64(path, &stat_buf) != 0)
			{
				return 0;
			}
			else
			{
				return stat_buf.st_size;
			}
		}
#endif
	};
}

