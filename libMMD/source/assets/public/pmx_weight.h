/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/8/9
File:			pmx_weight.h
Description:	pmx weight data

**************************************************************************/

#pragma once

#include "pmx_model_description.h"

namespace libmmd
{
	class pmx_vertex_skinning_BDEF1_impl final : public pmx_vertex_skinning_BDEF1
	{
		const pmx_index_reader* bone_index_reader_;

		// Bone index. 
		Int32 bone_index_ = 0;
	public:
		pmx_vertex_skinning_BDEF1_impl(const pmx_index_reader* bone_index_reader) : bone_index_reader_(bone_index_reader){}
		~pmx_vertex_skinning_BDEF1_impl() override = default;

		pmx_vertex_skinning_BDEF1_impl(const pmx_vertex_skinning_BDEF1_impl&) = default;
		pmx_vertex_skinning_BDEF1_impl& operator =(const pmx_vertex_skinning_BDEF1_impl&) = default;
		pmx_vertex_skinning_BDEF1_impl(pmx_vertex_skinning_BDEF1_impl&&) noexcept = default;
		pmx_vertex_skinning_BDEF1_impl& operator =(pmx_vertex_skinning_BDEF1_impl&&) noexcept = default;

		int32_t get_bone_index() override;
		void set_bone_index(int32_t bone_index) override;

		bool read_from_file(const file& file);
		bool write_to_file(const file& file) const;
	};

	class pmx_vertex_skinning_BDEF2_impl : public pmx_vertex_skinning_BDEF2
	{
		const pmx_index_reader* bone_index_reader_;

		// Bone index
		Int32	bone_index_[2] = { 0 };
		// Bone 1 weight
		Float32 bone_weight_ = 0.f;
	public:
		pmx_vertex_skinning_BDEF2_impl(const pmx_index_reader* bone_index_reader) : bone_index_reader_(bone_index_reader) {}
		~pmx_vertex_skinning_BDEF2_impl() override = default;

		pmx_vertex_skinning_BDEF2_impl(const pmx_vertex_skinning_BDEF2_impl&) = default;
		pmx_vertex_skinning_BDEF2_impl& operator =(const pmx_vertex_skinning_BDEF2_impl&) = default;
		pmx_vertex_skinning_BDEF2_impl(pmx_vertex_skinning_BDEF2_impl&&) noexcept = default;
		pmx_vertex_skinning_BDEF2_impl& operator =(pmx_vertex_skinning_BDEF2_impl&&) noexcept = default;

		int32_t get_bone_index1() override;
		void set_bone_index1(int32_t bone_index) override;
		int32_t get_bone_index2() override;
		void set_bone_index2(int32_t bone_index) override;
		float get_bone_weight() override;
		void set_bone_weight(float bone_weight) override;

		bool read_from_file(const file& file);
		bool write_to_file(const file& file) const;
	};

	class pmx_vertex_skinning_BDEF4_impl : public pmx_vertex_skinning_BDEF4
	{
		const pmx_index_reader* bone_index_reader_;

		// Bone index
		Int32	bone_index_[4] = { 0 };
		// Bone 1~4 weight
		Float32 bone_weight_[4] = { 0.f };
	public:
		pmx_vertex_skinning_BDEF4_impl(const pmx_index_reader* bone_index_reader) : bone_index_reader_(bone_index_reader) {}
		~pmx_vertex_skinning_BDEF4_impl() override = default;

		pmx_vertex_skinning_BDEF4_impl(const pmx_vertex_skinning_BDEF4_impl&) = default;
		pmx_vertex_skinning_BDEF4_impl& operator =(const pmx_vertex_skinning_BDEF4_impl&) = default;
		pmx_vertex_skinning_BDEF4_impl(pmx_vertex_skinning_BDEF4_impl&&) noexcept = default;
		pmx_vertex_skinning_BDEF4_impl& operator =(pmx_vertex_skinning_BDEF4_impl&&) noexcept = default;

