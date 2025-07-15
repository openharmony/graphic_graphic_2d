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

#include "vsync_receiver.h"
#include <memory>
#include <mutex>
#include <unistd.h>
#include <scoped_bytrace.h>
#include <fcntl.h>
#include <hitrace_meter.h>

#include "accesstoken_kit.h"
#include "event_handler.h"
#include "graphic_common.h"
#include "ipc_skeleton.h"
#include "parameters.h"
#include "rs_frame_report_ext.h"
#include "vsync_log.h"
#include "sandbox_utils.h"
#include <rs_trace.h>
#include "qos.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t INVALID_FD = -1;
static const int32_t APP_VSYNC_PRIORITY = system::GetIntParameter("const.graphic.app_vsync_priority", -1);
}
VSyncReceiver::VSyncReceiver(const sptr<IVSyncConnection>& conn,
    const sptr<IRemoteObject>& token,
    const std::shared_ptr<OHOS::AppExecFwk::EventHandler>& looper,
    const std::string& name)
    : connection_(conn), token_(token), looper_(looper),
    listener_(std::make_shared<VSyncCallBackListener>()),
    init_(false),
    fd_(INVALID_FD),
    name_(name)
{
};

void VSyncReceiver::RegisterFileDescriptorListener(bool hasVsyncThread)
{
    auto selfToken = IPCSkeleton::GetSelfTokenID();
    auto tokenType = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(static_cast<uint32_t>(selfToken));
    bool isAppMainThread = (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) && (getpid() == gettid());
    if (!hasVsyncThread && isAppMainThread &&
        (static_cast<int32_t>(AppExecFwk::EventQueue::Priority::VIP) <= APP_VSYNC_PRIORITY) &&
        (static_cast<int32_t>(AppExecFwk::EventQueue::Priority::IDLE) >= APP_VSYNC_PRIORITY)) {
        listener_->SetDeamonWaiter();
        listener_->SetType(AppExecFwk::FileDescriptorListener::ListenerType::LTYPE_VSYNC);
        looper_->AddFileDescriptorListener(fd_, AppExecFwk::FILE_DESCRIPTOR_INPUT_EVENT, listener_, "vSyncTask",
            static_cast<AppExecFwk::EventQueue::Priority>(APP_VSYNC_PRIORITY));
    } else {
        looper_->AddFileDescriptorListener(fd_, AppExecFwk::FILE_DESCRIPTOR_INPUT_EVENT, listener_, "vSyncTask");
    }
}

VsyncError VSyncReceiver::Init()
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (init_) {
        return VSYNC_ERROR_OK;
    }
    if (connection_ == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }

    bool hasVsyncThread = false;
    if (looper_ == nullptr) {
        std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("OS_VSyncThread");
        if (runner == nullptr) {
            return VSYNC_ERROR_API_FAILED;
        }
        looper_ = std::make_shared<AppExecFwk::EventHandler>(runner);
        runner->Run();
        looper_->PostTask([] {
            SetThreadQos(QOS::QosLevel::QOS_USER_INTERACTIVE);
        });
        hasVsyncThread = true;
    }
    VsyncError ret = connection_->GetReceiveFd(fd_);
    if (ret != VSYNC_ERROR_OK) {
        return ret;
    }

    int32_t retVal = fcntl(fd_, F_SETFL, O_NONBLOCK); // set fd to NonBlock mode
    if (retVal != 0) {
        VLOGW("%{public}s fcntl set fd_:%{public}d NonBlock failed, retVal:%{public}d, errno:%{public}d",
            __func__, fd_, retVal, errno);
    }

    listener_->SetName(name_);
    listener_->RegisterFdShutDownCallback([this](int32_t fileDescriptor) {
        std::lock_guard<std::mutex> locker(initMutex_);
        if (fileDescriptor != fd_) {
            VLOGE("OnShutdown Invalid fileDescriptor:%{public}d, fd_:%{public}d", fileDescriptor, fd_);
            return;
        }
        RemoveAndCloseFdLocked();
    });
    listener_->RegisterReadableCallback([this](int32_t fileDescriptor) -> bool {
        std::lock_guard<std::mutex> locker(initMutex_);
        if (fileDescriptor != fd_) {
            VLOGE("OnReadable Invalid fileDescriptor:%{public}d, fd_:%{public}d", fileDescriptor, fd_);
            return false;
        }
        return true;
    });

    RegisterFileDescriptorListener(hasVsyncThread);
    init_ = true;
    return VSYNC_ERROR_OK;
}

VSyncReceiver::~VSyncReceiver()
{
    listener_->RegisterFdShutDownCallback(nullptr);
    listener_->RegisterReadableCallback(nullptr);
    std::lock_guard<std::mutex> locker(initMutex_);
    RemoveAndCloseFdLocked();
    DestroyLocked();
}

void VSyncReceiver::RemoveAndCloseFdLocked()
{
    if (looper_ != nullptr) {
        looper_->RemoveFileDescriptorListener(fd_);
        VLOGI("%{public}s looper remove fd listener, fd=%{public}d", __func__, fd_);
    }

    std::lock_guard<std::mutex> locker(listener_->fdMutex_);
    if (fd_ >= 0) {
        close(fd_);
        listener_->SetFdClosedFlagLocked(true);
        fd_ = INVALID_FD;
    }
}

VsyncError VSyncReceiver::RequestNextVSync(FrameCallback callback)
{
    return RequestNextVSync(callback, "unknown", 0);
}

