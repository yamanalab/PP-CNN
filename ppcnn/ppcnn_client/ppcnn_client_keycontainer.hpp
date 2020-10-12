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

#ifndef PPCNN_CLIENT_KEYCONTAINER_HPP
#define PPCNN_CLIENT_KEYCONTAINER_HPP

#include <memory>
#include <seal/seal.h>

namespace ppcnn_share
{
class User2DecParam;
}

namespace ppcnn_client
{

enum KeyKind_t : int32_t
{
    kKindUnknown = -1,
    kKindPubKey = 0,
    kKindSecKey = 1,
    kKindGaloisKey = 2,
    kKindRelinKey = 3,
    kKindParam = 4,
    kNumOfKind,
};

/**
 * @brief This class is used to hold the SEAL keys.
 */
struct KeyContainer
{
    KeyContainer();
    virtual ~KeyContainer() = default;

    /**
     * Generate new keys.
     * @param[in] power power
     * @param[in] level level
     * @return key ID
     */
    int32_t new_keys(const size_t power, const size_t level);

    /**
     * Delete keys.
     * @param[in] key_id key ID
     */
    void delete_keys(const int32_t key_id);

    /**
     * get keys.
     * @param[in] key_id key ID
     * @param[in] kind key kind
     * @param[out] data key data
     */
    template <class T>
    void get(const int32_t key_id, const KeyKind_t kind, T& data) const;

    /**
     * get encryption parameters
     * @param[in] key_id key ID
     * @param[out] params encryption parameters
     */
    void get_param(const int32_t key_id,
                   seal::EncryptionParameters& params) const;

    /**
     * get data size
     * @param[in] key_id key ID
     * @param[in] kind key kind
     */
    size_t data_size(const int32_t key_id, const KeyKind_t kind) const;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace ppcnn_client */

#endif /* PPCNN_CLIENT_KEYCONTAINER_HPP */
