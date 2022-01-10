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

#include "rs_thread_looper_impl.h"

#include <algorithm>

namespace OHOS {
namespace Rosen {
static thread_local std::unique_ptr<ThreadLooperImpl> g_looperInstance = nullptr;

ThreadLooperImpl* ThreadLooperImpl::CreateThreadInstance()
{
    if (!g_looperInstance) {
        g_looperInstance = std::make_unique<ThreadLooperImpl>();
    }
    return g_looperInstance.get();
}

ThreadLooperImpl* ThreadLooperImpl::GetThreadInstance()
{
    return g_looperInstance.get();
}

bool ThreadLooperImpl::HaveDelayedMessageToProcess()
{
    if (delayedQueue_.empty()) {
        return false;
    }
    auto message = *delayedQueue_.begin();
    return std::get<time_t>(message) <= clock_t::now();
}

void ThreadLooperImpl::WaitForMessage(int timeoutMillis)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (queue_.empty() && !HaveDelayedMessageToProcess()) {
        wakeup_ = false;
        auto pred = [this]() { return !queue_.empty() || HaveDelayedMessageToProcess() || wakeup_; };
        if (timeoutMillis < 0) {
            cv_.wait(lock, pred);
        } else {
            auto time = clock_t::now() + std::chrono::milliseconds(timeoutMillis);
            if (!delayedQueue_.empty()) {
                auto message = *delayedQueue_.begin();
                auto msgTime = std::get<time_t>(message);
                time = std::min(time, msgTime);
            }
            cv_.wait_until(lock, time, pred);
        }
    }
}

bool ThreadLooperImpl::ProcessOneMessageInternal()
{
    message_t task = nullptr;
    int param = 0;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!queue_.empty()) {
            auto top = queue_.begin();
            auto message = *top;
            queue_.erase(top);
            task = std::get<message_t>(message);
            param = std::get<int>(message);
        } else if (HaveDelayedMessageToProcess()) {
            auto top = delayedQueue_.begin();
            auto message = *top;
            delayedQueue_.erase(top);
            task = std::get<message_t>(message);
            param = std::get<int>(message);
        }
    }
    if (!task) {
        return false;
    }
    task->Process(param);
    return true;
}

void ThreadLooperImpl::ProcessOneMessage(int timeoutMillis)
{
    WaitForMessage(timeoutMillis);
    ProcessOneMessageInternal();
}

void ThreadLooperImpl::ProcessAllMessages(int timeoutMillis)
{
    if (timeoutMillis < 0) {
        WaitForMessage(timeoutMillis);
        while (ProcessOneMessageInternal()) {
            // Do nothing, just process messages while we can
        }
    } else {
        auto waitUntil = clock_t::now() + std::chrono::milliseconds(timeoutMillis);
        std::chrono::milliseconds::rep timeLeft = timeoutMillis;
        while (timeLeft > 0) {
            ProcessOneMessage(timeoutMillis);
            timeLeft = std::chrono::duration_cast<std::chrono::milliseconds>(waitUntil - clock_t::now()).count();
        }
    }
}

void ThreadLooperImpl::WakeUp()
{
    std::lock_guard<std::mutex> lock(mutex_);
    wakeup_ = true;
    cv_.notify_all();
}

void ThreadLooperImpl::PostMessage(const std::shared_ptr<ThreadLooperMessage>& message, int param)
{
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.emplace_back(message, param);
    cv_.notify_all();
}

void ThreadLooperImpl::PostMessage(int64_t delay, const std::shared_ptr<ThreadLooperMessage>& message, int param)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto time = clock_t::now() + std::chrono::nanoseconds(delay);
    auto castTime = std::chrono::time_point_cast<clock_t::duration>(time);
    delayedQueue_.emplace(castTime, message, param);
    cv_.notify_all();
}

void ThreadLooperImpl::RemoveMessages(const std::shared_ptr<ThreadLooperMessage>& message)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto pos = std::begin(queue_); pos != std::end(queue_);) {
        if (std::get<message_t>(*pos) == message) {
            pos = queue_.erase(pos);
        } else {
            ++pos;
        }
    }
    for (auto pos = std::begin(delayedQueue_); pos != std::end(delayedQueue_);) {
        if (std::get<message_t>(*pos) == message) {
            pos = delayedQueue_.erase(pos);
        } else {
            ++pos;
        }
    }
}
} // namespace Rosen
} // namespace OHOS
