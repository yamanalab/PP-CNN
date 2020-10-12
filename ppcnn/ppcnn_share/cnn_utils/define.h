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

#ifndef __DEFINE_H__
#define __DEFINE_H__

#include <vector>
#include <map>

constexpr std::size_t PRE_SUF_PRIME_BIT_SIZE       = 50;
constexpr std::size_t INTERMEDIATE_PRIMES_BIT_SIZE = 30;

const std::map<std::pair<std::size_t, std::size_t>, float> EPSILON_MAP{
  { std::make_pair(50, 30), 0.0000001 },  // epsilon = 1e-7 when coeff_modulus is {50, 30, ..., 30, 50}
  { std::make_pair(60, 40), 0.0000001 }   // epsilon = 1e-7 when coeff_modulus is {60, 40, ..., 40, 60}
};
// Rounding value for when encode too small value (depending on SEAL parameter)
// if fabs(target_encode_value) < EPSILON, we change target_encode_value = EPSILON * (target_encode_value/fabs(target_encode_value))
static const float EPSILON = EPSILON_MAP.at(std::make_pair(PRE_SUF_PRIME_BIT_SIZE, INTERMEDIATE_PRIMES_BIT_SIZE));

/***********************
 * Swish approximation
 ***********************/
// ax^4 + bx^2 + cx + d
static std::vector<float> SWISH_RG4_DEG4_COEFFS = { -0.005075, 0.19566, 0.5, 0.03347 };
static std::vector<float> SWISH_RG6_DEG4_COEFFS = { -0.002012, 0.1473, 0.5, 0.1198 };
// x^4 + b'x^2 + c'x + d'
static std::vector<float> SWISH_RG4_DEG4_OPT_COEFFS = { -38.5537, -98.52222, -6.59507 };
static std::vector<float> SWISH_RG6_DEG4_OPT_COEFFS = { -73.2107, -248.5089, -59.5427 };

/***********************
 * Mish approximation
 ***********************/
// ax^4 + bx^3 + cx^2 + dx + e
static std::vector<float> MISH_RG4_DEG4_COEFFS = { -0.00609, -0.004142, 0.21051, 0.565775, 0.06021 };
static std::vector<float> MISH_RG6_DEG4_COEFFS = { -0.002096, -0.001277, 0.148529, 0.53663, 0.169 };
// x^4 + b'x^3 + c'x^2 + d'x + e'
static std::vector<float> MISH_RG4_DEG4_OPT_COEFFS = { 0.68013, -34.5665, -92.9023, -9.8867 };
static std::vector<float> MISH_RG6_DEG4_OPT_COEFFS = { 0.60926, -70.86307, -256.02576, -80.62977 };

#endif/* __DEFINE_H__*/
