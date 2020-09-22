#pragma once

#include "layer.hpp"

using std::size_t;

const string DENSE_CLASS_NAME = "Dense";

class Dense : public Layer {
public:
  Dense(const string& name,
        const size_t& in_units, const size_t& out_units,
        const string& activation,
        const Plaintext2D& plain_weights, const vector<Plaintext>& plain_biases);
  ~Dense();

  void printInfo() const override;
  void forward(vector<Ciphertext>& input) const;

private:
  size_t in_units_;
  size_t out_units_;
  string activation_;
  Plaintext2D plain_weights_;
  vector<Plaintext> plain_biases_;
};
