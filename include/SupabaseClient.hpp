// SupabaseClient.hpp
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "RescueAnimal.hpp"

namespace auth {

// Holds authentication tokens and user identification details
struct UserSession {
  std::string accessToken;
  std::string refreshToken;
  std::string userId;
  std::string email;
};

// Manages remote REST calls to Supabase for authentication and animal record persistence
class SupabaseClient {
private:
  std::string projectUrl_;
  std::string anonKey_;
  std::optional<UserSession> currentSession_;

public:
  SupabaseClient(std::string projectUrl, std::string anonKey);

  [[nodiscard]] bool signIn(const std::string &email, const std::string &password);
  [[nodiscard]] bool signUp(const std::string &email, const std::string &password);
  void signOut() noexcept;

  [[nodiscard]] bool isAuthenticated() const noexcept;
  [[nodiscard]] const std::optional<UserSession> &getSession() const noexcept;

  // Fetches animal records created by the authenticated user
  [[nodiscard]] std::vector<std::unique_ptr<RescueAnimal>> fetchAnimals() const;

  // Persists a newly added animal record to the database
  [[nodiscard]] bool saveAnimal(const RescueAnimal &animal) const;

  // Updates reservation flag in the database for a named animal
  [[nodiscard]] bool updateAnimalReservation(const std::string &animalName, bool reserved) const;

  // Updates current service continent in the database for a named animal
  [[nodiscard]] bool updateAnimalLocation(const std::string &animalName,
                                          const std::optional<std::string> &continent) const;
};

} // namespace auth