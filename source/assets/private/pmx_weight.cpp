/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/8/9
File:			pmx_weight.cpp
Description:	pmx weight data

**************************************************************************/

#include "pmx_weight.h"

namespace libmmd
{
	int32_t pmx_vertex_skinning_BDEF1_impl::get_bone_index() const
	{
		return bone_index_;
	}

	void pmx_vertex_skinning_BDEF1_impl::set_bone_index(const int32_t bone_index)
	{
		bone_index_ = bone_index;
	}

	int32_t pmx_vertex_skinning_BDEF2_impl::get_bone_index1() const
	{
		return bone_index_[0];
	}

	void pmx_vertex_skinning_BDEF2_impl::set_bone_index1(const int32_t bone_index)
	{
		bone_index_[0] = bone_index;
	}

	int32_t pmx_vertex_skinning_BDEF2_impl::get_bone_index2() const
	{
		return bone_index_[1];
	}

	void pmx_vertex_skinning_BDEF2_impl::set_bone_index2(const int32_t bone_index)
	{
		bone_index_[1] = bone_index;
	}

	float pmx_vertex_skinning_BDEF2_impl::get_bone_weight() const
	{
		return bone_weight_;
	}

	void pmx_vertex_skinning_BDEF2_impl::set_bone_weight(const float bone_weight)
	{
		bone_weight_ = bone_weight;
	}

	int32_t pmx_vertex_skinning_BDEF4_impl::get_bone_index1() const
	{
		return bone_index_[0];
	}

	void pmx_vertex_skinning_BDEF4_impl::set_bone_index1(const int32_t bone_index)
	{
		bone_index_[0] = bone_index;
	}

	int32_t pmx_vertex_skinning_BDEF4_impl::get_bone_index2() const
	{
		return bone_index_[1];
	}

	void pmx_vertex_skinning_BDEF4_impl::set_bone_index2(const int32_t bone_index)
	{
		bone_index_[1] = bone_index;
	}

	int32_t pmx_vertex_skinning_BDEF4_impl::get_bone_index3() const
	{
		return bone_index_[2];
	}

	void pmx_vertex_skinning_BDEF4_impl::set_bone_index3(const int32_t bone_index)
	{
		bone_index_[2] = bone_index;
	}

	int32_t pmx_vertex_skinning_BDEF4_impl::get_bone_index4() const
	{
		return bone_index_[3];
	}

	void pmx_vertex_skinning_BDEF4_impl::set_bone_index4(const int32_t bone_index)
	{
		bone_index_[3] = bone_index;
	}

	float pmx_vertex_skinning_BDEF4_impl::get_bone_weight1() const
	{
		return bone_weight_[0];
	}

	void pmx_vertex_skinning_BDEF4_impl::set_bone_weight1(const float bone_weight)
	{
		bone_weight_[0] = bone_weight;
	}

	float pmx_vertex_skinning_BDEF4_impl::get_bone_weight2() const
	{
		return bone_weight_[1];
	}

	void pmx_vertex_skinning_BDEF4_impl::set_bone_weight2(const float bone_weight)
	{
		bone_weight_[1] = bone_weight;
	}

	float pmx_vertex_skinning_BDEF4_impl::get_bone_weight3() const
	{
		return bone_weight_[2];
	}

	void pmx_vertex_skinning_BDEF4_impl::set_bone_weight3(const float bone_weight)
	{
		bone_weight_[2] = bone_weight;
	}

	float pmx_vertex_skinning_BDEF4_impl::get_bone_weight4() const
	{
		return bone_weight_[3];
	}

	void pmx_vertex_skinning_BDEF4_impl::set_bone_weight4(const float bone_weight)
	{
		bone_weight_[3] = bone_weight;
	}

	void pmx_vertex_skinning_BDEF4_impl::normalization()
	{
		const float weight = bone_weight_[0] + bone_weight_[1] + bone_weight_[2] + bone_weight_[3];
		bone_weight_[0] /= weight;
		bone_weight_[1] /= weight;
		bone_weight_[2] /= weight;
		bone_weight_[3] /= weight;
	}

	int32_t pmx_vertex_skinning_SDEF_impl::get_bone_index1() const
	{
		return bone_index_[0];
	}

	void pmx_vertex_skinning_SDEF_impl::set_bone_index1(const int32_t bone_index)
	{
		bone_index_[0] = bone_index;
	}

	int32_t pmx_vertex_skinning_SDEF_impl::get_bone_index2() const
	{
		return bone_index_[1];
	}

	void pmx_vertex_skinning_SDEF_impl::set_bone_index2(const int32_t bone_index)
	{
		bone_index_[1] = bone_index;
	}

	float pmx_vertex_skinning_SDEF_impl::get_bone_weight() const
	{
		return bone_weight_;
	}

	void pmx_vertex_skinning_SDEF_impl::set_bone_weight(const float bone_weight)
	{
		bone_weight_ = bone_weight;
	}

	const std::array<float, 3>& pmx_vertex_skinning_SDEF_impl::get_sdef_c() const
	{
		return c_;
	}

	void pmx_vertex_skinning_SDEF_impl::set_sdef_c(const std::array<float, 3>& sdef_c)
	{
		c_ = sdef_c;
	}

	const std::array<float, 3>& pmx_vertex_skinning_SDEF_impl::get_sdef_r0() const
	{
		return r0_;
	}

	void pmx_vertex_skinning_SDEF_impl::set_sdef_r0(const std::array<float, 3>& sdef_r0)
	{
		r0_ = sdef_r0;
	}

	const std::array<float, 3>& pmx_vertex_skinning_SDEF_impl::get_sdef_r1() const
	{
		return r1_;
	}

