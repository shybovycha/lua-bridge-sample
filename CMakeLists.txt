cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(lua-bridge-samples)

add_subdirectory(1-call-lua-function-from-cpp)
add_subdirectory(2-call-cpp-function-from-lua)
add_subdirectory(3-expose-cpp-class-to-lua)
add_subdirectory(4-use-class-operators-in-lua)
add_subdirectory(5-pass-list-of-objects-to-lua)
add_subdirectory(6-pass-map-from-lua-to-cpp)
add_subdirectory(7-return-multiple-values-from-function)
add_subdirectory(8-shared-library)
add_subdirectory(9-pass-map-from-cpp-to-lua)
