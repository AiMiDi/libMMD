/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/8/24
File:			pmx_bone.h
Description:	pmx bone data

**************************************************************************/

#pragma once

#include "pmx_element_array.hpp"
#include "pmx_model_info.h"

namespace libmmd
{
	/* Bone flags(2bytes) */
	struct pmx_bone_flags
	{
		// The position of the tail (tip) of the bone, 0 is the relative position of the connection, and 1 is the connection sub bone 
		bool	indexed_tail_position : 1 = false;
		// Enable rotation 
		bool	rotatable : 1 = true;
		// Enable move 
		bool	translatable : 1 = false;
		// Enable display 
		bool	is_visible : 1 = true;
		// Enable action 
		bool	enabled : 1 = true;
		// Enable IK bone 
		bool	is_IK : 1 = false;
		// seize a seat 
		bool : 2 = false;
		// Turn on inherit bone rotation 
		bool	inherit_rotation : 1 = false;
		// Enable inherited bone movement 
		bool	inherit_translation : 1 = false;
		// Enable axis limit 
		bool	fixed_axis : 1 = false;
		// Enable local axis 
		bool	local_coordinate : 1 = false;
		// First deform, then calculate physics 
		bool	physics_after_deform : 1 = false;
		// External bony deformation 
		bool	external_parent_deform : 1 = false;
		// seize a seat 
		bool : 2 = false;
	};

	class pmx_IK_link_impl final : public pmx_element_impl, pmx_IK_link
	{
		// Bone index
		Int32		bone_index_{};
		// When the value is 1, the angle limit is used
		bool		have_limits_{};
		// IK angle limit minimum angle (radians)
		Vector32	limit_min_{};
		// IK angle limit maximum angle (radians)
		Vector32	limit_max_{};
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit pmx_IK_link_impl(const pmx_model_description* model_info) : pmx_element_impl(model_info) {}
		/**
		 * \brief Destructor function
		 */
		~pmx_IK_link_impl() override = default;

		pmx_IK_link_impl(const pmx_IK_link_impl&) = default;
		pmx_IK_link_impl& operator =(const pmx_IK_link_impl&) = default;
		pmx_IK_link_impl(pmx_IK_link_impl&&) noexcept = default;
		pmx_IK_link_impl& operator =(pmx_IK_link_impl&&) noexcept = default;

		[[nodiscard]] int32_t get_bone_index() const override;
		void set_bone_index(int32_t index) override;

		[[nodiscard]] bool have_limits() const override;
		void set_have_limits(bool value) override;

		[[nodiscard]] const std::array<float, 3>& get_limit_min() const override;
		void set_limit_min(const std::array<float, 3>& min) override;

		[[nodiscard]] const std::array<float, 3>& get_limit_max() const override;
		void set_limit_max(const std::array<float, 3>& max) override;

		/**
		 * \brief Read PMX IK link data from a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write PMX IK link data to a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		[[nodiscard]] bool write_to_file(const file& file) const override;
	};

	class pmx_bone_impl : public pmx_element_impl, pmx_bone
	{
		// The local name of the skeleton, usually Japanese. 
		std::u8string	bone_name_local_{};
		// The common name of bones is usually English. 
		std::u8string	bone_name_universal_{};
		// Bone position 
		Vector32		position_{};
		// Bone affinity index, special: the bone affinity of the operation center is - 1 
		Int32			parent_bone_index_{};
		// Deformation stratum 
		Int32			layer_{};
		// Bone flags 
		pmx_bone_flags	bone_flags_{};
		// Bone tail coordinates (valid when relative on) 
		Vector32		tail_position_{};
		// Bone tail index (valid when it is relatively unopened) 
		Int32			tail_index_{};
		/* 尾部位置,相对位置或连接子骨骼 */
		// Bone inheritance - bone affinity index 
		Int32			inherit_bone_parent_index_{};
		// Bone inheritance - influence weights 
		Float32			inherit_bone_parent_influence_{};
		// Bone fixed axis - axis direction 
		Vector32		bone_fixed_axis_{};
		// Bone local coordinates - x vector 
		Vector32		bone_local_x_{};
		// Bone local coordinates - Z vector 
		Vector32		bone_local_z_{};
		/* 跳过外亲骨(index) */
		// IK bone - target bone index 
		Int32			IK_target_index_{};
		// IK bone - cycle count 
		Int32			IK_loop_count_{};
		// IK bone - unit angle 
		Float32			IK_limit_radian_{};
		// IK bones
		pmx_element_array_impl<pmx_IK_link, pmx_IK_link_impl> IK_links_{model_description_};
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit pmx_bone_impl(const pmx_model_description* model_info) : pmx_element_impl(model_info) {}
		/**
		 * \brief Destructor function
		 */
		~pmx_bone_impl() override = default;

