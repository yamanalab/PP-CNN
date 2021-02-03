/*
 * Copyright 2020  Yamana Laboratory, Waseda University
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

#ifndef PPCNN_ENCDATA_HPP
#define PPCNN_ENCDATA_HPP

#include <memory>
#include <vector>

#include <ppcnn_share/ppcnn_basicdata.hpp>

#include <seal/seal.h>

namespace ppcnn_share
{

/**
 * @brief This class is used to hold the encrypted data.
 */
struct EncData : public ppcnn_share::BasicData<seal::Ciphertext>
{
    /**
     * Constructor
     * @param[in] params encryption parameters
     */
    explicit EncData(const seal::EncryptionParameters& params);

    /**
     * Constructor
     * @param[in] params encryption parameters
     * @param[in] ctxt ciphertext
     */
    EncData(const seal::EncryptionParameters& params,
            const seal::Ciphertext& ctxt);

    /**
     * Constructor
     * @param[in] params encryption parameters
     * @param[in] ctxts ciphertexts
     */
    EncData(const seal::EncryptionParameters& params,
            const std::vector<seal::Ciphertext>& ctxts);

    /**
     * Constructor
     * @param[in] params encryption parameters
     * @param[in] ctxts pointer of ciphertexts
     * @param[in] n number of ciphertexts
     */
    EncData(const seal::EncryptionParameters& params,
            const seal::Ciphertext* ctxts, const size_t n);

    virtual ~EncData(void) = default;

    /**
     * Encrypt value
     * @param[in] input_value input value
     * @param[in] pubkey public key
     * @param[in] galoiskey galois keys
     */
    void encrypt(const int64_t input_value, const seal::PublicKey& pubkey,
                 const seal::GaloisKeys& galoiskey);

    /**
     * Encrypt values
     * @param[in] input_values input values
     * @param[in] pubkey public key
     * @param[in] galoiskey galois keys
     */
    void encrypt(const std::vector<int64_t>& input_value,
                 const seal::PublicKey& pubkey,
                 const seal::GaloisKeys& galoiskey);

    /**
     * Decrypt value
     * @param[in] seckey security key
     * @param[in] output_values output values
     */
    void decrypt(const seal::SecretKey& seckey,
                 std::vector<int64_t>& output_values) const;

    /**
     * Save ciphertexts to stream
     * @param[out] os output stream
     * @return saved size (bytes)
     */
    virtual size_t save(std::ostream& os) const override;

    /**
     * Load ciphertexts from stream
     * @param[in] is input stream
     * @return loaded size (bytes)
     */
    virtual size_t load(std::istream& is) override;

    /**
     * Save ciphertexts to file
     * @param[in] filepath filepath
     */
    void save_to_file(const std::string& filepath) const;

    /**
     * Load ciphertexts from file
     * @param[in] filepath filepath
     */
    void load_from_file(const std::string& filepath);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace ppcnn_share */

#endif /* PPCNN_ENCDATA_HPP */
