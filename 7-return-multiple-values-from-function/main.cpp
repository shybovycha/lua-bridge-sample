extern "C"
{
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

#include <memory>
#include <iostream>
#include <sstream>
#include <map>
#include <string>

#include <luabridge3/LuaBridge/LuaBridge.h>

void report_errors(lua_State *luaState, int status) {
    if (status == 0) {
        return;
    }

    std::cerr << "[LUA ERROR] " << lua_tostring(luaState, -1) << std::endl;

    // remove error message from Lua state
    lua_pop(luaState, 1);
}

int main() {
    // create a Lua state
    lua_State* luaState = luaL_newstate();

    // load standard libs
    luaL_openlibs(luaState);

    // load some code from Lua file
    int scriptLoadStatus = luaL_dofile(luaState, "sample-7.lua");

    // define error reporter for any Lua error
    report_errors(luaState, scriptLoadStatus);

    // call Lua script' function
    auto fibonacci5 = luabridge::getGlobal(luaState, "fibonacci5");

    auto res = fibonacci5();

    // extract multiple values from function result
    int x1 = res[0];
    int x2 = res[1];
    int x3 = res[2];
    int x4 = res[3];
    int x5 = res[4];

    std::cout << "[C++] [sample-7] First 5 Fibonacci numbers: "
        << x1 << " "
        << x2 << " "
        << x3 << " "
        << x4 << " "
        << x5 << " "
        << std::endl;

    // call another function
    auto randomValues = luabridge::getGlobal(luaState, "randomValues");

    auto res2 = randomValues();

    std::string s1 = res2[0];
    float s2 = res[1];

    std::cout << "[C++] [sample-7] Random values: "
        << s1 << " "
        << s2
        << std::endl;

    return 0;
}
