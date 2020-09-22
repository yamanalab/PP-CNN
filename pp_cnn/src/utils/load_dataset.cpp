#include "load_dataset.hpp"
#include "cifar/cifar10_reader.hpp"
#include "mnist/mnist_reader.hpp"

using std::move;
using std::size_t;
using std::string;
using std::vector;

const string MNIST_DATASET_PATH   = "../datasets/mnist";
const string CIFAR10_DATASET_PATH = "../datasets/cifar-10";

constexpr size_t NORMALIZE_DENOM = 255;

template <template <typename...> class Container, typename Image>
void normalize(Container<Image>& images) {
  size_t image_count           = images.size();
  size_t pixel_count_per_image = images[0].size();
  for (size_t i = 0; i < image_count; ++i) {
    for (size_t j = 0; j < pixel_count_per_image; ++j) {
      images[i][j] /= NORMALIZE_DENOM;
    }
  }
}

vector<vector<float>> loadMnistTestImages(const size_t& test_limit) {
  mnist::MNIST_dataset<vector, vector<float>, uint8_t> dataset =
      mnist::read_dataset<vector, vector, float, uint8_t>(MNIST_DATASET_PATH, 1, test_limit);

  normalize(dataset.test_images);

  return dataset.test_images;
}

vector<unsigned char> loadMnistTestLabels(const size_t& test_limit) {
  mnist::MNIST_dataset<vector, vector<float>, uint8_t> dataset =
      mnist::read_dataset<vector, vector, float, uint8_t>(MNIST_DATASET_PATH, 1, test_limit);

  return dataset.test_labels;
}

vector<vector<float>> loadCifar10TestImages(const size_t& test_limit) {
  cifar::CIFAR10_dataset<vector, vector<uint8_t>, uint8_t> dataset =
      cifar::read_dataset<vector, vector, uint8_t, uint8_t>(CIFAR10_DATASET_PATH, 1, test_limit);

  auto test_images = move(dataset.test_images);

  size_t image_count           = test_images.size();
  size_t pixel_count_per_image = test_images[0].size();
  vector<vector<float>> float_test_images(image_count, vector<float>(pixel_count_per_image));

  // Translate from uint8_t to float
  for (size_t i = 0; i < image_count; ++i) {
    for (size_t j = 0; j < pixel_count_per_image; ++j) {
      float_test_images[i][j] = static_cast<float>(move(test_images[i][j]));
    }
  }

  normalize(float_test_images);

  return float_test_images;
}

vector<unsigned char> loadCifar10TestLabels(const size_t& test_limit) {
  cifar::CIFAR10_dataset<vector, vector<float>, uint8_t> dataset =
      cifar::read_dataset<vector, vector, float, uint8_t>(CIFAR10_DATASET_PATH, 1, test_limit);

  return dataset.test_labels;
}
