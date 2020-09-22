#pragma once

#include <vector>
#include "globals.hpp"

using std::string;
using std::vector;

extern const string SECRETS_DIR;
extern const string PARAMS_FILE_PATH;
extern const string PK_FILE_PATH;
extern const string SK_FILE_PATH;
extern const string RK_FILE_PATH;
extern const string GK_FILE_PATH;

void setupSealTool();
void setupOptimizationOption(const EOptLevel& optimization_level);
void encryptImage(const vector<float>& origin_image, Ciphertext3D& target_image);
void encryptImages(const vector<vector<float>>& origin_images, Ciphertext3D& target_packed_images, const size_t& begin_idx, const size_t& end_idx);
