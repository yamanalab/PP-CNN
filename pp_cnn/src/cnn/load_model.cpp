#include "load_model.hpp"

#include <H5Cpp.h>

#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

#include "activation.hpp"
#include "average_pooling2d.hpp"
#include "batch_normalization.hpp"
#include "conv2d.hpp"
#include "conv2d_fused_bn.hpp"
#include "dense.hpp"
#include "dense_fused_bn.hpp"
#include "flatten.hpp"
#include "global_average_pooling2d.hpp"

using namespace H5;
using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::istreambuf_iterator;
using std::map;
using std::move;
using std::runtime_error;
using std::size_t;
using std::sqrt;

const string KERNEL_KEY          = "kernel:0";
const string BIAS_KEY            = "bias:0";
const string BETA_KEY            = "beta:0";
const string GAMMA_KEY           = "gamma:0";
const string MOVING_MEAN_KEY     = "moving_mean:0";
const string MOVING_VARIANCE_KEY = "moving_variance:0";

const map<const string, Layer* (*)(picojson::object&, const string&)> BUILD_LAYER_MAP{
  { CONV2D_CLASS_NAME, buildConv2D },
  { AVERAGE_POOLING2D_CLASS_NAME, buildAveragePooling2D },
  { BATCH_NORMALIZATION_CLASS_NAME, buildBatchNormalization },
  { FLATTEN_CLASS_NAME, buildFlatten },
  { DENSE_CLASS_NAME, buildDense },
  { ACTIVATION_CLASS_NAME, buildActivation },
  { GLOBAL_AVERAGE_POOLING2D_CLASS_NAME, buildGlobalAveragePooling2D }
};

size_t next_layer_in_height;
size_t next_layer_in_width;
size_t next_layer_in_channels;
size_t next_layer_in_units;

/**
 * Round target encode value when smaller than threshold (EPSILON)
 */
float roundValue(float& value) {
  int sign = 1;
  if (value != 0.0) {
    sign = value / fabs(value);
  }
  value = EPSILON * sign;
}

/**
 * Get picojson object from JSON file
 *
 * @param path: JSON file path
 * @return picojson::object
 * @throws std::runtime_error if fail to read JSON file or fail to parse
 */
picojson::object readJsonObj(const string& path) {
  ifstream ifs(path, ios::in);
  if (ifs.fail()) {
    throw runtime_error("Failed to read JSON file (" + path + ")");
  }

  istreambuf_iterator<char> it(ifs);
  istreambuf_iterator<char> last;
  const string json_str(it, last);
  ifs.close();

  picojson::value val;
  const string err = picojson::parse(val, json_str);
  if (const string err = picojson::parse(val, json_str); !err.empty()) {
    throw runtime_error("Failed to parse JSON (" + err + ")");
  }

  return val.get<picojson::object>();
}

/**
 * Load layers as picojson array from JSON file
 *
 * @param model_structure_path: JSON file path
 * (/plaintext_experiment/mnist|cifar-10/saved_models/*_structure.json)
 * @return picojson::array
 */
picojson::array loadLayers(const string& model_structure_path) {
  picojson::object json_obj = readJsonObj(model_structure_path);
  // picojson::object config_obj = json_obj["config"].get<picojson::object>();
  // return config_obj["layers"].get<picojson::array>();
  return json_obj["config"].get<picojson::array>();
}

/**
 * Create layer of CNN
 *
 * @param layer: picojson::object
 * @param layer_class_name: class_name of layer
 * @param model_weights_path: path of HDF5 (trained model parameters)
 * @return pointer of Layer object
 * @throws std::runtime_error if layer class name is not found from map
 */
Layer* buildLayer(picojson::object& layer, const string& layer_class_name, const string& model_weights_path) {
  if (auto map_iter = BUILD_LAYER_MAP.find(layer_class_name); map_iter != BUILD_LAYER_MAP.end()) {
    picojson::object layer_info = layer["config"].get<picojson::object>();
    return move(map_iter->second(layer_info, model_weights_path));
  } else {
    throw runtime_error("\"" + layer_class_name + "\" is not registered as layer class");
  }
}

