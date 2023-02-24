/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "rs_sync_transaction_controller_proxy.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSSyncTransactionControllerProxy::RSSyncTransactionControllerProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSISyncTransactionController>(impl)
{
}

bool RSSyncTransactionControllerProxy::WriteInterfaceToken(MessageParcel& data)
{
    if (!data.WriteInterfaceToken(RSSyncTransactionControllerProxy::GetDescriptor())) {
        ROSEN_LOGE("RSSyncTransactionControllerProxy Failed to write interface token!");
        return false;
    }

    return true;
}

void RSSyncTransactionControllerProxy::CreateTransactionFinished()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        return;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        ROSEN_LOGE("remote is null!");
        return;
    }

    auto ret = remote->SendRequest(RSSyncTransactionControllerProxy::CREATE_TRANSACTION_FINISHED, data, reply, option);
    if (ret != NO_ERROR) {
        ROSEN_LOGE("Failed to send sync transaction controller request, error code:%d", ret);
    }
}
} // namespace Rosen
} // namespace OHOS
