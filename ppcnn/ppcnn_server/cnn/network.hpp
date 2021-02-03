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

#include <memory>
#include <stdexcept>

#include "layer.hpp"

using std::runtime_error;
using std::shared_ptr;

class Network
{
public:
    Network();
    ~Network();

    int getLayerSize() const
    {
        return layers_.size();
    }
    const vector<shared_ptr<Layer>> getLayers() const
    {
        return layers_;
    }
    void addLayer(Layer* layer)
    {
        layers_.push_back(shared_ptr<Layer>(layer));
    }
    void printStructure() const noexcept;
    vector<Ciphertext> predict(Ciphertext3D& input_3d) const noexcept(false);

private:
    vector<shared_ptr<Layer>> layers_;
};

class InvalidDowncastException : public runtime_error
{
public:
    InvalidDowncastException(const string& message) : runtime_error(message)
    {
    }
};
