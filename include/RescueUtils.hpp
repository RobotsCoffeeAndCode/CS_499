#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

#include "RescueAnimal.hpp"

namespace RescueUtils {

// Single source of truth for continent names and their stable integer graph indices
inline constexpr std::array<std::string_view, 7> kValidContinents{
    "Africa",        // 0
    "Antarctica",    // 1
    "Asia",          // 2
    "Australia",     // 3
    "Europe",        // 4
    "North America", // 5
    "South America"  // 6
};

// Approved monkey species accepted by the rescue program
inline constexpr std::array<std::string_view, 6> kValidMonkeySpecies{
    "Capuchin", "Guenon", "Macaque", "Marmoset", "Squirrel", "Tamarin"
};

// Case-insensitive string comparison helper
[[nodiscard]] bool stringCompare(std::string_view lhs, std::string_view rhs) noexcept;

// Constructs and validates a standard calendar date
[[nodiscard]] constexpr auto makeDate(int year, int month, int day) noexcept
    -> std::optional<std::chrono::year_month_day> {
  if (month < 1 || month > 12 || day < 1 || day > 31) {
    return std::nullopt;
  }
  const std::chrono::year_month_day ymd{
      std::chrono::year{year},
      std::chrono::month{static_cast<unsigned>(month)},
      std::chrono::day{static_cast<unsigned>(day)}};

  return ymd.ok() ? std::optional{ymd} : std::nullopt;
}

// Parses string dates formatted as YYYY-MM-DD, YYYY/MM/DD, or YYYY MM DD
[[nodiscard]] auto parseDateString(const std::string &str)
    -> std::optional<std::chrono::year_month_day>;

// Parses string representation into a Gender enum
[[nodiscard]] auto parseGender(std::string_view str) noexcept -> Gender;

// Parses string representation into a TrainingStatus enum
[[nodiscard]] auto parseTrainingStatus(std::string_view str) noexcept -> TrainingStatus;

// Continent validation and bidirectional index conversion
[[nodiscard]] auto isValidContinent(std::string_view continent) noexcept -> std::optional<std::string_view>;
[[nodiscard]] auto encodeContinentValue(std::string_view continent) noexcept -> std::optional<std::size_t>;
[[nodiscard]] auto decodeContinentValue(std::size_t index) noexcept -> std::string_view;

// Reusable CLI input prompts that handle validation and errors internally
auto promptForString(std::string_view prompt) -> std::string;
auto promptForDate(std::string_view prompt) -> std::chrono::year_month_day;
auto promptForPositiveDouble(std::string_view prompt, std::string_view errorMsg) -> double;
auto promptForBool(std::string_view prompt) -> bool;
auto promptForContinent(std::string_view prompt, bool allowNone = false) -> std::optional<std::string>;
auto promptForMonkeySpecies(std::string_view prompt) -> std::string;

} // namespace RescueUtils