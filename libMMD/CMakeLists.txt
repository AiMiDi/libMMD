# CMakeList.txt: libMMD 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

if(NOT DEPENDENCY_INSTALL_PATH)
	set(DEPENDENCY_INSTALL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dependency/install)
endif()

list(APPEND CMAKE_MODULE_PATH ${DEPENDENCY_INSTALL_PATH}/lib/cmake)

if(NOT TARGET Iconv::Iconv)
	find_package(Iconv 
	CONFIG 
	COMPONENTS Iconv 
	PATHS ${DEPENDENCY_INSTALL_PATH})

	if(NOT TARGET Iconv::Iconv)
		find_package(iconv REQUIRED)
	endif()
endif()

if(NOT TARGET yalantinglibs::easylog)
	find_package(yalantinglibs 
	CONFIG 
	COMPONENTS easylog 
	PATHS ${DEPENDENCY_INSTALL_PATH})

	if(NOT TARGET yalantinglibs::easylog)
		find_package(yalantinglibs REQUIRED COMPONENTS easylog)
	endif()
endif()


# 将源代码添加到此项目的可执行文件。
file(GLOB_RECURSE SRC_LIST "source/*.cpp" "source/*.hpp" "source/*.c")

# Build
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

target_link_libraries(libmmd PRIVATE Iconv::Iconv)
target_link_libraries(libmmd PRIVATE yalantinglibs::easylog)

target_compile_features(libmmd PRIVATE cxx_std_20)

add_subdirectory(source) 