/**
 * Create layer of CNN
 *
 * @param layer: picojson::object
 * @param next_layer: picojson::object
 * @param layer_class_name: class_name of layer
 * @param model_weights_path: path of HDF5 (trained model parameters)
 * @return pointer of Layer object
 * @throws std::runtime_error if layer class name is not found from map
 */
Layer* buildLayer(picojson::object& layer, picojson::object& next_layer, const string& layer_class_name, const string& model_weights_path, picojson::array::const_iterator& layers_iterator) {
  const string next_layer_class_name = next_layer["class_name"].get<string>();

  if (layer_class_name == CONV2D_CLASS_NAME && next_layer_class_name == BATCH_NORMALIZATION_CLASS_NAME) {
    picojson::object conv2d_layer_info = layer["config"].get<picojson::object>();
    picojson::object bn_layer_info     = next_layer["config"].get<picojson::object>();
    layers_iterator++;
    return move(buildConv2DFusedBN(conv2d_layer_info, bn_layer_info, model_weights_path));
  }
  if (layer_class_name == DENSE_CLASS_NAME && next_layer_class_name == BATCH_NORMALIZATION_CLASS_NAME) {
    picojson::object dense_layer_info = layer["config"].get<picojson::object>();
    picojson::object bn_layer_info    = next_layer["config"].get<picojson::object>();
    layers_iterator++;
    return move(buildDenseFusedBN(dense_layer_info, bn_layer_info, model_weights_path));
  }

  return move(buildLayer(layer, layer_class_name, model_weights_path));
}

