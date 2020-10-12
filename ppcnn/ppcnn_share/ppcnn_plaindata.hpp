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

#ifndef PPCNN_PLAINDATA_HPP
#define PPCNN_PLAINDATA_HPP

#include <memory>

#include <ppcnn_share/ppcnn_basicdata.hpp>

namespace ppcnn_share
{

/**
 * @brief This clas is used to hold the plain data.
 */
template <class T>
struct PlainData : public ppcnn_share::BasicData<T>
{
    using super = ppcnn_share::BasicData<T>;

    PlainData() = default;
    virtual ~PlainData(void) = default;

    virtual size_t save(std::ostream& os) const override
    {
        if (super::vec_.size() == 0)
        {
            return -1;
        }

        size_t sz = super::vec_.size();
        os.write(reinterpret_cast<char*>(&sz), sizeof(sz));

        for (const auto& v : super::vec_)
        {
            os.write((char*)(&v), sizeof(v));
        }

        return 0;
    }
    virtual size_t load(std::istream& is) override
    {
        size_t sz;
        is.read(reinterpret_cast<char*>(&sz), sizeof(sz));

        super::clear();

        for (size_t i = 0; i < sz; ++i)
        {
            T v;
            is.read((char*)(&v), sizeof(v));
            super::vec_.push_back(v);
        }

        return 0;
    }
};

} /* namespace ppcnn_share */

#endif /* PPCNN_PLAINDATA_HPP */
