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

#include "animation_service_stub.h"

#include <gslogger.h>

namespace OHOS {
namespace {
DEFINE_HILOG_LABEL("AnimationServiceStub");
} // namespace

AnimationServiceStub::AnimationServiceStub()
{
    memberFuncMap_[START_ROTATION_ANIMATION] = &AnimationServiceStub::StartRotationAnimationRemote;
    memberFuncMap_[SPLIT_MODE_CREATE_BACKGOUND] = &AnimationServiceStub::SplitModeCreateBackgroundRemote;
    memberFuncMap_[SPLIT_MODE_CREATE_MIDDLE_LINE] = &AnimationServiceStub::SplitModeCreateMiddleLineRemote;
}

int32_t AnimationServiceStub::OnRemoteRequest(uint32_t code,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        GSLOG2HI(ERROR) << "Wrong Descriptor";
        return ERR_INVALID_STATE;
    }

    auto it = memberFuncMap_.find(code);
    if (it == memberFuncMap_.end()) {
        GSLOG2HI(ERROR) << "Cannot process " << code;
        return ERR_INVALID_STATE;
    }

    return (this->*(it->second))(data, reply, option);
}

int32_t AnimationServiceStub::StartRotationAnimationRemote(MessageParcel& data,
    MessageParcel& reply, MessageOption& options)
{
    auto did = data.ReadInt32();
    auto degree = data.ReadInt32();
    auto gret = StartRotationAnimation(did, degree);
    reply.WriteInt32(gret);
    return 0;
}

int32_t AnimationServiceStub::SplitModeCreateBackgroundRemote(MessageParcel& data,
    MessageParcel& reply, MessageOption& options)
{
    reply.WriteInt32(SplitModeCreateBackground());
    return 0;
}

int32_t AnimationServiceStub::SplitModeCreateMiddleLineRemote(MessageParcel& data,
    MessageParcel& reply, MessageOption& options)
{
    reply.WriteInt32(SplitModeCreateMiddleLine());
    return 0;
}
} // namespace OHOS
