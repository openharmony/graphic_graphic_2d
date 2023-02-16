/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "epoll_io_waiter.h"

#include <map>
#include <memory>
#include <mutex>

#include <uv.h>

#include "event_handler_utils.h"
#include "file_descriptor_listener.h"

DEFINE_HILOG_LABEL("EpollIoWaiter");

#define _UVERR(call, err) HILOGE(#call " failed because %{public}s", uv_strerror(err))
#define UVERR(call, err) { if (err) { _UVERR(call, err); } }
#define UVERRRET(call, err) { if (err) { _UVERR(call, err); return; } }
#define UVERRRETVAL(call, err) { if (err) { _UVERR(call, err); return err; } }
#define UVCALL(call, ...) { if (auto err = call(__VA_ARGS__)) { UVERR(#call, err); } }
#define UVCALLRET(call, ...) { if (auto err = call(__VA_ARGS__)) { UVERRRET(#call, err); } }
#define UVCALLRETVAL(call, ...) { if (auto err = call(__VA_ARGS__)) { UVERRRETVAL(#call, err); } }

namespace OHOS {
namespace AppExecFwk {
EpollIoWaiter::~EpollIoWaiter()
{
    uv_signal_stop(&signal_);
    uv_timer_stop(&timer_);
    uv_stop(&loop_);
    uv_loop_close(&loop_);
}

bool EpollIoWaiter::Init()
{
    UVCALL(uv_loop_init, &loop_);
    UVCALL(uv_signal_init, &loop_, &signal_);
    UVCALL(uv_signal_start, &signal_, OnSignal, SIGINT);
    UVCALL(uv_timer_init, &loop_, &timer_);
    return true;
}

bool EpollIoWaiter::WaitFor(std::unique_lock<std::mutex> &lock, int64_t nanoseconds)
{
    UVCALL(uv_timer_start, &timer_, OnTimer, nanoseconds / 1e6, 0);
    lock.unlock();
    uv_run(&loop_, UV_RUN_DEFAULT);
    lock.lock();
    return overtime;
}

void EpollIoWaiter::NotifyOne()
{
    NotifyAll();
}

void EpollIoWaiter::NotifyAll()
{
    std::lock_guard lock(timerMutex_);
    UVCALL(uv_timer_start, &timer_, OnTimer, 0, 0);
}

bool EpollIoWaiter::SupportListeningFileDescriptor() const
{
    return true;
}

bool EpollIoWaiter::AddFileDescriptor(int32_t fd, uint32_t events)
{
    if (pollMap_.find(fd) != pollMap_.end()) {
        return false;
    }

    pollMap_[fd] = {.data = this};
    UVCALL(uv_poll_init, &loop_, &pollMap_[fd], fd);
    UVCALL(uv_poll_start, &pollMap_[fd], static_cast<uint8_t>(events), OnPoll);
    return true;
}

void EpollIoWaiter::RemoveFileDescriptor(int32_t fd)
{
    const auto &it = pollMap_.find(fd);
    if (it == pollMap_.end()) {
        return;
    }

    UVCALL(uv_poll_stop, &it->second);
}

void EpollIoWaiter::SetFileDescriptorEventCallback(const IoWaiter::FileDescriptorEventCallback &callback)
{
    fdcallback_ = callback;
}

void EpollIoWaiter::OnPoll(uv_poll_t *poll, int status, int events)
{
    UVERR(OnPoll, status);
    auto &that = GetInstance(poll);
    std::lock_guard lock(that.timerMutex_);
    UVCALL(uv_timer_stop, &that.timer_);
    uv_stop(&that.loop_);
    if (that.fdcallback_) {
        int32_t pollfd = -1;
        for (const auto &[fd, p] : that.pollMap_) {
            if (&p == poll) {
                pollfd = fd;
                break;
            }
        }
        that.fdcallback_(pollfd, events);
    }
    that.overtime = false;
}

void EpollIoWaiter::OnTimer(uv_timer_t *timer)
{
    auto &that = GetInstance(timer);
    std::lock_guard lock(that.timerMutex_);
    UVCALL(uv_timer_stop, timer);
    uv_stop(&that.loop_);
    that.overtime = true;
}

void EpollIoWaiter::OnSignal(uv_signal_t *signal, int32_t signum)
{
    auto &that = GetInstance(signal);
    if (signum == SIGINT) {
        UVCALL(uv_signal_stop, signal);
        that.NotifyAll();
    }
}
}  // namespace AppExecFwk
}  // namespace OHOS
