#include "globals.hpp"

/***********************
 * Swish approximation
 ***********************/
// ax^4 + bx^2 + cx + d
std::vector<float> SWISH_RG4_DEG4_COEFFS = { -0.005075, 0.19566, 0.5, 0.03347 };
std::vector<float> SWISH_RG6_DEG4_COEFFS = { -0.002012, 0.1473, 0.5, 0.1198 };
// x^4 + b'x^2 + c'x + d'
std::vector<float> SWISH_RG4_DEG4_OPT_COEFFS = { -38.5537, -98.52222, -6.59507 };
std::vector<float> SWISH_RG6_DEG4_OPT_COEFFS = { -73.2107, -248.5089, -59.5427 };

/***********************
 * Mish approximation
 ***********************/
// ax^4 + bx^3 + cx^2 + dx + e
std::vector<float> MISH_RG4_DEG4_COEFFS = { -0.00609, -0.004142, 0.21051, 0.565775, 0.06021 };
std::vector<float> MISH_RG6_DEG4_COEFFS = { -0.002096, -0.001277, 0.148529, 0.53663, 0.169 };
// x^4 + b'x^3 + c'x^2 + d'x + e'
std::vector<float> MISH_RG4_DEG4_OPT_COEFFS = { 0.68013, -34.5665, -92.9023, -9.8867 };
std::vector<float> MISH_RG6_DEG4_OPT_COEFFS = { 0.60926, -70.86307, -256.02576, -80.62977 };

OptOption gOption;
SealTool gTool;
std::size_t gConsumedLevel;
float gHighestDegCoeff;
float gCurrentPoolingMulFactor;
bool gShouldMultiplyCoeff;
bool gShouldMultiplyPool;
EActivation gActivation;

#ifdef __DEBUG__
std::string DEBUG_FILE_PATH = "../logs/debug.txt";
#endif
