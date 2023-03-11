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
#ifndef RENDER_SERVICE_BASE_ADAPTER_RS_VSYNC_CLIENT_WINDOWS_H
#define RENDER_SERVICE_BASE_ADAPTER_RS_VSYNC_CLIENT_WINDOWS_H

#include "platform/drawing/rs_vsync_client.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace OHOS {
namespace Rosen {
template<class T>
class SemQueue {
public:
    void PopFront(T &t)
    {
        std::unique_lock lock(mutex_);
        if (queue_.empty()) {
            const auto &func = [this]() {
                return !queue_.empty();
            };
            cv_.wait(lock, func);
        }
        t = queue_.front();
        queue_.pop();
    }

    void Push(const T &t)
    {
        std::unique_lock lock(mutex_);
        queue_.push(std::move(t));
        cv_.notify_all();
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

class RSVsyncClientWindows : public RSVsyncClient {
public:
    RSVsyncClientWindows() = default;
    ~RSVsyncClientWindows() override;

    void RequestNextVsync() override;
    void SetVsyncCallback(VsyncCallback callback) override;

private:
    void VsyncThreadMain();

    VsyncCallback vsyncCallback_ = nullptr;
    std::unique_ptr<std::thread> vsyncThread_ = nullptr;
    std::atomic<bool> running_ {false};
    std::atomic<bool> having_ {false};
    std::mutex mutex_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_ADAPTER_RS_VSYNC_CLIENT_WINDOWS_H
