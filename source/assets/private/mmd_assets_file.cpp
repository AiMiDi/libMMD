#include "mmd_assets_file.h"

bool libmmd::mmd_assets_file_impl::read_from_file_u8(const std::string& file_name)
{
	const path path{ std::u8string_view{ reinterpret_cast<const char8_t*>(file_name.data())} };
	return read_from_file_impl(path);
}

bool libmmd::mmd_assets_file_impl::write_to_file_u8(const std::string& file_name) const
{
	const path path{ std::u8string_view{ reinterpret_cast<const char8_t*>(file_name.data())} };
	return write_to_file_impl(path);
}

bool libmmd::mmd_assets_file_impl::read_from_file(const std::string& file_name)
{
	const path path{ file_name };
	return read_from_file_impl(path);
}

bool libmmd::mmd_assets_file_impl::write_to_file(const std::string& file_name) const
{
	const path path{ file_name };
	return write_to_file_impl(path);
}

bool libmmd::mmd_assets_file_impl::read_from_file(const std::wstring& file_name)
{
	const path path{ file_name };
	return read_from_file_impl(path);
}

bool libmmd::mmd_assets_file_impl::write_to_file(const std::wstring& file_name) const
{
	const path path{ file_name };
	return write_to_file_impl(path);
}

#ifdef __cpp_lib_string_view
bool libmmd::mmd_assets_file_impl::read_from_file_u8(const std::string_view& file_name)
{
	const path path{ std::u8string_view{ reinterpret_cast<const char8_t*>(file_name.data())} };
	return read_from_file_impl(path);
}

bool libmmd::mmd_assets_file_impl::write_to_file_u8(const std::string_view& file_name) const
{
	const path path{ std::u8string_view{ reinterpret_cast<const char8_t*>(file_name.data())} };
	return write_to_file_impl(path);
}

bool libmmd::mmd_assets_file_impl::read_from_file(const std::string_view& file_name)
{
	const path path{ file_name };
	return read_from_file_impl(path);
}

bool libmmd::mmd_assets_file_impl::write_to_file(const std::string_view& file_name) const
{
	const path path{ file_name };
	return write_to_file_impl(path);
}

bool libmmd::mmd_assets_file_impl::read_from_file(const std::wstring_view& file_name)
{
	const path path{ file_name };
	return read_from_file_impl(path);
}

bool libmmd::mmd_assets_file_impl::write_to_file(const std::wstring_view& file_name) const
{
	const path path{ file_name };
	return write_to_file_impl(path);
}
#endif

