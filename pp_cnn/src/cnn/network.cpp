#include "network.hpp"
#include "flatten.hpp"
#include "global_average_pooling2d.hpp"

using std::cout;
using std::dynamic_pointer_cast;
using std::endl;

Network::Network() {}
Network::~Network() {}

void Network::printStructure() const noexcept {
  cout << "============================================================" << endl;
  cout << "Network Structure:" << endl;
  cout << "============================================================" << endl;
  for (const shared_ptr<Layer>& layer : layers_) {
    layer->printInfo();
  }
  cout << "============================================================" << endl;
}

/**
 * Predict label from encrypted image
 *
 * @param input_image: 3D encrypted image
 * @return result of prediction (encrypted)
 * @throws InvalidDowncastException if fail to conversion from Layer to Flatten
 */
vector<Ciphertext> Network::predict(Ciphertext3D& encrypted_3d) const noexcept(false) {
  vector<Ciphertext> encrypted_units;
  size_t input_dim = 3;
  // size_t layers_size = layers_.size();

  // for (size_t i = 0; i < layers_size; ++i) {
  //   shared_ptr<Layer>& layer = layers_.front();
  //   switch (layer->layer_class()) {
  //     case CONV2D:
  //     case AVERAGE_POOLING2D:
  //       layer->forward(encrypted_3d);
  //       break;
  //     case ACTIVATION:
  //     case BATCH_NORMALIZATION:
  //       if (input_dim == 1) {
  //         layer->forward(encrypted_units);
  //       } else {
  //         layer->forward(encrypted_3d);
  //       }
  //       break;
  //     case FLATTEN:
  //       if (shared_ptr<Flatten> flatten_layer = dynamic_pointer_cast<Flatten>(layer)) {
  //         encrypted_units = flatten_layer->flatten(encrypted_3d);
  //         input_dim       = 1;
  //       } else {
  //         throw InvalidDowncastException("Failed to downcast from Layer to Flatten (layer_name: " + flatten_layer->name() + ")");
  //       }
  //       break;
  //     case GLOBAL_AVERAGE_POOLING2D:
  //       if (shared_ptr<GlobalAveragePooling2D> global_average_pooling2d_layer = dynamic_pointer_cast<GlobalAveragePooling2D>(layer)) {
  //         encrypted_units = global_average_pooling2d_layer->flatten(encrypted_3d);
  //         input_dim       = 1;
  //       } else {
  //         throw InvalidDowncastException("Failed to downcast from Layer to GlobalAveragePooling2D (layer_name: " + global_average_pooling2d_layer->name() + ")");
  //       }
  //       break;
  //     case DENSE:
  //       layer->forward(encrypted_units);
  //       break;
  //   }
  //   layer.reset();
  //   layers_.front().reset();
  //   layers_.erase(layers_.begin());
  // }

  for (const shared_ptr<Layer>& layer : layers_) {
    switch (layer->layer_class()) {
      case CONV2D:
      case AVERAGE_POOLING2D:
        layer->forward(encrypted_3d);
        break;
      case ACTIVATION:
      case BATCH_NORMALIZATION:
        if (input_dim == 1) {
          layer->forward(encrypted_units);
        } else {
          layer->forward(encrypted_3d);
        }
        break;
      case FLATTEN:
        if (shared_ptr<Flatten> flatten_layer = dynamic_pointer_cast<Flatten>(layer)) {
          encrypted_units = flatten_layer->flatten(encrypted_3d);
          input_dim       = 1;
        } else {
          throw InvalidDowncastException("Failed to downcast from Layer to Flatten (layer_name: " + flatten_layer->name() + ")");
        }
        break;
      case GLOBAL_AVERAGE_POOLING2D:
        if (shared_ptr<GlobalAveragePooling2D> global_average_pooling2d_layer = dynamic_pointer_cast<GlobalAveragePooling2D>(layer)) {
          encrypted_units = global_average_pooling2d_layer->flatten(encrypted_3d);
          input_dim       = 1;
        } else {
          throw InvalidDowncastException("Failed to downcast from Layer to GlobalAveragePooling2D (layer_name: " + global_average_pooling2d_layer->name() + ")");
        }
        break;
      case DENSE:
        layer->forward(encrypted_units);
        break;
    }
  }

  return encrypted_units;
}
