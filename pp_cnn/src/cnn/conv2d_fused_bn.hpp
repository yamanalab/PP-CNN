#pragma once

#include "conv2d.hpp"

using std::size_t;

const string CONV2D_FUSED_BN_CLASS_NAME = "Conv2DFusedBN";

class Conv2DFusedBN : public Conv2D {
public:
  Conv2DFusedBN(const string& name,
                const size_t& in_height, const size_t& in_width, const size_t& in_channels,
                const size_t& filter_size, const size_t& filter_height, const size_t& filter_width,
                const size_t& stride_height, const size_t& stride_width,
                const string& padding, const string& activation,
                const Plaintext4D& plain_filters, const vector<Plaintext>& plain_biases);
  ~Conv2DFusedBN();

  void printInfo() const override;

private:
};
