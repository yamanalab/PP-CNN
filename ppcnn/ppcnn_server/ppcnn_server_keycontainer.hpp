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

#ifndef PPCNN_SERVER_KEYCONTAINER_HPP
#define PPCNN_SERVER_KEYCONTAINER_HPP

#include <memory>

namespace seal
{
class EncryptionParameters;
class PublicKey;
class RelinKeys;
} // namespace seal

namespace ppcnn_server
{

class EncryptionKeys;

class KeyContainer
{
public:
    /**
     * Constructor
     */
    KeyContainer();
    virtual ~KeyContainer() = default;

    /**
     * Register encryption keys
     * @param[in] key_id key ID
     * @param[in] params encryption parameters
     * @param[in] pubkey public key
     * @param[in] relinkey relin key
     */
    void register_keys(const int32_t key_id,
                       const seal::EncryptionParameters& params,
                       const seal::PublicKey& pubkey,
                       const seal::RelinKeys& relinkey);

    /**
     * Register encryption keys
     * @param[in] key_id key ID
     */
    // const seal::EncryptionParameters& get_params(const int32_t key_id) const;

    /**
     * Get encryption keys
     * @param[in] key_id key ID
     * @return encryption keys
     */
    const EncryptionKeys& get_keys(const int32_t key_id) const;

private:
    class Impl;
    std::shared_ptr<Impl> pimpl_;
};

struct EncryptionKeys
{
    EncryptionKeys(const seal::EncryptionParameters& params,
                   const seal::PublicKey& pubkey,
                   const seal::RelinKeys& relinkey);
    virtual ~EncryptionKeys() = default;

    std::shared_ptr<seal::EncryptionParameters> params;
    std::shared_ptr<seal::PublicKey> pubkey;
    std::shared_ptr<seal::RelinKeys> relinkey;
};

} /* namespace ppcnn_server */

#endif /* PPCNN_SERVER_KEYCONTAINER_HPP */
