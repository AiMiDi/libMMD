# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("libMMD" VERSION 0.0.1)

option(USE_SHARED_LIB "use shared lib" OFF)

# Definitions
if (USE_SHARED_LIB)
    add_definitions(-DLIBMMD_SHARED_BUILD)
endif()

if (NOT DEFINED CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "_Debug")
endif()

add_subdirectory ("libMMD")