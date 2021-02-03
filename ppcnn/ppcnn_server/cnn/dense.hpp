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

#include "layer.hpp"

using std::size_t;

const string DENSE_CLASS_NAME = "Dense";

class Dense : public Layer
{
public:
    Dense(const string& name, const size_t& in_units, const size_t& out_units,
          const string& activation, const Plaintext2D& plain_weights,
          const vector<Plaintext>& plain_biases, OptOption& option);
    ~Dense();

    void printInfo() const override;
    void forward(vector<Ciphertext>& input) const;

private:
    size_t in_units_;
    size_t out_units_;
    string activation_;
    Plaintext2D plain_weights_;
    vector<Plaintext> plain_biases_;

    OptOption& option_;
};
