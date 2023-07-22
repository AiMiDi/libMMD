/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/9/24
File:			pmx_joint.h
Description:	pmx joint data

**************************************************************************/

#pragma once

#include "pmx_element.hpp"
#include "pmx_model_description.h"

namespace libmmd
{

	class pmx_joint_impl final : public pmx_element_impl, public pmx_joint
	{
		// Joint point local name 
		std::u8string	joint_name_local_{};
		// Common name of joint point      
		std::u8string	joint_name_universal_{};
		// Joint point type
		joint_type		joint_type_ = joint_type::SPRING_6DOF;
		// Rigid body index a
		Int32		rigid_body_index_a_ = 0;
		// Rigid body index b           
		Int32		rigid_body_index_b_ = 0;
		// Rigid body position           
		Vector32	position_{};
		// rigid rotation           
		Vector32	rotation_{};
		// Position min           
		Vector32	position_minimum_{};
		// Position max            
		Vector32	position_maximum_{};
		// Rotation min            
		Vector32	rotation_minimum_{};
		// Rotation max            
		Vector32	rotation_maximum_{};
		// Positioning spring            
		Vector32	position_spring_{};
		// Rotating spring           
		Vector32	rotation_spring_{};
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit pmx_joint_impl(const pmx_model_description* model_info) : pmx_element_impl(model_info) {}
		/**
		 * \brief Destructor function
		 */
		~pmx_joint_impl() override = default;
		/**
		 * \brief Copy constructor function
		 */
		pmx_joint_impl(const pmx_joint_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		pmx_joint_impl& operator =(const pmx_joint_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		pmx_joint_impl(pmx_joint_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		pmx_joint_impl& operator =(pmx_joint_impl&&) noexcept = default;

		std::string get_joint_name_local() const override;

		void set_joint_name_local(const std::string& name) override;

		std::string get_joint_name_universal() const override;

		void set_joint_name_universal(const std::string& name) override;

		joint_type get_joint_type() const override;

		void set_joint_type(joint_type type) override;

		int32_t get_rigid_body_index_a() const override;

		void set_rigid_body_index_a(int32_t index) override;

		int32_t get_rigid_body_index_b() const override;

		void set_rigid_body_index_b(int32_t index) override;

		const std::array<float, 3>& get_position() const override;

		void set_position(const std::array<float, 3>& position) override;

		const std::array<float, 3>& get_rotation() const override;

		void set_rotation(const std::array<float, 3>& rotation) override;

		const std::array<float, 3>& get_position_minimum() const override;

		void set_position_minimum(const std::array<float, 3>& minimum) override;

		const std::array<float, 3>& get_position_maximum() const override;

		void set_position_maximum(const std::array<float, 3>& maximum) override;

		const std::array<float, 3>& get_rotation_minimum() const override;

		void set_rotation_minimum(const std::array<float, 3>& minimum) override;

		const std::array<float, 3>& get_rotation_maximum() const override;

		void set_rotation_maximum(const std::array<float, 3>& maximum) override;

		const std::array<float, 3>& get_position_spring() const override;

		void set_position_spring(const std::array<float, 3>& spring) override;

		const std::array<float, 3>& get_rotation_spring() const override;

		void set_rotation_spring(const std::array<float, 3>& spring) override;

		/**
		 * \brief Read PMX joint from a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write PMX joint to a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const  override;
	};
}
