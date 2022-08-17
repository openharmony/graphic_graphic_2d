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

#ifndef RENDER_SERVICE_CLIENT_CORE_TRANSACTION_RS_APPLICATION_AGENT_IMPL_H
#define RENDER_SERVICE_CLIENT_CORE_TRANSACTION_RS_APPLICATION_AGENT_IMPL_H

#ifdef ROSEN_OHOS
#include "ipc_callbacks/rs_application_agent_stub.h"
#endif

namespace OHOS {
namespace Rosen {
#ifdef ROSEN_OHOS
class RSApplicationAgentImpl final : public RSApplicationAgentStub {
#else
class RSApplicationAgentImpl final {
#endif
public:
    static RSApplicationAgentImpl& Instance();
    void RegisterRSApplicationAgent();

private:
    RSApplicationAgentImpl() = default;
    virtual ~RSApplicationAgentImpl() = default;

    RSApplicationAgentImpl(const RSApplicationAgentImpl&) = delete;
    RSApplicationAgentImpl(const RSApplicationAgentImpl&&) = delete;
    RSApplicationAgentImpl& operator=(const RSApplicationAgentImpl&) = delete;
    RSApplicationAgentImpl& operator=(const RSApplicationAgentImpl&&) = delete;

#ifdef ROSEN_OHOS
    void OnTransaction(std::shared_ptr<RSTransactionData> transactionData) override;
    void OnRenderModeChanged(bool renderThreadNeedRender) override;
#endif
};
}
}
#endif // RENDER_SERVICE_CLIENT_CORE_TRANSACTION_RS_APPLICATION_AGENT_IMPL_H
