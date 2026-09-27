#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "Dog.hpp"
#include "Monkey.hpp"
#include "RescueAnimal.hpp"

// ============================================================================
// Helper Utilities & Validation
// ============================================================================

constexpr std::array<std::string_view, 6> kValidMonkeySpecies{
    "Capuchin", "Guenon", "Macaque", "Marmoset", "Squirrel", "Tamarin"};

// Case-insensitive string comparison
[[nodiscard]] bool iequals(std::string_view lhs,
                           std::string_view rhs) noexcept {
  return std::ranges::equal(lhs, rhs, [](char first, char second) {
    return std::tolower(static_cast<unsigned char>(first)) ==
           std::tolower(static_cast<unsigned char>(second));
  });
}

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

// Parses string representation into Gender enum
[[nodiscard]] auto parseGender(std::string_view str) -> Gender {
  if (iequals(str, "male")) {
    return Gender::Male;
  }
  if (iequals(str, "female")) {
    return Gender::Female;
  }
  return Gender::Unknown;
}

// Parses string representation into TrainingStatus enum
[[nodiscard]] auto parseTrainingStatus(std::string_view str) -> TrainingStatus {
  if (iequals(str, "intake")) {
    return TrainingStatus::Intake;
  }
  if (iequals(str, "in training") || iequals(str, "intraining")) {
    return TrainingStatus::InTraining;
  }
  if (iequals(str, "phase i") || iequals(str, "phasei") ||
      iequals(str, "phase 1")) {
    return TrainingStatus::PhaseI;
  }
  if (iequals(str, "phase ii") || iequals(str, "phaseii") ||
      iequals(str, "phase 2")) {
    return TrainingStatus::PhaseII;
  }
  if (iequals(str, "phase iii") || iequals(str, "phaseiii") ||
      iequals(str, "phase 3")) {
    return TrainingStatus::PhaseIII;
  }
  if (iequals(str, "in service") || iequals(str, "inservice")) {
    return TrainingStatus::InService;
  }
  if (iequals(str, "retired")) {
    return TrainingStatus::Retired;
  }
  return TrainingStatus::Intake;
}

// ============================================================================
// Input Prompts
// ============================================================================

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
    }
    std::cout << errorMsg << '\n';
  }
}

auto promptForBool(std::string_view prompt) -> bool {
  std::cout << prompt << '\n';
  std::string line;
  std::getline(std::cin, line);
  return iequals(line, "true");
}

// ============================================================================
// Core Application Functions
// ============================================================================

void displayMenu() {
  std::cout << "\n\n";
  std::cout << "\t\t\t\tRescue Animal System Menu\n";
  std::cout << "[1] Intake a new dog\n";
  std::cout << "[2] Intake a new monkey\n";
  std::cout << "[3] Reserve an animal\n";
  std::cout << "[4] Print a list of all dogs\n";
  std::cout << "[5] Print a list of all monkeys\n";
  std::cout << "[6] Print a list of all animals that are not reserved\n";
  std::cout << "[q] Quit application\n\n";
  std::cout << "Enter a menu selection\n";
}

void initializeDogList(
    std::vector<std::unique_ptr<RescueAnimal>> &animalList) {
  animalList.push_back(std::make_unique<Dog>(
      "Spot", "German Shepherd", Gender::Male, *makeDate(2018, 5, 12), 25.6,
      *makeDate(2019, 5, 12), "United States", TrainingStatus::Intake, false,
      "United States"));

  animalList.push_back(std::make_unique<Dog>(
      "Rex", "Great Dane", Gender::Male, *makeDate(2017, 2, 3), 35.2,
      *makeDate(2020, 2, 3), "United States", TrainingStatus::PhaseI, false,
      "United States"));

  animalList.push_back(std::make_unique<Dog>(
      "Bella", "Chihuahua", Gender::Female, *makeDate(2015, 12, 12), 25.6,
      *makeDate(2019, 12, 12), "Canada", TrainingStatus::InService, true,
      "Canada"));

  animalList.push_back(std::make_unique<Dog>(
      "Chad", "Chihuahua", Gender::Male, *makeDate(2015, 12, 12), 25.6,
      *makeDate(2019, 12, 12), "Canada", TrainingStatus::InService, false,
      "Canada"));
}

