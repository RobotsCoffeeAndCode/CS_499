#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "RescueAnimal.hpp"

class Dog final : public RescueAnimal {
private:
  std::string breed_;

public:
  Dog() = default;

  // Full constructor initializing base and derived members
  Dog(std::string name, std::string breed, Gender gender,
      std::chrono::year_month_day birthDate, double weightKg,
      std::chrono::year_month_day acquisitionDate,
      std::string acquisitionCountry, TrainingStatus trainingStatus,
      bool reserved, std::optional<std::string> inServiceCountry)
      : RescueAnimal(std::move(name), gender, birthDate, weightKg,
                     acquisitionDate, std::move(acquisitionCountry),
                     trainingStatus, reserved, std::move(inServiceCountry)),
        breed_{std::move(breed)} {}

  // Implement the pure virtual method
  [[nodiscard]] auto getAnimalType() const noexcept
      -> std::string_view override {
    return "Dog";
  }

  // Getters and Setters
  [[nodiscard]] auto getBreed() const noexcept -> const std::string & {
    return breed_;
  }

  void setBreed(std::string breed) { breed_ = std::move(breed); }
};