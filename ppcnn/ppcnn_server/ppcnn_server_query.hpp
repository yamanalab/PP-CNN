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

#ifndef PPCNN_SERVER_QUERY_HPP
#define PPCNN_SERVER_QUERY_HPP

#include <cstdint>
#include <vector>

#include <ppcnn_share/ppcnn_cli2srvparam.hpp>
#include <ppcnn_share/ppcnn_concurrent_mapqueue.hpp>

#include <seal/seal.h>

namespace ppcnn_server
{

class EncryptionKeys;

/**
 * @brief This class is used to hold the query data.
 */
struct Query
{
    Query() = default;
    /**
     * Constructor
     * @param[in] key_id key ID
     * @param[in] comp_params computation parameters
     * @param[in] ctxts cipher texts
     * @param[in] enc_keys_p encryption keys
     */
    Query(const int32_t key_id, const ppcnn_share::ComputationParams& params,
          const std::vector<seal::Ciphertext>& ctxts,
          const EncryptionKeys* enc_keys_p);
    virtual ~Query() = default;

    /**
     * Copy constructor
     * @param[in] q query
     */
    Query(const Query& q)
      : key_id_(q.key_id_), params_(q.params_), enc_keys_p_(q.enc_keys_p_)
    {
        ctxts_.resize(q.ctxts_.size());
        std::copy(q.ctxts_.begin(), q.ctxts_.end(), ctxts_.begin());
    }

    int32_t key_id_;
    ppcnn_share::ComputationParams params_;
    std::vector<seal::Ciphertext> ctxts_;
    const EncryptionKeys* enc_keys_p_;
};

/**
 * @brief This class is used to hold the queue of queries.
 */
struct QueryQueue
  : public ppcnn_share::ConcurrentMapQueue<int32_t, ppcnn_server::Query>
{
    using super = ppcnn_share::ConcurrentMapQueue<int32_t, ppcnn_server::Query>;

    QueryQueue() = default;
    virtual ~QueryQueue() = default;

    /**
     * Push query in queue
     * @param[in] data query
     */
    virtual int32_t push(const Query& data);
};

} /* namespace ppcnn_server */

#endif /* PPCNN_SERVER_QUERY_HPP */
