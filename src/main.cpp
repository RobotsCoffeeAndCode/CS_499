#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "Dog.hpp"
#include "Monkey.hpp"
#include "RescueAnimal.hpp"
#include "RescueUtils.hpp"
#include "ContinentGraph.hpp"

// This ensures I don't have to prefix every call from RescueUtils
// with RescueUtils::
using namespace RescueUtils;

// ============================================================================
// Core Application Functions
// ============================================================================

void displayMenu() {
  std::cout << "\n\n";
  std::cout << "\t\t\t\tRescue Animal System Menu\n";
  std::cout << "[1] Intake a new rescue animal\n";
  std::cout << "[2] Reserve an animal\n";
  std::cout << "[3] Print a list of all dogs\n";
  std::cout << "[4] Print a list of all monkeys\n";
  std::cout << "[5] Print a list of all animals that are not reserved\n";
  std::cout << "[6] Print a list of all animals\n";
  std::cout << "[q] Quit application\n\n";
  std::cout << "Enter a menu selection\n";
}

// Simple seeding function for dog data in the RescueServiceList
void initializeDogList(std::vector<std::unique_ptr<RescueAnimal>> &animalList) {
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

// Simple seeding function for monkey data in the RescueServiceList
void initializeMonkeyList(
    std::vector<std::unique_ptr<RescueAnimal>> &animalList) {
  animalList.push_back(std::make_unique<Monkey>(
      "Rafiki", Gender::Male, *makeDate(2018, 5, 14), 25.6,
      *makeDate(2019, 5, 14), "United States", TrainingStatus::Intake, false,
      "United States", 12.0, 180.0, 10.0, "Macaque"));

  animalList.push_back(std::make_unique<Monkey>(
      "Abu", Gender::Male, *makeDate(2018, 5, 14), 25.6, *makeDate(2019, 5, 14),
      "United States", TrainingStatus::InService, false, "United States", 12.0,
      180.0, 10.0, "Marmoset"));

  animalList.push_back(std::make_unique<Monkey>(
      "Harambe", Gender::Male, *makeDate(2018, 5, 14), 25.6,
      *makeDate(2019, 5, 14), "United States", TrainingStatus::InService, true,
      "United States", 12.0, 180.0, 10.0, "Squirrel"));
}

// Function to add a new dog object to the RescueAnimalServiceList 
// Side Effect: Will add a new Dog to the RescueAnimalServiceList
void intakeNewDog(std::vector<std::unique_ptr<RescueAnimal>> &animalList) {
  const std::string name = promptForString("What is the dog's name?");
  for (const auto &animal : animalList) {
    if (animal->getAnimalType() == "Dog" &&
        stringCompare(animal->getName(), name)) {
      std::cout << "\n\nThis dog is already in our system\n\n";
      return;
    }
  }

  const std::string breed = promptForString("What is the breed of the dog?");
  const Gender gender =
      parseGender(promptForString("What gender is the dog? (Male/Female)"));
  const auto birthDate = promptForDate("What is the birth date of the dog?");
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
      serviceCountry.empty() || stringCompare(serviceCountry, "none")
          ? std::nullopt
          : std::optional<std::string>{serviceCountry};

  animalList.push_back(
      std::make_unique<Dog>(name, breed, gender, birthDate, weight,
                            acquisitionDate, acquisitionCountry, trainingStatus,
                            reserved, std::move(inServiceCountry)));

  std::cout << name << " has been added to the database\n";
}

// Function to add a new monkey object to the RescueAnimalServiceList
// Side Effect: Will add a new monkey to the RescueAnimalServiceList
void intakeNewMonkey(std::vector<std::unique_ptr<RescueAnimal>> &animalList) {
  const std::string name = promptForString("What is the monkey's name?");
  for (const auto &animal : animalList) {
    if (animal->getAnimalType() == "Monkey" &&
        stringCompare(animal->getName(), name)) {
      std::cout << "\n\nThis monkey is already in our system\n\n";
      return;
    }
  }

  const Gender gender =
      parseGender(promptForString("What gender is the monkey? (Male/Female)"));
  const auto birthDate = promptForDate("What is the birth date of the monkey?");
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
      serviceCountry.empty() || stringCompare(serviceCountry, "none")
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
    const auto *match =
        std::ranges::find_if(kValidMonkeySpecies, [&](std::string_view valid) {
          return stringCompare(valid, species);
        });
    if (match != kValidMonkeySpecies.end()) {
      species = std::string(*match);
      break;
    }

    std::cout << "Please only enter a valid monkey species, here is a list "
                 "of valid species:\n";
    for (std::string_view validSpecies : kValidMonkeySpecies) {
      std::cout << '|' << std::left << std::setw(10) << validSpecies << "|\n";
    }
  }

  animalList.push_back(std::make_unique<Monkey>(
      name, gender, birthDate, weight, acquisitionDate, acquisitionCountry,
      trainingStatus, reserved, std::move(inServiceCountry), tailLength, height,
      bodyLength, species));

  std::cout << name << " has been added to the database\n";
}

// Asks the user the type of animal and calls its associated intake function
// Side Effect: Will add a new animal to the RescueAnimalServiceList
void intakeNewAnimal(std::vector<std::unique_ptr<RescueAnimal>> &animalList){
  while (true) {
    const std::string animalType = promptForString(
        "Please input the animal you would like to reserve (\"dog\" or "
        "\"monkey\")\nOr press q to quit");

    if (animalType == "q" || animalType == "Q") {
      return;
    }

    const bool isDog = stringCompare(animalType, "dog");
    const bool isMonkey = stringCompare(animalType, "monkey");

    if (!isDog && !isMonkey) {
      std::cout << "Please enter \"dog\" or \"monkey\" as they are the only "
                   "available rescue animals at this time\n";
      continue;
    }
  
    if(isDog){
      intakeNewDog(animalList);
    } else {
      intakeNewMonkey(animalList);
    }

  }
}