		pmx_bone_impl(const pmx_bone_impl&) = default;
		pmx_bone_impl& operator =(const pmx_bone_impl&) = default;
		pmx_bone_impl(pmx_bone_impl&&) noexcept = default;
		pmx_bone_impl& operator =(pmx_bone_impl&&) noexcept = default;

		[[nodiscard]] std::string get_bone_name_local() const override;
		void set_bone_name_local(const std::string& name) override;

		[[nodiscard]] std::string get_bone_name_universal() const override;
		void set_bone_name_universal(const std::string& name) override;

		[[nodiscard]] const std::array<float, 3>& get_position() const override;
		void set_position(const std::array<float, 3>& position) override;

		[[nodiscard]] int32_t get_parent_bone_index() const override;
		void set_parent_bone_index(int32_t index) override;

		[[nodiscard]] int32_t get_layer() const override;
		void set_layer(int32_t layer) override;

		[[nodiscard]] bool is_rotatable() const override;
		void set_is_rotatable(bool value) override;

		[[nodiscard]] bool is_translatable() const override;
		void set_is_translatable(bool value) override;

		[[nodiscard]] bool is_visible() const override;
		void set_is_visible(bool value) override;

		[[nodiscard]] bool is_enabled() const override;
		void set_is_enabled(bool value) override;

		[[nodiscard]] bool is_physics_after_deform() const override;
		void set_is_physics_after_deform(bool value) override;

		[[nodiscard]] bool have_external_parent_deform() const override;
		void set_have_external_parent_deform(bool value) override;

		[[nodiscard]] bool is_tail_use_index() const override;
		void set_is_tail_use_index(bool value) override;

		[[nodiscard]] const std::array<float, 3>& get_tail_position() const override;
		void set_tail_position(const std::array<float, 3>& position) override;

		[[nodiscard]] int32_t get_tail_index() const override;
		void set_tail_index(int32_t index) override;

		[[nodiscard]] bool have_inherit_rotation() const override;
		void set_have_inherit_rotation(bool value) override;

		[[nodiscard]] bool have_inherit_translation() const override;
		void set_have_inherit_translation(bool value) override;

		[[nodiscard]] int32_t get_inherit_bone_parent_index() const override;
		void set_inherit_bone_parent_index(int32_t index) override;

		[[nodiscard]] float get_inherit_bone_parent_influence() const override;
		void set_inherit_bone_parent_influence(float influence) override;

		[[nodiscard]] bool have_fixed_axis() const override;
		void set_have_fixed_axis(bool value) override;

		[[nodiscard]] const std::array<float, 3>& get_bone_fixed_axis() const override;
		void set_bone_fixed_axis(const std::array<float, 3>& axis) override;

		[[nodiscard]] bool have_local_coordinate() const override;
		void set_have_local_coordinate(bool value) override;

		[[nodiscard]] const std::array<float, 3>& get_bone_local_coordinate_x() const override;
		void set_bone_local_coordinate_x(const std::array<float, 3>& x) override;

		[[nodiscard]] const std::array<float, 3>& get_bone_local_coordinate_z() const override;
		void set_bone_local_coordinate_z(const std::array<float, 3>& z) override;

		[[nodiscard]] bool is_IK() const override;
		void set_is_IK(bool value) override;

		[[nodiscard]] int32_t get_IK_target_index() const override;
		void set_IK_target_index(int32_t index) override;

		[[nodiscard]] int32_t get_IK_loop_count() const override;
		void set_IK_loop_count(int32_t count) override;

		[[nodiscard]] float get_IK_limit_radian() const override;
		void set_IK_limit_radian(float radian) override;

		[[nodiscard]] const pmx_IK_link_array& get_IK_link_array() const override;
		pmx_IK_link_array& mutable_IK_link_array() override;

		/**
		 * \brief Read PMX bone data from a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write PMX IK data to a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const override;
	};
}
