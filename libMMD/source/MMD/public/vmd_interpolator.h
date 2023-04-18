/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/11
File:			vmd_interpolator.h
Description:	MMD style animation interpolator

**************************************************************************/
#pragma once

namespace libmmd
{
	/**
	 * \brief MMD style animation interpolator
	 */
	class VMDInterpolator
	{
	protected:
		UChar m_ax = 20U;
		UChar m_ay = 20U;
		UChar m_bx = 107U;
		UChar m_by = 107U;
		bool  m_isLinear = true;
	public:
		/**
		 * \brief  Constructor function
		 */
		explicit VMDInterpolator(UChar ax = 20U, UChar ay = 20U, UChar bx = 107U, UChar by = 107U);
		/**
		 * \brief Copy constructor
		 */
		VMDInterpolator(const VMDInterpolator&) noexcept = default;
		/**
		 * \brief Move constructor
		 */
		VMDInterpolator(VMDInterpolator&&) noexcept;
		/**
		* \brief Copy operator=
		* \return Result reference
		*/
		VMDInterpolator& operator =(const VMDInterpolator&) = default;
		/**
		 * \brief Move operator=
		 * \return Result reference
		 */
		VMDInterpolator& operator =(VMDInterpolator&&) noexcept;

		bool operator==(const VMDInterpolator& other) const;
		/**
		* \brief Destructor function
		*/
		virtual ~VMDInterpolator() = default;
		/**
		 * \brief Get interpolator's C4D style tangent's value of right
		 * \return C4D style tangent's value of right
		 */
		[[nodiscard]] Float64 get_value_right() const;
		/**
		 * \brief Get interpolator's C4D style tangent's value of left
		 * \return C4D style tangent's value of left
		 */
		[[nodiscard]] Float64 get_value_left() const;
		/**
		 * \brief Set interpolator by C4D SplineData
		 * \return C4D style tangent of right
		 */
		[[nodiscard]] Vector2d64 get_tangent_right() const;
		/**
		 * \brief Set interpolator by C4D SplineData
		* \return C4D style tangent of left
		*/
		[[nodiscard]] Vector2d64 get_tangent_left() const;
		/**
		* \brief Set interpolator by 4 UChar
		*/
		void set(const UChar& ax = 20U, const UChar& ay = 20U, const UChar& bx = 107U, const UChar& by = 107U);
		/**
		* \brief Set interpolator by other interpolator
		*/
		void set(const VMDInterpolator& interpolator);
		/**
		* \brief Reset interpolator
		*/
		void reset();
		/**
		 * \brief Is the interpolator is linear
		 * \return TRUE is linear, other is FALSE
		 */
		[[nodiscard]] bool is_linear() const;
		/**
		 * \brief Read from a vmd file
		 * \param file vmd file
		 * \return Successful TRUE, other FALSE.
		 */
		virtual bool read(const file& file) = 0;
		/**
		 * \brief Write to vmd file
		 * \param file vmd file
		 * \return Successful TRUE, other FALSE.
		 */
		virtual bool write(const file& file) const = 0;

		friend struct std::hash<VMDInterpolator>;
	};

	class VMDBoneInterpolator final : public VMDInterpolator
	{
	public:
		/**
		* \brief Read from a vmd file
		* \param file vmd file
		* \return Successful TRUE, other FALSE.
		*/
		bool read(const file& file) override;
		/**
		* \brief Write to vmd file
		* \param file vmd file
		* \return Successful TRUE, other FALSE.
		*/
		bool write(const file& file) const override;
	};

	class VMDCameraInterpolator final : public VMDInterpolator
	{
	public:
		/**
		* \brief Read from a vmd file
		* \param file vmd file
		* \return Successful TRUE, other FALSE.
		*/
		bool read(const file& file) override;
		/**
		* \brief Write to vmd file
		* \param file vmd file
		* \return Successful TRUE, other FALSE.
		*/
		bool write(const file& file) const override;
	};
}

template<>
struct std::hash<libmmd::VMDInterpolator>
{
	size_t operator()(const libmmd::VMDInterpolator& rhs) const noexcept
	{
		return std::hash<int>()(rhs.m_ax) ^ std::hash<int>()(rhs.m_ay) ^ std::hash<int>()(rhs.m_bx) ^ std::hash<int>()(rhs.m_by) << 1;
	}
};
