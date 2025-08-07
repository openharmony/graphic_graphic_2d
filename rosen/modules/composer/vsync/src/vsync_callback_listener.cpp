/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#if defined(RS_ENABLE_DVSYNC_2)
#include "dvsync_delay.h"
#endif
#include "graphic_common.h"
#include "rs_frame_report_ext.h"
#include "vsync_log.h"
#include <rs_trace.h>

namespace OHOS {
namespace Rosen {
void VSyncCallBackListener::OnReadable(int32_t fileDescriptor)
{
    HitracePerfScoped perfTrace(ScopedDebugTrace::isEnabled(), HITRACE_TAG_GRAPHIC_AGP, "OnReadablePerfCount");
    {
        std::lock_guard<std::mutex> locker(cbMutex_);
        if (fileDescriptor < 0 || (readableCallback_ != nullptr && !readableCallback_(fileDescriptor))) {
            VLOGE("OnReadable Invalid fileDescriptor:%{public}d", fileDescriptor);
            return;
        }
    }
    // 3 is array size.
    int64_t data[3];
    ssize_t dataCount = 0;
    if (ReadFdInternal(fileDescriptor, data, dataCount) != VSYNC_ERROR_OK) {
        return;
    }
#if defined(RS_ENABLE_DVSYNC_2)
    auto taskFunc = [now = data[0], period = data[1], vsyncId = data[2],
            dataCount, fileDescriptor, weak = weak_from_this()]() {
        if (auto self = weak.lock()) {
            int64_t newData[3] = {now, period, vsyncId};
            DVSyncDelay::Instance().UpdateDelayInfo();
            self->HandleVsyncCallbacks(newData, dataCount, fileDescriptor);
        } else {
            VLOGE("VSyncCallBackListener::OnReadable, weak.lock error");
        }
    };
    DVSyncDelay::Instance().ToDelay(taskFunc, name_, fileDescriptor);
#else
    HandleVsyncCallbacks(data, dataCount, fileDescriptor);
#endif
}

void VSyncCallBackListener::OnShutdown(int32_t fileDescriptor)
{
    VLOGI("OnShutdown, fileDescriptor:%{public}d", fileDescriptor);
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

void VSyncCallBackListener::HandleVsyncCallbacks(int64_t data[], ssize_t dataCount, int32_t fileDescriptor)
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
    RS_TRACE_NAME_FMT("ReceiveVsync name:%s dataCount: %ldbytes now: %ld expectedEnd: %ld vsyncId: %ld, fd:%d",
        name_.c_str(), dataCount, now, expectedEnd, data[2], fileDescriptor); // data[2] is vsyncId
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
    if (period_ < 0 || now < period_ || now > INT64_MAX - period_) {
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

void VSyncCallBackListener::RegisterReadableCallback(ReadableCallback cb)
{
    std::lock_guard<std::mutex> locker(cbMutex_);
    readableCallback_ = cb;
}
} // namespace Rosen
} // namespace OHOS
