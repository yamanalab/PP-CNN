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

#ifndef __TYPES_H__
#define __TYPES_H__

#include <boost/multi_array.hpp>
#include <seal/seal.h>

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
    ALL_OPT        = 4,
};

enum EActivation {
    DEFAULT        = 0,
    SQUARE         = 1,
    SWISH_RG4_DEG4 = 2,
    SWISH_RG6_DEG4 = 3,
    MISH_RG4_DEG4  = 4,
    MISH_RG6_DEG4  = 5,
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

#endif/*__TYPES_H__*/
