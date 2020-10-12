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
#include <exception>
#include <fstream>
#include <iostream>

#include <ppcnn_share/cnn_utils/define.h>
#include <ppcnn_server/cnn/activation.hpp>

using std::cout;
using std::endl;
using std::move;
using std::runtime_error;

Activation::Activation(const string& name, const string& activation,
                       OptOption& option)
  : Layer(name, ACTIVATION), activation_(activation), option_(option)
{
    if (activation_ == SQUARE_NAME || option.activation == SQUARE)
    {
        option.consumed_level++;
    }
    else if (activation_ == SWISH_RG4_DEG4_NAME ||
             option.activation == SWISH_RG4_DEG4)
    {
        Plaintext plain_coeff;
        if (option_.enable_optimize_activation)
        {
            for (const float& coeff : SWISH_RG4_DEG4_OPT_COEFFS)
            {
                option_.encoder.encode(coeff, option_.scale_param, plain_coeff);
                for (size_t lv = 0; lv < option.consumed_level + 1; ++lv)
                {
                    option_.evaluator.mod_switch_to_next_inplace(plain_coeff);
                }
                plain_poly_coeffs_.push_back(plain_coeff);
            }
            option_.evaluator.mod_switch_to_next_inplace(
              plain_poly_coeffs_.back());
            option.consumed_level += 2;
        }
        else
        {
            for (const float& coeff : SWISH_RG4_DEG4_COEFFS)
            {
                option_.encoder.encode(coeff, option_.scale_param, plain_coeff);
                for (size_t lv = 0; lv < option.consumed_level + 2; ++lv)
                {
                    option_.evaluator.mod_switch_to_next_inplace(plain_coeff);
                }
                plain_poly_coeffs_.push_back(plain_coeff);
            }
            option_.evaluator.mod_switch_to_next_inplace(
              plain_poly_coeffs_.back());
            option.consumed_level += 3;
        }
    }
    else if (activation_ == SWISH_RG6_DEG4_NAME ||
             option.activation == SWISH_RG6_DEG4)
    {
        Plaintext plain_coeff;
        if (option_.enable_optimize_activation)
        {
            for (const float& coeff : SWISH_RG6_DEG4_OPT_COEFFS)
            {
                option_.encoder.encode(coeff, option_.scale_param, plain_coeff);
                for (size_t lv = 0; lv < option.consumed_level + 1; ++lv)
                {
                    option_.evaluator.mod_switch_to_next_inplace(plain_coeff);
                }
                plain_poly_coeffs_.push_back(plain_coeff);
            }
            option_.evaluator.mod_switch_to_next_inplace(
              plain_poly_coeffs_.back());
            option.consumed_level += 2;
        }
        else
        {
            for (const float& coeff : SWISH_RG6_DEG4_COEFFS)
            {
                option_.encoder.encode(coeff, option_.scale_param, plain_coeff);
                for (size_t lv = 0; lv < option.consumed_level + 2; ++lv)
                {
                    option_.evaluator.mod_switch_to_next_inplace(plain_coeff);
                }
                plain_poly_coeffs_.push_back(plain_coeff);
            }
            option_.evaluator.mod_switch_to_next_inplace(
              plain_poly_coeffs_.back());
            option.consumed_level += 3;
        }
    }
    else
    {
        throw runtime_error("\"" + activation_ +
                            "\" is not registered as activation function");
    }
}
Activation::~Activation()
{
}

void Activation::printInfo() const
{
    cout << ACTIVATION_CLASS_NAME << ": " << name() << endl;
}

void Activation::forward(Ciphertext3D& input) const
{
    cout << "\tForwarding " << name() << "..." << endl;
    cout << "\t  input shape: " << input.shape()[0] << "x" << input.shape()[1]
         << "x" << input.shape()[2] << endl;
    const size_t height = input.shape()[0];
    const size_t width = input.shape()[1];
    const size_t channels = input.shape()[2];
    Ciphertext tmp;

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
                input[h][w][c] = activate(input[h][w][c]);
#ifdef __DEBUG__
                option_.decryptor->decrypt(input[h][w][c], plain);
                option_.encoder.decode(plain, vec_tmp);
                debug_file << "\toutput[" << h << "][" << w << "][" << c
                           << "]: " << vec_tmp[0] << ", " << vec_tmp[1] << ", "
                           << vec_tmp[2] << endl;
#endif
            }
        }
    }
}

void Activation::forward(vector<Ciphertext>& input) const
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
        input[u] = activate(input[u]);
