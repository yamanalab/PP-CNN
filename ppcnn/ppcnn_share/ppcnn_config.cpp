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

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <stdsc/stdsc_exception.hpp>

#include <ppcnn_share/ppcnn_config.hpp>
#include <ppcnn_share/ppcnn_utility.hpp>

namespace ppcnn_share
{

#define THROW_IF_KEY_DOESNT_EXIST(key)                         \
    do                                                         \
    {                                                          \
        if (!config.is_exist_key(key))                         \
        {                                                      \
            std::ostringstream oss;                            \
            oss << "Err: Invalid key string. (" << key << ")"; \
            STDSC_THROW_FILE(oss.str());                       \
        }                                                      \
    } while (0)

struct Config::Impl
{
    Impl() = default;
    ~Impl() = default;

    std::string get_value(const std::string& key) const
    {
        return map_.at(key);
    }

    bool is_exist_key(const std::string& key) const
    {
        return map_.count(key) != 0;
    }

    void load_from_file(const std::string& filename)
    {
        std::ifstream ifs(filename);
        if (!ifs)
        {
            std::ostringstream oss;
            oss << "Err: Config file not found. (" << filename << ")";
            STDSC_THROW_FILE(oss.str());
        }

        ifs.peek();
        std::streamsize size = ifs.rdbuf()->in_avail();
        if (0 == size)
        {
            std::ostringstream oss;
            oss << "Err: Config file is empty. (" << filename << ")";
            STDSC_THROW_FILE(oss.str());
        }

        size_t line_num = 0;
        std::string line;

        while (std::getline(ifs, line))
        {

            line_num++;

            // Delete comments
            std::string::size_type pos = line.find("#");
            if (std::string::npos != pos)
            {
                line = line.substr(0, pos);
            }

            std::vector<std::string> vec_str;
            utility::split(line, " ,=\t\r\n", vec_str);

            if (vec_str.size() >= 2)
            {
                map_.emplace(vec_str[0], vec_str[1]);
            }
        }
    }

private:
    std::unordered_map<std::string, std::string> map_;
};

Config::Config(void) : pimpl_(new Impl())
{
}

std::string Config::get_value(const std::string& key) const
{
    return pimpl_->get_value(key);
}

bool Config::is_exist_key(const std::string& key) const
{
    return pimpl_->is_exist_key(key);
}

void Config::load_from_file(const std::string& filename)
{
    pimpl_->load_from_file(filename);
}

template <>
std::string config_get_value<std::string>(const Config& config,
                                          const std::string& key)
{
    THROW_IF_KEY_DOESNT_EXIST(key);
    return config.get_value(key);
}

template <>
double config_get_value<double>(const Config& config, const std::string& key)
{
    THROW_IF_KEY_DOESNT_EXIST(key);
    return std::stod(config.get_value(key));
}

template <>
float config_get_value<float>(const Config& config, const std::string& key)
{
    THROW_IF_KEY_DOESNT_EXIST(key);
    return std::stof(config.get_value(key));
}

template <>
int config_get_value<int>(const Config& config, const std::string& key)
{
    THROW_IF_KEY_DOESNT_EXIST(key);
    return std::stoi(config.get_value(key));
}

template <>
long config_get_value<long>(const Config& config, const std::string& key)
{
    THROW_IF_KEY_DOESNT_EXIST(key);
    return std::stol(config.get_value(key));
}

template <>
long long config_get_value<long long>(const Config& config,
                                      const std::string& key)
{
    THROW_IF_KEY_DOESNT_EXIST(key);
    return std::stoll(config.get_value(key));
}

template <>
unsigned long config_get_value<unsigned long>(const Config& config,
                                              const std::string& key)
{
    THROW_IF_KEY_DOESNT_EXIST(key);
    return std::stoul(config.get_value(key));
}

template <>
unsigned long long config_get_value<unsigned long long>(const Config& config,
                                                        const std::string& key)
{
    THROW_IF_KEY_DOESNT_EXIST(key);
    return std::stoull(config.get_value(key));
}

} /* namespace ppcnn_share */
