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

#ifndef HGM_POINTER_MANAGER_H
#define HGM_POINTER_MANAGER_H

#include <memory>

#include "hgm_command.h"
#include "hgm_one_shot_timer.h"
#include "hgm_state_machine.h"

namespace OHOS::Rosen {
enum PointerEvent : int32_t {
    POINTER_ACTIVE_EVENT,
    POINTER_ACTIVE_TIMEOUT_EVENT,
    POINTER_RS_IDLE_TIMEOUT_EVENT,
};

enum PointerState : int32_t {
    POINTER_IDLE_STATE,
    POINTER_ACTIVE_STATE,
};

struct PointerInfo {
    std::string pkgName;
    PointerState pointerState;
    int32_t upExpectFps;
};

class HgmFrameRateManager;
class HgmPointerManager final : public HgmStateMachine<PointerState, PointerEvent> {
public:
    HgmPointerManager();
    ~HgmPointerManager() = default;

    void HandlePointerEvent(PointerEvent event, const std::string& pkgName = "");
    void HandleRsFrame();
    void HandleTimerReset();

    std::string GetPkgName() const { return pkgName_; }
protected:
    std::string State2String(State state) const override;
    bool CheckChangeStateValid(State lastState, State newState) override;
private:
    void Vote();
    void UpdateStrategyByPointer();
    std::string pkgName_;
    HgmSimpleTimer activeTimeoutTimer_;
    HgmSimpleTimer rsIdleTimeoutTimer_;
    PointerInfo pointerInfo_ = { "", PointerState::POINTER_IDLE_STATE, OLED_120_HZ };
};
} // OHOS::Rosen
#endif // HGM_POINTER_MANAGER_H