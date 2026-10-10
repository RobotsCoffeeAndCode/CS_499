#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "RescueAnimal.hpp"

namespace auth {

struct UserSession {
  std::string accessToken;
  std::string refreshToken;
  std::string userId;
  std::string email;
};

class SupabaseClient {

private:
  std::string projectUrl_;
  std::string anonKey_;
  std::optional<UserSession> currentSession_;

public:
  SupabaseClient(std::string projectUrl, std::string anonKey);

  [[nodiscard]] bool signIn(const std::string &email,
                            const std::string &password);
  [[nodiscard]] bool signUp(const std::string &email,
                            const std::string &password);
  void signOut() noexcept;

  [[nodiscard]] bool isAuthenticated() const noexcept;
  [[nodiscard]] const std::optional<UserSession> &getSession() const noexcept;

  [[nodiscard]] std::vector<std::unique_ptr<RescueAnimal>> fetchAnimals() const;
  [[nodiscard]] bool saveAnimal(const RescueAnimal &animal) const;

};

} // namespace auth