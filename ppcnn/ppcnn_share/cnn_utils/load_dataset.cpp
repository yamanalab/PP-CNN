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

#include <memory>
#include <vector>

using std::move;
using std::size_t;
using std::string;
using std::vector;

#include <ppcnn_share/cnn_utils/cifar/cifar10_reader.hpp>
#include <ppcnn_share/cnn_utils/mnist/mnist_reader.hpp>
#include <ppcnn_share/cnn_utils/load_dataset.hpp>

const string MNIST_DATASET_PATH = "../datasets/mnist";
const string CIFAR10_DATASET_PATH = "../datasets/cifar-10";

constexpr size_t NORMALIZE_DENOM = 255;

template <template <typename...> class Container, typename Image>
void normalize(Container<Image>& images)
{
    size_t image_count = images.size();
    size_t pixel_count_per_image = images[0].size();
    for (size_t i = 0; i < image_count; ++i)
    {
        for (size_t j = 0; j < pixel_count_per_image; ++j)
        {
            images[i][j] /= NORMALIZE_DENOM;
        }
    }
}

vector<vector<float>> loadMnistTestImages(const std::string& dataset_dir,
                                          const size_t& test_limit)
{
    mnist::MNIST_dataset<vector, vector<float>, uint8_t> dataset =
      // mnist::read_dataset<vector, vector, float, uint8_t>(MNIST_DATASET_PATH,
      // 1, test_limit);
      mnist::read_dataset<vector, vector, float, uint8_t>(dataset_dir, 1,
                                                          test_limit);

    normalize(dataset.test_images);

    return dataset.test_images;
}

vector<unsigned char> loadMnistTestLabels(const std::string& dataset_dir,
                                          const size_t& test_limit)
{
    mnist::MNIST_dataset<vector, vector<float>, uint8_t> dataset =
      // mnist::read_dataset<vector, vector, float, uint8_t>(MNIST_DATASET_PATH,
      // 1, test_limit);
      mnist::read_dataset<vector, vector, float, uint8_t>(dataset_dir, 1,
                                                          test_limit);

    return dataset.test_labels;
}

vector<vector<float>> loadCifar10TestImages(const std::string& dataset_dir,
                                            const size_t& test_limit)
{
    cifar::CIFAR10_dataset<vector, vector<uint8_t>, uint8_t> dataset =
      // cifar::read_dataset<vector, vector, uint8_t,
      // uint8_t>(CIFAR10_DATASET_PATH, 1, test_limit);
      cifar::read_dataset<vector, vector, uint8_t, uint8_t>(dataset_dir, 1,
                                                            test_limit);

    auto test_images = move(dataset.test_images);

    size_t image_count = test_images.size();
    size_t pixel_count_per_image = test_images[0].size();
    vector<vector<float>> float_test_images(
      image_count, vector<float>(pixel_count_per_image));

    // Translate from uint8_t to float
    for (size_t i = 0; i < image_count; ++i)
    {
        for (size_t j = 0; j < pixel_count_per_image; ++j)
        {
            float_test_images[i][j] =
              static_cast<float>(move(test_images[i][j]));
        }
    }

    normalize(float_test_images);

    return float_test_images;
}

vector<unsigned char> loadCifar10TestLabels(const std::string& dataset_dir,
                                            const size_t& test_limit)
{
    cifar::CIFAR10_dataset<vector, vector<float>, uint8_t> dataset =
      // cifar::read_dataset<vector, vector, float,
      // uint8_t>(CIFAR10_DATASET_PATH, 1, test_limit);
      cifar::read_dataset<vector, vector, float, uint8_t>(dataset_dir, 1,
                                                          test_limit);

    return dataset.test_labels;
}