void initializeMonkeyList(
    std::vector<std::unique_ptr<RescueAnimal>> &animalList) {
  animalList.push_back(std::make_unique<Monkey>(
      "Rafiki", Gender::Male, *makeDate(2018, 5, 14), 25.6,
      *makeDate(2019, 5, 14), "United States", TrainingStatus::Intake, false,
      "United States", 12.0, 180.0, 10.0, "Macaque"));

  animalList.push_back(std::make_unique<Monkey>(
      "Abu", Gender::Male, *makeDate(2018, 5, 14), 25.6,
      *makeDate(2019, 5, 14), "United States", TrainingStatus::InService, false,
      "United States", 12.0, 180.0, 10.0, "Marmoset"));

  animalList.push_back(std::make_unique<Monkey>(
      "Harambe", Gender::Male, *makeDate(2018, 5, 14), 25.6,
      *makeDate(2019, 5, 14), "United States", TrainingStatus::InService, true,
      "United States", 12.0, 180.0, 10.0, "Squirrel"));
}

void intakeNewDog(std::vector<std::unique_ptr<RescueAnimal>> &animalList) {
  const std::string name = promptForString("What is the dog's name?");
  for (const auto &animal : animalList) {
    if (animal->getAnimalType() == "Dog" &&
        iequals(animal->getName(), name)) {
      std::cout << "\n\nThis dog is already in our system\n\n";
      return;
    }
  }

  const std::string breed =
      promptForString("What is the breed of the dog?");
  const Gender gender =
      parseGender(promptForString("What gender is the dog? (Male/Female)"));
  const auto birthDate =
      promptForDate("What is the birth date of the dog?");
  const double weight = promptForPositiveDouble(
      "What is the weight of the dog?", "Please enter a valid weight.");
  const auto acquisitionDate =
      promptForDate("What is the acquisition date of the dog?");
  const std::string acquisitionCountry =
      promptForString("What is the acquisition country of the dog?");
  const TrainingStatus trainingStatus = parseTrainingStatus(
      promptForString("What is the training status of the dog?"));
  const bool reserved = promptForBool(
      "What is the reservation status of the dog? (true or false)");

  const std::string serviceCountry =
      promptForString("What is the service country of the dog?");
  std::optional<std::string> inServiceCountry =
      serviceCountry.empty() || iequals(serviceCountry, "none")
          ? std::nullopt
          : std::optional<std::string>{serviceCountry};

  animalList.push_back(std::make_unique<Dog>(
      name, breed, gender, birthDate, weight, acquisitionDate,
      acquisitionCountry, trainingStatus, reserved, std::move(inServiceCountry)));

  std::cout << name << " has been added to the database\n";
}

void intakeNewMonkey(
    std::vector<std::unique_ptr<RescueAnimal>> &animalList) {
  const std::string name = promptForString("What is the monkey's name?");
  for (const auto &animal : animalList) {
    if (animal->getAnimalType() == "Monkey" &&
        iequals(animal->getName(), name)) {
      std::cout << "\n\nThis monkey is already in our system\n\n";
      return;
    }
  }

  const Gender gender =
      parseGender(promptForString("What gender is the monkey? (Male/Female)"));
  const auto birthDate =
      promptForDate("What is the birth date of the monkey?");
  const double weight = promptForPositiveDouble(
      "What is the weight of the monkey?", "Please enter a valid weight.");
  const auto acquisitionDate =
      promptForDate("What is the acquisition date of the monkey?");
  const std::string acquisitionCountry =
      promptForString("What is the acquisition country of the monkey?");
  const TrainingStatus trainingStatus = parseTrainingStatus(
      promptForString("What is the training status of the monkey?"));
  const bool reserved = promptForBool(
      "What is the reservation status of the monkey? (true or false)");

  const std::string serviceCountry =
      promptForString("What is the service country of the monkey?");
  std::optional<std::string> inServiceCountry =
      serviceCountry.empty() || iequals(serviceCountry, "none")
          ? std::nullopt
          : std::optional<std::string>{serviceCountry};

  const double tailLength = promptForPositiveDouble(
      "What is the length of the monkey's tail?",
      "Please enter the tail length as a positive decimal number ex) 5.43");
  const double height = promptForPositiveDouble(
      "What is the height of the monkey?",
      "Please enter the height as a positive decimal number ex) 5.43");
  const double bodyLength = promptForPositiveDouble(
      "What is the body_length of the monkey?",
      "Please enter the body_length as a positive decimal number ex) 5.43");

  std::string species;
  while (true) {
    species = promptForString("What species of monkey is it?");
    auto match = std::ranges::find_if(
        kValidMonkeySpecies, [&](std::string_view valid) {
          return iequals(valid, species);
        });
    if (match != kValidMonkeySpecies.end()) {
      species = std::string(*match);
      break;
    }

    std::cout << "Please only enter a valid monkey species, here is a list "
                 "of valid species:\n";
    for (std::string_view s : kValidMonkeySpecies) {
      std::cout << '|' << std::left << std::setw(10) << s << "|\n";
    }
  }

  animalList.push_back(std::make_unique<Monkey>(
      name, gender, birthDate, weight, acquisitionDate, acquisitionCountry,
      trainingStatus, reserved, std::move(inServiceCountry), tailLength,
      height, bodyLength, species));

  std::cout << name << " has been added to the database\n";
}

