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
  if (CMAKE_CXX_COMPILER_ID MATCHES "(Clang|GNU)")

    file(WRITE ${CMAKE_BINARY_DIR}/${TARGET_NAME}.ar.in
      "CREATE $<TARGET_FILE:${TARGET_NAME}>\n" )
        
    file(APPEND ${CMAKE_BINARY_DIR}/${TARGET_NAME}.ar.in
      "ADDLIB $<$<CONFIG:Debug>:${ICU_LIB_PATH}/${ICU_uc_LIBS_DEBUG}${CMAKE_STATIC_LIBRARY_SUFFIX}>$<$<CONFIG:Release>:${ICU_LIB_PATH}/${ICU_uc_LIBS_RELEASE}${CMAKE_STATIC_LIBRARY_SUFFIX}>\n")
	file(APPEND ${CMAKE_BINARY_DIR}/${TARGET_NAME}.ar.in
      "ADDLIB $<$<CONFIG:Debug>:${ICU_LIB_PATH}/${ICU_data_LIBS_DEBUG}${CMAKE_STATIC_LIBRARY_SUFFIX}>$<$<CONFIG:Release>:${ICU_LIB_PATH}/${ICU_data_LIBS_RELEASE}${CMAKE_STATIC_LIBRARY_SUFFIX}>\n")

    file(APPEND ${CMAKE_BINARY_DIR}/${TARGET_NAME}.ar.in "SAVE\n")
    file(APPEND ${CMAKE_BINARY_DIR}/${TARGET_NAME}.ar.in "END\n")

    file(GENERATE
      OUTPUT ${CMAKE_BINARY_DIR}/${TARGET_NAME}.ar
      INPUT ${CMAKE_BINARY_DIR}/${TARGET_NAME}.ar.in)

    set(ar_tool ${CMAKE_AR})
    set(ranlib_tool ${CMAKE_RANLIB})
    if (CMAKE_INTERPROCEDURAL_OPTIMIZATION)
      set(ar_tool ${CMAKE_CXX_COMPILER_AR})
      set(ranlib_tool ${CMAKE_CXX_COMPILER_RANLIB})
    endif()

    add_custom_command(
      COMMAND ${ar_tool} -M < ${CMAKE_BINARY_DIR}/${TARGET_NAME}.ar && ${ranlib_tool} "$<TARGET_FILE:${TARGET_NAME}>"
      OUTPUT "$<TARGET_FILE:${TARGET_NAME}>"
      COMMENT "Combine dependent icu static libraries: $<$<CONFIG:Debug>:${ICU_LIB_PATH}/${ICU_data_LIBS_DEBUG}${CMAKE_STATIC_LIBRARY_SUFFIX}>$<$<CONFIG:Release>:${ICU_LIB_PATH}/${ICU_data_LIBS_RELEASE}${CMAKE_STATIC_LIBRARY_SUFFIX}> and $<$<CONFIG:Debug>:${ICU_LIB_PATH}/${ICU_uc_LIBS_DEBUG}${CMAKE_STATIC_LIBRARY_SUFFIX}>$<$<CONFIG:Release>:${ICU_LIB_PATH}/${ICU_uc_LIBS_RELEASE}${CMAKE_STATIC_LIBRARY_SUFFIX}> into $<TARGET_FILE:${TARGET_NAME}>"
	  DEPENDS ${TARGET_NAME} ICU::uc ICU::dt
	  VERBATIM)
  elseif(MSVC)
    set(ICU_LIB_PATH "${CMAKE_BINARY_DIR}/lib")
    find_program(lib_tool lib)
    add_custom_command(
      TARGET ${TARGET_NAME} POST_BUILD
  	  COMMENT "Combine dependent icu static libraries: $<$<CONFIG:Debug>:${ICU_LIB_PATH}/${ICU_data_LIBS_DEBUG}${CMAKE_STATIC_LIBRARY_SUFFIX}>$<$<CONFIG:Release>:${ICU_LIB_PATH}/${ICU_data_LIBS_RELEASE}${CMAKE_STATIC_LIBRARY_SUFFIX}> and $<$<CONFIG:Debug>:${ICU_LIB_PATH}/${ICU_uc_LIBS_DEBUG}${CMAKE_STATIC_LIBRARY_SUFFIX}>$<$<CONFIG:Release>:${ICU_LIB_PATH}/${ICU_uc_LIBS_RELEASE}${CMAKE_STATIC_LIBRARY_SUFFIX}> into $<TARGET_FILE:${TARGET_NAME}>"
      COMMAND ${lib_tool} /NOLOGO /OUT:"$<TARGET_FILE:${TARGET_NAME}>"
  	  "$<TARGET_FILE:${TARGET_NAME}>"
  	  "$<$<CONFIG:Debug>:${ICU_LIB_PATH}/${ICU_uc_LIBS_DEBUG}${CMAKE_STATIC_LIBRARY_SUFFIX}>$<$<CONFIG:Release>:${ICU_LIB_PATH}/${ICU_uc_LIBS_RELEASE}${CMAKE_STATIC_LIBRARY_SUFFIX}>"
  	  "$<$<CONFIG:Debug>:${ICU_LIB_PATH}/${ICU_data_LIBS_DEBUG}${CMAKE_STATIC_LIBRARY_SUFFIX}>$<$<CONFIG:Release>:${ICU_LIB_PATH}/${ICU_data_LIBS_RELEASE}${CMAKE_STATIC_LIBRARY_SUFFIX}>"
  	  DEPENDS ${TARGET_NAME} ICU::uc ICU::dt
	  VERBATIM)
  endif()
endfunction()
