/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/9/24
File:			pmx_rigid_body.h
Description:	pmx rigid body data

**************************************************************************/

#pragma once

#include "pmx_element.hpp"
#include "pmx_model_description.h"

namespace libmmd
{
	struct pmx_rigid_body_non_collision_group_data
	{
		bool	G0 : 1 = false;
		bool	G1 : 1 = false;
		bool	G2 : 1 = false;
		bool	G3 : 1 = false;
		bool	G4 : 1 = false;
		bool	G5 : 1 = false;
		bool	G6 : 1 = false;
		bool	G7 : 1 = false;
		bool	G8 : 1 = false;
		bool	G9 : 1 = false;
		bool	G10 : 1 = false;
		bool	G11 : 1 = false;
		bool	G12 : 1 = false;
		bool	G13 : 1 = false;
		bool	G14 : 1 = false;
		bool	G15 : 1 = false;
	};

	static_assert(sizeof pmx_rigid_body_non_collision_group_data == 2);

	class pmx_rigid_body_non_collision_group_impl final : public pmx_rigid_body_non_collision_group, file_element_impl
	{
		pmx_rigid_body_non_collision_group_data non_collision_group_;
	public:
		/**
		 * \brief Default constructor function
		 */
		pmx_rigid_body_non_collision_group_impl() = default;
		/**
		 * \brief Destructor function
		 */
		~pmx_rigid_body_non_collision_group_impl() override = default;
		/**
		 * \brief Copy constructor function
		 */
		pmx_rigid_body_non_collision_group_impl(const pmx_rigid_body_non_collision_group_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		pmx_rigid_body_non_collision_group_impl& operator =(const pmx_rigid_body_non_collision_group_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		pmx_rigid_body_non_collision_group_impl(pmx_rigid_body_non_collision_group_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		pmx_rigid_body_non_collision_group_impl& operator =(pmx_rigid_body_non_collision_group_impl&&) noexcept = default;

		bool in_group_0() const override;

		void set_in_group_0(bool value) override;

		bool in_group_1() const override;

		void set_in_group_1(bool value) override;

		bool in_group_2() const override;

		void set_in_group_2(bool value) override;

		bool in_group_3() const override;

		void set_in_group_3(bool value) override;

		bool in_group_4() const override;

		void set_in_group_4(bool value) override;

		bool in_group_5() const override;

		void set_in_group_5(bool value) override;

		bool in_group_6() const override;

		void set_in_group_6(bool value) override;

		bool in_group_7() const override;

		void set_in_group_7(bool value) override;

		bool in_group_8() const override;

		void set_in_group_8(bool value) override;

		bool in_group_9() const override;

		void set_in_group_9(bool value) override;

		bool in_group_10() const override;

		void set_in_group_10(bool value) override;

		bool in_group_11() const override;

		void set_in_group_11(bool value) override;

		bool in_group_12() const override;

		void set_in_group_12(bool value) override;

		bool in_group_13() const override;

		void set_in_group_13(bool value) override;

		bool in_group_14() const override;

		void set_in_group_14(bool value) override;

		bool in_group_15() const override;

		void set_in_group_15(bool value) override;

		bool& operator[](uint8_t group_index) override;

		/**
		 * \brief Read PMX rigid body non collision group from a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write PMX rigid body non collision group to a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		auto write_to_file(const file& file) const -> bool override;
	};

	class pmx_rigid_body_impl final : public pmx_rigid_body, pmx_element_impl
	{
		// Rigid body local name 									
		std::u8string					rigid_body_name_local_{};
		// Rigid body common name 
		std::u8string					rigid_body_name_universal_{};
		// Associative bone index 
		Int32					related_bone_index_ = 0;
		// Group ID 
		Char					group_id_ = 0;
		// Collision exclusion group 
		pmx_rigid_body_non_collision_group_impl	non_collision_group_{};
		// Rigid body shape type
		shape_type shape_type_ = shape_type::SPHERE;
		// Shape size 
		Vector32	shape_size_{};
		// Shape position   
		Vector32	shape_position_{};
		// Shape rotation   
		Vector32	shape_rotation_{};
		// mass   
		Float32		mass_ = 0.f;
		// Movement attenuation   
		Float32		move_attenuation_ = 0.f;
		// Rotation attenuation   
		Float32		rotation_damping_ = 0.f;
		// Reaction force   
		Float32		repulsion_force_ = 0.f;
		// friction force   
		Float32		friction_force_ = 0.f;
		// Physical mode
		physics_mode physics_mode_ = physics_mode::TRACKING_BONES;
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit pmx_rigid_body_impl(const pmx_model_description* model_info) : pmx_element_impl(model_info) {}
		/**
		 * \brief Destructor function
		 */
		~pmx_rigid_body_impl() override = default;
		/**
		 * \brief Copy constructor function
		 */
		pmx_rigid_body_impl(const pmx_rigid_body_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		pmx_rigid_body_impl& operator =(const pmx_rigid_body_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		pmx_rigid_body_impl(pmx_rigid_body_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		pmx_rigid_body_impl& operator =(pmx_rigid_body_impl&&) noexcept = default;

		std::string get_rigid_body_name_local() const override;

		void set_rigid_body_name_local(const std::string& name) override;

		std::string get_rigid_body_name_universal() const override;

		void set_rigid_body_name_universal(const std::string& name) override;

		int32_t get_related_bone_index() const override;

		void set_related_bone_index(int32_t index) override;

		int8_t get_group_id() const override;

		void set_group_id(int8_t id) override;

		const pmx_rigid_body_non_collision_group& get_non_collision_group() const override;

		pmx_rigid_body_non_collision_group& mutable_non_collision_group() override;

		shape_type get_shape_type() const override;

		void set_shape_type(shape_type type) override;

		const std::array<float, 3>& get_shape_size() const override;

		void set_shape_size(const std::array<float, 3>& size) override;

		const std::array<float, 3>& get_shape_position() const override;

		void set_shape_position(const std::array<float, 3>& position) override;

		const std::array<float, 3>& get_shape_rotation() const override;

		void set_shape_rotation(const std::array<float, 3>& rotation) override;

		float get_mass() const override;

		void set_mass(float mass) override;

		float get_move_attenuation() const override;

		void set_move_attenuation(float attenuation) override;

		float get_rotation_damping() const override;

		void set_rotation_damping(float damping) override;

		float get_repulsion_force() const override;

		void set_repulsion_force(float force) override;

		float get_friction_force() const override;

		void set_friction_force(float force) override;

		physics_mode get_physics_mode() const override;

		void set_physics_mode(physics_mode mode) override;

		/**
		 * \brief Read PMX rigid body from a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write PMX rigid body to a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const  override;
	};
}
