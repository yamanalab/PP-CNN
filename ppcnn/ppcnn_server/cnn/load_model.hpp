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

#include "layer.hpp"
#include "picojson.h"

picojson::array loadLayers(const string& model_structure_path);

Layer* buildLayer(picojson::object& layer, const string& layer_class_name,
                  const string& model_weights_path, OptOption& option);

Layer* buildLayer(picojson::object& layer, picojson::object& next_layer,
                  const string& layer_class_name,
                  const string& model_weight_path,
                  picojson::array::const_iterator& layers_itrator,
                  OptOption& option);

Layer* buildConv2D(picojson::object& layer_info,
                   const string& model_weights_path, OptOption& option);

Layer* buildAveragePooling2D(picojson::object& layer_info,
                             const string& model_weights_path,
                             OptOption& option);

Layer* buildBatchNormalization(picojson::object& layer_info,
                               const string& model_weights_path,
                               OptOption& option);

Layer* buildFlatten(picojson::object& layer_info,
                    const string& model_weights_path, OptOption& option);

Layer* buildDense(picojson::object& layer_info,
                  const string& model_weights_path, OptOption& option);

Layer* buildActivation(picojson::object& layer_info,
                       const string& model_weights_path, OptOption& option);

Layer* buildGlobalAveragePooling2D(picojson::object& layer_info,
                                   const string& model_weights_path,
                                   OptOption& option);

Layer* buildConv2DFusedBN(picojson::object& conv2d_layer_info,
                          picojson::object& bn_layer_info,
                          const string& model_weights_path, OptOption& option);

Layer* buildDenseFusedBN(picojson::object& dense_layer_info,
                         picojson::object& bn_layer_info,
                         const string& model_weights_path, OptOption& option);