Layer* buildConv2D(picojson::object& layer_info, const string& model_weights_path) {
  const string layer_name = layer_info["name"].get<string>();
  size_t in_height, in_width, in_channels;
  try {
    const picojson::array batch_input_shape = layer_info["batch_input_shape"].get<picojson::array>();

    in_height   = batch_input_shape[1].get<double>();
    in_width    = batch_input_shape[2].get<double>();
    in_channels = batch_input_shape[3].get<double>();
  } catch (runtime_error& re) {
    in_height   = next_layer_in_height;
    in_width    = next_layer_in_width;
    in_channels = next_layer_in_channels;
  }
  const size_t filter_size        = layer_info["filters"].get<double>();
  const picojson::array filter_hw = layer_info["kernel_size"].get<picojson::array>();
  const size_t filter_height      = filter_hw[0].get<double>();
  const size_t filter_width       = filter_hw[1].get<double>();
  const picojson::array stride_hw = layer_info["strides"].get<picojson::array>();
  const size_t stride_height      = stride_hw[0].get<double>();
  const size_t stride_width       = stride_hw[1].get<double>();
  const string padding            = layer_info["padding"].get<string>();
  const string activation         = layer_info["activation"].get<string>();

  cout << "  Building " << layer_name << "..." << endl;

  H5File param_file(model_weights_path, H5F_ACC_RDONLY);
  Group group       = param_file.openGroup("/" + layer_name + "/" + layer_name);
  DataSet kernel_ds = group.openDataSet(KERNEL_KEY);
  DataSet bias_ds   = group.openDataSet(BIAS_KEY);

  float4D filters(boost::extents[filter_height][filter_width][in_channels][filter_size]);
  Plaintext4D plain_filters(boost::extents[filter_height][filter_width][in_channels][filter_size]);
  vector<float> biases(filter_size);
  vector<Plaintext> plain_biases(filter_size);

  kernel_ds.read(filters.data(), PredType::NATIVE_FLOAT);
  bias_ds.read(biases.data(), PredType::NATIVE_FLOAT);

  float folding_value = 1, weight;
  if (gOption.enable_optimize_activation() && gShouldMultiplyCoeff && gOption.enable_optimize_pooling() && gShouldMultiplyPool) {
    folding_value        = gHighestDegCoeff * gCurrentPoolingMulFactor;
    gShouldMultiplyCoeff = false;
    gShouldMultiplyPool  = false;
  } else if (gOption.enable_optimize_activation() && gShouldMultiplyCoeff) {
    folding_value        = gHighestDegCoeff;
    gShouldMultiplyCoeff = false;
  } else if (gOption.enable_optimize_pooling() && gShouldMultiplyPool) {
    folding_value       = gCurrentPoolingMulFactor;
    gShouldMultiplyPool = false;
  }

#ifdef _OPENMP
#pragma omp parallel for collapse(4) private(weight)
#endif
  for (size_t fh = 0; fh < filter_height; ++fh) {
    for (size_t fw = 0; fw < filter_width; ++fw) {
      for (size_t ic = 0; ic < in_channels; ++ic) {
        for (size_t fs = 0; fs < filter_size; ++fs) {
          weight = folding_value * filters[fh][fw][ic][fs];
          if (fabs(weight) < EPSILON) {
            roundValue(weight);
          }
          gTool.encoder()->encode(weight, gTool.scale_param(), plain_filters[fh][fw][ic][fs]);
          for (size_t lv = 0; lv < gConsumedLevel; ++lv) {
            gTool.evaluator()->mod_switch_to_next_inplace(plain_filters[fh][fw][ic][fs]);
          }
        }
      }
    }
  }

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (size_t fs = 0; fs < filter_size; ++fs) {
    gTool.encoder()->encode(biases[fs], gTool.scale_param(), plain_biases[fs]);
    for (size_t lv = 0; lv < gConsumedLevel + 1; ++lv) {
      gTool.evaluator()->mod_switch_to_next_inplace(plain_biases[fs]);
    }
  }

  Conv2D* conv2d = new Conv2D(layer_name,
                              in_height, in_width, in_channels,
                              filter_size, filter_height, filter_width,
                              stride_height, stride_width,
                              padding, activation,
                              plain_filters, plain_biases);

  next_layer_in_height   = conv2d->out_height();
  next_layer_in_width    = conv2d->out_width();
  next_layer_in_channels = conv2d->out_channels();

  return move((Layer*)conv2d);
}

Layer* buildAveragePooling2D(picojson::object& layer_info, const string& model_weights_path) {
  const string layer_name         = layer_info["name"].get<string>();
  const picojson::array pool_hw   = layer_info["pool_size"].get<picojson::array>();
  const size_t pool_height        = pool_hw[0].get<double>();
  const size_t pool_width         = pool_hw[1].get<double>();
  const picojson::array stride_hw = layer_info["strides"].get<picojson::array>();
  const size_t stride_height      = stride_hw[0].get<double>();
  const size_t stride_width       = stride_hw[1].get<double>();
  const string padding            = layer_info["padding"].get<string>();

  cout << "  Building " << layer_name << "..." << endl;

  Plaintext plain_mul_factor;

  if (gOption.enable_optimize_pooling()) {
    gCurrentPoolingMulFactor = 1.0 / (pool_height * pool_width);
    gShouldMultiplyPool      = true;
  } else if (gOption.enable_optimize_activation() && gShouldMultiplyCoeff) {
    gTool.encoder()->encode(gHighestDegCoeff / (pool_height * pool_width), gTool.scale_param(), plain_mul_factor);
    gShouldMultiplyCoeff = false;
    for (size_t lv = 0; lv < gConsumedLevel; ++lv) {
      gTool.evaluator()->mod_switch_to_next_inplace(plain_mul_factor);
    }
  } else {
    gTool.encoder()->encode(1.0 / (pool_height * pool_width), gTool.scale_param(), plain_mul_factor);
    for (size_t lv = 0; lv < gConsumedLevel; ++lv) {
      gTool.evaluator()->mod_switch_to_next_inplace(plain_mul_factor);
    }
  }

  AveragePooling2D* average_pooling2d = new AveragePooling2D(layer_name,
                                                             next_layer_in_height, next_layer_in_width, next_layer_in_channels,
                                                             pool_height, pool_width,
                                                             stride_height, stride_width,
                                                             padding,
                                                             plain_mul_factor);

  next_layer_in_height   = average_pooling2d->out_height();
  next_layer_in_width    = average_pooling2d->out_width();
  next_layer_in_channels = average_pooling2d->out_channels();

  return move((Layer*)average_pooling2d);
}

