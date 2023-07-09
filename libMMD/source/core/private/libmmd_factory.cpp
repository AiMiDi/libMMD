#include "pmx.h"
#include "vmd.h"

namespace libmmd
{
	static pmx_model* create_pmx_model()
	{
		return new pmx_model_impl();
	}

	static vmd_animation* create_vmd_animation()
	{
		return new vmd_animation_impl();
	}
}

