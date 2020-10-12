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

#include <iomanip> // for setw
#include <vector>

#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>

#include <ppcnn_share/ppcnn_encdata.hpp>
#include <ppcnn_share/ppcnn_utility.hpp>

namespace ppcnn_share
{

struct EncData::Impl
{
    explicit Impl(const seal::EncryptionParameters& params) : params_(params)
    {
    }

    const seal::EncryptionParameters& params_;
};

EncData::EncData(const seal::EncryptionParameters& params)
  : pimpl_(new Impl(params))
{
}

EncData::EncData(const seal::EncryptionParameters& params,
                 const seal::Ciphertext& ctxt)
  : pimpl_(new Impl(params))
{
    vec_.push_back(ctxt);
}

EncData::EncData(const seal::EncryptionParameters& params,
                 const std::vector<seal::Ciphertext>& ctxts)
  : pimpl_(new Impl(params))
{
    vec_.resize(ctxts.size());
    std::copy(ctxts.begin(), ctxts.end(), vec_.begin());
}

EncData::EncData(const seal::EncryptionParameters& params,
                 const seal::Ciphertext* ctxts, const size_t n)
  : pimpl_(new Impl(params))
{
    if (n > 0)
    {
        vec_.resize(n);
        for (size_t i = 0; i < n; ++i)
        {
            vec_[i] = ctxts[i];
        }
    }
}

void EncData::encrypt(const int64_t input_value, const seal::PublicKey& pubkey,
                      const seal::GaloisKeys& galoiskey)
{
    auto context = seal::SEALContext::Create(pimpl_->params_);

    seal::Encryptor encryptor(context, pubkey);
    seal::Evaluator evaluator(context);
    seal::BatchEncoder batch_encoder(context);

    size_t slot_count = batch_encoder.slot_count();
    size_t row_size = slot_count / 2;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);

    // encrypt the LUT query
    std::cout << "  Encrypting ..." << std::flush;
    std::vector<int64_t> query;
    for (size_t i = 0; i < row_size; i++)
    {
        query.push_back(input_value);
    }
    query.resize(slot_count);

    // Printing the matrix is a bit of a pain.
    auto print_matrix = [row_size](auto& matrix) {
        std::cout << std::endl;
        size_t print_size = 5;

        std::cout << "    [";
        for (size_t i = 0; i < print_size; i++)
        {
            std::cout << std::setw(3) << matrix[i] << ",";
        }
        std::cout << std::setw(3) << " ...,";
        for (size_t i = row_size - print_size; i < row_size; i++)
        {
            std::cout << std::setw(3) << matrix[i]
                      << ((i != row_size - 1) ? "," : " ]\n");
        }
        std::cout << "    [";
        for (size_t i = row_size; i < row_size + print_size; i++)
        {
            std::cout << std::setw(3) << matrix[i] << ",";
        }
        std::cout << std::setw(3) << " ...,";
        for (size_t i = 2 * row_size - print_size; i < 2 * row_size; i++)
        {
            std::cout << std::setw(3) << matrix[i]
                      << ((i != 2 * row_size - 1) ? "," : " ]\n");
        }
        std::cout << std::endl;
    };
    seal::Plaintext plaintext_query;
    batch_encoder.encode(query, plaintext_query);
    print_matrix(query);

    seal::Ciphertext ciphertext_query;
    encryptor.encrypt(plaintext_query, ciphertext_query);
    std::cout << "  Done" << std::endl;

    // save in a file
#if defined ENABLE_LOCAL_DEBUG
    std::ofstream queryFile;
    queryFile.open("query", std::ios::binary);
    ciphertext_query.save(queryFile);
    queryFile.close();
#endif

    vec_.push_back(ciphertext_query);
}

