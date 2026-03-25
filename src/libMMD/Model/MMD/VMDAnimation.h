//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef LIBMMD_MODEL_MMD_VMDANIMATION_H_
#define LIBMMD_MODEL_MMD_VMDANIMATION_H_

#include "MMDModel.h"
#include "VMDFile.h"

#include <vector>
#include <algorithm>
#include <memory>

#include <Eigen/Geometry>

namespace libmmd
{
	/**
	 * @brief Represents a Bezier curve used in VMD animation.
	 */
	struct VMDBezier
	{
		/**
		 * @brief Evaluate the X component of the Bezier curve at time t.
		 * @param t The time parameter.
		 * @return The evaluated X component.
		 */
		float EvalX(float t) const;

		/**
		 * @brief Evaluate the Y component of the Bezier curve at time t.
		 * @param t The time parameter.
		 * @return The evaluated Y component.
		 */
		float EvalY(float t) const;

		/**
		 * @brief Evaluate both X and Y components of the Bezier curve at time t.
		 * @param t The time parameter.
		 * @return The evaluated X and Y components as a Eigen::Vector2f.
		 */
		Eigen::Vector2f Eval(float t) const;

		/**
		 * @brief Evaluate the derivative of the X component at time t.
		 * @param t The time parameter.
		 * @return dX/dt at the given time.
		 */
		float EvalDX(float t) const;

		/**
		 * @brief Find the parameter t such that EvalX(t) ≈ time, using Newton-Raphson.
		 * @param time The target X value in [0, 1].
		 * @return The parameter t corresponding to the given X value.
		 */
		float FindBezierX(float time) const;

		Eigen::Vector2f	m_cp1; ///< Control point 1
		Eigen::Vector2f	m_cp2; ///< Control point 2
	};

	class VMDNodeController;
	class VMDMorphController;
	class VMDIKController;

	/**
	 * @brief Manages VMD animation for an MMD model.
	 */
	class VMDAnimation
	{
	public:
		VMDAnimation();
		~VMDAnimation();

		/**
		 * @brief Create the VMD animation with the given MMD model.
		 * @param model The MMD model.
		 * @return True if creation is successful, false otherwise.
		 */
		bool Create(const std::shared_ptr<MMDModel>& model);

		/**
		 * @brief Add a VMD file to the animation.
		 * @param vmd The VMD file.
		 * @return True if addition is successful, false otherwise.
		 */
		bool Add(const VMDFile& vmd);

		/**
		 * @brief Destroy the VMD animation.
		 */
		void Destroy();

		/**
		 * @brief Evaluate the animation at time t with the given weight.
		 * @param t The time parameter.
		 * @param weight The weight parameter.
		 */
		void Evaluate(float t, float weight = 1.0f) const;

		/**
		 * @brief Synchronize physics with the animation.
		 * @param t The time parameter.
		 * @param frameCount The number of frames to synchronize over.
		 * @param physicsElapsed Physics timestep per iteration (default: 1/30s).
		 */
		void SyncPhysics(float t, int frameCount = 30, float physicsElapsed = 1.0f / 30.0f) const;

		/**
		 * @brief Get the maximum key time of the animation.
		 * @return The maximum key time.
		 */
		int32_t GetMaxKeyTime() const { return static_cast<int32_t>(m_maxKeyTime); }

		/**
		* @brief Get the number of node (bone) controllers in the animation.
		* @return The number of node controllers.
		*/
		int32_t GetNodeKeyNum() const { return static_cast<int32_t>(m_nodeControllers.size()); }

		/**
		* @brief Get the number of morph controllers in the animation.
		* @return The number of morph controllers.
		*/
		int32_t GetMorphKeyNum() const { return static_cast<int32_t>(m_morphControllers.size()); }

		/**
		* @brief Get the number of IK controllers in the animation.
		* @return The number of IK controllers.
		*/
		int32_t GetIKKeyNum() const { return static_cast<int32_t>(m_ikControllers.size()); }

		/**
		 * @brief Export the animation data to a VMDFile structure.
		 * @param vmd The VMDFile to populate.
		 * @return True if export is successful, false otherwise.
		 */
		bool Save(VMDFile& vmd) const;

		/**
		 * @brief Export the animation data and write to a VMD file on disk.
		 * @param filename The output file path.
		 * @return True if the file is written successfully, false otherwise.
		 */
		bool Save(const char* filename) const;

		/**
		 * @brief Control whether IK enable/disable keyframes are applied during Evaluate.
		 * When set to false, Evaluate skips IK controller evaluation, allowing external
		 * systems (e.g. Cinema 4D keyframes) to control IK solver states.
		 * @param apply True to apply IK enable keyframes (default), false to skip.
		 */
		void SetApplyIKEnable(bool apply) { m_applyIKEnable = apply; }

		/**
		 * @brief Check whether IK enable keyframes are applied during Evaluate.
		 * @return True if IK enable keyframes are applied, false otherwise.
		 */
		bool GetApplyIKEnable() const { return m_applyIKEnable; }

		/**
		 * @brief Control whether morph keyframes are applied during Evaluate.
		 * When set to false, Evaluate skips morph controller evaluation, allowing external
		 * systems (e.g. Cinema 4D CTrack keyframes) to drive morph weights instead.
		 * @param apply True to apply morph keyframes (default), false to skip.
		 */
		void SetApplyMorphEnable(bool apply) { m_applyMorphEnable = apply; }

		/**
		 * @brief Check whether morph keyframes are applied during Evaluate.
		 * @return True if morph keyframes are applied, false otherwise.
		 */
		bool GetApplyMorphEnable() const { return m_applyMorphEnable; }

	private:
		int32_t CalculateMaxKeyTime() const;

		using NodeControllerPtr = std::unique_ptr<VMDNodeController>;
		using IKControllerPtr = std::unique_ptr<VMDIKController>;
		using MorphControllerPtr = std::unique_ptr<VMDMorphController>;

		std::shared_ptr<MMDModel>			m_model; ///< The MMD model
		std::vector<NodeControllerPtr>		m_nodeControllers; ///< Node controllers
		std::vector<IKControllerPtr>		m_ikControllers; ///< IK controllers
		std::vector<MorphControllerPtr>		m_morphControllers; ///< Morph controllers
		uint32_t	m_maxKeyTime; ///< Maximum key time
		bool		m_applyIKEnable = true; ///< Whether to apply IK enable keyframes during Evaluate
		bool		m_applyMorphEnable = true; ///< Whether to apply morph keyframes during Evaluate
	};

}

#endif // !LIBMMD_MODEL_MMD_VMDANIMATION_H_
