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
	bool pmx_texture_impl::read_from_file(const file& file)
	{
		if (!model_description_->read_text(file, path_))
			return false;
		return true;
	}

	bool pmx_texture_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return false;
	}
}
