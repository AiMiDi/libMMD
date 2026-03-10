//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef LIBMMD_MODEL_MMD_MMDMATERIAL_H_
#define LIBMMD_MODEL_MMD_MMDMATERIAL_H_

#include <string>
#include <cstdint>
#include <Eigen/Core>

namespace libmmd
{
	/**
	 * @brief Represents a material in MMD.
	 */
	struct MMDMaterial
	{
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		MMDMaterial();

		/**
		 * @brief Enum for sphere texture mode.
		 */
		enum class SphereTextureMode
		{
			None, ///< No sphere texture
			Mul,  ///< Multiply mode
			Add,  ///< Add mode
		};

		Eigen::Vector3f		m_diffuse; ///< Diffuse color
		float			m_alpha; ///< Alpha value
		Eigen::Vector3f		m_specular; ///< Specular color
		float			m_specularPower; ///< Specular power
		Eigen::Vector3f		m_ambient; ///< Ambient color
		uint8_t			m_edgeFlag; ///< Edge flag
		float			m_edgeSize; ///< Edge size
		Eigen::Vector4f		m_edgeColor; ///< Edge color
		std::string		m_texture; ///< Texture file path
		std::string		m_spTexture; ///< Sphere texture file path
		SphereTextureMode	m_spTextureMode; ///< Sphere texture mode
		std::string		m_toonTexture; ///< Toon texture file path
		Eigen::Vector4f		m_textureMulFactor; ///< Texture multiply factor
		Eigen::Vector4f		m_spTextureMulFactor; ///< Sphere texture multiply factor
		Eigen::Vector4f		m_toonTextureMulFactor; ///< Toon texture multiply factor
		Eigen::Vector4f		m_textureAddFactor; ///< Texture add factor
		Eigen::Vector4f		m_spTextureAddFactor; ///< Sphere texture add factor
		Eigen::Vector4f		m_toonTextureAddFactor; ///< Toon texture add factor
		bool			m_bothFace; ///< Both face flag
		bool			m_groundShadow; ///< Ground shadow flag
		bool			m_shadowCaster; ///< Shadow caster flag
		bool			m_shadowReceiver; ///< Shadow receiver flag
	};
}

#endif // !LIBMMD_MODEL_MMD_MMDMATERIAL_H_
