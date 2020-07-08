// This is an RLL test script.
// It is public domain:
// Copyright (c) 2020 Elijah Hopp, No Rights Reserved.
//----------------------------------INCLUDES----------------------------------//
#include <RLL/RLL.hpp>

#include <cstring>
#include <functional>

#define CATCH_CONFIG_MAIN 1
#include <catch-mini/catch-mini.hpp>
//----------------------------SHARED_LIBRARY_TEST-----------------------------//
using namespace rll;

TEST_CASE("Opening and using a shared library works"){
    bool exception_state = false;
    shared_library library;
    REQUIRE(library.is_loaded() == false);
    REQUIRE(library.get_platform_handle() == nullptr);

    try {
        library.load("./dummy_library.library");
    } catch(exception::rll_exception& e){
        std::cout << "Encountered an error loading the library:\n" << e.what() << "\n";
        exception_state = true;
    }
    REQUIRE(exception_state == false);
    REQUIRE(library.is_loaded() == true);
    REQUIRE(library.get_platform_handle() != nullptr);
    REQUIRE(library.has_symbol("add"));
    REQUIRE(library.has_symbol("abc"));
    
    std::function<int(int, int)> add_func;
    try {
        add_func = reinterpret_cast<int (*)(int, int)>(library.get_symbol("add"));
    } catch(exception::rll_exception& e){
        std::cout << "Encountered an error while getting the \"add\" symbol:\n" << e.what() << "\n";
        exception_state = true;
    }
    REQUIRE(exception_state == false);
    REQUIRE(add_func(2, 2) == 4);

    std::string abc;
    try {
        abc = static_cast<char *>(library.get_symbol("abc"));
    } catch(exception::rll_exception& e){
        std::cout << "Encountered an error while getting the \"abc\" symbol:\n" << e.what() << "\n";
        exception_state = true;
    }
    REQUIRE(exception_state == false);
    REQUIRE(abc == "abc");
}