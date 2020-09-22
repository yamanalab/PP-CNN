#pragma once

#include <boost/multi_array.hpp>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "opt_option.hpp"
#include "seal/seal.h"
#include "seal_tool.hpp"

constexpr std::size_t PRE_SUF_PRIME_BIT_SIZE       = 50;
constexpr std::size_t INTERMEDIATE_PRIMES_BIT_SIZE = 30;
// constexpr std::size_t PRE_SUF_PRIME_BIT_SIZE       = 60;
// constexpr std::size_t INTERMEDIATE_PRIMES_BIT_SIZE = 40;

const std::map<std::pair<std::size_t, std::size_t>, float> EPSILON_MAP{
  { std::make_pair(50, 30), 0.0000001 },  // epsilon = 1e-7 when coeff_modulus is {50, 30, ..., 30, 50}
  { std::make_pair(60, 40), 0.0000001 }   // epsilon = 1e-7 when coeff_modulus is {60, 40, ..., 40, 60}
};
// Rounding value for when encode too small value (depending on SEAL parameter)
// if fabs(target_encode_value) < EPSILON, we change target_encode_value = EPSILON * (target_encode_value/fabs(target_encode_value))
const float EPSILON = EPSILON_MAP.at(std::make_pair(PRE_SUF_PRIME_BIT_SIZE, INTERMEDIATE_PRIMES_BIT_SIZE));

using float2D      = boost::multi_array<float, 2>;
using float4D      = boost::multi_array<float, 4>;
using Plaintext2D  = boost::multi_array<seal::Plaintext, 2>;
using Plaintext3D  = boost::multi_array<seal::Plaintext, 3>;
using Plaintext4D  = boost::multi_array<seal::Plaintext, 4>;
using Ciphertext2D = boost::multi_array<seal::Ciphertext, 2>;
using Ciphertext3D = boost::multi_array<seal::Ciphertext, 3>;
using Ciphertext4D = boost::multi_array<seal::Ciphertext, 4>;

enum EOptLevel {
  NO_OPT         = 0,
  FUSE_LAYERS    = 1,
  OPT_ACTIVATION = 2,
  OPT_POOLING    = 3,
  ALL_OPT        = 4
};

enum EActivation {
  DEFAULT,
  SQUARE,
  SWISH_RG4_DEG4,
  SWISH_RG6_DEG4,
  MISH_RG4_DEG4,
  MISH_RG6_DEG4
};

enum ELayerClass {
  CONV2D,
  AVERAGE_POOLING2D,
  ACTIVATION,
  BATCH_NORMALIZATION,
  DENSE,
  FLATTEN,
  GLOBAL_AVERAGE_POOLING2D
};

extern std::vector<float> SWISH_RG4_DEG4_COEFFS;
extern std::vector<float> SWISH_RG6_DEG4_COEFFS;
extern std::vector<float> SWISH_RG4_DEG4_OPT_COEFFS;
extern std::vector<float> SWISH_RG6_DEG4_OPT_COEFFS;

extern OptOption gOption;
extern SealTool gTool;
/* memo of consumed multiplicative level that is used when load trained model */
extern std::size_t gConsumedLevel;
/* highest degree coeff of polynomial activation */
extern float gHighestDegCoeff;
/* pooling multiply factor */
extern float gCurrentPoolingMulFactor;
/* if optimize_activation option is true,
   multiply highest degree coeff of polynomial to parameter of next layer of activation layer */
extern bool gShouldMultiplyCoeff;
/* if optimize_pooling option is true,
   multiply 1/(pool_height * pool_width) to parameter of next layer of average pooling layer */
extern bool gShouldMultiplyPool;
/* activation functoin specify when use Plain_relu/Plain_swish model */
extern EActivation gActivation;

#ifdef __DEBUG__
extern std::string DEBUG_FILE_PATH;
#endif