		int32_t get_bone_index1() override;
		void set_bone_index1(int32_t bone_index) override;
		int32_t get_bone_index2() override;
		void set_bone_index2(int32_t bone_index) override;
		int32_t get_bone_index3() override;
		void set_bone_index3(int32_t bone_index) override;
		int32_t get_bone_index4() override;
		void set_bone_index4(int32_t bone_index) override;
		float get_bone_weight1() override;
		void set_bone_weight1(float bone_weight) override;
		float get_bone_weight2() override;
		void set_bone_weight2(float bone_weight) override;
		float get_bone_weight3() override;
		void set_bone_weight3(float bone_weight) override;
		float get_bone_weight4() override;
		void set_bone_weight4(float bone_weight) override;

		void normalization() override;

		bool read_from_file(const file& file);
		bool write_to_file(const file& file) const;
	};

	class pmx_vertex_skinning_SDEF_impl : public pmx_vertex_skinning_SDEF
	{
		const pmx_index_reader* bone_index_reader_;

		// Bone index
		Int32		bone_index_[2] = { 0 };
		// Bone 1 weight
		Float32		bone_weight_ = 0.f;
		// R0,R1,C
		Vector32	r0_{}, r1_{}, c_{};
	public:
		pmx_vertex_skinning_SDEF_impl(const pmx_index_reader* bone_index_reader) : bone_index_reader_(bone_index_reader) {}
		~pmx_vertex_skinning_SDEF_impl() override = default;

		pmx_vertex_skinning_SDEF_impl(const pmx_vertex_skinning_SDEF_impl&) = default;
		pmx_vertex_skinning_SDEF_impl& operator =(const pmx_vertex_skinning_SDEF_impl&) = default;
		pmx_vertex_skinning_SDEF_impl(pmx_vertex_skinning_SDEF_impl&&) noexcept = default;
		pmx_vertex_skinning_SDEF_impl& operator =(pmx_vertex_skinning_SDEF_impl&&) noexcept = default;

		int32_t get_bone_index1() override;
		void set_bone_index1(int32_t bone_index) override;
		int32_t get_bone_index2() override;
		void set_bone_index2(int32_t bone_index) override;
		float get_bone_weight() override;
		void set_bone_weight(float bone_weight) override;
		const std::array<float, 3>& get_sdef_c() const override;
		void set_sdef_c(const std::array<float, 3>& sdef_c) override;
		const std::array<float, 3>& get_sdef_r0() const override;
		void set_sdef_r0(const std::array<float, 3>& sdef_r0) override;
		const std::array<float, 3>& get_sdef_r1() const override;
		void set_sdef_r1(const std::array<float, 3>& sdef_r1) override;

		bool read_from_file(const file& file);
		bool write_to_file(const file& file) const;
	};

	class pmx_vertex_skinning_QDEF_impl : public pmx_vertex_skinning_QDEF
	{
		const pmx_index_reader* bone_index_reader_;

		// Bone index
		Int32	bone_index_[4] = { 0 };
		// Bone 1~4 weight
		Float32 bone_weight_[4] = { 0.f };
	public:
		pmx_vertex_skinning_QDEF_impl(const pmx_index_reader* bone_index_reader) : bone_index_reader_(bone_index_reader) {}
		~pmx_vertex_skinning_QDEF_impl() override = default;

		pmx_vertex_skinning_QDEF_impl(const pmx_vertex_skinning_QDEF_impl&) = default;
		pmx_vertex_skinning_QDEF_impl& operator =(const pmx_vertex_skinning_QDEF_impl&) = default;
		pmx_vertex_skinning_QDEF_impl(pmx_vertex_skinning_QDEF_impl&&) noexcept = default;
		pmx_vertex_skinning_QDEF_impl& operator =(pmx_vertex_skinning_QDEF_impl&&) noexcept = default;

		int32_t get_bone_index1() override;
		void set_bone_index1(int32_t bone_index) override;
		int32_t get_bone_index2() override;
		void set_bone_index2(int32_t bone_index) override;
		int32_t get_bone_index3() override;
		void set_bone_index3(int32_t bone_index) override;
		int32_t get_bone_index4() override;
		void set_bone_index4(int32_t bone_index) override;
		float get_bone_weight1() override;
		void set_bone_weight1(float bone_weight) override;
		float get_bone_weight2() override;
		void set_bone_weight2(float bone_weight) override;
		float get_bone_weight3() override;
		void set_bone_weight3(float bone_weight) override;
		float get_bone_weight4() override;
		void set_bone_weight4(float bone_weight) override;

		void normalization() override;

		bool read_from_file(const file& file);
		bool write_to_file(const file& file) const;
	};
}
