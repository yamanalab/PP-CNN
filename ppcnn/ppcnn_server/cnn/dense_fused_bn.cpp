/*
 * Copyright 2020 Yamana Laboratory, Waseda University
 * Supported by JST CREST Grant Number JPMJCR1503, Japan.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE‐2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dense_fused_bn.hpp"

using std::cout;
using std::endl;

DenseFusedBN::DenseFusedBN(const string& name, const size_t& in_units,
                           const size_t& out_units, const string& activation,
                           const Plaintext2D& plain_weights,
                           const vector<Plaintext>& plain_biases,
                           OptOption& option)
  : Dense(name, in_units, out_units, activation, plain_weights, plain_biases,
          option)
{
}
DenseFusedBN::~DenseFusedBN()
{
}

void DenseFusedBN::printInfo() const
{
    cout << DENSE_FUSED_BN_CLASS_NAME << ": " << name() << endl;
}
