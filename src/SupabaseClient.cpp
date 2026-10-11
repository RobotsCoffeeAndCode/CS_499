// SupabaseClient.cpp
#include "SupabaseClient.hpp"

#include <chrono>
#include <cpr/cpr.h>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

#include "Dog.hpp"
#include "Monkey.hpp"
#include "RescueUtils.hpp"

namespace auth {

using json = nlohmann::json;

// Helper to format chrono dates into database standard "YYYY-MM-DD"
static std::string formatDate(std::chrono::year_month_day ymd) {
  if (!ymd.ok()) {
    return "1950-01-01"; // Fallback sentinel date for uninitialized dates
  }
  std::ostringstream oss;
  oss << static_cast<int>(ymd.year()) << '-'
      << std::setfill('0') << std::setw(2) << static_cast<unsigned>(ymd.month()) << '-'
      << std::setfill('0') << std::setw(2) << static_cast<unsigned>(ymd.day());
  return oss.str();
}

SupabaseClient::SupabaseClient(std::string projectUrl, std::string anonKey)
    : projectUrl_(std::move(projectUrl)), anonKey_(std::move(anonKey)) {}

// Authenticates using email/password via Supabase Auth REST endpoint
bool SupabaseClient::signIn(const std::string &email, const std::string &password) {
  const std::string endpoint = projectUrl_ + "/auth/v1/token?grant_type=password";
  const json payload = {{"email", email}, {"password", password}};

  const auto response = cpr::Post(
      cpr::Url{endpoint},
      cpr::Header{{"apikey", anonKey_},
                  {"Authorization", "Bearer " + anonKey_},
                  {"Content-Type", "application/json"}},
      cpr::Body{payload.dump()});

  if (response.status_code != 200) {
    std::cerr << "Sign-in error (" << response.status_code << "): " << response.text << '\n';
    return false;
  }

  try {
    const auto body = json::parse(response.text);
    currentSession_ = UserSession{
        .accessToken = body.at("access_token").get<std::string>(),
        .refreshToken = body.at("refresh_token").get<std::string>(),
        .userId = body.at("user").at("id").get<std::string>(),
        .email = body.at("user").at("email").get<std::string>()};
    return true;
  } catch (const std::exception &e) {
    std::cerr << "Failed to parse sign-in session response: " << e.what() << '\n';
    return false;
  }
}

// Registers a new user account via Supabase Auth
bool SupabaseClient::signUp(const std::string &email, const std::string &password) {
  const std::string endpoint = projectUrl_ + "/auth/v1/signup";
  const json payload = {{"email", email}, {"password", password}};

  const auto response = cpr::Post(
      cpr::Url{endpoint},
      cpr::Header{{"apikey", anonKey_},
                  {"Authorization", "Bearer " + anonKey_},
                  {"Content-Type", "application/json"}},
      cpr::Body{payload.dump()});

  if (response.status_code != 200 && response.status_code != 201) {
    std::cerr << "Sign-up error (" << response.status_code << "): " << response.text << '\n';
    return false;
  }
  return true;
}

void SupabaseClient::signOut() noexcept {
  currentSession_ = std::nullopt;
}

bool SupabaseClient::isAuthenticated() const noexcept {
  return currentSession_.has_value();
}

const std::optional<UserSession> &SupabaseClient::getSession() const noexcept {
  return currentSession_;
}

// Queries all animal records from the PostgREST endpoint
std::vector<std::unique_ptr<RescueAnimal>> SupabaseClient::fetchAnimals() const {
  std::vector<std::unique_ptr<RescueAnimal>> animals;

  if (!isAuthenticated()) {
    std::cerr << "Error: Fetch requested without an authenticated session.\n";
    return animals;
  }

  const std::string endpoint = projectUrl_ + "/rest/v1/rescue_animals?select=*";
  const auto response = cpr::Get(
      cpr::Url{endpoint},
      cpr::Header{{"apikey", anonKey_},
                  {"Authorization", "Bearer " + currentSession_->accessToken}});

  if (response.status_code != 200) {
    std::cerr << "Fetch error (" << response.status_code << "): " << response.text << '\n';
    return animals;
  }

  try {
    const auto data = json::parse(response.text);

    for (const auto &item : data) {
      const std::string type = item.value("animal_type", "");
      const std::string name = item.value("name", "");
      const Gender gender = RescueUtils::parseGender(item.value("gender", ""));
      const auto birth = RescueUtils::parseDateString(item.value("birth_date", "2000-01-01"));
      const auto acq = RescueUtils::parseDateString(item.value("acquisition_date", "2000-01-01"));
      const double weight = item.value("weight_kg", 0.0);
      const std::string origin = item.value("acquisition_continent", "");
      const TrainingStatus status = RescueUtils::parseTrainingStatus(item.value("training_status", ""));
      const bool reserved = item.value("reserved", false);

      std::optional<std::string> serviceContinent;
      if (!item["in_service_continent"].is_null()) {
        serviceContinent = item["in_service_continent"].get<std::string>();
      }

      if (type == "Dog") {
        std::string breed = item.value("breed", "");
        animals.push_back(std::make_unique<Dog>(
            name, std::move(breed), gender, birth.value_or(std::chrono::year_month_day{}),
            weight, acq.value_or(std::chrono::year_month_day{}), origin, status,
            reserved, std::move(serviceContinent)));
      } else if (type == "Monkey") {
        std::string species = item.value("species", "");
        double tail = item.value("tail_length", 0.0);
        double height = item.value("height", 0.0);
        double body = item.value("body_length", 0.0);

        animals.push_back(std::make_unique<Monkey>(
            name, gender, birth.value_or(std::chrono::year_month_day{}), weight,
            acq.value_or(std::chrono::year_month_day{}), origin, status, reserved,
            std::move(serviceContinent), tail, height, body, std::move(species)));
      }
    }
  } catch (const std::exception &e) {
    std::cerr << "Error parsing animal payload: " << e.what() << '\n';
  }

  return animals;
}

// Converts the animal to a JSON object and inserts it into the database
bool SupabaseClient::saveAnimal(const RescueAnimal &animal) const {
  if (!isAuthenticated()) {
    return false;
  }

  const std::string endpoint = projectUrl_ + "/rest/v1/rescue_animals";

  // Build the common payload fields
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
           ? json(*animal.getInServiceContinent())
           : json(nullptr)}};

  // Polymorphic delegation: animal appends its own subclass-specific attributes
  animal.serializeSubclassFields(payload);

  const auto response = cpr::Post(
      cpr::Url{endpoint},
      cpr::Header{{"apikey", anonKey_},
                  {"Authorization", "Bearer " + currentSession_->accessToken},
                  {"Content-Type", "application/json"},
                  {"Prefer", "return=minimal"}},
      cpr::Body{payload.dump()});

  if (response.status_code != 201) {
    std::cerr << "Failed to persist animal (" << response.status_code << "): " << response.text << '\n';
    return false;
  }
  return true;
}

