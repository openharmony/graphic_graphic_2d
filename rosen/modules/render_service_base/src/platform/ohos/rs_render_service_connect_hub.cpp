/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "rs_render_service_connect_hub.h"

#include "app_image_observer_manager.h"
#include <if_system_ability_manager.h>
#include <iremote_stub.h>
#include <iservice_registry.h>
#include <mutex>
#include <system_ability_definition.h>
#include <unistd.h>
#include <thread>

#include "message_parcel.h"
#include "transaction/zidl/rs_client_to_service_connection_proxy.h"
#include "transaction/zidl/rs_client_to_render_connection_proxy.h"
#include "rs_render_service_proxy.h"
#include "pipeline/rs_render_thread.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
std::once_flag RSRenderServiceConnectHub::flag_;
sptr<RSRenderServiceConnectHub> RSRenderServiceConnectHub::instance_ = nullptr;
OnConnectCallback RSRenderServiceConnectHub::onConnectCallback_ = nullptr;
constexpr int32_t TOKEN_STRONG_REF_COUNT = 1;
constexpr int32_t WAIT_TIME_FOR_DEC_STRONG_REF = 50;

sptr<RSRenderServiceConnectHub> RSRenderServiceConnectHub::GetInstance()
{
    std::call_once(flag_, &RSRenderServiceConnectHub::Init);
    return instance_;
}

void RSRenderServiceConnectHub::Init()
{
    instance_ = new RSRenderServiceConnectHub();
    ::atexit(&RSRenderServiceConnectHub::Destroy);
}

void RSRenderServiceConnectHub::Destroy()
{
    instance_ = nullptr;
}

RSRenderServiceConnectHub::RSRenderServiceConnectHub()
{
}

void RSRenderServiceConnectHub::SetOnDiedCallback(RSOnDiedCallbackCode code, std::function<void()> cb)
{
    auto instance = RSRenderServiceConnectHub::GetInstance();
    std::lock_guard<std::mutex> lock(instance->onDiedCallbacksMutex_);
    instance->OnDiedCallbacks_[static_cast<int32_t>(code)] = cb;
    ROSEN_LOGI("RSRenderServiceConnectHub::SetOnDiedCallback, code:%{public}d", code);
}

void RSRenderServiceConnectHub::RemoveOnDiedCallback(RSOnDiedCallbackCode code, bool isDestreuctionProcess)
{
    if (isDestreuctionProcess) {
        return;
    }
    auto instance = RSRenderServiceConnectHub::GetInstance();
    std::lock_guard<std::mutex> lock(instance->onDiedCallbacksMutex_);
    instance->OnDiedCallbacks_.erase(static_cast<int32_t>(code));
    ROSEN_LOGI("RSRenderServiceConnectHub::RemoveOnDiedCallback, code:%{public}d", code);
}

void RSRenderServiceConnectHub::ExecuteAndClearDiedCallbacks()
{
    std::unordered_map<int32_t, std::function<void()>> callbacks;
    {
        std::lock_guard<std::mutex> lock(onDiedCallbacksMutex_);
        callbacks = std::move(OnDiedCallbacks_);
    }
    for (auto& [code, cb] : callbacks) {
        if (cb) {
            cb();
        } else {
            ROSEN_LOGW("ExecuteAndClearDiedCallbacks callback is null, code:%{public}d", code);
        }
    }
}

RSRenderServiceConnectHub::~RSRenderServiceConnectHub() noexcept
{
    if (UNLIKELY(!renderService_)) {
        ROSEN_LOGE(
            "~RSRenderServiceConnectHub Remove connection token and deathRecipient_ failed, renderService_ is null");
        return;
    }
    ROSEN_LOGI("~RSRenderServiceConnectHub");
    if (renderService_->AsObject() && deathRecipient_) {
        renderService_->AsObject()->RemoveDeathRecipient(deathRecipient_);
    }

    if (token_ == nullptr) {
        ROSEN_LOGI("token_ is deleted");
        return;
    }
    ROSEN_LOGI("RSRenderServiceConnectHub::RefCount: token_:%{public}d, conn_:%{public}d, renderConn_:%{public}d",
        token_->GetSptrRefCount(), conn_->GetSptrRefCount(), renderConn_->GetSptrRefCount());
    ExecuteAndClearDiedCallbacks();
    while (token_->GetSptrRefCount() != TOKEN_STRONG_REF_COUNT) {
        token_->DecStrongRef(this);
    }
    CleanConnectRenderProcess();
    renderService_->RemoveConnection(token_);
    token_ = nullptr;
    conn_ = nullptr;
    renderConn_ = nullptr;
    ROSEN_LOGI("~RSRenderServiceConnectHub end");
}

