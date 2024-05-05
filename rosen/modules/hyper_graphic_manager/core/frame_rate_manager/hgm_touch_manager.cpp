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

#include "hgm_touch_manager.h"

#include "common/rs_optional_trace.h"
#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "hgm_log.h"

namespace OHOS {
namespace Rosen {
void TouchStateMachine::TouchEventHandle(TouchEvent event, const std::string& pkgName)
{
    pkgName_ = pkgName;
    switch (event) {
        case TouchEvent::DOWN:
            TouchDown();
            break;
        case TouchEvent::UP:
            TouchUp();
            break;
        case TouchEvent::THREE_SECONDS_TIMEOUT:
            TouchUpTimeout();
            break;
        case TouchEvent::RS_TIMEOUT:
            RSIdleTimeout();
            break;
        default:
            break;
    }
}

void TouchStateMachine::TouchDown()
{
    auto& hgmCore = HgmCore::Instance();
    StopTouchTimer(hgmCore.GetFrameRateMgr()->GetCurScreenId());
    std::lock_guard<std::mutex> lock(touchStateMutex_);
    currentState_ = TouchState::DOWN;
    NotifyStateChange();
    HGM_LOGI("[touch machine] swicth to touch down status");
}

void TouchStateMachine::TouchUp()
{
    std::lock_guard<std::mutex> lock(touchStateMutex_);
    if (currentState_ == TouchState::DOWN) {
        auto& hgmCore = HgmCore::Instance();
        StartTouchTimer(hgmCore.GetFrameRateMgr()->GetCurScreenId(), TOUCH_UP_TIMEOUT_TIMER_EXPIRED, nullptr, [this]() {
            TouchUpTimeout();
        });
        currentState_ = TouchState::UP;
        NotifyStateChange();
        HGM_LOGI("[touch machine] down swicth to touch up status");
    } else {
        HGM_LOGD("[touch machine] not support to touch up status");
    }
}

void TouchStateMachine::TouchUpTimeout()
{
    std::lock_guard<std::mutex> lock(touchStateMutex_);
    if (currentState_ == TouchState::UP) {
        currentState_ = TouchState::IDLE;
        NotifyStateChange();
        if (idleEventCallback_ != nullptr) {
            idleEventCallback_();
        }
        HGM_LOGI("[touch machine] switch to 3s touch up idle status");
    } else {
        HGM_LOGD("[touch machine] 3s touch up already in idle status");
    }
}

void TouchStateMachine::RSIdleTimeout()
{
    std::lock_guard<std::mutex> lock(touchStateMutex_);
    if (currentState_ == TouchState::UP) {
        currentState_ = TouchState::IDLE;
        NotifyStateChange();
        if (idleEventCallback_ != nullptr) {
            idleEventCallback_();
        }
        auto& hgmCore = HgmCore::Instance();
        StopTouchTimer(hgmCore.GetFrameRateMgr()->GetCurScreenId());
        HGM_LOGI("[touch machine] switch to rs 600ms idle status");
    } else {
        HGM_LOGD("[touch machine] rs 600ms already in idle status");
    }
}

void TouchStateMachine::StartTouchTimer(ScreenId screenId, int32_t interval,
    std::function<void()> resetCallback, std::function<void()> expiredCallback)
{
    std::lock_guard<std::mutex> lock(touchTimerMapMutex_);
    auto newTimer = std::make_shared<HgmOneShotTimer>("idle_touchUp_timer" + std::to_string(screenId),
        std::chrono::milliseconds(interval), resetCallback, expiredCallback);
    touchTimerMap_[screenId] = newTimer;
    newTimer->Start();
}

void TouchStateMachine::StopTouchTimer(ScreenId screenId)
{
    std::lock_guard<std::mutex> lock(touchTimerMapMutex_);
    if (auto timer = touchTimerMap_.find(screenId); timer != touchTimerMap_.end()) {
        touchTimerMap_.erase(timer);
    }
}

void TouchStateMachine::NotifyStateChange()
{
    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    if (frameRateMgr == nullptr) {
        return;
    }
    frameRateMgr->GetMultiAppStrategy().HandleTouchInfo(pkgName_, currentState_);
}

void HgmTouchManager::HandleRSFrameUpdate(bool idleTimerExpired)
{
    RS_TRACE_FUNC();

    auto& hgmCore = HgmCore::Instance();
    if (touchMachine_.GetState() == TouchState::UP) {
        if (idleTimerExpired) {
            touchMachine_.TouchEventHandle(TouchEvent::RS_TIMEOUT);
        } else {
            HGM_LOGD("[touch manager] start rs 600ms idle timer");
            StartRSTimer(hgmCore.GetFrameRateMgr()->GetCurScreenId(), TOUCH_RS_IDLE_TIMER_EXPIRED, nullptr, [this]() {
                rsIdleUpdateCallback_(true);
            });
        }
    } else {
        StopRSTimer(hgmCore.GetFrameRateMgr()->GetCurScreenId());
    }
}

std::shared_ptr<HgmOneShotTimer> HgmTouchManager::GetRSTimer(ScreenId screenId) const
{
    if (auto timer = rsTimerMap_.find(screenId); timer != rsTimerMap_.end()) {
        return timer->second;
    }
    return nullptr;
}

void HgmTouchManager::StartRSTimer(ScreenId screenId, int32_t interval,
    std::function<void()> resetCallback, std::function<void()> expiredCallback)
{
    std::lock_guard<std::mutex> lock(timerMapMutex_);
    if (auto oldtimer = GetRSTimer(screenId); oldtimer == nullptr) {
        auto newTimer = std::make_shared<HgmOneShotTimer>("idle_rsIdle_timer" + std::to_string(screenId),
            std::chrono::milliseconds(interval), resetCallback, expiredCallback);
        rsTimerMap_[screenId] = newTimer;
        newTimer->Start();
    }
}

void HgmTouchManager::ResetRSTimer(ScreenId screenId) const
{
    std::lock_guard<std::mutex> lock(timerMapMutex_);
    if (auto timer = GetRSTimer(screenId); timer != nullptr) {
        timer->Reset();
    }
}

void HgmTouchManager::StopRSTimer(ScreenId screenId)
{
    std::lock_guard<std::mutex> lock(timerMapMutex_);
    if (auto timer = rsTimerMap_.find(screenId); timer != rsTimerMap_.end()) {
        rsTimerMap_.erase(timer);
    }
}

} // namespace Rosen
} // namespace OHOS