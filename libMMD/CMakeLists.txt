#Copyright:Copyright(c) 2023-present, Aimidi contributors.

cmake_minimum_required (VERSION 3.8)

# Add the source code to the executable file for this project.
file(GLOB_RECURSE SRC_LIST "source/*.cpp" "source/*.hpp" "source/*.c")

# target
if (USE_SHARED_LIB)
   add_library(libmmd ${SRC_LIST})
   set_target_properties(
	   libmmd 
	   PROPERTIES 
	   VERSION ${PROJECT_VERSION} 
	   SOVERSION ${PROJECT_VERSION} 
	   DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
else()
   add_library(libmmd STATIC ${SRC_LIST})
   set_target_properties(
	   libmmd 
	   PROPERTIES 
	   VERSION ${PROJECT_VERSION} 
	   SOVERSION ${PROJECT_VERSION}
	   INTERFACE_COMPILE_DEFINITIONS "LIBMMD_STATIC" 
	   DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
endif()


# add libicu::uc dependencies.
add_dependencies(libmmd icu::uc)
set(ICU_STATIC ON)
set(ICU_BUILD_UC ON)
set(ICU_BUILD_I18N OFF)
set(ICU_BUILD_IO OFF)
set(ICU_NO_INSTALL ON)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependency/icu")
target_link_libraries(libmmd PRIVATE icu::uc)
target_include_directories(libmmd PRIVATE ${ICU_INCLUDEDIR})

# add yalantinglibs::easylog dependencies.
add_dependencies(libmmd yalantinglibs::easylog)
set(BUILD_BENCHMARK OFF)
set(BUILD_EXAMPLES OFF)
set(INSTALL_INDEPENDENT_THIRDPARTY ON)
set(Build_ylt_struct_pb OFF)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependency/yalantinglibs")
target_link_libraries(libmmd PRIVATE yalantinglibs::easylog)


target_compile_features(libmmd PRIVATE cxx_std_20)

add_subdirectory(source) 