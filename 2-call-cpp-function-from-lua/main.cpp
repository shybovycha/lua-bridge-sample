extern "C"
{
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

#include <memory>
#include <iostream>

#include "LuaBridge/LuaBridge.h"

class Greeter {
public:
    Greeter(const std::string& name) : m_name(name) {}

    std::string getName() const {
        return m_name;
    }

    void printName() {
        std::cout << "[C++ CODE] Hello, my name is " << m_name << "!" << std::endl;
    }

private:
    std::string m_name;
};

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

    // expose the Greeter class to the Lua scripts
    luabridge::getGlobalNamespace(luaState)
        .beginClass<Greeter>("Greeter")
        .addConstructor<void(*) (const std::string&)>()
        .addFunction("getName", &(Greeter::getName))
        .addFunction("printName", &(Greeter::printName))
        .endClass();

    // create a global variable (an instance of a Greeter class) in Lua scope
    auto globalGreeter = std::make_unique<Greeter>("noname");
    luabridge::push(luaState, globalGreeter.get());
    lua_setglobal(luaState, "greeter");

    // load some code from Lua file
    int scriptLoadStatus = luaL_dofile(luaState, "sample-2.lua");

    // define error reporter for any Lua error
    report_errors(luaState, scriptLoadStatus);

    return 0;
}
