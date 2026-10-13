# Shared target registration for libMMD's own tests and embedding projects.
# The caller owns CTest registration, so SDK/plugin test runners can select
# their own labels without depending on libMMD's full test suite.
function(libmmd_add_motion_sizing_targets libmmd_root)
    if(TARGET mmd_motion_sizing_test)
        return()
    endif()
    add_executable(mmd_motion_sizing_test
        "${libmmd_root}/tests/MMDMotionSizing.test.cpp")
    add_executable(mmd_motion_sizing_advanced_test
        "${libmmd_root}/tests/MMDMotionSizingAdvanced.test.cpp"
        "${libmmd_root}/tests/MMDMotionSizingAppendReference.cpp")
    add_executable(mmd_motion_sizing_parallel_test
        "${libmmd_root}/tests/MMDMotionSizingParallel.test.cpp")
    target_compile_features(mmd_motion_sizing_parallel_test PRIVATE cxx_std_20)
    target_include_directories(mmd_motion_sizing_parallel_test PRIVATE "${libmmd_root}/src/MotionSizing/Pose")
    # Licensed local assets are opt-in and are not registered with CTest.
    add_executable(mmd_motion_sizing_assets_probe EXCLUDE_FROM_ALL
        "${libmmd_root}/tests/MMDMotionSizingAssetsProbe.cpp")
    add_executable(mmd_motion_contact_quality_probe EXCLUDE_FROM_ALL
        "${libmmd_root}/tests/MMDMotionContactQualityProbe.cpp")
    foreach(target IN ITEMS mmd_motion_sizing_test mmd_motion_sizing_advanced_test mmd_motion_sizing_parallel_test mmd_motion_sizing_assets_probe mmd_motion_contact_quality_probe)
        target_link_libraries(${target} PRIVATE libMMD)
        target_compile_options(${target} PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
        # Match the sizing implementation's preserved Eigen packet/alignment
        # configuration. The playback reference has its own ISA below.
        if(MSVC)
            target_compile_options(${target} PRIVATE /arch:SSE2)
        endif()
    endforeach()
    if(MSVC AND LIBMMD_ENABLE_AVX2)
        set_source_files_properties("${libmmd_root}/tests/MMDMotionSizingAppendReference.cpp"
            PROPERTIES COMPILE_OPTIONS "/arch:AVX2")
    endif()
    target_include_directories(mmd_motion_sizing_advanced_test PRIVATE
        "${libmmd_root}/src/MotionSizing/Pose"
        "${libmmd_root}/src/MotionSizing/Solvers")
    target_include_directories(mmd_motion_sizing_assets_probe PRIVATE
        "${libmmd_root}/src/MotionSizing/Pose")
    target_include_directories(mmd_motion_contact_quality_probe PRIVATE
        "${libmmd_root}/src/MotionSizing/Pose"
        "${libmmd_root}/src/MotionSizing/Solvers")
endfunction()
