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

#include "conv2d_fused_bn.hpp"

using std::cout;
using std::endl;

Conv2DFusedBN::Conv2DFusedBN(
  const string& name, const size_t& in_height, const size_t& in_width,
  const size_t& in_channels, const size_t& filter_size,
  const size_t& filter_height, const size_t& filter_width,
  const size_t& stride_height, const size_t& stride_width,
  const string& padding, const string& activation,
  const Plaintext4D& plain_filters, const vector<Plaintext>& plain_biases,
  OptOption& option)
  : Conv2D(name, in_height, in_width, in_channels, filter_size, filter_height,
           filter_width, stride_height, stride_width, padding, activation,
           plain_filters, plain_biases, option)
{
}
Conv2DFusedBN::~Conv2DFusedBN()
{
}

void Conv2DFusedBN::printInfo() const
{
    cout << CONV2D_FUSED_BN_CLASS_NAME << ": " << name() << endl;
}
