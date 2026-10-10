#include "SupabaseClient.hpp"

#include <chrono>
#include <cpr/cpr.h>
#include <iostream>
#include <nlohmann/json.hpp>

#include "Dog.hpp"
#include "Monkey.hpp"
#include "RescueUtils.hpp"

namespace auth {

using json = nlohmann::json;

SupabaseClient::SupabaseClient(std::string projectUrl, std::string anonKey)
    : projectUrl_(std::move(projectUrl)), anonKey_(std::move(anonKey)) {}

bool SupabaseClient::signIn(const std::string &email,
                            const std::string &password) {
  const std::string endpoint =
      projectUrl_ + "/auth/v1/token?grant_type=password";
  const json payload = {{"email", email}, {"password", password}};

  const auto response = cpr::Post(
      cpr::Url{endpoint},
      cpr::Header{{"apikey", anonKey_}, {"Authorization", "Bearer " + anonKey_}, {"Content-Type", "application/json"}},
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

bool SupabaseClient::signUp(const std::string &email,
                            const std::string &password) {
  const std::string endpoint = projectUrl_ + "/auth/v1/signup";
  const json payload = {{"email", email}, {"password", password}};

  const auto response = cpr::Post(
      cpr::Url{endpoint},
      cpr::Header{{"apikey", anonKey_}, {"Authorization", "Bearer " + anonKey_}, {"Content-Type", "application/json"}},
      cpr::Body{payload.dump()});

  if (response.status_code != 200 && response.status_code != 201) {
    std::cerr << "Sign-up error (" << response.status_code
              << "): " << response.text << '\n';
    return false;
  }
  return true;
}

void SupabaseClient::signOut() noexcept { currentSession_ = std::nullopt; }

bool SupabaseClient::isAuthenticated() const noexcept {
  return currentSession_.has_value();
}

const std::optional<UserSession> &SupabaseClient::getSession() const noexcept {
  return currentSession_;
}

std::vector<std::unique_ptr<RescueAnimal>>
SupabaseClient::fetchAnimals() const {
  std::vector<std::unique_ptr<RescueAnimal>> animals;
  if (!isAuthenticated()) {
    std::cerr << "Error: Attempted fetch without authentication.\n";
    return animals;
  }

  const std::string endpoint = projectUrl_ + "/rest/v1/rescue_animals?select=*";
  const auto response = cpr::Get(
      cpr::Url{endpoint},
      cpr::Header{{"apikey", anonKey_},
                  {"Authorization", "Bearer " + currentSession_->accessToken}});

  if (response.status_code != 200) {
    std::cerr << "Fetch error (" << response.status_code
              << "): " << response.text << '\n';
    return animals;
  }

  const auto data = json::parse(response.text);
  for (const auto &item : data) {
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

    std::optional<std::string> serviceContinent;
    if (!item["in_service_continent"].is_null()) {
      serviceContinent = item["in_service_continent"].get<std::string>();
    }

    if (type == "Dog") {
      const std::string breed = item.value("breed", "");
      animals.push_back(std::make_unique<Dog>(
          name, breed, gender, birth.value_or(std::chrono::year_month_day{}),
          weight, acq.value_or(std::chrono::year_month_day{}), origin, status,
          reserved, std::move(serviceContinent)));
    } else if (type == "Monkey") {
      const std::string species = item.value("species", "");
      const double tail = item.value("tail_length", 0.0);
      const double height = item.value("height", 0.0);
      const double body = item.value("body_length", 0.0);
      animals.push_back(std::make_unique<Monkey>(
          name, gender, birth.value_or(std::chrono::year_month_day{}), weight,
          acq.value_or(std::chrono::year_month_day{}), origin, status, reserved,
          std::move(serviceContinent), tail, height, body, species));
    }
  }

  return animals;
}

bool SupabaseClient::saveAnimal(const RescueAnimal &animal) const {
  if (!isAuthenticated()) {
    return false;
  }

  const std::string endpoint = projectUrl_ + "/rest/v1/rescue_animals";

  json payload = {{"user_id", currentSession_->userId},
                  {"name", animal.getName()},
                  {"animal_type", std::string(animal.getAnimalType())},
                  {"gender", std::string(animal.getGender())},
                  {"weight_kg", animal.getWeightKg()},
                  {"acquisition_continent", animal.getAcquisitionContinent()},
                  {"training_status", std::string(animal.getTrainingStatus())},
                  {"reserved", animal.isReserved()},
                  {"in_service_continent",
                   animal.getInServiceContinent().has_value()
                       ? json(animal.getInServiceContinent().value())
                       : json(nullptr)}};

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

  const auto response = cpr::Post(
      cpr::Url{endpoint},
      cpr::Header{{"apikey", anonKey_},
                  {"Authorization", "Bearer " + currentSession_->accessToken},
                  {"Content-Type", "application/json"},
                  {"Prefer", "return=minimal"}},
      cpr::Body{payload.dump()});

  return response.status_code == 201;
}

} // namespace auth