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

#ifndef PPCNN_SRV2CLIPARAM_HPP
#define PPCNN_SRV2CLIPARAM_HPP

#include <iostream>

namespace ppcnn_share
{

/**
 * @brief Enumeration for results of computation on Cs.
 */
enum ServerCalcResult_t : int32_t
{
    kServerCalcResultNil = -1,
    kServerCalcResultSuccess = 0,
    kServerCalcResultFailed = 1,
};

/**
 * @brief This class is used to hold the parameters to transfer from cs to user.
 */
struct Srv2CliParam
{
    ServerCalcResult_t result = kServerCalcResultNil;
    size_t enc_results_stream_sz;
};

std::ostream& operator<<(std::ostream& os, const Srv2CliParam& param);
std::istream& operator>>(std::istream& is, Srv2CliParam& param);

} /* namespace ppcnn_share */

#endif /* PPCNN_SRV2CLIPARAM_HPP */
