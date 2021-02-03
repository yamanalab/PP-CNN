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

#include <seal/seal.h>

#include <ppcnn_share/cnn_utils/opt_option.hpp>
#include <string>
#include <vector>

using seal::Ciphertext;
using seal::Plaintext;
using std::string;
using std::vector;

class Layer
{
public:
    Layer(const string& name, const ELayerClass& layer_class);
    virtual ~Layer();

    const string& name() const
    {
        return name_;
    };
    const ELayerClass& layer_class() const
    {
        return layer_class_;
    };

    virtual void printInfo() const = 0;
    virtual void forward(Ciphertext3D& input) const;
    virtual void forward(vector<Ciphertext>& input) const;

private:
    string name_;
    ELayerClass layer_class_;
};
