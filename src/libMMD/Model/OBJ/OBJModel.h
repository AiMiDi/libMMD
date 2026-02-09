//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef LIBMMD_MODEL_OBJ_OBJMODEL_H_
#define LIBMMD_MODEL_OBJ_OBJMODEL_H_

#include <vector>
#include <string>
#include <Eigen/Core>

namespace libmmd
{
	class OBJModel
	{
	public:
		struct Material
		{
			std::string	m_name;

			Eigen::Vector3f	m_ambient;
			Eigen::Vector3f	m_diffuse;
			Eigen::Vector3f	m_specular;
			float		m_specularPower;
			float		m_transparency;

			std::string	m_ambientTex;
			std::string	m_diffuseTex;
			std::string	m_specularTex;
			std::string	m_transparencyTex;
		};

		struct Face
		{
			int	m_position[3];
			int	m_normal[3];
			int	m_uv[3];
			int m_material;
		};

	public:
		bool Load(const char* filepath);
		void Destroy();

		const std::vector<Eigen::Vector3f>& GetPositions() const { return m_positions; }
		const std::vector<Eigen::Vector3f>& GetNormals() const { return m_normals; }
		const std::vector<Eigen::Vector2f>& GetUVs() const { return m_uvs; }
		const std::vector<Material>& GetMaterials() const { return m_materials; }
		const std::vector<Face>& GetFaces() const { return m_faces; }

		const Eigen::Vector3f& GetBBoxMin() const { return m_bboxMin; }
		const Eigen::Vector3f& GetBBoxMax() const { return m_bboxMax; }

	private:
		std::vector<Eigen::Vector3f>	m_positions;
		std::vector<Eigen::Vector3f>	m_normals;
		std::vector<Eigen::Vector2f>	m_uvs;

		Eigen::Vector3f		m_bboxMin;
		Eigen::Vector3f		m_bboxMax;

		std::vector<Material>	m_materials;
		std::vector<Face>		m_faces;
	};
}

#endif // !LIBMMD_MODEL_OBJ_OBJMODEL_H_

