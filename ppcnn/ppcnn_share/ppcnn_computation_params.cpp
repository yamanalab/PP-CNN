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

#include <ppcnn_share/ppcnn_computation_params.hpp>

namespace ppcnn_share
{

std::ostream& operator<<(std::ostream& os, const ComputationParams& params)
{
    os << params.img_width << std::endl;
    os << params.img_height << std::endl;
    os << params.img_channels << std::endl;
    os << params.labels << std::endl;
    os << std::string(params.dataset) << std::endl;
    os << std::string(params.model) << std::endl;
    os << params.opt_level << std::endl;
    os << params.activation << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, ComputationParams& params)
{
    is >> params.img_width;
    is >> params.img_height;
    is >> params.img_channels;
    is >> params.labels;
    std::string dataset, model;
    is >> dataset;
    is >> model;
    is >> params.opt_level;
    is >> params.activation;
    dataset.copy(params.dataset, dataset.size());
    dataset.copy(params.model, model.size());
    return is;
}

} /* namespace ppcnn_share */
