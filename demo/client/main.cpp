
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

#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_utility.hpp>

#include <ppcnn_share/cnn_utils/define.h>
#include <ppcnn_share/ppcnn_cli2srvparam.hpp>
#include <ppcnn_share/ppcnn_config.hpp>
#include <ppcnn_share/ppcnn_encdata.hpp>
#include <ppcnn_share/ppcnn_seal_utility.hpp>
#include <ppcnn_share/ppcnn_utility.hpp>
#include <ppcnn_share/cnn_utils/helper_functions.hpp>
#include <ppcnn_share/cnn_utils/load_dataset.hpp>
#include <ppcnn_client/ppcnn_client.hpp>
#include <ppcnn_client/ppcnn_client_keycontainer.hpp>
#include <ppcnn_client/ppcnn_client_result_thread.hpp>

#include <share/define.hpp>

constexpr size_t MNIST_CHANNELS = 1;
constexpr size_t MNIST_HEIGHT = 28;
constexpr size_t MNIST_WIDTH = 28;
constexpr size_t MNIST_LABELS = 10;
constexpr size_t CIFAR10_CHANNELS = 3;
constexpr size_t CIFAR10_HEIGHT = 32;
constexpr size_t CIFAR10_WIDTH = 32;
constexpr size_t CIFAR10_LABELS = 10;

constexpr const char* DEFAULT_DATASET = "mnist";
constexpr const char* DEFAULT_MODEL = "HCNN-DA";
constexpr int32_t DEFAULT_OPTIMIZATION_LEVEL = 0;
constexpr int32_t DEFAULT_ACTIVATION = 0;

//#define ENABLE_LOCAL_DEBUG

struct Option
{
    std::string dataset = DEFAULT_DATASET;
    std::string model = DEFAULT_MODEL;
    int32_t opt_level = DEFAULT_OPTIMIZATION_LEVEL;
    int32_t activation = DEFAULT_ACTIVATION;
    std::string config_filepath;
};

struct CallbackParam
{
    size_t labels = 0;
    size_t img_beg_idx = 0;
    size_t img_end_idx = 0;
    size_t img_count = 0;
    seal::Decryptor* decryptor = nullptr;
    seal::CKKSEncoder* encoder = nullptr;
    std::vector<unsigned char>* test_lbls = nullptr;
};

void callback_func(const int32_t query_id, const bool status,
                   const std::vector<seal::Ciphertext>& enc_results, void* args)
{
    STDSC_LOG_INFO("Callback function for query #%d", query_id);

#if defined ENABLE_LOCAL_DEBUG
    for (size_t i = 0; i < enc_results.size(); ++i)
    {
        std::ostringstream oss;
        oss << "enc_results-" << i << ".dat";
        ppcnn_share::seal_utility::write_to_file(oss.str(), enc_results[i]);
    }
#endif

    auto* param = static_cast<CallbackParam*>(args);

    auto slot_count = param->encoder->slot_count();
    std::vector<seal::Plaintext> plain_results(param->labels);
    std::vector<vector<double>> results(param->img_count,
                                        vector<double>(param->labels));
    std::vector<double> tmp_results(slot_count);

    for (size_t i = 0; i < param->labels; ++i)
    {
        param->decryptor->decrypt(enc_results[i], plain_results[i]);
        param->encoder->decode(plain_results[i], tmp_results);

        for (size_t j = param->img_beg_idx, counter = 0; j < param->img_end_idx;
             ++j)
        {
            results[j][i] = tmp_results[counter++];
        }
    }

    /* Calculate accuracy */
    std::vector<double>::iterator beg_iter, max_iter;
    size_t predicted_label, correct_prediction_count = 0;
    std::vector<unsigned char>& test_labels = *param->test_lbls;

    std::cout << "Calculating accuracy..." << std::endl;
    for (size_t i = 0; i < param->img_count; ++i)
    {
        beg_iter = results[i].begin();
        max_iter = std::max_element(beg_iter, results[i].end());
        predicted_label = std::distance(beg_iter, max_iter);

        if (predicted_label == static_cast<size_t>(test_labels[i]))
        {
            correct_prediction_count++;
        }

#if defined ENABLE_LOCAL_DEBUG
        if (i % 1000 == 0)
        {
            double max_res = -99999.0;
            size_t max_lbl = 0;

            std::cout << "[" << i << "] : ";
            for (size_t label = 0; label < param->labels; ++label)
            {
                double res = results[i][label];
                if (max_res < res)
                {
                    max_res = res;
                    max_lbl = label;
                }
                std::cout << "[" << label << "]: " << res << ", ";
            }
            std::cout << " : Max = " << max_lbl << std::endl;
        }
#endif
    }
    const double accuracy =
      static_cast<double>(correct_prediction_count) / param->img_count;
    std::cout << "Finish calculating!\n" << std::endl;
    std::cout << "Accuracy: " << accuracy << "\n" << std::endl;
}

