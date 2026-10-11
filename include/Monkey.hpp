#pragma once

#include "RescueAnimal.hpp"
#include <utility>

// Concrete rescue monkey class tracking species and physical body measurements
class Monkey final : public RescueAnimal {
private:
  double tailLength_{0.0};
  double height_{0.0};
  double bodyLength_{0.0};
  std::string species_{"Unknown"};

public:
  Monkey() = default;

  Monkey(std::string name,
         Gender gender,
         std::chrono::year_month_day birthDate,
         double weightKg,
         std::chrono::year_month_day acquisitionDate,
         std::string acquisitionContinent,
         TrainingStatus trainingStatus,
         bool reserved,
         std::optional<std::string> inServiceContinent,
         double tailLength,
         double height,
         double bodyLength,
         std::string species)
      : RescueAnimal(std::move(name), gender, birthDate, weightKg,
                     acquisitionDate, std::move(acquisitionContinent),
                     trainingStatus, reserved, std::move(inServiceContinent)),
        tailLength_{tailLength},
        height_{height},
        bodyLength_{bodyLength},
        species_{std::move(species)} {}

  // Identifies this animal class as "Monkey"
  [[nodiscard]] auto getAnimalType() const noexcept -> std::string_view override {
    return "Monkey";
  }

  // Serializes monkey-specific properties for database storage
  void serializeSubclassFields(nlohmann::json &payload) const override {
    payload["species"] = species_;
    payload["tail_length"] = tailLength_;
    payload["height"] = height_;
    payload["body_length"] = bodyLength_;
  }

  [[nodiscard]] auto getTailLength() const noexcept -> double { return tailLength_; }
  void setTailLength(double tailLength) noexcept { tailLength_ = tailLength; }

  [[nodiscard]] auto getHeight() const noexcept -> double { return height_; }
  void setHeight(double height) noexcept { height_ = height; }

  [[nodiscard]] auto getBodyLength() const noexcept -> double { return bodyLength_; }
  void setBodyLength(double bodyLength) noexcept { bodyLength_ = bodyLength; }

  [[nodiscard]] auto getSpecies() const noexcept -> const std::string & { return species_; }
  void setSpecies(std::string species) { species_ = std::move(species); }
};