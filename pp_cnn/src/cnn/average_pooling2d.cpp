#include "average_pooling2d.hpp"
#include <omp.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

using std::ceil;
using std::cout;
using std::endl;
using std::max;
using std::move;

AveragePooling2D::AveragePooling2D(const string& name,
                                   const size_t& in_height, const size_t& in_width, const size_t& in_channels,
                                   const size_t& pool_height, const size_t& pool_width,
                                   const size_t& stride_height, const size_t& stride_width,
                                   const string& padding,
                                   const Plaintext& plain_mul_factor)
    : Layer(name, AVERAGE_POOLING2D),
      in_height_(in_height),
      in_width_(in_width),
      in_channels_(in_channels),
      pool_height_(pool_height),
      pool_width_(pool_width),
      stride_height_(stride_height),
      stride_width_(stride_width),
      padding_(padding),
      plain_mul_factor_(plain_mul_factor) {
  if (padding == "valid") {
    out_height_ = ceil(static_cast<float>(in_height_ - pool_height_ + 1) / static_cast<float>(stride_height_));
    out_width_  = ceil(static_cast<float>(in_width_ - pool_width_ + 1) / static_cast<float>(stride_width_));
    pad_top_    = 0;
    pad_bottom_ = 0;
    pad_left_   = 0;
    pad_right_  = 0;
  } else if (padding == "same") {
    out_height_ = ceil(static_cast<float>(in_height_) / static_cast<float>(stride_height_));
    out_width_  = ceil(static_cast<float>(in_width_) / static_cast<float>(stride_width_));

    size_t pad_along_height, pad_along_width;
    if (size_t rem = in_height_ % stride_height_; rem == 0)
      pad_along_height = max(pool_height_ - stride_height_, static_cast<size_t>(0));
    else
      pad_along_height = max(pool_height_ - rem, static_cast<size_t>(0));
    if (size_t rem = in_width_ % stride_width_; rem == 0)
      pad_along_width = max(pool_width_ - stride_width_, static_cast<size_t>(0));
    else
      pad_along_width = max(pool_width_ - rem, static_cast<size_t>(0));

    pad_top_    = pad_along_height / 2;
    pad_bottom_ = pad_along_height - pad_top_;
    pad_left_   = pad_along_width / 2;
    pad_right_  = pad_along_width - pad_left_;
  }
  out_channels_ = in_channels_;

  if (!gOption.enable_optimize_pooling()) {
    gConsumedLevel++;
  }
}
AveragePooling2D::~AveragePooling2D() {}

void AveragePooling2D::printInfo() const {
  cout << AVERAGE_POOLING2D_CLASS_NAME << ": " << name() << endl;
}

bool AveragePooling2D::isOutOfRangeInput(const int& target_x, const int& target_y) const {
  return target_x < 0 || target_y < 0 || target_x >= in_width_ || target_y >= in_height_;
}

void AveragePooling2D::forward(Ciphertext3D& input) const {
  cout << "\tForwarding " << name() << "..." << endl;
  cout << "\t  input shape: "
       << input.shape()[0] << "x"
       << input.shape()[1] << "x"
       << input.shape()[2] << endl;
  Ciphertext3D output(boost::extents[out_height_][out_width_][out_channels_]);

  int target_top, target_left, target_x, target_y;

  if (gOption.enable_optimize_pooling()) {
#ifdef _OPENMP
#pragma omp parallel for collapse(2) private(target_top, target_left, target_x, target_y)
#endif
    for (size_t oh = 0; oh < out_height_; ++oh) {
      for (size_t ow = 0; ow < out_width_; ++ow) {
        target_top  = oh * stride_height_ - pad_top_;
        target_left = ow * stride_width_ - pad_left_;
        for (size_t oc = 0; oc < out_channels_; ++oc) {
          for (size_t ph = 0; ph < pool_height_; ++ph) {
            for (size_t pw = 0; pw < pool_width_; ++pw) {
              target_x = target_left + pw;
              target_y = target_top + ph;
              if (isOutOfRangeInput(target_x, target_y)) continue;
              if (ph == 0 && pw == 0) {
                output[oh][ow][oc] = input[target_y][target_x][oc];
              } else {
                gTool.evaluator()->add_inplace(output[oh][ow][oc], input[target_y][target_x][oc]);
              }
            }
          }
        }
      }
    }
  } else {
#ifdef _OPENMP
#pragma omp parallel for collapse(2) private(target_top, target_left, target_x, target_y)
#endif
    for (size_t oh = 0; oh < out_height_; ++oh) {
      for (size_t ow = 0; ow < out_width_; ++ow) {
        target_top  = oh * stride_height_ - pad_top_;
        target_left = ow * stride_width_ - pad_left_;
        for (size_t oc = 0; oc < out_channels_; ++oc) {
          for (size_t ph = 0; ph < pool_height_; ++ph) {
            for (size_t pw = 0; pw < pool_width_; ++pw) {
              target_x = target_left + pw;
              target_y = target_top + ph;
              if (isOutOfRangeInput(target_x, target_y)) continue;
              if (ph == 0 && pw == 0) {
                output[oh][ow][oc] = input[target_y][target_x][oc];
              } else {
                gTool.evaluator()->add_inplace(output[oh][ow][oc], input[target_y][target_x][oc]);
              }
            }
          }
          gTool.evaluator()->multiply_plain_inplace(output[oh][ow][oc], plain_mul_factor_);
          gTool.evaluator()->rescale_to_next_inplace(output[oh][ow][oc]);
        }
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
  for (size_t oh = 0; oh < out_height_; ++oh) {
    for (size_t ow = 0; ow < out_width_; ++ow) {
      for (size_t oc = 0; oc < out_channels_; ++oc) {
        input[oh][ow][oc] = move(output[oh][ow][oc]);
#ifdef __DEBUG__
        // if (omp_get_thread_num() == 10) {
        //   gTool.decryptor()->decrypt(input[oh][ow][oc], plain);
        //   gTool.encoder()->decode(plain, vec_tmp);
        //   debug_file << "\toutput[" << oh << "][" << ow << "][" << oc << "]: " << vec_tmp[0] << ", " << vec_tmp[1] << ", " << vec_tmp[2] << endl;
        // }
        gTool.decryptor()->decrypt(input[oh][ow][oc], plain);
        gTool.encoder()->decode(plain, vec_tmp);
        debug_file << "\toutput[" << oh << "][" << ow << "][" << oc << "]: " << vec_tmp[0] << ", " << vec_tmp[1] << ", " << vec_tmp[2] << endl;
#endif
      }
    }
  }
}
