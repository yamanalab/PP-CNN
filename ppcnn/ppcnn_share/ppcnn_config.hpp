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

#ifndef PPCNN_CONFIG_HPP
#define PPCNN_CONFIG_HPP

#include <memory>
#include <string>

namespace ppcnn_share
{

/**
 * @brief This class is used to hold the configuration data.
 */
struct Config
{
    Config(void);
    ~Config(void) = default;

    std::string get_value(const std::string& key) const;
    bool is_exist_key(const std::string& key) const;
    void load_from_file(const std::string& filename);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

template <class T>
T config_get_value(const Config& config, const std::string& key);

} /* namespace ppcnn_share */

#endif /* PPCNN_CONFIG_HPP */
