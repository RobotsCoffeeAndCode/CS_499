#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "ContinentGraph.hpp"
#include "Dijkstra.hpp"
#include "Dog.hpp"
#include "Monkey.hpp"
#include "RescueAnimal.hpp"
#include "RescueUtils.hpp"
#include "SupabaseClient.hpp"

using namespace RescueUtils;

// Default API credentials (can be overridden at runtime via environment variables)
static const std::string kDefaultUrl = "https://ldgztfeciviygfaiqwtr.supabase.co";
static const std::string kDefaultKey =
    "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9."
    "eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImxkZ3p0ZmVjaXZpeWdmYWlxd3RyIiwicm9sZSI6Im"
    "Fub24iLCJpYXQiOjE3OTE1NzMyODQsImV4cCI6MjEwNzE0OTI4NH0.ZOnUir1_"
    "t0e8kl9IWJbIMr939hn3YTnr6OayM5RLpFw";

// Reads an environment variable or returns the provided default fallback
static auto getEnvOrDefault(const char *varName, const std::string &fallback) -> std::string {
  if (const char *val = std::getenv(varName)) {
    return std::string{val};
  }
  return fallback;
}

// Presents the initial login/registration menu to the user
bool handleUserLogin(auth::SupabaseClient &client) {
  while (true) {
    std::cout << "\n=== Grazioso Salvare Authentication ===\n"
              << "[1] Sign In\n"
              << "[2] Sign Up (New Account)\n"
              << "[q] Exit\n";

    std::string choice = promptForString("Select an option:");
    if (stringCompare(choice, "q")) {
      return false;
    }

    if (choice == "1" || choice == "2") {
      std::string email = promptForString("Enter email:");
      const std::string password = promptForString("Enter password:");

      // Internal convenience: convert shorthand admin username to full email
      if (email == "admin") {
        email = "admin@grazioso.local";
      }

      if (choice == "1") {
        if (client.signIn(email, password)) {
          std::cout << "Login successful. Welcome back!\n";
          return true;
        }
        std::cout << "Invalid credentials. Please try again.\n";
      } else {
        if (client.signUp(email, password)) {
          std::cout << "Registration complete. Please sign in.\n";
        } else {
          std::cout << "Sign up failed. Please check password requirements.\n";
        }
      }
    }
  }
}

// Displays the primary application menu options
void displayMenu() {
  std::cout << "\n\n"
            << "\t\t\t\tRescue Animal System Menu\n"
            << "[1] Intake a new rescue animal\n"
            << "[2] Reserve an animal\n"
            << "[3] Move an animal\n"
            << "[4] Print a list of all dogs\n"
            << "[5] Print a list of all monkeys\n"
            << "[6] Print a list of all active animals that are not reserved\n"
            << "[7] Print a list of all animals\n"
            << "[q] Quit application\n\n"
            << "Enter a menu selection: ";
}

// Prints a formatted table of animals filtered by category
void printAnimals(
    std::string_view listType,
    const std::vector<std::unique_ptr<RescueAnimal>> &animalList) {
  std::cout << "|    Name    | Training Status | Service Continent | Reserved | Animal Type |\n"
            << "-----------------------------------------------------------------------------\n";

  auto printRow = [](const RescueAnimal &animal) {
    std::cout << '|' << std::left << std::setw(12) << animal.getName() << '|'
              << std::left << std::setw(17) << animal.getTrainingStatus() << '|'
              << std::left << std::setw(19)
              << (animal.getInServiceContinent().has_value()
                      ? *animal.getInServiceContinent()
                      : "N / A")
              << '|' << std::left << std::setw(10)
              << (animal.isReserved() ? "true" : "false") << '|'
              << std::left << std::setw(13) << animal.getAnimalType() << "|\n";
  };

  for (const auto &animal : animalList) {
    if (listType == "dog" && animal->getAnimalType() != "Dog") continue;
    if (listType == "monkey" && animal->getAnimalType() != "Monkey") continue;
    if (listType == "available" && (animal->isReserved() || animal->getTrainingStatus() != "In Service")) continue;
    if (listType == "reserved" && (!animal->isReserved() || animal->getTrainingStatus() != "In Service")) continue;
    printRow(*animal);
  }
}

