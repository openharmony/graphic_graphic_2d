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

#include <if_system_ability_manager.h>
#include <iremote_stub.h>
#include <iservice_registry.h>
#include <mutex>
#include <system_ability_definition.h>
#include <unistd.h>

#include "message_parcel.h"
#include "pipeline/rs_render_thread.h"
#include "platform/common/rs_log.h"
#include "rs_render_service_connection_proxy.h"
#include "rs_render_service_proxy.h"

namespace OHOS {
namespace Rosen {
std::once_flag RSRenderServiceConnectHub::flag_;
sptr<RSRenderServiceConnectHub> RSRenderServiceConnectHub::instance_ = nullptr;
OnConnectCallback RSRenderServiceConnectHub::onConnectCallback_ = nullptr;

sptr<RSRenderServiceConnectHub> RSRenderServiceConnectHub::GetInstance()
{
    std::call_once(flag_, &RSRenderServiceConnectHub::Init);
    return instance_;
}

void RSRenderServiceConnectHub::Init()
{
    RS_LOGD("RSRenderServiceConnectHub::Init");
    instance_ = new RSRenderServiceConnectHub();
    ::atexit(&RSRenderServiceConnectHub::Destroy);
}

void RSRenderServiceConnectHub::Destroy()
{
    RS_LOGI("RSRenderServiceConnectHub::Destroy");
    instance_ = nullptr;
}

RSRenderServiceConnectHub::RSRenderServiceConnectHub()
{
    RS_LOGD("RSRenderServiceConnectHub: ctor");
}

RSRenderServiceConnectHub::~RSRenderServiceConnectHub() noexcept
{
    if (renderService_ && renderService_->AsObject() && deathRecipient_) {
        renderService_->AsObject()->RemoveDeathRecipient(deathRecipient_);
    }
    RS_LOGI("~RSRenderServiceConnectHub: dtor");
}

sptr<RSIRenderServiceConnection> RSRenderServiceConnectHub::GetRenderService()
{
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    return connHub == nullptr ? nullptr : connHub->GetRenderServiceConnection();
}

sptr<RSIRenderServiceConnection> RSRenderServiceConnectHub::GetRenderServiceConnection()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (conn_ != nullptr && renderService_ != nullptr) {
        return conn_;
    }

    if (!Connect()) {
        ROSEN_LOGE("RenderService connect fail");
        return nullptr;
    }

    return conn_;
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
    sptr<RSIRenderServiceConnection> conn = renderService->CreateConnection(token_);

    if (conn == nullptr) {
        ROSEN_LOGD("RSRenderServiceConnectHub::Connect, failed to CreateConnection to render service.");
        return false;
    }

    renderService_ = renderService;
    conn_ = conn;

    if (onConnectCallback_) {
        onConnectCallback_(conn_);
    }

    return true;
}

void RSRenderServiceConnectHub::ConnectDied()
{
    mutex_.lock();
    RS_LOGI("RSRenderServiceConnectHub::ConnectDied lock pid: %{public}d", getpid());
    renderService_ = nullptr;
    if (conn_) {
        conn_->RunOnRemoteDiedCallback();
    }
    conn_ = nullptr;
    deathRecipient_ = nullptr;
    token_ = nullptr;
    mutex_.unlock();
    RS_LOGI("RSRenderServiceConnectHub::ConnectDied unlock pid: %{public}d", getpid());
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
} // namespace Rosen
} // namespace OHOS
