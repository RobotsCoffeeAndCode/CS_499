#include "RescueUtils.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <sstream>

namespace RescueUtils {

bool stringCompare(std::string_view lhs, std::string_view rhs) noexcept {
  return std::ranges::equal(lhs, rhs, [](char first, char second) {
    return std::tolower(static_cast<unsigned char>(first)) ==
           std::tolower(static_cast<unsigned char>(second));
  });
}

auto parseDateString(const std::string &str)
    -> std::optional<std::chrono::year_month_day> {
  std::string cleaned = str;
  for (char &separator : cleaned) {
    if (separator == '-' || separator == '/') {
      separator = ' ';
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

auto parseGender(std::string_view str) noexcept -> Gender {
  if (stringCompare(str, "male")) {
    return Gender::Male;
  }
  if (stringCompare(str, "female")) {
    return Gender::Female;
  }
  return Gender::Unknown;
}

auto parseTrainingStatus(std::string_view str) noexcept -> TrainingStatus {
  if (stringCompare(str, "intake")) {
    return TrainingStatus::Intake;
  }
  if (stringCompare(str, "in training") || stringCompare(str, "intraining")) {
    return TrainingStatus::InTraining;
  }
  if (stringCompare(str, "phase i") || stringCompare(str, "phasei") ||
      stringCompare(str, "phase 1")) {
    return TrainingStatus::PhaseI;
  }
  if (stringCompare(str, "phase ii") || stringCompare(str, "phaseii") ||
      stringCompare(str, "phase 2")) {
    return TrainingStatus::PhaseII;
  }
  if (stringCompare(str, "phase iii") || stringCompare(str, "phaseiii") ||
      stringCompare(str, "phase 3")) {
    return TrainingStatus::PhaseIII;
  }
  if (stringCompare(str, "in service") || stringCompare(str, "inservice")) {
    return TrainingStatus::InService;
  }
  if (stringCompare(str, "retired")) {
    return TrainingStatus::Retired;
  }
  return TrainingStatus::Intake;
}

auto isValidMonkeySpecies(std::string_view species) noexcept
    -> std::optional<std::string_view> {
  const auto *match = std::ranges::find_if(
      kValidMonkeySpecies,
      [&](std::string_view valid) { return stringCompare(valid, species); });

  if (match != kValidMonkeySpecies.end()) {
    return *match;
  }
  return std::nullopt;
}

auto promptForString(std::string_view prompt) -> std::string {
  std::cout << prompt << '\n';
  std::string input;
  std::getline(std::cin, input);
  return input;
}

auto promptForDate(std::string_view prompt) -> std::chrono::year_month_day {
  while (true) {
    std::cout << prompt << " (YYYY-MM-DD): ";
    std::string line;
    std::getline(std::cin, line);
    if (auto date = parseDateString(line); date.has_value()) {
      return *date;
    }
    std::cout << "Invalid date. Please use format YYYY-MM-DD.\n";
  }
}

auto promptForPositiveDouble(std::string_view prompt,
                            std::string_view errorMsg) -> double {
  while (true) {
    std::string str = promptForString(prompt);
    try {
      std::size_t idx = 0;
      double val = std::stod(str, &idx);
      if (idx == str.find_last_not_of(" \t\r\n") + 1) {
        return std::abs(val);
      }
    } catch (...) {
        std::cout << "There was a problem in promtForPositiveDouble" << '\n';
    }
    std::cout << errorMsg << '\n';
  }
}

auto promptForBool(std::string_view prompt) -> bool {
  std::cout << prompt << '\n';
  std::string line;
  std::getline(std::cin, line);
  return stringCompare(line, "true");
}

} // namespace RescueUtils