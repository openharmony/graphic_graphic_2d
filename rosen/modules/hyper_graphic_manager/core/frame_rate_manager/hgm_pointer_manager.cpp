/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "hgm_pointer_manager.h"

#include <chrono>
#include <map>
#include <set>

#include "hgm_core.h"
#include "hgm_multi_app_strategy.h"
#include "hgm_task_handle_thread.h"


namespace OHOS::Rosen {
namespace {
constexpr int32_t UP_TIMEOUT_MS = 3000;
constexpr int32_t RS_IDLE_TIMEOUT_MS = 1200;
}

HgmPointerManager::HgmPointerManager() : HgmStateMachine<PointerState, PointerEvent>(PointerState::POINTER_IDLE_STATE),
    activeTimeoutTimer_("up_timeout_timer", std::chrono::milliseconds(UP_TIMEOUT_MS), nullptr, [this]() {
        OnEvent(PointerEvent::POINTER_ACTIVE_TIMEOUT_EVENT);
    }),
    rsIdleTimeoutTimer_("rs_idle_timeout_timer", std::chrono::milliseconds(RS_IDLE_TIMEOUT_MS), nullptr, [this]() {
        OnEvent(PointerEvent::POINTER_RS_IDLE_TIMEOUT_EVENT);
    })
{
    RegisterEventCallback(PointerEvent::POINTER_ACTIVE_EVENT, [this](PointerEvent event) {
        ChangeState(PointerState::POINTER_ACTIVE_STATE);
    });
    RegisterEventCallback(PointerEvent::POINTER_ACTIVE_TIMEOUT_EVENT, [this](PointerEvent event) {
        ChangeState(PointerState::POINTER_IDLE_STATE);
    });
    RegisterEventCallback(PointerEvent::POINTER_RS_IDLE_TIMEOUT_EVENT, [this](PointerEvent event) {
        ChangeState(PointerState::POINTER_IDLE_STATE);
    });

    // register state callback
    RegisterEnterStateCallback(PointerState::POINTER_ACTIVE_STATE,
        [this](PointerState lastState, PointerState newState) {
            activeTimeoutTimer_.Start();
            rsIdleTimeoutTimer_.Start();
            pointerInfo_ = { GetPkgName(), newState, OLED_120_HZ };
            Vote();
        });
    RegisterEnterStateCallback(PointerState::POINTER_IDLE_STATE,
        [this](PointerState lastState, PointerState newState) {
            pointerInfo_ = { GetPkgName(), newState };
            activeTimeoutTimer_.Stop();
            rsIdleTimeoutTimer_.Stop();
            Vote();
        });
}

void HgmPointerManager::Vote()
{
    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    if (frameRateMgr == nullptr) {
        return;
    }
    HgmMultiAppStrategy& multiAppStrategy = frameRateMgr->GetMultiAppStrategy();
    std::vector<std::string> pkgs = multiAppStrategy.GetPackages();
    if (pointerInfo_.pkgName == "" && !pkgs.empty()) {
        auto [focusPkgName, pid, appType] = multiAppStrategy.AnalyzePkgParam(pkgs.front());
        pointerInfo_.pkgName = focusPkgName;
        HGM_LOGD("auto change pointer pkgName to focusPkgName:%{public}s", focusPkgName.c_str());
    }
    UpdateStrategyByPointer();
}

void HgmPointerManager::UpdateStrategyByPointer()
{
    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    if (frameRateMgr == nullptr) {
        return;
    }
    HgmMultiAppStrategy& multiAppStrategy = frameRateMgr->GetMultiAppStrategy();

    PolicyConfigData::StrategyConfig settingStrategy;
    if (multiAppStrategy.GetFocusAppStrategyConfig(settingStrategy) == EXEC_SUCCESS &&
        pointerInfo_.pointerState == PointerState::POINTER_ACTIVE_STATE &&
        settingStrategy.pointerMode == PointerModeType::POINTER_DISENABLED) {
        return;
    }

    if (pointerInfo_.pointerState == PointerState::POINTER_IDLE_STATE) {
        frameRateMgr->HandleRefreshRateEvent(DEFAULT_PID, {"VOTER_POINTER", false});
        return;
    }

    if (multiAppStrategy.GetFocusAppStrategyConfig(settingStrategy) != EXEC_SUCCESS ||
        settingStrategy.pointerMode == PointerModeType::POINTER_DISENABLED) {
        frameRateMgr->HandleRefreshRateEvent(DEFAULT_PID, {"VOTER_POINTER", false});
        return;
    }
    frameRateMgr->HandleRefreshRateEvent(DEFAULT_PID,
        {"VOTER_POINTER", true, settingStrategy.down, settingStrategy.down});
}

void HgmPointerManager::HandlePointerEvent(PointerEvent event, const std::string& pkgName)
{
    pkgName_ = pkgName;
    OnEvent(event);
}

void HgmPointerManager::HandleTimerReset()
{
    activeTimeoutTimer_.Reset();
    rsIdleTimeoutTimer_.Reset();
}

void HgmPointerManager::HandleRsFrame()
{
    rsIdleTimeoutTimer_.Reset();
}

std::string HgmPointerManager::State2String(State state) const
{
    static std::map<PointerState, std::string> stateStringMap = {
        { PointerState::POINTER_ACTIVE_STATE, "PointerActive" },
        { PointerState::POINTER_IDLE_STATE, "PointerIdle" },
    };
    if (auto iter = stateStringMap.find(state); iter != stateStringMap.end()) {
        return iter->second;
    }
    return std::to_string(state);
}

bool HgmPointerManager::CheckChangeStateValid(PointerState lastState, PointerState newState)
{
    static std::map<PointerState, std::set<PointerState>> stateChangeMap = {
        { PointerState::POINTER_ACTIVE_STATE, { PointerState::POINTER_IDLE_STATE } },
        { PointerState::POINTER_IDLE_STATE, { PointerState::POINTER_ACTIVE_STATE } },
    };
    if (auto iter = stateChangeMap.find(lastState); iter != stateChangeMap.end()) {
        return iter->second.find(newState) != iter->second.end();
    }
    return false;
}
} // OHOS::Rosen