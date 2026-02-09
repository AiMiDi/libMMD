//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "MMDMaterial.h"

namespace libmmd
{
	MMDMaterial::MMDMaterial()
		: m_diffuse(Eigen::Vector3f::Ones())
		, m_alpha(1)
		, m_specular(Eigen::Vector3f::Zero())
		, m_specularPower(1)
		, m_ambient(Eigen::Vector3f(0.2f, 0.2f, 0.2f))
		, m_edgeFlag(0)
		, m_edgeSize(0)
		, m_edgeColor(0.0f, 0.0f, 0.0f, 1.0f)
		, m_spTextureMode(SphereTextureMode::None)
		, m_textureMulFactor(Eigen::Vector4f::Ones())
		, m_spTextureMulFactor(Eigen::Vector4f::Ones())
		, m_toonTextureMulFactor(Eigen::Vector4f::Ones())
		, m_textureAddFactor(Eigen::Vector4f::Zero())
		, m_spTextureAddFactor(Eigen::Vector4f::Zero())
		, m_toonTextureAddFactor(Eigen::Vector4f::Zero())
		, m_bothFace(false)
		, m_groundShadow(true)
		, m_shadowCaster(true)
		, m_shadowReceiver(true)
	{
	}
}

