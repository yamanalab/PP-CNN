#pragma once

#include <string>
#include <vector>
#include "seal/seal.h"
#include "utils/globals.hpp"

using seal::Ciphertext;
using seal::Plaintext;
using std::string;
using std::vector;

class Layer {
public:
  Layer(const string& name, const ELayerClass& layer_class);
  virtual ~Layer();

  const string& name() const { return name_; };
  const ELayerClass& layer_class() const { return layer_class_; };

  virtual void printInfo() const = 0;
  virtual void forward(Ciphertext3D& input) const;
  virtual void forward(vector<Ciphertext>& input) const;

private:
  string name_;
  ELayerClass layer_class_;
};
