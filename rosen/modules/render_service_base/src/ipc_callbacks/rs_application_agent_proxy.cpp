/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "rs_application_agent_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS {
namespace Rosen {
RSApplicationAgentProxy::RSApplicationAgentProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IApplicationAgent>(impl)
{
}

void RSApplicationAgentProxy::OnTransaction(std::shared_ptr<RSTransactionData> transactionData)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(IApplicationAgent::GetDescriptor())) {
        return;
    }

    if (!data.WriteParcelable(transactionData.get())) {
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t err = Remote()->SendRequest(IApplicationAgent::COMMIT_TRANSACTION, data, reply, option);
    if (err != NO_ERROR) {
        // [PLANNING]: Error log
    }
}

void RSApplicationAgentProxy::OnRenderModeChanged(bool renderThreadNeedRender)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(IApplicationAgent::GetDescriptor())) {
        return;
    }

    if (!data.WriteBool(renderThreadNeedRender)) {
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t err = Remote()->SendRequest(IApplicationAgent::NOTIFY_RENDER_MODE_CHANGED, data, reply, option);
    if (err != NO_ERROR) {
        // [PLANNING]: Error log, unexpected error code 32 is returned when the interface is called correctly
    }
}

void RSApplicationAgentProxy::NotifyClearBufferCache()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(IApplicationAgent::GetDescriptor())) {
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t err = Remote()->SendRequest(IApplicationAgent::NOTIFY_CLEAR_BUFFER_CACHE, data, reply, option);
    if (err != NO_ERROR) {
        // [PLANNING]: Error log, unexpected error code 32 is returned when the interface is called correctly
    }
}
} // namespace Rosen
} // namespace OHOS
