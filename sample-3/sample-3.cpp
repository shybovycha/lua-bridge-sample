extern "C"
{
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

#include <memory>
#include <iostream>

#include "LuaBridge/LuaBridge.h"

class Vector3 {
public:
    Vector3() : m_x(0.0), m_y(0.0), m_z(0.0) {}

    Vector3(double x, double y, double z) : m_x(x), m_y(y), m_z(z) {}

    Vector3 operator+(Vector3 other) const {
        return Vector3(m_x + other.m_x, m_y + other.m_y, m_z + other.m_z);
    }

    Vector3 operator-(Vector3 other) const {
        return Vector3(m_x - other.m_x, m_y - other.m_y, m_z - other.m_z);
    }

    Vector3 operator*(double f) const {
        return Vector3(m_x * f, m_y * f, m_z * f);
    }

    double length() const {
        return std::sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
    }

    Vector3 normalize() const {
        const auto l = length();

        return Vector3(m_x / l, m_y / l, m_z / l);
    }

    double getX() const {
        return m_x;
    }

    double getY() const {
        return m_y;
    }

    double getZ() const {
        return m_z;
    }

    void setX(double x) {
        m_x = x;
    }

    void setY(double y) {
        m_y = y;
    }

    void setZ(double z) {
        m_z = z;
    }

private:
    double m_x;
    double m_y;
    double m_z;
};

class Entity {
public:
    Entity(const std::string& name, const Vector3& position) : m_name(name), m_position(position) {}

    std::string getName() const {
        return m_name;
    }

    Vector3 getPosition() const {
        return m_position;
    }

    void setPosition(const Vector3& position) {
        m_position = position;
    }

private:
    std::string m_name;
    Vector3 m_position;
};

void report_errors(lua_State *luaState, int status) {
    if (status == 0) {
        return;
    }

    std::cerr << "[LUA ERROR] " << lua_tostring(luaState, -1) << std::endl;

    // remove error message from Lua state
    lua_pop(luaState, 1);
}

void printString(const std::string& message) {
    std::cout << message << std::endl;
}

int main() {
    // create a Lua state
    lua_State* luaState = luaL_newstate();

    // load standard libs
    luaL_openlibs(luaState);

    // expose Vector3 class to Lua scripts
    luabridge::getGlobalNamespace(luaState)
        .beginClass<Vector3>("Vector3")
        .addConstructor<void(*) (double, double, double)>()
        .addProperty("x", &Vector3::getX, &Vector3::setX)
        .addProperty("y", &Vector3::getY, &Vector3::setY)
        .addProperty("z", &Vector3::getZ, &Vector3::setZ)
        .addProperty("length", &Vector3::length)
        .addFunction("normalize", &Vector3::normalize)
        .endClass();

    // expose helper functions to Lua scripts
    luabridge::getGlobalNamespace(luaState)
        .addFunction("print", &printString);

    // load some code from Lua file
    int scriptLoadStatus = luaL_dofile(luaState, "sample-3.lua");

    // define error reporter for any Lua error
    report_errors(luaState, scriptLoadStatus);

    // call Lua script function, passing a Vector3 instance
    luabridge::LuaRef testVector = luabridge::getGlobal(luaState, "testVector");
    testVector(Vector3(1.0, 2.0, 3.0));

    return 0;
}