// Temporary struct to bundle common attributes and avoid duplicate code in dog/monkey prompts
struct AnimalIntakeCommon {
  std::string name;
  Gender gender;
  std::chrono::year_month_day birthDate;
  double weightKg;
  std::chrono::year_month_day acquisitionDate;
  std::string acquisitionContinent;
  TrainingStatus trainingStatus;
  bool reserved;
  std::optional<std::string> inServiceContinent;
};

// Prompts for attributes shared by all animals and verifies uniqueness of name
static std::optional<AnimalIntakeCommon>
promptCommonAttributes(std::string_view animalType,
                       const std::vector<std::unique_ptr<RescueAnimal>> &animalList) {
  const std::string name = promptForString(std::string{"What is the "} + std::string{animalType} + "'s name?");

  for (const auto &animal : animalList) {
    if (animal->getAnimalType() == animalType && stringCompare(animal->getName(), name)) {
      std::cout << "\n\nThis " << animalType << " is already in our system\n\n";
      return std::nullopt;
    }
  }

  Gender gender = parseGender(promptForString("What gender? (Male/Female)"));
  auto birthDate = promptForDate("What is the birth date?");
  double weight = promptForPositiveDouble("What is the weight in kg?", "Please enter a positive decimal.");
  auto acqDate = promptForDate("What is the acquisition date?");
  
  std::string acqContinent = *promptForContinent("What continent was the animal acquired on?", false);
  TrainingStatus status = parseTrainingStatus(promptForString("What is the training status?"));
  bool reserved = promptForBool("Is the animal reserved? (true/false)");
  auto serviceContinent = promptForContinent("What continent should it start service in? (or 'none')", true);

  return AnimalIntakeCommon{
      .name = name,
      .gender = gender,
      .birthDate = birthDate,
      .weightKg = weight,
      .acquisitionDate = acqDate,
      .acquisitionContinent = std::move(acqContinent),
      .trainingStatus = status,
      .reserved = reserved,
      .inServiceContinent = std::move(serviceContinent)};
}

// Handles data intake and instantiation for a new Dog
void intakeNewDog(std::vector<std::unique_ptr<RescueAnimal>> &animalList,
                  const auth::SupabaseClient &client) {
  auto common = promptCommonAttributes("Dog", animalList);
  if (!common.has_value()) {
    return;
  }

  const std::string breed = promptForString("What is the breed of the dog?");

  auto newDog = std::make_unique<Dog>(
      common->name, breed, common->gender, common->birthDate, common->weightKg,
      common->acquisitionDate, common->acquisitionContinent,
      common->trainingStatus, common->reserved, common->inServiceContinent);

  if (client.saveAnimal(*newDog)) {
    std::cout << common->name << " has been saved to the database.\n";
    animalList.push_back(std::move(newDog));
  } else {
    std::cout << "Error: Failed to save " << common->name << " to the database.\n";
  }
}

// Handles data intake and instantiation for a new Monkey
void intakeNewMonkey(std::vector<std::unique_ptr<RescueAnimal>> &animalList,
                     const auth::SupabaseClient &client) {
  auto common = promptCommonAttributes("Monkey", animalList);
  if (!common.has_value()) {
    return;
  }

  double tailLength = promptForPositiveDouble("What is the monkey's tail length?", "Enter a positive number.");
  double height = promptForPositiveDouble("What is the monkey's height?", "Enter a positive number.");
  double bodyLength = promptForPositiveDouble("What is the monkey's body length?", "Enter a positive number.");
  std::string species = promptForMonkeySpecies("What species is the monkey?");

  auto newMonkey = std::make_unique<Monkey>(
      common->name, common->gender, common->birthDate, common->weightKg,
      common->acquisitionDate, common->acquisitionContinent,
      common->trainingStatus, common->reserved, common->inServiceContinent,
      tailLength, height, bodyLength, std::move(species));

  if (client.saveAnimal(*newMonkey)) {
    std::cout << common->name << " has been saved to the database.\n";
    animalList.push_back(std::move(newMonkey));
  } else {
    std::cout << "Error: Failed to save " << common->name << " to the database.\n";
  }
}

