#include "pmx.h"
#include "vmd.h"
#include "vpd.h"

namespace libmmd
{
	pmx_model* create_pmx_model()
	{
		return new pmx_model_impl();
	}

	vmd_animation* create_vmd_animation()
	{
		return new vmd_animation_impl();
	}

	vmd_post* create_vmd_post()
	{
		return new vpd_post_impl();
	}
}

