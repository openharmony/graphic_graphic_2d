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

#include "drm_module.h"

#include <mutex>

namespace OHOS {
namespace Vsync {
sptr<DrmModule> DrmModule::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new DrmModule();
        }
    }

    return instance;
}

void DrmModule::SetInstance(sptr<DrmModule> &mockInstance)
{
    instance = mockInstance;
}

sptr<ISystemAbilityManager> DrmModule::GetSystemAbilityManager()
{
    return SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
}

sptr<IRemoteObject> DrmModule::GetSystemAbility(sptr<ISystemAbilityManager>& sm, int32_t systemAbilityId)
{
    return sm->GetSystemAbility(systemAbilityId);
}

int DrmModule::DrmOpen(std::string name, std::string busid)
{
    return drmOpen(name.data(), busid.data());
}
int DrmModule::DrmClose(int32_t drmFd_)
{
    return drmClose(drmFd_);
}
int DrmModule::DrmWaitBlank(int32_t drmFd, drmVBlank vblank)
{
    return drmWaitVBlank(drmFd, &vblank);
}
} // namespace Vsync
} // namespace OHOS
