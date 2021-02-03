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

#pragma once

#include "seal/seal.h"

using std::size_t;

class SealTool
{
public:
    SealTool()
    {
    }
    ~SealTool()
    {
        delete relin_keys_;
        delete encryptor_;
        delete decryptor_;
        delete evaluator_;
        delete encoder_;
    }

    seal::RelinKeys* relin_keys() const
    {
        return relin_keys_;
    };
    seal::Encryptor* encryptor() const
    {
        return encryptor_;
    };
    seal::Decryptor* decryptor() const
    {
        return decryptor_;
    };
    seal::Evaluator* evaluator() const
    {
        return evaluator_;
    };
    seal::CKKSEncoder* encoder() const
    {
        return encoder_;
    };
    const size_t& slot_count() const
    {
        return slot_count_;
    };
    const double& scale_param() const
    {
        return scale_param_;
    };

    void setRelinKeys(seal::RelinKeys* relin_keys)
    {
        relin_keys_ = relin_keys;
    };
    void setEncryptor(seal::Encryptor* encryptor)
    {
        encryptor_ = encryptor;
    };
    void setDecryptor(seal::Decryptor* decryptor)
    {
        decryptor_ = decryptor;
    };
    void setEvaluator(seal::Evaluator* evaluator)
    {
        evaluator_ = evaluator;
    };
    void setEncoder(seal::CKKSEncoder* encoder)
    {
        encoder_ = encoder;
    };
    void setSlotCount(const size_t& slot_count)
    {
        slot_count_ = slot_count;
    };
    void setScaleParam(const double& scale_param)
    {
        scale_param_ = scale_param;
    };

private:
    seal::RelinKeys* relin_keys_;
    seal::Encryptor* encryptor_;
    seal::Decryptor* decryptor_;
    seal::Evaluator* evaluator_;
    seal::CKKSEncoder* encoder_;
    size_t slot_count_;
    double scale_param_;
};
