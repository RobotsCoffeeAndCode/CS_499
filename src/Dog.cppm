module;

#include <chrono>
#include <string>
#include <string_view>
#include <utility>

export module dog;

// Re-export rescue_animal so consumers only need to import dog
export import rescue_animal;

export class Dog final : public RescueAnimal {
private:
  std::string breed_;

public:
  Dog(std::string name, Gender gender, std::chrono::year_month_day birthDate,
      double weightKg, std::chrono::year_month_day acquisitionDate,
      std::string acquisitionCountry, std::string breed)
      : RescueAnimal(std::move(name), gender, birthDate, weightKg,
                     acquisitionDate, std::move(acquisitionCountry)),
        breed_{std::move(breed)} {}

  // Implement the sole pure virtual function from RescueAnimal
  [[nodiscard]] auto getAnimalType() const noexcept
      -> std::string_view override {
    return "Dog";
  }

  [[nodiscard]] auto getBreed() const noexcept -> const std::string & {
    return breed_;
  }

  void setBreed(std::string breed) { breed_ = std::move(breed); }
};