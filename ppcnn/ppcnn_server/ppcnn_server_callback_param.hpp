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

#ifndef PPCNN_SERVER_CALLBACK_PARAM_HPP
#define PPCNN_SERVER_CALLBACK_PARAM_HPP

#include <memory>

namespace ppcnn_server
{

class CalcManager;
class KeyContainer;

/**
 * @brief This class is used to hold the callback parameters for Server.
 */
struct CallbackParam
{
    CallbackParam(void);
    virtual ~CallbackParam(void) = default;
};

/**
 * @brief This class is used to hold the callback parameters for Server.
 * This parameter to shared on all connections.
 */
struct CommonCallbackParam
{
    CommonCallbackParam(CalcManager& calc_manager, KeyContainer& key_container)
      : calc_manager_(calc_manager), key_container_(key_container)
    {
    }
    virtual ~CommonCallbackParam(void) = default;
    CalcManager& calc_manager_;
    KeyContainer& key_container_;
};

} /* namespace ppcnn_server */

#endif /* PPCNN_SERVER_CALLBACK_PARAM_HPP */
