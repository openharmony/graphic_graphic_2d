/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef VSYNC_VSYNC_RECEIVER_H
#define VSYNC_VSYNC_RECEIVER_H

#include <refbase.h>
#include "ivsync_connection.h"
#include "file_descriptor_listener.h"

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <stdint.h>
#include <string>

namespace OHOS {
namespace Rosen {
class VSyncCallBackListener : public OHOS::AppExecFwk::FileDescriptorListener {
public:
    using VSyncCallback = std::function<void(int64_t, void*)>;
    struct FrameCallback {
        void *userData_;
        VSyncCallback callback_;
    };
    VSyncCallBackListener() : period_(0), timeStamp_(0), vsyncCallbacks_(nullptr), userData_(nullptr)
    {
    }

    ~VSyncCallBackListener()
    {
    }
    void SetCallback(FrameCallback cb)
    {
        std::lock_guard<std::mutex> locker(mtx_);
        vsyncCallbacks_ = cb.callback_;
        userData_ = cb.userData_;
    }
    void SetName(std::string &name)
    {
        std::lock_guard<std::mutex> locker(mtx_);
        name_ = name;
    }
    int64_t period_;
    int64_t timeStamp_;
    thread_local static inline int64_t periodShared_ = 0;
    thread_local static inline int64_t timeStampShared_ = 0;

private:
    void OnReadable(int32_t fileDescriptor) override;
    VSyncCallback vsyncCallbacks_;
    void *userData_;
    std::mutex mtx_;
    std::string name_;
};

#ifdef __OHOS__
class VSyncReceiver : public RefBase {
public:
    // check
    using FrameCallback = VSyncCallBackListener::FrameCallback;

    VSyncReceiver(const sptr<IVSyncConnection>& conn,
        const sptr<IRemoteObject>& token = nullptr,
        const std::shared_ptr<OHOS::AppExecFwk::EventHandler>& looper = nullptr,
        const std::string& name = "Uninitialized");
    ~VSyncReceiver();
    // nocopyable
    VSyncReceiver(const VSyncReceiver &) = delete;
    VSyncReceiver &operator=(const VSyncReceiver &) = delete;

    virtual VsyncError Init();
    virtual VsyncError RequestNextVSync(FrameCallback callback);
    virtual VsyncError SetVSyncRate(FrameCallback callback, int32_t rate);
    virtual VsyncError GetVSyncPeriod(int64_t &period);
    virtual VsyncError GetVSyncPeriodAndLastTimeStamp(int64_t &period, int64_t &timeStamp,
                                                        bool isThreadShared = false);
    int32_t GetFd() { return fd_; }

    /* transfer the FD to other process(want to use the FD),
      the current process does not use the FD, so close FD, but not close vsync connection
    */
    void CloseVsyncReceiverFd();
    virtual VsyncError RequestNextVSync(FrameCallback callback, const std::string &fromWhom,
                                        int64_t lastVSyncTS);
private:
    VsyncError Destroy();
    sptr<IVSyncConnection> connection_;
    sptr<IRemoteObject> token_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> looper_;
    std::shared_ptr<VSyncCallBackListener> listener_;

    std::mutex initMutex_;
    bool init_;
    int32_t fd_;
    std::string name_;
};
#else
class VSyncReceiver {
public:
    using FrameCallback = VSyncCallBackListener::FrameCallback;

    VSyncReceiver() = default;
    virtual ~VSyncReceiver() = default;
    VSyncReceiver(const VSyncReceiver &) = delete;
    VSyncReceiver &operator=(const VSyncReceiver &) = delete;

    virtual VsyncError Init() = 0;
    virtual VsyncError RequestNextVSync(FrameCallback callback) = 0;
    virtual VsyncError SetVSyncRate(FrameCallback callback, int32_t rate) = 0;
};
#endif
} // namespace Rosen
} // namespace OHOS

#endif
