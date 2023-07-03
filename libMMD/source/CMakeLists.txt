cmake_minimum_required (VERSION 3.8)

target_include_directories(libmmd PUBLIC "include")
target_precompile_headers(libmmd PRIVATE "pch.h")

add_subdirectory("core")
add_subdirectory("assets")
