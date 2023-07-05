/**************************************************************************

Copyright:Copyright(c) 2023-present, Aimidi & libmmd contributors.
Author:			Aimidi
Date:			2023/5/28
File:			pmx_model.h
Description:	pmx data interface

**************************************************************************/
#pragma once

#include <vector>
#include <string>
#include <array>

namespace libmmd
{
    template<typename T>
    class pmx_element_array
    {
    protected:
        using pmx_element_type = T;
    public:
        virtual ~pmx_element_array() = default;

        virtual int32_t size() const = 0;

        virtual const pmx_element_type& operator[](int32_t index) const = 0;
        virtual pmx_element_type& operator[](int32_t index) = 0;

        virtual typename std::vector<pmx_element_type>::iterator begin() = 0;
        virtual typename std::vector<pmx_element_type>::iterator end() = 0;
        virtual typename std::vector<pmx_element_type>::const_iterator begin() const = 0;
        virtual typename std::vector<pmx_element_type>::const_iterator end() const = 0;

        virtual pmx_element_type& add() = 0;
        virtual bool remove(int32_t index, uint32_t count = 1) = 0;
    };

    class pmx_vertex_skinning
    {
    public:
        virtual ~pmx_vertex_skinning() = default;

        enum class skinning_type : int8_t
        {
            NONE = -1,
            BDEF1,
            BDEF2,
            BDEF4,
            SDEF,
            QDEF
        };
    };

    class pmx_vertex_skinning_BDEF1 : public pmx_vertex_skinning
    {
    public:
        ~pmx_vertex_skinning_BDEF1() override = default;

        virtual int32_t get_bone_index() = 0;
        virtual void set_bone_index(int32_t bone_index) = 0;
    };

    class pmx_vertex_skinning_BDEF2 : public pmx_vertex_skinning
    {
    public:
        ~pmx_vertex_skinning_BDEF2() override = default;

        virtual int32_t get_bone_index1() = 0;
        virtual void set_bone_index1(int32_t bone_index) = 0;

        virtual int32_t get_bone_index2() = 0;
        virtual void set_bone_index2(int32_t bone_index) = 0;

        virtual float get_bone_weight() = 0;
        virtual void set_bone_weight(float bone_weight) = 0;
    };

    class pmx_vertex_skinning_BDEF4 : public pmx_vertex_skinning
    {
    public:
        ~pmx_vertex_skinning_BDEF4() override = default;

        virtual int32_t get_bone_index1() = 0;
        virtual void set_bone_index1(int32_t bone_index) = 0;

        virtual int32_t get_bone_index2() = 0;
        virtual void set_bone_index2(int32_t bone_index) = 0;

        virtual int32_t get_bone_index3() = 0;
        virtual void set_bone_index3(int32_t bone_index) = 0;

        virtual int32_t get_bone_index4() = 0;
        virtual void set_bone_index4(int32_t bone_index) = 0;

        virtual float get_bone_weight1() = 0;
        virtual void set_bone_weight1(float bone_weight) = 0;

        virtual float get_bone_weight2() = 0;
        virtual void set_bone_weight2(float bone_weight) = 0;

        virtual float get_bone_weight3() = 0;
        virtual void set_bone_weight3(float bone_weight) = 0;

        virtual float get_bone_weight4() = 0;
        virtual void set_bone_weight4(float bone_weight) = 0;

        virtual void normalization() = 0;

    };

    class pmx_vertex_skinning_SDEF : public pmx_vertex_skinning
    {
    public:
        ~pmx_vertex_skinning_SDEF() override = default;

        virtual int32_t get_bone_index1() = 0;
        virtual void set_bone_index1(int32_t bone_index) = 0;

        virtual int32_t get_bone_index2() = 0;
        virtual void set_bone_index2(int32_t bone_index) = 0;

        virtual float get_bone_weight() = 0;
        virtual void set_bone_weight(float bone_weight) = 0;

