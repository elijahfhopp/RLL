> Before you open an issue please check out the [common issues](#common-issues) section of the ReadMe.

# RLL

RLL is a multi-platform, thread-safe, runtime dynamic/shared library loader. Its name stands for exactly what it does: Runtime Library Loader. It's light (<300 *lines* of code) and is header-only library. And best of all it's public domain!

Care for an example?

```cpp 
#include <RLL/RLL.hpp>

int main(){
    rll::shared_library my_lib;

    try {
        my_lib.load("./my_lib.so");
        // OR
        // my_lib.load([path], [loader_flags])
    } catch (rll::exception::library_loading_error& e) {
        // Handle exceptions.
    }

    if(test_lib.has_symbol("add")){
        add_function = test_lib.get_function_symbol<int(int, int)>("add"));
    }
}
```

RLL is unit tested for safety and code reliability.

### Requirements:

A C++11 supported compiler. There is a C++98 supported branch that mirrors version 1.0.0 with the needed changes and hotfixes.

### Dependencies:

The only dependency RLL has is the `dl` library when you are compiling on a POSIX system, i.e. Linux or Mac. This can be easily remedied with something like:

```
if(NOT WIN32)
    target_link_libraries(${my_target} PRIVATE dl)
endif()
```

if you are using CMake.

## I just wanna jump into it!

The docs are generated with Doxygen and can be easily accessed in the `docs/` folder. Be sure to checkout the [common issues](#common-issues) area if you run into an issues. And let me know any other issues via opening an issue, or maybe even a PR.

## Common issues:

**Symbol visibility**:

 - On Windows (as with and DLL with exports) you must explicitly export symbols via a build system configuration, or `__declspec`.
 - With GCC/Clang most symbols are visible by default, but certain are not including non-`extern` `const` variables.
  
For more information on symbol visibility in the global scope checkout [this wonderful article by 
Federico Kircheis](https://fekir.info/post/global-variables-in-cpp-libraries/).

**Not using `extern "C"` for C++ libraries**:

C++ names are "mangled" to allow for multiple symbols with the same name in source code. This allows for namespaces and templates among other things. Reversing name-mangling may be a future RLL feature, but it isn't a priority right now as name mangling is non-standard compiler implemented.

**Not linking `dl`:**

It's important that on POSIX (Unix-like) systems you link `dl`. It is the system library that allows you to load shared libraries. If you don't link it you will get undefined symbol errors.

**SEG FAULT**:

I can't really help you with that. You are going to need a debugger.

## Licensure:

PUBLIC DOMAIN!!!!
