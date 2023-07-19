cmake_minimum_required (VERSION 3.8)

target_precompile_headers(${TARGET_NAME} PRIVATE "pch.h")

add_subdirectory("core")
add_subdirectory("assets")
