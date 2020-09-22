#include "activation.hpp"
#include <omp.h>
#include <exception>
#include <fstream>
#include <iostream>

using std::cout;
using std::endl;
using std::move;
using std::runtime_error;

Activation::Activation(const string& name, const string& activation)
    : Layer(name, ACTIVATION),
      activation_(activation) {
  if (activation_ == SQUARE_NAME || gActivation == SQUARE) {
    gConsumedLevel++;
  } else if (activation_ == SWISH_RG4_DEG4_NAME || gActivation == SWISH_RG4_DEG4) {
    Plaintext plain_coeff;
    if (gOption.enable_optimize_activation()) {
      for (const float& coeff : SWISH_RG4_DEG4_OPT_COEFFS) {
        gTool.encoder()->encode(coeff, gTool.scale_param(), plain_coeff);
        for (size_t lv = 0; lv < gConsumedLevel + 1; ++lv) {
          gTool.evaluator()->mod_switch_to_next_inplace(plain_coeff);
        }
        plain_poly_coeffs_.push_back(plain_coeff);
      }
      gTool.evaluator()->mod_switch_to_next_inplace(plain_poly_coeffs_.back());
      gConsumedLevel += 2;
    } else {
      for (const float& coeff : SWISH_RG4_DEG4_COEFFS) {
        gTool.encoder()->encode(coeff, gTool.scale_param(), plain_coeff);
        for (size_t lv = 0; lv < gConsumedLevel + 2; ++lv) {
          gTool.evaluator()->mod_switch_to_next_inplace(plain_coeff);
        }
        plain_poly_coeffs_.push_back(plain_coeff);
      }
      gTool.evaluator()->mod_switch_to_next_inplace(plain_poly_coeffs_.back());
      gConsumedLevel += 3;
    }
  } else if (activation_ == SWISH_RG6_DEG4_NAME || gActivation == SWISH_RG6_DEG4) {
    Plaintext plain_coeff;
    if (gOption.enable_optimize_activation()) {
      for (const float& coeff : SWISH_RG6_DEG4_OPT_COEFFS) {
        gTool.encoder()->encode(coeff, gTool.scale_param(), plain_coeff);
        for (size_t lv = 0; lv < gConsumedLevel + 1; ++lv) {
          gTool.evaluator()->mod_switch_to_next_inplace(plain_coeff);
        }
        plain_poly_coeffs_.push_back(plain_coeff);
      }
      gTool.evaluator()->mod_switch_to_next_inplace(plain_poly_coeffs_.back());
      gConsumedLevel += 2;
    } else {
      for (const float& coeff : SWISH_RG6_DEG4_COEFFS) {
        gTool.encoder()->encode(coeff, gTool.scale_param(), plain_coeff);
        for (size_t lv = 0; lv < gConsumedLevel + 2; ++lv) {
          gTool.evaluator()->mod_switch_to_next_inplace(plain_coeff);
        }
        plain_poly_coeffs_.push_back(plain_coeff);
      }
      gTool.evaluator()->mod_switch_to_next_inplace(plain_poly_coeffs_.back());
      gConsumedLevel += 3;
    }
  } else {
    throw runtime_error("\"" + activation_ + "\" is not registered as activation function");
  }
}
Activation::~Activation() {}

void Activation::printInfo() const {
  cout << ACTIVATION_CLASS_NAME << ": " << name() << endl;
}

void Activation::forward(Ciphertext3D& input) const {
  cout << "\tForwarding " << name() << "..." << endl;
  cout << "\t  input shape: "
       << input.shape()[0] << "x"
       << input.shape()[1] << "x"
       << input.shape()[2] << endl;
  const size_t height   = input.shape()[0];
  const size_t width    = input.shape()[1];
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
  for (size_t h = 0; h < height; ++h) {
    for (size_t w = 0; w < width; ++w) {
      for (size_t c = 0; c < channels; ++c) {
        input[h][w][c] = activate(input[h][w][c]);
#ifdef __DEBUG__
        // if (omp_get_thread_num() == 10) {
        //   gTool.decryptor()->decrypt(input[h][w][c], plain);
        //   gTool.encoder()->decode(plain, vec_tmp);
        //   debug_file << "\toutput[" << h << "][" << w << "][" << c << "]: " << vec_tmp[0] << ", " << vec_tmp[1] << ", " << vec_tmp[2] << endl;
        // }
        gTool.decryptor()->decrypt(input[h][w][c], plain);
        gTool.encoder()->decode(plain, vec_tmp);
        debug_file << "\toutput[" << h << "][" << w << "][" << c << "]: " << vec_tmp[0] << ", " << vec_tmp[1] << ", " << vec_tmp[2] << endl;
#endif
      }
    }
  }
}

