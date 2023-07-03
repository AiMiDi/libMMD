/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/9/18
File:			pmx_morph.h
Description:	pmx morph data

**************************************************************************/

#pragma once

#include "pmx_model_info.h"
#include "pmx_element_array.hpp"

namespace libmmd
{
	class pmx_group_morph_offset_impl final : public pmx_group_morph_offset, pmx_element_impl
	{
		// Deformation index
		Int32	morph_index_ = 0;
		// Influence
		Float32 morph_weight_ = 0.f;
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit pmx_group_morph_offset_impl(const pmx_model_description* model_info) : pmx_element_impl(model_info) {}
		/**
		 * \brief Destructor function
		 */
		~pmx_group_morph_offset_impl() override = default;
		/**
		 * \brief Copy constructor function
		 */
		pmx_group_morph_offset_impl(const pmx_group_morph_offset_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		pmx_group_morph_offset_impl& operator =(const pmx_group_morph_offset_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		pmx_group_morph_offset_impl(pmx_group_morph_offset_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		pmx_group_morph_offset_impl& operator =(pmx_group_morph_offset_impl&&) noexcept = default;

		int32_t get_morph_index() const override;

		void set_morph_index(int32_t morph_index) override;

		[[nodiscard]] float get_morph_weight() const override;

		void set_morph_weight(float morph_weight) override;

		/**
		 * \brief Read PMX morph offset data from a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write PMX morph offset data to a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const override;
	};

	class pmx_vertex_morph_offset_impl final : public pmx_vertex_morph_offset, pmx_element_impl
	{
		// Vertex index
		UInt32		vertex_index_ = 0;
		// Vertex position offset
		Vector32	offset_position_{};
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit pmx_vertex_morph_offset_impl(const pmx_model_description* model_info) : pmx_element_impl(model_info) {}
		/**
		 * \brief Destructor function
		 */
		~pmx_vertex_morph_offset_impl() override = default;
		/**
		 * \brief Copy constructor function
		 */
		pmx_vertex_morph_offset_impl(const pmx_vertex_morph_offset_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		pmx_vertex_morph_offset_impl& operator =(const pmx_vertex_morph_offset_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		pmx_vertex_morph_offset_impl(pmx_vertex_morph_offset_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		pmx_vertex_morph_offset_impl& operator =(pmx_vertex_morph_offset_impl&&) noexcept = default;

		uint32_t get_vertex_index() const override;

		void set_vertex_index(uint32_t vertex_index) override;

		const std::array<float, 3>& get_offset_position() const override;

		void set_offset_position(const std::array<float, 3>& offset_position) override;

		/**
		 * \brief Read PMX morph offset data from a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write PMX morph offset data to a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const override;
	};

	class pmx_bone_morph_offset_impl final : public pmx_bone_morph_offset, pmx_element_impl
	{
		// Bone index
		Int32		bone_index_ = 0;
		// Relative position of change
		Vector32	bone_translation_{};
		// Relative rotation quaternion (revolution Euler)
		Vector32	bone_rotation_{};
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit pmx_bone_morph_offset_impl(const pmx_model_description* model_info) : pmx_element_impl(model_info) {}
		/**
		 * \brief Destructor function
		 */
		~pmx_bone_morph_offset_impl() override = default;
		/**
		 * \brief Copy constructor function
		 */
		pmx_bone_morph_offset_impl(const pmx_bone_morph_offset_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		pmx_bone_morph_offset_impl& operator =(const pmx_bone_morph_offset_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		pmx_bone_morph_offset_impl(pmx_bone_morph_offset_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		pmx_bone_morph_offset_impl& operator =(pmx_bone_morph_offset_impl&&) noexcept = default;

		[[nodiscard]] int32_t get_bone_index() const override;
		void set_bone_index(int32_t bone_index) override;

		[[nodiscard]] const std::array<float, 3>& get_bone_translation() const override;
		void set_bone_translation(const std::array<float, 3>& bone_translation) override;

		[[nodiscard]] const std::array<float, 3>& get_bone_rotation() const override;
		void set_bone_rotation(const std::array<float, 3>& bone_rotation) override;

		/**
		 * \brief Read PMX morph offset data from a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write PMX morph offset data to a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const override;
	};

	class pmx_uv_morph_offset_impl final : public pmx_uv_morph_offset, pmx_element_impl
	{
		// Vertex index
		UInt32		vertex_index_ = 0;
		// Influence (only x and y are useful, Z and W are 0)
		Vector4d32	uv_offset_{};
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit pmx_uv_morph_offset_impl(const pmx_model_description* model_info) : pmx_element_impl(model_info) {}
		/**
		 * \brief Destructor function
		 */
		~pmx_uv_morph_offset_impl() override = default;
		/**
		 * \brief Copy constructor function
		 */
		pmx_uv_morph_offset_impl(const pmx_uv_morph_offset_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		pmx_uv_morph_offset_impl& operator =(const pmx_uv_morph_offset_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		pmx_uv_morph_offset_impl(pmx_uv_morph_offset_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		pmx_uv_morph_offset_impl& operator =(pmx_uv_morph_offset_impl&&) noexcept = default;

		uint32_t get_vertex_index() const override;

		void set_vertex_index(uint32_t vertex_index) override;

		const std::array<float, 4>& get_uv_offset() const override;

		void set_uv_offset(const std::array<float, 4>& offset) override;

		/**
		 * \brief Read PMX morph offset data from a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write PMX morph offset data to a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const override;
	};

	class pmx_material_morph_offset_impl final : public pmx_material_morph_offset, pmx_element_impl
	{
		// Material index
		Int32		material_index_ = 0;
		// Hybrid method
		blend_mode	blend_mode_ = blend_mode::MULTIPLY;
		// Diffuse (diffuse)
		Vector4d32	diffuse_{};
		// Specular light (reflected color)
		Vector32	specular_{};
		// Specular light intensity
		Float32		specular_intensity_ = 0.f;
		// Ambient light (ambient color)
		Vector32	ambient_{};
		// Edge color
		Vector4d32	edge_color_{};
		// Edge size
		Float32		edge_size_ = 0.f;
		// Texture tone
		Vector4d32	texture_tint_{};
		// Environmental tone
		Vector4d32	environment_tint_{};
		// Map hue
		Vector4d32	toon_tint_{};
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit pmx_material_morph_offset_impl(const pmx_model_description* model_info) : pmx_element_impl(model_info) {}
		/**
		 * \brief Destructor function
		 */
		~pmx_material_morph_offset_impl() override = default;
		/**
		 * \brief Copy constructor function
		 */
		pmx_material_morph_offset_impl(const pmx_material_morph_offset_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		pmx_material_morph_offset_impl& operator =(const pmx_material_morph_offset_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		pmx_material_morph_offset_impl(pmx_material_morph_offset_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		pmx_material_morph_offset_impl& operator =(pmx_material_morph_offset_impl&&) noexcept = default;

		int32_t get_material_index() const override;

		void set_material_index(int32_t index) override;

		blend_mode get_blend_mode() const override;

		void set_blend_mode(const blend_mode& mode) override;

		const std::array<float, 4>& get_diffuse() const override;

		void set_diffuse(const std::array<float, 4>& diffuse) override;

		const std::array<float, 3>& get_specular() const override;

		void set_specular(const std::array<float, 3>& specular) override;

		float get_specular_intensity() const override;

		void set_specular_intensity(float specular_intensity) override;

		const std::array<float, 3>& get_ambient() const override;

		void set_ambient(const std::array<float, 3>& ambient) override;

		const std::array<float, 4>& get_edge_color() const override;

		void set_edge_color(const std::array<float, 4>& edge_color) override;

		float get_edge_size() const override;

		void set_edge_size(float edge_size) override;

		const std::array<float, 4>& get_texture_tint() const override;

		void set_texture_tint(const std::array<float, 4>& texture_tint) override;

		const std::array<float, 4>& get_environment_tint() const override;

		void set_environment_tint(const std::array<float, 4>& environment_tint) override;

		const std::array<float, 4>& get_toon_tint() const override;

		void set_toon_tint(const std::array<float, 4>& toon_tint) override;

		/**
		 * \brief Read PMX morph offset data from a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write PMX morph offset data to a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const override;
	};

	class pmx_flip_morph_offset_impl final : public pmx_flip_morph_offset, pmx_element_impl
	{
		// Deformation index
		Int32	morph_index_ = 0;
		// Influence
		Float32 morph_weight_ = 0.f;
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit pmx_flip_morph_offset_impl(const pmx_model_description* model_info) : pmx_element_impl(model_info) {}
		/**
		 * \brief Destructor function
		 */
		~pmx_flip_morph_offset_impl() override = default;
		/**
		 * \brief Copy constructor function
		 */
		pmx_flip_morph_offset_impl(const pmx_flip_morph_offset_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		pmx_flip_morph_offset_impl& operator =(const pmx_flip_morph_offset_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		pmx_flip_morph_offset_impl(pmx_flip_morph_offset_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		pmx_flip_morph_offset_impl& operator =(pmx_flip_morph_offset_impl&&) noexcept = default;

		int32_t get_morph_index() const override;

		void set_morph_index(int32_t morph_index) override;

		float get_morph_weight() const override;

		void set_morph_weight(float morph_weight) override;

		/**
		 * \brief Read PMX morph offset data from a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write PMX morph offset data to a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const override;
	};

	class pmx_impulse_morph_offset_impl final : public pmx_impulse_morph_offset, pmx_element_impl
	{
		// Rigid body index
		Int32		rigid_body_index_ = 0;
		// Local flag
		bool		is_local_ = 0;
		// Moving speed
		Vector32	movement_speed_{};
		// Rotational torque
		Vector32	rotation_torque_{};
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit pmx_impulse_morph_offset_impl(const pmx_model_description* model_info) : pmx_element_impl(model_info) {}
		/**
		 * \brief Destructor function
		 */
		~pmx_impulse_morph_offset_impl() override = default;
		/**
		 * \brief Copy constructor function
		 */
		pmx_impulse_morph_offset_impl(const pmx_impulse_morph_offset_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		pmx_impulse_morph_offset_impl& operator =(const pmx_impulse_morph_offset_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		pmx_impulse_morph_offset_impl(pmx_impulse_morph_offset_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		pmx_impulse_morph_offset_impl& operator =(pmx_impulse_morph_offset_impl&&) noexcept = default;

		int32_t get_rigid_body_index() const override;

		void set_rigid_body_index(int32_t rigid_body_index) override;

		bool is_local() const override;

		void set_local(bool value) override;

		const std::array<float, 3>& get_movement_speed() const override;

		void set_movement_speed(const std::array<float, 3>& speed) override;

		const std::array<float, 3>& get_rotation_torque() const override;

		void set_rotation_torque(const std::array<float, 3>& torque) override;

		/**
		 * \brief Read PMX morph offset data from a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write PMX morph offset data to a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const override;
	};

	class pmx_morph_impl final : public pmx_morph, pmx_element_impl
	{
		// Local deformation name
		std::u8string	morph_name_local_{};
		// Generic deformation name
		std::u8string	morph_name_universal_{};
		// Panel position, { 1,2,3,4 }，The position of the expression in the MMD panel
		display_panel_type display_panel_type_ = display_panel_type::OTHER;
		// Morph Type
		morph_type morph_type_ = morph_type::GROUP;
		// Offset data

	public:
		using pmx_group_morph_array_impl = pmx_element_array_impl<pmx_morph_offset, pmx_group_morph_offset_impl>;
		using pmx_vertex_morph_array_impl = pmx_element_array_impl<pmx_morph_offset, pmx_vertex_morph_offset_impl>;
		using pmx_bone_morph_array_impl = pmx_element_array_impl<pmx_morph_offset, pmx_bone_morph_offset_impl>;
		using pmx_uv_morph_array_impl = pmx_element_array_impl<pmx_morph_offset, pmx_uv_morph_offset_impl>;
		using pmx_material_morph_array_impl = pmx_element_array_impl<pmx_morph_offset, pmx_material_morph_offset_impl>;
		using pmx_flip_morph_array_impl = pmx_element_array_impl<pmx_morph_offset, pmx_flip_morph_offset_impl>;
		using pmx_impulse_morph_array_impl = pmx_element_array_impl<pmx_morph_offset, pmx_impulse_morph_offset_impl>;

		using offset_impl_type = std::variant<
			pmx_group_morph_array_impl,
			pmx_vertex_morph_array_impl,
			pmx_bone_morph_array_impl,
			pmx_uv_morph_array_impl,
			pmx_material_morph_array_impl,
			pmx_flip_morph_array_impl,
			pmx_impulse_morph_array_impl>;

	private:

		offset_impl_type offset_data_ = pmx_group_morph_array_impl{ model_description_ };
		
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit pmx_morph_impl(const pmx_model_description* model_info) : pmx_element_impl(model_info) {}
		/**
		 * \brief Destructor function
		 */
		~pmx_morph_impl() override = default;
		/**
		 * \brief Copy constructor function
		 */
		pmx_morph_impl(const pmx_morph_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		pmx_morph_impl& operator =(const pmx_morph_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		pmx_morph_impl(pmx_morph_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		pmx_morph_impl& operator =(pmx_morph_impl&&) noexcept = default;

		std::string get_morph_name_local() const override;

		void set_morph_name_local(const std::string& name) override;

		std::string get_morph_name_universal() const override;

		void set_morph_name_universal(const std::string& name) override;

		display_panel_type get_display_panel_type() const override;

		void set_display_panel_type(display_panel_type type) override;

		morph_type get_morph_offset_type() const override;

		void set_morph_offset_type(morph_type type) override;

		const pmx_morph_offset_array& get_morph_offset_array() const override;

		pmx_morph_offset_array& mutable_morph_offset_array() override;

		/**
		 * \brief Read PMX morph offset data from a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write PMX morph offset data to a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const override;
	};
}
