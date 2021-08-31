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
#ifndef FRAMEWORKS_VSYNC_INCLUDE_DRM_MODULE_H
#define FRAMEWORKS_VSYNC_INCLUDE_DRM_MODULE_H

#include <refbase.h>
#include <iservice_registry.h>
#include <xf86drm.h>

namespace OHOS {
namespace Vsync {
class DrmModule : public RefBase {
public:
    static sptr<DrmModule> GetInstance();
    static void SetInstance(sptr<DrmModule>& mockInstance);

    virtual sptr<ISystemAbilityManager> GetSystemAbilityManager();
    virtual sptr<IRemoteObject> GetSystemAbility(sptr<ISystemAbilityManager>& sm, int32_t systemAbilityId);
    virtual int DrmOpen(std::string name, std::string busid);
    virtual int DrmClose(int32_t drmFd_);
    virtual int DrmWaitBlank(int32_t drmFd, drmVBlank vblank);
    
private:
    DrmModule() = default;
    virtual ~DrmModule() = default;
    static inline sptr<DrmModule> instance = nullptr;
};
} // namespace Vsync
} // namespace OHOS

#endif // FRAMEWORKS_VSYNC_INCLUDE_DRM_MODULE_H
