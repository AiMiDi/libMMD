cmake_minimum_required (VERSION 3.8)

project ("libMMD" VERSION 0.0.2)

option(USE_SHARED_LIB "use shared lib" OFF)

# Definitions
if (USE_SHARED_LIB)
    add_definitions(-DLIBMMD_SHARED_BUILD)
endif()

if (NOT DEFINED CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "_Debug")
endif()

if(MSVC)
  add_compile_options(/MP)
endif()

# Add the source code to the executable file for this project.
file(GLOB_RECURSE SRC_LIST "source/*.cpp" "source/*.hpp" "source/*.c")

# target
set(TARGET_NAME libmmd)

if (USE_SHARED_LIB)
   add_library(${TARGET_NAME} ${SRC_LIST})
   set_target_properties(
	   ${TARGET_NAME} 
	   PROPERTIES 
	   VERSION ${PROJECT_VERSION} 
	   SOVERSION ${PROJECT_VERSION} 
	   DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
else()
   add_library(${TARGET_NAME} STATIC ${SRC_LIST})
   set_target_properties(
	   ${TARGET_NAME} 
	   PROPERTIES 
	   VERSION ${PROJECT_VERSION} 
	   SOVERSION ${PROJECT_VERSION}
	   INTERFACE_COMPILE_DEFINITIONS "LIBMMD_STATIC" 
	   DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
endif()


# add libicu::uc dependencies.
add_dependencies(${TARGET_NAME} icu::uc)
set(ICU_STATIC ON)
set(ICU_BUILD_UC ON)
set(ICU_BUILD_I18N OFF)
set(ICU_BUILD_IO OFF)
set(ICU_NO_INSTALL ON)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/icu")
target_link_libraries(${TARGET_NAME} PRIVATE icu::uc)
target_include_directories(${TARGET_NAME} PRIVATE ${ICU_INCLUDEDIR})

# add yalantinglibs::easylog dependencies.
add_dependencies(${TARGET_NAME} yalantinglibs::easylog)
set(BUILD_BENCHMARK OFF)
set(BUILD_EXAMPLES OFF)
set(BUILD_UNIT_TESTS OFF)
set(Build_ylt_struct_pb OFF)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/yalantinglibs")
target_link_libraries(${TARGET_NAME} PRIVATE yalantinglibs::easylog)

target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)

target_include_directories(${TARGET_NAME} PUBLIC "include")
add_subdirectory(source)