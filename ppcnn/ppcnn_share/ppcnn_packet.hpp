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

#ifndef PPCNN_PACKET_HPP
#define PPCNN_PACKET_HPP

#include <cstdint>

namespace ppcnn_share
{

/**
 * @brief Enumeration for control code of packet.
 */
enum ControlCode_t : uint64_t
{
    /* Code for Request packet: 0x201-0x2FF */

    /* Code for Data packet: 0x401-0x4FF */
    kControlCodeDataEncKeys = 0x401,
    kControlCodeDataParam = 0x402,
    kControlCodeDataQueryID = 0x403,
    kControlCodeDataResult = 0x404,

    /* Code for Download packet: 0x801-0x8FF */

    /* Code for UpDownload packet: 0x1000-0x10FF */
    kControlCodeUpDownloadQuery = 0x1001,
    kControlCodeUpDownloadResult = 0x1002,
};

} /* namespace ppcnn_share */

#endif /* PPCNN_PACKET_HPP */