Layer* buildBatchNormalization(picojson::object& layer_info, const string& model_weights_path) {
  const string layer_name = layer_info["name"].get<string>();
  // const size_t axis       = layer_info["axis"].get<picojson::array>()[0].get<double>();

  cout << "  Building " << layer_name << "..." << endl;

  H5File param_file(model_weights_path, H5F_ACC_RDONLY);
  Group group                = param_file.openGroup("/" + layer_name + "/" + layer_name);
  DataSet beta_ds            = group.openDataSet(BETA_KEY);
  DataSet gamma_ds           = group.openDataSet(GAMMA_KEY);
  DataSet moving_mean_ds     = group.openDataSet(MOVING_MEAN_KEY);
  DataSet moving_variance_ds = group.openDataSet(MOVING_VARIANCE_KEY);

  const size_t dim = next_layer_in_units != 0
                         ? next_layer_in_units
                         : next_layer_in_channels;

  vector<float> beta(dim), gamma(dim), moving_mean(dim), moving_variance(dim);

  beta_ds.read(beta.data(), PredType::NATIVE_FLOAT);
  gamma_ds.read(gamma.data(), PredType::NATIVE_FLOAT);
  moving_mean_ds.read(moving_mean.data(), PredType::NATIVE_FLOAT);
  moving_variance_ds.read(moving_variance.data(), PredType::NATIVE_FLOAT);

  float weight, bias;
  vector<Plaintext> plain_weights(dim), plain_biases(dim);

#ifdef _OPENMP
#pragma omp parallel for private(weight, bias)
#endif
  for (size_t i = 0; i < dim; ++i) {
    weight = gamma[i] / sqrt(moving_variance[i] + BN_EPSILON);
    bias   = beta[i] - (weight * moving_mean[i]);

    gTool.encoder()->encode(weight, gTool.scale_param(), plain_weights[i]);
    gTool.encoder()->encode(bias, gTool.scale_param(), plain_biases[i]);
    for (size_t lv = 0; lv < gConsumedLevel; ++lv) {
      gTool.evaluator()->mod_switch_to_next_inplace(plain_weights[i]);
      gTool.evaluator()->mod_switch_to_next_inplace(plain_biases[i]);
    }
    gTool.evaluator()->mod_switch_to_next_inplace(plain_biases[i]);
  }

  return move((Layer*)new BatchNormalization(layer_name, plain_weights, plain_biases));
}

Layer* buildFlatten(picojson::object& layer_info, const string& model_weights_path) {
  const string layer_name = layer_info["name"].get<string>();
  next_layer_in_units     = next_layer_in_height * next_layer_in_width * next_layer_in_channels;

  cout << "  Building " << layer_name << "..." << endl;

  return move((Layer*)new Flatten(layer_name,
                                  next_layer_in_height, next_layer_in_width, next_layer_in_channels,
                                  next_layer_in_units));
}

