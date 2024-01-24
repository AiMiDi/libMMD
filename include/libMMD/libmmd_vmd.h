/**************************************************************************

Copyright:Copyright(c) 2023-present, Aimidi & libmmd contributors.
Author:			Aimidi
Date:			2023/7/3
File:			vmd_animation.h
Description:	vmd data interface

**************************************************************************/

#pragma once

#include "libmmd_marco.h"
#include "libmmd_assets_file.h"
#include <string>
#include <array>

namespace libmmd
{
	template<typename T>
	class LIBMMD_API vmd_element_array
	{
	protected:
		using vmd_element_type = T;
	public:
		virtual ~vmd_element_array() = default;

		virtual uint64_t size() const = 0;

		LIBMMD_NODISCARD virtual const vmd_element_type& operator[](uint64_t index) const = 0;
		virtual vmd_element_type& operator[](uint64_t index) = 0;

		virtual vmd_element_type& add() = 0;
		virtual bool remove(uint64_t index, uint64_t count = 1) = 0;

		virtual void sort() = 0;
	};

	class LIBMMD_API vmd_interpolator
	{
	public:
		virtual ~vmd_interpolator() = default;

		LIBMMD_NODISCARD virtual uint8_t get_ax() const = 0;
		virtual void set_ax(uint8_t value) = 0;

		LIBMMD_NODISCARD virtual uint8_t get_ay() const = 0;
		virtual void set_ay(uint8_t value) = 0;

		LIBMMD_NODISCARD virtual uint8_t get_bx() const = 0;
		virtual void set_bx(uint8_t value) = 0;

		LIBMMD_NODISCARD virtual uint8_t get_by() const = 0;
		virtual void set_by(uint8_t value) = 0;

		LIBMMD_NODISCARD virtual bool is_linear() const = 0;
		virtual void set_linear() = 0;
	};

	class LIBMMD_API vmd_key_frame
	{
	public:
		virtual ~vmd_key_frame() = default;

		LIBMMD_NODISCARD virtual uint32_t get_frame_at() const = 0;
		virtual void set_frame_at(uint32_t frame_at) = 0;
	};

	class LIBMMD_API vmd_bone_key_frame : virtual public vmd_key_frame
	{
	public:
		~vmd_bone_key_frame() override = default;

		LIBMMD_NODISCARD virtual std::string get_bone_name() const = 0;
		virtual void set_bone_name(const std::string& name) = 0;

		LIBMMD_NODISCARD virtual const std::array<float, 3>& get_position() const = 0;
		virtual void set_position(const std::array<float, 3>& position) = 0;

		LIBMMD_NODISCARD virtual const std::array<float, 4>& get_rotation() const = 0;
		virtual void set_rotation(const std::array<float, 4>& rotation) = 0;

		LIBMMD_NODISCARD virtual const vmd_interpolator& get_position_x_interpolator() const = 0;
		virtual vmd_interpolator& mutable_position_x_interpolator() = 0;

		LIBMMD_NODISCARD virtual const vmd_interpolator& get_position_y_interpolator() const = 0;
		virtual vmd_interpolator& mutable_position_y_interpolator() = 0;

		LIBMMD_NODISCARD virtual const vmd_interpolator& get_position_z_interpolator() const = 0;
		virtual vmd_interpolator& mutable_position_z_interpolator() = 0;

		LIBMMD_NODISCARD virtual const vmd_interpolator& get_rotation_interpolator() const = 0;
		virtual vmd_interpolator& mutable_rotation_interpolator() = 0;
	};

	class LIBMMD_API vmd_morph_key_frame : virtual public vmd_key_frame
	{
	public:
		~vmd_morph_key_frame() override = default;

		LIBMMD_NODISCARD virtual std::string get_morph_name() const = 0;
		virtual void set_morph_name(const std::string& name) = 0;

		LIBMMD_NODISCARD virtual float get_weight() const = 0;
		virtual void set_weight(float weight) = 0;
	};

	class LIBMMD_API vmd_camera_key_frame : virtual public vmd_key_frame
	{
	public:
		~vmd_camera_key_frame() override = default;

		LIBMMD_NODISCARD virtual float get_distance() const = 0;
		virtual void set_distance(float distance) = 0;

		LIBMMD_NODISCARD virtual const std::array<float, 3>& get_position() const = 0;
		virtual void set_position(const std::array<float, 3>& position) = 0;

		LIBMMD_NODISCARD virtual const std::array<float, 3>& get_rotation() const = 0;
		virtual void set_rotation(const std::array<float, 3>& rotation) = 0;

		LIBMMD_NODISCARD virtual uint32_t get_view_angle() const = 0;
		virtual void set_view_angle(uint32_t view_angle) = 0;

		LIBMMD_NODISCARD virtual bool is_perspective() const = 0;
		virtual void set_perspective(bool value) = 0;

		LIBMMD_NODISCARD virtual const vmd_interpolator& get_position_x_interpolator() const = 0;
		virtual vmd_interpolator& mutable_position_x_interpolator() = 0;

		LIBMMD_NODISCARD virtual const vmd_interpolator& get_position_y_interpolator() const = 0;
		virtual vmd_interpolator& mutable_position_y_interpolator() = 0;

