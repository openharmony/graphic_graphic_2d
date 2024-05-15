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

#include <chrono>
#include <map>
#include <set>

#include "hgm_task_handle_thread.h"
#include "hgm_touch_manager.h"

namespace OHOS::Rosen {
namespace {
    constexpr int32_t UP_TIMEOUT_MS = 3000;
    constexpr int32_t RS_IDLE_TIMEOUT_MS = 600;
}

HgmTouchManager::HgmTouchManager() : HgmStateMachine<TouchState, TouchEvent>(TouchState::IDLE_STATE),
    upTimeoutTimer_("up_timeout_timer", std::chrono::milliseconds(UP_TIMEOUT_MS), nullptr, [this] () {
        ChangeState(TouchState::IDLE_STATE);
    }),
    rsIdleTimeoutTimer_("rs_idle_timeout_timer", std::chrono::milliseconds(RS_IDLE_TIMEOUT_MS), nullptr, [this] () {
        ChangeState(TouchState::IDLE_STATE);
    })
{
    // register event callback
    RegisterEventCallback(TouchEvent::DOWN_EVENT, [this] (TouchEvent event) {
        ChangeState(TouchState::DOWN_STATE);
    });
    RegisterEventCallback(TouchEvent::UP_EVENT, [this] (TouchEvent event) {
        ChangeState(TouchState::UP_STATE);
    });

    // register state callback
    RegisterEnterStateCallback(TouchState::UP_STATE, [this] (TouchState lastState, TouchState newState) {
        upTimeoutTimer_.Start();
        rsIdleTimeoutTimer_.Start();
    });
    RegisterExitStateCallback(TouchState::UP_STATE, [this] (TouchState lastState, TouchState newState) {
        upTimeoutTimer_.Stop();
        rsIdleTimeoutTimer_.Stop();
    });
}

void HgmTouchManager::HandleTouchEvent(TouchEvent event, const std::string& pkgName)
{
    pkgName_ = pkgName;
    OnEvent(event);
}

void HgmTouchManager::HandleRsFrame()
{
    rsIdleTimeoutTimer_.Reset();
}

void HgmTouchManager::HandleThirdFrameIdle()
{
    ChangeState(TouchState::IDLE_STATE);
}

bool HgmTouchManager::CheckChangeStateValid(TouchState lastState, TouchState newState)
{
    static std::map<TouchState, std::set<TouchState>> stateChangeMap = {
        { TouchState::DOWN_STATE, { TouchState::UP_STATE } },
        { TouchState::UP_STATE, { TouchState::DOWN_STATE, TouchState::IDLE_STATE } },
        { TouchState::IDLE_STATE, { TouchState::DOWN_STATE } },
    };
    if (auto iter = stateChangeMap.find(lastState); iter != stateChangeMap.end()) {
        return iter->second.find(newState) != iter->second.end();
    }
    return false;
}

void HgmTouchManager::ExecuteCallback(const std::function<void()>& callback)
{
    if (callback != nullptr) {
        HgmTaskHandleThread::Instance().PostTask(callback);
    }
}
} // OHOS::Rosen