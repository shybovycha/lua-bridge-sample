extern "C" 
{
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

#include "LuaBridge/LuaBridge.h"

#include <iostream>

class A {
protected:
    std::string name;

public:
    A(std::string s) {
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
    if (status != 0) {
        std::cout << "[LUA ERROR] " << lua_tostring(luaState, -1) << "\n";

        // remove error message from Lua state
        lua_pop(luaState, 1);
    }
}

int main() {
    // create a Lua state
    lua_State* luaState = luaL_newstate();

    // load standard libs 
    luaL_openlibs(luaState);

    // expose the A class to the Lua scripts
    luabridge::getGlobalNamespace(luaState)
        .beginClass<A>("A")
        .addConstructor<void(*) (std::string)>()
        .addFunction("getName", &(A::getName))
        .addFunction("printName", &(A::printName))
        .endClass();

    // create a global variable (an instance of A class) in Lua scope
    A* a = new A("some_var");
    luabridge::push(luaState, a);
    lua_setglobal(luaState, "a");

    // load some code from Lua file
    int lscript = luaL_dofile(luaState, "test1.lua");

    // define error reporter for any Lua error
    report_errors(luaState, lscript);

    // call function defined in Lua script
    luabridge::LuaRef runHandler = luabridge::getGlobal(luaState, "runHandler");

    int x = runHandler(15, 12);

    std::cout << "[EVALUATE LUA] (15 + 12) * 2 = " << x << "\n";
 
    return 0;
}
