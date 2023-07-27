#include "pmx.h"
#include "vmd.h"
#include "vpd.h"

namespace libmmd
{
	pmx_model* create_pmx_model()
	{
		return new pmx_model_impl();
	}

	void delete_pmx_model(const pmx_model** model)
	{
		delete* model;
		*model = nullptr;
	}

	vmd_animation* create_vmd_animation()
	{
		return new vmd_animation_impl();
	}

	void delete_vmd_animation(const vmd_animation** animation)
	{
		delete* animation;
		*animation = nullptr;
	}


	vpd_post* create_vpd_post()
	{
		return new vpd_post_impl();
	}

	void delete_vpd_post(const vpd_post** post)
	{
		delete *post;
		*post = nullptr;
	}
}

