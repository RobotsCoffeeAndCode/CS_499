// This ensures this file is only read once during a single compilation
#pragma once

// Included standard C++ Libraries with reasoning
//-----------------------------------------------------------------------------

// Used for time keeping (date, calender, etc.)
#include <chrono>

// Gives access to C++ optional object
// it wraps a boolean value around a std data type the
// state of the boolean tells whether a value exists or not and
// then the data can also be accessed giving extra info on function return
#include <optional>

// Gives access to C++ string manipulation methods
#include <string>

// Gives access to C++ string_view object
// allows passing around a read-only reference to a string
#include <string_view>

// Gives access to
#include <utility>

// Gives access to uint8_t type for clang-tidy to work properly
#include <cstdint>

//-----------------------------------------------------------------------------


// Enumerations to ensure that Gender and
// TrainingStatus can only take on specific predefined values
enum class Gender : std::uint8_t { Unknown, Female, Male };
enum class TrainingStatus : std::uint8_t {
  Intake,
  InTraining,
  PhaseI,
  PhaseII,
  PhaseIII,
  InService,
  Retired
};

// Helper function to convert Gender enum to a string for printing
constexpr auto toString(Gender gender) -> std::string_view {
  switch (gender) {
  case Gender::Female:
    return "Female";
  case Gender::Male:
    return "Male";
  default:
    return "Unknown";
  }
}

// Helper function to convert Training Status enum to a string for printing
constexpr auto toString(TrainingStatus status) -> std::string_view {
  switch (status) {
  case TrainingStatus::Intake:
    return "Intake";
  case TrainingStatus::InTraining:
    return "In Training";
  case TrainingStatus::PhaseI:
    return "PhaseI";
  case TrainingStatus::PhaseII:
    return "PhaseII";
  case TrainingStatus::PhaseIII:
    return "PhaseIII";
  case TrainingStatus::InService:
    return "In Service";
  case TrainingStatus::Retired:
    return "Retired";
  default:
    return "unknown";
  }
}

// The Abstract RescueAnimal Class
class RescueAnimal {

  // Data members and functions only accessible by this class
private:
  // Name of the animal
  std::string name_;

  // The Gender of the animal (see enum above class def)
  Gender gender_{Gender::Unknown};

  // Birthday of the animal in chrono::year_month_day form
  std::chrono::year_month_day birthDate_{};

  // Weight of the animal in Kg (default is zero)
  double weightKg_{0.0};

  // Date animal was acquired for training/service chrono::year_month_day form
  std::chrono::year_month_day acquisitionDate_{};

  // Where the animal was acquired as a string
  std::string acquisitionCountry_;

  // The TrainingStatus of the animal (see enum above class def)
  TrainingStatus trainingStatus_{TrainingStatus::Intake};

  // Boolean value to represent if the animal has been reserved for service
  bool reserved_{false};

  // Optional to represent 1) if the animal is in service by the bool val
  // 2) what country they are in service in by the data val
  std::optional<std::string> inServiceCountry_{std::nullopt};

  // Data members and functions only accessible by this class and its children
protected:
  // Protected constructors ensure this class cannot be instantiated directly
  RescueAnimal() = default;

  // Constructor to initialize all data members with parameters
  RescueAnimal(std::string name, Gender gender,
               std::chrono::year_month_day birthDate, double weightKg,
               std::chrono::year_month_day acquisitionDate,
               std::string acquisitionCountry, TrainingStatus trainingStatus,
               bool reserved, std::optional<std::string> inServiceCountry)
      : name_{std::move(name)}, gender_{gender}, birthDate_{birthDate},
        weightKg_{weightKg}, acquisitionDate_{acquisitionDate},
        acquisitionCountry_{std::move(acquisitionCountry)},
        trainingStatus_{trainingStatus}, reserved_{reserved},
        inServiceCountry_{std::move(inServiceCountry)} {}