void Activation::forward(vector<Ciphertext>& input) const {
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
  for (size_t u = 0; u < units; ++u) {
    input[u] = activate(input[u]);
#ifdef __DEBUG__
    // if (omp_get_thread_num() == 10) {
    //   gTool.decryptor()->decrypt(input[u], plain);
    //   gTool.encoder()->decode(plain, vec_tmp);
    //   debug_file << "\toutput[" << u << "]: " << vec_tmp[0] << ", " << vec_tmp[1] << ", " << vec_tmp[2] << endl;
    // }
    gTool.decryptor()->decrypt(input[u], plain);
    gTool.encoder()->decode(plain, vec_tmp);
    debug_file << "\toutput[" << u << "]: " << vec_tmp[0] << ", " << vec_tmp[1] << ", " << vec_tmp[2] << endl;
#endif
  }
}

Ciphertext Activation::activate(Ciphertext& x) const {
  if (activation_ == SQUARE_NAME) {
    return square(x);
  } else {
    if (gOption.enable_optimize_activation()) {
      return swishDeg4Opt(x);
    } else {
      return swishDeg4(x);
    }
  }
}

Ciphertext Activation::square(Ciphertext& x) const {
  Ciphertext y;

  /* Assume that input level is l */
  // Calculate x^2 (Level: l-1)
  gTool.evaluator()->square(x, y);
  gTool.evaluator()->relinearize_inplace(y, *(gTool.relin_keys()));
  gTool.evaluator()->rescale_to_next_inplace(y);

  return move(y);
}

Ciphertext Activation::swishDeg4(Ciphertext& x) const {
  Ciphertext y, x2, x4, ax4, bx2, cx;

  /* Assume that input level is l */
  // Calculate x^2 (Level: l-1)
  gTool.evaluator()->square(x, x2);
  gTool.evaluator()->relinearize_inplace(x2, *(gTool.relin_keys()));
  gTool.evaluator()->rescale_to_next_inplace(x2);
  // Calculate x^4 (Level: l-2)
  gTool.evaluator()->square(x2, x4);
  gTool.evaluator()->relinearize_inplace(x4, *(gTool.relin_keys()));
  gTool.evaluator()->rescale_to_next_inplace(x4);
  // Reduce modulus of x^2 (Level: l-2)
  gTool.evaluator()->mod_switch_to_next_inplace(x2);
  // Reduce modulus of x (Level: l-2)
  gTool.evaluator()->mod_switch_to_next_inplace(x);
  gTool.evaluator()->mod_switch_to_next_inplace(x);

  // Calculate ax^4 (Level: l-3)
  gTool.evaluator()->multiply_plain(x4, plain_poly_coeffs_[0], ax4);
  // Calculate bx^2 (Level: l-3)
  gTool.evaluator()->multiply_plain(x2, plain_poly_coeffs_[1], bx2);
  // Calculate cx (Level: l-3)
  gTool.evaluator()->multiply_plain(x, plain_poly_coeffs_[2], cx);

  // Normalize scales
  ax4.scale() = gTool.scale_param();
  bx2.scale() = gTool.scale_param();
  cx.scale()  = gTool.scale_param();
  // Calculate ax^4 + bx^2 + cx + d (Level: l-3)
  gTool.evaluator()->add(ax4, bx2, y);
  gTool.evaluator()->add_inplace(y, cx);
  gTool.evaluator()->rescale_to_next_inplace(y);
  y.scale() = gTool.scale_param();
  gTool.evaluator()->add_plain_inplace(y, plain_poly_coeffs_[3]);

  return move(y);
}

Ciphertext Activation::swishDeg4Opt(Ciphertext& x) const {
  Ciphertext y, x2, x4, bx2, cx;

  /* Assume that input level is l */
  // Calculate x^2 (Level: l-1)
  gTool.evaluator()->square(x, x2);
  gTool.evaluator()->relinearize_inplace(x2, *(gTool.relin_keys()));
  gTool.evaluator()->rescale_to_next_inplace(x2);
  // Calculate x^4 (Level: l-2)
  gTool.evaluator()->square(x2, x4);
  gTool.evaluator()->relinearize_inplace(x4, *(gTool.relin_keys()));
  // Reduce modulus of x (Level: l-1)
  gTool.evaluator()->mod_switch_to_next_inplace(x);

  // Calculate b'x^2 (Level: l-2)
  gTool.evaluator()->multiply_plain(x2, plain_poly_coeffs_[0], bx2);
  // Calculate c'x (Level: l-2)
  gTool.evaluator()->multiply_plain(x, plain_poly_coeffs_[1], cx);

  // Normalize scales
  x4.scale()  = gTool.scale_param();
  bx2.scale() = gTool.scale_param();
  cx.scale()  = gTool.scale_param();
  // Calculate x^4 + b'x^2 + c'x + d' (Level: l-2)
  gTool.evaluator()->add(x4, bx2, y);
  gTool.evaluator()->add_inplace(y, cx);
  gTool.evaluator()->rescale_to_next_inplace(y);
  y.scale() = gTool.scale_param();
  gTool.evaluator()->add_plain_inplace(y, plain_poly_coeffs_[2]);

  return move(y);
}
