#pragma once

#include "layer.hpp"

using std::size_t;

const string AVERAGE_POOLING2D_CLASS_NAME = "AveragePooling2D";

class AveragePooling2D : public Layer {
public:
  AveragePooling2D(const string& name,
                   const size_t& in_height, const size_t& in_width, const size_t& in_channels,
                   const size_t& pool_height, const size_t& pool_width,
                   const size_t& stride_height, const size_t& stride_width,
                   const string& padding,
                   const Plaintext& plain_mul_factor);
  ~AveragePooling2D();

  const size_t& out_height() const { return out_height_; }
  const size_t& out_width() const { return out_width_; }
  const size_t& out_channels() const { return out_channels_; }

  void printInfo() const override;
  bool isOutOfRangeInput(const int& target_x, const int& target_y) const;
  void forward(Ciphertext3D& input) const;

private:
  size_t in_height_;
  size_t in_width_;
  size_t in_channels_;
  size_t pool_height_;
  size_t pool_width_;
  size_t stride_height_;
  size_t stride_width_;
  string padding_;
  size_t out_height_;
  size_t out_width_;
  size_t out_channels_;
  size_t pad_top_;
  size_t pad_bottom_;
  size_t pad_left_;
  size_t pad_right_;
  Plaintext plain_mul_factor_;
};
