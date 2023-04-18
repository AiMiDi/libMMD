﻿/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/9/18
File:			pmx_morph.h
Description:	pmx morph data

**************************************************************************/

#pragma once

#include "pmx_element.hpp"
#include "pmx_model_info.h"

namespace libmmd
{
	class PMXMorphOffset : public PMXElement
	{
		CMT_DEFAULT_MOVE_BODY(PMXMorphOffset)
		CMT_DISALLOW_COPY_AND_ASSIGN_BODY(PMXMorphOffset)
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit PMXMorphOffset(const PMXModelInfo* model_info) : PMXElement(model_info) {}
		/**
		 * \brief Destructor function
		 */
		~PMXMorphOffset() override = default;
	public:
		/**
		 * \brief Read PMX morph offset data from a pmx file
		 * \param file file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override = 0;
	};

	struct PMXGroupMorphOffsetData
	{
		// Deformation index
		Int32	morph_index = 0;
		// Influence
		Float32 influence = 0.f;
	};

	class PMXGroupMorphOffset final : public PMXMorphOffset
	{
		GENERATE_MMD_CLASS_BODY(PMXGroupMorphOffset, PMXGroupMorphOffsetData)
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit PMXGroupMorphOffset(const PMXModelInfo* model_info) :
			PMXMorphOffset(model_info), m_data(std::make_unique<data_type>()) {}
		/**
		 * \brief Destructor function
		 */
		~PMXGroupMorphOffset() override = default;
		/**
		 * \brief Read PMX group morph offset data from a pmx file
		 * \param file file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
	};

	struct PMXVertexMorpOffsetData
	{
		// Vertex index
		UInt32		vertex_index = 0;
		// Vertex position offset
		Vector32	position_offset{};
	};

	class PMXVertexMorpOffset final : public PMXMorphOffset
	{
		GENERATE_MMD_CLASS_BODY(PMXVertexMorpOffset, PMXVertexMorpOffsetData)
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit PMXVertexMorpOffset(const PMXModelInfo* model_info) :
			PMXMorphOffset(model_info), m_data(std::make_unique<data_type>()) {}
		/**
		 * \brief Destructor function
		 */
		~PMXVertexMorpOffset() override = default;
		/**
		 * \brief Read PMX vertex morph offset data from a pmx file
		 * \param file file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
	};

	struct PMXBoneMorphOffsetData
	{
		// Bone index
		Int32		bone_index = 0;
		// Relative position of change
		Vector32	translation{};
		// Relative rotation quaternion (revolution Euler)
		Vector32	rotation{};
	};

	class PMXBoneMorphOffset final : public PMXMorphOffset
	{
		GENERATE_MMD_CLASS_BODY(PMXBoneMorphOffset, PMXBoneMorphOffsetData)
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit PMXBoneMorphOffset(const PMXModelInfo* model_info) :
			PMXMorphOffset(model_info), m_data(std::make_unique<data_type>()) {}
		/**
		 * \brief Destructor function
		 */
		~PMXBoneMorphOffset() override = default;
		/**
		 * \brief Read PMX bone morph offset data from a pmx file
		 * \param file file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
	};

	struct PMXUVMorphOffsetData
	{
		// UV type
		enum
		{
			UV, // UV
			UV1,// expansion UV1
			UV2,// expansion UV2
			UV3,// expansion UV3
			UV4 // expansion UV4
		};
		Int32 UV_type = UV;
		// Vertex index
		UInt32		vertex_index = 0;
		// Influence (only x and y are useful, Z and W are 0)
		Vector4d32	UV_offset;

		explicit PMXUVMorphOffsetData() = default;
		explicit PMXUVMorphOffsetData(const Int32& UV_type) : UV_type(UV_type) {}
	};

	class PMXUVMorphOffset final : public PMXMorphOffset
	{
		GENERATE_MMD_CLASS_BODY(PMXUVMorphOffset, PMXUVMorphOffsetData)
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit PMXUVMorphOffset(const PMXModelInfo* model_info, const Int32& UV_type) :
			PMXMorphOffset(model_info), m_data(std::make_unique<data_type>(UV_type)) {}
		/**
		 * \brief Destructor function
		 */
		~PMXUVMorphOffset() override = default;
		/**
		 * \brief Read PMX UV morph offset data from a pmx file
		 * \param file file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
	};

	struct PMXMaterialMorphOffsetData
	{
		// Material index
		Int32		material_index = 0;
		// Hybrid method
		Char		blend_mode = 0;
		// Diffuse (diffuse)
		Vector4d32	diffuse{};
		// Specular light (reflected color)
		Vector32	specular{};
		// Specular light intensity
		Float32		specularity = 0.f;
		// Ambient light (ambient color)
		Vector32	ambient{};
		// Edge color
		Vector4d32	edge_colour{};
		// Edge size
		Float32		edge_size = 0.f;
		// Texture tone
		Vector4d32	texture_tint{};
		// Environmental tone
		Vector4d32	environment_tint{};
		// Map hue
		Vector4d32	toon_tint{};
	};

	class PMXMaterialMorphOffset final : public PMXMorphOffset
	{
		GENERATE_MMD_CLASS_BODY(PMXMaterialMorphOffset, PMXMaterialMorphOffsetData)
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit PMXMaterialMorphOffset(const PMXModelInfo* model_info) :
			PMXMorphOffset(model_info), m_data(std::make_unique<data_type>()) {}
		/**
		 * \brief Destructor function
		 */
		~PMXMaterialMorphOffset() override = default;
		/**
		 * \brief Read PMX material morph offset data from a pmx file
		 * \param file file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
	};

	struct PMXFlipMorphOffsetData
	{
		// Deformation index
		Int32	morph_index = 0;
		// Influence
		Float32 influence = 0.f;
	};

	class PMXFlipMorphOffset final : public PMXMorphOffset
	{
		GENERATE_MMD_CLASS_BODY(PMXFlipMorphOffset, PMXFlipMorphOffsetData)
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit PMXFlipMorphOffset(const PMXModelInfo* model_info) :
			PMXMorphOffset(model_info), m_data(std::make_unique<data_type>()) {}
		/**
		 * \brief Destructor function
		 */
		~PMXFlipMorphOffset() override = default;
		/**
		 * \brief Read PMX flip morph offset data from a pmx file
		 * \param file file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
	};

	struct PMXImpulseMorphOffsetData
	{
		// Rigid body index
		Int32		rigid_body_index = 0;
		// Local flag
		Char		local_flag = 0;
		// Moving speed
		Vector32	movement_speed{};
		// Rotational torque
		Vector32	rotation_torque{};
	};

	class PMXImpulseMorphOffset final : public PMXMorphOffset
	{
		GENERATE_MMD_CLASS_BODY(PMXImpulseMorphOffset, PMXImpulseMorphOffsetData)
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit PMXImpulseMorphOffset(const PMXModelInfo* model_info) :
			PMXMorphOffset(model_info), m_data(std::make_unique<data_type>()) {}
		/**
		 * \brief Destructor function
		 */
		~PMXImpulseMorphOffset() override = default;
		/**
		 * \brief Read PMX impulse morph offset data from a pmx file
		 * \param file file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
	};

	struct PMXMorphData
	{
		// Local deformation name
		std::u8string	morph_name_local{};
		// Generic deformation name
		std::u8string	morph_name_universal{};
		// Panel position, { 1,2,3,4 }，The position of the expression in the MMD panel
		Char	panel_type = 0;
		enum
		{
			GROUP,// 组合（Group）
			VERTEX,// 顶点（Vertex）
			BONE,// 骨骼（Bone）
			UV,// UV
			UV1,// expansion UV1
			UV2,// expansion UV2
			UV3,// expansion UV3
			UV4,// expansion UV4
			MATERIAL,// 材质（Material）
			FLIP,// 切换（Flip）
			IMPULSE// 脉冲（Impulse）
		};
		Char	morph_type = GROUP;
		// Number of offsets 
		Int32	offset_count = 0;
		// Offset data  
		std::vector<std::unique_ptr<PMXMorphOffset>> offset_data;
	};

	class PMXMorph final : public PMXElement
	{
		GENERATE_MMD_CLASS_BODY(PMXMorph, PMXMorphData)
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit PMXMorph(const PMXModelInfo* model_info) : PMXElement(model_info), m_data(std::make_unique<data_type>()) {}
		/**
		 * \brief Destructor function
		 */
		~PMXMorph() override = default;
		/**
		 * \brief Read PMX morph data from a pmx file
		 * \param file file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
	};
}
