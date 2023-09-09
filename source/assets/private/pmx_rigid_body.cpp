/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/9/24
File:			pmx_rigid_body.cpp
Description:	pmx rigid body data

**************************************************************************/

#include "pmx_rigid_body.h"

namespace libmmd
{
	 bool pmx_rigid_body_non_collision_group_impl::in_group_0() const
	{
		return non_collision_group_.G0;
	}

	 void pmx_rigid_body_non_collision_group_impl::set_in_group_0(bool value)
	{
		non_collision_group_.G0 = value;
	}

	 bool pmx_rigid_body_non_collision_group_impl::in_group_1() const
	{
		return non_collision_group_.G1;
	}

	 void pmx_rigid_body_non_collision_group_impl::set_in_group_1(bool value)
	{
		non_collision_group_.G1 = value;
	}

	 bool pmx_rigid_body_non_collision_group_impl::in_group_2() const
	{
		return non_collision_group_.G2;
	}

	 void pmx_rigid_body_non_collision_group_impl::set_in_group_2(bool value)
	{
		non_collision_group_.G2 = value;
	}

	 bool pmx_rigid_body_non_collision_group_impl::in_group_3() const
	{
		return non_collision_group_.G3;
	}

	 void pmx_rigid_body_non_collision_group_impl::set_in_group_3(bool value)
	{
		non_collision_group_.G3 = value;
	}

	 bool pmx_rigid_body_non_collision_group_impl::in_group_4() const
	{
		return non_collision_group_.G4;
	}

	 void pmx_rigid_body_non_collision_group_impl::set_in_group_4(bool value)
	{
		non_collision_group_.G4 = value;
	}

	 bool pmx_rigid_body_non_collision_group_impl::in_group_5() const
	{
		return non_collision_group_.G5;
	}

	 void pmx_rigid_body_non_collision_group_impl::set_in_group_5(bool value)
	{
		non_collision_group_.G5 = value;
	}

	 bool pmx_rigid_body_non_collision_group_impl::in_group_6() const
	{
		return non_collision_group_.G6;
	}

	 void pmx_rigid_body_non_collision_group_impl::set_in_group_6(bool value)
	{
		non_collision_group_.G6 = value;
	}

	 bool pmx_rigid_body_non_collision_group_impl::in_group_7() const
	{
		return non_collision_group_.G7;
	}

	 void pmx_rigid_body_non_collision_group_impl::set_in_group_7(bool value)
	{
		non_collision_group_.G7 = value;
	}

	 bool pmx_rigid_body_non_collision_group_impl::in_group_8() const
	{
		return non_collision_group_.G8;
	}

	 void pmx_rigid_body_non_collision_group_impl::set_in_group_8(bool value)
	{
		non_collision_group_.G8 = value;
	}

	 bool pmx_rigid_body_non_collision_group_impl::in_group_9() const
	{
		return non_collision_group_.G9;
	}

	 void pmx_rigid_body_non_collision_group_impl::set_in_group_9(bool value)
	{
		non_collision_group_.G9 = value;
	}

	 bool pmx_rigid_body_non_collision_group_impl::in_group_10() const
	{
		return non_collision_group_.G10;
	}

	 void pmx_rigid_body_non_collision_group_impl::set_in_group_10(bool value)
	{
		non_collision_group_.G10 = value;
	}

	 bool pmx_rigid_body_non_collision_group_impl::in_group_11() const
	{
		return non_collision_group_.G11;
	}

	 void pmx_rigid_body_non_collision_group_impl::set_in_group_11(bool value)
	{
		non_collision_group_.G11 = value;
	}

	 bool pmx_rigid_body_non_collision_group_impl::in_group_12() const
	{
		return non_collision_group_.G12;
	}

	 void pmx_rigid_body_non_collision_group_impl::set_in_group_12(bool value)
	{
		non_collision_group_.G12 = value;
	}

	 bool pmx_rigid_body_non_collision_group_impl::in_group_13() const
	{
		return non_collision_group_.G13;
	}

	 void pmx_rigid_body_non_collision_group_impl::set_in_group_13(bool value)
	{
		non_collision_group_.G13 = value;
	}

	 bool pmx_rigid_body_non_collision_group_impl::in_group_14() const
	{
		return non_collision_group_.G14;
	}

