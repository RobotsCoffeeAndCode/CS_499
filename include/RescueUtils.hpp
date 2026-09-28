#pragma once

#include <array>
#include <chrono>
#include <optional>
#include <string>
#include <string_view>

#include "RescueAnimal.hpp"

namespace RescueUtils {

// Valid monkey species accepted by the system
inline constexpr std::array<std::string_view, 6> kValidMonkeySpecies{
    "Capuchin", "Guenon", "Macaque", "Marmoset", "Squirrel", "Tamarin"};

// Case-insensitive string comparison
[[nodiscard]] bool stringCompare(std::string_view lhs,
                                 std::string_view rhs) noexcept;

// Validates and constructs a year_month_day value
[[nodiscard]] constexpr auto makeDate(int year, int month, int day) noexcept
    -> std::optional<std::chrono::year_month_day> {
  if (month < 1 || month > 12 || day < 1 || day > 31) {
    return std::nullopt;
  }
  const std::chrono::year_month_day ymd{
      std::chrono::year{year}, std::chrono::month{static_cast<unsigned>(month)},
      std::chrono::day{static_cast<unsigned>(day)}};

  if (!ymd.ok()) {
    return std::nullopt;
  }
  return ymd;
}

// Parses dates formatted as YYYY-MM-DD, YYYY/MM/DD, or YYYY MM DD
[[nodiscard]] auto parseDateString(const std::string &str)
    -> std::optional<std::chrono::year_month_day>;

// Parses string representation into Gender enum (see RescueAnimal.hpp)
[[nodiscard]] auto parseGender(std::string_view str) noexcept -> Gender;

// Parses string representation into TrainingStatus enum (see RescueAnimal.hpp)
[[nodiscard]] auto parseTrainingStatus(std::string_view str) noexcept
    -> TrainingStatus;

// Validates whether a species matches the allowed monkey species
[[nodiscard]] auto isValidMonkeySpecies(std::string_view species) noexcept
    -> std::optional<std::string_view>;

// Console Input Helpers
auto promptForString(std::string_view prompt) -> std::string;
auto promptForDate(std::string_view prompt) -> std::chrono::year_month_day;
auto promptForPositiveDouble(std::string_view prompt, std::string_view errorMsg)
    -> double;
auto promptForBool(std::string_view prompt) -> bool;

} // namespace RescueUtils