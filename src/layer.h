#pragma once

#include "libstatsig_ffi.h"
#include "user.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <unordered_map>

namespace statsig_cpp_core {

using json = nlohmann::json;

class Layer {
public:
  std::string rule_id;
  std::string id_type;
  std::unordered_map<std::string, json> value;
  EvaluationDetails details;
  Layer() = default;
  Layer(uint64_t statsig_ref, const std::string json_str);
  ~Layer() = default;

  void
  statsig_manually_log_layer_parameter_exposure(const User &user,
                                                const std::string &param_name);

  template <typename T> T get(const std::string &key, T fallback) {
    auto it = value.find(key);
    if (it == value.end())
      return fallback;

    try {
      T result = it->second.get<T>();
      logParamExposure(key.c_str());
      return result;
    } catch (...) {
      return fallback;
    }
  }

private:
  uint64_t statsig_ref_;
  std::string json_str_; // store a copy, safer than reference

  void logParamExposure(const char *param_name) {
    statsig_log_layer_param_exposure(statsig_ref_, json_str_.c_str(),
                                     param_name);
  }
};

// from_json function to deserialize JSON into Layer
inline void from_json(const json &j, Layer &l) {
  j.at("rule_id").get_to(l.rule_id);
  j.at("id_type").get_to(l.id_type);
  j.at("__value").get_to(l.value);
  j.at("details").get_to(l.details);
}

inline Layer::Layer(uint64_t statsig_ref, const std::string json_str)
    : statsig_ref_(statsig_ref), json_str_(json_str) {
  json j = json::parse(json_str_);
  from_json(j, *this); // populate fields safely
}

} // namespace statsig_cpp_core
