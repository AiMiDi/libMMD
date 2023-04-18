/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/10/2
File:			pmx_texture.cpp
Description:	pmx texture data

**************************************************************************/

#include "pmx_texture.h"

namespace libmmd
{
	bool PMXTexture::read_from_file(const file& file)
	{
		if (!model_info_->m_text_reader(file, m_data->path))
			return false;
		return true;
	}
}