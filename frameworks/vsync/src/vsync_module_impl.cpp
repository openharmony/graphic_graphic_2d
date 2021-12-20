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


#include <chrono>
#include <mutex>
#include <thread>
#include <unistd.h>

#include <graphic_bytrace.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "vsync_log.h"
#include "vsync_module_impl.h"

using namespace std::chrono_literals;

namespace OHOS {
namespace Vsync {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "VsyncModuleImpl" };
}

sptr<VsyncModuleImpl> VsyncModuleImpl::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new VsyncModuleImpl();
        }
    }
    return instance;
}

VsyncError VsyncModuleImpl::Start()
{
    VLOGI("Start");
    VsyncError ret = InitSA();
    if (ret != VSYNC_ERROR_OK) {
        VLOG_FAILURE("Start");
        return ret;
    }

    vsyncThreadRunning_ = true;
    vsyncThread_ = std::make_unique<std::thread>(std::bind(&VsyncModuleImpl::VsyncMainThread, this));
    VLOG_SUCCESS("Start");
    return VSYNC_ERROR_OK;
}

VsyncError VsyncModuleImpl::Trigger()
{
    if (IsRunning() == false) {
        VLOG_FAILURE("Trigger");
        return VSYNC_ERROR_INVALID_OPERATING;
    }

    const auto &now = std::chrono::steady_clock::now().time_since_epoch();
    int64_t occurTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();

    std::lock_guard<std::mutex> lock(promisesMutex_);
    constexpr int32_t overwriteTop = 10;
    if (promises_.size() < overwriteTop) {
        promisesSem_.Inc();
    } else {
        promises_.pop();
    }
    promises_.push(occurTimestamp);
    return VSYNC_ERROR_OK;
}

VsyncError VsyncModuleImpl::Stop()
{
    VLOGI("Stop");
    if (vsyncThreadRunning_ == false) {
        VLOG_FAILURE_RET(VSYNC_ERROR_INVALID_OPERATING);
    }

    vsyncThreadRunning_ = false;
    vsyncThread_->join();
    vsyncThread_.reset();

    if (isRegisterSA_ == true) {
        UnregisterSystemAbility();
    }
    VLOG_SUCCESS("Stop");
    return VSYNC_ERROR_OK;
}

bool VsyncModuleImpl::IsRunning()
{
    return vsyncThreadRunning_;
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
        constexpr int retryTimes = 5;
        if (tryCount++ >= retryTimes) {
            VLOGE("RegisterSystemAbility failed after %{public}d tries!!!", retryTimes);
            return VSYNC_ERROR_SERVICE_NOT_FOUND;
        } else {
            VLOGE("RegisterSystemAbility failed, try again:%{public}d", tryCount);
            std::this_thread::sleep_for(100ms);
        }
    }

    return VSYNC_ERROR_OK;
}

VsyncModuleImpl::~VsyncModuleImpl()
{
    VLOGI("~VsyncModuleImpl");
    Stop();
}

int64_t VsyncModuleImpl::WaitNextVsync()
{
    promisesSem_.Dec();
    std::lock_guard<std::mutex> lock(promisesMutex_);
    const auto &ret = promises_.front();
    promises_.pop();
    return ret;
}

void VsyncModuleImpl::VsyncMainThread()
{
    while (IsRunning()) {
        int64_t timestamp = WaitNextVsync();
        if (timestamp < 0) {
            VLOGE("WaitNextVsync return negative time");
            continue;
        }
        ScopedBytrace vsyncSending("VsyncSending");
        vsyncManager_.Callback(timestamp);
    }
}

bool VsyncModuleImpl::RegisterSystemAbility()
{
    if (isRegisterSA_ == true) {
        return true;
    }
    auto sam = DrmModule::GetInstance()->GetSystemAbilityManager();
    if (sam) {
        if (sam->AddSystemAbility(vsyncSystemAbilityId_, &vsyncManager_) != ERR_OK) {
            VLOGW("AddSystemAbility failed");
        } else {
            isRegisterSA_ = true;
        }
    }
    return isRegisterSA_;
}

void VsyncModuleImpl::UnregisterSystemAbility()
{
    auto sam = DrmModule::GetInstance()->GetSystemAbilityManager();
    if (sam) {
        sam->RemoveSystemAbility(vsyncSystemAbilityId_);
        isRegisterSA_ = false;
    }
}
} // namespace Vsync
} // namespace OHOS
