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

#include <omp.h>
#include <seal/seal.h>
#include <sys/syscall.h> // for thread id
#include <sys/types.h>   // for thread id
#include <unistd.h>
#include <algorithm> // for sort
#include <chrono>
#include <fstream>
#include <random>

#include <stdsc/stdsc_log.hpp>

#include <ppcnn_share/cnn_utils/define.h>
#include <ppcnn_share/cnn_utils/types.h>
#include <ppcnn_share/ppcnn_seal_utility.hpp>
#include <ppcnn_share/ppcnn_utility.hpp>
#include <ppcnn_server/cnn/picojson.h>
#include <ppcnn_server/ppcnn_server_calcthread.hpp>
#include <ppcnn_server/ppcnn_server_keycontainer.hpp>
#include <ppcnn_server/ppcnn_server_query.hpp>
#include <ppcnn_server/ppcnn_server_result.hpp>
#include <ppcnn_server/cnn/load_model.hpp>
#include <ppcnn_server/cnn/network.hpp>

//#define ENABLE_LOCAL_DEBUG

namespace ppcnn_server
{

static Network BuildNetwork(const std::string& model_structure_path,
                            const std::string& model_weights_path,
                            OptOption& option)
{
    Network network;
    picojson::array layers = loadLayers(model_structure_path);

    if (option.enable_fuse_layers)
    {
        for (picojson::array::const_iterator it = layers.cbegin(),
                                             layers_end = layers.cend();
             it != layers_end; ++it)
        {
            picojson::object layer = (*it).get<picojson::object>();
            const string layer_class_name = layer["class_name"].get<string>();

            if (it + 1 != layers_end)
            {
                picojson::object next_layer =
                  (*(it + 1)).get<picojson::object>();
                network.addLayer(buildLayer(layer, next_layer, layer_class_name,
                                            model_weights_path, it, option));
            }
            else
            {
                network.addLayer(buildLayer(layer, layer_class_name,
                                            model_weights_path, option));
            }
        }
        return network;
    }

    for (picojson::array::const_iterator it = layers.cbegin(),
                                         layers_end = layers.cend();
         it != layers_end; ++it)
    {
        picojson::object layer = (*it).get<picojson::object>();
        const string layer_class_name = layer["class_name"].get<string>();
        network.addLayer(
          buildLayer(layer, layer_class_name, model_weights_path, option));
    }

    return network;
}

#define LOGINFO(fmt, ...) \
    STDSC_LOG_INFO("[th:%d,query:%d] " fmt, th_id, query_id, ##__VA_ARGS__)

struct CalcThread::Impl
{
    Impl(QueryQueue& in_queue, ResultQueue& out_queue)
      : in_queue_(in_queue), out_queue_(out_queue)
    {
    }

    void exec(CalcThreadParam& args, std::shared_ptr<stdsc::ThreadException> te)
    {
        auto th_id = syscall(SYS_gettid);
        STDSC_LOG_INFO("Launched calcuration thread. (thread ID: %d)", th_id);

        while (!args.force_finish)
        {

            STDSC_LOG_INFO("[th:%d] Try getting query from QueryQueue.", th_id);

            int32_t query_id;
            Query query;
            while (!in_queue_.pop(query_id, query))
            {
                usleep(args.retry_interval_msec * 1000);
            }

            LOGINFO("Get query. (%s)", query.params_.to_string().c_str());

            const auto dataset_name = std::string(query.params_.dataset);
            const auto model_name = std::string(query.params_.model);
            const std::string base_model_path = args.plaintext_experiment_path +
                                                dataset_name +
                                                "/saved_models/" + model_name;
            const std::string model_structure_path =
              base_model_path + "_structure.json";
            const std::string model_weights_path =
              base_model_path + "_weights.h5";

            if (!ppcnn_share::utility::file_exist(model_structure_path))
            {
                std::ostringstream oss;
                oss << "File not fount. (" << model_structure_path << ")";
                STDSC_THROW_FILE(oss.str());
            }
            if (!ppcnn_share::utility::file_exist(model_weights_path))
            {
                std::ostringstream oss;
                oss << "File not fount. (" << model_weights_path << ")";
                STDSC_THROW_FILE(oss.str());
            }

            std::vector<Ciphertext> encrypted_results(query.params_.labels);

            bool status =
              compute(th_id, query_id, query.params_, *(query.enc_keys_p_),
                      query.ctxts_, model_structure_path, model_weights_path,
                      encrypted_results);

            Result result(query.key_id_, query_id, status, encrypted_results);
            out_queue_.push(query_id, result);

#if defined ENABLE_LOCAL_DEBUG
            for (size_t i = 0; i < encrypted_results.size(); ++i)
            {
                std::ostringstream oss;
                oss << "_enc_results-" << i << ".dat";
                ppcnn_share::seal_utility::write_to_file(oss.str(),
                                                         encrypted_results[i]);
            }
#endif

            LOGINFO("Set result of query.");
        }
    }

    bool compute(const int32_t th_id, const int32_t query_id,
                 const ppcnn_share::ComputationParams& params,
                 const EncryptionKeys& enc_keys,
                 const std::vector<seal::Ciphertext>& ctxts,
                 const std::string& model_structure_path,
                 const std::string& model_weights_path,
                 std::vector<Ciphertext>& encrypted_results)
    {
        LOGINFO("Start computation.\n");
        bool res = true;
        auto context = seal::SEALContext::Create(*(enc_keys.params));

        auto& pubkey = *(enc_keys.pubkey);
        auto& relin_keys = *(enc_keys.relinkey);
        std::shared_ptr<seal::Evaluator> evaluator(
          new seal::Evaluator(context));
        std::shared_ptr<seal::CKKSEncoder> encoder(
          new seal::CKKSEncoder(context));

        auto opt_level = static_cast<EOptLevel>(params.opt_level);
        auto activation = static_cast<EActivation>(params.activation);
        OptOption option(opt_level, activation, relin_keys, *evaluator,
                         *encoder);

        auto trained_model_name = std::string(params.model);
        if (option.enable_optimize_activation)
        {
            if (trained_model_name.find("CKKS-swish_rg4_deg4") !=
                  std::string::npos ||
                activation == SWISH_RG4_DEG4)
            {
                option.highest_deg_coeff = SWISH_RG4_DEG4_COEFFS.front();
            }
            else if (trained_model_name.find("CKKS-swish_rg6_deg4") !=
                       string::npos ||
                     activation == SWISH_RG6_DEG4)
            {
                option.highest_deg_coeff = SWISH_RG6_DEG4_COEFFS.front();
            }
        }

        LOGINFO("Buiding network from trained model...\n");
        Network network =
          BuildNetwork(model_structure_path, model_weights_path, option);
        STDSC_LOG_INFO("Finish buiding.\n");

        network.printStructure();

        LOGINFO("Predicting...\n");

        const auto rows = params.img_height;
        const auto cols = params.img_width;
        const auto channels = params.img_channels;
        Ciphertext3D encrypted_packed_images(
          boost::extents[rows][cols][channels]);

        auto* dst = encrypted_packed_images.data();
        std::memcpy(dst, ctxts.data(), sizeof(ctxts[0]) * ctxts.size());

#if defined ENABLE_LOCAL_DEBUG
        for (size_t i = 0; i < rows * cols * channels; ++i)
        {
            std::ostringstream oss;
            oss << "_enc_inputs-" << i << ".dat";
            ppcnn_share::seal_utility::write_to_file(oss.str(), dst[i]);
        }
#endif

        encrypted_results = network.predict(encrypted_packed_images);

        STDSC_LOG_INFO("Finish predicting.\n");

        return res;
    }

    QueryQueue& in_queue_;
    ResultQueue& out_queue_;
    CalcThreadParam param_;
    std::shared_ptr<stdsc::ThreadException> te_;
};

CalcThread::CalcThread(QueryQueue& in_queue, ResultQueue& out_queue)
  : pimpl_(new Impl(in_queue, out_queue))
{
}

void CalcThread::start()
{
    pimpl_->param_.force_finish = false;
    super::start(pimpl_->param_, pimpl_->te_);
}

void CalcThread::stop()
{
    STDSC_LOG_INFO("Stop calculation thread.");
    pimpl_->param_.force_finish = true;
}

void CalcThread::exec(CalcThreadParam& args,
                      std::shared_ptr<stdsc::ThreadException> te) const
{
    pimpl_->exec(args, te);
}

} /* namespace ppcnn_server */
