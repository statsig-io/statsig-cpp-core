#pragma once 

#ifndef STATSIG_H
#define STATSIG_H

#include "types.h"
#include "layer.h"
#include "user.h"
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
// Forward declarations
namespace statsig_cpp_core {
class Statsig {
public:
  // Constructor and destructor
  Statsig(const std::string &sdk_key);
  ~Statsig();

  // Copy constructor and assignment operator
  Statsig(const Statsig &other);
  Statsig &operator=(const Statsig &other);

  // Move constructor and assignment operator
  Statsig(Statsig &&other) noexcept;
  Statsig &operator=(Statsig &&other) noexcept;

  // Initialization methods
  void initialize(std::function<void()> callback = nullptr);
  void initializeWithDetails(
      std::function<void(const std::string &)> callback = nullptr);
  std::string initializeWithDetailsBlocking();
  void initializeBlocking();

  // Shutdown methods
  void shutdown(std::function<void()> callback = nullptr);
  void shutdownBlocking();

  // Event logging
  void flushEvents(std::function<void()> callback = nullptr);
  void flushEventsBlocking();
  void
  logEvent(const statsig_cpp_core::User &user, const std::string &event_name,
           const std::unordered_map<std::string, std::string> &event_value = {},
           const std::string &metadata = "");

  // Feature Gates
  bool checkGate(const statsig_cpp_core::User &user,
                 const std::string &gate_name,
                 const std::string &options_json = "{}");
  FeatureGate getFeatureGate(const statsig_cpp_core::User &user,
                             const std::string &gate_name,
                             const std::string &options_json = "{}");

  Experiment getExperiment(const User &user, const std::string &experiment_name,
                           const std::string &options_json = "{}");
  DynamicConfig getConfig(const User &user, const std::string &config_name,
                          const std::string &options_json = "{}");
                        
  Layer getLayer(const User &user, const std::string &layer_name,
                 const std::string &options_json = "{}");

private:
  uint64_t ref_;
  std::string sdk_key_;
};
} // namespace statsig_cpp_core

#endif // STATSIG_H
