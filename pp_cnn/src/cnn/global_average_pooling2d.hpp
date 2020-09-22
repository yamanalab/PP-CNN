#pragma once

#include "layer.hpp"

using std::size_t;

const string GLOBAL_AVERAGE_POOLING2D_CLASS_NAME = "GlobalAveragePooling2D";

class GlobalAveragePooling2D : public Layer {
public:
  GlobalAveragePooling2D(const string& name,
                         const size_t& in_height, const size_t& in_width, const size_t& in_channels,
                         const size_t& out_units,
                         const Plaintext& plain_mul_factor);
  ~GlobalAveragePooling2D();

  void printInfo() const override;
  vector<Ciphertext> flatten(Ciphertext3D& input) const;

private:
  size_t in_height_;
  size_t in_width_;
  size_t in_channels_;
  size_t out_units_;
  Plaintext plain_mul_factor_;
};