  // Protect copy/move operations to prevent slicing
  // slicing is when a child loses its unique data members due to
  // it being passed by value or assigned with this classes's type
  //
  // E.G.) RescueAnimal exampleAnimal = myUniqueDogObject;
  // Here myUniqueDogObject would lose its unique data w/o the functions below
  RescueAnimal(const RescueAnimal &) = default;
  auto operator=(const RescueAnimal &) -> RescueAnimal & = default;
  RescueAnimal(RescueAnimal &&) noexcept = default;
  auto operator=(RescueAnimal &&) noexcept -> RescueAnimal & = default;

  // Data members and functions accessible to any other code
  // NOTE: [[nodiscard]] requires the function's return to be handled
public:
  // Virtual destructor is mandatory for polymorphic base classes
  virtual ~RescueAnimal() = default;

  // Virtual method (This must be implemented by a concrete child class)
  [[nodiscard]] virtual auto getAnimalType() const noexcept
      -> std::string_view = 0;

  // Concrete methods (Children can use this implementation or override)
  // Get and Set the name of the animal
  [[nodiscard]] auto getName() const noexcept -> const std::string & {
    return name_;
  }
  void setName(std::string name) { name_ = std::move(name); }

  // Get and Set the Gender of the animal
  [[nodiscard]] auto getGender() const noexcept -> std::string_view {
    return toString(gender_);
  }
  void setGender(Gender gender) noexcept { gender_ = gender; }

  // Get and Set the birthday of the animal
  [[nodiscard]] auto getBirthDate() const noexcept
      -> std::chrono::year_month_day {
    return birthDate_;
  }
  void setBirthDate(std::chrono::year_month_day birthDate) noexcept {
    birthDate_ = birthDate;
  }

  // Calculate and return the age of the animal in years based on given date
  [[nodiscard]] auto
  getAgeInYears(std::chrono::year_month_day currentDate) const noexcept
      -> std::chrono::years {
    if (!birthDate_.ok() || !currentDate.ok() || currentDate < birthDate_) {
      return std::chrono::years{0};
    }
    auto years = currentDate.year() - birthDate_.year();
    const std::chrono::year_month_day birthdayThisYear{
        currentDate.year(), birthDate_.month(), birthDate_.day()};
    if (birthdayThisYear.ok() && currentDate < birthdayThisYear) {
      --years;
    }
    return years;
  }

  // Get and Set the weight of the animal
  [[nodiscard]] auto getWeightKg() const noexcept -> double {
    return weightKg_;
  }
  void setWeightKg(double weightKg) noexcept { weightKg_ = weightKg; }

  // Get and Set the date acquired of the animal
  [[nodiscard]] auto getAcquisitionDate() const noexcept
      -> std::chrono::year_month_day {
    return acquisitionDate_;
  }
  void
  setAcquisitionDate(std::chrono::year_month_day acquisitionDate) noexcept {
    acquisitionDate_ = acquisitionDate;
  }

  // Get and Set the origin country of the animal
  [[nodiscard]] auto getAcquisitionCountry() const noexcept
      -> const std::string & {
    return acquisitionCountry_;
  }
  void setAcquisitionCountry(std::string country) {
    acquisitionCountry_ = std::move(country);
  }

  // Get and Set the Training Status of the animal
  [[nodiscard]] auto getTrainingStatus() const noexcept -> std::string_view {
    return toString(trainingStatus_);
  }
  void setTrainingStatus(TrainingStatus status) noexcept {
    trainingStatus_ = status;
  }

  // Get and Set the Reserved Status of the animal
  [[nodiscard]] auto isReserved() const noexcept -> bool { return reserved_; }
  void setReserved(bool reserved) noexcept { reserved_ = reserved; }

  // Get and Set the Status of Service and Country of the animal
  [[nodiscard]] auto getInServiceCountry() const noexcept
      -> const std::optional<std::string> & {
    return inServiceCountry_;
  }
  void setInServiceCountry(std::optional<std::string> country) {
    inServiceCountry_ = std::move(country);
  }
};