VsyncError VSyncReceiver::RequestNextVSync(
    FrameCallback callback, const std::string& fromWhom, int64_t lastVSyncTS, const int64_t& requestVsyncTime)
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (!init_) {
        VLOGE("%{public}s not init", __func__);
        return VSYNC_ERROR_NOT_INIT;
    }
    listener_->SetCallback(callback);
    listener_->SetRNVFlag(true);
    ScopedDebugTrace func("VSyncReceiver::RequestNextVSync:" + name_);
    if (OHOS::Rosen::RsFrameReportExt::GetInstance().GetEnable()) {
        OHOS::Rosen::RsFrameReportExt::GetInstance().RequestNextVSync();
    }
    if (listener_->GetType() == AppExecFwk::FileDescriptorListener::ListenerType::LTYPE_VSYNC) {
        looper_->RequestVsyncNotification(listener_->GetTimeStamp(), listener_->GetPeriod());
    }
    return connection_->RequestNextVSync(fromWhom, lastVSyncTS, requestVsyncTime);
}

VsyncError VSyncReceiver::RequestNextVSyncWithMultiCallback(FrameCallback callback)
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (!init_) {
        VLOGE("%{public}s not init", __func__);
        return VSYNC_ERROR_NOT_INIT;
    }
    listener_->AddCallback(callback);
    listener_->SetRNVFlag(true);
    ScopedDebugTrace func("VSyncReceiver::RequestNextVSync:" + name_);
    if (OHOS::Rosen::RsFrameReportExt::GetInstance().GetEnable()) {
        OHOS::Rosen::RsFrameReportExt::GetInstance().RequestNextVSync();
    }
    if (listener_->GetType() == AppExecFwk::FileDescriptorListener::ListenerType::LTYPE_VSYNC) {
        looper_->RequestVsyncNotification(listener_->GetTimeStamp(), listener_->GetPeriod());
    }
    return connection_->RequestNextVSync();
}

VsyncError VSyncReceiver::SetVSyncRate(FrameCallback callback, int32_t rate)
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (!init_) {
        return VSYNC_ERROR_API_FAILED;
    }
    listener_->SetCallback(callback);
    return connection_->SetVSyncRate(rate);
}

/* 设置每帧回调 */
VsyncError VSyncReceiver::SetVsyncCallBackForEveryFrame(FrameCallback callback, bool isOpen)
{
    if (isOpen) {
        return SetVSyncRate(callback, 1);
    } else {
        return SetVSyncRate(callback, -1);
    }
}

VsyncError VSyncReceiver::GetVSyncPeriod(int64_t &period)
{
    int64_t timeStamp;
    return GetVSyncPeriodAndLastTimeStamp(period, timeStamp);
}

VsyncError VSyncReceiver::GetVSyncPeriodAndLastTimeStamp(int64_t &period, int64_t &timeStamp, bool isThreadShared)
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (!init_) {
        VLOGE("%{public}s not init", __func__);
        return VSYNC_ERROR_NOT_INIT;
    }
    if (isThreadShared == false) {
        int64_t periodNotShared = listener_->GetPeriod();
        int64_t timeStampNotShared = listener_->GetTimeStamp();
        if (periodNotShared == 0 || timeStampNotShared == 0) {
            VLOGD("%{public}s Hardware vsync is not available. please try again later!", __func__);
            return VSYNC_ERROR_UNKOWN;
        }
        period = periodNotShared;
        timeStamp = timeStampNotShared;
    } else {
        int64_t periodShared = listener_->GetPeriodShared();
        int64_t timeStampShared = listener_->GetTimeStampShared();
        if (periodShared == 0 || timeStampShared == 0) {
            VLOGD("%{public}s Hardware vsync is not available. please try again later!", __func__);
            return VSYNC_ERROR_UNKOWN;
        }
        period = periodShared;
        timeStamp = timeStampShared;
    }
    RS_TRACE_NAME_FMT("VSyncReceiver:period:%ld timeStamp:%ld isThreadShared:%d", period, timeStamp, isThreadShared);
    return VSYNC_ERROR_OK;
}

void VSyncReceiver::CloseVsyncReceiverFd()
{
    std::lock_guard<std::mutex> locker(initMutex_);
    RemoveAndCloseFdLocked();
}

VsyncError VSyncReceiver::DestroyLocked()
{
    if (connection_ == nullptr) {
        return VSYNC_ERROR_API_FAILED;
    }
    return connection_->Destroy();
}

bool VSyncReceiver::IsRequestedNextVSync()
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (!init_) {
        return false;
    }
    return listener_->GetRNVFlag();
}

VsyncError VSyncReceiver::SetUiDvsyncSwitch(bool dvsyncSwitch)
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (!init_) {
        return VSYNC_ERROR_API_FAILED;
    }
    return connection_->SetUiDvsyncSwitch(dvsyncSwitch);
}

VsyncError VSyncReceiver::SetUiDvsyncConfig(int32_t bufferCount, bool delayEnable, bool nativeDelayEnable)
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (!init_) {
        return VSYNC_ERROR_API_FAILED;
    }
    VLOGI("SetUiDvsyncConfig bufferCount:%d delayEnable:%d nativeDelayEnable:%d",
        bufferCount, delayEnable, nativeDelayEnable);
    return connection_->SetUiDvsyncConfig(bufferCount, delayEnable, nativeDelayEnable);
}

VsyncError VSyncReceiver::SetNativeDVSyncSwitch(bool dvsyncSwitch)
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (!init_) {
        return VSYNC_ERROR_API_FAILED;
    }
    return connection_->SetNativeDVSyncSwitch(dvsyncSwitch);
}
} // namespace Rosen
} // namespace OHOS