Layer* buildDense(picojson::object& layer_info, const string& model_weights_path) {
  const string layer_name = layer_info["name"].get<string>();
  const size_t out_units  = layer_info["units"].get<double>();
  const string activation = layer_info["activation"].get<string>();

  cout << "  Building " << layer_name << "..." << endl;

  H5File param_file(model_weights_path, H5F_ACC_RDONLY);
  Group group       = param_file.openGroup("/" + layer_name + "/" + layer_name);
  DataSet kernel_ds = group.openDataSet(KERNEL_KEY);
  DataSet bias_ds   = group.openDataSet(BIAS_KEY);

  float2D weights(boost::extents[next_layer_in_units][out_units]);
  Plaintext2D plain_weights(boost::extents[next_layer_in_units][out_units]);
  vector<float> biases(out_units);
  vector<Plaintext> plain_biases(out_units);

  kernel_ds.read(weights.data(), PredType::NATIVE_FLOAT);
  bias_ds.read(biases.data(), PredType::NATIVE_FLOAT);

  float folding_value = 1, weight;
  if (gOption.enable_optimize_activation() && gShouldMultiplyCoeff && gOption.enable_optimize_pooling() && gShouldMultiplyPool) {
    folding_value        = gHighestDegCoeff * gCurrentPoolingMulFactor;
    gShouldMultiplyCoeff = false;
    gShouldMultiplyPool  = false;
  } else if (gOption.enable_optimize_activation() && gShouldMultiplyCoeff) {
    folding_value        = gHighestDegCoeff;
    gShouldMultiplyCoeff = false;
  } else if (gOption.enable_optimize_pooling() && gShouldMultiplyPool) {
    folding_value       = gCurrentPoolingMulFactor;
    gShouldMultiplyPool = false;
  }

#ifdef _OPENMP
#pragma omp parallel for collapse(2) private(weight)
#endif
  for (size_t iu = 0; iu < next_layer_in_units; ++iu) {
    for (size_t ou = 0; ou < out_units; ++ou) {
      weight = folding_value * weights[iu][ou];
      if (fabs(weight) < EPSILON) {
        roundValue(weight);
      }
      gTool.encoder()->encode(weight, gTool.scale_param(), plain_weights[iu][ou]);
      for (size_t lv = 0; lv < gConsumedLevel; ++lv) {
        gTool.evaluator()->mod_switch_to_next_inplace(plain_weights[iu][ou]);
      }
    }
  }

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (size_t ou = 0; ou < out_units; ++ou) {
    gTool.encoder()->encode(biases[ou], gTool.scale_param(), plain_biases[ou]);
    for (size_t lv = 0; lv < gConsumedLevel + 1; ++lv) {
      gTool.evaluator()->mod_switch_to_next_inplace(plain_biases[ou]);
    }
  }

  Dense* dense = new Dense(layer_name,
                           next_layer_in_units, out_units,
                           activation,
                           plain_weights, plain_biases);

  next_layer_in_units = out_units;

  return move((Layer*)dense);
}

Layer* buildActivation(picojson::object& layer_info, const string& model_weights_path) {
  const string layer_name = layer_info["name"].get<string>();
  const string activation = layer_info["activation"].get<string>();

  cout << "  Building " << layer_name << "..." << endl;

  if (gOption.enable_optimize_activation()) {
    gShouldMultiplyCoeff = true;
  }

  return move((Layer*)new Activation(layer_name, activation));
}

Layer* buildGlobalAveragePooling2D(picojson::object& layer_info, const string& model_weights_path) {
  const string layer_name = layer_info["name"].get<string>();
  next_layer_in_units     = next_layer_in_channels;

  cout << "  Building " << layer_name << "..." << endl;

  Plaintext plain_mul_factor;

  // if (gOption.enable_optimize_pooling()) {
  if (true || gOption.enable_optimize_pooling()) {
    if (gShouldMultiplyPool) {
      gCurrentPoolingMulFactor *= (1.0 / (next_layer_in_height * next_layer_in_width));
    } else {
      gCurrentPoolingMulFactor = 1.0 / (next_layer_in_height * next_layer_in_width);
    }
    gShouldMultiplyPool = true;
  } else {
    gTool.encoder()->encode(1.0 / (next_layer_in_height * next_layer_in_width), gTool.scale_param(), plain_mul_factor);
    for (size_t lv = 0; lv < gConsumedLevel; ++lv) {
      gTool.evaluator()->mod_switch_to_next_inplace(plain_mul_factor);
    }
  }

  return move((Layer*)new GlobalAveragePooling2D(layer_name,
                                                 next_layer_in_height, next_layer_in_width, next_layer_in_channels,
                                                 next_layer_in_units,
                                                 plain_mul_factor));
}