        virtual const std::array<float, 3>& get_sdef_c() const = 0;
        virtual void set_sdef_c(const std::array<float, 3>& sdef_c) = 0;

        virtual const std::array<float, 3>& get_sdef_r0() const = 0;
        virtual void set_sdef_r0(const std::array<float, 3>& sdef_r0) = 0;

        virtual const std::array<float, 3>& get_sdef_r1() const = 0;
        virtual void set_sdef_r1(const std::array<float, 3>& sdef_r1) = 0;
    };

    class pmx_vertex_skinning_QDEF : public pmx_vertex_skinning
    {
    public:
        ~pmx_vertex_skinning_QDEF() override = default;

        virtual int32_t get_bone_index1() = 0;
        virtual void set_bone_index1(int32_t bone_index) = 0;

        virtual int32_t get_bone_index2() = 0;
        virtual void set_bone_index2(int32_t bone_index) = 0;

        virtual int32_t get_bone_index3() = 0;
        virtual void set_bone_index3(int32_t bone_index) = 0;

        virtual int32_t get_bone_index4() = 0;
        virtual void set_bone_index4(int32_t bone_index) = 0;

        virtual float get_bone_weight1() = 0;
        virtual void set_bone_weight1(float bone_weight) = 0;

        virtual float get_bone_weight2() = 0;
        virtual void set_bone_weight2(float bone_weight) = 0;

        virtual float get_bone_weight3() = 0;
        virtual void set_bone_weight3(float bone_weight) = 0;

        virtual float get_bone_weight4() = 0;
        virtual void set_bone_weight4(float bone_weight) = 0;

        virtual void normalization() = 0;
    };

    class pmx_vertex
    {
    public:
        virtual ~pmx_vertex() = default;

        virtual const std::array<float, 3>& get_position() const = 0;
        virtual void set_position(const std::array<float, 3>& position) = 0;

        virtual const std::array<float, 3>& get_normal() const = 0;
        virtual void set_normal(const std::array<float, 3>& normal) = 0;

        virtual const std::array<float, 2>& get_uv() const = 0;
        virtual void set_uv(const std::array<float, 2>& uv) = 0;

        virtual pmx_vertex_skinning::skinning_type get_skinning_type() const = 0;
        virtual const pmx_vertex_skinning* get_skinning() const = 0;
        virtual pmx_vertex_skinning* create_skinning(pmx_vertex_skinning::skinning_type skinning_type) = 0;

        virtual float get_edge_scale() const = 0;
        virtual void set_edge_scale(float scale) = 0;
    };

    class pmx_surface
    {
    public:
        virtual ~pmx_surface() = default;

        virtual const uint32_t& get_a() const = 0;
        virtual void set_a(const uint32_t& point_index) = 0;

        virtual const uint32_t& get_b() const = 0;
        virtual void set_b(const uint32_t& point_index) = 0;

        virtual const uint32_t& get_c() const = 0;
        virtual void set_c(const uint32_t& point_index) = 0;
    };

    class pmx_texture {
    public:
        virtual ~pmx_texture() = default;
        virtual std::string get_texture_path() const = 0;
        virtual void set_texture_path(std::string& path) = 0;
    };

    class pmx_material
    {
    public:
        virtual ~pmx_material() = default;

        enum class environment_blend_mode : int8_t
        {
            OFF,
            MULTIPLY,
            ADD,
            EXTRA
        };
        enum class toon_reference_type : int8_t
        {
            TEXTURE,
            INTERNAL
        };

        virtual std::string get_material_name_local() const = 0;
        virtual void set_material_name_local(const std::string& name) = 0;

        virtual std::string get_material_name_universal() const = 0;
        virtual void set_material_name_universal(const std::string& name) = 0;

        virtual const std::array<float, 4>& get_diffuse_color() const = 0;
        virtual void set_diffuse_color(const std::array<float, 4>& color) = 0;

