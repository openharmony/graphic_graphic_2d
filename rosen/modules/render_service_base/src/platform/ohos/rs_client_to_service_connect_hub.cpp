/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_client_to_service_connect_hub.h"

#include <if_system_ability_manager.h>
#include <iremote_stub.h>
#include <iservice_registry.h>
#include <mutex>
#include <system_ability_definition.h>
#include <unistd.h>

#include "platform/common/rs_log.h"
#include "transaction/zidl/rs_client_to_service_connection_proxy.h"
#include "vsync_iconnection_token.h"
#include "platform/ohos/transaction/rs_irender_connection_token.h"
#include "rs_render_service_proxy.h"

namespace OHOS {
namespace Rosen {
std::once_flag RSClientToServiceConnectHub::flag_;
sptr<RSClientToServiceConnectHub> RSClientToServiceConnectHub::instance_ = nullptr;

sptr<RSClientToServiceConnectHub> RSClientToServiceConnectHub::GetInstance()
{
    std::call_once(flag_, &RSClientToServiceConnectHub::Init);
    return instance_;
}

void RSClientToServiceConnectHub::Init()
{
    instance_ = new RSClientToServiceConnectHub();
    ::atexit(&RSClientToServiceConnectHub::Destroy);
}

void RSClientToServiceConnectHub::Destroy()
{
    instance_ = nullptr;
}

RSClientToServiceConnectHub::RSClientToServiceConnectHub()
{
}

RSClientToServiceConnectHub::~RSClientToServiceConnectHub() noexcept
{
    if (UNLIKELY(!renderService_)) {
        ROSEN_LOGE("~RSClientToServiceConnectHub renderService_ is null");
        return;
    }
    ROSEN_LOGI("~RSClientToServiceConnectHub");
    if (renderService_->AsObject() && deathRecipient_) {
        renderService_->AsObject()->RemoveDeathRecipient(deathRecipient_);
    }
    if (token_ == nullptr) {
        ROSEN_LOGI("token_ is deleted");
        return;
    }
    renderService_ = nullptr;
    conn_ = nullptr;
    token_ = nullptr;
}

sptr<RSIClientToServiceConnection> RSClientToServiceConnectHub::GetClientToServiceConnection()
{
    auto hub = GetInstance();
    if (hub == nullptr) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(hub->mutex_);
    if (hub->conn_ != nullptr && hub->renderService_ != nullptr) {
        return hub->conn_;
    }
    if (!hub->Connect()) {
        ROSEN_LOGE("RSClientToServiceConnectHub::GetClientToServiceConnection connect fail");
        return nullptr;
    }
    return hub->conn_;
}

bool RSClientToServiceConnectHub::Connect()
{
    RS_LOGD("RSClientToServiceConnectHub::Connect");
    int tryCnt = 0;
    sptr<RSIRenderService> renderService = nullptr;
    do {
        usleep(1000 * tryCnt);
        ++tryCnt;
        if (tryCnt == 5) {
            RS_LOGD("RSClientToServiceConnectHub::Connect failed, tried %{public}d times.", tryCnt);
            break;
        }
        auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgr == nullptr) {
            continue;
        }
        auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
        if (remoteObject == nullptr || !remoteObject->IsProxyObject()) {
            continue;
        }
        renderService = iface_cast<RSRenderServiceProxy>(remoteObject);
        if (renderService != nullptr) {
            break;
        }
    } while (true);

    if (renderService == nullptr) {
        RS_LOGD("RSClientToServiceConnectHub::Connect, failed to get render service proxy.");
        return false;
    }
    wptr<RSClientToServiceConnectHub> hub = this;
    deathRecipient_ = new RenderServiceDeathRecipient(hub);
    if (!renderService->AsObject()->AddDeathRecipient(deathRecipient_)) {
        ROSEN_LOGW("RSClientToServiceConnectHub::Connect, failed to AddDeathRecipient.");
    }

    if (token_ == nullptr) {
        token_ = new IRemoteStub<RSIConnectionToken>();
    }
    ROSEN_LOGI("RSClientToServiceConnectHub call CreateConnection");
    auto [conn, renderConn] = renderService->CreateConnection(token_, false);

    if (conn == nullptr) {
        RS_LOGD("RSClientToServiceConnectHub::Connect, failed to CreateConnection.");
        return false;
    }

    renderService_ = renderService;
    conn_ = conn;
    return true;
}

void RSClientToServiceConnectHub::ConnectDied()
{
    mutex_.lock();
    renderService_ = nullptr;
    if (conn_) {
        conn_->RunOnRemoteDiedCallback();
    }
    conn_ = nullptr;
    deathRecipient_ = nullptr;
    token_ = nullptr;
    mutex_.unlock();
}

void RSClientToServiceConnectHub::RenderServiceDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject>& remote)
{
    auto remoteSptr = remote.promote();
    if (remoteSptr == nullptr) {
        ROSEN_LOGW("RSClientToServiceConnectHub::OnRemoteDied: can't promote.");
        return;
    }
    auto hub = hub_.promote();
    if (hub == nullptr) {
        ROSEN_LOGW("RSClientToServiceConnectHub::OnRemoteDied: hub was dead.");
        return;
    }
    hub->ConnectDied();
}

} // namespace Rosen
} // namespace OHOS
