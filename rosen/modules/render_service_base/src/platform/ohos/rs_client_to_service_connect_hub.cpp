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

#include "ipc_callbacks/rs_ipc_callbacks_check.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/transaction/rs_irender_connection_token.h"
#include "transaction/zidl/rs_client_to_service_connection_proxy.h"

namespace OHOS {
namespace Rosen {
std::once_flag RSClientToServiceConnectHub::flag_;
sptr<RSClientToServiceConnectHub> RSClientToServiceConnectHub::instance_ = nullptr;
constexpr int32_t TOKEN_STRONG_REF_COUNT = 1;

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

RSClientToServiceConnectHub::RSClientToServiceConnectHub() {}

RSClientToServiceConnectHub::~RSClientToServiceConnectHub() noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (UNLIKELY(!renderService_)) {
        ROSEN_LOGE("~RSClientToServiceConnectHub renderService_ is null");
        return;
    }
    ROSEN_LOGI("~RSClientToServiceConnectHub");
    if (renderService_ && deathRecipient_) {
        renderService_->RemoveDeathRecipient(deathRecipient_);
    }
    if (token_ == nullptr) {
        ROSEN_LOGI("token_ is deleted");
        return;
    }
    ROSEN_LOGI("RefCount: %{public}d", token_->GetSptrRefCount());
    while (token_->GetSptrRefCount() != TOKEN_STRONG_REF_COUNT) {
        token_->DecStrongRef(this);
    }
    // Remove connection
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    IPCObjectProxy* proxy = reinterpret_cast<IPCObjectProxy*>(renderService_.GetRefPtr());
    std::u16string descriptor = proxy->GetInterfaceDescriptor();
    if (!data.WriteInterfaceToken(descriptor)) {
        ROSEN_LOGE("RemoveConnection(): WriteInterfaceToken failed.");
        return;
    }
    if (!data.WriteRemoteObject(token_->AsObject())) {
        ROSEN_LOGE("RemoveConnection(): WriteRemoteObject failed.");
        return;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceInterfaceCode::REMOVE_CONNECTION);
    int32_t err = SendRequestRemote::SendRequest(renderService_.GetRefPtr(), code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RemoveConnection(): SendRequest failed, err is %{public}d.", err);
        return;
    }

    token_ = nullptr;
    conn_ = nullptr;
}

sptr<RSIClientToServiceConnection> RSClientToServiceConnectHub::GetClientToServiceConnection()
{
    auto hub = RSClientToServiceConnectHub::GetInstance();
    if (hub == nullptr) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(hub->mutex_);
    if (hub->conn_ != nullptr && hub->renderService_ != nullptr) {
        return hub->conn_;
    }
    if (!hub->Connect()) {
        ROSEN_LOGE("RSClientToServiceConnectHub connect fail");
        return nullptr;
    }
    return hub->conn_;
}

bool RSClientToServiceConnectHub::Connect()
{
    ROSEN_LOGD("RSClientToServiceConnectHub::Connect");
    sptr<IRemoteObject> remoteObject = nullptr;
    // try most 5 times to get render service.
    for (int tryCnt = 0; tryCnt < 5; ++tryCnt) {
        // sleep move time (1000us * tryCnt) when tryCnt++
        usleep(1000 * tryCnt);
        auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgr == nullptr) {
            continue;
        }
        remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
        if (remoteObject == nullptr || !remoteObject->IsProxyObject()) {
            continue;
        }
        break;
    }

    if (remoteObject == nullptr) {
        ROSEN_LOGE("RSClientToServiceConnectHub::Connect, failed to get remoteObject.");
        return false;
    }

    wptr<RSClientToServiceConnectHub> clientToServiceConnectHub = this;
    deathRecipient_ = new RenderServiceDeathRecipient(clientToServiceConnectHub);
    remoteObject->AddDeathRecipient(deathRecipient_);

    if (token_ == nullptr) {
        token_ = new IRemoteStub<RSIConnectionToken>();
    }
    ROSEN_LOGI("RSClientToServiceConnectHub Create Connection");

    // Create Connection
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    IPCObjectProxy* proxy = reinterpret_cast<IPCObjectProxy*>(remoteObject.GetRefPtr());
    std::u16string descriptor = proxy->GetInterfaceDescriptor();
    if (!data.WriteInterfaceToken(descriptor)) {
        ROSEN_LOGE("CreateConnection(): WriteInterfaceToken failed.");
        return false;
    }
    if (!data.WriteRemoteObject(token_->AsObject())) {
        ROSEN_LOGE("CreateConnection(): WriteRemoteObject failed.");
        return false;
    }
    if (!data.WriteBool(false)) {
        ROSEN_LOGE("CreateConnection(): WriteBool failed.");
        return false;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceInterfaceCode::CREATE_CONNECTION);
    int32_t err = SendRequestRemote::SendRequest(remoteObject.GetRefPtr(), code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("CreateConnection(): SendRequest failed, err is %{public}d.", err);
        return false;
    }

    auto remoteToService = reply.ReadRemoteObject();
    if (remoteToService == nullptr || !remoteToService->IsProxyObject()) {
        ROSEN_LOGE("RS CreateConnection(): Reply remoteToService is null.");
        return false;
    }

    auto conn = sptr<RSClientToServiceConnectionProxy>::MakeSptr(remoteToService);
    if (conn == nullptr) {
        ROSEN_LOGE("RSClientToServiceConnectHub::Connect, failed to iface_cast.");
        return false;
    }

    renderService_ = remoteObject;
    conn_ = conn;
    return true;
}

void RSClientToServiceConnectHub::ConnectDied()
{
    std::lock_guard<std::mutex> lock(mutex_);
    renderService_ = nullptr;
    if (conn_) {
        conn_->RunOnRemoteDiedCallback();
    }
    conn_ = nullptr;
    deathRecipient_ = nullptr;
    token_ = nullptr;
}

void RSClientToServiceConnectHub::RenderServiceDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
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