        virtual const std::array<float, 3>& get_specular_color() const = 0;
        virtual void set_specular_color(const std::array<float, 3>& color) = 0;

        virtual float get_specular_strength() const = 0;
        virtual void set_specular_strength(float strength) = 0;

        virtual const std::array<float, 3>& get_ambient_color() const = 0;
        virtual void set_ambient_color(const std::array<float, 3>& color) = 0;

        virtual bool is_double_side() const = 0;
        virtual void set_is_double_side(bool value) = 0;

        virtual bool have_ground_shadow() const = 0;
        virtual void set_have_ground_shadow(bool value) = 0;

        virtual bool is_draw_shadow() const = 0;
        virtual void set_is_draw_shadow(bool value) = 0;

        virtual bool is_receive_shadow() const = 0;
        virtual void set_is_receive_shadow(bool value) = 0;

        virtual bool is_draw_edge() const = 0;
        virtual void set_have_edge(bool value) = 0;

        virtual bool is_vertex_color() const = 0;
        virtual void set_is_vertex_color(bool value) = 0;

        virtual bool is_point_drawing() const = 0;
        virtual void set_is_point_drawing(bool value) = 0;

        virtual bool is_line_drawing() const = 0;
        virtual void set_is_line_drawing(bool value) = 0;

        virtual const std::array<float, 4>& get_edge_color() const = 0;
        virtual void set_edge_color(const std::array<float, 4>& color) = 0;

        virtual float get_edge_scale() const = 0;
        virtual void set_edge_scale(float scale) = 0;

        virtual int32_t get_texture_index() const = 0;
        virtual void set_texture_index(int32_t index) = 0;

        virtual environment_blend_mode get_environment_blend_mode() const = 0;
        virtual void set_environment_blend_mode(environment_blend_mode mode) = 0;

        virtual int32_t get_environment_index() const = 0;
        virtual void set_environment_index(int32_t index) = 0;

        virtual toon_reference_type get_toon_reference() const = 0;
        virtual void set_toon_reference(toon_reference_type reference) = 0;

        virtual int32_t get_toon_part() const = 0;
        virtual void set_toon_part(int32_t part) = 0;

        virtual int8_t get_toon_internal() const = 0;
        virtual void set_toon_internal(int8_t internal) = 0;

        virtual std::string get_meta_data() const = 0;
        virtual void set_meta_data(const std::string& data) = 0;

        virtual int32_t get_surface_count() const = 0;
        virtual void set_surface_count(int32_t count) = 0;
    };

    class pmx_IK_link
    {
    public:
        virtual ~pmx_IK_link() = default;

        virtual int32_t get_bone_index() const = 0;
        virtual void set_bone_index(int32_t index) = 0;

        virtual bool have_limits() const = 0;
        virtual void set_have_limits(bool value) = 0;

        virtual const std::array<float, 3>& get_limit_min() const = 0;
        virtual void set_limit_min(const std::array<float, 3>& min) = 0;

        virtual const std::array<float, 3>& get_limit_max() const = 0;
        virtual void set_limit_max(const std::array<float, 3>& max) = 0;
    };

    class pmx_bone
    {
    public:
        virtual ~pmx_bone() = default;

        using pmx_IK_link_array = pmx_element_array<pmx_IK_link>;

        virtual std::string get_bone_name_local() const = 0;
        virtual void set_bone_name_local(const std::string& name) = 0;

        virtual std::string get_bone_name_universal() const = 0;
        virtual void set_bone_name_universal(const std::string& name) = 0;

        virtual const std::array<float, 3>& get_position() const = 0;
        virtual void set_position(const std::array<float, 3>& position) = 0;

        virtual int32_t get_parent_bone_index() const = 0;
        virtual void set_parent_bone_index(int32_t index) = 0;

        virtual int32_t get_layer() const = 0;
        virtual void set_layer(int32_t layer) = 0;

        virtual bool is_rotatable() const = 0;
        virtual void set_is_rotatable(bool value) = 0;

