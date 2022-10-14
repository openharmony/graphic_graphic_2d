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

#ifndef WINDOW_ANIMATION_RS_WINDOW_ANIMATION_PROXY_H
#define WINDOW_ANIMATION_RS_WINDOW_ANIMATION_PROXY_H

#include <string>

#include <iremote_proxy.h>

#include "rs_iwindow_animation_controller.h"

namespace OHOS {
namespace Rosen {
class RSWindowAnimationProxy : public IRemoteProxy<RSIWindowAnimationController> {
public:
    explicit RSWindowAnimationProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSWindowAnimationProxy() = default;

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
    bool WriteInterfaceToken(MessageParcel& data);
    bool WriteTargetAndCallback(MessageParcel& data, const sptr<RSWindowAnimationTarget>& windowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback);
    static inline BrokerDelegator<RSWindowAnimationProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS

#endif // WINDOW_ANIMATION_RS_WINDOW_ANIMATION_PROXY_H
