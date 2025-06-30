//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef LIBMMD_MODEL_MMD_MMDCAMERA_H_
#define LIBMMD_MODEL_MMD_MMDCAMERA_H_

#include <glm/gtc/quaternion.hpp>

namespace libmmd
{
	/**
	 * @brief Represents a camera in MMD.
	 */
	struct MMDCamera
	{
		MMDCamera();

		glm::vec3	m_interest{}; ///< Interest point
		glm::vec3	m_rotate{}; ///< Rotation
		float		m_distance{}; ///< Distance
		float		m_fov{}; ///< Field of view
	};

	/**
	 * @brief Represents a look-at camera in MMD.
	 */
	struct MMDLookAtCamera
	{
		/**
		 * @brief Construct a look-at camera from MMDCamera.
		 * @param cam Reference to the MMDCamera.
		 */
		explicit MMDLookAtCamera(const MMDCamera& cam);

		glm::vec3	m_center{}; ///< Center point
		glm::vec3	m_eye{}; ///< Eye point
		glm::vec3	m_up{}; ///< Up vector
	};
}

#endif // !LIBMMD_MODEL_MMD_MMDCAMERA_H_
