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

#ifndef PPCNN_SERVER_HPP
#define PPCNN_SERVER_HPP

#include <memory>

#include <ppcnn_share/ppcnn_define.hpp>

namespace stdsc
{
    class CallbackFunctionContainer;
    class StateContext;
}

namespace ppcnn_server
{

/**
 * @brief Provides Server.
 */
class Server
{
public:
    /**
     * constructor
     * @param[in] port port              number
     * @param[in] callback               callback functions
     * @param[in] state                  state machine
     * @param[in] max_concurrent_queries max concurrent query number
     * @param[in] max_results            max result number
     * @param[in] result_lifetime_sec    result linefile (sec)
     */
    Server(const char* port, stdsc::CallbackFunctionContainer& callback,
           stdsc::StateContext& state,
           const uint32_t max_concurrent_queries =
             PPCNN_DEFAULT_MAX_CONCURRENT_QUERIES,
           const uint32_t max_results = PPCNN_DEFAULT_MAX_RESULTS,
           const uint32_t result_lifetime_sec =
             PPCNN_DEFAULT_MAX_RESULT_LIFETIME_SEC);
    ~Server(void) = default;

    /**
     * start server
     */
    void start();
    /**
     * stop server
     */
    void stop(void);
    /**
     * wait for stopping
     */
    void wait(void);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace ppcnn_server */

#endif /* PPCNN_SERVER_SRV_HPP */