#ifdef __DEBUG__
        option.decryptor->decrypt(input[u], plain);
        option_.encoder.decode(plain, vec_tmp);
        debug_file << "\toutput[" << u << "]: " << vec_tmp[0] << ", "
                   << vec_tmp[1] << ", " << vec_tmp[2] << endl;
#endif
    }
}

Ciphertext Activation::activate(Ciphertext& x) const
{
    if (activation_ == SQUARE_NAME)
    {
        return square(x);
    }
    else
    {
        if (option_.enable_optimize_activation)
        {
            return swishDeg4Opt(x);
        }
        else
        {
            return swishDeg4(x);
        }
    }
}

Ciphertext Activation::square(Ciphertext& x) const
{
    Ciphertext y;

    /* Assume that input level is l */
    // Calculate x^2 (Level: l-1)
    option_.evaluator.square(x, y);
    option_.evaluator.relinearize_inplace(y, option_.relin_keys);
    option_.evaluator.rescale_to_next_inplace(y);

    return move(y);
}

Ciphertext Activation::swishDeg4(Ciphertext& x) const
{
    Ciphertext y, x2, x4, ax4, bx2, cx;

    /* Assume that input level is l */
    // Calculate x^2 (Level: l-1)
    option_.evaluator.square(x, x2);
    option_.evaluator.relinearize_inplace(x2, option_.relin_keys);
    option_.evaluator.rescale_to_next_inplace(x2);
    // Calculate x^4 (Level: l-2)
    option_.evaluator.square(x2, x4);
    option_.evaluator.relinearize_inplace(x4, option_.relin_keys);
    option_.evaluator.rescale_to_next_inplace(x4);
    // Reduce modulus of x^2 (Level: l-2)
    option_.evaluator.mod_switch_to_next_inplace(x2);
    // Reduce modulus of x (Level: l-2)
    option_.evaluator.mod_switch_to_next_inplace(x);
    option_.evaluator.mod_switch_to_next_inplace(x);

    // Calculate ax^4 (Level: l-3)
    option_.evaluator.multiply_plain(x4, plain_poly_coeffs_[0], ax4);
    // Calculate bx^2 (Level: l-3)
    option_.evaluator.multiply_plain(x2, plain_poly_coeffs_[1], bx2);
    // Calculate cx (Level: l-3)
    option_.evaluator.multiply_plain(x, plain_poly_coeffs_[2], cx);

    // Normalize scales
    ax4.scale() = option_.scale_param;
    bx2.scale() = option_.scale_param;
    cx.scale() = option_.scale_param;
    // Calculate ax^4 + bx^2 + cx + d (Level: l-3)
    option_.evaluator.add(ax4, bx2, y);
    option_.evaluator.add_inplace(y, cx);
    option_.evaluator.rescale_to_next_inplace(y);
    y.scale() = option_.scale_param;
    option_.evaluator.add_plain_inplace(y, plain_poly_coeffs_[3]);

    return move(y);
}

Ciphertext Activation::swishDeg4Opt(Ciphertext& x) const
{
    Ciphertext y, x2, x4, bx2, cx;

    /* Assume that input level is l */
    // Calculate x^2 (Level: l-1)
    option_.evaluator.square(x, x2);
    option_.evaluator.relinearize_inplace(x2, option_.relin_keys);
    option_.evaluator.rescale_to_next_inplace(x2);
    // Calculate x^4 (Level: l-2)
    option_.evaluator.square(x2, x4);
    option_.evaluator.relinearize_inplace(x4, option_.relin_keys);
    // Reduce modulus of x (Level: l-1)
    option_.evaluator.mod_switch_to_next_inplace(x);

    // Calculate b'x^2 (Level: l-2)
    option_.evaluator.multiply_plain(x2, plain_poly_coeffs_[0], bx2);
    // Calculate c'x (Level: l-2)
    option_.evaluator.multiply_plain(x, plain_poly_coeffs_[1], cx);

    // Normalize scales
    x4.scale() = option_.scale_param;
    bx2.scale() = option_.scale_param;
    cx.scale() = option_.scale_param;
    // Calculate x^4 + b'x^2 + c'x + d' (Level: l-2)
    option_.evaluator.add(x4, bx2, y);
    option_.evaluator.add_inplace(y, cx);
    option_.evaluator.rescale_to_next_inplace(y);
    y.scale() = option_.scale_param;
    option_.evaluator.add_plain_inplace(y, plain_poly_coeffs_[2]);

    return move(y);
}
