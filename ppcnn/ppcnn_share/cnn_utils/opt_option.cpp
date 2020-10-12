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

#include <ppcnn_share/cnn_utils/define.h>
#include <ppcnn_share/cnn_utils/opt_option.hpp>

OptOption::OptOption(const EOptLevel opt_level, const EActivation act,
                     seal::RelinKeys& _relin_keys, seal::Evaluator& _evaluator,
                     seal::CKKSEncoder& _encoder)
  : enable_fuse_layers(false),
    enable_optimize_activation(false),
    enable_optimize_pooling(false),
    should_multiply_coeff(false),
    should_multiply_pool(false),
    activation(act),
    highest_deg_coeff(0.0f),
    current_pooling_mul_factor(0.0f),
    consumed_level(0),
    relin_keys(_relin_keys),
    evaluator(_evaluator),
    encoder(_encoder)
{
    switch (opt_level)
    {
        case FUSE_LAYERS:
            enable_fuse_layers = true;
            break;
        case OPT_ACTIVATION:
            enable_optimize_activation = true;
            break;
        case OPT_POOLING:
            enable_optimize_pooling = true;
            break;
        case ALL_OPT:
            enable_fuse_layers = true;
            enable_optimize_activation = true;
            enable_optimize_pooling = true;
            break;
        case NO_OPT:
        default:
            break;
    }

    slot_count = encoder.slot_count();
    scale_param = pow(2.0, INTERMEDIATE_PRIMES_BIT_SIZE);
}
