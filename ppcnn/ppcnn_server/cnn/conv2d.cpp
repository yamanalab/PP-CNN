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
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

#include "conv2d.hpp"

using std::ceil;
using std::cout;
using std::endl;
using std::max;
using std::move;

Conv2D::Conv2D(const string& name, const size_t& in_height,
               const size_t& in_width, const size_t& in_channels,
               const size_t& filter_size, const size_t& filter_height,
               const size_t& filter_width, const size_t& stride_height,
               const size_t& stride_width, const string& padding,
               const string& activation, const Plaintext4D& plain_filters,
               const vector<Plaintext>& plain_biases, OptOption& option)
  : Layer(name, CONV2D),
    in_height_(in_height),
    in_width_(in_width),
    in_channels_(in_channels),
    filter_size_(filter_size),
    filter_height_(filter_height),
    filter_width_(filter_width),
    stride_height_(stride_height),
    stride_width_(stride_width),
    padding_(padding),
    activation_(activation),
    plain_filters_(plain_filters),
    plain_biases_(plain_biases),
    option_(option)
{
    if (padding == "valid")
    {
        out_height_ = ceil(static_cast<float>(in_height_ - filter_height_ + 1) /
                           static_cast<float>(stride_height_));
        out_width_ = ceil(static_cast<float>(in_width_ - filter_width_ + 1) /
                          static_cast<float>(stride_width_));
        pad_top_ = 0;
        pad_bottom_ = 0;
        pad_left_ = 0;
        pad_right_ = 0;
    }
    else if (padding == "same")
    {
        out_height_ = ceil(static_cast<float>(in_height_) /
                           static_cast<float>(stride_height_));
        out_width_ = ceil(static_cast<float>(in_width_) /
                          static_cast<float>(stride_width_));

        size_t pad_along_height, pad_along_width;
        if (size_t rem = in_height_ % stride_height_; rem == 0)
            pad_along_height =
              max(filter_height_ - stride_height_, static_cast<size_t>(0));
        else
            pad_along_height =
              max(filter_height_ - rem, static_cast<size_t>(0));
        if (size_t rem = in_width_ % stride_width_; rem == 0)
            pad_along_width =
              max(filter_width_ - stride_width_, static_cast<size_t>(0));
        else
            pad_along_width = max(filter_width_ - rem, static_cast<size_t>(0));

        pad_top_ = pad_along_height / 2;
        pad_bottom_ = pad_along_height - pad_top_;
        pad_left_ = pad_along_width / 2;
        pad_right_ = pad_along_width - pad_left_;
    }
    out_channels_ = filter_size_;

    option_.consumed_level++;
}
Conv2D::~Conv2D()
{
}

void Conv2D::printInfo() const
{
    cout << CONV2D_CLASS_NAME << ": " << name() << endl;
}

bool Conv2D::isOutOfRangeInput(const int& target_x, const int& target_y) const
{
    return target_x < 0 || target_y < 0 || target_x >= in_width_ ||
           target_y >= in_height_;
}

void Conv2D::forward(Ciphertext3D& input) const
{
    cout << "\tForwarding " << name() << "..." << endl;
    cout << "\t  input shape: " << input.shape()[0] << "x" << input.shape()[1]
         << "x" << input.shape()[2] << endl;
    Ciphertext3D output(boost::extents[out_height_][out_width_][out_channels_]);

    int target_top, target_left, target_x, target_y;
    size_t within_range_counter;
    Ciphertext weighted_pixel;
#ifdef _OPENMP
#pragma omp parallel for collapse(2) private(                        \
  target_top, target_left, target_x, target_y, within_range_counter, \
  weighted_pixel)
#endif
    for (size_t oh = 0; oh < out_height_; ++oh)
    {
        for (size_t ow = 0; ow < out_width_; ++ow)
        {
            target_top = oh * stride_height_ - pad_top_;
            target_left = ow * stride_width_ - pad_left_;
            for (size_t oc = 0; oc < out_channels_; ++oc)
            {
                within_range_counter = 0;
                for (size_t fh = 0; fh < filter_height_; ++fh)
                {
                    for (size_t fw = 0; fw < filter_width_; ++fw)
                    {
                        target_x = target_left + fw;
                        target_y = target_top + fh;
                        if (isOutOfRangeInput(target_x, target_y))
                            continue;
                        within_range_counter++;
                        for (size_t ic = 0; ic < in_channels_; ++ic)
                        {
                            option_.evaluator.multiply_plain(
                              input[target_y][target_x][ic],
                              plain_filters_[fh][fw][ic][oc], weighted_pixel);
                            if (within_range_counter == 1 && ic == 0)
                            {
                                output[oh][ow][oc] = weighted_pixel;
                            }
                            else
                            {
                                option_.evaluator.add_inplace(
                                  output[oh][ow][oc], weighted_pixel);
                            }
                        }
                    }
                }
                option_.evaluator.rescale_to_next_inplace(output[oh][ow][oc]);
                output[oh][ow][oc].scale() = option_.scale_param;
                option_.evaluator.add_plain_inplace(output[oh][ow][oc],
                                                    plain_biases_[oc]);
            }
        }
    }

    input.resize(boost::extents[out_height_][out_width_][out_channels_]);
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
    for (size_t oh = 0; oh < out_height_; ++oh)
    {
        for (size_t ow = 0; ow < out_width_; ++ow)
        {
            for (size_t oc = 0; oc < out_channels_; ++oc)
            {
                input[oh][ow][oc] = move(output[oh][ow][oc]);
#ifdef __DEBUG__
                // if (omp_get_thread_num() == 10) {
                //   gTool.decryptor()->decrypt(input[oh][ow][oc], plain);
                //   gTool.encoder()->decode(plain, vec_tmp);
                //   debug_file << "\toutput[" << oh << "][" << ow << "][" << oc
                //   << "]: " << vec_tmp[0] << ", " << vec_tmp[1] << ", " <<
                //   vec_tmp[2] << endl;
                // }
                gTool.decryptor()->decrypt(input[oh][ow][oc], plain);
                gTool.encoder()->decode(plain, vec_tmp);
                debug_file << "\toutput[" << oh << "][" << ow << "][" << oc
                           << "]: " << vec_tmp[0] << ", " << vec_tmp[1] << ", "
                           << vec_tmp[2] << endl;
#endif
            }
        }
    }
}
