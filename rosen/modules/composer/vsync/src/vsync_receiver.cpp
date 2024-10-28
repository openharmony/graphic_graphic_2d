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
#include "event_handler.h"
#include "graphic_common.h"
#include "res_sched_client.h"
#include "res_type.h"
#include "rs_frame_report_ext.h"
#include "vsync_log.h"
#include "sandbox_utils.h"
#include <rs_trace.h>
#include "qos.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t INVALID_FD = -1;
}
void VSyncCallBackListener::OnReadable(int32_t fileDescriptor)
{
    HitracePerfScoped perfTrace(ScopedDebugTrace::isEnabled(), HITRACE_TAG_GRAPHIC_AGP, "OnReadablePerfCount");
    if (fileDescriptor < 0) {
        return;
    }
    // 3 is array size.
    int64_t data[3];
    ssize_t dataCount = 0;
    if (ReadFdInternal(fileDescriptor, data, dataCount) != VSYNC_ERROR_OK) {
        return;
    }
    HandleVsyncCallbacks(data, dataCount);
}

void VSyncCallBackListener::OnShutdown(int32_t fileDescriptor)
{
    std::lock_guard<std::mutex> locker(cbMutex_);
    if (fdShutDownCallback_ != nullptr) {
        fdShutDownCallback_(fileDescriptor);
    }
}

VsyncError VSyncCallBackListener::ReadFdInternal(int32_t fd, int64_t (&data)[3], ssize_t &dataCount)
{
    std::lock_guard<std::mutex> locker(fdMutex_);
    if (fdClosed_) {
        return VSYNC_ERROR_API_FAILED;
    }
    ssize_t ret = 0;
    do {
        // only take the latest timestamp
        ret = read(fd, data, sizeof(data));
        if (ret == 0) {
            VLOGE("ReadFdInternal, ret is 0, read fd:%{public}d failed, errno:%{public}d", fd, errno);
            return VSYNC_ERROR_OK;
        }
        if (ret == -1) {
            if (errno == EINTR) {
                ret = 0;
                continue;
            } else if (errno != EAGAIN) {
                VLOGE("ReadFdInternal, read fd:%{public}d failed, errno:%{public}d", fd, errno);
            }
        } else {
            dataCount += ret;
        }
    } while (ret != -1);

    return VSYNC_ERROR_OK;
}

void VSyncCallBackListener::HandleVsyncCallbacks(int64_t data[], ssize_t dataCount)
{
    VSyncCallback cb = nullptr;
    VSyncCallbackWithId cbWithId = nullptr;
    void *userData = nullptr;
    int64_t now = 0;
    int64_t expectedEnd = 0;
    std::vector<FrameCallback> callbacks;
    {
        std::lock_guard<std::mutex> locker(mtx_);
        cb = vsyncCallbacks_;
        cbWithId = vsyncCallbacksWithId_;
        userData = userData_;
        RNVFlag_ = false;
        now = data[0];
        period_ = data[1];
        periodShared_ = data[1];
        timeStamp_ = data[0];
        timeStampShared_ = data[0];
        expectedEnd = CalculateExpectedEndLocked(now);
        callbacks = frameCallbacks_;
        frameCallbacks_.clear();
    }

    VLOGD("dataCount:%{public}d, cb == nullptr:%{public}d", dataCount, (cb == nullptr));
    // 1, 2: index of array data.
    RS_TRACE_NAME_FMT("ReceiveVsync dataCount: %ldbytes now: %ld expectedEnd: %ld vsyncId: %ld",
        dataCount, now, expectedEnd, data[2]); // data[2] is vsyncId
    if (callbacks.empty() && dataCount > 0 && (cbWithId != nullptr || cb != nullptr)) {
        // data[2] is frameCount
        cbWithId != nullptr ? cbWithId(now, data[2], userData) : cb(now, userData);
    }
    for (const auto& cb : callbacks) {
        if (cb.callback_ != nullptr) {
            cb.callback_(now, cb.userData_);
        } else if (cb.callbackWithId_ != nullptr) {
            cb.callbackWithId_(now, data[2], cb.userData_); // data[2] is vsyncId
        }
    }
    if (OHOS::Rosen::RsFrameReportExt::GetInstance().GetEnable()) {
        OHOS::Rosen::RsFrameReportExt::GetInstance().ReceiveVSync();
    }
}

int64_t VSyncCallBackListener::CalculateExpectedEndLocked(int64_t now)
{
    int64_t expectedEnd = 0;
    if (now < period_ || now > INT64_MAX - period_) {
        RS_TRACE_NAME_FMT("invalid timestamps, now:%ld, period_:%ld", now, period_);
        VLOGE("invalid timestamps, now:" VPUBI64 ", period_:" VPUBI64, now, period_);
        return 0;
    }
    expectedEnd = now + period_;
    if (name_ == "rs") {
        // rs vsync offset is 5000000ns
        expectedEnd = expectedEnd + period_ - 5000000;
    }
    return expectedEnd;
}

void VSyncCallBackListener::SetFdClosedFlagLocked(bool fdClosed)
{
    fdClosed_ = fdClosed;
}

void VSyncCallBackListener::RegisterFdShutDownCallback(FdShutDownCallback cb)
{
    std::lock_guard<std::mutex> locker(cbMutex_);
    fdShutDownCallback_ = cb;
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

VsyncError VSyncReceiver::Init()
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (init_) {
        return VSYNC_ERROR_OK;
    }
    if (connection_ == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }

    if (looper_ == nullptr) {
        std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("OS_VSyncThread");
        if (runner == nullptr) {
            return VSYNC_ERROR_API_FAILED;
        }
        looper_ = std::make_shared<AppExecFwk::EventHandler>(runner);
        runner->Run();
        looper_->PostTask([this] {
            SetThreadQos(QOS::QosLevel::QOS_USER_INTERACTIVE);
            this->ThreadCreateNotify();
        });
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
            VLOGE("Invalid fileDescriptor:%{public}d, fd_:%{public}d", fileDescriptor, fd_);
            return;
        }
        RemoveAndCloseFdLocked();
    });

    looper_->AddFileDescriptorListener(fd_, AppExecFwk::FILE_DESCRIPTOR_INPUT_EVENT, listener_, "vSyncTask");
    init_ = true;
    return VSYNC_ERROR_OK;
}

void VSyncReceiver::ThreadCreateNotify()
{
    int32_t pid = getprocpid();
    uint32_t uid = getuid();
    int32_t tid = static_cast<int32_t>(getproctid());
    VLOGI("vsync thread pid=%{public}d, tid=%{public}d, uid=%{public}u.", pid, tid, uid);

    std::unordered_map<std::string, std::string> mapPayload;
    mapPayload["pid"] = std::to_string(pid);
    mapPayload["uid"] = std::to_string(uid);
    mapPayload["tid"] = std::to_string(tid);
    OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(
        ResourceSchedule::ResType::RES_TYPE_REPORT_VSYNC_TID, tid, mapPayload);
}

VSyncReceiver::~VSyncReceiver()
{
    listener_->RegisterFdShutDownCallback(nullptr);
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

VsyncError VSyncReceiver::RequestNextVSync(FrameCallback callback, const std::string &fromWhom, int64_t lastVSyncTS)
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
    return connection_->RequestNextVSync(fromWhom, lastVSyncTS);
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

VsyncError VSyncReceiver::SetUiDvsyncConfig(int32_t bufferCount)
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (!init_) {
        return VSYNC_ERROR_API_FAILED;
    }
    VLOGI("%{public}s bufferCount:%{public}d", __func__, bufferCount);
    return connection_->SetUiDvsyncConfig(bufferCount);
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
