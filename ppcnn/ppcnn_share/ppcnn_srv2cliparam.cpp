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

#include <ppcnn_share/ppcnn_srv2cliparam.hpp>

namespace ppcnn_share
{

std::ostream& operator<<(std::ostream& os, const Srv2CliParam& param)
{
    auto i32_result = static_cast<int32_t>(param.result);
    os << i32_result << std::endl;
    os << param.enc_results_stream_sz;
    return os;
}

std::istream& operator>>(std::istream& is, Srv2CliParam& param)
{
    int32_t i32_result;
    is >> i32_result;
    is >> param.enc_results_stream_sz;
    param.result = static_cast<ServerCalcResult_t>(i32_result);
    return is;
}

} /* namespace ppcnn_share */
