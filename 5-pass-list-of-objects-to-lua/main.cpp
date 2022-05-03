extern "C"
{
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

#include <memory>
#include <iostream>
#include <sstream>

#include "LuaBridge/LuaBridge.h"
#include "LuaBridge/Vector.h"

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

    Vector3 negate() const {
        return Vector3(-m_x, -m_y, -m_z);
    }

    std::string toString() const {
        std::ostringstream s;
        s << "cppVector(" << m_x << ", " << m_y << ", " << m_z << ")";
        return s.str();
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
    Entity(const std::string& name, const Vector3& position) : m_name(name), m_position(position) {
        entities.push_back(this);
    }

    std::string getName() const {
        return m_name;
    }

    Vector3 getPosition() const {
        return m_position;
    }

    void setPosition(const Vector3& position) {
        m_position = position;
    }

    bool operator==(const Entity& other) const {
        return m_name == other.m_name;
    }

    static std::vector<Entity*> all() {
        return Entity::entities;
    }

private:
    static std::vector<Entity*> entities;

    std::string m_name;
    Vector3 m_position;
};

std::vector<Entity*> Entity::entities{};

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

    // expose Vector3 class to Lua scripts
    luabridge::getGlobalNamespace(luaState)
        .beginClass<Vector3>("Vector3")
        .addConstructor<void(*) (double, double, double)>()
        .addProperty("x", &Vector3::getX, &Vector3::setX)
        .addProperty("y", &Vector3::getY, &Vector3::setY)
        .addProperty("z", &Vector3::getZ, &Vector3::setZ)
        .addProperty("length", &Vector3::length)
        .addFunction("normalize", &Vector3::normalize)
        .addFunction("__add", &Vector3::operator+)
        .addFunction("__sub", &Vector3::operator-)
        .addFunction("__mul", &Vector3::operator*)
        .addFunction("__unm", &Vector3::negate)
        .addFunction("__tostring", &Vector3::toString)
        .endClass();

    // expose Entity class to Lua scripts
    luabridge::getGlobalNamespace(luaState)
        .beginClass<Entity>("Entity")
        // can't construct entities from scripts
        // .addConstructor<void(*) (const std::string&, const Vector3&)>()
        .addProperty("name", &Entity::getName)
        .addProperty("position", &Entity::getPosition, &Entity::setPosition)
        .addStaticFunction("all", &Entity::all)
        .addFunction("__eq", &Entity::operator==)
        .endClass();

    // load some code from Lua file
    int scriptLoadStatus = luaL_dofile(luaState, "sample-5.lua");

    // define error reporter for any Lua error
    report_errors(luaState, scriptLoadStatus);

    // create a few entities; all be tracked in Entity::entities vector
    auto entity1 = new Entity("entity-1", Vector3(0.0, 0.0, 0.0));
    auto entity2 = new Entity("entity-2", Vector3(0.0, -1.0, 0.0));
    auto entity3 = new Entity("entity-3", Vector3(0.0, 1.0, 0.0));
    auto entity4 = new Entity("entity-4", Vector3(0.0, 2.0, 0.0));

    // call Lua script' function
    luabridge::LuaRef findCollisions = luabridge::getGlobal(luaState, "findCollisions");

    std::vector<Entity*> collisionTestResult = findCollisions(entity1, 1.0);

    std::cout << "[C++] [test1] Collisions detected: " << collisionTestResult.size() << std::endl;

    for (size_t i = 0; i < collisionTestResult.size(); ++i) {
        const Entity* entity = collisionTestResult[i];

        std::cout << "[C++] [test1] collision: " << entity->getName() << std::endl;
    }

    // call Lua script' function
    luabridge::LuaRef applyGravity = luabridge::getGlobal(luaState, "applyGravity");

    applyGravity(collisionTestResult);

    std::vector<Entity*> collisionTestResult2 = findCollisions(entity1, 1.0);

    std::cout << "[C++] [test2] Collisions detected: " << collisionTestResult2.size() << std::endl;

    for (size_t i = 0; i < collisionTestResult2.size(); ++i) {
        const Entity* entity = collisionTestResult2[i];

        std::cout << "[C++] [test2] collision: " << entity->getName() << std::endl;
    }

    return 0;
}
