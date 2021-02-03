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

#pragma once

#include <unistd.h>
#include <memory>

#include <ppcnn_share/cnn_utils/types.h>

#include <seal/seal.h>

struct OptOption
{
    OptOption(const EOptLevel opt_level, const EActivation act,
              seal::RelinKeys& relinkey, seal::Evaluator& evaluator,
              seal::CKKSEncoder& encoder);
    ~OptOption() = default;

    bool enable_fuse_layers;
    bool enable_optimize_activation;
    bool enable_optimize_pooling;

    bool should_multiply_coeff;
    bool should_multiply_pool;

    EActivation activation;

    float highest_deg_coeff;
    float current_pooling_mul_factor;

    size_t consumed_level;

    seal::RelinKeys& relin_keys;
    seal::Evaluator& evaluator;
    seal::CKKSEncoder& encoder;
    size_t slot_count;
    double scale_param;
};
