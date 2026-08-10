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

#include "rs_application_agent_impl.h"

#ifdef ROSEN_OHOS
#include "platform/ohos/rs_render_service_connect_hub.h"
#include "platform/common/rs_log.h"
#endif
#include "rs_trace.h"
#include "ui/rs_ui_director.h"
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
#ifdef OHOS_PLATFORM
static sptr<RSApplicationAgentImpl> gRSApplicationAgentImplInstance;
#endif
RSApplicationAgentImpl::~RSApplicationAgentImpl()
{
}

RSApplicationAgentImpl* RSApplicationAgentImpl::Instance()
{
#ifdef OHOS_PLATFORM
    if (gRSApplicationAgentImplInstance == nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (gRSApplicationAgentImplInstance == nullptr) {
            gRSApplicationAgentImplInstance = new RSApplicationAgentImpl();
        }
    }
    return gRSApplicationAgentImplInstance.GetRefPtr();
#else
    return nullptr;
#endif
}

void RSApplicationAgentImpl::Release(bool isRegistered)
{
#ifdef OHOS_PLATFORM
    std::lock_guard<std::mutex> lock(mutex_);
    if (gRSApplicationAgentImplInstance == nullptr) {
        return;
    }
    // Drop the client-side reference first. The stub is NOT destroyed here: while the server holds
    // a proxy, the binder kernel keeps a strong reference on it (BR_ACQUIRE -> IncStrongRef), so it
    // survives until the server releases that proxy. UnregisterFromAllConnections is static and
    // sends the unregister IPC through the render connection proxy, which does not need the stub;
    // the stub is finally released on the binder thread once the server drops its proxy (BR_RELEASE).
    while (gRSApplicationAgentImplInstance->GetSptrRefCount() != TOKEN_STRONG_REF_COUNT) {
        gRSApplicationAgentImplInstance->DecStrongRef(gRSApplicationAgentImplInstance);
    }
    
    gRSApplicationAgentImplInstance = nullptr;
    if (isRegistered) {
        UnregisterFromAllConnections();
    }
#endif
}

void RSApplicationAgentImpl::UnregisterFromAllConnections()
{
#ifdef ROSEN_OHOS
    // Cover every render-process connection the agent may have been registered with, so that no UI
    // director / render process leaks a server-side proxy reference. The server resolves the agent by
    // the calling pid, so nothing needs to be passed across the IPC.
    auto connRenderProcesses = RSRenderServiceConnectHub::GetAllClientToRenderConnections();
    if (connRenderProcesses.size() == 0) {
        auto connection = RSRenderServiceConnectHub::GetClientToRenderConnection(INVALID_TOKEN_MASK_ID);
        if (connection != nullptr) {
            connection->UnRegisterApplicationAgent();
            return;
        }
        ROSEN_LOGE("RSApplicationAgentImpl::UnregisterFromAllConnections has not connection");
    }
    for (const auto& entry : connRenderProcesses) {
        const auto& info = entry.second;
        if (info.clientToRenderConnection != nullptr) {
            info.clientToRenderConnection->UnRegisterApplicationAgent();
        }
    }
#endif
}

void RSApplicationAgentImpl::RegisterRSApplicationAgent(std::shared_ptr<RSUIContext> rsUIContext)
{
#ifdef ROSEN_OHOS
    std::lock_guard<std::mutex> lock(mutex_);
    if (rsUIContext == nullptr) {
        ROSEN_LOGE("RSApplicationAgentImpl::RegisterRSApplicationAgent rsUIContext return");
        return;
    }
    if (auto renderInterface = rsUIContext->GetRSRenderInterface()) {
        if (auto renderPipelineClient = renderInterface->GetRSRenderPipelineClient()) {
            ROSEN_LOGI("RSApplicationAgentImpl::RegisterRSApplicationAgent!");
            renderPipelineClient->RegisterApplicationAgent(0, sptr<RSApplicationAgentImpl>(this));
        }
    }
#endif
}

#ifdef ROSEN_OHOS
void RSApplicationAgentImpl::OnTransaction(std::shared_ptr<RSTransactionData> transactionData)
{
    RS_TRACE_NAME("RSApplicationAgentImpl::OnTransaction");
    RSUIDirector::RecvMessages(transactionData);
}
#endif

RSApplicationAgentLifecycleOwner& RSApplicationAgentLifecycleOwner::Instance()
{
    // Function-local static: constructed on first use (RSUIDirector::Init) and destroyed at static
    // teardown, which drives RSApplicationAgentImpl::Release() once.
    static RSApplicationAgentLifecycleOwner instance;
    return instance;
}

RSApplicationAgentLifecycleOwner::~RSApplicationAgentLifecycleOwner()
{
    RSApplicationAgentImpl::Release(isRegistered_.load());
}

void RSApplicationAgentLifecycleOwner::EnsureRegistered(std::shared_ptr<RSUIContext> rsUIContext)
{
    if (!isRegistered_.load()) {
        if (auto agent = RSApplicationAgentImpl::Instance()) {
            agent->RegisterRSApplicationAgent(rsUIContext);
            isRegistered_.store(true);
        }
    }
}
}
}
