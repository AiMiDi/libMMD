/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/8/14
File:			pmx_model_info.h
Description:	pmx model information data

**************************************************************************/

#pragma once

#include "libmmd_pmx_util.h"

namespace libmmd
{
	/**
	 * \brief PMX model global information struct
	 */
	class pmx_model_description
	{
		// String encoding. 0 is UTF16LE encoding and 1 is UTF8 encoding
		Char	text_encoding_ = 0;
		// Vertex index size
		Char	vertex_index_size_ = 4;
		// Texture index size
		Char	texture_index_size_ = 2;
		// Material index size 
		Char	material_index_size_ = 2;
		// Bone index size
		Char	bone_index_size_ = 2;
		// Morph index size 
		Char	morph_index_size_ = 2;
		// Rigid-body index size 
		Char	rigid_body_index_size_ = 2;

		// model_version
		Float32 model_version_ = 2.0f;
		// Additional Vector4d32 numbers
		Char	additional_vec4_count_ = 1;
		// Local model name 
		std::u8string	model_name_local_;
		// Generic model name 
		std::u8string	model_name_universal_;
		// Local language model description 
		std::u8string	comments_local_;
		// Common language model description
		std::u8string	comments_universal_;
		// Is there a UV morph  
		bool	have_UV_morph = false;
		// Is there a vertex morph 
		bool	have_vertex_morph = false;
	public:
		pmx_text_reader				read_text{};
		pmx_unsigned_index_reader	read_vertex_index{};
		pmx_index_reader			read_texture_index{};
		pmx_index_reader			read_material_index{};
		pmx_index_reader			read_bone_index{};
		pmx_index_reader			read_morph_index{};
		pmx_index_reader			read_rigid_body_index{};

		/**
		 * \brief Default constructor function
		 */
		pmx_model_description() = default;
		/**
		 * \brief Constructor function
		 */
		explicit pmx_model_description(std::u8string model_name_local, std::u8string model_name_universal, std::u8string comments_local, std::u8string comments_universal):
			model_name_local_(std::move(model_name_local)),
			model_name_universal_(std::move(model_name_universal)),
			comments_local_(std::move(comments_local)),
			comments_universal_(std::move(comments_universal)){}
		/**
		* \brief Destructor function
		*/
		~pmx_model_description() = default;
		float get_model_version() const
		{
			return model_version_;
		}
		void set_model_version(float version)
		{
			model_version_ = version;
		}
		const std::u8string& get_model_name_local() const
		{
			return  model_name_local_;
		}
		void set_model_name_local(const std::u8string& name)
		{
			model_name_local_ = name;
		}
		const std::u8string& get_model_name_universal() const
		{
			return  model_name_universal_;
		}
		void set_model_name_universal(const std::u8string& name)
		{
			model_name_universal_ = name;
		}
		const std::u8string& get_comments_local() const
		{
			return  comments_local_;
		}
		void set_comments_local(const std::u8string& comments)
		{
			comments_local_ = comments;
		}
		const std::u8string& get_comments_universal() const
		{
			return  comments_universal_;
		}
		void set_comments_universal(const std::u8string& comments)
		{
			comments_universal_ = comments;
		}
		void reset_reader();
		/**
		 * \brief Read PMX model info data from a pmx file
		 * \param file file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file);
	};
}

