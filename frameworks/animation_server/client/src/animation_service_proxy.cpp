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

#include "animation_service_proxy.h"

#include <gslogger.h>

namespace OHOS {
namespace {
DEFINE_HILOG_LABEL("AnimationServiceProxy");
} // namespace

AnimationServiceProxy::AnimationServiceProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IAnimationService>(impl)
{
}

GSError AnimationServiceProxy::StartRotationAnimation(int32_t did, int32_t degree)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    auto retval = data.WriteInterfaceToken(GetDescriptor());
    if (!retval) {
        GSLOG2HI(ERROR) << "WriteInterfaceToken failed";
        return GSERROR_INVALID_ARGUMENTS;
    }

    data.WriteInt32(did);
    data.WriteInt32(degree);

    int32_t res = Remote()->SendRequest(START_ROTATION_ANIMATION, data, reply, option);
    if (res) {
        GSLOG2HI(ERROR) << "SendRequest failed, retval=" << res;
        return GSERROR_BINDER;
    }

    GSError ret = static_cast<enum GSError>(reply.ReadInt32());
    if (ret != GSERROR_OK) {
        GSLOG2HI(ERROR) << "Call return failed: " << ret;
    }

    return ret;
}
} // namespace OHOS
