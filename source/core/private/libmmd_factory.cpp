#include "pmx.h"
#include "vmd.h"
#include "vpd.h"

namespace libmmd
{
	pmx_model* pmx_model::create()
	{
		return new pmx_model_impl();
	}

	void pmx_model::free(pmx_model* model)
	{
		delete model;
	}

	vmd_animation* vmd_animation::create()
	{
		return new vmd_animation_impl();
	}

	void vmd_animation::free(vmd_animation* animation)
	{
		delete animation;
	}

	vpd_post* vpd_post::create()
	{
		return new vpd_post_impl();
	}

	void vpd_post::free(vpd_post* post)
	{
		delete post;
	}
}

