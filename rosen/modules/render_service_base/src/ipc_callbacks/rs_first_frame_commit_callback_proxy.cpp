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

#include "rs_first_frame_commit_callback_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSFirstFrameCommitCallbackProxy::RSFirstFrameCommitCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIFirstFrameCommitCallback>(impl)
{
}

void RSFirstFrameCommitCallbackProxy::OnFirstFrameCommit(uint64_t screenId, int64_t timestamp)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIFirstFrameCommitCallback::GetDescriptor())) {
        ROSEN_LOGE("RSFirstFrameCommitCallbackProxy::OnFirstFrameCommit WriteInterfaceToken failed");
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(screenId)) {
        ROSEN_LOGE("RSFirstFrameCommitCallbackProxy::OnFirstFrameCommit WriteUint64 failed");
        return;
    }
    if (!data.WriteInt64(timestamp)) {
        ROSEN_LOGE("RSFirstFrameCommitCallbackProxy::OnFirstFrameCommit WriteInt64 failed");
        return;
    }
    uint32_t code =
        static_cast<uint32_t>(RSIFirstFrameCommitCallbackInterfaceCode::ON_FIRST_FRAME_COMMIT);
    auto remote = Remote();
    if (remote == nullptr) {
        ROSEN_LOGE("RSFirstFrameCommitCallbackProxy::OnFirstFrameCommit remote is null!");
        return;
    }

    int32_t err = remote->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSFirstFrameCommitCallbackProxy::OnFirstFrameCommit error = %{public}d", err);
    }
}
} // namespace Rosen
} // namespace OHOS
