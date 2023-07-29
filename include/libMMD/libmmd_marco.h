#pragma once

#ifdef _MSC_VER 
#define	LIBMMD_ATTRIBUTE_FORCE_INLINE __forceinline
#else
#define	LIBMMD_ATTRIBUTE_FORCE_INLINE inline __attribute__((always_inline))
#endif

#ifdef LIBMMD_EXPORT
/* Use the predefined value. */
#elif defined(LIBMMD_STATIC)
#   define LIBMMD_EXPORT
#elif defined(_MSC_VER)
#   define LIBMMD_EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
#   define LIBMMD_EXPORT __attribute__((visibility("default")))
#elif (defined(__SUNPRO_CC) && __SUNPRO_CC >= 0x550) \
   || (defined(__SUNPRO_C) && __SUNPRO_C >= 0x550) 
#   define LIBMMD_EXPORT __global
#elif defined(__HP_aCC) || defined(__HP_cc)
#   define LIBMMD_EXPORT __declspec(dllexport)
#else
#   define LIBMMD_EXPORT
#endif

#ifdef LIBMMD_IMPORT
/* Use the predefined value. */
#elif defined(LIBMMD_STATIC)
#   define LIBMMD_IMPORT
#elif defined(_MSC_VER)
/* Windows needs to export/import data. */
#   define LIBMMD_IMPORT __declspec(dllimport)
#else
#   define LIBMMD_IMPORT 
#endif

#if LIBMMD_BUILD_SHARED
#define LIBMMD_API LIBMMD_EXPORT
#elif defined(LIBMMD_STATIC)
#define LIBMMD_API
#else
#define LIBMMD_API LIBMMD_IMPORT
#endif

#ifdef LIBMMD_USE_NODISCARD
#  ifdef __has_cpp_attribute
#    if __has_cpp_attribute(nodiscard)
#      define LIBMMD_NODISCARD [[nodiscard]]
#    else
#      define LIBMMD_NODISCARD
#    endif
#  else
#    define LIBMMD_NODISCARD
#  endif
#else
#  define LIBMMD_NODISCARD
#endif
