# LuaBridge samples

## Overview

This project shows how simply and elegantly could Lua be bind to C++ using the LuaBridge3 library.
The [official documentation for LuaBridge](https://kunitoki.github.io/LuaBridge3/Manual)
is fine, but it still lacks few examples, in my opinion. This repo is aimed to fill those gaps.

For the `LuaBridge` (not `LuaBridge3`) samples, check the [`luabridge`](https://github.com/shybovycha/lua-bridge-sample/tree/luabridge) branch - the API of the libraries differs slightly.

There are multiple samples now in the project

1. `1-call-lua-function-from-cpp` - demonstrates the simplest interaction between C++ and Lua by calling a Lua script' function from C++ code
2. `2-call-cpp-function-from-lua` - does the exact opposite - provides Lua code with references back to C++ class and lets Lua code to call C++ code
3. `3-expose-cpp-class-to-lua` - shows the use of properties and methods of a C++ class from Lua code
4. `4-use-class-operators-in-lua` - adds operators (read more about [metatables and metamethods](http://www.lua.org/manual/5.4/manual.html#2.4)) to the C++/Lua class and uses those operators in Lua code
5. `5-pass-list-of-objects-to-lua` - showcases passing lists of objects between C++ and Lua
6. `6-pass-map-from-lua-to-cpp` - using global variables and maps (aka hashtables, dictionaries)
7. `7-return-multiple-values-from-function` - returning multiple values from a Lua function and handling those in C++
8. `8-shared-library` - making a library in C++ to be used in Lua (with `require('library_name')`)

## Build and run

You'll need [vcpkg](https://github.com/microsoft/vcpkg) and [CMake](https://cmake.org/) in order to build this project.

Then just use

```bash
$ cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$VCPKG_PATH/scripts/buildsystems/vcpkg.cmake
$ cmake --build build
```

This will create executables under the `build` directory (e.g. `build/1-call-lua-function-from-cpp/Debug/1-call-lua-function-from-cpp.exe`).

## How it works

### A bit of Lua VM

Each Lua script works on top of Lua Virtual Machine. And variables and
code calls between your host (core code, which invokes Lua VM and thus -
scripts' runs) are done in an interesting stack-manner.

See, you have a stack, where you could `push` something to and then `pop`
something out of it. Before any action like function call, you should
push its name and arguments in correct order into the stack and then
call the `lua_pcall` function to run a script-defined routine.

The same with variables: first, you push its value and then register it as
a global or local variable.

That's much more simpler when using LuaBridge.

### Bringing C++ classes to Lua

**LuaBridge** seems to be an awesome... well... bridge, between C++ and Lua.
It's charming relays on how simply you could pass data between your core
and your scripts.

Say, you have a class named `A`:

```cpp
class A
{
public:
    A(const std::string_view name) : m_name(name) {}

    std::string getName()
    {
        return m_name;
    }

    void printName()
    {
        std::cout << "Hello, my name is " << m_name << "!" << std::endl;
    }

private:
    std::string m_name;
};
```

And you really want use it within your scripts without any wrapping classes
and code mess.

So, here's what you do:

1. you describe the class with the help of LuaBridge
2. you use the class within your Lua scripts

Yeah! That simple! See yourself:

```cpp
luabridge::getGlobalNamespace(L)
    .beginClass<A>("A")
        .addConstructor<void(*) (const std::string&)>()
        .addFunction("getName", &A::getName)
        .addFunction("printName", &A::printName)
    .endClass();
```

...and then in your Lua script:

```lua
local a = A('moo')

a:printName()
```

And this will print `Hello, my name is moo!` on the screen.

### Binding Lua functions to C++

When you want to run the function, declared and defined within the script file,
you shall use great `LuaRef` class:

```cpp
luabridge::LuaRef runHandler = luabridge::getGlobal(L, "runHandler");
```

This single line grants you access to a globally defined function `runHandler`.
So, if you have your script arranged to this just like:

```lua
function runHandler(arg)
    print(string.format("I was run with '%s'", arg))
end
```

then you might call

```cpp
runHandler("some awesome data");
```

and you shall see `I was run with 'some awesome data'` on your screen!

The great `LuaRef` class is something like `callable` in Ruby and other great
languages: it allows you to pass arguments and get the result of single run.

**But beware of type checks! Lua is not statically typed language!**

Let's demonstrate this by rewriting our script like this:

```lua
function runHandler(a, b)
    return (a + b) * 2
end
```

and the application:

```cpp
luabridge::LuaResult res = runHandler(1, 3);

int x = res[0];

std::cout << "(1 + 3) * 2 = " << x << std::endl;
```

When you run this, you'll get the `8` number on your screen. Isn't that charming?

## Making a Lua library in C++

In order to create a whole library in C++, you'll need to know about few caveats:

1. there __must__ be an entry point in the library, an exported function `extern "C" __declspect(dllexport) luaopen_{library_name_underscore_case}(lua_State* L)`
2. the function should register any classes, namespaces, functions and/or constants you want to expose from your library - by default no C++ code is exposed to Lua, despite being exported from the shared library
3. smart pointers and collections (like `std::vector`) from standard C++ library won't work out of the box (even though you are using LuaBridge!), hence you will have to figure it out yourself, unfortunately

The simplest library could be as follows:

```cpp
extern "C"
{
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

#include <string>
#include <format>

#include <luabridge3/LuaBridge/LuaBridge.h>

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

extern "C" __declspec(dllexport) int luaopen_cpp_library(lua_State* L)
{
    luabridge::getGlobalNamespace(L)
        .beginNamespace("cpp_library")
            .addFunction("cpp_func1", cpp_func1)
            .addFunction("cpp_func2", cpp_func2)
        .endNamespace();

    return 0;
}
```

Once you have created a library in C++ and built it (as either an `.so` or `.dll` file, depending on a platform), you can use it in Lua by calling `require('library_name')`.

In the example above, I deliberately created a namespace `cpp_library`, to encapsulate all the exposed functions in a contained manner.
So that instead of just calling `cpp_func1()` in Lua code you have them all nicely packed within the `cpp_library` namespace.

## Note about changes introduced in LuaBridge3

Since Lua supports returning multiple values from a function, calling a function now returns `luabridge::LuaResult` instead of the raw type.

Besides of checking if the call was successful or not (by using `.wasOk()`, `.hasFailed()`, `.errorMessage()`, etc. on the `LuaResult` variable)
one can treat it as Lua table.

Thus if a function returns multiple values:

```lua
function moo()
    return 1, 2
end
```

one can get the values like the folls:

```cpp
auto moo = luabridge::getGlobal(luaState, "moo");
auto res = moo();

int x1 = res[0];
int x2 = res[1];
```

Note how values returned by a function can all have different type:

```lua
function foo()
    return -3.14, "moo", 12
end
```

Accessing them would require a type casting (potentially implicit):

```cpp
auto foo = luabridge::getGlobal(luaState, "foo");
auto res = foo();

// implicit casting to string
std::cout << res[0] << " " << res[1] << " " << res[2] << "\n";

// explicit casting
std::cout << static_cast<float>(res[0]) << " " << res[1] << " " << static_cast<int>(res[2]) << "\n";
```
