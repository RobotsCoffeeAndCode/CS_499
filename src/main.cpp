#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

import dog;

auto main() -> int {
    using namespace std::chrono_literals;

    std::vector<std::unique_ptr<RescueAnimal>> RescueAnimalServiceList;

    // Direct instantiation
    Dog dog(
        "Kona",
        Gender::Female,
        2021y / std::chrono::June / 15,
        29.4,
        2023y / std::chrono::March / 10,
        "USA",
        "Golden Retriever"
    );

    std::cout << "Direct call:\n";
    std::cout << "Name:  " << dog.getName() << '\n';
    std::cout << "Breed: " << dog.getBreed() << '\n';
    std::cout << "Type:  " << dog.getAnimalType() << '\n';
    std::cout << "NATHANIEL WAS HERE:  " << dog.getTrainingStatus() << '\n';

    std::cout << "\nPolymorphic call via Base pointer:\n";

    RescueAnimalServiceList.push_back(std::make_unique<Dog>(
        "Balto",
        Gender::Male,
        2019y / std::chrono::January / 1,
        34.0,
        2021y / std::chrono::August / 12,
        "USA",
        "Siberian Husky"
    ));

    const auto today = 2026y / std::chrono::September / 26;

    for (const auto& animal : RescueAnimalServiceList) {
        std::cout << "Type: " << animal->getAnimalType() << '\n';
        std::cout << "Name: " << animal->getName() << '\n';
        std::cout << "Age:  " << animal->getAgeInYears(today).count()
                  << " years\n";
    }

    return 0;
}