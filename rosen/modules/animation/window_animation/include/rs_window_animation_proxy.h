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
    virtual ~RSWindowAnimationProxy() =default;

    virtual void OnTransition(const sptr<RSWindowAnimationTarget>& from,
                              const sptr<RSWindowAnimationTarget>& to,
                              const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override;

    virtual void OnMinimizeWindow(const sptr<RSWindowAnimationTarget>& minimizingWindow,
                                  const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override;

    virtual void OnCloseWindow(const sptr<RSWindowAnimationTarget>& closingWindow,
                               const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override;

private:
    bool WriteInterfaceToken(MessageParcel& data);
    static inline BrokerDelegator<RSWindowAnimationProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS

#endif // WINDOW_ANIMATION_RS_WINDOW_ANIMATION_PROXY_H
