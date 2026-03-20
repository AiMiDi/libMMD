cmake_minimum_required(VERSION 3.10...3.27)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(libMMD CXX)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
option (LIBMMD_BUILD_MMD2OBJ "Build mmd2obj." off)
if (LIBMMD_BUILD_MMD2OBJ)
    option (LIBMMD_BUILD_OBJ_MODEL "Build obj model." on)
else()
    option (LIBMMD_BUILD_OBJ_MODEL "Build obj model." off)
endif()
option (LIBMMD_BUILD_X_MODEL "Build x model." on)
set (LIBMMD_BULLET_ROOT "" CACHE PATH "Bullet Root Directory")
set (LIBMMD_BULLET_SOURCE_DIR "" CACHE PATH "Bullet source root when LIBMMD_BULLET_USE_SUBDIRECTORY is ON (e.g. dependency/bullet3)")
option (LIBMMD_BULLET_USE_SUBDIRECTORY "Bullet provided by parent add_subdirectory (targets LinearMath, BulletDynamics, ...)" OFF)
set (LIBMMD_EIGEN_ROOT "${PROJECT_SOURCE_DIR}/external/eigen" CACHE PATH "Eigen Root Directory")
option (LIBMMD_ENABLE_TEST "Enable CTest." on)
option (LIBMMD_INSTALL "libMMD install." off)
option (LIBMMD_ENABLE_AVX2 "Enable AVX2 instruction set." off)

set (BULLET_ROOT ${LIBMMD_BULLET_ROOT})
MESSAGE( STATUS "BULLET_ROOT=${BULLET_ROOT}")

if (LIBMMD_BULLET_USE_SUBDIRECTORY)
  if (NOT LIBMMD_BULLET_SOURCE_DIR)
    message(FATAL_ERROR "libMMD: LIBMMD_BULLET_SOURCE_DIR must be set when LIBMMD_BULLET_USE_SUBDIRECTORY is ON")
  endif()
  set (BULLET_FOUND TRUE)
  # Same target names as BULLET_LIBRARIES in Bullet-generated BulletConfig.cmake
  set (BULLET_LIBRARIES LinearMath Bullet3Common BulletInverseDynamics BulletCollision BulletDynamics BulletSoftBody)
  set (BULLET_INCLUDE_DIRS "${LIBMMD_BULLET_SOURCE_DIR}/src")
  message(STATUS "libMMD: using Bullet from add_subdirectory (${LIBMMD_BULLET_SOURCE_DIR})")
else()
  find_package (Bullet REQUIRED)
endif()

MESSAGE( STATUS "BULLET_FOUND=${BULLET_FOUND}")
MESSAGE( STATUS "BULLET_INCLUDE_DIRS=${BULLET_INCLUDE_DIRS}")
MESSAGE( STATUS "BULLET_LIBRARIES=${BULLET_LIBRARIES}")

if (LIBMMD_ENABLE_TEST)
    enable_testing()
endif()

if (LIBMMD_ENABLE_EXAMPLE_VULKAN)
    find_package(Vulkan REQUIRED)
endif()

set(CMAKE_CXX_STANDARD 17)

include_directories(${BULLET_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/src)

if(WIN32)
    ADD_DEFINITIONS(-DUNICODE -D_UNICODE)
endif(WIN32)

if(MSVC)
    ADD_DEFINITIONS(/MP /utf-8)
    if(LIBMMD_ENABLE_AVX2)
        add_compile_options(/arch:AVX2)
    endif()
endif()

add_subdirectory(src)
if(LIBMMD_ENABLE_TEST)
    add_subdirectory(tests)
endif ()

if (LIBMMD_BUILD_MMD2OBJ)
    add_executable(mmd2obj mmd2obj.cpp)
    target_link_libraries(mmd2obj libMMD)
endif()

# Install
if (LIBMMD_INSTALL)
    install (DIRECTORY src/libMMD DESTINATION include  FILES_MATCHING PATTERN "*.h")
    # copy Eigen to include
    install (DIRECTORY ${LIBMMD_EIGEN_ROOT}/Eigen DESTINATION include)
    if (LIBMMD_BUILD_MMD2OBJ)
        install (TARGETS mmd2obj RUNTIME DESTINATION bin)
    endif()
endif()
