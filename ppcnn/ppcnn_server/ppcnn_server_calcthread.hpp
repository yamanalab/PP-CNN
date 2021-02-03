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

#ifndef PPCNN_SERVER_CALCTHREAD_HPP
#define PPCNN_SERVER_CALCTHREAD_HPP

#include <cstdbool>
#include <memory>
#include <vector>

#include <stdsc/stdsc_thread.hpp>

#include <ppcnn_share/ppcnn_define.hpp>

namespace ppcnn_server
{

class CalcThreadParam;
class QueryQueue;
class ResultQueue;

/**
 * @brief Calculation thread
 */
class CalcThread : public stdsc::Thread<CalcThreadParam>
{
    using super = Thread<CalcThreadParam>;

public:
    /**
     * Constructor
     * @param[in] in_queue query queue
     * @param[out] out_queue result queue
     */
    CalcThread(QueryQueue& in_queue, ResultQueue& out_queue);
    virtual ~CalcThread(void) = default;

    /**
     * Start thread
     */
    void start();

    /**
     * Stop thread
     */
    void stop();

private:
    virtual void exec(
      CalcThreadParam& args,
      std::shared_ptr<stdsc::ThreadException> te) const override;

    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

/**
 * @brief This class is used to hold the calcration parameters for CalcThread.
 */
struct CalcThreadParam
{
    static constexpr uint32_t DefaultRetryIntervalMsec = 100;

    uint32_t retry_interval_msec = DefaultRetryIntervalMsec;
    bool force_finish = false;
    std::string plaintext_experiment_path =
      PPCNN_DEFAULT_PLAINTEXT_EXPERIMENT_PATH;
};

} /* namespace ppcnn_server */

#endif /* PPCNN_SERVER_CALCTHREAD_HPP */
