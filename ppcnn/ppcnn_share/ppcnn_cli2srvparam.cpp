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

#include <ppcnn_share/ppcnn_cli2srvparam.hpp>

namespace ppcnn_share
{

std::ostream& operator<<(std::ostream& os, const C2SEnckeyParam& param)
{
    os << param.key_id << std::endl;
    os << param.enc_params_stream_sz << std::endl;
    os << param.pubkey_stream_sz << std::endl;
    os << param.relinkey_stream_sz << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, C2SEnckeyParam& param)
{
    is >> param.key_id;
    is >> param.enc_params_stream_sz;
    is >> param.pubkey_stream_sz;
    is >> param.relinkey_stream_sz;
    return is;
}

std::ostream& operator<<(std::ostream& os, const C2SQueryParam& param)
{
    os << param.comp_params;
    os << param.enc_inputs_stream_sz << std::endl;
    os << param.key_id << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, C2SQueryParam& param)
{
    is >> param.comp_params;
    is >> param.enc_inputs_stream_sz;
    is >> param.key_id;
    return is;
}

std::ostream& operator<<(std::ostream& os, const C2SResreqParam& param)
{
    os << param.query_id << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, C2SResreqParam& param)
{
    is >> param.query_id;
    return is;
}

} /* namespace ppcnn_share */
