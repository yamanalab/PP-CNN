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

#include <string>
#include <vector>

using std::string;
using std::vector;

extern const string MNIST_DATASET_PATH;
extern const string CIFAR10_DATASET_PATH;

template <template <typename...> class Container, typename Image>
void normalize(Container<Image>& images);

vector<vector<float>> loadMnistTestImages(const std::string& dataset_dir,
                                          const size_t& test_limit);
vector<unsigned char> loadMnistTestLabels(const std::string& dataset_dir,
                                          const size_t& test_limit);
vector<vector<float>> loadCifar10TestImages(const std::string& dataset_dir,
                                            const size_t& test_limit);
vector<unsigned char> loadCifar10TestLabels(const std::string& dataset_dir,
                                            const size_t& test_limit);
