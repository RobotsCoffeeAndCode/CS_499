#include "SupabaseClient.hpp"

#include <chrono>
#include <cpr/cpr.h>
#include <iostream>
#include <nlohmann/json.hpp>

#include "Dog.hpp"
#include "Monkey.hpp"
#include "RescueUtils.hpp"

namespace auth {

// Access the nlohmann library using the json type
using json = nlohmann::json;

// Helper function to format chrono::year_month_day into "YYYY-MM-DD" format
std::string formatDate(std::chrono::year_month_day ymd) {

  // If there is an issue with the passed date, return default valid one
  // By making this date unique (no service animal is alive from 1950!)
  // we can use this value for error checking in the db
  if (!ymd.ok()) {
    return "1950-01-01";
  }

  // If there is no issue, build the string representation with stringstream
  std::ostringstream oss;
  oss << static_cast<int>(ymd.year()) << '-' << std::setfill('0')
      << std::setw(2) << static_cast<unsigned>(ymd.month()) << '-'
      << std::setfill('0') << std::setw(2) << static_cast<unsigned>(ymd.day());

  return oss.str();
}

// Constructor takes base URL to make API requests to and public API key
SupabaseClient::SupabaseClient(std::string projectUrl, std::string anonKey)
    : projectUrl_(std::move(projectUrl)), anonKey_(std::move(anonKey)) {}

// Takes the given email and pass from the user and creates a supabase client
// Returns true upon successful creation, false with error info otherwise
bool SupabaseClient::signIn(const std::string &email,
                            const std::string &password) {
  const std::string endpoint =
      projectUrl_ + "/auth/v1/token?grant_type=password";
  const json payload = {{"email", email}, {"password", password}};

  const auto response =
      cpr::Post(cpr::Url{endpoint},
                cpr::Header{{"apikey", anonKey_},
                            {"Authorization", "Bearer " + anonKey_},
                            {"Content-Type", "application/json"}},
                cpr::Body{payload.dump()});

  if (response.status_code != 200) {
    std::cerr << "Sign-in error (" << response.status_code
              << "): " << response.text << '\n';
    return false;
  }

  const auto body = json::parse(response.text);
  currentSession_ =
      UserSession{.accessToken = body.at("access_token").get<std::string>(),
                  .refreshToken = body.at("refresh_token").get<std::string>(),
                  .userId = body.at("user").at("id").get<std::string>(),
                  .email = body.at("user").at("email").get<std::string>()};

  return true;
}

// Adds a new user to the db users with the given email and password
bool SupabaseClient::signUp(const std::string &email,
                            const std::string &password) {
  const std::string endpoint = projectUrl_ + "/auth/v1/signup";
  const json payload = {{"email", email}, {"password", password}};

  const auto response =
      cpr::Post(cpr::Url{endpoint},
                cpr::Header{{"apikey", anonKey_},
                            {"Authorization", "Bearer " + anonKey_},
                            {"Content-Type", "application/json"}},
                cpr::Body{payload.dump()});

  if (response.status_code != 200 && response.status_code != 201) {
    std::cerr << "Sign-up error (" << response.status_code
              << "): " << response.text << '\n';
    return false;
  }
  return true;
}

// Signs out a user (called just before exiting application)
void SupabaseClient::signOut() noexcept { currentSession_ = std::nullopt; }

// Simply checks if a user is authenticated currently
// returns true to indicate that they are, false otherwise
bool SupabaseClient::isAuthenticated() const noexcept {
  return currentSession_.has_value();
}

// Get current session (returns an optional holding the user session value)
const std::optional<UserSession> &SupabaseClient::getSession() const noexcept {
  return currentSession_;
}

// This holds the animal we are indexing over in the db in fetchAnimals()
std::vector<std::unique_ptr<RescueAnimal>>

// This retrieves all the animals that a user has saved with their current
// account in the db, this is called upon sign in to populate the 
// rescueAnimalServiceList in main
SupabaseClient::fetchAnimals() const {

  // Mimic the structure of rescueAnimalServiceList to return
  std::vector<std::unique_ptr<RescueAnimal>> animals;

  // Guard against unauthenticated users
  if (!isAuthenticated()) {
    std::cerr << "Error: Attempted fetch without authentication.\n";
    return animals;
  }

  // Endpoint
  const std::string endpoint = projectUrl_ + "/rest/v1/rescue_animals?select=*";

  // Build and send GET request
  const auto response = cpr::Get(
      cpr::Url{endpoint},
      cpr::Header{{"apikey", anonKey_},
                  {"Authorization", "Bearer " + currentSession_->accessToken}});

  // Upon error, return error info
  if (response.status_code != 200) {
    std::cerr << "Fetch error (" << response.status_code
              << "): " << response.text << '\n';
    return animals;
  }

  // Parse the json response and store it
  const auto data = json::parse(response.text);

  // For each animal (item here), extract all values with default fallbacks
  // collect unique values based on the animaltype value
  // call make_unique<T> with the specific animal type and push onto the
  // service list
  for (const auto &item : data) {

    // Extract non-unique values
    const std::string type = item.value("animal_type", "");
    const std::string name = item.value("name", "");
    const Gender gender = RescueUtils::parseGender(item.value("gender", ""));
    const auto birth =
        RescueUtils::parseDateString(item.value("birth_date", "2000-01-01"));
    const auto acq = RescueUtils::parseDateString(
        item.value("acquisition_date", "2000-01-01"));
    const double weight = item.value("weight_kg", 0.0);
    const std::string origin = item.value("acquisition_continent", "");
    const TrainingStatus status =
        RescueUtils::parseTrainingStatus(item.value("training_status", ""));
    const bool reserved = item.value("reserved", false);

    // Ensure to check optional before retrieving
    std::optional<std::string> serviceContinent;
    if (!item["in_service_continent"].is_null()) {
      serviceContinent = item["in_service_continent"].get<std::string>();
    }

    // Check for animal type, and extract unique values based on it
    if (type == "Dog") {
      const std::string breed = item.value("breed", "");

      // Add the dog to the servicelist
      animals.push_back(std::make_unique<Dog>(
          name, breed, gender, birth.value_or(std::chrono::year_month_day{}),
          weight, acq.value_or(std::chrono::year_month_day{}), origin, status,
          reserved, std::move(serviceContinent)));
    } else if (type == "Monkey") {
      const std::string species = item.value("species", "");
      const double tail = item.value("tail_length", 0.0);
      const double height = item.value("height", 0.0);
      const double body = item.value("body_length", 0.0);

      // Add the monkey to the servicelist
      animals.push_back(std::make_unique<Monkey>(
          name, gender, birth.value_or(std::chrono::year_month_day{}), weight,
          acq.value_or(std::chrono::year_month_day{}), origin, status, reserved,
          std::move(serviceContinent), tail, height, body, species));
    }
  }

  return animals;
}

// Save a specific animal back to the database
// Called upon calling intakeAnimal() in main
bool SupabaseClient::saveAnimal(const RescueAnimal &animal) const {
  if (!isAuthenticated()) {
    return false;
  }

  const std::string endpoint = projectUrl_ + "/rest/v1/rescue_animals";

  // Create the core animal json payload to send to supabase
  json payload = {
      {"user_id", currentSession_->userId},
      {"name", animal.getName()},
      {"animal_type", std::string(animal.getAnimalType())},
      {"gender", std::string(animal.getGender())},
      {"birth_date", formatDate(animal.getBirthDate())},
      {"acquisition_date", formatDate(animal.getAcquisitionDate())},
      {"weight_kg", animal.getWeightKg()},
      {"acquisition_continent", animal.getAcquisitionContinent()},
      {"training_status", std::string(animal.getTrainingStatus())},
      {"reserved", animal.isReserved()},
      {"in_service_continent",
       animal.getInServiceContinent().has_value()
           ? json(animal.getInServiceContinent().value())
           : json(nullptr)}};

  // Unique animal fields, determined by animaltype value
  if (animal.getAnimalType() == "Dog") {
    const auto &dog = dynamic_cast<const Dog &>(animal);
    payload["breed"] = dog.getBreed();
  } else if (animal.getAnimalType() == "Monkey") {
    const auto &monkey = dynamic_cast<const Monkey &>(animal);
    payload["species"] = monkey.getSpecies();
    payload["tail_length"] = monkey.getTailLength();
    payload["height"] = monkey.getHeight();
    payload["body_length"] = monkey.getBodyLength();
  }

  // Send the post request to supabase with json as body
  const auto response = cpr::Post(
      cpr::Url{endpoint},
      cpr::Header{
          {"apikey", anonKey_},
          {"Authorization", "Bearer " + currentSession_->accessToken},
          {"Content-Type", "application/json"},
          {"Prefer", "return=minimal"}},
      cpr::Body{payload.dump()});

  // Catch errors and print info
  if (response.status_code != 201) {
    std::cerr << "Failed to save animal (" << response.status_code
              << "): " << response.text << '\n';
    return false;
  }

  return true;
}

// Changes an animals reservation status in the database based on name
bool SupabaseClient::updateAnimalReservation(const std::string &animalName,
                                            bool reserved) const {
  if (!isAuthenticated()) {
    return false;
  }

  // Filter by name and the current user's ID
  const std::string endpoint =
      projectUrl_ + "/rest/v1/rescue_animals?name=eq." +
      cpr::util::urlEncode(animalName) +
      "&user_id=eq." + currentSession_->userId;

  const json payload = {{"reserved", reserved}};

  // HTTP Patch request send to supabase
  const auto response = cpr::Patch(
      cpr::Url{endpoint},
      cpr::Header{
          {"apikey", anonKey_},
          {"Authorization", "Bearer " + currentSession_->accessToken},
          {"Content-Type", "application/json"},
          {"Prefer", "return=minimal"}},
      cpr::Body{payload.dump()});

  // Error checking
  if (response.status_code != 200 && response.status_code != 204) {
    std::cerr << "Failed to update reservation status (" << response.status_code
              << "): " << response.text << '\n';
    return false;
  }

  return true;
}

// Changes an animals serviceContinent in the database based on name
bool SupabaseClient::updateAnimalLocation(
    const std::string &animalName,
    const std::optional<std::string> &continent) const {
  if (!isAuthenticated()) {
    return false;
  }

  const std::string endpoint =
      projectUrl_ + "/rest/v1/rescue_animals?name=eq." +
      cpr::util::urlEncode(animalName) +
      "&user_id=eq." + currentSession_->userId;

  json payload;
  if (continent.has_value()) {
    payload["in_service_continent"] = *continent;
  } else {
    payload["in_service_continent"] = nullptr;
  }

  const auto response = cpr::Patch(
      cpr::Url{endpoint},
      cpr::Header{
          {"apikey", anonKey_},
          {"Authorization", "Bearer " + currentSession_->accessToken},
          {"Content-Type", "application/json"},
          {"Prefer", "return=minimal"}},
      cpr::Body{payload.dump()});

  if (response.status_code != 200 && response.status_code != 204) {
    std::cerr << "Failed to update animal location (" << response.status_code
              << "): " << response.text << '\n';
    return false;
  }

  return true;
}

} // namespace auth