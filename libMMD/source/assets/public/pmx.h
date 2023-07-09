/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/2
File:			pmx_data.h
Description:	pmx file data

**************************************************************************/

#pragma once

#include "pmx_model_description.h"
#include "pmx_vertex.h"
#include "pmx_surface.h"
#include "pmx_texture.h"
#include "pmx_material.h"
#include "pmx_bone.h"
#include "pmx_morph.h"
#include "pmx_display.h"
#include "pmx_rigid_body.h"
#include "pmx_joint.h"
#include "pmx_element.hpp"

namespace libmmd
{
	class pmx_model_impl final : public pmx_model
	{
		// 模型信息 
		pmx_model_description model_description_{ u8"model", u8"model", u8"", u8"" };
		// 顶点数据
		pmx_element_array_impl<pmx_vertex, pmx_vertex_impl>					vertex_data_{ &model_description_ };
		// 面数据  
		pmx_element_array_impl<pmx_surface, pmx_surface_impl>				surface_data_{ &model_description_ };
		// 贴图数据 
		pmx_element_array_impl<pmx_texture, pmx_texture_impl>				texture_data_{ &model_description_ };
		// 材质数据 
		pmx_element_array_impl<pmx_material, pmx_material_impl>				material_data_{ &model_description_ };
		// 骨骼数据 
		pmx_element_array_impl<pmx_bone, pmx_bone_impl>						bone_data_{ &model_description_ };
		// 变形数据 
		pmx_element_array_impl<pmx_morph, pmx_morph_impl>					morph_data_{ &model_description_ };
		// 表示枠数据
		pmx_element_array_impl<pmx_display_frame, pmx_display_frame_impl>	display_data_{ &model_description_ };
		// 刚体数据 
		pmx_element_array_impl<pmx_rigid_body, pmx_rigid_body_impl>			rigid_body_data_{ &model_description_ };
		// joint数据  
		pmx_element_array_impl<pmx_joint, pmx_joint_impl>					joint_data_{ &model_description_ };
	public:
		/**
		 * \brief Default constructor function
		 */
		pmx_model_impl() = default;
		/**
		 * \brief Destructor function
		 */
		~pmx_model_impl() override = default;
		/**
		 * \brief Copy constructor function
		 */
		pmx_model_impl(const pmx_model_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		pmx_model_impl& operator =(const pmx_model_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		pmx_model_impl(pmx_model_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		pmx_model_impl& operator =(pmx_model_impl&&) noexcept = default;

		[[nodiscard]] pmx_version get_version() const override;
		void set_version(pmx_version version) override;

		[[nodiscard]] std::string get_model_name_local() const override;
		void set_model_name_local(const std::string& name) override;

		[[nodiscard]] std::string get_model_name_universal() const override;
		void set_model_name_universal(const std::string& name) override;

		[[nodiscard]] std::string get_comments_local() const override;
		void set_comments_local(const std::string& comments) override;

		[[nodiscard]] std::string get_comments_universal() const override;
		void set_comments_universal(const std::string& comments) override;

		const pmx_vertex_array& get_pmx_vertex_array() override;
		pmx_vertex_array& mutable_pmx_vertex_array() override;

		const pmx_surface_array& get_pmx_surface_array() override;
		pmx_surface_array& mutable_pmx_surface_array() override;

		const pmx_texture_array& get_pmx_texture_array() override;
		pmx_texture_array& mutable_pmx_texture_array() override;

		const pmx_material_array& get_pmx_material_array() override;
		pmx_material_array& mutable_pmx_material_array() override;

		const pmx_bone_array& get_pmx_bone_array() override;
		pmx_bone_array& mutable_pmx_bone_array() override;

		const pmx_morph_array& get_pmx_morph_array() override;
		pmx_morph_array& mutable_pmx_morph_array() override;

		const pmx_display_frame_array& get_pmx_display_frame_array() override;
		pmx_display_frame_array& mutable_pmx_display_frame_array() override;

		const pmx_rigid_body_array& get_pmx_rigid_body_array() override;
		pmx_rigid_body_array& mutable_pmx_rigid_body_array() override;

		const pmx_joint_array& get_pmx_joint_array() override;
		pmx_joint_array& mutable_pmx_joint_array() override;

		bool read_from_file(const std::string& file_name) override;
		[[nodiscard]] bool write_to_file(const std::string& file_name) const override;

		bool read_from_file(const std::wstring& file_name) override;
		[[nodiscard]] bool write_to_file(const std::wstring& file_name) const override;

	private:
		bool read_from_file_impl(const path& path);
		[[nodiscard]] bool write_to_file_impl(const path& path) const;
	};
}