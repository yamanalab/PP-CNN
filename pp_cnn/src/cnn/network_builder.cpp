#include "network_builder.hpp"
#include <stdexcept>
#include "load_model.hpp"
#include "picojson.h"

/**
 * Create CNN
 *
 * @return Network built from pre-trained model
 * @throws std::runtime_error
 */
Network NetworkBuilder::buildNetwork() noexcept(false) {
  Network network;
  picojson::array layers = loadLayers(model_structure_path_);

  if (gOption.enable_fuse_layers()) {
    for (picojson::array::const_iterator it = layers.cbegin(), layers_end = layers.cend(); it != layers_end; ++it) {
      picojson::object layer        = (*it).get<picojson::object>();
      const string layer_class_name = layer["class_name"].get<string>();

      if (it + 1 != layers_end) {
        picojson::object next_layer = (*(it + 1)).get<picojson::object>();
        network.addLayer(buildLayer(layer, next_layer, layer_class_name, model_weights_path_, it));
      } else {
        network.addLayer(buildLayer(layer, layer_class_name, model_weights_path_));
      }
    }
    return network;
  }

  for (picojson::array::const_iterator it = layers.cbegin(), layers_end = layers.cend(); it != layers_end; ++it) {
    picojson::object layer        = (*it).get<picojson::object>();
    const string layer_class_name = layer["class_name"].get<string>();
    network.addLayer(buildLayer(layer, layer_class_name, model_weights_path_));
  }
  return network;
}
