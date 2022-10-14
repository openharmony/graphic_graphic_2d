/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_KITS_NAPI_GRAPHIC_ANIMATION_RS_WINDOW_ANIMATION_CONTROLLER_H
#define INTERFACES_KITS_NAPI_GRAPHIC_ANIMATION_RS_WINDOW_ANIMATION_CONTROLLER_H

#include <string>

#include <native_engine/native_engine.h>
#include <event_handler.h>

#include <rs_window_animation_finished_callback.h>
#include <rs_window_animation_stub.h>
#include <rs_window_animation_target.h>
#include "rs_window_animation_callback.h"

namespace OHOS {
namespace Rosen {
class RSWindowAnimationController : public RSWindowAnimationStub {
public:
    explicit RSWindowAnimationController(NativeEngine& engine);
    virtual ~RSWindowAnimationController() = default;

    void SetJsController(NativeValue* jsController);

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

    void CallJsFunction(const std::string& methodName, NativeValue* const* argv, size_t argc);

    NativeEngine& engine_;
    std::unique_ptr<NativeReference> jsController_;
};
} // namespace Rosen
} // namespace OHOS

#endif // INTERFACES_KITS_NAPI_GRAPHIC_ANIMATION_RS_WINDOW_ANIMATION_CONTROLLER_H
