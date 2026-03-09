//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef LIBMMD_MODEL_MMD_MMDFILESTRING_H_
#define LIBMMD_MODEL_MMD_MMDFILESTRING_H_

#include <libMMD/Base/UnicodeUtil.h>
#include <libMMD/Base/File.h>

#include "SjisToUnicode.h"

#include <string>


namespace libmmd
{
	/**
	 * @brief Represents a string in MMD file.
	 * @tparam Size Size of the string buffer.
	 */
	template <size_t Size>
	struct MMDFileString
	{
		MMDFileString()
		{
			Clear();
		}

		/**
		 * @brief Clear the string buffer.
		 */
		void Clear()
		{
			for (auto& ch : m_buffer)
			{
				ch = '\0';
			}
		}

		/**
		 * @brief Set the string value.
		 * @param str Pointer to the string.
		 */
		void Set(const char* str)
		{
			size_t i = 0;
			while (i < Size && str[i] != '\0')
			{
				m_buffer[i] = str[i];
				i++;
			}

			for (; i < Size + 1; i++)
			{
				m_buffer[i] = '\0';
			}
		}

		/**
		 * @brief Convert to C string.
		 * @return Pointer to the C string.
		 */
		const char* ToCString() const { return m_buffer; }

		/**
		 * @brief Convert to std::string.
		 * @return std::string representation.
		 */
		std::string ToString() const { return std::string(m_buffer); }
		//std::wstring ToWString() const { return ConvertSjisToWString(m_buffer); }

		/**
		 * @brief Convert to UTF-8 string.
		 * @return UTF-8 string representation.
		 */
		std::string ToUtf8String() const;

		char	m_buffer[Size + 1]{}; ///< String buffer
	};

	/**
	 * @brief Read MMDFileString from file.
	 * @tparam Size Size of the string buffer.
	 * @param str Pointer to the MMDFileString.
	 * @param file Reference to the file.
	 * @return True if read successfully, false otherwise.
	 */
	template <size_t Size>
	bool Read(MMDFileString<Size>* str, File& file)
	{
		return file.Read(str->m_buffer, Size);
	}

	/**
	 * @brief Read MMDFileString from MemoryReader.
	 * @tparam Size Size of the string buffer.
	 * @param str Pointer to the MMDFileString.
	 * @param reader Reference to the MemoryReader.
	 * @return True if read successfully, false otherwise.
	 */
	template <size_t Size>
	bool Read(MMDFileString<Size>* str, MemoryReader& reader)
	{
		return reader.Read(str->m_buffer, Size);
	}

	/**
	 * @brief Write MMDFileString to file.
	 * @tparam Size Size of the string buffer.
	 * @param str Reference to the MMDFileString.
	 * @param file Reference to the file.
	 * @return True if written successfully, false otherwise.
	 */
	template <size_t Size>
	bool Write(const MMDFileString<Size>& str, File& file)
	{
		return file.Write(str.m_buffer, Size);
	}

	template <size_t Size>
	bool Write(const MMDFileString<Size>& str, MemoryWriter& writer)
	{
		return writer.Write(str.m_buffer, Size);
	}

	/**
	 * @brief Convert to UTF-8 string.
	 * @return UTF-8 string representation.
	 */
	template<size_t Size>
	std::string MMDFileString<Size>::ToUtf8String() const
	{
		const std::u16string u16Str = libmmd::ConvertSjisToU16String(m_buffer);
		std::string u8Str;
		ConvU16ToU8(u16Str, u8Str);
		return u8Str;
	}
}

#endif // !LIBMMD_MODEL_MMD_MMDFILESTRING_H_
