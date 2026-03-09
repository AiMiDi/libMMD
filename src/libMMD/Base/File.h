//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef LIBMMD_BASE_FILE_H_
#define LIBMMD_BASE_FILE_H_

#include <cstdio>
#include <cstring>
#include <vector>
#include <cstdint>
#include <string>

namespace libmmd
{
	class File
	{
	public:
		using Offset = int64_t;

		File();
		~File();

		File(const File&) = delete;
		File& operator = (const File&) = delete;

		bool Open(const char* filepath);
		bool OpenText(const char* filepath);
		bool Create(const char* filepath);
		bool CreateText(const char* filepath);

		bool Open(const std::string& filepath) { return Open(filepath.c_str()); }
		bool OpenText(const std::string& filepath) { return OpenText(filepath.c_str()); }
		bool Create(const std::string& filepath) { return Create(filepath.c_str()); }
		bool CreateText(const std::string& filepath) { return CreateText(filepath.c_str()); }

		void Close();
		bool IsOpen() const;
		Offset GetSize() const;
		bool IsBad() const;
		void ClearBadFlag();
		bool IsEOF() const;

		FILE* GetFilePointer() const;

		bool ReadAll(std::vector<char>* buffer);
		bool ReadAll(std::vector<uint8_t>* buffer);
		bool ReadAll(std::vector<int8_t>* buffer);

		enum class SeekDir
		{
			Begin,
			Current,
			End,
		};
		bool Seek(Offset offset, SeekDir origin);
		Offset Tell();

		template <typename T>
		bool Read(T* buffer, size_t count = 1)
		{
			if (buffer == nullptr)
			{
				return false;
			}

			if (!IsOpen())
			{
				return false;
			}
#if _MSC_VER
			if (fread_s(buffer, sizeof(T) * count, sizeof(T), count, m_fp) != count)
			{
				m_badFlag = true;
				return false;
			}
#else // !_MSC_VER
			if (fread(buffer, sizeof(T), count, m_fp) != count)
			{
				m_badFlag = true;
				return false;
			}
#endif //!_MSC_VER
			return true;
		}

		template <typename T>
		bool Write(T* buffer, size_t count = 1)
		{
			if (buffer == nullptr)
			{
				return false;
			}

			if (!IsOpen())
			{
				return false;
			}

			if (fwrite(buffer, sizeof(T), count, m_fp) != count)
			{
				m_badFlag = true;
				return false;
			}
			return true;
		}

	private:
		bool OpenFile(const char* filepath, const char* mode);

		FILE*	m_fp;
		Offset	m_fileSize;
		bool	m_badFlag;
	};

	class MemoryReader
	{
	public:
		using Offset = int64_t;

		MemoryReader() : m_data(nullptr), m_size(0), m_pos(0), m_badFlag(false) {}
		MemoryReader(const uint8_t* data, size_t size)
			: m_data(data), m_size(size), m_pos(0), m_badFlag(false) {}

		MemoryReader(const MemoryReader&) = delete;
		MemoryReader& operator=(const MemoryReader&) = delete;

		MemoryReader(MemoryReader&&) noexcept = default;
		MemoryReader& operator=(MemoryReader&&) noexcept = default;

		bool IsBad() const { return m_badFlag; }
		void ClearBadFlag() { m_badFlag = false; }
		bool IsEOF() const { return m_pos >= m_size; }
		Offset GetSize() const { return static_cast<Offset>(m_size); }
		Offset Tell() const { return static_cast<Offset>(m_pos); }

		enum class SeekDir
		{
			Begin,
			Current,
			End,
		};

		bool Seek(Offset offset, SeekDir origin)
		{
			int64_t newPos;
			switch (origin)
			{
			case SeekDir::Begin:
				newPos = offset;
				break;
			case SeekDir::Current:
				newPos = static_cast<int64_t>(m_pos) + offset;
				break;
			case SeekDir::End:
				newPos = static_cast<int64_t>(m_size) + offset;
				break;
			default:
				return false;
			}
			if (newPos < 0 || static_cast<size_t>(newPos) > m_size)
			{
				m_badFlag = true;
				return false;
			}
			m_pos = static_cast<size_t>(newPos);
			return true;
		}

		template <typename T>
		bool Read(T* buffer, size_t count = 1)
		{
			if (buffer == nullptr)
			{
				return false;
			}
			const size_t bytes = sizeof(T) * count;
			if (m_pos + bytes > m_size)
			{
				m_badFlag = true;
				return false;
			}
			std::memcpy(buffer, m_data + m_pos, bytes);
			m_pos += bytes;
			return true;
		}

	private:
		const uint8_t*	m_data;
		size_t			m_size;
		size_t			m_pos;
		bool			m_badFlag;
	};

	class MemoryWriter
	{
	public:
		MemoryWriter() : m_badFlag(false) {}

		bool IsBad() const { return m_badFlag; }
		void ClearBadFlag() { m_badFlag = false; }

		std::vector<uint8_t>& GetData() { return m_data; }
		const std::vector<uint8_t>& GetData() const { return m_data; }

		template <typename T>
		bool Write(const T* buffer, size_t count = 1)
		{
			if (buffer == nullptr)
			{
				return false;
			}
			const size_t bytes = sizeof(T) * count;
			const auto* ptr = reinterpret_cast<const uint8_t*>(buffer);
			m_data.insert(m_data.end(), ptr, ptr + bytes);
			return true;
		}

	private:
		std::vector<uint8_t> m_data;
		bool m_badFlag;
	};

	class TextFileReader
	{
	public:
		TextFileReader() = default;
		explicit TextFileReader(const char* filepath);
		explicit TextFileReader(const std::string& filepath);

		bool Open(const char* filepath);
		bool Open(const std::string& filepath);
		void Close();
		bool IsOpen();

		std::string ReadLine();
		void ReadAllLines(std::vector<std::string>& lines);
		std::string ReadAll();
		bool IsEof();

	private:
		File	m_file;
	};
}

#endif // !BASE_FILE_H_
