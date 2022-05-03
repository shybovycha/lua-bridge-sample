cmake_minimum_required(VERSION 3.10)

project(1-call-lua-function-from-cpp)

set(TARGET_NAME "1-call-lua-function-from-cpp")
set(SOURCES "main.cpp")

add_executable(${TARGET_NAME} ${SOURCES})

include(FindLua)
find_package(lua REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE ${LUA_LIBRARIES})
target_include_directories(${TARGET_NAME} PRIVATE ${LUA_INCLUDE_DIR})

find_path(LUABRIDGE_INCLUDE_DIRS "LuaBridge/List.h")
target_include_directories(${TARGET_NAME} PRIVATE ${LUABRIDGE_INCLUDE_DIRS})

# copy Lua sources to the target directory
add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/sample-1.lua ${CMAKE_CURRENT_BINARY_DIR})
