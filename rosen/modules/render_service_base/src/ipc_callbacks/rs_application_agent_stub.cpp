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

#include "rs_application_agent_stub.h"

#include "command/rs_command.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS {
namespace Rosen {
int RSApplicationAgentStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (!securityManager_.IsInterfaceCodeAccessible(code)) {
        RS_LOGE("RSApplicationAgentStub::OnRemoteRequest no permission to access codeID=%{public}u.", code);
        return ERR_INVALID_STATE;
    }

    auto token = data.ReadInterfaceToken();
    if (token != IApplicationAgent::GetDescriptor()) {
        return ERR_INVALID_STATE;
    }

    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(IApplicationAgentInterfaceCode::COMMIT_TRANSACTION): {
            std::shared_ptr<RSTransactionData> transactionData(data.ReadParcelable<RSTransactionData>());
            OnTransaction(transactionData);
            break;
        }
        default: {
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }

    return ret;
}

const RSInterfaceCodeSecurityManager RSApplicationAgentStub::securityManager_ = \
    RSInterfaceCodeSecurityManager::CreateInstance<IApplicationAgentInterfaceCodeAccessVerifier>();
} // namespace Rosen
} // namespace OHOS