        virtual bool is_translatable() const = 0;
        virtual void set_is_translatable(bool value) = 0;

        virtual bool is_visible() const = 0;
        virtual void set_is_visible(bool value) = 0;

        virtual bool is_enabled() const = 0;
        virtual void set_is_enabled(bool value) = 0;

        virtual bool is_physics_after_deform() const = 0;
        virtual void set_is_physics_after_deform(bool value) = 0;

        virtual bool have_external_parent_deform() const = 0;
        virtual void set_have_external_parent_deform(bool value) = 0;

        virtual bool is_tail_use_index() const = 0;
        virtual void set_is_tail_use_index(bool value) = 0;

        virtual const std::array<float, 3>& get_tail_position() const = 0;
        virtual void set_tail_position(const std::array<float, 3>& position) = 0;

        virtual int32_t get_tail_index() const = 0;
        virtual void set_tail_index(int32_t index) = 0;

        virtual bool have_inherit_rotation() const = 0;
        virtual void set_have_inherit_rotation(bool value) = 0;

        virtual bool have_inherit_translation() const = 0;
        virtual void set_have_inherit_translation(bool value) = 0;

        virtual int32_t get_inherit_bone_parent_index() const = 0;
        virtual void set_inherit_bone_parent_index(int32_t index) = 0;

        virtual float get_inherit_bone_parent_influence() const = 0;
        virtual void set_inherit_bone_parent_influence(float influence) = 0;

        virtual bool have_fixed_axis() const = 0;
        virtual void set_have_fixed_axis(bool value) = 0;

        virtual const std::array<float, 3>& get_bone_fixed_axis() const = 0;
        virtual void set_bone_fixed_axis(const std::array<float, 3>& axis) = 0;

        virtual bool have_local_coordinate() const = 0;
        virtual void set_have_local_coordinate(bool value) = 0;

        virtual const std::array<float, 3>& get_bone_local_coordinate_x() const = 0;
        virtual void set_bone_local_coordinate_x(const std::array<float, 3>& x) = 0;

        virtual const std::array<float, 3>& get_bone_local_coordinate_z() const = 0;
        virtual void set_bone_local_coordinate_z(const std::array<float, 3>& z) = 0;

        virtual bool is_IK() const = 0;
        virtual void set_is_IK(bool value) = 0;

        virtual int32_t get_IK_target_index() const = 0;
        virtual void set_IK_target_index(int32_t index) = 0;

        virtual int32_t get_IK_loop_count() const = 0;
        virtual void set_IK_loop_count(int32_t count) = 0;

        virtual float get_IK_limit_radian() const = 0;
        virtual void set_IK_limit_radian(float radian) = 0;

        virtual const pmx_IK_link_array& get_IK_link_array() const = 0;
        virtual pmx_IK_link_array& mutable_IK_link_array() = 0;
    };

    class pmx_morph_offset
    {
    public:
        virtual ~pmx_morph_offset() = default;
    };

    class pmx_group_morph_offset : public pmx_morph_offset
    {
    public:
        ~pmx_group_morph_offset() override = default;

        virtual int32_t get_morph_index() const = 0;
        virtual void set_morph_index(int32_t morph_index) = 0;

        virtual float get_morph_weight() const = 0;
        virtual void set_morph_weight(float morph_weight) = 0;
    };

    class pmx_vertex_morph_offset : public pmx_morph_offset
    {
    public:
        ~pmx_vertex_morph_offset() override = default;

        virtual uint32_t get_vertex_index() const = 0;
        virtual void set_vertex_index(uint32_t vertex_index) = 0;

        virtual const std::array<float, 3>& get_offset_position() const = 0;
        virtual void set_offset_position(const std::array<float, 3>& offset_position) = 0;
    };

    class pmx_bone_morph_offset : public pmx_morph_offset
    {
    public:
        ~pmx_bone_morph_offset() override = default;