	void pmx_vertex_skinning_SDEF_impl::set_sdef_r1(const std::array<float, 3>& sdef_r1)
	{
		r1_ = sdef_r1;
	}

	int32_t pmx_vertex_skinning_QDEF_impl::get_bone_index1() const
	{
		return bone_index_[0];
	}

	void pmx_vertex_skinning_QDEF_impl::set_bone_index1(const int32_t bone_index)
	{
		bone_index_[0] = bone_index;
	}

	int32_t pmx_vertex_skinning_QDEF_impl::get_bone_index2() const
	{
		return bone_index_[1];
	}

	void pmx_vertex_skinning_QDEF_impl::set_bone_index2(const int32_t bone_index)
	{
		bone_index_[1] = bone_index;
	}

	int32_t pmx_vertex_skinning_QDEF_impl::get_bone_index3() const
	{
		return bone_index_[2];
	}

	void pmx_vertex_skinning_QDEF_impl::set_bone_index3(const int32_t bone_index)
	{
		bone_index_[2] = bone_index;
	}

	int32_t pmx_vertex_skinning_QDEF_impl::get_bone_index4() const
	{
		return bone_index_[3];
	}

	void pmx_vertex_skinning_QDEF_impl::set_bone_index4(const int32_t bone_index)
	{
		bone_index_[3] = bone_index;
	}

	float pmx_vertex_skinning_QDEF_impl::get_bone_weight1() const
	{
		return bone_weight_[0];
	}

	void pmx_vertex_skinning_QDEF_impl::set_bone_weight1(const float bone_weight)
	{
		bone_weight_[0] = bone_weight;
	}

	float pmx_vertex_skinning_QDEF_impl::get_bone_weight2() const
	{
		return bone_weight_[1];
	}

	void pmx_vertex_skinning_QDEF_impl::set_bone_weight2(const float bone_weight)
	{
		bone_weight_[1] = bone_weight;
	}

	float pmx_vertex_skinning_QDEF_impl::get_bone_weight3() const
	{
		return bone_weight_[2];
	}

	void pmx_vertex_skinning_QDEF_impl::set_bone_weight3(const float bone_weight)
	{
		bone_weight_[2] = bone_weight;
	}

	float pmx_vertex_skinning_QDEF_impl::get_bone_weight4() const
	{
		return bone_weight_[3];
	}

	void pmx_vertex_skinning_QDEF_impl::set_bone_weight4(const float bone_weight)
	{
		bone_weight_[3] = bone_weight;
	}

	void pmx_vertex_skinning_QDEF_impl::normalization()
	{
		const float weight = bone_weight_[0] + bone_weight_[1] + bone_weight_[2] + bone_weight_[3];
		bone_weight_[0] /= weight;
		bone_weight_[1] /= weight;
		bone_weight_[2] /= weight;
		bone_weight_[3] /= weight;
	}

	 bool pmx_vertex_skinning_BDEF1_impl::read_from_file(const file& file)
	{
		if (!(*bone_index_reader_)(file, bone_index_))
			return false;
		return true;
	}

	bool pmx_vertex_skinning_BDEF1_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return true;
	}

	 bool pmx_vertex_skinning_BDEF2_impl::read_from_file(const file& file)
	{
		if (!(*bone_index_reader_)(file, bone_index_[0]))
			return false;
		if (!(*bone_index_reader_)(file, bone_index_[1]))
			return false;
		if (!file.read_element(bone_weight_))
			return false;
		return true;
	}

	bool pmx_vertex_skinning_BDEF2_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return true;
	}

	 bool pmx_vertex_skinning_BDEF4_impl::read_from_file(const file& file)
	{
		if (!(*bone_index_reader_)(file, bone_index_[0]))
			return false;
		if (!(*bone_index_reader_)(file, bone_index_[1]))
			return false;
		if (!(*bone_index_reader_)(file, bone_index_[2]))
			return false;
		if (!(*bone_index_reader_)(file, bone_index_[3]))
			return false;
		if (!file.read_element(bone_weight_[0]))
			return false;
		if (!file.read_element(bone_weight_[1]))
			return false;
		if (!file.read_element(bone_weight_[2]))
			return false;
		if (!file.read_element(bone_weight_[3]))
			return false;
		return true;
	}

	bool pmx_vertex_skinning_BDEF4_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return true;
	}

	 bool pmx_vertex_skinning_SDEF_impl::read_from_file(const file& file)
	{
		if (!(*bone_index_reader_)(file, bone_index_[0]))
			return false;
		if (!(*bone_index_reader_)(file, bone_index_[1]))
			return false;
		if (!file.read_element(bone_weight_))
			return false;
		if (!file.read_element(r0_))
			return false;
		if (!file.read_element(r1_))
			return false;
		if (!file.read_element(c_))
			return false;
		return true;
	}

	bool pmx_vertex_skinning_SDEF_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return true;
	}

	 bool pmx_vertex_skinning_QDEF_impl::read_from_file(const file& file)
	{
		if (!(*bone_index_reader_)(file, bone_index_[0]))
			return false;
		if (!(*bone_index_reader_)(file, bone_index_[1]))
			return false;
		if (!(*bone_index_reader_)(file, bone_index_[2]))
			return false;
		if (!(*bone_index_reader_)(file, bone_index_[3]))
			return false;
		if (!file.read_element(bone_weight_[0]))
			return false;
		if (!file.read_element(bone_weight_[1]))
			return false;
		if (!file.read_element(bone_weight_[2]))
			return false;
		if (!file.read_element(bone_weight_[3]))
			return false;
		return true;
	}

	bool pmx_vertex_skinning_QDEF_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return true;
	}
}
