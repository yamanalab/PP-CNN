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

const string GLOBAL_AVERAGE_POOLING2D_CLASS_NAME = "GlobalAveragePooling2D";

class GlobalAveragePooling2D : public Layer
{
public:
    GlobalAveragePooling2D(const string& name, const size_t& in_height,
                           const size_t& in_width, const size_t& in_channels,
                           const size_t& out_units,
                           const Plaintext& plain_mul_factor,
                           OptOption& option);
    ~GlobalAveragePooling2D();

    void printInfo() const override;
    vector<Ciphertext> flatten(Ciphertext3D& input) const;

private:
    size_t in_height_;
    size_t in_width_;
    size_t in_channels_;
    size_t out_units_;
    Plaintext plain_mul_factor_;

    OptOption& option_;
};
