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

#ifndef PPCNN_SEAL_UTILITY_HPP
#define PPCNN_SEAL_UTILITY_HPP

#include <string>
#include <vector>

namespace seal
{
class EncryptionParameters;
}

namespace ppcnn_share
{

namespace seal_utility
{
template <class T>
void write_to_file(const std::string& filepath, const T& data);

template <class T>
void write_to_file(const std::string& filepath, const std::vector<T>& vdata);

template <>
void write_to_file<seal::EncryptionParameters>(
  const std::string& filepath, const seal::EncryptionParameters& params);

template <class T>
size_t stream_size(const T& data);

template <>
size_t stream_size<seal::EncryptionParameters>(
  const seal::EncryptionParameters& params);

template <class T>
void write_to_binary_stream(std::iostream& stream, void* base_ptr_in_stream,
                            const T& data,
                            const bool shift_pos_in_stream = true);

template <class T>
void read_from_binary_stream(std::iostream& stream, void* base_ptr_in_stream,
                             const size_t read_sz, T& data,
                             const bool shift_pos_in_stream = true);

template <class T>
void read_from_binary_stream(std::iostream& stream, void* base_ptr_in_stream,
                             const size_t read_sz,
                             const seal::EncryptionParameters& enc_params,
                             T& data, const bool shift_pos_in_stream = true);

} /* namespace seal_utility */

} /* namespace ppcnn_share */

#endif /* PPCNN_SEAL_UTILITY_HPP */
