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

#include "vsync_helper_impl.h"

#include <sys/time.h>

#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "vsync_log.h"
#include "vsync_type.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "VsyncHelperImpl" };
constexpr int SEC_TO_USEC = 1000000;
}

thread_local sptr<VsyncHelperImpl> VsyncHelperImpl::currentHelper;

static int64_t GetNowTime()
{
    struct timeval tv = {};
    gettimeofday(&tv, nullptr);
    return (int64_t)tv.tv_usec + (int64_t)tv.tv_sec * SEC_TO_USEC;
}

VsyncCallback::VsyncCallback(sptr<VsyncHelperImpl>& helper) : helper_(helper)
{
}

VsyncCallback::~VsyncCallback()
{
}

void VsyncCallback::OnVsync(int64_t timestamp)
{
    if (helper_) {
        helper_->DispatchFrameCallback(timestamp);
    }
}

sptr<VsyncHelperImpl> VsyncHelperImpl::Current()
{
    if (currentHelper == nullptr) {
        auto currentRunner = AppExecFwk::EventRunner::Current();
        if (currentRunner == nullptr) {
            VLOG_FAILURE("AppExecFwk::EventRunner::Current() return nullptr");
            return nullptr;
        }

        std::shared_ptr<AppExecFwk::EventHandler> handler =
            std::make_shared<AppExecFwk::EventHandler>(currentRunner);
        currentHelper = new VsyncHelperImpl(handler);
        VLOG_SUCCESS("new VsyncHelperImpl");
    }

    return currentHelper;
}

VsyncHelperImpl::VsyncHelperImpl(std::shared_ptr<AppExecFwk::EventHandler>& handler)
{
    handler_ = handler;
}

VsyncHelperImpl::~VsyncHelperImpl()
{
}

VsyncError VsyncHelperImpl::Init()
{
    return InitSA();
}

VsyncError VsyncHelperImpl::InitSA()
{
    return InitSA(VSYNC_MANAGER_ID);
}

VsyncError VsyncHelperImpl::InitSA(int32_t systemAbilityId)
{
    if (service_ != nullptr) {
        VLOG_SUCCESS("service_ != nullptr");
        return VSYNC_ERROR_OK;
    }

    auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm == nullptr) {
        VLOG_FAILURE_RET(VSYNC_ERROR_SAMGR);
    }

    auto remoteObject = sm->GetSystemAbility(systemAbilityId);
    if (remoteObject == nullptr) {
        VLOG_FAILURE_RET(VSYNC_ERROR_SERVICE_NOT_FOUND);
    }

    service_ = iface_cast<IVsyncManager>(remoteObject);
    if (service_ == nullptr) {
        VLOG_FAILURE_RET(VSYNC_ERROR_PROXY_NOT_INCLUDE);
    }

    VLOG_SUCCESS("service_ = iface_cast");
    return VSYNC_ERROR_OK;
}

VsyncError VsyncHelperImpl::RequestFrameCallback(struct FrameCallback &cb)
{
    if (cb.callback_ == nullptr) {
        VLOG_FAILURE_RET(VSYNC_ERROR_NULLPTR);
    }

    if (service_ == nullptr) {
        VsyncError ret = Init();
        if (ret != VSYNC_ERROR_OK) {
            return ret;
        }
    }

    int64_t delayTime = cb.timestamp_;
    cb.timestamp_ += GetNowTime();

    {
        std::lock_guard<std::mutex> lockGuard(callbacks_mutex_);
        callbacks_.push(cb);
    }

    if (delayTime <= 0 && handler_->GetEventRunner()->IsCurrentRunnerThread()) {
        RequestNextVsync();
        VLOG_SUCCESS("RequestNextVsync time: " VPUBI64, delayTime);
    } else {
        handler_->PostTask([this]() { RequestNextVsync(); }, "FrameCallback", delayTime);
        VLOG_SUCCESS("PostTask time: " VPUBI64, delayTime);
    }
    return VSYNC_ERROR_OK;
}

void VsyncHelperImpl::DispatchFrameCallback(int64_t timestamp)
{
    handler_->PostTask([this, timestamp]() {
        isVsyncRequested = false;

        int64_t now = GetNowTime();
        VLOGI("DispatchFrameCallback, time: " VPUBI64 ", timestamp: " VPUBI64, now, timestamp);

        std::list<struct FrameCallback> frameCallbacks;
        {
            std::lock_guard<std::mutex> lockGuard(callbacks_mutex_);
            while (callbacks_.empty() != true) {
                if (callbacks_.top().timestamp_ <= now) {
                    frameCallbacks.push_back(callbacks_.top());
                    callbacks_.pop();
                } else {
                    break;
                }
            }
        }

        for (auto it = frameCallbacks.begin(); it != frameCallbacks.end(); it++) {
            it->callback_(timestamp, it->userdata_);
        }
    });
}

void VsyncHelperImpl::RequestNextVsync()
{
    struct FrameCallback cb;
    {
        std::lock_guard<std::mutex> lockGuard(callbacks_mutex_);
        if (isVsyncRequested || callbacks_.empty()) {
            return;
        }
        cb = callbacks_.top();
    }

    if (cb.timestamp_ <= GetNowTime()) {
        sptr<VsyncHelperImpl> helper = this;
        sptr<IVsyncCallback> ivcb = new VsyncCallback(helper);
        VsyncError ret = service_->ListenNextVsync(ivcb);
        if (ret == VSYNC_ERROR_OK) {
            isVsyncRequested = true;
            VLOG_SUCCESS("ListenNextVsync");
        } else {
            VLOG_FAILURE_API(ListenNextVsync, ret);
        }
    }
}
} // namespace OHOS
