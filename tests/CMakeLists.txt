if (LIBMMD_ENABLE_TEST)
    function(libmmd_add_ctest_runner target source)
        set(options)
        set(one_value_args LABELS)
        set(multi_value_args INCLUDE_DIRS LINK_LIBS COMPILE_DEFINITIONS)
        cmake_parse_arguments(RUNNER "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

        add_executable(${target}
            ${CMAKE_CURRENT_SOURCE_DIR}/${source}
        )

        target_include_directories(${target} PRIVATE
            ${PROJECT_SOURCE_DIR}/external/spdlog/include
            ${RUNNER_INCLUDE_DIRS}
        )

        target_link_libraries(${target} PRIVATE
            libMMD
            ${RUNNER_LINK_LIBS}
        )

        if (RUNNER_COMPILE_DEFINITIONS)
            target_compile_definitions(${target} PRIVATE
                ${RUNNER_COMPILE_DEFINITIONS}
            )
        endif()

        add_test(
            NAME ${target}
            COMMAND $<TARGET_FILE:${target}>
        )

        if (RUNNER_LABELS)
            set_tests_properties(${target} PROPERTIES LABELS "${RUNNER_LABELS}")
        endif()
    endfunction()

    libmmd_add_ctest_runner(mmd_model_test MMDModel.test.cpp
        INCLUDE_DIRS
            ${LIBMMD_EIGEN_ROOT}
        COMPILE_DEFINITIONS
            TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
            LIBMMD_TEST_GROUP_CORE=1
        LABELS "core;unit"
    )

    libmmd_add_ctest_runner(mmd_model_io_test MMDModel.test.cpp
        INCLUDE_DIRS
            ${LIBMMD_EIGEN_ROOT}
        COMPILE_DEFINITIONS
            TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
            LIBMMD_TEST_GROUP_IO=1
        LABELS "io;parser"
    )

    libmmd_add_ctest_runner(mmd_model_animation_test MMDModel.test.cpp
        INCLUDE_DIRS
            ${LIBMMD_EIGEN_ROOT}
        COMPILE_DEFINITIONS
            TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
            LIBMMD_TEST_GROUP_ANIMATION=1
        LABELS "animation;integration"
    )

    libmmd_add_ctest_runner(mmd_model_regression_test MMDModel.test.cpp
        INCLUDE_DIRS
            ${LIBMMD_EIGEN_ROOT}
        COMPILE_DEFINITIONS
            TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
            LIBMMD_TEST_GROUP_REGRESSION=1
        LABELS "regression;optimization"
    )

    libmmd_add_ctest_runner(mmd_model_benchmark_test MMDModel.test.cpp
        INCLUDE_DIRS
            ${LIBMMD_EIGEN_ROOT}
        COMPILE_DEFINITIONS
            TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
            LIBMMD_TEST_GROUP_BENCHMARK=1
        LABELS "benchmark;performance"
    )

    libmmd_add_ctest_runner(file_base_test FileBase.test.cpp
        LABELS "base;io;unit"
    )

    libmmd_add_ctest_runner(path_util_test PathUtil.test.cpp
        LABELS "path;unit"
    )

    libmmd_add_ctest_runner(file_roundtrip_test FileRoundTrip.test.cpp
        INCLUDE_DIRS
            ${LIBMMD_EIGEN_ROOT}
        LABELS "io;roundtrip"
    )

    libmmd_add_ctest_runner(vpd_file_test VPDFile.test.cpp
        INCLUDE_DIRS
            ${LIBMMD_EIGEN_ROOT}
        LABELS "io;parser;roundtrip"
    )

    libmmd_add_ctest_runner(pmx_export_invariants_test PmxExportInvariants.test.cpp
        INCLUDE_DIRS
            ${LIBMMD_EIGEN_ROOT}
        LABELS "io;roundtrip;export"
    )

    libmmd_add_ctest_runner(pmx_material_morph_test PmxMaterialMorph.test.cpp
        INCLUDE_DIRS
            ${LIBMMD_EIGEN_ROOT}
        LABELS "io;roundtrip;morph;material"
    )

    libmmd_add_ctest_runner(mmd_physics_test MMDPhysics.test.cpp
        INCLUDE_DIRS
            ${LIBMMD_EIGEN_ROOT}
            ${BULLET_INCLUDE_DIRS}
        LINK_LIBS
            ${BULLET_LIBRARIES}
        LABELS "integration;physics"
    )

    libmmd_add_ctest_runner(mmd_ik_solver_test MMDIkSolver.test.cpp
        INCLUDE_DIRS
            ${LIBMMD_EIGEN_ROOT}
        LABELS "ik;unit"
    )

    libmmd_add_ctest_runner(vmd_interpolation_test VMDInterpolation.test.cpp
        INCLUDE_DIRS
            ${LIBMMD_EIGEN_ROOT}
        LABELS "animation;interpolation"
    )
endif()
