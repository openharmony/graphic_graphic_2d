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

#include "rs_window_animation_finished_callback_stub.h"

#include "rs_window_animation_log.h"

namespace OHOS {
namespace Rosen {
int RSWindowAnimationFinishedCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    WALOGD("Window animation finished callback on remote request!");
    int ret = ERR_NONE;
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WALOGE("Failed to check interface token!");
        return ERR_INVALID_STATE;
    }

    switch (code) {
        case RSIWindowAnimationFinishedCallback::ON_ANIMATION_FINISHED: {
            OnAnimationFinished();
            break;
        }
        default: {
            WALOGE("Unknown transaction!");
            ret = ERR_UNKNOWN_TRANSACTION;
            break;
        }
    }

    return ret;
}
} // namespace Rosen
} // namespace OHOS
