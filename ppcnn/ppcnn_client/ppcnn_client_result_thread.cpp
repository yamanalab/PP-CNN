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

#include <cstring>
#include <fstream>
#include <memory>
#include <vector>

#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_packet.hpp>

#include <ppcnn_share/ppcnn_encdata.hpp>
#include <ppcnn_share/ppcnn_packet.hpp>
#include <ppcnn_client/ppcnn_client.hpp>
#include <ppcnn_client/ppcnn_client_result_thread.hpp>

namespace ppcnn_client
{

struct ResultThread::Impl
{
    std::shared_ptr<stdsc::ThreadException> te_;

    Impl(const Client& client, const seal::EncryptionParameters& enc_params,
         cbfunc_t cbfunc, void* cbargs)
      : client_(client),
        enc_params_(enc_params),
        cbfunc_(cbfunc),
        cbargs_(cbargs)
    {
        te_ = stdsc::ThreadException::create();
    }

    void exec(ResultThreadParam& args,
              std::shared_ptr<stdsc::ThreadException> te)
    {
        try
        {
            STDSC_LOG_INFO("Launched result thread for query #%d",
                           args.query_id);

            bool status = false;
            ppcnn_share::EncData enc_results(enc_params_);
            client_.recv_results(args.query_id, status, enc_results);

            STDSC_LOG_INFO("Invoke callback function of query #%d",
                           args.query_id);
            cbfunc_(args.query_id, status, enc_results.vdata(), cbargs_);
        }
        catch (const stdsc::AbstractException& e)
        {
            STDSC_LOG_TRACE("Failed to execute result thread. (%s)", e.what());
            te->set_current_exception();
        }
    }

private:
    const Client& client_;
    const seal::EncryptionParameters& enc_params_;
    cbfunc_t cbfunc_;
    void* cbargs_;
};

ResultThread::ResultThread(const Client& client,
                           const seal::EncryptionParameters& enc_params,
                           cbfunc_t cbfunc, void* cbargs)
  : pimpl_(new Impl(client, enc_params, cbfunc, cbargs))
{
}

ResultThread::~ResultThread(void)
{
    super::join();
}

void ResultThread::start(ResultThreadParam& param)
{
    STDSC_LOG_INFO("Start result thread.");
    super::start(param, pimpl_->te_);
}

void ResultThread::wait(void)
{
    STDSC_LOG_INFO("Waiting for finish of result thread.");
    super::join();
    pimpl_->te_->rethrow_if_has_exception();
}

void ResultThread::exec(ResultThreadParam& args,
                        std::shared_ptr<stdsc::ThreadException> te) const
{
    try
    {
        pimpl_->exec(args, te);
    }
    catch (...)
    {
        te->set_current_exception();
    }
}

} /* namespace ppcnn_client */