void EncData::encrypt(const std::vector<int64_t>& input_values,
                      const seal::PublicKey& pubkey,
                      const seal::GaloisKeys& galoiskey)
{
    auto context = seal::SEALContext::Create(pimpl_->params_);

    seal::Encryptor encryptor(context, pubkey);
    seal::Evaluator evaluator(context);
    seal::BatchEncoder batch_encoder(context);

    size_t slot_count = batch_encoder.slot_count();
    size_t row_size = slot_count / 2;

    // Printing the matrix is a bit of a pain.
    auto print_matrix = [row_size](auto& matrix) {
        std::cout << std::endl;
        size_t print_size = 5;

        std::cout << "    [";
        for (size_t i = 0; i < print_size; i++)
        {
            std::cout << std::setw(3) << matrix[i] << ",";
        }
        std::cout << std::setw(3) << " ...,";
        for (size_t i = row_size - print_size; i < row_size; i++)
        {
            std::cout << std::setw(3) << matrix[i]
                      << ((i != row_size - 1) ? "," : " ]\n");
        }
        std::cout << "    [";
        for (size_t i = row_size; i < row_size + print_size; i++)
        {
            std::cout << std::setw(3) << matrix[i] << ",";
        }
        std::cout << std::setw(3) << " ...,";
        for (size_t i = 2 * row_size - print_size; i < 2 * row_size; i++)
        {
            std::cout << std::setw(3) << matrix[i]
                      << ((i != 2 * row_size - 1) ? "," : " ]\n");
        }
        std::cout << std::endl;
    };

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);

    for (const auto& input_value : input_values)
    {

        // encrypt the LUT query
        std::cout << "  Encrypting ..." << std::flush;
        std::vector<int64_t> query;
        for (size_t i = 0; i < row_size; i++)
        {
            query.push_back(input_value);
        }
        query.resize(slot_count);

        seal::Plaintext plaintext_query;
        batch_encoder.encode(query, plaintext_query);
        print_matrix(query);

        seal::Ciphertext ciphertext_query;
        encryptor.encrypt(plaintext_query, ciphertext_query);
        std::cout << "  Done" << std::endl;

        vec_.push_back(ciphertext_query);
    }

    // save in a file
#if defined ENABLE_LOCAL_DEBUG
    std::ofstream queryFile;
    queryFile.open("query", std::ios::binary);
    ciphertext_query.save(queryFile);
    queryFile.close();
#endif
}

void EncData::decrypt(const seal::SecretKey& secret_key,
                      std::vector<int64_t>& output_values) const
{
    auto& ctxt = vec_[0];
    auto context = seal::SEALContext::Create(pimpl_->params_);

    seal::Decryptor decryptor(context, secret_key);
    seal::BatchEncoder batch_encoder(context);

    // std::vector<int64_t> query;
    seal::Plaintext plaintext_query;
    decryptor.decrypt(ctxt, plaintext_query);

    std::vector<int64_t> tmp_outputs;
    batch_encoder.decode(plaintext_query, tmp_outputs);

    STDSC_LOG_INFO("Decrypt: Noise budget: %ld bits",
                   decryptor.invariant_noise_budget(ctxt));

    for (size_t i = 0; i < tmp_outputs.size(); ++i)
    {
        if (tmp_outputs[i] != 0)
        {
            output_values.push_back(tmp_outputs[i]);
        }
    }
}

size_t EncData::save(std::ostream& os) const
{
    size_t sz = vec_.size();
    os.write(reinterpret_cast<char*>(&sz), sizeof(sz));

    size_t saved_bytes = sizeof(sz);
    for (const auto& v : vec_)
    {
        saved_bytes += v.save(os);
    }
    return saved_bytes;
}

size_t EncData::load(std::istream& is)
{
    size_t sz;
    is.read(reinterpret_cast<char*>(&sz), sizeof(sz));

    clear();

    auto context = seal::SEALContext::Create(pimpl_->params_);

    size_t loaded_bytes = sizeof(sz);
    for (size_t i = 0; i < sz; ++i)
    {
        seal::Ciphertext ctxt;
        loaded_bytes += ctxt.load(context, is);
        vec_.push_back(ctxt);
    }
    return loaded_bytes;
}

void EncData::save_to_file(const std::string& filepath) const
{
    std::ofstream ofs(filepath);
    save(ofs);
    ofs.close();
}

void EncData::load_from_file(const std::string& filepath)
{
    if (!ppcnn_share::utility::file_exist(filepath))
    {
        std::ostringstream oss;
        oss << "File not found. (" << filepath << ")";
        STDSC_THROW_FILE(oss.str());
    }
    std::ifstream ifs(filepath, std::ios::binary);
    load(ifs);
    ifs.close();
}

} /* namespace ppcnn_share */
