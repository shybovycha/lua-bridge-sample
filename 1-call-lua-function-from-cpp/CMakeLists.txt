cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

set(PROJECT_NAME "1-call-lua-function-from-cpp")

project(${PROJECT_NAME} LANGUAGES CXX)

set(TARGET_NAME ${PROJECT_NAME})
set(SOURCES "main.cpp")

add_executable(${TARGET_NAME} ${SOURCES})

target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)

find_package(Lua REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE ${LUA_LIBRARIES})
target_include_directories(${TARGET_NAME} PRIVATE ${LUA_INCLUDE_DIR})

find_path(LUABRIDGE3_INCLUDE_DIRS "luabridge3/LuaBridge/Array.h")
target_include_directories(${TARGET_NAME} PRIVATE ${LUABRIDGE3_INCLUDE_DIRS})

# copy Lua sources to the target directory
add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/sample-1.lua ${CMAKE_CURRENT_BINARY_DIR})
