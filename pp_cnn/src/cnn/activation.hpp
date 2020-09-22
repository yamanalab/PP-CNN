#pragma once

#include <functional>
#include "layer.hpp"

using std::function;

const string ACTIVATION_CLASS_NAME = "Activation";
const string SQUARE_NAME           = "square";
const string SWISH_RG4_DEG4_NAME   = "swish_rg4_deg4";
const string SWISH_RG6_DEG4_NAME   = "swish_rg6_deg4";

class Activation : Layer {
public:
  Activation(const string& name, const string& activation);
  ~Activation();

  void printInfo() const override;
  void forward(Ciphertext3D& input) const;
  void forward(vector<Ciphertext>& input) const;

private:
  string activation_;
  vector<Plaintext> plain_poly_coeffs_;
  Ciphertext activate(Ciphertext& x) const;
  Ciphertext square(Ciphertext& x) const;
  Ciphertext swishDeg4(Ciphertext& x) const;
  Ciphertext swishDeg4Opt(Ciphertext& x) const;
};
