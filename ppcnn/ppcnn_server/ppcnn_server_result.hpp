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

#ifndef PPCNN_SERVER_RESULT_HPP
#define PPCNN_SERVER_RESULT_HPP

#include <chrono>
#include <cstdbool>
#include <cstdint>
#include <vector>

#include <ppcnn_share/ppcnn_concurrent_mapqueue.hpp>

#include <seal/seal.h>


namespace ppcnn_server
{

/**
 * @brief This class is used to hold the result data.
 */
struct Result
{
    Result() = default;
    /**
     * Constructor
     * @param[in] key_id key ID
     * @param[in] query_id query ID
     * @param[in] status   calcuration status
     * @param[in] ctxts     cipher texts
     */
    Result(const int32_t key_id, const int32_t query_id, const bool status,
           const std::vector<seal::Ciphertext>& ctxts);
    virtual ~Result() = default;

    double elapsed_time() const;

    int32_t key_id_;
    int32_t query_id_;
    bool status_;
    std::vector<seal::Ciphertext> ctxts_;
    std::chrono::system_clock::time_point created_time_;
};

/**
 * @brief This class is used to hold the queue of results.
 */
struct ResultQueue : public ppcnn_share::ConcurrentMapQueue<int32_t, Result>
{
    using super = ppcnn_share::ConcurrentMapQueue<int32_t, Result>;

    ResultQueue() = default;
    virtual ~ResultQueue() = default;
};

} /* namespace ppcnn_server */

#endif /* PPCNN_SERVER_RESULT_HPP */
