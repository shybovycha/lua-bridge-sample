# LuaBridge samples

## Overview

This project shows how simply and elegantly could Lua be bind to C++ using the LuaBridge library.
The [official documentation for LuaBridge](http://vinniefalco.github.io/LuaBridge/Manual.html#s4.1)
is fine, but it still lacks few examples, in my opinion. This repo is aimed to fill those gaps.

There are multiple samples now in the project

1. `1-call-lua-function-from-cpp` - demonstrates the simplest interaction between C++ and Lua by calling a Lua script' function from C++ code
2. `2-call-cpp-function-from-lua` - does the exact opposite - provides Lua code with references back to C++ class and lets Lua code to call C++ code
3. `3-expose-cpp-class-to-lua` - shows the use of properties and methods of a C++ class from Lua code
4. `4-use-class-operators-in-lua` - adds operators (read more about [metatables and metamethods](http://www.lua.org/manual/5.4/manual.html#2.4)) to the C++/Lua class and uses those operators in Lua code
5. `5-pass-list-of-objects-to-lua` - showcases passing lists of objects between C++ and Lua
6. `6-pass-map-from-lua-to-cpp` - using global variables and maps (aka hashtables, dictionaries)

## Build and run

You'll need [vcpkg](https://github.com/microsoft/vcpkg) and [CMake](https://cmake.org/) in order to build this project.

Then just use

```bash
$ cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$VCPKG_PATH/scripts/buildsystems/vcpkg.cmake
$ cmake --build build
```

This will create an executable at `build/lua-bridge-sample`.

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
    A(const std::string& name) : m_name(name) {}

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

Let's demonstrate this rewriting our script like this:

```lua
function runHandler(a, b)
    return (a + b) * 2
end
```

and the core:

```cpp
int x = runHandler(1, 3);

std::cout << "(1 + 3) * 2 = " << x << std::endl;
```

When you run this, you'll get the `8` number on your screen. Isn't that charming?
