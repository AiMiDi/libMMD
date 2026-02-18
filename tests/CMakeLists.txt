if (LIBMMD_ENABLE_TEST)
    # MMD Model tests (CTest, no external test framework)
    add_executable(mmd_model_test
        ${CMAKE_CURRENT_SOURCE_DIR}/MMDModel.test.cpp
    )
    target_include_directories(mmd_model_test PRIVATE
        ${PROJECT_SOURCE_DIR}/external/spdlog/include
        ${LIBMMD_EIGEN_ROOT}
    )
    target_compile_definitions(mmd_model_test PRIVATE
        TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    )
    target_link_libraries(mmd_model_test libMMD)

    add_test(
        NAME mmd_model_test
        COMMAND $<TARGET_FILE:mmd_model_test>
    )

    # File base tests (MemoryReader + TextFileReader)
    add_executable(file_base_test
        ${CMAKE_CURRENT_SOURCE_DIR}/FileBase.test.cpp
    )
    target_include_directories(file_base_test PRIVATE
        ${PROJECT_SOURCE_DIR}/external/spdlog/include
    )
    target_link_libraries(file_base_test libMMD)

    add_test(
        NAME file_base_test
        COMMAND $<TARGET_FILE:file_base_test>
    )

    # PathUtil tests
    add_executable(path_util_test
        ${CMAKE_CURRENT_SOURCE_DIR}/PathUtil.test.cpp
    )
    target_include_directories(path_util_test PRIVATE
        ${PROJECT_SOURCE_DIR}/external/spdlog/include
    )
    target_link_libraries(path_util_test libMMD)

    add_test(
        NAME path_util_test
        COMMAND $<TARGET_FILE:path_util_test>
    )
endif()
