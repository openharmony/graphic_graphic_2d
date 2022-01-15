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

#include "static_call.h"

#include <mutex>

namespace OHOS {
namespace Vsync {
sptr<StaticCall> StaticCall::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new StaticCall();
        }
    }

    return instance;
}

void StaticCall::SetInstance(sptr<StaticCall> &mockInstance)
{
    instance = mockInstance;
}

sptr<IVsyncManager> StaticCall::GetCast(sptr<IRemoteObject>& remoteObject)
{
    auto cast = iface_cast<IVsyncManager>(remoteObject);
    return cast;
}

sptr<ISystemAbilityManager> StaticCall::GetSystemAbilityManager()
{
    return SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
}

sptr<IRemoteObject> StaticCall::GetSystemAbility(sptr<ISystemAbilityManager>& sm, int32_t systemAbilityId)
{
    return sm->GetSystemAbility(systemAbilityId);
}

GSError StaticCall::GetVsyncFrequency(sptr<IVsyncManager>& server, uint32_t &freq)
{
    return server->GetVsyncFrequency(freq);
}

GSError StaticCall::ListenVsync(sptr<IVsyncManager>& server, sptr<IVsyncCallback>& cb, int32_t &cbid)
{
    return server->ListenVsync(cb, cbid);
}

void StaticCall::Sync(int64_t, void *data)
{
}

std::shared_ptr<AppExecFwk::EventHandler> StaticCall::Current()
{
    return AppExecFwk::EventHandler::Current();
}
} // namespace Vsync
} // namespace OHOS
