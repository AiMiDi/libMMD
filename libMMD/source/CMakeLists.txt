# CMakeList.txt: libMMD 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 将源代码添加到此项目的可执行文件。
file(GLOB_RECURSE SRC_LIST "*.cpp" "*.c")

# Build
if (NOT DEFINED CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "_Debug")
endif()

if (USE_SHARED_LIB)
   add_library(libmmd ${SRC_LIST})
   set_target_properties(libmmd PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION} DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
else()
   add_library(libmmd STATIC ${SRC_LIST})
   set_target_properties(libmmd PROPERTIES INTERFACE_COMPILE_DEFINITIONS "LIBMMD_STATIC" DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
endif()

target_compile_features(libmmd PRIVATE cxx_std_20)

target_include_directories(libmmd PUBLIC "${PROJECT_SOURCE_DIR}/libMMD/dependency/install/include")
target_link_directories(libmmd PUBLIC "${PROJECT_SOURCE_DIR}/libMMD/dependency/install/lib")

target_precompile_headers(libmmd PRIVATE "pch.h")

add_subdirectory("core")
add_subdirectory("MMD")