// Sends a PATCH request to update the animal's reservation status
bool SupabaseClient::updateAnimalReservation(const std::string &animalName, bool reserved) const {
  if (!isAuthenticated()) {
    return false;
  }

  const std::string endpoint =
      projectUrl_ + "/rest/v1/rescue_animals?name=eq." +
      cpr::util::urlEncode(animalName) +
      "&user_id=eq." + currentSession_->userId;

  const json payload = {{"reserved", reserved}};

  const auto response = cpr::Patch(
      cpr::Url{endpoint},
      cpr::Header{{"apikey", anonKey_},
                  {"Authorization", "Bearer " + currentSession_->accessToken},
                  {"Content-Type", "application/json"},
                  {"Prefer", "return=minimal"}},
      cpr::Body{payload.dump()});

  return (response.status_code == 200 || response.status_code == 204);
}

// Sends a PATCH request to update an animal's location
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

  const json payload = {
      {"in_service_continent",
       continent.has_value() ? json(*continent) : json(nullptr)}};

  const auto response = cpr::Patch(
      cpr::Url{endpoint},
      cpr::Header{{"apikey", anonKey_},
                  {"Authorization", "Bearer " + currentSession_->accessToken},
                  {"Content-Type", "application/json"},
                  {"Prefer", "return=minimal"}},
      cpr::Body{payload.dump()});

  return (response.status_code == 200 || response.status_code == 204);
}

} // namespace auth