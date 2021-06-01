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

#include "vsync_module_impl.h"

#include <unistd.h>
#include <xf86drm.h>
#include <chrono>

#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "vsync_log.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "VsyncModuleImpl" };
constexpr int USLEEP_TIME = 100 * 1000;
constexpr int RETRY_TIMES = 5;
}

int64_t VsyncModuleImpl::WaitNextVBlank()
{
    drmVBlank vblank = {
        .request = drmVBlankReq {
            .type = DRM_VBLANK_RELATIVE,
            .sequence = 1,
        }
    };

    int ret = drmWaitVBlank(drmFd_, &vblank);
    if (ret != 0) {
        VLOG_ERROR_API(errno, drmWaitVBlank);
        return -1;
    }

    // uptime
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return (int64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}

void VsyncModuleImpl::VsyncMainThread()
{
    while (vsyncThreadRunning_) {
        vsyncManager_->CheckVsyncRequest();
        if (!vsyncThreadRunning_) {
            break;
        }
        int64_t timestamp = WaitNextVBlank();
        if (timestamp < 0) {
            continue;
        }
        vsyncManager_->Callback(timestamp);
    }
}

bool VsyncModuleImpl::RegisterSystemAbility()
{
    if (isRegisterSA_) {
        return true;
    }
    auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm) {
        sm->AddSystemAbility(vsyncSystemAbilityId_, vsyncManager_);
        isRegisterSA_ = true;
    }
    return isRegisterSA_;
}

void VsyncModuleImpl::UnregisterSystemAbility()
{
    auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm) {
        sm->RemoveSystemAbility(vsyncSystemAbilityId_);
        isRegisterSA_ = false;
    }
}

VsyncModuleImpl::VsyncModuleImpl()
{
    VLOGD("DEBUG VsyncModuleImpl");
    drmFd_ = -1;
    vsyncThread_ = nullptr;
    vsyncThreadRunning_ = false;
    vsyncSystemAbilityId_ = 0;
    isRegisterSA_ = false;
    vsyncManager_ = new VsyncManager();
}

VsyncModuleImpl::~VsyncModuleImpl()
{
    VLOGD("DEBUG ~VsyncModuleImpl");
    if (vsyncThreadRunning_) {
        Stop();
    }

    if (isRegisterSA_) {
        UnregisterSystemAbility();
    }
}

VsyncError VsyncModuleImpl::Start()
{
    VsyncError ret = InitSA();
    if (ret != VSYNC_ERROR_OK) {
        return ret;
    }

    drmFd_ = drmOpen(DRM_MODULE_NAME, nullptr);
    if (drmFd_ < 0) {
        VLOG_ERROR_API(errno, drmOpen);
        return VSYNC_ERROR_API_FAILED;
    }
    VLOGD("drmOpen fd is %{public}d", drmFd_);

    vsyncThreadRunning_ = true;
    vsyncThread_ = std::make_unique<std::thread>([this]()->void {
        VsyncMainThread();
    });

    return VSYNC_ERROR_OK;
}

VsyncError VsyncModuleImpl::Stop()
{
    if (!vsyncThreadRunning_) {
        return VSYNC_ERROR_INVALID_OPERATING;
    }

    vsyncThreadRunning_ = false;
    vsyncManager_->StopCheck();
    vsyncThread_->join();
    vsyncThread_.reset();

    int ret = drmClose(drmFd_);
    if (ret) {
        VLOG_ERROR_API(errno, drmClose);
        return VSYNC_ERROR_API_FAILED;
    }
    drmFd_ = -1;

    if (isRegisterSA_) {
        UnregisterSystemAbility();
    }

    return VSYNC_ERROR_OK;
}

VsyncError VsyncModuleImpl::InitSA()
{
    return InitSA(VSYNC_MANAGER_ID);
}

VsyncError VsyncModuleImpl::InitSA(int32_t vsyncSystemAbilityId)
{
    vsyncSystemAbilityId_ = vsyncSystemAbilityId;

    int tryCount = 0;
    while (!RegisterSystemAbility()) {
        if (tryCount++ >= RETRY_TIMES) {
            VLOGE("RegisterSystemAbility failed after %{public}d tries!!!", RETRY_TIMES);
            return VSYNC_ERROR_SERVICE_NOT_FOUND;
        } else {
            VLOGE("RegisterSystemAbility failed, try again:%{public}d", tryCount);
            usleep(USLEEP_TIME);
        }
    }

    return VSYNC_ERROR_OK;
}
} // namespace OHOS