Layer* buildConv2DFusedBN(picojson::object& conv2d_layer_info, picojson::object& bn_layer_info, const string& model_weights_path) {
  // read variables of conv2d
  const string conv2d_layer_name = conv2d_layer_info["name"].get<string>();
  size_t in_height, in_width, in_channels;
  try {
    const picojson::array batch_input_shape = conv2d_layer_info["batch_input_shape"].get<picojson::array>();

    in_height   = batch_input_shape[1].get<double>();
    in_width    = batch_input_shape[2].get<double>();
    in_channels = batch_input_shape[3].get<double>();
  } catch (runtime_error& re) {
    in_height   = next_layer_in_height;
    in_width    = next_layer_in_width;
    in_channels = next_layer_in_channels;
  }
  const size_t filter_size        = conv2d_layer_info["filters"].get<double>();
  const picojson::array filter_hw = conv2d_layer_info["kernel_size"].get<picojson::array>();
  const size_t filter_height      = filter_hw[0].get<double>();
  const size_t filter_width       = filter_hw[1].get<double>();
  const picojson::array stride_hw = conv2d_layer_info["strides"].get<picojson::array>();
  const size_t stride_height      = stride_hw[0].get<double>();
  const size_t stride_width       = stride_hw[1].get<double>();
  const string padding            = conv2d_layer_info["padding"].get<string>();
  const string activation         = conv2d_layer_info["activation"].get<string>();

  // read variables of batch normalization
  const string bn_layer_name = bn_layer_info["name"].get<string>();

  const string layer_name = conv2d_layer_name + "-fused-with-" + bn_layer_name;

  cout << "  Building " << layer_name << "..." << endl;

  H5File param_file(model_weights_path, H5F_ACC_RDONLY);
  Group conv2d_group = param_file.openGroup("/" + conv2d_layer_name + "/" + conv2d_layer_name);
  Group bn_group     = param_file.openGroup("/" + bn_layer_name + "/" + bn_layer_name);

  DataSet kernel_ds          = conv2d_group.openDataSet(KERNEL_KEY);
  DataSet bias_ds            = conv2d_group.openDataSet(BIAS_KEY);
  DataSet beta_ds            = bn_group.openDataSet(BETA_KEY);
  DataSet gamma_ds           = bn_group.openDataSet(GAMMA_KEY);
  DataSet moving_mean_ds     = bn_group.openDataSet(MOVING_MEAN_KEY);
  DataSet moving_variance_ds = bn_group.openDataSet(MOVING_VARIANCE_KEY);

  float4D filters(boost::extents[filter_height][filter_width][in_channels][filter_size]);
  Plaintext4D plain_filters(boost::extents[filter_height][filter_width][in_channels][filter_size]);
  vector<float> biases(filter_size),
      beta(filter_size), gamma(filter_size), moving_mean(filter_size), moving_variance(filter_size),
      weights_bn(filter_size), biases_bn(filter_size);
  vector<Plaintext> plain_biases(filter_size);

  kernel_ds.read(filters.data(), PredType::NATIVE_FLOAT);
  bias_ds.read(biases.data(), PredType::NATIVE_FLOAT);
  beta_ds.read(beta.data(), PredType::NATIVE_FLOAT);
  gamma_ds.read(gamma.data(), PredType::NATIVE_FLOAT);
  moving_mean_ds.read(moving_mean.data(), PredType::NATIVE_FLOAT);
  moving_variance_ds.read(moving_variance.data(), PredType::NATIVE_FLOAT);

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (size_t fs = 0; fs < filter_size; ++fs) {
    weights_bn[fs] = gamma[fs] / sqrt(moving_variance[fs] + BN_EPSILON);
    biases_bn[fs]  = beta[fs] - (weights_bn[fs] * moving_mean[fs]);
    biases[fs]     = biases[fs] * weights_bn[fs] + biases_bn[fs];
    gTool.encoder()->encode(biases[fs], gTool.scale_param(), plain_biases[fs]);
    for (size_t lv = 0; lv < gConsumedLevel + 1; ++lv) {
      gTool.evaluator()->mod_switch_to_next_inplace(plain_biases[fs]);
    }
  }

  float folding_value = 1, weight;
  if (gOption.enable_optimize_activation() && gShouldMultiplyCoeff && gOption.enable_optimize_pooling() && gShouldMultiplyPool) {
    folding_value        = gHighestDegCoeff * gCurrentPoolingMulFactor;
    gShouldMultiplyCoeff = false;
    gShouldMultiplyPool  = false;
  } else if (gOption.enable_optimize_activation() && gShouldMultiplyCoeff) {
    folding_value        = gHighestDegCoeff;
    gShouldMultiplyCoeff = false;
  } else if (gOption.enable_optimize_pooling() && gShouldMultiplyPool) {
    folding_value       = gCurrentPoolingMulFactor;
    gShouldMultiplyPool = false;
  }

#ifdef _OPENMP
#pragma omp parallel for collapse(4) private(weight)
#endif
  for (size_t fh = 0; fh < filter_height; ++fh) {
    for (size_t fw = 0; fw < filter_width; ++fw) {
      for (size_t ic = 0; ic < in_channels; ++ic) {
        for (size_t fs = 0; fs < filter_size; ++fs) {
          weight = folding_value * filters[fh][fw][ic][fs] * weights_bn[fs];
          if (fabs(weight) < EPSILON) {
            roundValue(weight);
          }
          gTool.encoder()->encode(weight, gTool.scale_param(), plain_filters[fh][fw][ic][fs]);
          for (size_t lv = 0; lv < gConsumedLevel; ++lv) {
            gTool.evaluator()->mod_switch_to_next_inplace(plain_filters[fh][fw][ic][fs]);
          }
        }
      }
    }
  }

  Conv2DFusedBN* conv2d_fused_bn = new Conv2DFusedBN(layer_name,
                                                     in_height, in_width, in_channels,
                                                     filter_size, filter_height, filter_width,
                                                     stride_height, stride_width,
                                                     padding, activation,
                                                     plain_filters, plain_biases);

  next_layer_in_height   = conv2d_fused_bn->out_height();
  next_layer_in_width    = conv2d_fused_bn->out_width();
  next_layer_in_channels = conv2d_fused_bn->out_channels();

  return move((Layer*)conv2d_fused_bn);
}

