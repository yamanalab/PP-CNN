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

#include <dirent.h>   // scandir
#include <string.h>   // strcmp
#include <sys/stat.h> // stat
#include <algorithm> // std::all_of
#include <cctype>    // isdigit
#include <cstdlib>   // std::env, std::rand
#include <cstdlib>   // std::env
#include <ctime>     // std::time
#include <fstream>
#include <sstream>

#include <stdsc/stdsc_exception.hpp>

#include <ppcnn_share/ppcnn_utility.hpp>

namespace ppcnn_share
{

namespace utility
{

bool file_exist(const std::string& filename)
{
    std::ifstream ifs(filename, std::ios::binary);
    return ifs.is_open();
}

bool dir_exist(const std::string& dirname)
{
    struct stat stat_dir;
    return stat(dirname.c_str(), &stat_dir) == 0;
}

size_t file_size(const std::string& filename)
{
    size_t size = -1;
    if (!file_exist(filename))
    {
        std::ostringstream ss;
        ss << "failed to open " << filename;
        STDSC_THROW_FILE(ss.str());
    }
    else
    {
        std::ifstream ifs(filename, std::ios::binary);
        ifs.seekg(0, std::fstream::end);
        size_t epos = ifs.tellg();

        ifs.clear();

        ifs.seekg(0, std::fstream::beg);
        size_t bpos = ifs.tellg();

        size = epos - bpos;
    }
    return size;
}

bool remove_file(const std::string& filename)
{
    bool ret = false;
    if (file_exist(filename))
    {
        ret = (std::remove(filename.c_str()) == 0) ? true : false;
    }
    return ret;
}

std::string basename(const std::string& filepath)
{
    std::string filename = filepath;
    filename.erase(filename.begin(),
                   filename.begin() + filename.find_last_of("/") + 1);
    return filename;
}

bool isdigit(const std::string& str)
{
    return !str.empty() && std::all_of(str.cbegin(), str.cend(), ::isdigit);
}

std::string getenv(const char* env_var)
{
    std::string env;
    if (const char* env_p = std::getenv(env_var))
    {
        env = env_p;
    }
    return env;
}

void split(const std::string& str, const std::string& delims,
           std::vector<std::string>& vec_str)
{
    std::string::size_type index = str.find_first_not_of(delims);
    std::string::size_type str_size = str.size();
    vec_str.clear();
    while ((std::string::npos != index) && (0 <= str_size && index < str_size))
    {
        std::string::size_type next_index = str.find_first_of(delims, index);
        if ((std::string::npos == next_index) || (next_index > str_size))
        {
            next_index = str_size;
        }
        vec_str.push_back(str.substr(index, next_index - index));
        index = str.find_first_not_of(delims, next_index + 1);
    }
}

int32_t gen_uuid(void)
{
    std::srand(std::time(nullptr));
    return std::rand();
}

std::string trim_string(const std::string& str, const std::string& whitespace)
{
    const auto bgn = str.find_first_not_of(whitespace);
    if (bgn == std::string::npos)
    {
        return "";
    }

    const auto end = str.find_last_not_of(whitespace);
    const auto len = end - bgn + 1;

    return str.substr(bgn, len);
}

std::vector<std::string> get_filelist(const std::string& dir,
                                      const std::string& ext)
{
    std::vector<std::string> flist;

    struct stat stat_buf;
    struct dirent** namelist = nullptr;

    auto n = ::scandir(dir.c_str(), &namelist, NULL, NULL);
    for (int i = 0; i < n; ++i)
    {
        if (::strcmp(namelist[i]->d_name, ".\0") &&
            ::strcmp(namelist[i]->d_name, "..\0"))
        {
            auto fullpath = dir + "/" + std::string(namelist[i]->d_name);
            if (::stat(fullpath.c_str(), &stat_buf) == 0 &&
                (stat_buf.st_mode & S_IFMT) != S_IFDIR)
            {
                auto extname = get_extname(fullpath);
                if (ext.empty() || ext == extname)
                {
                    flist.push_back(fullpath);
                }
            }
        }
    }

    return flist;
}

std::string get_filename(const std::string& path, const bool without_ext)
{
    int endpos =
      without_ext ? path.length() - (path.find_last_of('.') + 1) : -1;
    return path.substr(path.find_last_of('/') + 1, endpos);
}

std::string get_dirname(const std::string& path)
{
    return path.substr(0, path.find_last_of('/') + 1);
}

std::string get_extname(const std::string& path)
{
    auto filename = get_filename(path);
    return filename.substr(filename.find_last_of('.') + 1);
}

} /* namespace utility */

} /* namespace ppcnn_share */
