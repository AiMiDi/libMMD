/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/9/19
File:			pmx_display.h
Description:	pmx display data

**************************************************************************/

#pragma once

#include "pmx_model_info.h"
#include "pmx_element_array.hpp"

namespace libmmd
{

	class pmx_display_item_impl final : public pmx_display_item, pmx_element_impl
	{
		// Frame type: 0: index - bone, 1: index - morph
		display_frame_type	frame_type_ = display_frame_type::BONE;
		// Index
		Int32	index_ = 0;
	public:
		/**
		 * \brief Default constructor function
		 */
		pmx_display_item_impl(const pmx_model_description* model_info) : pmx_element_impl(model_info){}
		/**
		 * \brief Destructor function
		 */
		~pmx_display_item_impl() override = default;
		/**
		 * \brief Copy constructor function
		 */
		pmx_display_item_impl(const pmx_display_item_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		pmx_display_item_impl& operator =(const pmx_display_item_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		pmx_display_item_impl(pmx_display_item_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		pmx_display_item_impl& operator =(pmx_display_item_impl&&) noexcept = default;

		[[nodiscard]] display_frame_type get_frame_type() const override;

		void set_frame_type(display_frame_type type) override;

		[[nodiscard]] int32_t get_index() const override;

		void set_index(int32_t data) override;

		/**
		 * \brief Read PMX display item data from a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write PMX display item data to a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		auto write_to_file(const file& file) const -> bool override;
	};

	class pmx_display_frame_impl : public pmx_display_frame, pmx_element_impl
	{
		// Represents a local name
		std::u8string	display_name_local_{};
		// Indicates the common name of the beam
		std::u8string	display_name_universal_{};
		// 0 represents normal frame and 1 represents special frame
		special_flag	special_flag_ = special_flag::NORMAL_FRAME;
		// Frame data
		pmx_element_array_impl<pmx_display_item, pmx_display_item_impl> frame_item_array_{ model_description_ };
	public:
		/**
		 * \brief Default constructor function
		 */
		pmx_display_frame_impl(const pmx_model_description* model_info) : pmx_element_impl(model_info) {}
		/**
		 * \brief Destructor function
		 */
		~pmx_display_frame_impl() override = default;
		/**
		 * \brief Copy constructor function
		 */
		pmx_display_frame_impl(const pmx_display_frame_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		pmx_display_frame_impl& operator =(const pmx_display_frame_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		pmx_display_frame_impl(pmx_display_frame_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		pmx_display_frame_impl& operator =(pmx_display_frame_impl&&) noexcept = default;

		[[nodiscard]] std::string get_display_name_local() const override;

		void set_display_name_local(const std::string& name) override;

		[[nodiscard]] std::string get_display_name_universal() const override;

		void set_display_name_universal(const std::string& name) override;

		[[nodiscard]] special_flag get_special_flag() const override;

		void set_special_flag(special_flag flag) override;

		[[nodiscard]] const pmx_display_item_array& get_display_item_array() const override;

		pmx_display_item_array& mutable_display_item_array() override;

		/**
		 * \brief Read PMX display data from a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write PMX display  data to a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		auto write_to_file(const file& file) const -> bool override;
	};
}