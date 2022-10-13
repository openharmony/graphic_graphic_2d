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

#ifndef WINDOW_ANIMATION_RS_IWINDOW_ANIMATION_CONTROLLER_H
#define WINDOW_ANIMATION_RS_IWINDOW_ANIMATION_CONTROLLER_H

#include <vector>

#include <iremote_broker.h>

namespace OHOS {
namespace Rosen {
struct RSWindowAnimationTarget;
class RSIWindowAnimationFinishedCallback;

enum StartingAppType {
    FROM_LAUNCHER,
    FROM_RECENT,
    FROM_OTHER,
};

class RSIWindowAnimationController : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.RSIWindowAnimationController");

    enum {
        ON_START_APP,
        ON_APP_TRANSITION,
        ON_APP_BACK_TRANSITION,
        ON_MINIMIZE_WINDOW,
        ON_MINIMIZE_ALLWINDOW,
        ON_CLOSE_WINDOW,
        ON_SCREEN_UNLOCK,
        ON_WINDOW_ANIMATION_TARGETS_UPDATE,
        ON_WALLPAPER_UPDATE,
    };

    virtual void OnStartApp(StartingAppType type, const sptr<RSWindowAnimationTarget>& startingWindowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) = 0;

    virtual void OnAppTransition(const sptr<RSWindowAnimationTarget>& from, const sptr<RSWindowAnimationTarget>& to,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) = 0;

    virtual void OnAppBackTransition(const sptr<RSWindowAnimationTarget>& from, const sptr<RSWindowAnimationTarget>& to,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) = 0;

    virtual void OnMinimizeWindow(const sptr<RSWindowAnimationTarget>& minimizingWindow,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) = 0;

    virtual void OnMinimizeAllWindow(std::vector<sptr<RSWindowAnimationTarget>> minimizingWindows,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) = 0;

    virtual void OnCloseWindow(const sptr<RSWindowAnimationTarget>& closingWindow,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) = 0;

    virtual void OnScreenUnlock(const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) = 0;

    virtual void OnWindowAnimationTargetsUpdate(const sptr<RSWindowAnimationTarget>& fullScreenWindowTarget,
        const std::vector<sptr<RSWindowAnimationTarget>>& floatingWindowTargets) = 0;
    
    virtual void OnWallpaperUpdate(const sptr<RSWindowAnimationTarget>& wallpaperTarget) = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // WINDOW_ANIMATION_RS_IWINDOW_ANIMATION_CONTROLLER_H
