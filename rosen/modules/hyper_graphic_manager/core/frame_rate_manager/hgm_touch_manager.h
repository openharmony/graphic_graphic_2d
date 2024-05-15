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

#ifndef HGM_TOUCH_MANAGER_H
#define HGM_TOUCH_MANAGER_H

#include <memory>

#include "hgm_one_shot_timer.h"
#include "hgm_state_machine.h"

namespace OHOS::Rosen {
enum TouchEvent : int32_t {
    DOWN_EVENT,
    UP_EVENT,
};

enum TouchState : int32_t {
    IDLE_STATE,
    DOWN_STATE,
    UP_STATE,
};

class HgmTouchManager final : public HgmStateMachine<TouchState, TouchEvent> {
public:
    HgmTouchManager();
    ~HgmTouchManager() override = default;

    void HandleTouchEvent(TouchEvent event, const std::string& pkgName = "");
    void HandleRsFrame();
    void HandleThirdFrameIdle();

    std::string GetPkgName() const { return pkgName_; }
protected:
    bool CheckChangeStateValid(State lastState, State newState) override;
    void ExecuteCallback(const std::function<void()>& callback) override;
private:
    std::string pkgName_;
    HgmOneShotTimer upTimeoutTimer_;
    HgmOneShotTimer rsIdleTimeoutTimer_;
};
} // OHOS::Rosen
#endif // HGM_TOUCH_MANAGER_H