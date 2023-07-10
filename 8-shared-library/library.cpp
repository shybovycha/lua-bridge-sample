extern "C"
{
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

#include <iostream>
#include <memory>
#include <string>
#include <format>

#include <luabridge3/LuaBridge/LuaBridge.h>

struct Entity
{
    int x, y;
};

static int cpp_func1(lua_State* L)
{
    std::string s = "hello, c++ function!";

    lua_pushlstring(L, s.c_str(), s.size());

    return 1;
}

static int cpp_func2(lua_State* L)
{
    lua_pushnumber(L, 3.14f);

    return 1;
}

extern "C" __declspec(dllexport) int luaopen_cpp_library(lua_State* L)
{
    std::cout << "registering cpp_func1..." << L << ":";
    lua_register(L, "cpp_func1", cpp_func1);
    lua_register(L, "cpp_func2", cpp_func2);
    /*luaL_Reg lib[] = {
        {"cpp_func1", &cpp_func1},
        {NULL, NULL}
    };
    // luaL_openlib is removed
    // luaL_openlib(L, "cpp_library", lib, 0);
    lua_newtable(L);
    std::cout << "[1]";
    luaL_setfuncs(L, lib, 0);
    std::cout << "[2]";
    lua_setglobal(L, "cpp_library");
    std::cout << "[3]";
    std::cout << "done\n";*/
    return 0;
}
