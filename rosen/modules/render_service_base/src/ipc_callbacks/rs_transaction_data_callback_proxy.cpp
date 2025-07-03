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

#include "rs_transaction_data_callback_proxy.h"
#include <message_option.h>
#include <message_parcel.h>
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSTransactionDataCallbackProxy::RSTransactionDataCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSITransactionDataCallback>(impl)
{
}

void RSTransactionDataCallbackProxy::OnAfterProcess(uint64_t token, uint64_t timeStamp)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSITransactionDataCallback::GetDescriptor())) {
        ROSEN_LOGE("RSTransactionDataCallbackProxy: OnAfterProcess writeInterfaceToken error");
        return;
    }
    if (!data.WriteUint64(token)) {
        ROSEN_LOGE("RSTransactionDataCallbackProxy: OnAfterProcess write multi token error");
        return;
    }
    if (!data.WriteUint64(timeStamp)) {
        ROSEN_LOGE("RSTransactionDataCallbackProxy: OnAfterProcess write timeStamp error");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSITransactionDataCallbackInterfaceCode::ON_AFTER_PROCESS);
    RS_LOGD("RSTransactionDataCallbackProxy: OnAfterProcess send data, timeStamp: %{public}"
        PRIu64 " token: %{public}" PRIu64, timeStamp, token);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSTransactionDataCallbackProxy: Remote()->SendRequest() error");
    }
}
} // namespace Rosen
} // namespace OHOS