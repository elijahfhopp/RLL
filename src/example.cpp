#include <RLL/RLL.hpp>
#include <iostream>
#include <cstring>

using namespace rll::windows_flags;

int main() {
    const char * abc = "abc";

    std::cout << std::boolalpha << strcmp(abc, "abc") << "\n";
}