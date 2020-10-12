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

const string BATCH_NORMALIZATION_CLASS_NAME = "BatchNormalization";

constexpr float BN_EPSILON = 0.001;

class BatchNormalization : public Layer
{
public:
    BatchNormalization(const string& name,
                       const vector<Plaintext>& plain_weights,
                       const vector<Plaintext>& plain_biases,
                       OptOption& option);
    ~BatchNormalization();

    void printInfo() const override;
    void forward(Ciphertext3D& input) const;
    void forward(vector<Ciphertext>& input) const;

private:
    vector<Plaintext> plain_weights_;
    vector<Plaintext> plain_biases_;

    OptOption& option_;
};
