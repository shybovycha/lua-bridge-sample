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
#include <vector>

#include <luabridge3/LuaBridge/LuaBridge.h>

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
        return std::format("cppVector({}, {}, {})", m_x, m_y, m_z);
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

    bool operator==(const Entity& other) const {
        return m_name == other.m_name;
    }

    std::string toString() const {
        return std::format("<Entity '{0}' position <{1}>>", m_name, m_position.toString());
    }

private:
    std::string m_name;
    Vector3 m_position;
};

int cpp_func1(lua_State* L)
{
    std::string s = "hello, c++ function!";

    lua_pushlstring(L, s.c_str(), s.size());

    return 1;
}

int cpp_func2(int a, int b)
{
    return (a + b) / 2;
}

Entity* random_entity()
{
    auto pos = Vector3{ static_cast<double>(42), static_cast<double>(42), static_cast<double>(42) };
    auto name = "random_entity";
    auto entity = new Entity(name, pos);

    return entity;
}

// entrypoint to the Lua library
// this will be called any time you run `require('cpp_library')` in Lua code
// note how the name is dictated by Lua convention, `luaopen_${library_name.replaceAll('.', '_').replaceAll('-', '_')}`
// it also _has_ to be an `extern "C"` and exported from the library (`__declspec(dllexport)`)
extern "C" __declspec(dllexport) int luaopen_cpp_library(lua_State* L)
{
    luabridge::getGlobalNamespace(L)
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

    luabridge::getGlobalNamespace(L)
        .beginClass<Entity>("Entity")
        .addConstructor<void(*) (const std::string&, const Vector3&)>()
        .addProperty("name", &Entity::getName)
        .addProperty("position", &Entity::getPosition, &Entity::setPosition)
        .addFunction("__eq", &Entity::operator==)
        .addFunction("__tostring", &Entity::toString)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("cpp_library")
            .addFunction("cpp_func1", cpp_func1)
            .addFunction("cpp_func2", cpp_func2)
            .addFunction("random_entity", random_entity)
        .endNamespace();

    return 0;
}
