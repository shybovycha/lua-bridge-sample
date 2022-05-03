extern "C"
{
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

#include <memory>
#include <iostream>

#include "LuaBridge/LuaBridge.h"

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
    int scriptLoadStatus = luaL_dofile(luaState, "sample-1.lua");

    // define error reporter for any Lua error
    report_errors(luaState, scriptLoadStatus);

    // call function defined in Lua script
    luabridge::LuaRef addAndDouble = luabridge::getGlobal(luaState, "addAndDouble");

    int x = addAndDouble(15, 12);

    std::cout << "[EVALUATE LUA] (15 + 12) * 2 = " << x << std::endl;

    return 0;
}
