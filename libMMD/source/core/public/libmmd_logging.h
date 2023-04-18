/**************************************************************************

Copyright:Copyright(c) 2023-present, Aimidi & libmmd contributors.
Author:			Aimidi
Date:			2023/4/19
File:			libmmd_logging.h
Description:	logging util

**************************************************************************/
#pragma once

namespace libmmd
{
	class log
	{
		log() = default;
		~log() = default;
	public:
		void info(std::string str);
	};
}