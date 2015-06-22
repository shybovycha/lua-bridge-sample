# LuaBridge sample

## Overview

This project shows how simply and elegantly could Lua be bind to C++.

## Dependencies

You'll need **Lua 5.2** development package to be installed in order to compile this sample. 

For example, on Debian-based Linux distros you may use `sudo apt-get install liblua5.2-dev`. 

On MacOS X you may use `brew install lua`.

## Explanation

### A bit of Lua VM

Each Lua script works on top of Lua Virtual Machine. And variables and 
code calls between your host (core code, which invokes Lua VM and though - 
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

```
class A 
{
protected:
    std::string name;

public:
    A(std::string s)
    {
        this->name = s;
    }

    std::string getName()
    {
        return this->name;
    }

    void printName()
    {
        printf("Hello, my name is %s!\n", this->name.c_str());
    }
};
```

And you really want use it within your scripts without any wrapping classes
and code mess.

So, here's what you do:

1. you describe the class with the help of LuaBridge
2. you use the class within your Lua scripts

Yeah! That simple! See yourself:

```
luabridge::getGlobalNamespace(L)
    .beginClass<A>("A")
        .addConstructor<void(*) (std::string)>()
        .addFunction("getName", &A::getName)
        .addFunction("printName", &A::printName)
    .endClass();
```

...and then in your Lua script:

```
local a = A('moo')

a:printName()
```

And this will print `Hello, my name is moo!` on the screen.

### Binding Lua functions to C++

When you want to run the function, declared and defined within the script file,
you shall use great `LuaRef` class:

```
luabridge::LuaRef runHandler = luabridge::getGlobal(L, "runHandler");
```

This single line grants you access to a globally defined function `runHandler`.
So, if you have your script arranged to this just like:

```
function runHandler(arg)
    print(string.format("I was run with '%s'", arg))
end
```

then you might call 

```
runHandler("some awesome data");
```

and you shall see `I was run with 'some awesome data'` on your screen!

The great `LuaRef` class is something like `callable` in Ruby and other great
languages: it allows you to pass arguments and get the result of single run.

**But beware of type checks! Lua is not statically typed language!**

Let's demonstrate this rewriting our script like this:

```
function runHandler(a, b)
    return (a + b) * 2
end
```

and the core:

```
int x = runHandler(1, 3);

printf("(1 + 3) * 2 = %d\n", x);
```

When you run this, you'll get the `8` number on your screen. Isn't that charming?!
