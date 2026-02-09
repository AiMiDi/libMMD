if (LIBMMD_ENABLE_TEST)
    # MMD Model tests (CTest, no external test framework)
    add_executable(mmd_model_test
        ${CMAKE_CURRENT_SOURCE_DIR}/MMDModel.test.cpp
    )
    target_include_directories(mmd_model_test PRIVATE
        ${PROJECT_SOURCE_DIR}/external/spdlog/include
        ${LIBMMD_EIGEN_ROOT}
    )
    target_link_libraries(mmd_model_test libMMD)

    add_test(
        NAME mmd_model_test
        COMMAND $<TARGET_FILE:mmd_model_test>
    )
endif()
