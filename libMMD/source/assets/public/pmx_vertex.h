/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/8/13
File:			pmx_vertex.h
Description:	pmx vertex data

**************************************************************************/

#pragma once

#include "pmx_model_description.h"
#include "pmx_element.hpp"

namespace libmmd
{
	class pmx_vertex_impl final : public pmx_element_impl, pmx_vertex
	{
		// position
		Vector32 position_{};
		// normal vector
		Vector32 normal_{};
		// UV coordinates
		Vector2d32 uv_{};
		// variant weight type
		pmx_vertex_skinning::skinning_type skinning_type_ = pmx_vertex_skinning::skinning_type::NONE;
		// variant weight 
		std::shared_ptr<pmx_vertex_skinning> skinning_{ nullptr };
		// Edge magnification 
		Float32 edge_scale_ = 0.f;
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit pmx_vertex_impl(const pmx_model_description* model_info) : pmx_element_impl(model_info) {}
		/**
		 * \brief Destructor function
		 */
		~pmx_vertex_impl() override = default;

		pmx_vertex_impl(const pmx_vertex_impl&) = default;
		pmx_vertex_impl& operator =(const pmx_vertex_impl&) = default;
		pmx_vertex_impl(pmx_vertex_impl&&) noexcept = default;
		pmx_vertex_impl& operator =(pmx_vertex_impl&&) noexcept = default;

		[[nodiscard]] const std::array<float, 3>& get_position() const override;
		void set_position(const std::array<float, 3>& position) override;

		[[nodiscard]] const std::array<float, 3>& get_normal() const override;
		void set_normal(const std::array<float, 3>& normal) override;

		[[nodiscard]] const std::array<float, 2>& get_uv() const override;
		void set_uv(const std::array<float, 2>& uv) override;

		[[nodiscard]] pmx_vertex_skinning::skinning_type get_skinning_type() const override;
		[[nodiscard]] const pmx_vertex_skinning* get_skinning() const override;
		pmx_vertex_skinning* create_skinning(pmx_vertex_skinning::skinning_type skinning_type) override;

		[[nodiscard]] float get_edge_scale() const override;
		void set_edge_scale(float scale) override;

		/**
		 * \brief Read PMX vertex data from a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write PMX vertex data to a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const override;

	private:
		[[nodiscard]] std::shared_ptr<pmx_vertex_skinning> new_skinning(pmx_vertex_skinning::skinning_type type) const;
	};
}

