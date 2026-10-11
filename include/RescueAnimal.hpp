#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <nlohmann/json.hpp>

// Allowed animal biological genders
enum class Gender : std::uint8_t {
  Unknown,
  Female,
  Male
};

// Lifecycle training phases for a rescue animal
enum class TrainingStatus : std::uint8_t {
  Intake,
  InTraining,
  PhaseI,
  PhaseII,
  PhaseIII,
  InService,
  Retired
};

// Converts the Gender enum to a displayable string
constexpr auto toString(Gender gender) noexcept -> std::string_view {
  switch (gender) {
    case Gender::Female: return "Female";
    case Gender::Male:   return "Male";
    default:             return "Unknown";
  }
}

// Converts the TrainingStatus enum to a displayable string
constexpr auto toString(TrainingStatus status) noexcept -> std::string_view {
  switch (status) {
    case TrainingStatus::Intake:     return "Intake";
    case TrainingStatus::InTraining: return "In Training";
    case TrainingStatus::PhaseI:     return "PhaseI";
    case TrainingStatus::PhaseII:    return "PhaseII";
    case TrainingStatus::PhaseIII:   return "PhaseIII";
    case TrainingStatus::InService:  return "In Service";
    case TrainingStatus::Retired:    return "Retired";
    default:                         return "Unknown";
  }
}

// Abstract base class representing any animal tracked by the organization.
// Holds general identification, health, and status attributes common across species.
class RescueAnimal {
private:
  std::string name_;
  Gender gender_{Gender::Unknown};
  std::chrono::year_month_day birthDate_{};
  double weightKg_{0.0};
  std::chrono::year_month_day acquisitionDate_{};
  std::string acquisitionContinent_;
  TrainingStatus trainingStatus_{TrainingStatus::Intake};
  bool reserved_{false};
  std::optional<std::string> inServiceContinent_{std::nullopt};

protected:
  // Default constructor only accessible by derived classes
  RescueAnimal() = default;

  // Full constructor for initializing base attributes
  RescueAnimal(std::string name,
               Gender gender,
               std::chrono::year_month_day birthDate,
               double weightKg,
               std::chrono::year_month_day acquisitionDate,
               std::string acquisitionContinent,
               TrainingStatus trainingStatus,
               bool reserved,
               std::optional<std::string> inServiceContinent)
      : name_{std::move(name)},
        gender_{gender},
        birthDate_{birthDate},
        weightKg_{weightKg},
        acquisitionDate_{acquisitionDate},
        acquisitionContinent_{std::move(acquisitionContinent)},
        trainingStatus_{trainingStatus},
        reserved_{reserved},
        inServiceContinent_{std::move(inServiceContinent)} {}

  // Protected copy/move constructors prevent object slicing when copying objects
  RescueAnimal(const RescueAnimal &) = default;
  auto operator=(const RescueAnimal &) -> RescueAnimal & = default;
  RescueAnimal(RescueAnimal &&) noexcept = default;
  auto operator=(RescueAnimal &&) noexcept -> RescueAnimal & = default;

public:
  // Virtual destructor ensures proper cleanup of derived class resources
  virtual ~RescueAnimal() = default;

  // Returns the concrete animal type string (e.g., "Dog", "Monkey")
  [[nodiscard]] virtual auto getAnimalType() const noexcept -> std::string_view = 0;

  // Derived classes implement this to write their unique fields into the JSON
  // payload. This removes dynamic casts in database storage functions.
  virtual void serializeSubclassFields(nlohmann::json &payload) const = 0;

  // --- Getters and Setters ---

  [[nodiscard]] auto getName() const noexcept -> const std::string & { return name_; }
  void setName(std::string name) { name_ = std::move(name); }

  [[nodiscard]] auto getGender() const noexcept -> std::string_view { return toString(gender_); }
  void setGender(Gender gender) noexcept { gender_ = gender; }

  [[nodiscard]] auto getBirthDate() const noexcept -> std::chrono::year_month_day { return birthDate_; }
  void setBirthDate(std::chrono::year_month_day birthDate) noexcept { birthDate_ = birthDate; }

  [[nodiscard]] auto getWeightKg() const noexcept -> double { return weightKg_; }
  void setWeightKg(double weightKg) noexcept { weightKg_ = weightKg; }

  [[nodiscard]] auto getAcquisitionDate() const noexcept -> std::chrono::year_month_day { return acquisitionDate_; }
  void setAcquisitionDate(std::chrono::year_month_day acquisitionDate) noexcept { acquisitionDate_ = acquisitionDate; }

  [[nodiscard]] auto getAcquisitionContinent() const noexcept -> const std::string & { return acquisitionContinent_; }
  void setAcquisitionContinent(std::string continent) { acquisitionContinent_ = std::move(continent); }

  [[nodiscard]] auto getTrainingStatus() const noexcept -> std::string_view { return toString(trainingStatus_); }
  void setTrainingStatus(TrainingStatus status) noexcept { trainingStatus_ = status; }

  [[nodiscard]] auto isReserved() const noexcept -> bool { return reserved_; }
  void setReserved(bool reserved) noexcept { reserved_ = reserved; }

  [[nodiscard]] auto getInServiceContinent() const noexcept -> const std::optional<std::string> & {
    return inServiceContinent_;
  }

  void setInServiceContinent(std::optional<std::string> continent) {
    inServiceContinent_ = std::move(continent);
  }
};