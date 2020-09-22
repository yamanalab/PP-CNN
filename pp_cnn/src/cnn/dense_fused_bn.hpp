#pragma once

#include "dense.hpp"

using std::size_t;

const string DENSE_FUSED_BN_CLASS_NAME = "DenseFusedBN";

class DenseFusedBN : public Dense {
public:
  DenseFusedBN(const string& name,
               const size_t& in_units, const size_t& out_units,
               const string& activation,
               const Plaintext2D& plain_weights, const vector<Plaintext>& plain_biases);
  ~DenseFusedBN();

  void printInfo() const override;

private:
};
