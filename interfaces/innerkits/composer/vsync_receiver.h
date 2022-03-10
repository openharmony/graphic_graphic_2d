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


#ifndef VSYNC_VSYNC_RECEIVER_H
#define VSYNC_VSYNC_RECEIVER_H

#include <refbase.h>
#include "ivsync_connection.h"
#include "file_descriptor_listener.h"

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
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
    VSyncCallBackListener() : vsyncCallbacks_(nullptr), userData_(nullptr)
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

private:
    void OnReadable(int32_t fileDescriptor) override;
    VSyncCallback vsyncCallbacks_;
    void *userData_;
    std::mutex mtx_;
};

class VSyncReceiver : public RefBase {
public:
    // check
    using FrameCallback = VSyncCallBackListener::FrameCallback;

    VSyncReceiver(const sptr<IVSyncConnection>& conn,
        const std::shared_ptr<OHOS::AppExecFwk::EventHandler>& looper = nullptr,
        const std::string& name = "Uninitialized");
    ~VSyncReceiver();
    // nocopyable
    VSyncReceiver(const VSyncReceiver &) = delete;
    VSyncReceiver &operator=(const VSyncReceiver &) = delete;

    VsyncError Init();
    VsyncError RequestNextVSync(FrameCallback callback);
    VsyncError SetVSyncRate(FrameCallback callback, int32_t rate);

private:
    sptr<IVSyncConnection> connection_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> looper_;
    std::shared_ptr<VSyncCallBackListener> listener_;

    std::mutex initMutex_;
    bool init_;
    int32_t fd_;
    std::string name_;
};
} // namespace Rosen
} // namespace OHOS

#endif
