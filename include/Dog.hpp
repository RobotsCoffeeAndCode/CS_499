#pragma once

#include "RescueAnimal.hpp"
#include <utility>

// Concrete rescue dog class tracking dog-specific traits like breed
class Dog final : public RescueAnimal {
private:
  std::string breed_;

public:
  Dog() = default;

  Dog(std::string name,
      std::string breed,
      Gender gender,
      std::chrono::year_month_day birthDate,
      double weightKg,
      std::chrono::year_month_day acquisitionDate,
      std::string acquisitionContinent,
      TrainingStatus trainingStatus,
      bool reserved,
      std::optional<std::string> inServiceContinent)
      : RescueAnimal(std::move(name), gender, birthDate, weightKg,
                     acquisitionDate, std::move(acquisitionContinent),
                     trainingStatus, reserved, std::move(inServiceContinent)),
        breed_{std::move(breed)} {}

  // Identifies this animal class as "Dog"
  [[nodiscard]] auto getAnimalType() const noexcept -> std::string_view override {
    return "Dog";
  }

  // Serializes dog-specific properties for database storage
  void serializeSubclassFields(nlohmann::json &payload) const override {
    payload["breed"] = breed_;
  }

  [[nodiscard]] auto getBreed() const noexcept -> const std::string & { return breed_; }
  void setBreed(std::string breed) { breed_ = std::move(breed); }
};