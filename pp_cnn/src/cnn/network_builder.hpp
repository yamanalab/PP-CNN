#pragma once

#include "network.hpp"

using std::string;

class NetworkBuilder final {
public:
  static void setup(const string& model_structure_path, const string& model_weights_path) {
    model_structure_path_ = model_structure_path;
    model_weights_path_   = model_weights_path;
  }
  static Network buildNetwork() noexcept(false);

private:
  inline static string model_structure_path_;
  inline static string model_weights_path_;
  NetworkBuilder();
  ~NetworkBuilder();
};
