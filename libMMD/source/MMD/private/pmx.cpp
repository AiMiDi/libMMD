/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/2
File:			pmx_data.cpp
Description:	pmx file data

**************************************************************************/

#include "pmx.h"

namespace libmmd
{
	PMX::PMX(std::u8string model_name_local, std::u8string model_name_universal) :
		m_data(std::make_unique<data_type>(std::move(model_name_local), std::move(model_name_universal))) {}

	bool PMX::load_from_file(const path& path)
	{
		file file;
		if(!path.check_suffix("pmx"))
			return false;

		if (!file.open(path, file::FILEOPEN::READ))
			return false;

		// read signature, value is "PMX "
		if (char signature[5]{ 0 }; !file.read_elements(signature, 4) && strncmp(signature, "PMX", 3) != 0)
		{
			ApplicationOutput("PMX::LoadFromFile() error at read signature.");
			return false;
		}

		// read model info
		if (!m_data->model_info.read_from_file(file))
		{
			ApplicationOutput("PMX::LoadFromFile() error at read model info");
			return false;
		}

		// read vertex data
		if (!m_data->vertex_data.read_from_file(file))
		{
			ApplicationOutput("PMX::LoadFromFile() error at read vertex data");
			return false;
		}

		// read surface data
		if (!m_data->surface_data.read_from_file(file))
		{
			ApplicationOutput("PMX::LoadFromFile() error at read surface data");
			return false;
		}

		// read texture data
		if (!m_data->texture_data.read_from_file(file))
		{
			ApplicationOutput("PMX::LoadFromFile() error at read texture data");
			return false;
		}

		// read material data
		if (!m_data->material_data.read_from_file(file))
		{
			ApplicationOutput("PMX::LoadFromFile() error at read material data");
			return false;
		}

		// read bone data
		if (!m_data->bone_data.read_from_file(file))
		{
			ApplicationOutput("PMX::LoadFromFile() error at read bone data");
			return false;
		}

		// read morph data
		if (!m_data->morph_data.read_from_file(file))
		{
			ApplicationOutput("PMX::LoadFromFile() error at read morph data");
			return false;
		}

		// read rigid body data
		if (!m_data->rigid_body_data.read_from_file(file))
		{
			ApplicationOutput("PMX::LoadFromFile() error at read rigid body data");
			return false;
		}

		// read joint data
		if (!m_data->joint_data.read_from_file(file))
		{
			ApplicationOutput("PMX::LoadFromFile() error at read joint data");
			return false;
		}

		return true;
	}
}