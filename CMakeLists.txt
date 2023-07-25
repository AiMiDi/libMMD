# Copyright:Copyright(c) 2023-present, Aimidi & contributors.

cmake_minimum_required (VERSION 3.14)

project("libMMD" 
        VERSION 0.0.2
        HOMEPAGE_URL "https://github.com/AiMiDi/libMMD"
        LANGUAGES CXX)

set(LIBMMD_VERSION 0.1.0)


# ----- Setting -----

option(LIBMMD_BUILD_SHARED "Build shared library" OFF)
option(LIBMMD_INSTALL "Generate the install target" ON)

message(STATUS "Is build libMMD shared library: ${LIBMMD_BUILD_SHARED}")
message(STATUS "Is generate the libMMD install target: ${LIBMMD_INSTALL}")

if (LIBMMD_BUILD_SHARED)
  set(BUILD_SHARED_LIBS True)
endif()

if (NOT DEFINED CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "_debug")
endif()

if(NOT CMAKE_INSTALL_BINDIR)
  set(CMAKE_INSTALL_BINDIR "${CMAKE_INSTALL_PREFIX}/bin")
endif()

if(NOT CMAKE_INSTALL_LIBDIR)
  set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/lib")
endif()

if(NOT CMAKE_INSTALL_INCLUDEDIR)
  set(CMAKE_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/include")
endif()

if(MSVC)
  add_compile_options(/MP)
endif()


# ----- Find source file -----

# Add the source code to the executable file for this project.
file(GLOB_RECURSE SRC_LIST "source/*.cpp" "source/*.hpp" "source/*.c")


# ----- Target -----
set(TARGET_NAME libMMD)

add_library(${TARGET_NAME} ${SRC_LIST})
set_target_properties(
 ${TARGET_NAME} 
 PROPERTIES 
 VERSION ${PROJECT_VERSION} 
 SOVERSION ${PROJECT_VERSION} 
 DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")

if (NOT LIBMMD_BUILD_SHARED)
   target_compile_definitions(${TARGET_NAME} PRIVATE LIBMMD_STATIC=1)
   target_compile_definitions(${TARGET_NAME} PUBLIC LIBMMD_STATIC=1)
   set_target_properties(
	   ${TARGET_NAME} 
	   PROPERTIES 
	   INTERFACE_COMPILE_DEFINITIONS "LIBMMD_STATIC=1")
  if(MSVC)
    set_target_properties(
      ${TARGET_NAME}
      PROPERTIES
      DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}_static"
      MINSIZEREL_POSTFIX "_static"
      RELEASE_POSTFIX "_static"
      RELWITHDEBINFO_POSTFIX "_static"
    )
  endif()
else()
  target_compile_definitions(${TARGET_NAME} PRIVATE LIBMMD_BUILD_SHARED=1)
endif()


# add icu::uc dependencies.
set(ICU_BUILD_UC ON)
set(ICU_BUILD_I18N OFF)
set(ICU_BUILD_IO OFF)
set(ICU_INSTALL LIBMMD_INSTALL)
add_subdirectory("thirdparty/icu")
target_link_libraries(${TARGET_NAME} PRIVATE icu::uc)

# add yalantinglibs::easylog dependencies.
target_include_directories(${TARGET_NAME} PRIVATE "thirdparty/yalantinglibs/include")

# set compile feature C++20
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)

# add public include for interface
target_include_directories(${TARGET_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/libMMD>"
                                                 "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# add private include for internal implementation
target_include_directories(${TARGET_NAME} PRIVATE "source/core/public")
target_include_directories(${TARGET_NAME} PRIVATE "source/assets/public")

# add precompile header
target_precompile_headers(${TARGET_NAME} PRIVATE "source/pch.h")


# ----- Install -----
if(LIBMMD_INSTALL)

  message(STATUS "libMMD generating install")
  set(project_config_in "${CMAKE_CURRENT_LIST_DIR}/cmake/libMMDConfig.cmake.in")
  set(project_config_out "${CMAKE_CURRENT_BINARY_DIR}/libMMDConfig.cmake")
  set(version_config_file "${CMAKE_CURRENT_BINARY_DIR}/libMMDConfigVersion.cmake")
  set(config_targets_file "libMMDConfigTargets.cmake")
  set(export_dest_dir "${CMAKE_INSTALL_LIBDIR}/cmake/libMMD")

  install(
    TARGETS ${TARGET_NAME}
    EXPORT "${TARGET_NAME}Target"
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
  
  # Installation header file
  install(DIRECTORY "include/libMMD" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
  
  #Export target
  export(EXPORT "${TARGET_NAME}Target" NAMESPACE "${TARGET_NAME}::" FILE "${CMAKE_CURRENT_BINARY_DIR}/${config_targets_file}")
  
  #Install Targets.cmake
  install(EXPORT "${TARGET_NAME}Target" DESTINATION "${export_dest_dir}" NAMESPACE "${TARGET_NAME}::" FILE "${config_targets_file}")
  
  #Export config.cmake
  include(CMakePackageConfigHelpers)
  configure_package_config_file("${project_config_in}" "${project_config_out}" INSTALL_DESTINATION "${export_dest_dir}")
  write_basic_package_version_file("${version_config_file}" COMPATIBILITY SameMajorVersion)

  #Install config.cmake
  install(FILES "${project_config_out}" "${version_config_file}" DESTINATION "${export_dest_dir}")
endif()