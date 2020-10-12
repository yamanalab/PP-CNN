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

#ifndef PPCNN_UTILITY_HPP
#define PPCNN_UTILITY_HPP

#include <string>
#include <vector>

namespace ppcnn_share
{

namespace utility
{

bool file_exist(const std::string& filename);
bool dir_exist(const std::string& dirname);
size_t file_size(const std::string& filename);
bool remove_file(const std::string& filename);
std::string basename(const std::string& filepath);
bool isdigit(const std::string& str);
std::string getenv(const char* env_var);
void split(const std::string& str, const std::string& delims,
           std::vector<std::string>& vec_str);
int32_t gen_uuid(void);
std::string trim_string(const std::string& str,
                        const std::string& whitespace = " \t");
std::vector<std::string> get_filelist(const std::string& dir,
                                      const std::string& ext = "");
std::string get_filename(const std::string& path,
                         const bool without_ext = false);
std::string get_dirname(const std::string& path);
std::string get_extname(const std::string& path);

} /* namespace utility */

} /* namespace ppcnn_share */

#endif /* PPCNN_UTILITY_HPP */
