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


#include <unordered_map>

#include <stdsc/stdsc_exception.hpp>

#include <ppcnn_server/ppcnn_server_keycontainer.hpp>

#include <seal/seal.h>

namespace ppcnn_server
{
struct KeyContainer::Impl
{
    Impl()
    {
    }

    std::unordered_map<int32_t, EncryptionKeys> keymap_;
};

KeyContainer::KeyContainer() : pimpl_(new Impl())
{
}

void KeyContainer::register_keys(const int32_t key_id,
                                 const seal::EncryptionParameters& params,
                                 const seal::PublicKey& pubkey,
                                 const seal::RelinKeys& relinkey)
{
    EncryptionKeys enckeys(params, pubkey, relinkey);
    pimpl_->keymap_.emplace(key_id, enckeys);
}

// const seal::EncryptionParameters& KeyContainer::get_params(const int32_t
// key_id) const
//{
//    if (pimpl_->keymap_.count(key_id) == 0) {
//        std::ostringstream oss;
//        oss << "Value not found. (key_id: ";
//        oss << key_id << ")";
//        STDSC_THROW_INVPARAM(oss.str());
//    }
//    return *(pimpl_->keymap_.at(key_id).params_);
//}
const EncryptionKeys& KeyContainer::get_keys(const int32_t key_id) const
{
    if (pimpl_->keymap_.count(key_id) == 0)
    {
        std::ostringstream oss;
        oss << "Value not found. (key_id: ";
        oss << key_id << ")";
        STDSC_THROW_INVPARAM(oss.str());
    }
    return pimpl_->keymap_.at(key_id);
}

EncryptionKeys::EncryptionKeys(const seal::EncryptionParameters& params,
                               const seal::PublicKey& pubkey,
                               const seal::RelinKeys& relinkey)
  : params(new seal::EncryptionParameters(params)),
    pubkey(new seal::PublicKey(pubkey)),
    relinkey(new seal::RelinKeys(relinkey))
{
}

} /* namespace ppcnn_server */
