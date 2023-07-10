local path = "cpp-library.dll"
local func1 = loadlib(path, "cpp_func1")

print(func1())