void init(Option& option, int argc, char* argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "D:M:O:A:C:h")) != -1)
    {
        switch (opt)
        {
            case 'D':
                option.dataset = optarg;
                break;
            case 'M':
                option.model = optarg;
                break;
            case 'O':
                option.opt_level = std::stol(optarg);
                break;
            case 'A':
                option.activation = std::stol(optarg);
                break;
            case 'C':
                option.config_filepath = optarg;
                break;
            case 'h':
            default:
                printf(
                  "Usage: %s [-D dataset] [-M model] [-O opt_level] [-A "
                  "activation] [-C config_filepath]\n",
                  argv[0]);
                exit(1);
        }
    }
}

int32_t init_keys(const std::string& config_filepath, seal::SecretKey& seckey,
                  seal::PublicKey& pubkey, seal::RelinKeys& relinkey,
                  seal::EncryptionParameters& params)
{
    size_t power = 0, level = 0;

    if (ppcnn_share::utility::file_exist(config_filepath))
    {
        ppcnn_share::Config conf;
        conf.load_from_file(config_filepath);

#define READ(key, val, type, vfmt)                                       \
    do                                                                   \
    {                                                                    \
        if (conf.is_exist_key(#key))                                     \
            val = ppcnn_share::config_get_value<type>(conf, #key);       \
        STDSC_LOG_INFO("read fhe parameter. (%s: " vfmt ")", #key, val); \
    } while (0)

        READ(power, power, size_t, "%lu");
        READ(level, level, size_t, "%lu");

#undef READ
    }

    ppcnn_client::KeyContainer keycont;
    auto key_id = keycont.new_keys(power, level);

    keycont.get(key_id, ppcnn_client::KeyKind_t::kKindPubKey, pubkey);
    keycont.get(key_id, ppcnn_client::KeyKind_t::kKindSecKey, seckey);
    keycont.get(key_id, ppcnn_client::KeyKind_t::kKindRelinKey, relinkey);
    keycont.get_param(key_id, params);

#if defined ENABLE_LOCAL_DEBUG
    ppcnn_share::seal_utility::write_to_file("pubkey.dat", pubkey);
    ppcnn_share::seal_utility::write_to_file("seckey.dat", seckey);
    ppcnn_share::seal_utility::write_to_file("relinkey.dat", relinkey);
    ppcnn_share::seal_utility::write_to_file("params.dat", params);
#endif

    return key_id;
}

void compute(const int32_t key_id,
             const std::vector<std::vector<float>> test_imgs,
             const ppcnn_share::ComputationParams& comp_params,
             const std::string& host, const std::string& port,
             const size_t test_img_limit, const size_t number_prediction_trials,
             const seal::PublicKey& pubkey, const seal::RelinKeys& relinkey,
             const seal::EncryptionParameters& enc_params,
             CallbackParam& callback_param)
{
    STDSC_LOG_INFO("Encrypt imgs");

    auto context = seal::SEALContext::Create(enc_params);
    std::shared_ptr<seal::CKKSEncoder> encoder(new seal::CKKSEncoder(context));
    const size_t slot_count = encoder->slot_count();

    const size_t test_img_count = test_imgs.size();
    const size_t step_count = test_img_count / slot_count + 1;
    std::cout << "Number of steps: " << step_count << std::endl;
    std::cout << std::endl;

    size_t rows = comp_params.img_height;
    size_t cols = comp_params.img_width;
    size_t channels = comp_params.img_channels;
    size_t remain_img_count = test_img_count;

    const double scale_param = std::pow(2.0, INTERMEDIATE_PRIMES_BIT_SIZE);
    std::shared_ptr<seal::Encryptor> encryptor(
      new seal::Encryptor(context, pubkey));

    ppcnn_client::Client client(host.c_str(), port.c_str(), enc_params);
    client.connect();
    client.register_enckeys(key_id, pubkey, relinkey);

    for (size_t step = 0, img_count_in_step; step < step_count; ++step)
    {
        std::cout << "Step " << step + 1 << ":\n"
                  << "\t--------------------------------------------------"
                  << std::endl;

        if (slot_count < remain_img_count)
        {
            img_count_in_step = slot_count;
        }
        else
        {
            img_count_in_step = remain_img_count;
        }
        const size_t beg_idx = step * slot_count;
        const size_t end_idx = beg_idx + img_count_in_step;

        callback_param.img_beg_idx = beg_idx;
        callback_param.img_end_idx = end_idx;

        for (size_t n = 0; n < number_prediction_trials; ++n)
        {
            Ciphertext3D enc_packed_imgs(boost::extents[rows][cols][channels]);

            std::cout << "\t<Trial " << n + 1 << ">\n"
                      << "\tEncrypting " << img_count_in_step << " imgs..."
                      << std::endl;
            encryptImages(test_imgs, enc_packed_imgs, beg_idx, end_idx,
                          scale_param, *encryptor, *encoder);

            auto elem_num = rows * cols * channels;
            ppcnn_share::EncData enc_inputs(enc_params, enc_packed_imgs.data(),
                                            elem_num);

#if defined ENABLE_LOCAL_DEBUG
            // ppcnn_share::seal_utility::write_to_file("enc_inputs.txt",
            // enc_inputs.vdata());
            printf("enc_inputs.size(): %ld\n", enc_inputs.vdata().size());
            for (size_t i = 0; i < elem_num; ++i)
            {
                std::ostringstream oss;
                oss << "enc_inputs-" << i << ".dat";
                ppcnn_share::seal_utility::write_to_file(oss.str(),
                                                         enc_inputs.vdata()[i]);
            }
#endif
            client.send_query(key_id, comp_params, enc_inputs, callback_func,
                              &callback_param);
        }
    }

    // wait for finish
    usleep(600 * 1000 * 1000);
}

void exec(Option& option)
{
    const char* host = "localhost";

    STDSC_LOG_INFO("server: %s:%s", host, PORT_SRV);

    seal::SecretKey seckey;
    seal::PublicKey pubkey;
    seal::RelinKeys relinkey;
    seal::EncryptionParameters enc_params(seal::scheme_type::CKKS);
    auto key_id =
      init_keys(option.config_filepath, seckey, pubkey, relinkey, enc_params);
    STDSC_LOG_INFO("Generated encryption keys. (key_id:%d)", key_id);

    size_t test_img_limit = 0, number_prediction_trials = 1;

    std::cout << "Loading test imgs & labels..." << std::endl;
    std::vector<std::vector<float>> test_imgs;
    std::vector<unsigned char> test_lbls;
    std::string datasets_dir = PPCNN_DEFAULT_DATASETS_PATH + option.dataset;
    test_imgs = loadMnistTestImages(datasets_dir, test_img_limit);
    test_lbls = loadMnistTestLabels(datasets_dir, test_img_limit);
    std::cout << "Finish loading!" << std::endl;

    std::cout << "Number of imgs for test: " << test_imgs.size() << std::endl;
    std::cout << std::endl;

    ppcnn_share::ComputationParams comp_params;
    comp_params.img_width = MNIST_WIDTH;
    comp_params.img_height = MNIST_HEIGHT;
    comp_params.img_channels = MNIST_CHANNELS;
    comp_params.labels = MNIST_LABELS;
    strcpy(comp_params.dataset, option.dataset.c_str());
    strcpy(comp_params.model, option.model.c_str());
    comp_params.opt_level = option.opt_level;
    comp_params.activation = option.activation;

    auto context = seal::SEALContext::Create(enc_params);
    std::shared_ptr<seal::Decryptor> decryptor(
      new seal::Decryptor(context, seckey));
    std::shared_ptr<seal::CKKSEncoder> encoder(new seal::CKKSEncoder(context));

    CallbackParam callback_param;
    callback_param.labels = comp_params.labels;
    callback_param.img_count = test_imgs.size();
    callback_param.decryptor = decryptor.get();
    callback_param.encoder = encoder.get();
    callback_param.test_lbls = &test_lbls;

    compute(key_id, test_imgs, comp_params, host, PORT_SRV, test_img_limit,
            number_prediction_trials, pubkey, relinkey, enc_params,
            callback_param);
}

int main(int argc, char* argv[])
{
    STDSC_INIT_LOG();
    try
    {
        Option option;
        init(option, argc, argv);
        STDSC_LOG_INFO("Launched Client demo app.");
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