	 void pmx_rigid_body_non_collision_group_impl::set_in_group_14(bool value)
	{
		non_collision_group_.G14 = value;
	}

	 bool pmx_rigid_body_non_collision_group_impl::in_group_15() const
	{
		return non_collision_group_.G15;
	}

	 void pmx_rigid_body_non_collision_group_impl::set_in_group_15(bool value)
	{
		non_collision_group_.G15 = value;
	}


	bool& pmx_rigid_body_non_collision_group_impl::operator[](uint8_t group_index)
	{
		switch (group_index % 15)
		{
		case 0:
		default:
			return non_collision_group_.G0;
		case 1:
			return non_collision_group_.G1;
		case 2:
			return non_collision_group_.G2;
		case 3:
			return non_collision_group_.G3;
		case 4:
			return non_collision_group_.G4;
		case 5:
			return non_collision_group_.G5;
		case 6:
			return non_collision_group_.G6;
		case 7:
			return non_collision_group_.G7;
		case 8:
			return non_collision_group_.G8;
		case 9:
			return non_collision_group_.G9;
		case 10:
			return non_collision_group_.G10;
		case 11:
			return non_collision_group_.G11;
		case 12:
			return non_collision_group_.G12;
		case 13:
			return non_collision_group_.G13;
		case 14:
			return non_collision_group_.G14;
		case 15:
			return non_collision_group_.G15;
		}
	}

	bool pmx_rigid_body_non_collision_group_impl::read_from_file(const file& file)
	{
		pmx_rigid_body_non_collision_group_file_data data;
		if(!file.read_element(data))
			return false;
		non_collision_group_.G0 = data.G0;
		non_collision_group_.G1 = data.G1;
		non_collision_group_.G2 = data.G2;
		non_collision_group_.G3 = data.G3;
		non_collision_group_.G4 = data.G4;
		non_collision_group_.G5 = data.G5;
		non_collision_group_.G6 = data.G6;
		non_collision_group_.G7 = data.G7;
		non_collision_group_.G8 = data.G8;
		non_collision_group_.G9 = data.G9;
		non_collision_group_.G10 = data.G10;
		non_collision_group_.G11 = data.G11;
		non_collision_group_.G12 = data.G12;
		non_collision_group_.G13 = data.G13;
		non_collision_group_.G14 = data.G14;
		non_collision_group_.G15 = data.G15;
		return true;
	}

	auto pmx_rigid_body_non_collision_group_impl::write_to_file(const file& file) const -> bool
	{
		pmx_rigid_body_non_collision_group_file_data data;
		data.G0 = non_collision_group_.G0;
		data.G1 = non_collision_group_.G1;
		data.G2 = non_collision_group_.G2;
		data.G3 = non_collision_group_.G3;
		data.G4 = non_collision_group_.G4;
		data.G5 = non_collision_group_.G5;
		data.G6 = non_collision_group_.G6;
		data.G7 = non_collision_group_.G7;
		data.G8 = non_collision_group_.G8;
		data.G9 = non_collision_group_.G9;
		data.G10 = non_collision_group_.G10;
		data.G11 = non_collision_group_.G11;
		data.G12 = non_collision_group_.G12;
		data.G13 = non_collision_group_.G13;
		data.G14 = non_collision_group_.G14;
		data.G15 = non_collision_group_.G15;
		return file.write_element(data);
	}

	 std::string pmx_rigid_body_impl::get_rigid_body_name_local() const
	{
		return std::string{rigid_body_name_local_.begin(), rigid_body_name_local_.end()};
	}

	 void pmx_rigid_body_impl::set_rigid_body_name_local(const std::string& name)
	{
		rigid_body_name_local_ = std::u8string{ name.begin(),name.end() };
	}

	 std::string pmx_rigid_body_impl::get_rigid_body_name_universal() const
	{
		return std::string{rigid_body_name_universal_.begin(), rigid_body_name_universal_.end()};
	}

	 void pmx_rigid_body_impl::set_rigid_body_name_universal(const std::string& name)
	{
		rigid_body_name_universal_ = std::u8string{ name.begin(),name.end() };
	}

	 int32_t pmx_rigid_body_impl::get_related_bone_index() const
	{
		return related_bone_index_;
	}

