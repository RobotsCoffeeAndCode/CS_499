#include <iostream>
#include <format>
#include <chrono>
#include <thread>

int main() {
    std::string x;
    std::cout << std::format("Hello from Clang {} on Windows!\n", __clang_version__);
    std::getline(std::cin, x); 
    std::cout << "You wrote: " << x << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}