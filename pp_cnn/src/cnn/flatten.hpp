#pragma once

#include "layer.hpp"

using std::size_t;

const string FLATTEN_CLASS_NAME = "Flatten";

class Flatten : public Layer {
public:
  Flatten(const string& name,
          const size_t& in_height, const size_t& in_width, const size_t& in_channels,
          const size_t& out_units);
  ~Flatten();

  void printInfo() const override;
  vector<Ciphertext> flatten(Ciphertext3D& input) const;

private:
  size_t in_height_;
  size_t in_width_;
  size_t in_channels_;
  size_t out_units_;
};
