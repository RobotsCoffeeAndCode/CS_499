module;

#include <chrono>
#include <string>
#include <string_view>
#include <utility>

export module monkey;

// Re-export rescue_animal so consumers only need to import monkey
export import rescue_animal;

// Enum to ensure only valid monkey species are allowed
export enum class monkeySpecies : std::uint8_t { Capuchin, Guenon, Macaque, Marmoset, Squirrel, Tamarin };

export class Monkey final : RescueAnimal{
    private:
        double tailLength_;
        double height_;
        double bodyLength_;
        monkeySpecies species_;
};