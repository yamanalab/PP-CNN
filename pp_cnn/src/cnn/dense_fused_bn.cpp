#include "dense_fused_bn.hpp"

using std::cout;
using std::endl;

DenseFusedBN::DenseFusedBN(const string& name,
                           const size_t& in_units, const size_t& out_units,
                           const string& activation,
                           const Plaintext2D& plain_weights, const vector<Plaintext>& plain_biases)
    : Dense(name,
            in_units, out_units,
            activation,
            plain_weights, plain_biases) {}
DenseFusedBN::~DenseFusedBN() {}

void DenseFusedBN::printInfo() const {
  cout << DENSE_FUSED_BN_CLASS_NAME << ": " << name() << endl;
}
