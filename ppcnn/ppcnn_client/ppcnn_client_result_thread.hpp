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

#ifndef PPCNN_CLIENT_RESULT_THREAD_HPP
#define PPCNN_CLIENT_RESULT_THREAD_HPP


#include <string>
#include <vector>
#include <functional>
#include <memory>

#include <stdsc/stdsc_thread.hpp>

#include <ppcnn_client/ppcnn_client_result_cbfunc.hpp>

#include <seal/seal.h>

namespace ppcnn_client
{

class ResultThreadParam;
class Client;

/**
 * @brief The thread to receive result from computation server.
 */
class ResultThread : public stdsc::Thread<ResultThreadParam>
{
    using super = stdsc::Thread<ResultThreadParam>;

public:
    /**
     * Constructor
     * @param[in] client computation server client
     * @parma[in] enc_params encryption parameters
     * @paran[in] cbfunc callback function
     * @param[in] cbargs arguments for callback function
     */
    ResultThread(const Client& client,
                 const seal::EncryptionParameters& enc_params, cbfunc_t cbfunc,
                 void* cbargs);
    virtual ~ResultThread(void);

    /**
     * Start threads
     * @param[in] param thread parameters
     */
    void start(ResultThreadParam& param);

    /**
     * Wait for finish
     */
    void wait(void);

private:
    virtual void exec(
      ResultThreadParam& args,
      std::shared_ptr<stdsc::ThreadException> te) const override;

    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

/**
 * @brief The thread parameters
 */
struct ResultThreadParam
{
    int32_t query_id;
};

} /* namespace ppcnn_client */

#endif /* PPCNN_CLIENT_RESULT_THREAD_HPP */
