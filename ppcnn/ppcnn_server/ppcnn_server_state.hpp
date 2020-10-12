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

#ifndef PPCNN_SERVER_STATE_HPP
#define PPCNN_SERVER_STATE_HPP

#include <cstdbool>
#include <memory>

#include <stdsc/stdsc_state.hpp>

namespace ppcnn_server
{

/**
 * @brief Enumeration for state.
 */
enum StateId_t : int32_t
{
    kStateNil = 0,
    kStateReady = 1,
    kStateExit = 2,
};

/**
 * @brief Enumeration for events.
 */
enum Event_t : uint64_t
{
    kEventNil = 0,
    kEventQuery = 1,
    kEventResultRequest = 2,
};

/**
 * @brief Provides 'Ready' state.
 */
struct StateReady : public stdsc::State
{
    static std::shared_ptr<State> create();
    StateReady(void);
    virtual void set(stdsc::StateContext& sc, uint64_t event) override;
    STDSC_STATE_DEFID(kStateReady);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace ppcnn_server */

#endif /* PPCNN_SERVER_STATE_HPP */
