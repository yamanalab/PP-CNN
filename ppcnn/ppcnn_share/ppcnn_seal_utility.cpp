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


#include <fstream>
#include <sstream>

#include <ppcnn_share/ppcnn_encdata.hpp>
#include <ppcnn_share/ppcnn_seal_utility.hpp>

#include <seal/seal.h>

namespace ppcnn_share
{

namespace seal_utility
{

template <class T>
void write_to_file(const std::string& filepath, const T& data)
{
    std::ofstream ofs(filepath, std::ios::binary);
    data.save(ofs);
    ofs.close();
}
#define TEMPLATE_INSTANTIATE(type) \
    template void write_to_file(const std::string& filepath, const type& data)

TEMPLATE_INSTANTIATE(seal::SecretKey);
TEMPLATE_INSTANTIATE(seal::PublicKey);
TEMPLATE_INSTANTIATE(seal::GaloisKeys);
TEMPLATE_INSTANTIATE(seal::RelinKeys);
TEMPLATE_INSTANTIATE(seal::Ciphertext);

#undef TEMPLATE_INSTANTIATE

template <class T>
void write_to_file(const std::string& filepath, const std::vector<T>& vdata)
{
    if (vdata.size() > 0)
    {
        std::ofstream ofs(filepath, std::ios::binary);
        for (const auto& data : vdata)
        {
            data.save(ofs);
        }
        ofs.close();
    }
}
#define TEMPLATE_INSTANTIATE(type)                           \
    template void write_to_file(const std::string& filepath, \
                                const std::vector<type>& vdata)

TEMPLATE_INSTANTIATE(seal::SecretKey);
TEMPLATE_INSTANTIATE(seal::PublicKey);
TEMPLATE_INSTANTIATE(seal::GaloisKeys);
TEMPLATE_INSTANTIATE(seal::RelinKeys);
TEMPLATE_INSTANTIATE(seal::Ciphertext);

#undef TEMPLATE_INSTANTIATE

template <>
void write_to_file<seal::EncryptionParameters>(
  const std::string& filepath, const seal::EncryptionParameters& params)
{
    std::ofstream ofs(filepath, std::ios::binary);
    params.save(ofs);
    ofs.close();
}

template <class T>
size_t stream_size(const T& data)
{
    std::ostringstream oss;
    data.save(oss);
    return oss.str().size();
}
#define TEMPLATE_INSTANTIATE(type) template size_t stream_size(const type& data)

TEMPLATE_INSTANTIATE(seal::SecretKey);
TEMPLATE_INSTANTIATE(seal::PublicKey);
TEMPLATE_INSTANTIATE(seal::GaloisKeys);
TEMPLATE_INSTANTIATE(seal::RelinKeys);
TEMPLATE_INSTANTIATE(seal::Ciphertext);

#undef TEMPLATE_INSTANTIATE

template <>
size_t stream_size<seal::EncryptionParameters>(
  const seal::EncryptionParameters& params)
{
    std::ostringstream oss;
    params.save(oss);
    return oss.str().size();
}

template <class T>
void write_to_binary_stream(std::iostream& stream, void* base_ptr_in_stream,
                            const T& data, const bool shift_pos_in_stream)
{
    std::ostringstream oss(std::istringstream::binary);
    auto saved_sz = data.save(oss);

    auto* p = static_cast<uint8_t*>(base_ptr_in_stream) + stream.tellp();
    std::memcpy(p, oss.str().data(), saved_sz);

    if (shift_pos_in_stream)
    {
        stream.seekp(saved_sz, std::ios_base::cur);
    }
}
#define TEMPLATE_INSTANTIATE(type)                                       \
    template void write_to_binary_stream(                                \
      std::iostream& stream, void* base_ptr_in_stream, const type& data, \
      const bool shift_pos_in_stream)

TEMPLATE_INSTANTIATE(seal::SecretKey);
TEMPLATE_INSTANTIATE(seal::PublicKey);
TEMPLATE_INSTANTIATE(seal::GaloisKeys);
TEMPLATE_INSTANTIATE(seal::RelinKeys);
TEMPLATE_INSTANTIATE(seal::Ciphertext);
TEMPLATE_INSTANTIATE(EncData);

#undef TEMPLATE_INSTANTIATE

template <>
void write_to_binary_stream<seal::EncryptionParameters>(
  std::iostream& stream, void* base_ptr_in_stream,
  const seal::EncryptionParameters& params, const bool shift_pos_in_stream)
{
    std::ostringstream oss(std::istringstream::binary);
    auto saved_sz = params.save(oss);

    auto* p = static_cast<uint8_t*>(base_ptr_in_stream) + stream.tellp();
    std::memcpy(p, oss.str().data(), saved_sz);

    if (shift_pos_in_stream)
    {
        stream.seekp(saved_sz, std::ios_base::cur);
    }
}

template <class T>
void read_from_binary_stream(std::iostream& stream, void* base_ptr_in_stream,
                             const size_t read_sz, T& data,
                             const bool shift_pos_in_stream)
{
    auto* p = static_cast<uint8_t*>(base_ptr_in_stream) + stream.tellg();
    std::string s(p, p + read_sz);

    std::istringstream iss(s, std::istringstream::binary);
    data.load(iss);

    if (shift_pos_in_stream)
    {
        stream.seekg(read_sz, std::ios_base::cur);
    }
}
#define TEMPLATE_INSTANTIATE(type)                                           \
    template void read_from_binary_stream(                                   \
      std::iostream& stream, void* base_ptr_in_stream, const size_t read_sz, \
      type& data, const bool shift_pos_in_stream)

TEMPLATE_INSTANTIATE(EncData);

#undef TEMPLATE_INSTANTIATE

template <>
void read_from_binary_stream<seal::EncryptionParameters>(
  std::iostream& stream, void* base_ptr_in_stream, const size_t read_sz,
  seal::EncryptionParameters& params, const bool shift_pos_in_stream)
{
    auto* p = static_cast<uint8_t*>(base_ptr_in_stream) + stream.tellg();
    std::string s(p, p + read_sz);

    std::istringstream iss(s, std::istringstream::binary);
    auto loaded_sz = params.load(iss);

    if (shift_pos_in_stream)
    {
        stream.seekg(loaded_sz, std::ios_base::cur);
    }
}

template <class T>
void read_from_binary_stream(std::iostream& stream, void* base_ptr_in_stream,
                             const size_t read_sz,
                             const seal::EncryptionParameters& enc_params,
                             T& data, const bool shift_pos_in_stream)
{
    auto* p = static_cast<uint8_t*>(base_ptr_in_stream) + stream.tellg();
    std::string s(p, p + read_sz);

    auto context = seal::SEALContext::Create(enc_params);
    std::istringstream iss(s, std::istringstream::binary);
    auto loaded_sz = data.load(context, iss);

    if (shift_pos_in_stream)
    {
        stream.seekg(loaded_sz, std::ios_base::cur);
    }
}
#define TEMPLATE_INSTANTIATE(type)                                           \
    template void read_from_binary_stream(                                   \
      std::iostream& stream, void* base_ptr_in_stream, const size_t read_sz, \
      const seal::EncryptionParameters& enc_params, type& data,              \
      const bool shift_pos_in_stream)

TEMPLATE_INSTANTIATE(seal::SecretKey);
TEMPLATE_INSTANTIATE(seal::PublicKey);
TEMPLATE_INSTANTIATE(seal::GaloisKeys);
TEMPLATE_INSTANTIATE(seal::RelinKeys);
TEMPLATE_INSTANTIATE(seal::Ciphertext);

#undef TEMPLATE_INSTANTIATE

} /* namespace seal_utility */

} /* namespace ppcnn_share */
