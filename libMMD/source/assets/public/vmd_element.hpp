/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/17
File:			vmd_element.hpp
Description:	vmd element base class.

**************************************************************************/
#pragma once

#include "file_element_array.hpp"

namespace libmmd
{
	template<typename T>
	// T must have < and == operator
	concept is_vmd_element = requires(T a,T b)
	{
		a < b;
		a == b;
	};

	
}
