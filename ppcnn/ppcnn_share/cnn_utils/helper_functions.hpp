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

#include <ppcnn_share/cnn_utils/types.h>

#include <vector>

using std::string;
using std::vector;

extern const string SECRETS_DIR;
extern const string PARAMS_FILE_PATH;
extern const string PK_FILE_PATH;
extern const string SK_FILE_PATH;
extern const string RK_FILE_PATH;
extern const string GK_FILE_PATH;

void setupSealTool();

void encryptImage(const vector<float>& origin_image, Ciphertext3D& target_image,
                  const double scale_param, seal::Encryptor& encryptor,
                  seal::CKKSEncoder& encoder);

void encryptImages(const vector<vector<float>>& origin_images,
                   Ciphertext3D& target_packed_images, const size_t& begin_idx,
                   const size_t& end_idx, const double scale_param,
                   seal::Encryptor& encryptor, seal::CKKSEncoder& encoder);
