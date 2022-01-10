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

#ifndef RENDER_SERVICE_BASE_ADAPTER_GENERIC_RS_THREAD_LOOPER_IMPL_H
#define RENDER_SERVICE_BASE_ADAPTER_GENERIC_RS_THREAD_LOOPER_IMPL_H

#include <condition_variable>
#include <deque>
#include <mutex>
#include <set>

namespace OHOS {
namespace Rosen {
class ThreadLooperMessage {
public:
    ThreadLooperMessage() = default;
    virtual ~ThreadLooperMessage() = default;
    virtual void Process(int param) const = 0;
};

class ThreadLooperImpl final {
public:
#ifdef ROSEN_USE_SYSTEM_CLOCK
    using clock_t = std::chrono::system_clock;
#else
    using clock_t = std::chrono::steady_clock;
#endif
    ThreadLooperImpl() = default;
    ~ThreadLooperImpl() = default;

    static ThreadLooperImpl* CreateThreadInstance();
    static ThreadLooperImpl* GetThreadInstance();
    void ProcessOneMessage(int timeoutMillis);
    void ProcessAllMessages(int timeoutMillis);
    void PostMessage(const std::shared_ptr<ThreadLooperMessage>& message, int param);
    void PostMessage(int64_t delay, const std::shared_ptr<ThreadLooperMessage>& message, int param);
    void RemoveMessages(const std::shared_ptr<ThreadLooperMessage>& message);
    void WakeUp();

private:
    using time_t = std::chrono::time_point<clock_t>;
    using message_t = std::shared_ptr<ThreadLooperMessage>;
    using queue_element_t = std::tuple<message_t, int>;
    using delayed_queue_element_t = std::tuple<time_t, message_t, int>;

    class Comp final {
    public:
        bool operator()(const delayed_queue_element_t& a, const delayed_queue_element_t& b) const
        {
            if (std::get<time_t>(a) != std::get<time_t>(b)) {
                return std::get<time_t>(a) < std::get<time_t>(b);
            }
            // If we actually have same timestamp, that's unlikely, but still, then just comapre pointers of handlers
            return std::get<message_t>(a).get() < std::get<message_t>(b).get();
        }
    };

    bool HaveDelayedMessageToProcess();
    void WaitForMessage(int timeoutMillis);
    bool ProcessOneMessageInternal();

    std::mutex mutex_;
    std::condition_variable cv_;
    // Set is used as ordered queue, since we can't have 2 exactly same elements
    std::set<delayed_queue_element_t, Comp> delayedQueue_;
    std::deque<queue_element_t> queue_;
    bool wakeup_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_ADAPTER_GENERIC_RS_THREAD_LOOPER_IMPL_H