Layer* buildDenseFusedBN(picojson::object& dense_layer_info, picojson::object& bn_layer_info, const string& model_weights_path) {
  // read variables of dense
  const string dense_layer_name = dense_layer_info["name"].get<string>();
  const size_t out_units        = dense_layer_info["units"].get<double>();
  const string activation       = dense_layer_info["activation"].get<string>();

  // read variables of batch normalization
  const string bn_layer_name = bn_layer_info["name"].get<string>();

  const string layer_name = dense_layer_name + "-fused-with-" + bn_layer_name;

  cout << "  Building " << layer_name << "..." << endl;

  H5File param_file(model_weights_path, H5F_ACC_RDONLY);
  Group dense_group = param_file.openGroup("/" + dense_layer_name + "/" + layer_name);
  Group bn_group    = param_file.openGroup("/" + bn_layer_name + "/" + bn_layer_name);

  DataSet kernel_ds          = dense_group.openDataSet(KERNEL_KEY);
  DataSet bias_ds            = dense_group.openDataSet(BIAS_KEY);
  DataSet beta_ds            = bn_group.openDataSet(BETA_KEY);
  DataSet gamma_ds           = bn_group.openDataSet(GAMMA_KEY);
  DataSet moving_mean_ds     = bn_group.openDataSet(MOVING_MEAN_KEY);
  DataSet moving_variance_ds = bn_group.openDataSet(MOVING_VARIANCE_KEY);

  float2D weights(boost::extents[next_layer_in_units][out_units]);
  Plaintext2D plain_weights(boost::extents[next_layer_in_units][out_units]);
  vector<float> biases(out_units),
      beta(out_units), gamma(out_units), moving_mean(out_units), moving_variance(out_units),
      weights_bn(out_units), biases_bn(out_units);
  vector<Plaintext> plain_biases(out_units);

  kernel_ds.read(weights.data(), PredType::NATIVE_FLOAT);
  bias_ds.read(biases.data(), PredType::NATIVE_FLOAT);
  beta_ds.read(beta.data(), PredType::NATIVE_FLOAT);
  gamma_ds.read(gamma.data(), PredType::NATIVE_FLOAT);
  moving_mean_ds.read(moving_mean.data(), PredType::NATIVE_FLOAT);
  moving_variance_ds.read(moving_variance.data(), PredType::NATIVE_FLOAT);

  for (size_t ou = 0; ou < out_units; ++ou) {
    weights_bn[ou] = gamma[ou] / sqrt(moving_variance[ou] + BN_EPSILON);
    biases_bn[ou]  = beta[ou] - (weights_bn[ou] * moving_mean[ou]);
    biases[ou]     = biases[ou] * weights_bn[ou] + biases_bn[ou];
    gTool.encoder()->encode(biases[ou], gTool.scale_param(), plain_biases[ou]);
    for (size_t lv = 0; lv < gConsumedLevel + 1; ++lv) {
      gTool.evaluator()->mod_switch_to_next_inplace(plain_biases[ou]);
    }
  }

  float folding_value = 1, weight;
  if (gOption.enable_optimize_activation() && gShouldMultiplyCoeff && gOption.enable_optimize_pooling() && gShouldMultiplyPool) {
    folding_value        = gHighestDegCoeff * gCurrentPoolingMulFactor;
    gShouldMultiplyCoeff = false;
    gShouldMultiplyPool  = false;
  } else if (gOption.enable_optimize_activation() && gShouldMultiplyCoeff) {
    folding_value        = gHighestDegCoeff;
    gShouldMultiplyCoeff = false;
  } else if (gOption.enable_optimize_pooling() && gShouldMultiplyPool) {
    folding_value       = gCurrentPoolingMulFactor;
    gShouldMultiplyPool = false;
  }

#ifdef _OPENMP
#pragma omp parallel for collapse(2) private(weight)
#endif
  for (size_t iu = 0; iu < next_layer_in_units; ++iu) {
    for (size_t ou = 0; ou < out_units; ++ou) {
      weight = folding_value * weights[iu][ou] * weights_bn[ou];
      if (fabs(weight) < EPSILON) {
        roundValue(weight);
      }
      gTool.encoder()->encode(weight, gTool.scale_param(), plain_weights[iu][ou]);
      for (size_t lv = 0; lv < gConsumedLevel; ++lv) {
        gTool.evaluator()->mod_switch_to_next_inplace(plain_weights[iu][ou]);
      }
    }
  }

  DenseFusedBN* dense_fused_bn = new DenseFusedBN(layer_name,
                                                  next_layer_in_units, out_units,
                                                  activation,
                                                  plain_weights, plain_biases);

  next_layer_in_units = out_units;

  return move((Layer*)dense_fused_bn);
}