std::pair<sptr<RSIClientToServiceConnection>, sptr<RSIClientToRenderConnection>>
    RSRenderServiceConnectHub::GetRenderService()
{
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    return connHub == nullptr ? std::make_pair(nullptr, nullptr) : connHub->GetRenderServiceConnection();
}

sptr<RSIClientToServiceConnection> RSRenderServiceConnectHub::GetClientToServiceConnection()
{
    return GetRenderService().first;
}

uint64_t RSRenderServiceConnectHub::GetDefaultTokenMaskIdInner()
{
    std::unique_lock<std::mutex> lock(renderPipelineClientMutex_);
    for (auto [tokenMaskId, renderProcessInfo] : connRenderProcesses_) {
        if (tokenMaskId != INVALID_TOKEN_MASK_ID) {
            return tokenMaskId;
        }
    }
    return INVALID_TOKEN_MASK_ID;
}

uint64_t RSRenderServiceConnectHub::GetDefaultTokenMaskId()
{
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    return connHub->GetDefaultTokenMaskIdInner();
}

sptr<RSIClientToRenderConnection> RSRenderServiceConnectHub::FindClientToRenderConnection(uint64_t tokenMaskId)
{
    std::unique_lock<std::mutex> lock(renderPipelineClientMutex_);
    auto iter = connRenderProcesses_.find(tokenMaskId);
    if (iter == connRenderProcesses_.end()) {
        ROSEN_LOGW("RSRenderServiceConnectHub::SetOnRenderProcessDiedCallback "
            "tokenMaskId is not find ClientToRenderConnection");
        return nullptr;
    }
    auto renderProcessInfo = iter->second;
    return renderProcessInfo.clientToRenderConnection;
}

sptr<RSIClientToRenderConnection> RSRenderServiceConnectHub::GetClientToRenderConnection(uint64_t tokenMaskId)
{
    static bool isUniRender = RSSystemProperties::GetUniRenderEnabled();
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    if (connHub == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectHub::GetClientToRenderConnection connHub is nullptr");
        return nullptr;
    }
    auto conn = connHub->FindClientToRenderConnection(tokenMaskId);
    if (isUniRender && conn) {
        return conn;
    }
    ROSEN_LOGI("RSRenderServiceConnectHub::GetClientToRenderConnection Trigger Guarantee mechanism");
    return GetRenderService().second;
}

uint64_t RSRenderServiceConnectHub::FindTokenMaskIdByRenderRemote(sptr<IRemoteObject>& connectToRenderRemote)
{
    std::unique_lock<std::mutex> lock(renderPipelineClientMutex_);
    for (auto [tokenMaskId, renderProcessInfo] : connRenderProcesses_) {
        if (connectToRenderRemote == renderProcessInfo.connectToRenderRemote &&
            renderProcessInfo.clientToRenderConnection != nullptr) {
            return tokenMaskId;
        }
    }
    return INVALID_TOKEN_MASK_ID;
}

uint64_t RSRenderServiceConnectHub::GetRenderProcessTokenMaskId(sptr<IRemoteObject>& connectToRenderRemote)
{
    if (connectToRenderRemote == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectHub::%{public}s connectToRenderRemote is nullptr", __func__);
        return INVALID_TOKEN_MASK_ID;
    }
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    bool needRefresh = AppExecFwk::AppImageObserverManager::GetInstance().IsBeforeImageCreationPoint();

    auto oldTokenMaskId = connHub->FindTokenMaskIdByRenderRemote(connectToRenderRemote);
    if (oldTokenMaskId != INVALID_TOKEN_MASK_ID) {
        return oldTokenMaskId;
    }
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    uint64_t tokenMaskId =
        ((static_cast<uint64_t>(GetRealPid())) << 32) |
            static_cast<uint64_t>(reinterpret_cast<unsigned long int>(token.GetRefPtr()));

    auto renderProcess = iface_cast<RSIConnectToRenderProcess>(connectToRenderRemote);
    if (renderProcess == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectHub::%{public}s, iface_cast failed", __func__);
        return INVALID_TOKEN_MASK_ID;
    }

    auto [clientToRenderConnection, rpTokenMaskId] = renderProcess->CreateRenderConnection(
        tokenMaskId, token, needRefresh);
    connHub->AddRenderProcessConnectionToken(rpTokenMaskId, token,
        connectToRenderRemote, renderProcess, clientToRenderConnection);

    return rpTokenMaskId;
}


