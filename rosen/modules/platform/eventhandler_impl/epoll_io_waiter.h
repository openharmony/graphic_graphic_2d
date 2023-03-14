/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef BASE_EVENTHANDLER_FRAMEWORKS_EVENTHANDLER_INCLUDE_EPOLL_IO_WAITER_H
#define BASE_EVENTHANDLER_FRAMEWORKS_EVENTHANDLER_INCLUDE_EPOLL_IO_WAITER_H

#include <atomic>
#include <mutex>

#include "io_waiter.h"
#include "nocopyable.h"

#include <map>
#include <uv.h>

namespace OHOS {
namespace AppExecFwk {
// Use epoll to listen file descriptor.
class EpollIoWaiter final : public IoWaiter {
public:
    EpollIoWaiter() = default;
    ~EpollIoWaiter() final;
    DISALLOW_COPY_AND_MOVE(EpollIoWaiter);

    /**
     * Initialize epoll.
     *
     * @return True if succeeded.
     */
    bool Init();

    bool WaitFor(std::unique_lock<std::mutex> &lock, int64_t nanoseconds) final;

    void NotifyOne() final;
    void NotifyAll() final;

    bool SupportListeningFileDescriptor() const final;

    bool AddFileDescriptor(int32_t fd, uint32_t events) final;
    void RemoveFileDescriptor(int32_t fd) final;

    void SetFileDescriptorEventCallback(const FileDescriptorEventCallback &callback) final;

private:
    static void OnPoll(uv_poll_t *poll, int status, int events);
    static void OnTimer(uv_timer_t *timer);
    static void OnSignal(uv_signal_t *signal, int32_t signum);
    template<typename T>
    static EpollIoWaiter &GetInstance(T *that)
    {
        return *reinterpret_cast<EpollIoWaiter *>(that->data);
    }

    uv_loop_t loop_ = {.data = this};
    uv_signal_t signal_ = {.data = this};
    uv_timer_t timer_ = {.data = this};
    std::mutex timerMutex_;
    std::map<int32_t, uv_poll_t> pollMap_;
    bool overtime = false;
    FileDescriptorEventCallback fdcallback_ = nullptr;
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // #ifndef BASE_EVENTHANDLER_FRAMEWORKS_EVENTHANDLER_INCLUDE_EPOLL_IO_WAITER_H
