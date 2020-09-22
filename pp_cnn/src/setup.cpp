#include <boost/program_options.hpp>
#include <cmath>
#include <experimental/filesystem>
#include <fstream>
#include "seal/seal.h"
#include "utils/globals.hpp"
#include "utils/timer.hpp"

using namespace std;
using namespace seal;
namespace fs = experimental::filesystem;
namespace po = boost::program_options;

mytimer::Timer timer = mytimer::Timer();

const string SECRETS_DIR      = "../secrets/";
const string PARAMS_FILE_PATH = SECRETS_DIR + "params.bin";
const string PK_FILE_PATH     = SECRETS_DIR + "pk.bin";
const string SK_FILE_PATH     = SECRETS_DIR + "sk.bin";
const string RK_FILE_PATH     = SECRETS_DIR + "rk.bin";
const string GK_FILE_PATH     = SECRETS_DIR + "gk.bin";

inline void saveParams(const EncryptionParameters& params);
inline void savePublicKey(const PublicKey& public_key);
inline void saveSecretKey(const SecretKey& secret_key);
inline void saveRelinKeys(const RelinKeys& relin_keys);
inline void print_parameters(std::shared_ptr<seal::SEALContext> context);

/* Setup program options specified by command line args */
inline void setupOptions(po::variables_map& options, const int& argc, char** argv) {
  po::options_description option_description("option");
  // clang-format off
  option_description.add_options()
    ("help,h", "Display help")
    ("power,P", po::value<size_t>(), "power of polynomial modulus degree (ex. 13, 14, 15)")
    ("level,L", po::value<size_t>(), "required multiplicative level")
    ;
  // clang-format on
  po::store(po::parse_command_line(argc, argv, option_description), options);
  po::notify(options);

  if (options.count("help")) {
    cout << option_description << endl;
    exit(EXIT_SUCCESS);
  } else if (!options.count("power") || !options.count("level")) {
    cerr << "\'--power\' or \'--level\' option are missing" << endl;
    cerr << option_description << endl;
    exit(EXIT_FAILURE);
  }
}

/* Setup SEAL settings for FHE */
int main(int argc, char** argv) {
  po::variables_map options;
  try {
    setupOptions(options, argc, argv);
  } catch (const boost::program_options::error_with_option_name& poe) {
    cerr << poe.what() << endl;
    return EXIT_FAILURE;
  }

  size_t power;
  size_t multiplicative_level;
  try {
    power                = options["power"].as<size_t>();
    multiplicative_level = options["level"].as<size_t>();  // TODO: Automatically determine from network structure
  } catch (const boost::bad_any_cast& bac) {
    cerr << bac.what() << endl;
    return EXIT_FAILURE;
  }
  const size_t poly_modulus_degree = static_cast<size_t>(pow(2.0, power));

  cout << "Start SEAL setup for FHE" << endl;
  timer.set();

  EncryptionParameters params(scheme_type::CKKS);
  params.set_poly_modulus_degree(poly_modulus_degree);
  // Define bit sizes of primes
  // ex) [60, 40, ..., 40, 60] (size of intermediate elements -> multiplicative level)
  vector<int> bit_sizes(multiplicative_level, INTERMEDIATE_PRIMES_BIT_SIZE);
  bit_sizes.push_back(PRE_SUF_PRIME_BIT_SIZE);
  bit_sizes.insert(bit_sizes.begin(), PRE_SUF_PRIME_BIT_SIZE);
  params.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, bit_sizes));

  auto context = SEALContext::Create(params);
  print_parameters(context);
  cout << endl;

  KeyGenerator keygen(context);
  PublicKey public_key = keygen.public_key();
  SecretKey secret_key = keygen.secret_key();
  RelinKeys relin_keys = keygen.relin_keys();

  timer.print("Finish setup for FHE:");
  cout << endl;

  if (!fs::exists(SECRETS_DIR)) fs::create_directory(SECRETS_DIR);
  cout << "Saving the params & generaged keys to " << SECRETS_DIR << endl;

  saveParams(params);
  savePublicKey(public_key);
  saveSecretKey(secret_key);
  saveRelinKeys(relin_keys);

  cout << "Finish saving!" << endl;

  return EXIT_SUCCESS;
}

inline void saveParams(const EncryptionParameters& params) {
  ofstream params_ofs(PARAMS_FILE_PATH, ios::binary);
  params.save(params_ofs);
  params_ofs.close();
}

inline void savePublicKey(const PublicKey& public_key) {
  ofstream pk_ofs(PK_FILE_PATH, ios::binary);
  public_key.save(pk_ofs);
  pk_ofs.close();
}

inline void saveSecretKey(const SecretKey& secret_key) {
  ofstream sk_ofs(SK_FILE_PATH, ios::binary);
  secret_key.save(sk_ofs);
  sk_ofs.close();
}

inline void saveRelinKeys(const RelinKeys& relin_keys) {
  ofstream rk_ofs(RK_FILE_PATH, ios::binary);
  relin_keys.save(rk_ofs);
  rk_ofs.close();
}

/*
Helper function: Prints the parameters in a SEALContext.
*/
inline void print_parameters(std::shared_ptr<seal::SEALContext> context) {
  // Verify parameters
  if (!context) {
    throw std::invalid_argument("context is not set");
  }
  auto& context_data = *context->key_context_data();

  /*
    Which scheme are we using?
    */
  std::string scheme_name;
  switch (context_data.parms().scheme()) {
    case seal::scheme_type::BFV:
      scheme_name = "BFV";
      break;
    case seal::scheme_type::CKKS:
      scheme_name = "CKKS";
      break;
    default:
      throw std::invalid_argument("unsupported scheme");
  }
  std::cout << "/" << std::endl;
  std::cout << "| Encryption parameters :" << std::endl;
  std::cout << "|   scheme: " << scheme_name << std::endl;
  std::cout << "|   poly_modulus_degree: " << context_data.parms().poly_modulus_degree() << std::endl;

  /*
    Print the size of the true (product) coefficient modulus.
    */
  std::cout << "|   coeff_modulus size: ";
  std::cout << context_data.total_coeff_modulus_bit_count() << " (";
  auto coeff_modulus          = context_data.parms().coeff_modulus();
  std::size_t coeff_mod_count = coeff_modulus.size();
  for (std::size_t i = 0; i < coeff_mod_count - 1; i++) {
    std::cout << coeff_modulus[i].bit_count() << " + ";
  }
  std::cout << coeff_modulus.back().bit_count();
  std::cout << ") bits" << std::endl;

  /*
    For the BFV scheme print the plain_modulus parameter.
    */
  if (context_data.parms().scheme() == seal::scheme_type::BFV) {
    std::cout << "|   plain_modulus: " << context_data.parms().plain_modulus().value() << std::endl;
  }

  std::cout << "\\" << std::endl;
}
