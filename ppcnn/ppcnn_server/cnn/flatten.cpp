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

#include "flatten.hpp"

using std::cout;
using std::endl;
using std::move;

Flatten::Flatten(const string& name, const size_t& in_height,
                 const size_t& in_width, const size_t& in_channels,
                 const size_t& out_units)
  : Layer(name, FLATTEN),
    in_height_(in_height),
    in_width_(in_width),
    in_channels_(in_channels),
    out_units_(out_units)
{
}
Flatten::~Flatten()
{
}

void Flatten::printInfo() const
{
    cout << FLATTEN_CLASS_NAME << ": " << name() << endl;
}

vector<Ciphertext> Flatten::flatten(Ciphertext3D& input) const
{
    cout << "\tForwarding " << name() << "..." << endl;
    cout << "\t  input shape: " << input.shape()[0] << "x" << input.shape()[1]
         << "x" << input.shape()[2] << endl;
    vector<Ciphertext> flattened_input(out_units_);
    size_t pos;

#ifdef _OPENMP
#pragma omp parallel for collapse(3) private(pos)
#endif
    for (size_t ih = 0; ih < in_height_; ++ih)
    {
        for (size_t iw = 0; iw < in_width_; ++iw)
        {
            for (size_t ic = 0; ic < in_channels_; ++ic)
            {
                pos = ih * in_width_ * in_channels_ + iw * in_channels_ + ic;

                flattened_input[pos] = move(input[ih][iw][ic]);
            }
        }
    }

    return flattened_input;
}
