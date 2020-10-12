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
#include <fstream>
#include <unordered_map>

#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>

#include <ppcnn_share/cnn_utils/define.h>
#include <ppcnn_share/ppcnn_utility.hpp>
#include <ppcnn_client/ppcnn_client_keycontainer.hpp>

#include <seal/seal.h>

#define CHECK_KIND(k)                                                    \
    do                                                                   \
    {                                                                    \
        if (!((k) < kNumOfKind))                                         \
        {                                                                \
            std::ostringstream oss;                                      \
            oss << "Err: Invalid securekey kind. (kind: " << (k) << ")"; \
            STDSC_THROW_INVPARAM(oss.str().c_str());                     \
        }                                                                \
    } while (0)

namespace ppcnn_client
{

inline void print_parameters(std::shared_ptr<seal::SEALContext> context)
{
    // Verify parameters
    if (!context)
    {
        throw std::invalid_argument("context is not set");
    }
    auto& context_data = *context->key_context_data();

    /*
      Which scheme are we using?
    */
    std::string scheme_name;
    switch (context_data.parms().scheme())
    {
        case seal::scheme_type::BFV:
            scheme_name = "BFV";
            break;
        case seal::scheme_type::CKKS:
            scheme_name = "CKKS";
            break;
        default:
            throw std::invalid_argument("unsupported scheme");
    }
    std::cout << "/" << std::endl;
    std::cout << "| Encryption parameters :" << std::endl;
    std::cout << "|   scheme: " << scheme_name << std::endl;
    std::cout << "|   poly_modulus_degree: "
              << context_data.parms().poly_modulus_degree() << std::endl;

    /*
      Print the size of the true (product) coefficient modulus.
    */
    std::cout << "|   coeff_modulus size: ";
    std::cout << context_data.total_coeff_modulus_bit_count() << " (";
    auto coeff_modulus = context_data.parms().coeff_modulus();
    std::size_t coeff_mod_count = coeff_modulus.size();
    for (std::size_t i = 0; i < coeff_mod_count - 1; i++)
    {
        std::cout << coeff_modulus[i].bit_count() << " + ";
    }
    std::cout << coeff_modulus.back().bit_count();
    std::cout << ") bits" << std::endl;

    /*
      For the BFV scheme print the plain_modulus parameter.
    */
    if (context_data.parms().scheme() == seal::scheme_type::BFV)
    {
        std::cout << "|   plain_modulus: "
                  << context_data.parms().plain_modulus().value() << std::endl;
    }

    std::cout << "\\" << std::endl;
}

struct KeyContainer::Impl
{
    struct KeyFilenames
    {
        KeyFilenames(const int32_t id)
        {
            filenames_.emplace(kKindPubKey,
                               std::string("pubkey_") + std::to_string(id));
            filenames_.emplace(kKindSecKey,
                               std::string("seckey_") + std::to_string(id));
            filenames_.emplace(kKindRelinKey,
                               std::string("relinkey_") + std::to_string(id));
            filenames_.emplace(kKindParam,
                               std::string("param_") + std::to_string(id));
        }

        std::string filename(const KeyKind_t kind) const
        {
            CHECK_KIND(kind);
            return filenames_.at(kind);
        }

        std::unordered_map<KeyKind_t, std::string> filenames_;
    };

    Impl()
    {
    }

    int32_t new_keys(const size_t power, const size_t level)
    {
        int32_t key_id = ppcnn_share::utility::gen_uuid();
        map_.emplace(key_id, KeyFilenames(key_id));
        generate_keyfiles(power, level, map_.at(key_id));
        return key_id;
    }

    void delete_keys(const int32_t key_id)
    {
        remove_keyfiles(map_.at(key_id));
        map_.erase(key_id);
    }

    template <class T>
    void get(const int32_t key_id, const KeyKind_t kind, T& data) const
    {
        const auto& filename = map_.at(key_id).filename(kind);
        if (!ppcnn_share::utility::file_exist(filename))
        {
            std::ostringstream oss;
            oss << "File is not found. (" << filename << ")";
            STDSC_THROW_FILE(oss.str());
        }

        seal::EncryptionParameters params;
        get_param(key_id, params);
        auto context = seal::SEALContext::Create(params);

        std::ifstream ifs(filename);
        data.unsafe_load(context, ifs);

        ifs.close();
    }

    void get_param(const int32_t key_id,
                   seal::EncryptionParameters& param) const
    {
        const auto& filename = map_.at(key_id).filename(KeyKind_t::kKindParam);
        if (!ppcnn_share::utility::file_exist(filename))
        {
            std::ostringstream oss;
            oss << "File is not found. (" << filename << ")";
            STDSC_THROW_FILE(oss.str());
        }
        std::ifstream ifs(filename);
        param.load(ifs);
        ifs.close();
    }

