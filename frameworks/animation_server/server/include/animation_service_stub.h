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

#ifndef FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_ANIMATION_SERVICE_STUB
#define FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_ANIMATION_SERVICE_STUB

#include <ianimation_service.h>
#include <iremote_stub.h>
#include <message_option.h>
#include <message_parcel.h>

namespace OHOS {
class AnimationServiceStub : public IRemoteStub<IAnimationService> {
public:
    AnimationServiceStub();

    int32_t OnRemoteRequest(uint32_t code,
        MessageParcel& data, MessageParcel& reply, MessageOption& options) override;

private:
    int32_t StartRotationAnimationRemote(MessageParcel& data, MessageParcel& reply, MessageOption& options);

    using AnimationServiceStubFunc = int32_t (AnimationServiceStub::*)(MessageParcel &arguments,
        MessageParcel &reply, MessageOption &option);
    std::map<uint32_t, AnimationServiceStubFunc> memberFuncMap_;
};
} // namespace OHOS

#endif // FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_ANIMATION_SERVICE_STUB
