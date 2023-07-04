#include "pmx.h"

static libmmd::pmx_model* create_pmx_model()
{
	return new libmmd::pmx_model_impl();
}