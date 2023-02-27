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

#include "rs_sync_transaction_controller_stub.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
int RSSyncTransactionControllerStub::OnRemoteRequest(uint32_t code,MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    ROSEN_LOGD("RS transaction controller on remote request!");
    int ret = ERR_NONE;
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        ROSEN_LOGE("Failed to check interface token!");
        return ERR_INVALID_STATE;
    }

    switch (code) {
        case RSISyncTransactionController::CREATE_TRANSACTION_FINISHED: {
            CreateTransactionFinished();
            break;
        }
        default: {
            ROSEN_LOGE("Unknown transaction!");
            ret = ERR_UNKNOWN_TRANSACTION;
            break;
        }
    }

    return ret;
}
} // namespace Rosen
} // namespace OHOS
