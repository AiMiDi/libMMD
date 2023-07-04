/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/9/14
File:			pmx_material.h
Description:	pmx material data

**************************************************************************/

#pragma once

#include "pmx_element.hpp"
#include "pmx_model_description.h"

namespace libmmd
{
	/* Material symbol(1 byte) */
	struct pmx_material_flags
	{
		// Double sided depiction 
		bool	double_side : 1 = false;
		// Cast shadows on geometry 
		bool	ground_shadow : 1 = false;
		// Render to shadow map（本影标示）
		bool	draw_shadow : 1 = false;
		// Accept shadows from shadow maps（本影）
		bool	receive_shadow : 1 = false;
		// Edge depiction（轮廓线有效） 
		bool	draw_edge : 1 = false;
		// Use additional vector4d32 as the color of the vertices 
		bool	vertex_color : 1 = false;
		// All three vertices are drawn 
		bool	point_drawing : 1 = false;
		// Three sides are drawn 
		bool	line_drawing : 1 = false;
	};


	class pmx_material_impl : public pmx_element_impl, pmx_material
	{
		// Local material name, Japanese, Chinese, etc 
		std::u8string		material_name_local_{};
		// General material name, usually in English 
		std::u8string		material_name_universal_{};
		// diffuse color RGBA 
		Vector4d32			diffuse_color_{ 1, 1, 1, 1 };
		// Specular (specular) color RGB 
		Vector32			specular_color_{};
		// Specular light intensity 
		Float32				specular_strength_ = 5.0f;
		// Environment color, the shadow color when the light is insufficient (i.e. the base color, which makes the shadow less black) 
		Vector32			ambient_color_{ 1, 1, 1 };
		// Draw marker 
		pmx_material_flags	material_flags_{};
		// Edge color RGBA 
		Vector4d32			edge_color_{ 0, 0, 0, 1 };
		// Edge scale[0, 1] 
		Float32				edge_scale_ = 1.f;
		// Texture index 
		Int32				texture_index_ = -1;
		// Environment (specular map) index for environment mapping 
		Int32				environment_index_ = 0;
		// Environment (specular map) blend mode, 0 = off, 1 = multiply, 2 = add, 3 = extra vector4d32 */
		environment_blend_mode	environment_blend_mode_ = environment_blend_mode::OFF;
		// Map reference 0 = reference texture, 1 = internal reference 
		toon_reference_type		toon_reference_ = toon_reference_type::INTERNAL;
		// toon texture part 
		Int32				toon_part_ = 0;
		// toon internal index 
		Char				toon_internal_ = 0;
		// Metadata, used for scripts and other data. 
		std::u8string		meta_data_{};
		// Number of faces, which indicates how many faces are affected by the current material. 
		Int32				surface_count_ = 0;
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit pmx_material_impl(const pmx_model_description* model_info) : pmx_element_impl(model_info) {}
		/**
		 * \brief Destructor function
		 */
		~pmx_material_impl() override = default;

		pmx_material_impl(const pmx_material_impl&) = default;
		pmx_material_impl& operator =(const pmx_material_impl&) = default;
		pmx_material_impl(pmx_material_impl&&) noexcept = default;
		pmx_material_impl& operator =(pmx_material_impl&&) noexcept = default;

		[[nodiscard]] std::string get_material_name_local() const override;
		void set_material_name_local(const std::string& name) override;

		[[nodiscard]] std::string get_material_name_universal() const override;
		void set_material_name_universal(const std::string& name) override;

		[[nodiscard]] const std::array<float, 4>& get_diffuse_color() const override;
		void set_diffuse_color(const std::array<float, 4>& color) override;

		[[nodiscard]] const std::array<float, 3>& get_specular_color() const override;
		void set_specular_color(const std::array<float, 3>& color) override;

		[[nodiscard]] float get_specular_strength() const override;
		void set_specular_strength(float strength) override;

		[[nodiscard]] const std::array<float, 3>& get_ambient_color() const override;
		void set_ambient_color(const std::array<float, 3>& color) override;

		[[nodiscard]] bool is_double_side() const override;
		void set_is_double_side(bool value) override;

		[[nodiscard]] bool have_ground_shadow() const override;
		void set_have_ground_shadow(bool value) override;

		[[nodiscard]] bool is_draw_shadow() const override;
		void set_is_draw_shadow(bool value) override;

		[[nodiscard]] bool is_receive_shadow() const override;
		void set_is_receive_shadow(bool value) override;

		[[nodiscard]] bool is_draw_edge() const override;
		void set_have_edge(bool value) override;

		[[nodiscard]] bool is_vertex_color() const override;
		void set_is_vertex_color(bool value) override;

		[[nodiscard]] bool is_point_drawing() const override;
		void set_is_point_drawing(bool value) override;

		[[nodiscard]] bool is_line_drawing() const override;
		void set_is_line_drawing(bool value) override;

		[[nodiscard]] const std::array<float, 4>& get_edge_color() const override;
		void set_edge_color(const std::array<float, 4>& color) override;

		[[nodiscard]] float get_edge_scale() const override;
		void set_edge_scale(float scale) override;

		[[nodiscard]] int32_t get_texture_index() const override;
		void set_texture_index(int32_t index) override;

		[[nodiscard]] environment_blend_mode get_environment_blend_mode() const override;
		void set_environment_blend_mode(environment_blend_mode mode) override;

		[[nodiscard]] int32_t get_environment_index() const override;
		void set_environment_index(int32_t index) override;

		[[nodiscard]] toon_reference_type get_toon_reference() const override;
		void set_toon_reference(toon_reference_type reference) override;

		[[nodiscard]] int32_t get_toon_part() const override;
		void set_toon_part(int32_t part) override;

		[[nodiscard]] int8_t get_toon_internal() const override;
		void set_toon_internal(int8_t internal) override;

		[[nodiscard]] std::string get_meta_data() const override;
		void set_meta_data(const std::string& data) override;

		[[nodiscard]] int32_t get_surface_count() const override;
		void set_surface_count(int32_t count) override;

		/**
		 * \brief Read PMX material data from a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write PMX material data to a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const override;
	};
}