        virtual int32_t get_bone_index() const = 0;
        virtual void set_bone_index(int32_t bone_index) = 0;

        virtual const std::array<float, 3>& get_bone_translation() const = 0;
        virtual void set_bone_translation(const std::array<float, 3>& bone_translation) = 0;

        virtual const std::array<float, 3>& get_bone_rotation() const = 0;
        virtual void set_bone_rotation(const std::array<float, 3>& bone_rotation) = 0;
    };

    class pmx_uv_morph_offset : public pmx_morph_offset
    {
    public:
        ~pmx_uv_morph_offset() override = default;

        virtual uint32_t get_vertex_index() const = 0;
        virtual void set_vertex_index(uint32_t vertex_index) = 0;

        virtual const std::array<float, 4>& get_uv_offset() const = 0;
        virtual void set_uv_offset(const std::array<float, 4>& offset) = 0;
    };

    class pmx_material_morph_offset : public pmx_morph_offset
    {
    public:
        ~pmx_material_morph_offset() override = default;

        enum class blend_mode : int8_t
        {
            MULTIPLY = 0,
            ADD
        };

        virtual int32_t get_material_index() const = 0;
        virtual void set_material_index(int32_t index) = 0;

        virtual blend_mode get_blend_mode() const = 0;
        virtual void set_blend_mode(const blend_mode& mode) = 0;

        virtual const std::array<float, 4>& get_diffuse() const = 0;
        virtual void set_diffuse(const std::array<float, 4>& diffuse) = 0;

        virtual const std::array<float, 3>& get_specular() const = 0;
        virtual void set_specular(const std::array<float, 3>& specular) = 0;

        virtual float get_specular_intensity() const = 0;
        virtual void set_specular_intensity(float specular_intensity) = 0;

        virtual const std::array<float, 3>& get_ambient() const = 0;
        virtual void set_ambient(const std::array<float, 3>& ambient) = 0;

        virtual const std::array<float, 4>& get_edge_color() const = 0;
        virtual void set_edge_color(const std::array<float, 4>& edge_color) = 0;

        virtual float get_edge_size() const = 0;
        virtual void set_edge_size(float edge_size) = 0;

        virtual const std::array<float, 4>& get_texture_tint() const = 0;
        virtual void set_texture_tint(const std::array<float, 4>& texture_tint) = 0;

        virtual const std::array<float, 4>& get_environment_tint() const = 0;
        virtual void set_environment_tint(const std::array<float, 4>& environment_tint) = 0;

        virtual const std::array<float, 4>& get_toon_tint() const = 0;
        virtual void set_toon_tint(const std::array<float, 4>& toon_tint) = 0;
    };

    class pmx_flip_morph_offset : public pmx_morph_offset
    {
    public:
        ~pmx_flip_morph_offset() override = default;

        virtual int32_t get_morph_index() const = 0;
        virtual void set_morph_index(int32_t morph_index) = 0;

        virtual float get_morph_weight() const = 0;
        virtual void set_morph_weight(float morph_weight) = 0;
    };

    class pmx_impulse_morph_offset : public pmx_morph_offset
    {
    public:
        ~pmx_impulse_morph_offset() override = default;

        virtual int32_t get_rigid_body_index() const = 0;
        virtual void set_rigid_body_index(int32_t rigid_body_index) = 0;

        virtual bool is_local() const = 0;
        virtual void set_local(bool value) = 0;

        virtual const std::array<float, 3>& get_movement_speed() const = 0;
        virtual void set_movement_speed(const std::array<float, 3>& speed) = 0;

        virtual const std::array<float, 3>& get_rotation_torque() const = 0;
        virtual void set_rotation_torque(const std::array<float, 3>& torque) = 0;
    };

    class pmx_morph
    {
    public:
        virtual ~pmx_morph() = default;

        enum class display_panel_type : int8_t
        {
            EYE = 1,
            MOUTH,
            EYEBROW,
            OTHER
        };