// Routes animal intake to Dog or Monkey routines based on user selection
void intakeNewAnimal(std::vector<std::unique_ptr<RescueAnimal>> &animalList,
                     const auth::SupabaseClient &client) {
  while (true) {
    const std::string choice = promptForString(
        "Enter animal type to intake (\"dog\" or \"monkey\"), or 'q' to return:");

    if (stringCompare(choice, "q")) {
      return;
    }

    if (stringCompare(choice, "dog")) {
      intakeNewDog(animalList, client);
      return;
    } else if (stringCompare(choice, "monkey")) {
      intakeNewMonkey(animalList, client);
      return;
    }

    std::cout << "Unsupported species. Please select 'dog' or 'monkey'.\n";
  }
}

// Finds an available animal of the requested type in a specific continent and reserves it
void reserveAnimal(std::vector<std::unique_ptr<RescueAnimal>> &animalList,
                   const auth::SupabaseClient &client) {
  while (true) {
    const std::string animalType = promptForString(
        "Enter animal type to reserve (\"dog\" or \"monkey\"), or 'q' to return:");

    if (stringCompare(animalType, "q")) {
      return;
    }

    const bool isDog = stringCompare(animalType, "dog");
    const bool isMonkey = stringCompare(animalType, "monkey");

    if (!isDog && !isMonkey) {
      std::cout << "Unsupported species. Please select 'dog' or 'monkey'.\n";
      continue;
    }

    const auto continentOpt = promptForContinent("Enter continent of service required:", false);
    if (!continentOpt.has_value()) {
      continue;
    }

    const std::string_view targetType = isDog ? "Dog" : "Monkey";
    RescueAnimal *selected = nullptr;

    for (const auto &animal : animalList) {
      if (animal->getAnimalType() == targetType && !animal->isReserved()) {
        const auto &inServiceContinent = animal->getInServiceContinent();
        if (inServiceContinent.has_value() && stringCompare(*inServiceContinent, *continentOpt)) {
          selected = animal.get();
          break;
        }
      }
    }

    if (selected != nullptr) {
      if (client.updateAnimalReservation(selected->getName(), true)) {
        selected->setReserved(true);
        std::cout << "Reserved " << selected->getName() << " successfully.\n";
      } else {
        std::cout << "Failed to persist reservation update to the database.\n";
      }
      return;
    }

    std::cout << "No unreserved " << targetType << " currently available in " << *continentOpt << ".\n";
  }
}

// Calculates shortest travel cost using Dijkstra's algorithm and updates an animal's location
void moveAnimal(std::vector<std::unique_ptr<RescueAnimal>> &animalList,
                const graph::ContinentGraph &graph,
                const auth::SupabaseClient &client) {
  RescueAnimal *selected = nullptr;

  // 1. Select the animal to move
  while (true) {
    printAnimals("reserved", animalList);
    std::string name = promptForString("Select an active reserved animal to move by name (or 'q' to cancel):");
    if (stringCompare(name, "q")) {
      return;
    }

    for (const auto &animal : animalList) {
      if (animal->getInServiceContinent().has_value() &&
          stringCompare(animal->getTrainingStatus(), "In Service") &&
          animal->isReserved() && 
          stringCompare(animal->getName(), name)) {
        selected = animal.get();
        break;
      }
    }

    if (selected != nullptr) {
      break;
    }
    std::cout << "Animal not found or not currently active and reserved. Try again.\n";
  }

  // 2. Select destination continent
  const auto destContinentOpt = promptForContinent("Enter destination continent:", false);
  if (!destContinentOpt.has_value()) {
    return;
  }
  const std::string &destination = *destContinentOpt;
  const std::string &current = *selected->getInServiceContinent();

  const auto startIdx = encodeContinentValue(current);
  const auto endIdx = encodeContinentValue(destination);

  if (!startIdx.has_value() || !endIdx.has_value()) {
    std::cerr << "Internal error: Invalid continent mapping index.\n";
    return;
  }

  // 3. Calculate shortest path transit cost using Dijkstra
  auto pathResult = graph::dijkstra(graph, *startIdx);
  int cost = pathResult.distances[*endIdx];

  if (cost == graph::DijkstraResult::infinity) {
    std::cout << "No travel route found between " << current << " and " << destination << ".\n";
    return;
  }

  std::cout << "Calculated transit cost for " << selected->getName() 
            << " from " << current << " to " << destination << " is $" << cost << ".\n";

  // 4. Confirm and update location
  while (true) {
    std::string confirm = promptForString("Confirm relocation? (Y/N):");
    if (stringCompare(confirm, "Y")) {
      if (client.updateAnimalLocation(selected->getName(), destination)) {
        selected->setInServiceContinent(destination);
        std::cout << selected->getName() << " successfully relocated to " << destination << ".\n";
      } else {
        std::cout << "Failed to persist location update to database.\n";
      }
      break;
    } else if (stringCompare(confirm, "N")) {
      std::cout << "Relocation aborted.\n";
      break;
    }
  }
}

