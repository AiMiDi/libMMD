#pragma once

#ifdef _MSC_VER 
#define	LIBMMD_ATTRIBUTE_FORCE_INLINE __forceinline
#else
#define	LIBMMD_ATTRIBUTE_FORCE_INLINE inline __attribute__((always_inline))
#endif


