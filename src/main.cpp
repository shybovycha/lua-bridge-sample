extern "C" 
{
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

#include "LuaBridge/LuaBridge.h"

#include <memory>
#include <iostream>

class Greeter {
protected:
    std::string name;

public:
    Greeter(std::string s) {
        this->name = s;
    }

    std::string getName() {
        return this->name;
    }

    void printName() {
        std::cout << "[C++ CODE] Hello, my name is " << this->name << "!\n";
    }
};

void report_errors(lua_State *luaState, int status) {
    if (status == 0) {
        return;
    }

    std::cout << "[LUA ERROR] " << lua_tostring(luaState, -1) << "\n";

    // remove error message from Lua state
    lua_pop(luaState, 1);
}

int main() {
    // create a Lua state
    lua_State* luaState = luaL_newstate();

    // load standard libs 
    luaL_openlibs(luaState);

    // expose the A class to the Lua scripts
    luabridge::getGlobalNamespace(luaState)
        .beginClass<Greeter>("Greeter")
        .addConstructor<void(*) (std::string)>()
        .addFunction("getName", &(Greeter::getName))
        .addFunction("printName", &(Greeter::printName))
        .endClass();

    // create a global variable (an instance of A class) in Lua scope
    Greeter* globalGreeter = new Greeter("noname");
    luabridge::push(luaState, *globalGreeter);
    lua_setglobal(luaState, "greeter");

    // load some code from Lua file
    int scriptLoadStatus = luaL_dofile(luaState, "test1.lua");

    // define error reporter for any Lua error
    report_errors(luaState, scriptLoadStatus);

    // call function defined in Lua script
    luabridge::LuaRef addAndDouble = luabridge::getGlobal(luaState, "addAndDouble");

    int x = addAndDouble(15, 12);

    std::cout << "[EVALUATE LUA] (15 + 12) * 2 = " << x << "\n";

    delete globalGreeter;

    return 0;
}
