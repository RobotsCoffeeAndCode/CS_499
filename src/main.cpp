#include <iostream>
#include <format>
#include <chrono>
#include <thread>

auto main() -> int {

    const int timeToSleep = 5;

    std::string userInput;
    std::cout << std::format("Hello from Clang {} on Windows!\n", __clang_version__);
    std::getline(std::cin, userInput); 
    std::cout << "You wrote: " << userInput << '\n';

    std::this_thread::sleep_for(std::chrono::seconds(timeToSleep));

    return 0;
}