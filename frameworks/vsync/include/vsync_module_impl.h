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

#include <queue>
#include <thread>

#include <local_semaphore.h>
#include <vsync_module.h>

#include "drm_module.h"
#include "vsync_manager.h"

namespace OHOS {
namespace Vsync {
class VsyncModuleImpl : public VsyncModule {
public:
    static sptr<VsyncModuleImpl> GetInstance();

    virtual VsyncError Start() override;
    virtual VsyncError Trigger() override;
    virtual VsyncError Stop() override;
    virtual bool IsRunning() override;

protected:
    virtual VsyncError InitSA();
    VsyncError InitSA(int32_t vsyncSystemAbilityId);

private:
    VsyncModuleImpl() = default;
    virtual ~VsyncModuleImpl() override;
    static inline sptr<VsyncModuleImpl> instance = nullptr;

    int64_t WaitNextVsync();
    void VsyncMainThread();
    bool RegisterSystemAbility();
    void UnregisterSystemAbility();

    LocalSemaphore promisesSem_;
    std::mutex promisesMutex_;
    std::queue<int64_t> promises_;
    std::unique_ptr<std::thread> vsyncThread_ = nullptr;
    bool vsyncThreadRunning_ = false;
    int32_t vsyncSystemAbilityId_ = 0;
    bool isRegisterSA_ = false;
    VsyncManager vsyncManager_;
};
} // namespace Vsync
} // namespace OHOS

#endif // FRAMEWORKS_VSYNC_INCLUDE_VSYNC_MODULE_IMPL_H
