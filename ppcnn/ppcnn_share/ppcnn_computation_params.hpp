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

#ifndef PPCNN_COMPUTATION_PARAMS_HPP
#define PPCNN_COMPUTATION_PARAMS_HPP

#include <sstream>
#include <string>

namespace ppcnn_share
{

/**
 * @brief This class is used to hold computation params
 */
struct ComputationParams
{
    size_t img_width;
    size_t img_height;
    size_t img_channels;
    size_t labels;
    char dataset[1024];
    char model[1024];
    int32_t opt_level;
    int32_t activation;

    std::string to_string() const
    {
        std::ostringstream oss;
        oss << img_width << ", " << img_height << ", " << img_channels << ", "
            << labels << ", " << std::string(dataset) << ", "
            << std::string(model) << ", " << opt_level << ", " << activation;
        return oss.str();
    }
};

std::ostream& operator<<(std::ostream& os, const ComputationParams& params);
std::istream& operator>>(std::istream& is, ComputationParams& params);

} /* namespace ppcnn_share */

#endif /* PPCNN_COMPUTATION_PARAMS_HPP */