std::pair<sptr<RSIClientToServiceConnection>, sptr<RSIClientToRenderConnection>>
    RSRenderServiceConnectHub::GetRenderServiceConnection()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (conn_ != nullptr && renderConn_ != nullptr && renderService_ != nullptr) {
        return {conn_, renderConn_};
    }

    if (!Connect()) {
        ROSEN_LOGE("RenderService connect fail");
        return {nullptr, nullptr};
    }

    return {conn_, renderConn_};
}

bool RSRenderServiceConnectHub::Connect()
{
    RS_LOGD("RSRenderServiceConnectHub::Connect");
    int tryCnt = 0;
    sptr<RSIRenderService> renderService = nullptr;
    do {
        // sleep move time (1000us * tryCnt) when tryCnt++
        usleep(1000 * tryCnt);
        ++tryCnt;
        // try most 5 times to get render service.
        if (tryCnt == 5) {
            ROSEN_LOGD("RSRenderServiceConnectHub::Connect failed, tried %{public}d times.", tryCnt);
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
        ROSEN_LOGD("RSRenderServiceConnectHub::Connect, failed to get render service proxy.");
        return false;
    }
    wptr<RSRenderServiceConnectHub> rsConnhub = this;
    deathRecipient_ = new RenderServiceDeathRecipient(rsConnhub);
    if (!renderService->AsObject()->AddDeathRecipient(deathRecipient_)) {
        ROSEN_LOGW("RSRenderServiceConnectHub::Connect, failed to AddDeathRecipient of render service.");
    }

    if (token_ == nullptr) {
        token_ = new IRemoteStub<RSIConnectionToken>();
    }
    bool needRefresh = AppExecFwk::AppImageObserverManager::GetInstance().IsBeforeImageCreationPoint();
    ROSEN_LOGI("RSRenderServiceConnectHub call CreateConnection, needRefresh:[%{public}d]", needRefresh);
    auto [conn, renderConn] = renderService->CreateConnection(token_, needRefresh);

    if (conn == nullptr || renderConn == nullptr) {
        ROSEN_LOGD("RSRenderServiceConnectHub::Connect, failed to CreateConnection to render service.");
        return false;
    }

    renderService_ = renderService;
    conn_ = conn;
    renderConn_ = renderConn;

    if (onConnectCallback_) {
        onConnectCallback_(renderConn_);
    }

    return true;
}

void RSRenderServiceConnectHub::ConnectDied()
{
    mutex_.lock();
    renderService_ = nullptr;
    if (conn_) {
        conn_->RunOnRemoteDiedCallback();
    }
    conn_ = nullptr;
    renderConn_ = nullptr;
    deathRecipient_ = nullptr;
    token_ = nullptr;
    mutex_.unlock();
}

void RSRenderServiceConnectHub::RenderServiceDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    auto remoteSptr = remote.promote();
    if (remoteSptr == nullptr) {
        ROSEN_LOGW("RSRenderServiceConnectHub::RenderServiceDeathRecipient::OnRemoteDied: can't promote.");
        return;
    }

    auto rsConnHub = rsConnHub_.promote();
    if (rsConnHub == nullptr) {
        ROSEN_LOGW("RSRenderServiceConnectHub::RenderServiceDeathRecipient::OnRemoteDied: connHub was dead.");
        return;
    }

    rsConnHub->ConnectDied();
}

void RSRenderServiceConnectHub::SetOnRenderProcessDiedCallback(uint64_t tokenMaskId,
    std::function<void()> callback)
{
    std::unique_lock<std::mutex> lock(renderPipelineClientMutex_);
    auto iter = connRenderProcesses_.find(tokenMaskId);
    if (iter == connRenderProcesses_.end()) {
        ROSEN_LOGW("RSRenderServiceConnectHub::SetOnRenderProcessDiedCallback tokenMaskId is Error");
        return;
    }

    RenderProcessInfo& renderProcessInfo = iter->second;
    if (renderProcessInfo.deathRecipient == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectHub::SetOnRenderProcessDiedCallback Error deathRecipient");
        return;
    }
    renderProcessInfo.deathRecipient->SetOnRenderProcessDiedCallback(callback);
}


void RSConnectRenderProcessDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    auto remoteSptr = remote.promote();
    if (remoteSptr == nullptr) {
        ROSEN_LOGW("RSConnectRenderProcessDeathRecipient::OnRemoteDied can't promote.");
        return;
    }

    auto rsConnHub = rsConnHub_.promote();
    if (rsConnHub == nullptr) {
        ROSEN_LOGW("RSConnectRenderProcessDeathRecipient::OnRemoteDied connHub was dead.");
        return;
    }

    if (callback_) {
        callback_();
    }
    rsConnHub->ConnectRenderProcessDied(tokenMaskId_);
}

