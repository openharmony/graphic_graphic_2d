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
#include <scoped_bytrace.h>
#include "event_handler.h"
#include "graphic_common.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t INVALID_FD = -1;
}
void VSyncCallBackListener::OnReadable(int32_t fileDescriptor)
{
    if (fileDescriptor < 0) {
        return;
    }
    int64_t now;
    ssize_t retVal = read(fileDescriptor, &now, sizeof(int64_t));
    VSyncCallback cb = nullptr;
    {
        std::lock_guard<std::mutex> locker(mtx_);
        cb = vsyncCallbacks_;
    }
    if (retVal > 0 && cb != nullptr) {
        ScopedBytrace func("ReceiveVsync");
        cb(now, userData_);
    }
}

VSyncReceiver::VSyncReceiver(const sptr<IVSyncConnection>& conn,
    const std::shared_ptr<OHOS::AppExecFwk::EventHandler> looper)
    : connection_(conn), looper_(looper),
    listener_(std::make_shared<VSyncCallBackListener>()),
    init_(false),
    fd_(INVALID_FD)
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

    VsyncError ret = connection_->GetReceiveFd(fd_);
    if (ret != VSYNC_ERROR_OK) {
        return ret;
    }

    if (looper_ == nullptr) {
        std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(true);
        looper_ = std::make_shared<AppExecFwk::EventHandler>(runner);
        runner->Run();
    }

    looper_->AddFileDescriptorListener(fd_, OHOS::AppExecFwk::FILE_DESCRIPTOR_INPUT_EVENT, listener_);
    init_ = true;
    return VSYNC_ERROR_OK;
}

VSyncReceiver::~VSyncReceiver()
{
    if (fd_ != INVALID_FD) {
        looper_->RemoveFileDescriptorListener(fd_);
        close(fd_);
        fd_ = INVALID_FD;
    }
}

VsyncError VSyncReceiver::RequestNextVSync(FrameCallback callback)
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (!init_) {
        return VSYNC_ERROR_API_FAILED;
    }
    listener_->SetCallback(callback);
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
} // namespace Rosen
} // namespace OHOS
