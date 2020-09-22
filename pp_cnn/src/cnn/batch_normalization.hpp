#pragma once

#include "layer.hpp"

using std::size_t;

const string BATCH_NORMALIZATION_CLASS_NAME = "BatchNormalization";

constexpr float BN_EPSILON = 0.001;

class BatchNormalization : public Layer {
public:
  BatchNormalization(const string& name,
                     const vector<Plaintext>& plain_weights, const vector<Plaintext>& plain_biases);
  ~BatchNormalization();

  void printInfo() const override;
  void forward(Ciphertext3D& input) const;
  void forward(vector<Ciphertext>& input) const;

private:
  vector<Plaintext> plain_weights_;
  vector<Plaintext> plain_biases_;
};
