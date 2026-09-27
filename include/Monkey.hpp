#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "RescueAnimal.hpp"

class Monkey final : public RescueAnimal {
private:
    double tailLength_{0.0F};
    double height_{0.0F};
    double bodyLength_{0.0F};
    std::string species_{"Unknown"};

public:
    Monkey() = default;

    // Overloaded full constructor
    Monkey(
        std::string name,
        Gender gender,
        std::chrono::year_month_day birthDate,
        double weightKg,
        std::chrono::year_month_day acquisitionDate,
        std::string acquisitionCountry,
        TrainingStatus trainingStatus,
        bool reserved,
        std::optional<std::string> inServiceCountry,
        double tailLength,
        double height,
        double bodyLength,
        std::string species
    ) : RescueAnimal(
            std::move(name),
            gender,
            birthDate,
            weightKg,
            acquisitionDate,
            std::move(acquisitionCountry),
            trainingStatus,
            reserved,
            std::move(inServiceCountry)
        ),
        tailLength_{tailLength},
        height_{height},
        bodyLength_{bodyLength},
        species_{std::move(species)} {}

    // Implement the pure virtual method
    [[nodiscard]] auto getAnimalType() const noexcept
        -> std::string_view override {
        return "Monkey";
    }

    // Getters and Setters
    [[nodiscard]] auto getTailLength() const noexcept -> double {
        return tailLength_;
    }
    void setTailLength(double tailLength) noexcept {
        tailLength_ = tailLength;
    }

    [[nodiscard]] auto getHeight() const noexcept -> double {
        return height_;
    }
    void setHeight(double height) noexcept {
        height_ = height;
    }

    [[nodiscard]] auto getBodyLength() const noexcept -> double {
        return bodyLength_;
    }
    void setBodyLength(double bodyLength) noexcept {
        bodyLength_ = bodyLength;
    }

    [[nodiscard]] auto getSpecies() const noexcept -> const std::string& {
        return species_;
    }
    void setSpecies(std::string species) {
        species_ = std::move(species);
    }
};