	 void pmx_rigid_body_impl::set_related_bone_index(int32_t index)
	{
		related_bone_index_ = index;
	}

	 int8_t pmx_rigid_body_impl::get_group_id() const
	{
		return group_id_;
	}

	 void pmx_rigid_body_impl::set_group_id(int8_t id)
	{
		group_id_ = id;
	}

	 const pmx_rigid_body_non_collision_group& pmx_rigid_body_impl::get_non_collision_group() const
	{
		return non_collision_group_;
	}

	 pmx_rigid_body_non_collision_group& pmx_rigid_body_impl::mutable_non_collision_group()
	{
		return non_collision_group_;
	}

	 pmx_rigid_body::shape_type pmx_rigid_body_impl::get_shape_type() const
	{
		return shape_type_;
	}

	 void pmx_rigid_body_impl::set_shape_type(shape_type type)
	{
		shape_type_ = type;
	}

	 const std::array<float, 3>& pmx_rigid_body_impl::get_shape_size() const
	{
		return shape_size_;
	}

	 void pmx_rigid_body_impl::set_shape_size(const std::array<float, 3>& size)
	{
		shape_size_ = size;
	}

	 const std::array<float, 3>& pmx_rigid_body_impl::get_shape_position() const
	{
		return shape_position_;
	}

	 void pmx_rigid_body_impl::set_shape_position(const std::array<float, 3>& position)
	{
		shape_position_ = position;
	}

	 const std::array<float, 3>& pmx_rigid_body_impl::get_shape_rotation() const
	{
		return shape_rotation_;
	}

	 void pmx_rigid_body_impl::set_shape_rotation(const std::array<float, 3>& rotation)
	{
		shape_rotation_ = rotation;
	}

	 float pmx_rigid_body_impl::get_mass() const
	{
		return mass_;
	}

	 void pmx_rigid_body_impl::set_mass(float mass)
	{
		mass_ = mass;
	}

	 float pmx_rigid_body_impl::get_move_attenuation() const
	{
		return move_attenuation_;
	}

	 void pmx_rigid_body_impl::set_move_attenuation(float attenuation)
	{
		move_attenuation_ = attenuation;
	}

	 float pmx_rigid_body_impl::get_rotation_damping() const
	{
		return rotation_damping_;
	}

	 void pmx_rigid_body_impl::set_rotation_damping(float damping)
	{
		rotation_damping_ = damping;
	}

	 float pmx_rigid_body_impl::get_repulsion_force() const
	{
		return repulsion_force_;
	}

	 void pmx_rigid_body_impl::set_repulsion_force(float force)
	{
		repulsion_force_ = force;
	}

	 float pmx_rigid_body_impl::get_friction_force() const
	{
		return friction_force_;
	}

	 void pmx_rigid_body_impl::set_friction_force(float force)
	{
		friction_force_ = force;
	}

	 pmx_rigid_body::physics_mode pmx_rigid_body_impl::get_physics_mode() const
	{
		return physics_mode_;
	}

	 void pmx_rigid_body_impl::set_physics_mode(physics_mode mode)
	{
		physics_mode_ = mode;
	}


	bool pmx_rigid_body_impl::read_from_file(const file& file)
	{
		if (!model_description_->read_text(file, rigid_body_name_local_))
			return false;
		if (!model_description_->read_text(file, rigid_body_name_universal_))
			return false;
		if (!model_description_->read_bone_index(file, related_bone_index_))
			return false;
		if (!file.read_element(group_id_))
			return false;
		if (!non_collision_group_.read_from_file(file))
			return false;
		if (!file.read_element(shape_type_))
			return false;
		if (!file.read_element(shape_size_))
			return false;
		if (!file.read_element(shape_position_))
			return false;
		if (!file.read_element(shape_rotation_))
			return false;
		if (!file.read_element(mass_))
			return false;
		if (!file.read_element(move_attenuation_))
			return false;
		if (!file.read_element(rotation_damping_))
			return false;
		if (!file.read_element(repulsion_force_))
			return false;
		if (!file.read_element(friction_force_))
			return false;
		if (!file.read_element(physics_mode_))
			return false;
		return true;
	}

	bool pmx_rigid_body_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return false;
	}
}
