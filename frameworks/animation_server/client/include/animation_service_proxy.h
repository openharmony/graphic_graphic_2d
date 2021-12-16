/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef ANIMATION_SERVICE_PROXY_H
#define ANIMATION_SERVICE_PROXY_H

#include <iremote_proxy.h>

#include <ianimation_service.h>

namespace OHOS {
class AnimationServiceProxy : public IRemoteProxy<IAnimationService> {
public:
    AnimationServiceProxy(const sptr<IRemoteObject>& impl);

    GSError StartRotationAnimation(int32_t did, int32_t degree) override;

private:
    static inline BrokerDelegator<AnimationServiceProxy> delegator_;
};
} // namespace OHOS

#endif // ANIMATION_SERVICE_PROXY_H
