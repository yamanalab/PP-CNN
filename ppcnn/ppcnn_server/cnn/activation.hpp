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

#include <functional>

#include "layer.hpp"

using std::function;

const string ACTIVATION_CLASS_NAME = "Activation";
const string SQUARE_NAME = "square";
const string SWISH_RG4_DEG4_NAME = "swish_rg4_deg4";
const string SWISH_RG6_DEG4_NAME = "swish_rg6_deg4";

class Activation : Layer
{
public:
    Activation(const string& name, const string& activation, OptOption& option);
    ~Activation();

    void printInfo() const override;
    void forward(Ciphertext3D& input) const;
    void forward(vector<Ciphertext>& input) const;

private:
    string activation_;
    vector<Plaintext> plain_poly_coeffs_;
    Ciphertext activate(Ciphertext& x) const;
    Ciphertext square(Ciphertext& x) const;
    Ciphertext swishDeg4(Ciphertext& x) const;
    Ciphertext swishDeg4Opt(Ciphertext& x) const;

    OptOption& option_;
};
