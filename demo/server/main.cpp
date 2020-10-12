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

#include <unistd.h>
#include <iostream>
#include <memory>
#include <string>

#include <stdsc/stdsc_callback_function.hpp>
#include <stdsc/stdsc_callback_function_container.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_state.hpp>

#include <ppcnn_share/ppcnn_packet.hpp>
#include <ppcnn_share/ppcnn_utility.hpp>
#include <ppcnn_server/ppcnn_server.hpp>
#include <ppcnn_server/ppcnn_server_callback_function.hpp>
#include <ppcnn_server/ppcnn_server_callback_param.hpp>
#include <ppcnn_server/ppcnn_server_state.hpp>

#include <share/define.hpp>

struct Option
{
    std::string port = PORT_SRV;
    uint32_t max_queries = PPCNN_DEFAULT_MAX_CONCURRENT_QUERIES;
    uint32_t max_results = PPCNN_DEFAULT_MAX_RESULTS;
    uint32_t max_result_lifetime_sec = PPCNN_DEFAULT_MAX_RESULT_LIFETIME_SEC;
};

void init(Option& option, int argc, char* argv[])
{
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "p:q:r:l:h")) != -1)
    {
        switch (opt)
        {
            case 'p':
                option.port = optarg;
                break;
            case 'q':
                option.max_queries = std::stol(optarg);
                break;
            case 'r':
                option.max_results = std::stol(optarg);
                break;
            case 'l':
                option.max_result_lifetime_sec = std::stol(optarg);
                break;
            case 'h':
            default:
                printf(
                  "Usage: %s [-p port] [-q max_queries] [-r max_results] [-l "
                  "max_lifetime_sec]\n",
                  argv[0]);
                exit(1);
        }
    }
}

void exec(Option& option)
{
    stdsc::StateContext state(std::make_shared<ppcnn_server::StateReady>());

    stdsc::CallbackFunctionContainer callback;
    {
        std::shared_ptr<stdsc::CallbackFunction> cb_enckeys(
          new ppcnn_server::CallbackFunctionEncryptionKeys());
        callback.set(ppcnn_share::kControlCodeDataEncKeys, cb_enckeys);

        std::shared_ptr<stdsc::CallbackFunction> cb_query(
          new ppcnn_server::CallbackFunctionQuery());
        callback.set(ppcnn_share::kControlCodeUpDownloadQuery, cb_query);

        std::shared_ptr<stdsc::CallbackFunction> cb_result(
          new ppcnn_server::CallbackFunctionResultRequest());
        callback.set(ppcnn_share::kControlCodeUpDownloadResult, cb_result);
    }

    const char* host = "localhost";

    std::shared_ptr<ppcnn_server::Server> server(new ppcnn_server::Server(
      option.port.c_str(), callback, state, option.max_queries,
      option.max_results, option.max_result_lifetime_sec));

    server->start();
    server->wait();
}

int main(int argc, char* argv[])
{
    STDSC_INIT_LOG();
    try
    {
        Option option;
        init(option, argc, argv);
        STDSC_LOG_INFO("Launched Server demo app.");
        exec(option);
    }
    catch (stdsc::AbstractException& e)
    {
        STDSC_LOG_ERR("Err: %s", e.what());
    }
    catch (...)
    {
        STDSC_LOG_ERR("Catch unknown exception");
    }

    return 0;
}
