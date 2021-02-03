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

#ifndef PPCNN_CLIENT_RESULT_CBFUNC_HPP
#define PPCNN_CLIENT_RESULT_CBFUNC_HPP

#include <functional>
#include <vector>

#include <seal/seal.h>

namespace ppcnn_client
{

using cbfunc_t =
  std::function<void(const int32_t query_id, const bool status,
                     const std::vector<seal::Ciphertext>&, void*)>;

} /* namespace ppcnn_client */

#endif /* PPCNN_CLIENT_RESULT_CBFUNC_HPP */