		LIBMMD_NODISCARD virtual const vmd_interpolator& get_position_z_interpolator() const = 0;
		virtual vmd_interpolator& mutable_position_z_interpolator() = 0;

		LIBMMD_NODISCARD virtual const vmd_interpolator& get_rotation_interpolator() const = 0;
		virtual vmd_interpolator& mutable_rotation_interpolator() = 0;

		LIBMMD_NODISCARD virtual const vmd_interpolator& get_distance_interpolator() const = 0;
		virtual vmd_interpolator& mutable_distance_interpolator() = 0;

		LIBMMD_NODISCARD virtual const vmd_interpolator& get_view_angle_interpolator() const = 0;
		virtual vmd_interpolator& mutable_view_angle_interpolator() = 0;
	};

	class LIBMMD_API vmd_light_key_frame : virtual public vmd_key_frame
	{
	public:
		~vmd_light_key_frame() override = default;

		LIBMMD_NODISCARD virtual const std::array<float, 3>& get_color() const = 0;
		virtual void set_color(const std::array<float, 3>& color) = 0;

		LIBMMD_NODISCARD virtual const std::array<float, 3>& get_position() const = 0;
		virtual void set_position(const std::array<float, 3>& position) = 0;
	};

	class LIBMMD_API vmd_shadow_key_frame : virtual public vmd_key_frame
	{
	public:
		~vmd_shadow_key_frame() override = default;

		enum class LIBMMD_API shadow_type
		{
			OFF,
			MODE1,
			MODE2
		};

		LIBMMD_NODISCARD virtual shadow_type get_shadow_type() const = 0;
		virtual void set_shadow_type(shadow_type type) = 0;

		LIBMMD_NODISCARD virtual float get_distance() const = 0;
		virtual void set_distance(float distance) = 0;
	};

	class LIBMMD_API vmd_IK_controller
	{
	public:
		virtual ~vmd_IK_controller() = default;

		LIBMMD_NODISCARD virtual std::string get_bone_name() const = 0;
		virtual void set_bone_name(const std::string& name) = 0;

		LIBMMD_NODISCARD virtual bool is_IK_enable() const = 0;
		virtual void set_IK_enable(bool value) = 0;
	};

	class LIBMMD_API vmd_model_controller_key_frame : virtual public vmd_key_frame
	{
	public:
		~vmd_model_controller_key_frame() override = default;

		using vmd_IK_controller_array = vmd_element_array<vmd_IK_controller>;

		LIBMMD_NODISCARD virtual bool is_mode_show() const = 0;
		virtual void set_mode_show(bool value) = 0;

		LIBMMD_NODISCARD virtual const vmd_IK_controller_array& get_vmd_IK_controller_array() const = 0;
		virtual vmd_IK_controller_array& mutable_vmd_IK_controller_array() = 0;
	};

	class LIBMMD_API vmd_animation : virtual public mmd_assets_file
	{
	protected:
		vmd_animation() = default;
	public:
		~vmd_animation() override = default;

		using vmd_bone_key_frame_array = vmd_element_array<vmd_bone_key_frame>;
		using vmd_morph_key_frame_array = vmd_element_array<vmd_morph_key_frame>;
		using vmd_camera_key_frame_array = vmd_element_array<vmd_camera_key_frame>;
		using vmd_light_key_frame_array = vmd_element_array<vmd_light_key_frame>;
		using vmd_shadow_key_frame_array = vmd_element_array<vmd_shadow_key_frame>;
		using vmd_model_controller_key_frame_array = vmd_element_array<vmd_model_controller_key_frame>;

		static vmd_animation* create();
		static void free(vmd_animation* animation);

		LIBMMD_NODISCARD virtual std::string get_model_name() const = 0;
		virtual void set_model_name(const std::string& name) = 0;

		LIBMMD_NODISCARD virtual bool is_camera() const = 0;
		virtual void set_camera(bool value) = 0;

		LIBMMD_NODISCARD virtual const vmd_bone_key_frame_array& get_vmd_bone_key_frame_array() const = 0;
		virtual vmd_bone_key_frame_array& mutable_vmd_bone_key_frame_array() = 0;

		LIBMMD_NODISCARD virtual const vmd_morph_key_frame_array& get_vmd_morph_key_frame_array() const = 0;
		virtual vmd_morph_key_frame_array& mutable_vmd_morph_key_frame_array() = 0;

		LIBMMD_NODISCARD virtual const vmd_camera_key_frame_array& get_vmd_camera_key_frame_array() const = 0;
		virtual vmd_camera_key_frame_array& mutable_vmd_camera_key_frame_array() = 0;

		LIBMMD_NODISCARD virtual const vmd_light_key_frame_array& get_vmd_light_key_frame_array() const = 0;
		virtual vmd_light_key_frame_array& mutable_vmd_light_key_frame_array() = 0;

		LIBMMD_NODISCARD virtual const vmd_shadow_key_frame_array& get_vmd_shadow_key_frame_array() const = 0;
		virtual vmd_shadow_key_frame_array& mutable_vmd_shadow_key_frame_array() = 0;

		LIBMMD_NODISCARD virtual const vmd_model_controller_key_frame_array& get_vmd_model_controller_key_frame_array() const = 0;
		virtual vmd_model_controller_key_frame_array& mutable_vmd_model_controller_key_frame_array() = 0;
	};
}
