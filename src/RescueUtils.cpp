#include "RescueUtils.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace RescueUtils {

// Compares two strings character-by-character ignoring case
bool stringCompare(std::string_view lhs, std::string_view rhs) noexcept {
  return std::ranges::equal(lhs, rhs, [](char a, char b) {
    return std::tolower(static_cast<unsigned char>(a)) ==
           std::tolower(static_cast<unsigned char>(b));
  });
}

// Parses string dates by converting common delimiters into spaces, then extracting integers
auto parseDateString(const std::string &str)
    -> std::optional<std::chrono::year_month_day> {
  std::string cleaned = str;
  for (char &ch : cleaned) {
    if (ch == '-' || ch == '/') {
      ch = ' ';
    }
  }

  std::istringstream iss{cleaned};
  int year = 0;
  int month = 0;
  int day = 0;
  if (iss >> year >> month >> day) {
    return makeDate(year, month, day);
  }
  return std::nullopt;
}

// Maps user string input to the corresponding Gender enum
auto parseGender(std::string_view str) noexcept -> Gender {
  if (stringCompare(str, "male"))   return Gender::Male;
  if (stringCompare(str, "female")) return Gender::Female;
  return Gender::Unknown;
}

// Maps user string input to the corresponding TrainingStatus enum
auto parseTrainingStatus(std::string_view str) noexcept -> TrainingStatus {
  if (stringCompare(str, "intake"))      return TrainingStatus::Intake;
  if (stringCompare(str, "in training") || stringCompare(str, "intraining")) return TrainingStatus::InTraining;
  if (stringCompare(str, "phase i")     || stringCompare(str, "phase 1"))   return TrainingStatus::PhaseI;
  if (stringCompare(str, "phase ii")    || stringCompare(str, "phase 2"))   return TrainingStatus::PhaseII;
  if (stringCompare(str, "phase iii")   || stringCompare(str, "phase 3"))   return TrainingStatus::PhaseIII;
  if (stringCompare(str, "in service")  || stringCompare(str, "inservice"))  return TrainingStatus::InService;
  if (stringCompare(str, "retired"))     return TrainingStatus::Retired;
  return TrainingStatus::Intake;
}

// Checks if the given continent name matches our permitted list
auto isValidContinent(std::string_view continent) noexcept
    -> std::optional<std::string_view> {
  const auto it = std::ranges::find_if(kValidContinents, [&](std::string_view c) {
    return stringCompare(c, continent);
  });
  return (it != kValidContinents.end()) ? std::optional{*it} : std::nullopt;
}

// Converts a continent name to its corresponding graph node index
auto encodeContinentValue(std::string_view continent) noexcept
    -> std::optional<std::size_t> {
  for (std::size_t i = 0; i < kValidContinents.size(); ++i) {
    if (stringCompare(kValidContinents[i], continent)) {
      return i;
    }
  }
  return std::nullopt;
}

// Converts a graph node index back to its human-readable continent name
auto decodeContinentValue(std::size_t index) noexcept -> std::string_view {
  if (index < kValidContinents.size()) {
    return kValidContinents[index];
  }
  return "Unknown";
}

// Displays a prompt and reads a line of text from standard input
auto promptForString(std::string_view prompt) -> std::string {
  std::cout << prompt << '\n';
  std::string input;
  std::getline(std::cin, input);
  return input;
}

// Repeatedly prompts until the user enters a valid YYYY-MM-DD date
auto promptForDate(std::string_view prompt) -> std::chrono::year_month_day {
  while (true) {
    std::cout << prompt << " (YYYY-MM-DD): ";
    std::string line;
    std::getline(std::cin, line);
    if (auto date = parseDateString(line); date.has_value()) {
      return *date;
    }
    std::cout << "Invalid date format. Please use YYYY-MM-DD.\n";
  }
}

// Prompts for a non-negative decimal value and checks for invalid input characters
auto promptForPositiveDouble(std::string_view prompt, std::string_view errorMsg)
    -> double {
  while (true) {
    std::string str = promptForString(prompt);
    try {
      std::size_t idx = 0;
      double val = std::stod(str, &idx);
      // Ensure the whole string was consumed (ignoring trailing whitespace)
      if (idx == str.find_last_not_of(" \t\r\n") + 1 && val >= 0.0) {
        return val;
      }
    } catch (...) {
      // Input conversion failed, will print errorMsg below
    }
    std::cout << errorMsg << '\n';
  }
}

// Prompts for a boolean value (accepts true, t, yes, y)
auto promptForBool(std::string_view prompt) -> bool {
  std::cout << prompt << '\n';
  std::string line;
  std::getline(std::cin, line);
  return stringCompare(line, "true") || stringCompare(line, "t") ||
         stringCompare(line, "yes")  || stringCompare(line, "y");
}

// Prompts for a continent and re-prompts showing valid options until valid input is given
auto promptForContinent(std::string_view prompt, bool allowNone) -> std::optional<std::string> {
  while (true) {
    std::string input = promptForString(prompt);
    if (allowNone && (input.empty() || stringCompare(input, "none"))) {
      return std::nullopt;
    }

    if (auto match = isValidContinent(input); match.has_value()) {
      return std::string(*match);
    }

    std::cout << "Invalid continent. Permitted values:\n";
    for (std::string_view continent : kValidContinents) {
      std::cout << " - " << continent << '\n';
    }
  }
}

// Prompts for a monkey species and ensures it matches our accepted species list
auto promptForMonkeySpecies(std::string_view prompt) -> std::string {
  while (true) {
    std::string species = promptForString(prompt);
    const auto it = std::ranges::find_if(kValidMonkeySpecies, [&](std::string_view item) {
      return stringCompare(item, species);
    });

    if (it != kValidMonkeySpecies.end()) {
      return std::string(*it);
    }

    std::cout << "Invalid species. Permitted species:\n";
    for (std::string_view valid : kValidMonkeySpecies) {
      std::cout << " - " << valid << '\n';
    }
  }
}

} // namespace RescueUtils