        enum class morph_type : int8_t
        {
            GROUP,      // Group
            VERTEX,     // Vertex
            BONE,       //Bone
            UV0,        // UV
            UV1,        // expansion UV1
            UV2,        // expansion UV2
            UV3,        // expansion UV3
            UV4,        // expansion UV4
            MATERIAL,   // Material
            FLIP,       // Flip
            IMPULSE     // Impulse
        };

        using pmx_morph_offset_array = pmx_element_array<pmx_morph_offset>;

        virtual std::string get_morph_name_local() const = 0;
        virtual void set_morph_name_local(const std::string& name) = 0;

        virtual std::string get_morph_name_universal() const = 0;
        virtual void set_morph_name_universal(const std::string& name) = 0;

        virtual display_panel_type get_display_panel_type() const = 0;
        virtual void set_display_panel_type(display_panel_type type) = 0;

        virtual morph_type get_morph_offset_type() const = 0;
        virtual void set_morph_offset_type(morph_type type) = 0;

        virtual const pmx_morph_offset_array& get_morph_offset_array() const = 0;
        virtual pmx_morph_offset_array& mutable_morph_offset_array() = 0;
    };

    class pmx_display_item
    {
    public:
        virtual ~pmx_display_item() = default;

        enum class display_frame_type : int8_t
        {
            BONE = 0,
            MORPH,
        };

        virtual display_frame_type get_frame_type() const = 0;
        virtual void set_frame_type(display_frame_type type) = 0;

        virtual int32_t get_index() const = 0;
        virtual void set_index(int32_t data) = 0;
    };

    class pmx_display_frame
    {
    public:
        virtual ~pmx_display_frame() = default;

        enum class special_flag : int8_t
        {
            NORMAL_FRAME,
            SPECIAL_FRAME
        };

        using pmx_display_item_array = pmx_element_array<pmx_display_item>;

        virtual std::string get_display_name_local() const = 0;
        virtual void set_display_name_local(const std::string& name) = 0;

        virtual std::string get_display_name_universal() const = 0;
        virtual void set_display_name_universal(const std::string& name) = 0;

        virtual special_flag get_special_flag() const = 0;
        virtual void set_special_flag(special_flag flag) = 0;

        virtual const pmx_display_item_array& get_display_item_array() const = 0;
        virtual pmx_display_item_array& mutable_display_item_array() = 0;
    };

    class pmx_rigid_body_non_collision_group
    {
    public:
        virtual ~pmx_rigid_body_non_collision_group() = default;

        virtual bool in_group_0() const = 0;
        virtual void set_in_group_0(bool value) = 0;

        virtual bool in_group_1() const = 0;
        virtual void set_in_group_1(bool value) = 0;

        virtual bool in_group_2() const = 0;
        virtual void set_in_group_2(bool value) = 0;

        virtual bool in_group_3() const = 0;
        virtual void set_in_group_3(bool value) = 0;

        virtual bool in_group_4() const = 0;
        virtual void set_in_group_4(bool value) = 0;

        virtual bool in_group_5() const = 0;
        virtual void set_in_group_5(bool value) = 0;

        virtual bool in_group_6() const = 0;
        virtual void set_in_group_6(bool value) = 0;

        virtual bool in_group_7() const = 0;
        virtual void set_in_group_7(bool value) = 0;

        virtual bool in_group_8() const = 0;
        virtual void set_in_group_8(bool value) = 0;

        virtual bool in_group_9() const = 0;
        virtual void set_in_group_9(bool value) = 0;

        virtual bool in_group_10() const = 0;
        virtual void set_in_group_10(bool value) = 0;

        virtual bool in_group_11() const = 0;
        virtual void set_in_group_11(bool value) = 0;

        virtual bool in_group_12() const = 0;
        virtual void set_in_group_12(bool value) = 0;

        virtual bool in_group_13() const = 0;
        virtual void set_in_group_13(bool value) = 0;

