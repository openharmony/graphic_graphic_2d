/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HGM_STATE_MACHINE_H
#define HGM_STATE_MACHINE_H

#include <atomic>
#include <functional>
#include <memory>
#include <unordered_map>

#include "hgm_log.h"

namespace OHOS::Rosen {
template<typename StateType, typename EventType>
class HgmStateMachine {
public:
    using State = StateType;
    using Event = EventType;

    using EventCallback = std::function<void(Event)>;
    // lastState, newState
    using StateChangeCallback = std::function<void(State, State)>;
    // state, {callbackId, stateChangeCallback}
    using StateChangeCallbacksType = std::unordered_map<State, std::unordered_map<int32_t, StateChangeCallback>>;

    explicit HgmStateMachine(State state) : state_(state) {}
    virtual ~HgmStateMachine() = default;

    State GetState() { return state_.load(); }
    void ChangeState(State state);
    void OnEvent(Event event);

    int32_t RegisterEnterStateCallback(State state, const StateChangeCallback& callback);
    void UnRegisterEnterStateCallback(State state, int32_t callbackId);
    int32_t RegisterExitStateCallback(State state, const StateChangeCallback& callback);
    void UnRegisterExitStateCallback(State state, int32_t callbackId);

    void RegisterEventCallback(Event event, const EventCallback& callback);
    void UnRegisterEventCallback(Event event);

protected:
    virtual bool CheckChangeStateValid(State lastState, State newState) { return true; }
    // callback should be run in same thread
    virtual void ExecuteCallback(const std::function<void()>& callback)
    {
        if (callback != nullptr) {
            callback();
        }
    }

private:
    // return callbackId
    int32_t RegisterStateChangeCallback(
        StateChangeCallbacksType& callbacks, State state, const StateChangeCallback& callback);
    void UnRegisterStateChangeCallback(StateChangeCallbacksType& callbacks, State state, int32_t callbackId);

    std::atomic<State> state_;
    StateChangeCallbacksType enterStateCallbacks_;
    StateChangeCallbacksType exitStateCallbacks_;
    std::atomic<int32_t> stateCallbackId_ = 0;

    std::unordered_map<Event, EventCallback> eventCallbacks_;
};

template<typename State, typename Event>
void HgmStateMachine<State, Event>::ChangeState(State state)
{
    ExecuteCallback([this, state = state]() {
        auto lastState = state_.load();
        if (lastState == state) {
            return;
        }
        if (!CheckChangeStateValid(lastState, state)) {
            return;
        }

        // exit state callback
        for (auto &[id, callback] : exitStateCallbacks_[lastState]) {
            if (callback != nullptr) {
                callback(lastState, state);
            }
        }

        // change state
        HGM_LOGI("StateMachine state change: %{public}d -> %{public}d", lastState, state);
        state_.store(state);

        // enter state callback
        for (auto &[id, callback] : enterStateCallbacks_[state]) {
            if (callback != nullptr) {
                callback(lastState, state);
            }
        }
    });
}

template<typename State, typename Event>
void HgmStateMachine<State, Event>::OnEvent(Event event)
{
    if (auto iter = eventCallbacks_.find(event); iter != eventCallbacks_.end()) {
        if (iter->second != nullptr) {
            ExecuteCallback([callback = iter->second, event = event] () { callback(event); });
        }
    }
}

template<typename State, typename Event>
int32_t HgmStateMachine<State, Event>::RegisterEnterStateCallback(State state, const StateChangeCallback& callback)
{
    return RegisterStateChangeCallback(enterStateCallbacks_, state, callback);
}

template<typename State, typename Event>
void HgmStateMachine<State, Event>::UnRegisterEnterStateCallback(State state, int32_t callbackId)
{
    return UnRegisterStateChangeCallback(enterStateCallbacks_, state, callbackId);
}

template<typename State, typename Event>
int32_t HgmStateMachine<State, Event>::RegisterExitStateCallback(State state, const StateChangeCallback& callback)
{
    return RegisterStateChangeCallback(exitStateCallbacks_, state, callback);
}

template<typename State, typename Event>
void HgmStateMachine<State, Event>::UnRegisterExitStateCallback(State state, int32_t callbackId)
{
    return UnRegisterStateChangeCallback(exitStateCallbacks_, state, callbackId);
}

template<typename State, typename Event>
void HgmStateMachine<State, Event>::RegisterEventCallback(Event event, const EventCallback& callback)
{
    eventCallbacks_[event] = callback;
}

template<typename State, typename Event>
void HgmStateMachine<State, Event>::UnRegisterEventCallback(Event event)
{
    if (auto iter = eventCallbacks_.find(event); iter != eventCallbacks_.end()) {
        eventCallbacks_.erase(iter);
    }
}

template<typename State, typename Event>
int32_t HgmStateMachine<State, Event>::RegisterStateChangeCallback(
    StateChangeCallbacksType& callbacks, State state, const StateChangeCallback& callback)
{
    stateCallbackId_++;
    callbacks[state][stateCallbackId_.load()] = callback;
    return stateCallbackId_;
}

template<typename State, typename Event>
void HgmStateMachine<State, Event>::UnRegisterStateChangeCallback(
    StateChangeCallbacksType& callbacks, State state, int32_t callbackId)
{
    if (auto iter = callbacks.find(state); iter != callbacks.end()) {
        if (auto toDelCallbackIter = iter->second; toDelCallbackIter != iter->second.end()) {
            iter->second.erase(toDelCallbackIter);
        }
    }
}
}
#endif // HGM_STATE_MACHINE_H