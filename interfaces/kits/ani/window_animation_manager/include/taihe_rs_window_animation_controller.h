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

#ifndef INTERFACES_KITS_ANI_TAIHE_RS_WINDOW_ANIMATION_CONTROLLER_H
#define INTERFACES_KITS_ANI_TAIHE_RS_WINDOW_ANIMATION_CONTROLLER_H

#include <event_handler.h>
#include <rs_window_animation_finished_callback.h>
#include <rs_window_animation_stub.h>
#include <rs_window_animation_target.h>
#include <string>

#include "ohos.animation.windowAnimationManager.impl.hpp"
#include "ohos.animation.windowAnimationManager.proj.hpp"
#include "taihe_window_animation_finished_callback.h"

namespace OHOS {
namespace Rosen {
class TaiheRSWindowAnimationController : public RSWindowAnimationStub {
public:
    explicit TaiheRSWindowAnimationController();
    virtual ~TaiheRSWindowAnimationController() = default;

    void SetJsController(ohos::animation::windowAnimationManager::WindowAnimationController jsController);

    void OnStartApp(StartingAppType type, const sptr<RSWindowAnimationTarget>& startingWindowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override;

    void OnAppTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
        const sptr<RSWindowAnimationTarget>& toWindowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override;

    void OnAppBackTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
        const sptr<RSWindowAnimationTarget>& toWindowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override;

    void OnMinimizeWindow(const sptr<RSWindowAnimationTarget>& minimizingWindowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override;

    void OnMinimizeAllWindow(std::vector<sptr<RSWindowAnimationTarget>> minimizingWindowsTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override;

    void OnCloseWindow(const sptr<RSWindowAnimationTarget>& closingWindowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override;

    void OnScreenUnlock(const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override;

    void OnWindowAnimationTargetsUpdate(const sptr<RSWindowAnimationTarget>& fullScreenWindowTarget,
        const std::vector<sptr<RSWindowAnimationTarget>>& floatingWindowTargets) override;

    void OnWallpaperUpdate(const sptr<RSWindowAnimationTarget>& wallpaperTarget) override;

private:
    void HandleOnStartApp(StartingAppType type, const sptr<RSWindowAnimationTarget>& startingWindowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback);

    void HandleOnAppTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
        const sptr<RSWindowAnimationTarget>& toWindowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback);

    void HandleOnAppBackTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
        const sptr<RSWindowAnimationTarget>& toWindowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback);

    void HandleOnMinimizeWindow(const sptr<RSWindowAnimationTarget>& minimizingWindowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback);

    void HandleOnCloseWindow(const sptr<RSWindowAnimationTarget>& closingWindowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback);

    void HandleOnScreenUnlock(const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback);

    void HandleOnWindowAnimationTargetsUpdate(const sptr<RSWindowAnimationTarget>& fullScreenWindowTarget,
        const std::vector<sptr<RSWindowAnimationTarget>>& floatingWindowTargets);

    void HandleOnWallpaperUpdate(const sptr<RSWindowAnimationTarget>& wallpaperTarget);

    std::shared_ptr<AppExecFwk::EventHandler> handler_;
};
} // namespace Rosen
} // namespace OHOS

#endif // INTERFACES_KITS_ANI_TAIHE_RS_WINDOW_ANIMATION_CONTROLLER_H