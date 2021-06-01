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

#ifndef FRAMEWORKS_VSYNC_INCLUDE_VSYNC_MODULE_IMPL_H
#define FRAMEWORKS_VSYNC_INCLUDE_VSYNC_MODULE_IMPL_H

#include <thread>

#include <vsync_module.h>

#include "vsync_type.h"
#include "vsync_manager.h"

namespace OHOS {
class VsyncModuleImpl : public VsyncModule {
public:
    static sptr<VsyncModuleImpl> GetInstance()
    {
        static sptr<VsyncModuleImpl> ptr = new VsyncModuleImpl();
        return ptr;
    }

    VsyncError Start();
    VsyncError Stop();

protected:
    virtual VsyncError InitSA();

    VsyncError InitSA(int32_t vsyncSystemAbilityId);

private:
    VsyncModuleImpl();
    virtual ~VsyncModuleImpl();

    void VsyncMainThread();
    bool RegisterSystemAbility();
    void UnregisterSystemAbility();
    int64_t WaitNextVBlank();

    int32_t drmFd_;
    std::unique_ptr<std::thread> vsyncThread_;
    bool vsyncThreadRunning_;
    int32_t vsyncSystemAbilityId_;
    bool isRegisterSA_;
    sptr<VsyncManager> vsyncManager_;
};
} // namespace OHOS

#endif // FRAMEWORKS_VSYNC_INCLUDE_VSYNC_MODULE_IMPL_H
