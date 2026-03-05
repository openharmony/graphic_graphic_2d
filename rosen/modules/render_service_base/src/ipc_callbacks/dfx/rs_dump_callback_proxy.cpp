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

#include "rs_dump_callback_proxy.h"
#include "gfx/dump/rs_dump_manager.h"

namespace OHOS {
namespace Rosen {
void RSDumpCallbackProxy::OnDumpResult(std::string& dumpResult)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIDumpCallback::GetDescriptor())) {
        RS_LOGE("%{public}s: WriteInterfaceToken failed", __func__);
        return;
    }
    int32_t dumpStringLength = static_cast<int32_t>(dumpResult.length());
    const char* dumpData = dumpResult.c_str();
    if (!data.WriteInt32(dumpStringLength)) {
        RS_LOGE("%{public}s: WriteSize failed.", __func__);
        return;
    }
    if (!RSDumpManager::WriteAshmemDataToParcel(data, dumpStringLength, dumpData)) {
        RS_LOGE("%{public}s: WritedumpData failed.", __func__);
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSDumpCallbackInterfaceCode::REPLY_DUMP_RESULT_TO_SERVICE);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        RS_LOGE("%{public}s: SendRequest failed, err is %{public}d", __func__, err);
        return;
    }
}
} // namespace Rosen
} // namespace OHOS