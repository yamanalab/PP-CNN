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

#ifndef PPCNN_CLI2SRVPARAM_HPP
#define PPCNN_CLI2SRVPARAM_HPP

#include <iostream>
#include <ppcnn_share/ppcnn_computation_params.hpp>

namespace ppcnn_share
{

/**
 * @brief This class is used to hold the parameters of encryption keys from
 * client to server.
 */
struct C2SEnckeyParam
{
    int32_t key_id;
    size_t enc_params_stream_sz;
    size_t pubkey_stream_sz;
    size_t relinkey_stream_sz;
};

std::ostream& operator<<(std::ostream& os, const C2SEnckeyParam& param);
std::istream& operator>>(std::istream& is, C2SEnckeyParam& param);

/**
 * @brief This class is used to hold the parameters of query from client to
 * server.
 */
struct C2SQueryParam
{
    ComputationParams comp_params;
    size_t enc_inputs_stream_sz;
    int32_t key_id;
};

std::ostream& operator<<(std::ostream& os, const C2SQueryParam& param);
std::istream& operator>>(std::istream& is, C2SQueryParam& param);

/**
 * @brief This class is used to hold the parameters of result request from
 * client to server.
 */
struct C2SResreqParam
{
    int32_t query_id;
};

std::ostream& operator<<(std::ostream& os, const C2SResreqParam& param);
std::istream& operator>>(std::istream& is, C2SResreqParam& param);

} /* namespace ppcnn_share */

#endif /* PPCNN_CLI2SRVPARAM_HPP */