// Builds the bi-directional weighted continent transit network
[[nodiscard]] auto generateContinentGraph() -> graph::ContinentGraph {
  graph::ContinentGraph graph(RescueUtils::kValidContinents.size());

  // Edge weights based on $0.18/mile between geographical center points
  graph.addEdge(0, 1, 1200); // Africa <-> Antarctica
  graph.addEdge(0, 2, 930);  // Africa <-> Asia
  graph.addEdge(0, 3, 1400); // Africa <-> Australia
  graph.addEdge(0, 4, 650);  // Africa <-> Europe

  graph.addEdge(1, 3, 800);  // Antarctica <-> Australia
  graph.addEdge(1, 6, 900);  // Antarctica <-> South America

  graph.addEdge(2, 3, 1000); // Asia <-> Australia
  graph.addEdge(2, 4, 500);  // Asia <-> Europe
  graph.addEdge(2, 5, 1100); // Asia <-> North America

  graph.addEdge(3, 5, 1600); // Australia <-> North America
  graph.addEdge(3, 6, 1700); // Australia <-> South America

  graph.addEdge(4, 5, 840);  // Europe <-> North America

  graph.addEdge(5, 6, 930);  // North America <-> South America

  return graph;
}

int main() {
  // Read database credentials from environment variables (or fall back to defaults)
  const std::string supabaseUrl = getEnvOrDefault("SUPABASE_URL", kDefaultUrl);
  const std::string supabaseAnonKey = getEnvOrDefault("SUPABASE_ANON_KEY", kDefaultKey);

  auth::SupabaseClient client(supabaseUrl, supabaseAnonKey);
  if (!handleUserLogin(client)) {
    std::cout << "Exiting application.\n";
    return 0;
  }

  // Load existing animals for the authenticated account
  std::vector<std::unique_ptr<RescueAnimal>> rescueAnimalServiceList = client.fetchAnimals();
  std::cout << "Loaded " << rescueAnimalServiceList.size() << " records for this session.\n";

  const graph::ContinentGraph continentGraph = generateContinentGraph();
  std::cout << "Welcome to Grazioso Salvare.\n";

  std::string userInput;
  while (true) {
    displayMenu();
    std::getline(std::cin, userInput);

    if (userInput.empty()) {
      continue;
    }

    if (stringCompare(userInput, "q")) {
      break;
    }

    try {
      std::size_t idx = 0;
      const int selection = std::stoi(userInput, &idx);

      if (idx == userInput.length() && selection >= 1 && selection <= 7) {
        switch (selection) {
          case 1: intakeNewAnimal(rescueAnimalServiceList, client); break;
          case 2: reserveAnimal(rescueAnimalServiceList, client); break;
          case 3: moveAnimal(rescueAnimalServiceList, continentGraph, client); break;
          case 4: printAnimals("dog", rescueAnimalServiceList); break;
          case 5: printAnimals("monkey", rescueAnimalServiceList); break;
          case 6: printAnimals("available", rescueAnimalServiceList); break;
          case 7: printAnimals("all", rescueAnimalServiceList); break;
          default: break;
        }
      } else {
        std::cout << "Please enter a valid numeric choice from 1 to 7.\n";
      }
    } catch (const std::exception &) {
      std::cout << "Invalid input. Please enter a choice from 1-7 or 'q' to quit.\n";
    }
  }

  std::cout << "\nThank you for using Grazioso Salvare Rescue Services.\n";
  return 0;
}