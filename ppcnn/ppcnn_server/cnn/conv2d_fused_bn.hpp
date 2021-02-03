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

#pragma once

#include "conv2d.hpp"

using std::size_t;

const string CONV2D_FUSED_BN_CLASS_NAME = "Conv2DFusedBN";

class Conv2DFusedBN : public Conv2D
{
public:
    Conv2DFusedBN(const string& name, const size_t& in_height,
                  const size_t& in_width, const size_t& in_channels,
                  const size_t& filter_size, const size_t& filter_height,
                  const size_t& filter_width, const size_t& stride_height,
                  const size_t& stride_width, const string& padding,
                  const string& activation, const Plaintext4D& plain_filters,
                  const vector<Plaintext>& plain_biases, OptOption& option);
    ~Conv2DFusedBN();

    void printInfo() const override;

private:
};
