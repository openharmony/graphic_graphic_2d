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

#include "rs_render_service_connect.h"

#include <if_system_ability_manager.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "rs_render_service_proxy.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

RSRenderServiceConnect::RSRenderServiceConnect()
{
}

RSRenderServiceConnect::~RSRenderServiceConnect()
{
}

sptr<RSIRenderService> RSRenderServiceConnect::GetRenderService()
{
    std::lock_guard<std::mutex> lock(RSRenderServiceConnect::GetInstance().renderConnectlock_);

    if (RSRenderServiceConnect::GetInstance().renderService_ == nullptr) {
        bool result = RSRenderServiceConnect::GetInstance().Connect();
        if (!result) {
            ROSEN_LOGE("RenderService connect fail");
            return nullptr;
        }
    }

    return RSRenderServiceConnect::GetInstance().renderService_;
}

bool RSRenderServiceConnect::Connect()
{
    if (renderService_ != nullptr) {
        return true;
    }

    // sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    // if (samgr == nullptr) {
    //     return false;
    // }

    // sptr<IRemoteObject> remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    // if (remoteObject == nullptr) {
    //     return false;
    // }

    // if (deathRecipient_ == nullptr) {
    //     deathRecipient_ = sptr<RenderServiceDeathRecipient>(new RenderServiceDeathRecipient());
    // }

    // if ((remoteObject->IsProxyObject()) && (!remoteObject->AddDeathRecipient(deathRecipient_))) {
    // }

    // renderService_ = iface_cast<RSRenderServiceProxy>(remoteObject);
    // if (renderService_ == nullptr) {
    //     return false;
    // }

    return true;
}

void RSRenderServiceConnect::ConnectDied()
{
    std::lock_guard<std::mutex> lock(renderConnectlock_);
    renderService_ = nullptr;
    deathRecipient_ = nullptr;
}

void RSRenderServiceConnect::RenderServiceDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    RSRenderServiceConnect::GetInstance().ConnectDied();
}

} // namespace Rosen
} // namespace OHOS