        virtual bool in_group_14() const = 0;
        virtual void set_in_group_14(bool value) = 0;

        virtual bool in_group_15() const = 0;
        virtual void set_in_group_15(bool value) = 0;

        virtual bool& operator[](uint8_t group_index) = 0;
    };

    class pmx_rigid_body
    {
    public:
        virtual ~pmx_rigid_body() = default;

        enum class shape_type : int8_t
        {
            SPHERE,
            BOX,
            CAPSULE
        };

        enum class physics_mode : int8_t
        {
            TRACKING_BONES,
            PHYSICAL_CALCULUS,
            PHYSICS_AND_BONE
        };

        virtual std::string get_rigid_body_name_local() const = 0;
        virtual void set_rigid_body_name_local(const std::string& name) = 0;

        virtual std::string get_rigid_body_name_universal() const = 0;
        virtual void set_rigid_body_name_universal(const std::string& name) = 0;

        virtual int32_t get_related_bone_index() const = 0;
        virtual void set_related_bone_index(int32_t index) = 0;

        virtual int8_t get_group_id() const = 0;
        virtual void set_group_id(int8_t id) = 0;

        virtual const pmx_rigid_body_non_collision_group& get_non_collision_group() const = 0;
        virtual pmx_rigid_body_non_collision_group& mutable_non_collision_group() = 0;

        virtual shape_type get_shape_type() const = 0;
        virtual void set_shape_type(shape_type type) = 0;

        virtual const std::array<float, 3>& get_shape_size() const = 0;
        virtual void set_shape_size(const std::array<float, 3>& size) = 0;

        virtual const std::array<float, 3>& get_shape_position() const = 0;
        virtual void set_shape_position(const std::array<float, 3>& position) = 0;

        virtual const std::array<float, 3>& get_shape_rotation() const = 0;
        virtual void set_shape_rotation(const std::array<float, 3>& rotation) = 0;

        virtual float get_mass() const = 0;
        virtual void set_mass(float mass) = 0;

        virtual float get_move_attenuation() const = 0;
        virtual void set_move_attenuation(float attenuation) = 0;

        virtual float get_rotation_damping() const = 0;
        virtual void set_rotation_damping(float damping) = 0;

        virtual float get_repulsion_force() const = 0;
        virtual void set_repulsion_force(float force) = 0;

        virtual float get_friction_force() const = 0;
        virtual void set_friction_force(float force) = 0;

        virtual physics_mode get_physics_mode() const = 0;
        virtual void set_physics_mode(physics_mode mode) = 0;
    };

    class pmx_joint
    {
    public:
        virtual ~pmx_joint() = default;

        enum class joint_type : int8_t
        {
            SPRING_6DOF,
            NON_SPRING_6DOF,
            P2P,
            CONE_TWIST,
            SLIDER,
            HINGE
        };

        virtual std::string get_joint_name_local() const = 0;
        virtual void set_joint_name_local(const std::string& name) = 0;

        virtual std::string get_joint_name_universal() const = 0;
        virtual void set_joint_name_universal(const std::string& name) = 0;

        virtual joint_type get_joint_type() const = 0;
        virtual void set_joint_type(joint_type type) = 0;

        virtual int32_t get_rigid_body_index_a() const = 0;
        virtual void set_rigid_body_index_a(int32_t index) = 0;

        virtual int32_t get_rigid_body_index_b() const = 0;
        virtual void set_rigid_body_index_b(int32_t index) = 0;

        virtual const std::array<float, 3>& get_position() const = 0;
        virtual void set_position(const std::array<float, 3>& position) = 0;

        virtual const std::array<float, 3>& get_rotation() const = 0;
        virtual void set_rotation(const std::array<float, 3>& rotation) = 0;

        virtual const std::array<float, 3>& get_position_minimum() const = 0;
        virtual void set_position_minimum(const std::array<float, 3>& minimum) = 0;

