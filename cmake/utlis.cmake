# Copyright:Copyright(c) 2023-present, Aimidi & contributors.


# Define a function to simplify Conan CMake integration
function(conan_cmake_setup BUILD_TYPE)
    # Detect Conan settings
    conan_cmake_autodetect(settings BUILD_TYPE ${BUILD_TYPE})

    # Install dependencies using Conan
    conan_cmake_install(PATH_OR_REFERENCE ${PROJECT_SOURCE_DIR}/conanfile.txt
                        BUILD missing
                        REMOTE conancenter
                        SETTINGS ${settings})
endfunction()

# Add add_custom_command only if it's an MSVC build system
function(combine_icu_static_libraries TARGET_NAME)
  # Custom build step to use the LIB tool to combine dependent static libraries into one lib file
  if(MSVC)
    set(ICU_LIB_PATH "${CMAKE_BINARY_DIR}/lib")
    find_program(lib_tool lib)
    add_custom_command(
    TARGET ${TARGET_NAME} POST_BUILD
  	COMMAND ${CMAKE_COMMAND} -E echo " - combine dependent icu static libraries: $<$<CONFIG:Debug>:${ICU_LIB_PATH}/${ICU_data_LIBS_DEBUG}.lib>$<$<CONFIG:Release>:${ICU_LIB_PATH}/${ICU_data_LIBS_RELEASE}.lib> and $<$<CONFIG:Debug>:${ICU_LIB_PATH}/${ICU_uc_LIBS_DEBUG}.lib>$<$<CONFIG:Release>:${ICU_LIB_PATH}/${ICU_uc_LIBS_RELEASE}.lib> into $<TARGET_FILE:${TARGET_NAME}>"
    COMMAND ${lib_tool} /NOLOGO /OUT:"$<TARGET_FILE:${TARGET_NAME}>"
  	"$<TARGET_FILE:${TARGET_NAME}>"
  	"$<$<CONFIG:Debug>:${ICU_LIB_PATH}/${ICU_uc_LIBS_DEBUG}.lib>$<$<CONFIG:Release>:${ICU_LIB_PATH}/${ICU_uc_LIBS_RELEASE}.lib>"
  	"$<$<CONFIG:Debug>:${ICU_LIB_PATH}/${ICU_data_LIBS_DEBUG}.lib>$<$<CONFIG:Release>:${ICU_LIB_PATH}/${ICU_data_LIBS_RELEASE}.lib>"
  	DEPENDS ${TARGET_NAME} ICU::uc ICU::dt)
  endif()
endfunction()