    size_t data_size(const int32_t key_id, const KeyKind_t kind) const
    {
        const auto& filename = map_.at(key_id).filename(kind);
        if (!ppcnn_share::utility::file_exist(filename))
        {
            std::ostringstream oss;
            oss << "File is not found. (" << filename << ")";
            STDSC_THROW_FILE(oss.str());
        }
        return ppcnn_share::utility::file_size(filename);
    }

private:
    void generate_keyfiles(const std::size_t power, const std::size_t level,
                           const KeyFilenames& filenames)
    {
        STDSC_LOG_INFO("Generating keys");
        seal::EncryptionParameters params(seal::scheme_type::CKKS);

        const size_t poly_mod_degree =
          static_cast<size_t>(std::pow(2.0, power));
        // Define bit sizes of primes
        // ex) [60, 40, ..., 40, 60] (size of intermediate elements ->
        // multiplicative level)
        std::vector<int> bit_sizes(level, INTERMEDIATE_PRIMES_BIT_SIZE);
        bit_sizes.push_back(PRE_SUF_PRIME_BIT_SIZE);
        bit_sizes.insert(bit_sizes.begin(), PRE_SUF_PRIME_BIT_SIZE);

        params.set_poly_modulus_degree(poly_mod_degree);
        params.set_coeff_modulus(
          seal::CoeffModulus::Create(poly_mod_degree, bit_sizes));

        auto context = seal::SEALContext::Create(params);
        print_parameters(context);

        seal::KeyGenerator keygen(context);
        seal::PublicKey public_key = keygen.public_key();
        seal::SecretKey secret_key = keygen.secret_key();
        seal::RelinKeys relin_keys = keygen.relin_keys();

        std::cout << "Save public key and secret key..." << std::flush;
        std::ofstream pkFile(filenames.filename(KeyKind_t::kKindPubKey),
                             std::ios::binary);
        public_key.save(pkFile);
        pkFile.close();

        std::ofstream skFile(filenames.filename(KeyKind_t::kKindSecKey),
                             std::ios::binary);
        secret_key.save(skFile);
        skFile.close();

        std::ofstream paramsFile(filenames.filename(KeyKind_t::kKindParam),
                                 std::ios::binary);
        params.save(paramsFile);
        paramsFile.close();

        std::ofstream relinFile(filenames.filename(KeyKind_t::kKindRelinKey),
                                std::ios::binary);
        relin_keys.save(relinFile);
        relinFile.close();

        std::cout << "End" << std::endl;
    }

    void remove_keyfiles(const KeyFilenames& filenames)
    {
        int32_t bgn = static_cast<int32_t>(KeyKind_t::kKindPubKey);
        int32_t end = static_cast<int32_t>(KeyKind_t::kKindParam);

        for (auto i = bgn; i <= end; ++i)
        {
            const auto key = static_cast<KeyKind_t>(i);
            const auto& filename = filenames.filename(key);
            auto ret = ppcnn_share::utility::remove_file(filename);
            if (!ret)
            {
                std::ostringstream oss;
                oss << "Failed to remove file. (" << filename << ")";
                STDSC_THROW_FILE(oss.str());
            }
        }
    }

private:
    std::unordered_map<int32_t, KeyFilenames> map_;
};

KeyContainer::KeyContainer() : pimpl_(new Impl())
{
}

int32_t KeyContainer::new_keys(const size_t power, const size_t level)
{
    auto key_id = pimpl_->new_keys(power, level);
    STDSC_LOG_INFO("Generate new keys. (key ID: %d)", key_id);
    return key_id;
}

void KeyContainer::delete_keys(const int32_t key_id)
{
    pimpl_->delete_keys(key_id);
    STDSC_LOG_INFO("Deleted key #d.", key_id);
}

template <class T>
void KeyContainer::get(const int32_t key_id, const KeyKind_t kind,
                       T& data) const
{
    pimpl_->get<T>(key_id, kind, data);
}

#define DEF_GET_WITH_TYPE(type, name)                                     \
    template <>                                                           \
    void KeyContainer::get(const int32_t key_id, const KeyKind_t kind,    \
                           type& data) const                              \
    {                                                                     \
        STDSC_LOG_INFO("Get keys. (key ID: %d, kind: %s)", key_id, name); \
        pimpl_->get(key_id, kind, data);                                  \
    }

DEF_GET_WITH_TYPE(seal::PublicKey, "public key");
DEF_GET_WITH_TYPE(seal::SecretKey, "secret key");
DEF_GET_WITH_TYPE(seal::RelinKeys, "relin keys");

#undef DEF_GET_WITH_TYPE

size_t KeyContainer::data_size(const int32_t key_id, const KeyKind_t kind) const
{
    return pimpl_->data_size(key_id, kind);
}

void KeyContainer::get_param(const int32_t key_id,
                             seal::EncryptionParameters& param) const
{
    STDSC_LOG_INFO("Get encryption parameters. (key ID: %d)", key_id);
    pimpl_->get_param(key_id, param);
}

} /* namespace ppcnn_client */
