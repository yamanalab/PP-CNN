#include <algorithm>
#include <boost/multi_array.hpp>
#include <boost/program_options.hpp>
#include <exception>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>

#include "cnn/network.hpp"
#include "cnn/network_builder.hpp"
#include "utils/globals.hpp"
#include "utils/helper_functions.hpp"
#include "utils/load_dataset.hpp"

using namespace std;
namespace fs = experimental::filesystem;
namespace po = boost::program_options;

const string MNIST_NAME                = "mnist";
const string CIFAR10_NAME              = "cifar-10";
const string PLAINTEXT_EXPERIMENT_PATH = "../../plaintext_experiment/";
const string LOG_PATH                  = "../logs/";

constexpr size_t MNIST_CHANNELS   = 1;
constexpr size_t MNIST_HEIGHT     = 28;
constexpr size_t MNIST_WIDTH      = 28;
constexpr size_t MNIST_LABELS     = 10;
constexpr size_t CIFAR10_CHANNELS = 3;
constexpr size_t CIFAR10_HEIGHT   = 32;
constexpr size_t CIFAR10_WIDTH    = 32;
constexpr size_t CIFAR10_LABELS   = 10;

/* Setup program options specified by command line args */
inline void setupOptions(po::variables_map& options, const int& argc, char** argv) {
  po::options_description option_description("option");
  // clang-format off
  option_description.add_options()
    ("help,h", "Display help")
    ("dataset,D", po::value<string>(), "Dataset name (mnist|cifar-10)")
    ("model,M", po::value<string>(), "Trained model name")
    ("optimize,O", po::value<size_t>()->default_value(0), "Optimization level (0: no opt, 1: fusing Convolution & Batch Normalization, 2: reduction level of polynomial activation function, 3: reduction level of average pooling, 4: all 1 & 2 & 3 opts)")
    ("limit,L", po::value<size_t>()->default_value(0), "Limit number of test images")
    ("trials,N", po::value<size_t>()->default_value(1), "Number of prediction trials for time measurement")
    ("activation,A", po::value<string>(), "Activation function name (square|swish_rg4_deg4|swish_rg6_deg4")
    ("log", po::value<string>()->default_value("main_log.txt"), "Logfile name")
    ;
  // clang-format on
  po::store(po::parse_command_line(argc, argv, option_description), options);
  po::notify(options);

  if (options.count("help")) {
    cout << option_description << endl;
    exit(EXIT_SUCCESS);
  } else if (!options.count("dataset") || !options.count("model")) {
    cerr << "\'--dataset\' or \'--model\' option are missing" << endl;
    cerr << option_description << endl;
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char** argv) {
  po::variables_map options;
  try {
    setupOptions(options, argc, argv);
  } catch (const boost::program_options::error_with_option_name& poe) {
    cerr << poe.what() << endl;
    return EXIT_FAILURE;
  }

  string dataset_name, trained_model_name, activation, log_file_name;
  size_t test_image_limit, number_prediction_trials;
  EOptLevel optimization_level;
  try {
    dataset_name             = options["dataset"].as<string>();
    trained_model_name       = options["model"].as<string>();
    test_image_limit         = options["limit"].as<size_t>();
    number_prediction_trials = options["trials"].as<size_t>();
    log_file_name            = options["log"].as<string>();
    optimization_level       = static_cast<EOptLevel>(options["optimize"].as<size_t>());
    if (options.count("activation")) {
      activation = options["activation"].as<string>();
    }
  } catch (const boost::bad_any_cast& bac) {
    cerr << bac.what() << endl;
    return EXIT_FAILURE;
  }
  const string base_model_path      = PLAINTEXT_EXPERIMENT_PATH + dataset_name + "/saved_models/" + trained_model_name;
  const string model_structure_path = base_model_path + "_structure.json";
  const string model_weights_path   = base_model_path + "_weights.h5";
  const string log_file_path        = LOG_PATH + log_file_name;

  /* Check options */
  if (dataset_name != MNIST_NAME && dataset_name != CIFAR10_NAME) {
    cerr << "Dataset name is incorrect!\n"
         << "Please specify 'mnist' of 'cifar-10' for dataset.\n"
         << endl;
    return EXIT_FAILURE;
  }
  if (optimization_level < 0 || 4 < optimization_level) {
    cerr << "Optimization level is incorrect!\n"
         << "Please specify 0 ~ 4 for optimization level.\n"
         << endl;
    return EXIT_FAILURE;
  }
  if (!fs::exists(model_structure_path)) {
    cerr << "Model structure path is not found: " << model_structure_path << endl;
    return EXIT_FAILURE;
  }
  if (!fs::exists(model_weights_path)) {
    cerr << "Model weights path is not found: " << model_weights_path << endl;
    return EXIT_FAILURE;
  }
  if (!fs::exists(log_file_path)) {
    cerr << "Log file path is not found: " << log_file_path << endl;
    return EXIT_FAILURE;
  }
#ifdef __LOG__
  // Open log file
  ofstream log_file;
  log_file.open(log_file_path, ios::app);
#endif
#ifdef __DEBUG__
  // Open debug file
  ofstream debug_file;
  debug_file.open(DEBUG_FILE_PATH, ios::app);
#endif

  /* Setup optimization option */
  setupOptimizationOption(optimization_level);
  // Initialize global variables
  gConsumedLevel       = 0;
  gShouldMultiplyCoeff = false;
  gShouldMultiplyPool  = false;
  if (activation == "square") {
    gActivation = SQUARE;
  } else if (activation == "swish_rg4_deg4") {
    gActivation = SWISH_RG4_DEG4;
  } else if (activation == "swish_rg6_deg4") {
    gActivation = SWISH_RG6_DEG4;
  } else {
    gActivation = DEFAULT;
  }
  if (gOption.enable_optimize_activation()) {
    if (trained_model_name.find("CKKS-swish_rg4_deg4") != string::npos || gActivation == SWISH_RG4_DEG4) {
      gHighestDegCoeff = SWISH_RG4_DEG4_COEFFS.front();
    } else if (trained_model_name.find("CKKS-swish_rg6_deg4") != string::npos || gActivation == SWISH_RG6_DEG4) {
      gHighestDegCoeff = SWISH_RG6_DEG4_COEFFS.front();
    }
  }

  /* Load test dataset for prediction */
  vector<vector<float>> test_images;
  vector<unsigned char> test_labels;
  size_t channels, rows, cols, labels;

  cout << "Loading test images & labels..." << endl;
  if (dataset_name == MNIST_NAME) {
    test_images = loadMnistTestImages(test_image_limit);
    test_labels = loadMnistTestLabels(test_image_limit);
    channels = MNIST_CHANNELS, rows = MNIST_HEIGHT, cols = MNIST_WIDTH, labels = MNIST_LABELS;
  } else {
    test_images = loadCifar10TestImages(test_image_limit);
    test_labels = loadCifar10TestLabels(test_image_limit);
    channels = CIFAR10_CHANNELS, rows = CIFAR10_HEIGHT, cols = CIFAR10_WIDTH, labels = CIFAR10_LABELS;
  }

  cout << "Finish loading!" << endl;

  const size_t test_image_count = test_images.size();
  cout << "Number of images for test: " << test_image_count << endl;
  cout << endl;

  /* Setup SEAL Tool */
  setupSealTool();
  cout << "Number of slots: " << gTool.slot_count() << endl;

  const size_t step_count = test_image_count / gTool.slot_count() + 1;
  cout << "Number of steps: " << step_count << endl;
  cout << endl;

  /* Build network based on trained model (from json & h5) */
  NetworkBuilder::setup(model_structure_path, model_weights_path);
  Network network;
  cout << "Building network from trained model..." << endl;
#ifdef __LOG__
  log_file << "\tBuilding network from trained model..." << endl;
#endif
#ifdef __DEBUG__
  debug_file << "Building network fron trained model..." << endl;
#endif
  auto build_network_begin_point = chrono::high_resolution_clock::now();
  try {
    network = NetworkBuilder::buildNetwork();
  } catch (const runtime_error& re) {
    cerr << "Error has occurred in building network." << endl;
    cerr << "RuntimeError: " << re.what() << endl;
    return EXIT_FAILURE;
  } catch (const exception& e) {
    cerr << "Error has occurred in building network." << endl;
    cerr << "Exception: " << e.what() << endl;
    return EXIT_FAILURE;
  }
  auto build_network_end_point = chrono::high_resolution_clock::now();

  chrono::duration<double> build_network_sec = build_network_end_point - build_network_begin_point;
  cout << "Finish building! (" << build_network_sec.count() << " sec)\n"
       << endl;
  network.printStructure();
  cout << endl;
#ifdef __LOG__
  log_file << "\tFinish building! (" << build_network_sec.count() << " sec)\n"
           << endl;
#endif
#ifdef __DEBUG__
  debug_file << "Finish building! (" << build_network_sec.count() << " sec)\n"
             << endl;
#endif

  /* Make predictions on encrypted images */
  size_t remain_image_count = test_image_count;
  vector<Ciphertext> encrypted_results(labels);
  vector<Plaintext> plain_results(labels);
  vector<vector<double>> results(test_image_count, vector<double>(labels));
  vector<double> tmp_results(gTool.slot_count());

  cout << "Start to make predictions on encrypted images\n"
       << endl;
#ifdef __LOG__
  log_file << "\tStart to make predictions on encrypted images\n"
           << endl;
#endif
  chrono::duration<double> sum_encryption_trials_sec, sum_prediction_trials_sec, sum_decryption_trials_sec;
  auto whole_process_begin_point = chrono::high_resolution_clock::now();
  for (size_t step = 0, image_count_in_step; step < step_count; ++step) {
    sum_encryption_trials_sec = chrono::duration<double>::zero();
    sum_prediction_trials_sec = chrono::duration<double>::zero();
    sum_decryption_trials_sec = chrono::duration<double>::zero();
    cout << "Step " << step + 1 << ":\n"
         << "\t--------------------------------------------------" << endl;
#ifdef __LOG__
    log_file << "\tStep " << step + 1 << ":\n"
             << "\t--------------------------------------------------" << endl;
#endif
    if (gTool.slot_count() < remain_image_count) {
      image_count_in_step = gTool.slot_count();
    } else {
      image_count_in_step = remain_image_count;
    }
    const size_t begin_idx = step * gTool.slot_count();
    const size_t end_idx   = begin_idx + image_count_in_step;

    for (size_t n = 0; n < number_prediction_trials; ++n) {
      /* Encrypt images in step */
      Ciphertext3D encrypted_packed_images(boost::extents[rows][cols][channels]);  // Keras default setting is channels_last ([N,H,W,C])
      cout << "\t<Trial " << n + 1 << ">\n"
           << "\tEncrypting " << image_count_in_step << " images..." << endl;
#ifdef __LOG__
      log_file << "\t<Trial " << n + 1 << ">\n"
               << "\tEncrypting " << image_count_in_step << " images..." << endl;
#endif
      auto step_encrypt_images_begin_point = chrono::high_resolution_clock::now();
      encryptImages(test_images, encrypted_packed_images, begin_idx, end_idx);
      auto step_encrypt_images_end_point = chrono::high_resolution_clock::now();

      chrono::duration<double> step_encrypt_images_sec = step_encrypt_images_end_point - step_encrypt_images_begin_point;
      sum_encryption_trials_sec += step_encrypt_images_sec;
      cout << "\tFinish encrypting! (" << step_encrypt_images_sec.count() << " sec)\n"
           << "\t  encrypted packed images shape: "
           << encrypted_packed_images.shape()[0] << "x"
           << encrypted_packed_images.shape()[1] << "x"
           << encrypted_packed_images.shape()[2] << "\n"
           << endl;
#ifdef __LOG__
      log_file << "\tFinish encrypting! (" << step_encrypt_images_sec.count() << " sec)\n"
               << "\t  encrypted packed images shape: "
               << encrypted_packed_images.shape()[0] << "x"
               << encrypted_packed_images.shape()[1] << "x"
               << encrypted_packed_images.shape()[2] << "\n"
               << endl;
#endif

      /* Execute encrypted prediction */
      cout << "\tPredicting..." << endl;
#ifdef __LOG__
      log_file << "\tPredicting..." << endl;
#endif
      auto step_predict_begin_point = chrono::high_resolution_clock::now();
      try {
        encrypted_results = network.predict(encrypted_packed_images);
      } catch (const InvalidDowncastException& ide) {
        cerr << "Error has occurred during prediction." << endl;
        cerr << "InvalidDowncastException: " << ide.what() << endl;
        return EXIT_FAILURE;
      } catch (const runtime_error& re) {
        cerr << "Error has occurred during prediction." << endl;
        cerr << "RuntimeError: " << re.what() << endl;
        return EXIT_FAILURE;
      } catch (const exception& e) {
        cerr << "Error has occurred during prediction." << endl;
        cerr << "Exception: " << e.what() << endl;
        return EXIT_FAILURE;
      }
      auto step_predict_end_point = chrono::high_resolution_clock::now();

      chrono::duration<double> step_predict_sec = step_predict_end_point - step_predict_begin_point;
      sum_prediction_trials_sec += step_predict_sec;
      cout << "\tFinish predicting! (" << step_predict_sec.count() << " sec)\n"
           << "\t  encrypted results size: " << encrypted_results.size() << "\n"
           << endl;
#ifdef __LOG__
      log_file << "\tFinish predicting! (" << step_predict_sec.count() << " sec)\n"
               << "\t  encrypted results size: " << encrypted_results.size() << "\n"
               << endl;
#endif

      /* Decrypt encrypted results of prediction */
      cout << "\tDecrypting prediction results..." << endl;
#ifdef __LOG__
      log_file << "\tDecrypting prediction results..." << endl;
#endif
      auto step_decrypt_results_begin_point = chrono::high_resolution_clock::now();
      for (size_t label = 0; label < labels; ++label) {
        gTool.decryptor()->decrypt(encrypted_results[label], plain_results[label]);
        gTool.encoder()->decode(plain_results[label], tmp_results);

        for (size_t image_idx = begin_idx, counter = 0; image_idx < end_idx; ++image_idx) {
          results[image_idx][label] = tmp_results[counter++];
        }
      }
      auto step_decrypt_results_end_point = chrono::high_resolution_clock::now();

      chrono::duration<double> step_decrypt_results_sec = step_decrypt_results_end_point - step_decrypt_results_begin_point;
      sum_decryption_trials_sec += step_decrypt_results_sec;
      cout << "\tFinish decrypting! (" << step_decrypt_results_sec.count() << " sec)\n"
           << "\t--------------------------------------------------" << endl;
#ifdef __LOG__
      log_file << "\tFinish decrypting! (" << step_decrypt_results_sec.count() << " sec)\n"
               << "\t--------------------------------------------------" << endl;
#endif
    }
    chrono::duration<double> average_encryption_trials_sec = sum_encryption_trials_sec / number_prediction_trials;
    chrono::duration<double> average_prediction_trials_sec = sum_prediction_trials_sec / number_prediction_trials;
    chrono::duration<double> average_decryption_trials_sec = sum_decryption_trials_sec / number_prediction_trials;
    cout << "\n\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
         << "\tAverage of " << number_prediction_trials << " encryption trials: " << average_encryption_trials_sec.count() << " sec\n"
         << "\tAverage of " << number_prediction_trials << " prediction trials: " << average_prediction_trials_sec.count() << " sec\n"
         << "\tAverage of " << number_prediction_trials << " decryption trials: " << average_decryption_trials_sec.count() << " sec\n"
         << "\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
         << endl;
#ifdef __LOG__
    log_file << "\n\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
             << "\tAverage of " << number_prediction_trials << " encryption trials: " << average_encryption_trials_sec.count() << " sec\n"
             << "\tAverage of " << number_prediction_trials << " prediction trials: " << average_prediction_trials_sec.count() << " sec\n"
             << "\tAverage of " << number_prediction_trials << " decryption trials: " << average_decryption_trials_sec.count() << " sec\n"
             << "\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
             << endl;
#endif

    remain_image_count -= image_count_in_step;
  }
  auto whole_process_end_point = chrono::high_resolution_clock::now();

  chrono::duration<double> average_process_sec = (whole_process_end_point - whole_process_begin_point) / number_prediction_trials;
  cout << "Finish predictions for all test images! (Average of " << number_prediction_trials << " trials: " << average_process_sec.count() << " sec)\n"
       << endl;
#ifdef __LOG__
  log_file << "\tFinish predictions for all test images! (Average of " << number_prediction_trials << " trials: " << average_process_sec.count() << " sec)\n"
           << endl;
#endif

  /* For prediction on single encrypted image */
  // for (size_t i = 0; i < test_image_count; ++i) {
  //   Ciphertext3D encrypted_image(boost::extents[rows][cols][channels]);
  //   cout << "Encrypting image [" << i << "]..." << endl;
  //   encryptImage(test_images[i], encrypted_image);
  //   cout << "Finish encrypting!\n"
  //        << endl;

  //   cout << "Predicting [" << i << "]..." << endl;
  //   try {
  //     result = network.predict(encrypted_image);
  //   } catch (const InvalidDowncastException& ide) {
  //     cerr << "Error has occurred during prediction." << endl;
  //     cerr << "InvalidDowncastException: " << ide.what() << endl;
  //     return EXIT_FAILURE;
  //   } catch (const runtime_error& re) {
  //     cerr << "Error has occurred during prediction." << endl;
  //     cerr << "RuntimeError: " << re.what() << endl;
  //     return EXIT_FAILURE;
  //   } catch (const exception& e) {
  //     cerr << "Error has occurred during prediction." << endl;
  //     cerr << "Exception: " << e.what() << endl;
  //     return EXIT_FAILURE;
  //   }
  //   cout << "Finish predicting!\n"
  //        << endl;
  // }

  /* Calculate accuracy */
  vector<double>::iterator begin_iter, max_iter;
  size_t predicted_label, correct_prediction_count = 0;

  cout << "Calculating accuracy..." << endl;
#ifdef __LOG__
  log_file << "\tCalculating accuracy..." << endl;
#endif
  for (size_t image_idx = 0; image_idx < test_image_count; ++image_idx) {
    begin_iter      = results[image_idx].begin();
    max_iter        = max_element(begin_iter, results[image_idx].end());
    predicted_label = distance(begin_iter, max_iter);

    if (predicted_label == static_cast<size_t>(test_labels[image_idx])) {
      correct_prediction_count++;
    }
#ifdef __DEBUG__
    debug_file << "image[" << image_idx << "]:" << endl;
    debug_file << "\t";
    for (size_t label = 0; label < labels; ++label) {
      debug_file << "[" << label << "]: " << results[image_idx][label] << ", ";
    }
    debug_file << endl;
#endif
  }
  const double accuracy = static_cast<double>(correct_prediction_count) / test_image_count;
  cout << "Finish calculating!\n"
       << endl;
  cout << "Accuracy: " << accuracy << "\n"
       << endl;
#ifdef __LOG__
  log_file << "\tFinish calculating!\n"
           << endl;
  log_file << "\tAccuracy: " << accuracy << endl;
#endif

  return 0;
}