// Changes an unreserved animals status to reserved
// Side Effect: Will change the reserved type on an animal in RescueAnimalServiceList
void reserveAnimal(std::vector<std::unique_ptr<RescueAnimal>> &animalList) {
  while (true) {
    const std::string animalType = promptForString(
        "Please input the animal you would like to reserve (\"dog\" or "
        "\"monkey\")\nOr press q to quit");

    if (animalType == "q" || animalType == "Q") {
      return;
    }

    const bool isDog = stringCompare(animalType, "dog");
    const bool isMonkey = stringCompare(animalType, "monkey");

    if (!isDog && !isMonkey) {
      std::cout << "Please enter \"dog\" or \"monkey\" as they are the only "
                   "available rescue animals at this time\n";
      continue;
    }

    const std::string country =
        promptForString("Please input the country you would like service in");
    const std::string_view targetType = isDog ? "Dog" : "Monkey";

    RescueAnimal *selected = nullptr;
    for (const auto &animal : animalList) {
      if (animal->getAnimalType() == targetType && !animal->isReserved()) {
        const auto &inService = animal->getInServiceCountry();
        if (inService.has_value() && stringCompare(*inService, country)) {
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

// Takes in a listType (filter parameter) and prints the RescueAnimalServiceList 
// based on its value
// Side Effect: Prints filtered list output to the console
void printAnimals(
    std::string_view listType,
    const std::vector<std::unique_ptr<RescueAnimal>> &animalList) {
  std::cout
      << "|    Name    | Training Status | Country Acquired | Reserved |\n";
  std::cout
      << "--------------------------------------------------------------\n";

  auto printRow = [](const RescueAnimal &animal) {
    std::cout << '|' << std::left << std::setw(12) << animal.getName() << '|'
              << std::left << std::setw(17) << animal.getTrainingStatus() << '|'
              << std::left << std::setw(18) << animal.getAcquisitionCountry()
              << '|' << std::left << std::setw(10)
              << (animal.isReserved() ? "true" : "false") << "|\n";
  };

  if (stringCompare(listType, "dog")) {
    for (const auto &animal : animalList) {
      if (animal->getAnimalType() == "Dog") {
        printRow(*animal);
      }
    }
  } else if (stringCompare(listType, "monkey")) {
    for (const auto &animal : animalList) {
      if (animal->getAnimalType() == "Monkey") {
        printRow(*animal);
      }
    }
  } else if (stringCompare(listType, "available")) {
    for (const auto &animal : animalList) {
      if (!animal->isReserved() &&
          animal->getTrainingStatus() == "In Service") {
        printRow(*animal);
      }
    }
  } else if (stringCompare(listType, "all")){
    for (const auto &animal : animalList) {
        printRow(*animal);
    }
  }
}

// Generates and returns the bi-directional weighted graph representation
[[nodiscard]] auto generateContinentGraph() -> graph::ContinentGraph {

  // generate a graph with seven vertices / nodes
  graph::ContinentGraph graph(7);

  // All dollar amounts below are calculated by multiplying
  // $0.18 times the miles between the continents geographical
  // centers rounded to the nearest 100th

  // 0 = Africa
  // The first line below sets the cost to travel 
  // from 0 (Africa) to 1 (Antarctica) to $1,100
  graph.addEdge(0, 1, 1200);
  graph.addEdge(0, 2, 930);
  graph.addEdge(0, 3, 1400);
  graph.addEdge(0, 4, 650);

  // 1 = Antarctica
  graph.addEdge(1, 3, 800);
  graph.addEdge(1, 6, 900);

  // 2 = Asia
  graph.addEdge(2, 3, 1000);
  graph.addEdge(2, 4, 500);
  graph.addEdge(2, 5, 1100);

  // 3 = Australia
  graph.addEdge(3, 5, 1600);
  graph.addEdge(3, 6, 1700);

  // 4 = Europe
  graph.addEdge(4, 5, 840);

  // 5 = North America
  graph.addEdge(5, 6, 930);

  // 6 = South America (already defined)

  return graph;

}

// Entry point of program execution
int main() {
  std::vector<std::unique_ptr<RescueAnimal>> rescueAnimalList;

  // TODO change this to one seeding function that generates random animals
  initializeDogList(rescueAnimalList);
  initializeMonkeyList(rescueAnimalList);

  // Make the continent graph data structure (see function above)
  graph::ContinentGraph continentGraph = generateContinentGraph();


  std::cout << "Welcome to Grazioso Salvare.\n";

  continentGraph.printGraph();

  std::string userInput;

  while (true) {

    displayMenu();
    std::getline(std::cin, userInput);

    if (userInput.empty()) {
      continue;
    }

    if (!stringCompare(userInput, "q")) {
      try {
        std::size_t idx = 0;
        const int selection = std::stoi(userInput, &idx);

        if (idx == userInput.length() && selection >= 1 && selection <= 6) {
          switch (selection) {
          case 1:
            intakeNewAnimal(rescueAnimalList);
            break;
          case 2:
            reserveAnimal(rescueAnimalList);
            break;
          case 3:
            printAnimals("dog", rescueAnimalList);
            break;
          case 4:
            printAnimals("monkey", rescueAnimalList);
            break;
          case 5:
            printAnimals("available", rescueAnimalList);
            break;
          case 6:
            printAnimals("all", rescueAnimalList);
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

    if (stringCompare(userInput, "q")) {
      break;
    }
  }

  std::cout << "\n\nThank you for using our rescue animal services.\n";
  return 0;
}