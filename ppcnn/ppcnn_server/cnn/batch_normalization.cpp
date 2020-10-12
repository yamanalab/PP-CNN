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

#include <omp.h>
#include <fstream>
#include <iostream>

#include "batch_normalization.hpp"

using std::cout;
using std::endl;
using std::sqrt;

BatchNormalization::BatchNormalization(const string& name,
                                       const vector<Plaintext>& plain_weights,
                                       const vector<Plaintext>& plain_biases,
                                       OptOption& option)
  : Layer(name, BATCH_NORMALIZATION),
    plain_weights_(plain_weights),
    plain_biases_(plain_biases),
    option_(option)
{
    option.consumed_level++;
}
BatchNormalization::~BatchNormalization()
{
}

void BatchNormalization::printInfo() const
{
    cout << BATCH_NORMALIZATION_CLASS_NAME << ": " << name() << endl;
}

void BatchNormalization::forward(Ciphertext3D& input) const
{
    cout << "\tForwarding " << name() << "..." << endl;
    cout << "\t  input shape: " << input.shape()[0] << "x" << input.shape()[1]
         << "x" << input.shape()[2] << endl;
    const size_t height = input.shape()[0];
    const size_t width = input.shape()[1];
    const size_t channels = input.shape()[2];

#ifdef __DEBUG__
    Plaintext plain;
    vector<double> vec_tmp;
    std::ofstream debug_file;
    debug_file.open(DEBUG_FILE_PATH, std::ios::app);
    debug_file << "In " << name() << ":" << endl;
#endif
#ifdef _OPENMP
#pragma omp parallel for collapse(3)
#endif
    for (size_t h = 0; h < height; ++h)
    {
        for (size_t w = 0; w < width; ++w)
        {
            for (size_t c = 0; c < channels; ++c)
            {
                option_.evaluator.multiply_plain_inplace(input[h][w][c],
                                                         plain_weights_[c]);
                option_.evaluator.rescale_to_next_inplace(input[h][w][c]);
                input[h][w][c].scale() = option_.scale_param;
                option_.evaluator.add_plain_inplace(input[h][w][c],
                                                    plain_biases_[c]);
#ifdef __DEBUG__
                // if (omp_get_thread_num() == 10) {
                //   gTool.decryptor()->decrypt(input[h][w][c], plain);
                //   gTool.encoder()->decode(plain, vec_tmp);
                //   debug_file << "\toutput[" << h << "][" << w << "][" << c <<
                //   "]: " << vec_tmp[0] << ", " << vec_tmp[1] << ", " <<
                //   vec_tmp[2] << endl;
                // }
                gTool.decryptor()->decrypt(input[h][w][c], plain);
                gTool.encoder()->decode(plain, vec_tmp);
                debug_file << "\toutput[" << h << "][" << w << "][" << c
                           << "]: " << vec_tmp[0] << ", " << vec_tmp[1] << ", "
                           << vec_tmp[2] << endl;
#endif
            }
        }
    }
}

void BatchNormalization::forward(vector<Ciphertext>& input) const
{
    cout << "\tForwarding " << name() << "..." << endl;
    cout << "\t  input size: " << input.size() << endl;
    const size_t units = input.size();

#ifdef __DEBUG__
    Plaintext plain;
    vector<double> vec_tmp;
    std::ofstream debug_file;
    debug_file.open(DEBUG_FILE_PATH, std::ios::app);
    debug_file << "In " << name() << ":" << endl;
#endif
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (size_t u = 0; u < units; ++u)
    {
        option_.evaluator.multiply_plain_inplace(input[u], plain_weights_[u]);
        option_.evaluator.rescale_to_next_inplace(input[u]);
        input[u].scale() = option_.scale_param;
        option_.evaluator.add_plain_inplace(input[u], plain_biases_[u]);
#ifdef __DEBUG__
        // if (omp_get_thread_num() == 10) {
        //   gTool.decryptor()->decrypt(input[u], plain);
        //   gTool.encoder()->decode(plain, vec_tmp);
        //   debug_file << "\toutput[" << u << "]: " << vec_tmp[0] << ", " <<
        //   vec_tmp[1] << ", " << vec_tmp[2] << endl;
        // }
        gTool.decryptor()->decrypt(input[u], plain);
        gTool.encoder()->decode(plain, vec_tmp);
        debug_file << "\toutput[" << u << "]: " << vec_tmp[0] << ", "
                   << vec_tmp[1] << ", " << vec_tmp[2] << endl;
#endif
    }
}
