#include <RLL/RLL.hpp>
#include <iostream>
#include <cstring>

void use_example_lib(std::string path);
void print_help();

int main(int argc, char const * argv[]){
    if(argc != 2){
        std::cout << "Invalid number of arguments.\n\n";
        print_help();
        return 1;
    }

    if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0){
        print_help();
        return 1;
    } 

    use_example_lib(argv[1]);

    return 0;
}

void print_help(){
    std::cout << "Usage: ./example <path-to-library>\n";
    std::cout << "The library must be a *shared* library compiled for the same system.\n";
    std::cout << "It must have a void function \"example_func\" that takes no arguments and is wrapped in extern \"C\" if it is a C++ library.";
    std::cout << "This program will run that function (not safely) if that symbol exists. It's nice for Hello World libraries or the sort.\n";
}

void use_example_lib(std::string path){
    rll::shared_library lib;

    lib.load(path);

    void * example_function = lib.get_symbol("example_func");

    ((void (*)())example_function)(); //Ignore this wizardry...
}