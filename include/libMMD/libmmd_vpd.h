/**************************************************************************

Copyright:Copyright(c) 2023-present, Aimidi & libmmd contributors.
Author:			Aimidi
Date:			2023/7/10
File:			vpd_post.h
Description:	vpd data interface

**************************************************************************/
#pragma once

#include "libmmd_marco.h"
#include "libmmd_assets_file.h"
#include <string>
#include <array>

namespace libmmd
{
    template<typename T>
    class LIBMMD_API vpd_element_array
    {
    protected:
        using vpd_element_type = T;
    public:
        virtual ~vpd_element_array() = default;

		LIBMMD_NODISCARD virtual uint64_t size() const = 0;

		LIBMMD_NODISCARD virtual const vpd_element_type& operator[](uint64_t index) const = 0;
        virtual vpd_element_type& operator[](uint64_t index) = 0;

        virtual vpd_element_type& add() = 0;
        virtual bool remove(uint64_t index, uint64_t count = 1) = 0;
    };

    class LIBMMD_API vpd_bone_post
    {
    public:
        virtual ~vpd_bone_post() = default;

        virtual std::string get_bone_name() = 0;
        virtual void set_bone_name(const std::string& name) = 0;

		LIBMMD_NODISCARD virtual const std::array<float, 3>& get_position() const = 0;
        virtual void set_position(const std::array<float, 3>& position) = 0;

		LIBMMD_NODISCARD virtual const std::array<float, 4>& get_rotation() const = 0;
        virtual void set_rotation(const std::array<float, 4>& rotation) = 0;
    };

    class LIBMMD_API vpd_morph_post
    {
    public:
        virtual ~vpd_morph_post() = default;

        virtual std::string get_morph_name() = 0;
        virtual void set_morph_name(const std::string& name) = 0;

		LIBMMD_NODISCARD virtual float get_weight() const = 0;
        virtual void set_weight(float weight) = 0;
    };

    class LIBMMD_API vpd_post : virtual public mmd_assets_file
    {
    protected:
		vpd_post() = default;
    public:
	    ~vpd_post() override = default;

        using vpd_bone_post_array = vpd_element_array<vpd_bone_post>;
        using vpd_morph_post_array = vpd_element_array<vpd_morph_post>;

		static vpd_post* create();
		static void free(vpd_post* post);

		LIBMMD_NODISCARD virtual std::string get_model_name() const = 0;
        virtual void set_model_name(const std::string& name) = 0;

		LIBMMD_NODISCARD virtual const vpd_bone_post_array& get_vpd_bone_post_array() const = 0;
        virtual vpd_bone_post_array& mutable_vpd_bone_post_array() = 0;

		LIBMMD_NODISCARD virtual const vpd_morph_post_array& get_vpd_morph_post_array() const = 0;
        virtual vpd_morph_post_array& mutable_vpd_morph_post_array() = 0;
    };
}
