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
#include <iostream>

#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_state.hpp>

#include <ppcnn_share/ppcnn_cli2srvparam.hpp>
#include <ppcnn_share/ppcnn_encdata.hpp>
#include <ppcnn_share/ppcnn_packet.hpp>
#include <ppcnn_share/ppcnn_plaindata.hpp>
#include <ppcnn_share/ppcnn_seal_utility.hpp>
#include <ppcnn_share/ppcnn_srv2cliparam.hpp>
#include <ppcnn_server/ppcnn_server_calcmanager.hpp>
#include <ppcnn_server/ppcnn_server_callback_function.hpp>
#include <ppcnn_server/ppcnn_server_callback_param.hpp>
#include <ppcnn_server/ppcnn_server_keycontainer.hpp>
#include <ppcnn_server/ppcnn_server_query.hpp>
#include <ppcnn_server/ppcnn_server_result.hpp>
#include <ppcnn_server/ppcnn_server_state.hpp>

#include <seal/seal.h>

#define ENABLE_LOCAL_DEBUG

namespace ppcnn_server
{

// CallbackFunction for Encryption keys
DEFUN_DATA(CallbackFunctionEncryptionKeys)
{
    STDSC_LOG_INFO("Received encryption keys. (current state : %s)",
                   state.current_state_str().c_str());

    DEF_CDATA_ON_ALL(ppcnn_server::CommonCallbackParam);
    auto& key_container = cdata_a->key_container_;

    stdsc::BufferStream rbuffstream(buffer);
    std::iostream rstream(&rbuffstream);

    ppcnn_share::PlainData<ppcnn_share::C2SEnckeyParam> rplaindata;
    rplaindata.load(rstream);
    const auto param = rplaindata.data();
    STDSC_LOG_INFO("Encryption key upload params: key_id: %d", param.key_id);

    seal::EncryptionParameters enc_params(seal::scheme_type::CKKS);
    ppcnn_share::seal_utility::read_from_binary_stream(
      rstream, rbuffstream.data(), param.enc_params_stream_sz, enc_params);
    STDSC_LOG_INFO("Uploaded encryption params.");

    auto context = seal::SEALContext::Create(enc_params);

    seal::PublicKey pubkey;
    ppcnn_share::seal_utility::read_from_binary_stream(
      rstream, rbuffstream.data(), param.pubkey_stream_sz, enc_params, pubkey);
    STDSC_LOG_INFO("Uploaded public key.");

    seal::RelinKeys relinkey;
    ppcnn_share::seal_utility::read_from_binary_stream(
      rstream, rbuffstream.data(), param.relinkey_stream_sz, enc_params,
      relinkey);
    STDSC_LOG_INFO("Uploaded relin keys.");

#if defined ENABLE_LOCAL_DEBUG
    ppcnn_share::seal_utility::write_to_file("params.dat", enc_params);
    ppcnn_share::seal_utility::write_to_file("pubkey.dat", pubkey);
    ppcnn_share::seal_utility::write_to_file("relinkey.dat", relinkey);
#endif

    key_container.register_keys(param.key_id, enc_params, pubkey, relinkey);
    STDSC_LOG_INFO("Registered encryptions keys.");
}

// CallbackFunction for Query
DEFUN_UPDOWNLOAD(CallbackFunctionQuery)
{
    STDSC_LOG_INFO("Received query. (current state : %s)",
                   state.current_state_str().c_str());

    DEF_CDATA_ON_ALL(ppcnn_server::CommonCallbackParam);
    auto& calc_manager = cdata_a->calc_manager_;
    auto& key_container = cdata_a->key_container_;

    stdsc::BufferStream rbuffstream(buffer);
    std::iostream rstream(&rbuffstream);

    ppcnn_share::PlainData<ppcnn_share::C2SQueryParam> rplaindata;
    rplaindata.load(rstream);
    const auto& param = rplaindata.data();
    STDSC_LOG_INFO(
      "Query params: comp_params: {%s}, "
      "enc_inputs_stream_sz: %lu, "
      "key_id: %d",
      param.comp_params.to_string().c_str(), param.enc_inputs_stream_sz,
      param.key_id);

    const auto& enc_keys = key_container.get_keys(param.key_id);
    const auto& enc_params =
      *enc_keys.params; // key_container.get_params(param.key_id);

    ppcnn_share::EncData enc_inputs(enc_params);
    ppcnn_share::seal_utility::read_from_binary_stream(
      rstream, rbuffstream.data(), param.enc_inputs_stream_sz, enc_inputs);
    STDSC_LOG_INFO("Uploaded encryption inputs. (elements: %lu)",
                   enc_inputs.vdata().size());

#if defined ENABLE_LOCAL_DEBUG
    ppcnn_share::seal_utility::write_to_file("params_on_query.dat", enc_params);

    for (size_t i = 0; i < enc_inputs.vdata().size(); ++i)
    {
        std::ostringstream oss;
        oss << "enc_inputs-" << i << ".dat";
        ppcnn_share::seal_utility::write_to_file(oss.str(),
                                                 enc_inputs.vdata()[i]);
    }
#endif

    Query query(param.key_id, param.comp_params, enc_inputs.vdata(), &enc_keys);
    int32_t query_id = calc_manager.push_query(query);
    STDSC_LOG_INFO("Generated query ID. (%d)", query_id);

    ppcnn_share::PlainData<int32_t> splaindata;
    splaindata.push(query_id);

    auto sz = splaindata.stream_size();
    stdsc::BufferStream sbuffstream(sz);
    std::iostream sstream(&sbuffstream);

    splaindata.save(sstream);

    STDSC_LOG_INFO("Sending query ID. (%d)", query_id);
    stdsc::Buffer* bsbuff = &sbuffstream;
    sock.send_packet(
      stdsc::make_data_packet(ppcnn_share::kControlCodeDataQueryID, sz));
    sock.send_buffer(*bsbuff);
    state.set(kEventQuery);
}

// CallbackFunction for Result Request
DEFUN_UPDOWNLOAD(CallbackFunctionResultRequest)
{
    STDSC_LOG_INFO("Received result request. (current state : %s)",
                   state.current_state_str().c_str());

    DEF_CDATA_ON_ALL(ppcnn_server::CommonCallbackParam);
    auto& calc_manager = cdata_a->calc_manager_;
    auto& key_container = cdata_a->key_container_;

    stdsc::BufferStream rbuffstream(buffer);
    std::iostream rstream(&rbuffstream);

    ppcnn_share::PlainData<ppcnn_share::C2SResreqParam> rplaindata;
    rplaindata.load(rstream);
    const auto& param = rplaindata.data();
    STDSC_LOG_INFO("Result request params: query_id: %d", param.query_id);

    Result result;
    calc_manager.pop_result(param.query_id, result);
    STDSC_LOG_INFO("Pop result: key_id:%d, query_id:%d, status:%d\n",
                   result.key_id_, result.query_id_, result.status_);

    const auto& enc_params = *(key_container.get_keys(result.key_id_).params);
#if defined ENABLE_LOCAL_DEBUG
    ppcnn_share::seal_utility::write_to_file("params_on_resreq.dat",
                                             enc_params);
#endif

    ppcnn_share::EncData enc_results(enc_params, result.ctxts_.data(),
                                     result.ctxts_.size());

    ppcnn_share::PlainData<ppcnn_share::Srv2CliParam> splaindata;
    ppcnn_share::Srv2CliParam s2c_param;
    s2c_param.result = result.status_ ? ppcnn_share::kServerCalcResultSuccess
                                      : ppcnn_share::kServerCalcResultFailed;
    s2c_param.enc_results_stream_sz = enc_results.stream_size();
    splaindata.push(s2c_param);
    STDSC_LOG_INFO("Result request ack: result: %d, enc_resutls_stream_sz:%lu",
                   s2c_param.result, s2c_param.enc_results_stream_sz);

    auto sz = splaindata.stream_size() + enc_results.stream_size();
    stdsc::BufferStream sbuffstream(sz);
    std::iostream sstream(&sbuffstream);

    splaindata.save(sstream);
    ppcnn_share::seal_utility::write_to_binary_stream(
      sstream, sbuffstream.data(), enc_results);

    STDSC_LOG_INFO("Sending results... (query ID: %d)", param.query_id);
    stdsc::Buffer* bsbuff = &sbuffstream;
    sock.send_packet(
      stdsc::make_data_packet(ppcnn_share::kControlCodeDataResult, sz));
    sock.send_buffer(*bsbuff);
    STDSC_LOG_INFO("Finish sending results! (query ID: %d)", param.query_id);
    state.set(kEventResultRequest);
}

} /* namespace ppcnn_server */
