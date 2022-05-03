cmake_minimum_required(VERSION 3.10)

project(6-pass-map-from-lua-to-cpp)

set(TARGET_NAME "6-pass-map-from-lua-to-cpp")
set(SOURCES "main.cpp")

add_executable(${TARGET_NAME} ${SOURCES})

include(FindLua)
find_package(lua REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE ${LUA_LIBRARIES})
target_include_directories(${TARGET_NAME} PRIVATE ${LUA_INCLUDE_DIR})

find_path(LUABRIDGE_INCLUDE_DIRS "LuaBridge/List.h")
target_include_directories(${TARGET_NAME} PRIVATE ${LUABRIDGE_INCLUDE_DIRS})

# copy Lua sources to the target directory
add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/sample-6.lua ${CMAKE_CURRENT_BINARY_DIR})