        virtual const std::array<float, 3>& get_position_maximum() const = 0;
        virtual void set_position_maximum(const std::array<float, 3>& maximum) = 0;

        virtual const std::array<float, 3>& get_rotation_minimum() const = 0;
        virtual void set_rotation_minimum(const std::array<float, 3>& minimum) = 0;

        virtual const std::array<float, 3>& get_rotation_maximum() const = 0;
        virtual void set_rotation_maximum(const std::array<float, 3>& maximum) = 0;

        virtual const std::array<float, 3>& get_position_spring() const = 0;
        virtual void set_position_spring(const std::array<float, 3>& spring) = 0;

        virtual const std::array<float, 3>& get_rotation_spring() const = 0;
        virtual void set_rotation_spring(const std::array<float, 3>& spring) = 0;
    };

    class pmx_model {
    public:
        virtual ~pmx_model();

        enum class pmx_version : int8_t
        {
            V2_0,
            V2_1
        };

        using pmx_vertex_array = pmx_element_array<pmx_vertex>;
        using pmx_surface_array = pmx_element_array<pmx_surface>;
        using pmx_texture_array = pmx_element_array<pmx_texture>;
        using pmx_material_array = pmx_element_array<pmx_material>;
        using pmx_bone_array = pmx_element_array<pmx_bone>;
        using pmx_morph_array = pmx_element_array<pmx_morph>;
        using pmx_display_frame_array = pmx_element_array<pmx_display_frame>;
        using pmx_rigid_body_array = pmx_element_array<pmx_rigid_body>;
        using pmx_joint_array = pmx_element_array<pmx_joint>;

        virtual bool read_from_file(const std::string& file_name);
        virtual bool write_to_file(const std::string& file_name) const;

        virtual bool read_from_file(const std::wstring& file_name);
        virtual bool write_to_file(const std::wstring& file_name) const;

        virtual pmx_version get_version() const = 0;
        virtual void set_version(pmx_version version) = 0;

        virtual std::string get_model_name_local() const = 0;
        virtual void set_model_name_local(const std::string& name) = 0;

        virtual std::string get_model_name_universal() const = 0;
        virtual void set_model_name_universal(const std::string& name) = 0;

        virtual std::string get_comments_local() const = 0;
        virtual void set_comments_local(const std::string& comments) = 0;

        virtual std::string get_comments_universal() const = 0;
        virtual void set_comments_universal(const std::string& comments) = 0;

        virtual const pmx_vertex_array& get_pmx_vertex_array() = 0;
        virtual pmx_vertex_array& mutable_pmx_vertex_array() = 0;

        virtual const pmx_surface_array& get_pmx_surface_array() = 0;
        virtual pmx_surface_array& mutable_pmx_surface_array() = 0;

        virtual const pmx_texture_array& get_pmx_texture_array() = 0;
        virtual pmx_texture_array& mutable_pmx_texture_array() = 0;

        virtual const pmx_material_array& get_pmx_material_array() = 0;
        virtual pmx_material_array& mutable_pmx_material_array() = 0;

        virtual const pmx_bone_array& get_pmx_bone_array() = 0;
        virtual pmx_bone_array& mutable_pmx_bone_array() = 0;

        virtual const pmx_morph_array& get_pmx_morph_array() = 0;
        virtual pmx_morph_array& mutable_pmx_morph_array() = 0;

        virtual const pmx_display_frame_array& get_pmx_display_frame_array() = 0;
        virtual pmx_display_frame_array& mutable_pmx_display_frame_array() = 0;

        virtual const pmx_rigid_body_array& get_pmx_rigid_body_array() = 0;
        virtual pmx_rigid_body_array& mutable_pmx_rigid_body_array() = 0;

        virtual const pmx_joint_array& get_pmx_joint_array() = 0;
        virtual pmx_joint_array& mutable_pmx_joint_array() = 0;
    };

    static pmx_model* create_pmx_model();
}