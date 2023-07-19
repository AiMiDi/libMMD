# Copyright:Copyright(c) 2023-present, Aimidi & contributors.

cmake_minimum_required (VERSION 3.8)

project("libMMD" 
        VERSION 0.0.2
        HOMEPAGE_URL "https://github.com/AiMiDi/libMMD"
        LANGUAGES CXX)

option(USE_SHARED_LIB "use shared lib" OFF)

# Definitions
if (USE_SHARED_LIB)
    add_definitions(-DLIBMMD_SHARED_BUILD)
endif()

if (NOT DEFINED CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "_debug")
endif()

if(MSVC)
  add_compile_options(/MP)
endif()

# Add the source code to the executable file for this project.
file(GLOB_RECURSE SRC_LIST "source/*.cpp" "source/*.hpp" "source/*.c")

# target
set(TARGET_NAME libMMD)

add_library(${TARGET_NAME} ${SRC_LIST})
  set_target_properties(
     ${TARGET_NAME} 
     PROPERTIES 
     VERSION ${PROJECT_VERSION} 
     SOVERSION ${PROJECT_VERSION} 
     DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")

if (NOT USE_SHARED_LIB)
   set_target_properties(
	   ${TARGET_NAME} 
	   PROPERTIES 
	   INTERFACE_COMPILE_DEFINITIONS "LIBMMD_STATIC")
  if(MSVC)
    set_target_properties(
      ${TARGET_NAME}
      PROPERTIES
      DEBUG_POSTFIX "_static${CMAKE_DEBUG_POSTFIX}"
      MINSIZEREL_POSTFIX "_static"
      RELEASE_POSTFIX "_static"
      RELWITHDEBINFO_POSTFIX "_static"
    )
  endif()
endif()

# add libicu::uc dependencies.
add_dependencies(${TARGET_NAME} icu::uc)
set(ICU_STATIC ON)
set(ICU_BUILD_UC ON)
set(ICU_BUILD_I18N OFF)
set(ICU_BUILD_IO OFF)
set(ICU_NO_INSTALL ON)
add_subdirectory("thirdparty/icu")
target_link_libraries(${TARGET_NAME} PRIVATE icu::uc)
target_include_directories(${TARGET_NAME} PRIVATE ${ICU_INCLUDEDIR})

# add yalantinglibs::easylog dependencies.
add_dependencies(${TARGET_NAME} yalantinglibs::easylog)
set(BUILD_BENCHMARK OFF)
set(BUILD_EXAMPLES OFF)
set(BUILD_UNIT_TESTS OFF)
set(Build_ylt_struct_pb OFF)
add_subdirectory("thirdparty/yalantinglibs")
target_link_libraries(${TARGET_NAME} PRIVATE yalantinglibs::easylog)

# set compile feature C++20
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)

# add precompile header
target_precompile_headers(${TARGET_NAME} PRIVATE "source/pch.h")

# add public include for interface
target_include_directories(${TARGET_NAME} PUBLIC "include")

# add private include for internal implementation
target_include_directories(${TARGET_NAME} PRIVATE "source/core/public")
target_include_directories(${TARGET_NAME} PRIVATE "source/assets/public")
