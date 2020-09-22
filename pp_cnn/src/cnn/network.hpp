#pragma once

#include <memory>
#include <stdexcept>
#include "layer.hpp"

using std::runtime_error;
using std::shared_ptr;

class Network {
public:
  Network();
  ~Network();

  int getLayerSize() const { return layers_.size(); }
  const vector<shared_ptr<Layer>> getLayers() const { return layers_; }
  void addLayer(Layer* layer) { layers_.push_back(shared_ptr<Layer>(layer)); }
  void printStructure() const noexcept;
  vector<Ciphertext> predict(Ciphertext3D& input_3d) const noexcept(false);

private:
  vector<shared_ptr<Layer>> layers_;
};

class InvalidDowncastException : public runtime_error {
public:
  InvalidDowncastException(const string& message) : runtime_error(message) {}
};
