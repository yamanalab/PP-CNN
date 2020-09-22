#include "global_average_pooling2d.hpp"

using std::cout;
using std::endl;
using std::move;

GlobalAveragePooling2D::GlobalAveragePooling2D(const string& name,
                                               const size_t& in_height, const size_t& in_width, const size_t& in_channels,
                                               const size_t& out_units,
                                               const Plaintext& plain_mul_factor)
    : Layer(name, GLOBAL_AVERAGE_POOLING2D),
      in_height_(in_height),
      in_width_(in_width),
      in_channels_(in_channels),
      out_units_(out_units),
      plain_mul_factor_(plain_mul_factor) {
  // if (!gOption.enable_optimize_pooling()) {
  //   gConsumedLevel++;
  // }
}
GlobalAveragePooling2D::~GlobalAveragePooling2D() {}

void GlobalAveragePooling2D::printInfo() const {
  cout << GLOBAL_AVERAGE_POOLING2D_CLASS_NAME << ": " << name() << endl;
}

vector<Ciphertext> GlobalAveragePooling2D::flatten(Ciphertext3D& input) const {
  cout << "\tForwarding " << name() << "..." << endl;
  cout << "\t  input shape: "
       << input.shape()[0] << "x"
       << input.shape()[1] << "x"
       << input.shape()[2] << endl;
  vector<Ciphertext> flattened_input(out_units_);

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (size_t ou = 0; ou < out_units_; ++ou) {
    flattened_input[ou] = move(input[0][0][ou]);
  }

  // #ifdef _OPENMP
  // #pragma omp parallel for collapse(2)
  // #endif
  for (size_t ih = 0; ih < in_height_; ++ih) {
    for (size_t iw = 0; iw < in_width_; ++iw) {
      if (ih != 0 || iw != 0) {
        for (size_t ic = 0; ic < in_channels_; ++ic) {
          gTool.evaluator()->add_inplace(flattened_input[ic], move(input[ih][iw][ic]));
        }
      }
    }
  }

  //   if (!gOption.enable_optimize_pooling()) {
  // #ifdef _OPENMP
  // #pragma omp parallel for
  // #endif
  //     for (size_t ou = 0; ou < out_units_; ++ou) {
  //       gTool.evaluator()->multiply_plain_inplace(flattened_input[ou], plain_mul_factor_);
  //     }
  //   }

  return flattened_input;
}
