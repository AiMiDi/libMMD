/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/9/24
File:			pmx_rigid_body.h
Description:	pmx rigid body data

**************************************************************************/

#pragma once

#include "pmx_element.hpp"
#include "pmx_model_info.h"

namespace libmmd
{
	struct PMXRigidBodyNonCollisionGroup
	{
		bool	G1 : 1;
		bool	G2 : 1;
		bool	G3 : 1;
		bool	G4 : 1;
		bool	G5 : 1;
		bool	G6 : 1;
		bool	G7 : 1;
		bool	G8 : 1;
		bool	G9 : 1;
		bool	G10 : 1;
		bool	G11 : 1;
		bool	G12 : 1;
		bool	G13 : 1;
		bool	G14 : 1;
		bool	G15 : 1;
		bool	G16 : 1;

		explicit PMXRigidBodyNonCollisionGroup(
			const bool G1_ = false,
			const bool G2_ = false,
			const bool G3_ = false,
			const bool G4_ = false,
			const bool G5_ = false,
			const bool G6_ = false,
			const bool G7_ = false,
			const bool G8_ = false,
			const bool G9_ = false,
			const bool G10_ = false,
			const bool G11_ = false,
			const bool G12_ = false,
			const bool G13_ = false,
			const bool G14_ = false,
			const bool G15_ = false,
			const bool G16_ = false) :
			G1(G1_),
			G2(G2_),
			G3(G3_),
			G4(G4_),
			G5(G5_),
			G6(G6_),
			G7(G7_),
			G8(G8_),
			G9(G9_),
			G10(G10_),
			G11(G11_),
			G12(G12_),
			G13(G13_),
			G14(G14_),
			G15(G15_),
			G16(G16_)
		{
			static_assert(sizeof PMXRigidBodyNonCollisionGroup == 2);
		}
	};

	struct PMXRigidBodyData
	{
		// Rigid body local name 									
		std::u8string					rigid_body_name_local{};
		// Rigid body common name 
		std::u8string					rigid_body_name_universal{};
		// Associative bone index 
		Int32					related_bone_index = 0;
		// Group ID 
		Char					group_id = 0;
		// Collision exclusion group 
		PMXRigidBodyNonCollisionGroup	non_collision_group{};
		// Rigid body shape type
		enum
		{
			SPHERE,
			BOX,
			CAPSULE
		};
		Char		shape_type = SPHERE;
		// Shape size 
		Vector32	shape_size{};
		// Shape position   
		Vector32	shape_position{};
		// Shape rotation   
		Vector32	shape_rotation{};
		// mass   
		Float32		mass = 0.f;
		// Movement attenuation   
		Float32		move_attenuation = 0.f;
		// Rotation attenuation   
		Float32		rotation_damping = 0.f;
		// Reaction force   
		Float32		repulsion_force = 0.f;
		// friction force   
		Float32		friction_force = 0.f;
		// Physical mode
		enum
		{
			TRACKING_BONES,
			PHYSICAL_CALCULUS,
			PHYSICS_AND_BONE
		};
		Char physics_mode = TRACKING_BONES;
	};

	class PMXRigidBody final : public PMXElement
	{
		GENERATE_MMD_CLASS_BODY(PMXRigidBody, PMXRigidBodyData)
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit PMXRigidBody(const PMXModelInfo* model_info) : PMXElement(model_info), m_data(std::make_unique<data_type>()) {}
		/**
		 * \brief Destructor function
		 */
		~PMXRigidBody() override = default;
		/**
		 * \brief Read PMX rigid body data from a pmx file
		 * \param file file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
	};
}
