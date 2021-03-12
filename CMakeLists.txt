cmake_minimum_required(VERSION 3.10)

project(lua-bridge-sample)

set(SOURCES "src/main.cpp")

add_executable(lua-bridge-sample ${SOURCES})

include(FindLua)
find_package(lua REQUIRED)
target_link_libraries(lua-bridge-sample PRIVATE ${LUA_LIBRARIES})
target_include_directories(lua-bridge-sample PRIVATE ${LUA_INCLUDE_DIR})

find_path(LUABRIDGE_INCLUDE_DIRS "LuaBridge/List.h")
target_include_directories(lua-bridge-sample PRIVATE ${LUABRIDGE_INCLUDE_DIRS})

# copy Lua sources to the target directory
add_custom_command(TARGET lua-bridge-sample POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/test1.lua ${CMAKE_CURRENT_BINARY_DIR}/test1.lua)
