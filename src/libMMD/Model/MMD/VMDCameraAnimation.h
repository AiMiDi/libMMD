//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef LIBMMD_MODEL_MMD_VMDCAMERAANIMATION_H_
#define LIBMMD_MODEL_MMD_VMDCAMERAANIMATION_H_

#include "MMDCamera.h"
#include "VMDAnimation.h"
#include <cstdint>
#include <memory>

namespace libmmd
{
	class VMDCameraController;
	class VMDCameraAnimation
	{
	public:
		VMDCameraAnimation();
		~VMDCameraAnimation();

		/**
		 * @brief Create the VMD camera animation from a VMD file.
		 * @param vmd Reference to the VMD file.
		 * @return True if creation is successful, false otherwise.
		 */
		bool Create(const VMDFile& vmd);

		/**
		 * @brief Destroy the VMD camera animation.
		 */
		void Destroy();

		/**
		 * @brief Evaluate the camera animation at a given time.
		 * @param t Time to evaluate the animation.
		 */
		void Evaluate(float t);

		/**
		 * @brief Get the camera.
		 * @return Reference to the camera.
		 */
		const MMDCamera& GetCamera() const { return m_camera; }

		/**
		 * @brief Get the number of keys in the animation.
		 * @return Number of keys.
		 */
		size_t GetKeyCount() const;

		/**
		 * @brief Get the maximum key time in the animation.
		 * @return Maximum key time.
		 */
		int32_t GetMaxKeyTime() const;

	private:
		VMDCameraController*	m_cameraController;

		MMDCamera	m_camera;
	};

}

#endif // !LIBMMD_MODEL_MMD_VMDCAMERAANIMATION_H_
