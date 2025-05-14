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
#include <vector>

namespace OHOS {
namespace Rosen {
class VSyncCallBackListener : public OHOS::AppExecFwk::FileDescriptorListener {
public:
    using VSyncCallback = std::function<void(int64_t, void*)>;
    using VSyncCallbackWithId = std::function<void(int64_t, int64_t, void*)>;
    using FdShutDownCallback = std::function<void(int32_t)>;
    using ReadableCallback = std::function<bool(int32_t)>;
    struct FrameCallback {
        void *userData_;
        VSyncCallback callback_;
        VSyncCallbackWithId callbackWithId_;
    };
    VSyncCallBackListener()
        : vsyncCallbacks_(nullptr), vsyncCallbacksWithId_(nullptr), userData_(nullptr)
    {}

    ~VSyncCallBackListener()
    {
    }
    void SetCallback(FrameCallback cb)
    {
        std::lock_guard<std::mutex> locker(mtx_);
        userData_ = cb.userData_;
        vsyncCallbacks_ = cb.callback_;
        vsyncCallbacksWithId_ = cb.callbackWithId_;
    }
    void SetName(std::string &name)
    {
        std::lock_guard<std::mutex> locker(mtx_);
        name_ = name;
    }
    void SetRNVFlag(bool RNVFlag)
    {
        std::lock_guard<std::mutex> locker(mtx_);
        RNVFlag_ = RNVFlag;
    }
    bool GetRNVFlag()
    {
        std::lock_guard<std::mutex> locker(mtx_);
        return RNVFlag_;
    }

    int64_t GetPeriod()
    {
        std::lock_guard<std::mutex> locker(mtx_);
        return period_;
    }

    int64_t GetTimeStamp()
    {
        std::lock_guard<std::mutex> locker(mtx_);
        return timeStamp_;
    }

    int64_t GetPeriodShared()
    {
        std::lock_guard<std::mutex> locker(mtx_);
        return periodShared_;
    }

    int64_t GetTimeStampShared()
    {
        std::lock_guard<std::mutex> locker(mtx_);
        return timeStampShared_;
    }

    void AddCallback(FrameCallback cb)
    {
        std::lock_guard<std::mutex> locker(mtx_);
        frameCallbacks_.push_back(cb);
    }

    void RegisterFdShutDownCallback(FdShutDownCallback cb);
    void RegisterReadableCallback(ReadableCallback cb);

    void SetFdClosedFlagLocked(bool fdClosed);

    std::mutex fdMutex_;
private:
    void OnReadable(int32_t fileDescriptor) override;
    void OnShutdown(int32_t fileDescriptor) override;
    int64_t CalculateExpectedEndLocked(int64_t now);
    void HandleVsyncCallbacks(int64_t data[], ssize_t dataCount, int32_t fileDescriptor);
    VsyncError ReadFdInternal(int32_t fd, int64_t (&data)[3], ssize_t &dataCount);
    VSyncCallback vsyncCallbacks_;
    VSyncCallbackWithId vsyncCallbacksWithId_;
    void *userData_;
    std::mutex mtx_;
    std::string name_;
    bool RNVFlag_ = false;
    int64_t period_ = 0;
    int64_t timeStamp_ = 0;
    thread_local static inline int64_t periodShared_ = 0;
    thread_local static inline int64_t timeStampShared_ = 0;
    std::vector<FrameCallback> frameCallbacks_ = {};
    bool fdClosed_ = false;
    FdShutDownCallback fdShutDownCallback_ = nullptr;
    ReadableCallback readableCallback_ = nullptr;
    std::mutex cbMutex_;
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
                                        int64_t lastVSyncTS, const int64_t &requestVsyncTime = 0);
    virtual bool IsRequestedNextVSync();
    virtual VsyncError SetVsyncCallBackForEveryFrame(FrameCallback callback, bool isOpen);
    virtual VsyncError SetUiDvsyncSwitch(bool dvsyncSwitch);
    virtual VsyncError SetUiDvsyncConfig(int32_t bufferCount);
    virtual VsyncError RequestNextVSyncWithMultiCallback(FrameCallback callback);
    virtual VsyncError SetNativeDVSyncSwitch(bool dvsyncSwitch);
private:
    void RegisterFileDescriptorListener();
    VsyncError DestroyLocked();
    void RemoveAndCloseFdLocked();
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
