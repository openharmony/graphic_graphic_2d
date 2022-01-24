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

#ifdef ROSEN_OHOS
#include "rs_application_render_thread_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "command/rs_command.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS {
namespace Rosen {
RSApplicationRenderThreadProxy::RSApplicationRenderThreadProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IApplicationRenderThread>(impl)
{
}

void RSApplicationRenderThreadProxy::OnTransaction(std::shared_ptr<RSTransactionData> transactionData)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(IApplicationRenderThread::GetDescriptor())) {
        return;
    }

    if (!data.WriteParcelable(transactionData.get())) {
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t err = Remote()->SendRequest(IApplicationRenderThread::COMMIT_TRANSACTION, data, reply, option);
    if (err != NO_ERROR) {
        // TODO: Error log
    }
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS
