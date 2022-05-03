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

#include "LuaBridge/LuaBridge.h"
#include "LuaBridge/Map.h"

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
    int scriptLoadStatus = luaL_dofile(luaState, "sample-6.lua");

    // define error reporter for any Lua error
    report_errors(luaState, scriptLoadStatus);

    // call Lua script' function
    std::map<std::string, std::string> configuration = luabridge::getGlobal(luaState, "configuration");

    std::cout << "[C++] [sample-6] Configuration from Lua file: {\n";

    for (auto& kv : configuration) {
        std::cout << "  [" << kv.first << "] = \"" << kv.second << "\"" << std::endl;
    }

    std::cout << "}" << std::endl;

    return 0;
}
