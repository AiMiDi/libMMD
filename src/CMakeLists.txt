# Base
set (
    BASE_SOURCE
    libMMD/Base/File.cpp
    libMMD/Base/Log.cpp
    libMMD/Base/Path.cpp
    libMMD/Base/Singleton.cpp
    libMMD/Base/Time.cpp
    libMMD/Base/UnicodeUtil.cpp
)
set (
    BASE_HEADER
    libMMD/Base/File.h
    libMMD/Base/Log.h
    libMMD/Base/Path.h
    libMMD/Base/Singleton.h
    libMMD/Base/Time.h
    libMMD/Base/UnicodeUtil.h
)

# OBJ Model
if (LIBMMD_BUILD_OBJ_MODEL)
    set (
        MODEL_OBJ_SOURCE
        libMMD/Model/OBJ/OBJModel.cpp
        libMMD/Model/OBJ/tinyobjloader.cpp
    )
    set (
        MODEL_OBJ_HEADER
        libMMD/Model/OBJ/OBJModel.h
    )
endif ()

# XFile Model
if (LIBMMD_BUILD_X_MODEL)
set (
    MODEL_XFILE_SOURCE
    libMMD/Model/XFile/XFileModel.cpp
)

set (
    MODEL_XFILE_HEADER
    libMMD/Model/XFile/XFileModel.h
)
endif ()

# MMD Model
set (
    MODEL_MMD_SOURCE
    libMMD/Model/MMD/MMDIkSolver.cpp
    libMMD/Model/MMD/MMDMaterial.cpp
    libMMD/Model/MMD/MMDModel.cpp
    libMMD/Model/MMD/MMDMorph.cpp
    libMMD/Model/MMD/MMDNode.cpp
    libMMD/Model/MMD/MMDPhysics.cpp
    libMMD/Model/MMD/MMDCamera.cpp
    libMMD/Model/MMD/PMDFile.cpp
    libMMD/Model/MMD/PMDModel.cpp
    libMMD/Model/MMD/PMXFile.cpp
    libMMD/Model/MMD/PMXModel.cpp
    libMMD/Model/MMD/SjisToUnicode.cpp
    libMMD/Model/MMD/VMDAnimation.cpp
    libMMD/Model/MMD/VMDCameraAnimation.cpp
    libMMD/Model/MMD/VMDFile.cpp
    libMMD/Model/MMD/VPDFile.cpp
)
set (
    MODEL_MMD_HEADER
    libMMD/Model/MMD/MMDFileString.h
    libMMD/Model/MMD/MMDIkSolver.h
    libMMD/Model/MMD/MMDMaterial.h
    libMMD/Model/MMD/MMDModel.h
    libMMD/Model/MMD/MMDMorph.h
    libMMD/Model/MMD/MMDNode.h
    libMMD/Model/MMD/MMDPhysics.h
    libMMD/Model/MMD/MMDCamera.h
    libMMD/Model/MMD/PMDFile.h
    libMMD/Model/MMD/PMDModel.h
    libMMD/Model/MMD/PMXFile.h
    libMMD/Model/MMD/PMXModel.h
    libMMD/Model/MMD/SjisToUnicode.h
    libMMD/Model/MMD/VMDAnimation.h
    libMMD/Model/MMD/VMDCameraAnimation.h
    libMMD/Model/MMD/VMDAnimationCommon.hpp
    libMMD/Model/MMD/VMDFile.h
    libMMD/Model/MMD/VPDFile.h
)

add_library(
    libMMD
    ${BASE_SOURCE}
    ${BASE_HEADER}
    ${MODEL_OBJ_SOURCE}
    ${MODEL_OBJ_HEADER}
    ${MODEL_XFILE_SOURCE}
    ${MODEL_XFILE_HEADER}
    ${MODEL_MMD_SOURCE}
    ${MODEL_MMD_HEADER}
)

target_include_directories(libMMD PUBLIC ${PROJECT_SOURCE_DIR}/external/glm)
target_include_directories(libMMD PRIVATE ${PROJECT_SOURCE_DIR}/external/spdlog/include)

if (LIBMMD_BUILD_OBJ_MODEL)
    target_include_directories(libMMD PRIVATE ${PROJECT_SOURCE_DIR}/external/tinyobjloader/include)
endif ()
if (LIBMMD_BUILD_X_MODEL)
    target_include_directories(libMMD PRIVATE ${PROJECT_SOURCE_DIR}/external/tinyxfileloader/include)
endif ()

set (LIBMMD_LIBRARIES ${BULLET_LIBRARIES})
if (UNIX)
    find_package(Threads REQUIRED)
    list (APPEND LIBMMD_LIBRARIES ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
endif ()
target_link_libraries(libMMD PRIVATE ${LIBMMD_LIBRARIES})

SOURCE_GROUP(Base FILES ${BASE_SOURCE} ${BASE_HEADER})
if (LIBMMD_BUILD_OBJ_MODEL)
    SOURCE_GROUP(Model\\OBJ FILES ${MODEL_OBJ_SOURCE} ${MODEL_OBJ_HEADER})
endif ()
if (LIBMMD_BUILD_X_MODEL)
    SOURCE_GROUP(Model\\XFile FILES ${MODEL_XFILE_SOURCE} ${MODEL_XFILE_HEADER})
endif ()
SOURCE_GROUP(Model\\MMD FILES ${MODEL_MMD_SOURCE} ${MODEL_MMD_HEADER})

# install
if (LIBMMD_INSTALL)
    install (TARGETS libMMD DESTINATION lib)
endif()