void RSRenderServiceConnectHub::ConnectRenderProcessDied(uint64_t tokenMaskId)
{
    std::unique_lock<std::mutex> lock(renderPipelineClientMutex_);
    connRenderProcesses_.erase(tokenMaskId);
}

void RSRenderServiceConnectHub::AddRenderProcessConnectionToken(uint64_t tokenMaskId, sptr<RSIConnectionToken> token,
    sptr<IRemoteObject> connectToRenderRemote,
    sptr<RSIConnectToRenderProcess> renderPrecess,
    sptr<RSIClientToRenderConnection> clientToRenderConnection)
{
    if (token == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectHub::AddRenderProcessConnectionToken token is nullptr");
        return;
    }
    std::unique_lock<std::mutex> lock(renderPipelineClientMutex_);
    if (connRenderProcesses_.find(tokenMaskId) != connRenderProcesses_.end()) {
        return;
    }
    ROSEN_LOGI("RSRenderServiceConnectHub::AddRenderProcessConnectionToken");
    wptr<RSRenderServiceConnectHub> rsConnHub = this;
    sptr<RSConnectRenderProcessDeathRecipient> deathRecipient = new RSConnectRenderProcessDeathRecipient(rsConnHub,
        tokenMaskId);
    
    if (!renderPrecess->AsObject()->AddDeathRecipient(deathRecipient)) {
        ROSEN_LOGW("RSRenderServiceConnectHub::AddRenderProcessConnectionToken,"
            "failed to AddDeathRecipient of render service.");
    }
    RenderProcessInfo renderProcessInfo = {
        .token = token,
        .connectToRenderRemote = connectToRenderRemote,
        .renderProcess = renderPrecess,
        .deathRecipient = deathRecipient,
        .clientToRenderConnection = clientToRenderConnection
    };
    connRenderProcesses_[tokenMaskId] = renderProcessInfo;
}

void RSRenderServiceConnectHub::RemoveRenderProcessDeathRecipient(uint64_t tokenMaskId,
    sptr<RSIConnectToRenderProcess> renderProcess)
{
    auto iter = connRenderProcesses_.find(tokenMaskId);
    if (iter == connRenderProcesses_.end()) {
        ROSEN_LOGE("RSRenderServiceConnectHub::RemoveRenderProcessDeathRecipient Error not find tokenMaskId");
        return;
    }
    RenderProcessInfo& renderProcessInfo = iter->second;
    if (renderProcessInfo.renderProcess == nullptr ||
        !renderProcessInfo.renderProcess->AsObject() ||
        renderProcessInfo.deathRecipient == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectHub::RemoveRenderProcessDeathRecipient Error");
        return;
    }
    renderProcess->AsObject()->RemoveDeathRecipient(renderProcessInfo.deathRecipient);
}

void RSRenderServiceConnectHub::CleanConnectRenderProcess()
{
    std::unique_lock<std::mutex> lock(renderPipelineClientMutex_);
    if (connRenderProcesses_.size() == 0) {
        ROSEN_LOGI("RSRenderServiceConnectHub::CleanConnectRenderProcess connection already release");
        return;
    }

    for (auto iter = connRenderProcesses_.begin(); iter != connRenderProcesses_.end();) {
        RenderProcessInfo& renderProcessInfo = iter->second;
        if (renderProcessInfo.token == nullptr) {
            ROSEN_LOGE("RSRenderServiceConnectHub::CleanConnectRenderProcess token is nullptr");
            iter = connRenderProcesses_.erase(iter);
            continue;
        }
        sptr<RSIConnectToRenderProcess> renderProcess = renderProcessInfo.renderProcess;

        uint64_t tokenMaskId = iter->first;
        RemoveRenderProcessDeathRecipient(tokenMaskId, renderProcess);
        // Token DecStrongRe, Avoid the token destruction returned by server, which may cause crash.
        while (renderProcessInfo.token->GetSptrRefCount() != TOKEN_STRONG_REF_COUNT) {
            renderProcessInfo.token->DecStrongRef(renderProcessInfo.token.GetRefPtr());
        }
        iter = connRenderProcesses_.erase(iter);
        if (renderProcess != nullptr) {
            renderProcess->RemoveConnection(tokenMaskId);
        }
    }
}

} // namespace Rosen
} // namespace OHOS
