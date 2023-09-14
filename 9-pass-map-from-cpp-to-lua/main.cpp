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
#include <luabridge3/LuaBridge/Map.h>

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
    int scriptLoadStatus = luaL_dofile(luaState, "sample-9.lua");

    // define error reporter for any Lua error
    report_errors(luaState, scriptLoadStatus);

    // call Lua script' function
    std::map<std::string, std::string> configuration {
        {{ "name", "library" }, { "version", "1.0.0" }, { "description", "sample config map" }}
    };

    std::cout << "[C++] [sample-9] Configuration from C++ code: {\n";

    for (auto& kv : configuration) {
        std::cout << "  [" << kv.first << "] = \"" << kv.second << "\"" << std::endl;
    }

    std::cout << "}" << std::endl;

    luabridge::LuaRef testConfig = luabridge::getGlobal(luaState, "testConfig");

    luabridge::LuaResult res = testConfig(configuration);

    return 0;
}
