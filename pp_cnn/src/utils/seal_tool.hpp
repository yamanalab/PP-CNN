#pragma once

#include "seal/seal.h"

using std::size_t;

class SealTool {
public:
  SealTool() {}
  ~SealTool() {
    delete relin_keys_;
    delete encryptor_;
    delete decryptor_;
    delete evaluator_;
    delete encoder_;
  }

  seal::RelinKeys* relin_keys() const { return relin_keys_; };
  seal::Encryptor* encryptor() const { return encryptor_; };
  seal::Decryptor* decryptor() const { return decryptor_; };
  seal::Evaluator* evaluator() const { return evaluator_; };
  seal::CKKSEncoder* encoder() const { return encoder_; };
  const size_t& slot_count() const { return slot_count_; };
  const double& scale_param() const { return scale_param_; };

  void setRelinKeys(seal::RelinKeys* relin_keys) {
    relin_keys_ = relin_keys;
  };
  void setEncryptor(seal::Encryptor* encryptor) {
    encryptor_ = encryptor;
  };
  void setDecryptor(seal::Decryptor* decryptor) {
    decryptor_ = decryptor;
  };
  void setEvaluator(seal::Evaluator* evaluator) {
    evaluator_ = evaluator;
  };
  void setEncoder(seal::CKKSEncoder* encoder) {
    encoder_ = encoder;
  };
  void setSlotCount(const size_t& slot_count) {
    slot_count_ = slot_count;
  };
  void setScaleParam(const double& scale_param) {
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