void reserveAnimal(
    std::vector<std::unique_ptr<RescueAnimal>> &animalList) {
  while (true) {
    const std::string animalType = promptForString(
        "Please input the animal you would like to reserve (\"dog\" or "
        "\"monkey\")\nOr press q to quit");

    if (animalType == "q" || animalType == "Q") {
      return;
    }

    const bool isDog = iequals(animalType, "dog");
    const bool isMonkey = iequals(animalType, "monkey");

    if (!isDog && !isMonkey) {
      std::cout << "Please enter \"dog\" or \"monkey\" as they are the only "
                   "available rescue animals at this time\n";
      continue;
    }

    const std::string country = promptForString(
        "Please input the country you would like service in");
    const std::string_view targetType = isDog ? "Dog" : "Monkey";

    RescueAnimal *selected = nullptr;
    for (const auto &animal : animalList) {
      if (animal->getAnimalType() == targetType && !animal->isReserved()) {
        const auto &inService = animal->getInServiceCountry();
        if (inService.has_value() && iequals(*inService, country)) {
          selected = animal.get();
          break;
        }
      }
    }

    if (selected != nullptr) {
      selected->setReserved(true);
      std::cout << "We have changed the reservation status of "
                << selected->getName() << ".\n";
      return;
    }

    std::cout << "We currently have no rescue animals in " << country
              << ", Sorry for the inconvience!\n";
  }
}

void printAnimals(
    std::string_view listType,
    const std::vector<std::unique_ptr<RescueAnimal>> &animalList) {
  std::cout << "|    Name    | Training Status | Country Acquired | Reserved |\n";
  std::cout << "--------------------------------------------------------------\n";

  auto printRow = [](const RescueAnimal &animal) {
    std::cout << '|' << std::left << std::setw(12) << animal.getName()
              << '|' << std::left << std::setw(17)
              << animal.getTrainingStatus()
              << '|' << std::left << std::setw(18)
              << animal.getAcquisitionCountry()
              << '|' << std::left << std::setw(10)
              << (animal.isReserved() ? "true" : "false")
              << "|\n";
  };

  if (iequals(listType, "dog")) {
    for (const auto &animal : animalList) {
      if (animal->getAnimalType() == "Dog") {
        printRow(*animal);
      }
    }
  } else if (iequals(listType, "monkey")) {
    for (const auto &animal : animalList) {
      if (animal->getAnimalType() == "Monkey") {
        printRow(*animal);
      }
    }
  } else if (iequals(listType, "available")) {
    for (const auto &animal : animalList) {
      if (!animal->isReserved() &&
          animal->getTrainingStatus() == "In Service") {
        printRow(*animal);
      }
    }
  }
}

// ============================================================================
// Main Application Loop
// ============================================================================

int main() {
  std::vector<std::unique_ptr<RescueAnimal>> rescueAnimalList;

  initializeDogList(rescueAnimalList);
  initializeMonkeyList(rescueAnimalList);

  std::cout << "Welcome to Grazioso Salvare.\n";

  std::string userInput;
  do {
    displayMenu();
    std::getline(std::cin, userInput);

    if (userInput.empty()) {
      continue;
    }

    if (!iequals(userInput, "q")) {
      try {
        std::size_t idx = 0;
        const int selection = std::stoi(userInput, &idx);

        if (idx == userInput.length() && selection >= 1 && selection <= 6) {
          switch (selection) {
          case 1:
            intakeNewDog(rescueAnimalList);
            break;
          case 2:
            intakeNewMonkey(rescueAnimalList);
            break;
          case 3:
            reserveAnimal(rescueAnimalList);
            break;
          case 4:
            printAnimals("dog", rescueAnimalList);
            break;
          case 5:
            printAnimals("monkey", rescueAnimalList);
            break;
          case 6:
            printAnimals("available", rescueAnimalList);
            break;
          default:
            break;
          }
        } else {
          std::cout << "The inputted number must be in the range 1-6\n";
        }
      } catch (const std::invalid_argument &) {
        std::cout
            << "Please only enter an integer or the letter \"q\" or \"Q\"\n";
      } catch (const std::out_of_range &) {
        std::cout << "The inputted number must be in the range 1-6\n";
      }
    }
  } while (!iequals(userInput, "q"));

  std::cout << "\n\nThank you for using our animal rescue services.\n";
  return 0;
}