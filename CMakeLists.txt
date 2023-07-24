# Copyright:Copyright(c) 2023-present, Aimidi & contributors.

cmake_minimum_required (VERSION 3.8)

project("libMMD" 
        VERSION 0.0.2
        HOMEPAGE_URL "https://github.com/AiMiDi/libMMD"
        LANGUAGES CXX)

option(USE_SHARED_LIB "use shared lib" OFF)

set(LIBMMD_VERSION 0.1.0)


# ----- Setting -----

if (USE_SHARED_LIB)
    add_definitions(-DLIBMMD_SHARED_BUILD)
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
  set(CMAKE_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/include/libMMD")
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

# add icu::uc dependencies.
set(ICU_BUILD_UC ON)
set(ICU_BUILD_I18N OFF)
set(ICU_BUILD_IO OFF)
add_subdirectory("thirdparty/icu")
target_link_libraries(${TARGET_NAME} PRIVATE icu::uc)

# add yalantinglibs::easylog dependencies.
target_include_directories(${TARGET_NAME} PRIVATE "thirdparty/yalantinglibs/include")

# set compile feature C++20
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)

# add precompile header
target_precompile_headers(${TARGET_NAME} PRIVATE "source/pch.h")

# add public include for interface
target_include_directories(${TARGET_NAME} PRIVATE "include/libMMD")

# add private include for internal implementation
target_include_directories(${TARGET_NAME} PRIVATE "source/core/public")
target_include_directories(${TARGET_NAME} PRIVATE "source/assets/public")


# ----- Install -----

install(
    TARGETS ${TARGET_NAME}
    EXPORT "${TARGET_NAME}Target"
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

# Installation header file
install(
  DIRECTORY "/include"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

#Export target
export(
  EXPORT "${TARGET_NAME}Target"
  NAMESPACE "${TARGET_NAME}::"
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}Target.cmake"
)

#Install Targets.cmake
install(
  EXPORT "${TARGET_NAME}Target"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake"
  NAMESPACE "${TARGET_NAME}::"
  FILE "${TARGET_NAME}Target.cmake"
)

#Export config.cmake
include(CMakePackageConfigHelpers)
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake"
)
#Install config.cmake
install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake"
)