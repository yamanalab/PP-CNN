#include "conv2d_fused_bn.hpp"

using std::cout;
using std::endl;

Conv2DFusedBN::Conv2DFusedBN(const string& name,
                             const size_t& in_height, const size_t& in_width, const size_t& in_channels,
                             const size_t& filter_size, const size_t& filter_height, const size_t& filter_width,
                             const size_t& stride_height, const size_t& stride_width,
                             const string& padding, const string& activation,
                             const Plaintext4D& plain_filters, const vector<Plaintext>& plain_biases)
    : Conv2D(name,
             in_height, in_width, in_channels,
             filter_size, filter_height, filter_width,
             stride_height, stride_width,
             padding, activation,
             plain_filters, plain_biases) {}
Conv2DFusedBN::~Conv2DFusedBN() {}

void Conv2DFusedBN::printInfo() const {
  cout << CONV2D_FUSED_BN_CLASS_NAME << ": " << name() << endl;
}
