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

#include "vsync_generator.h"

namespace OHOS {
namespace Rosen {
namespace impl {
namespace {
static int64_t GetSysTimeNs()
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}
}

std::once_flag VSyncGenerator::createFlag_;
sptr<OHOS::Rosen::VSyncGenerator> VSyncGenerator::instance_ = nullptr;

sptr<OHOS::Rosen::VSyncGenerator> VSyncGenerator::GetInstance() noexcept
{
    std::call_once(createFlag_, []() {
        auto vsyncGenerator = new VSyncGenerator();
        instance_ = vsyncGenerator;
    });

    return instance_;
}

VSyncGenerator::VSyncGenerator()
    : period_(0), phase_(0), refrenceTime_(0), wakeupDelay_(0), vsyncThreadRunning_(false)
{
    thread_ = std::thread(std::bind(&VSyncGenerator::ThreadLoop, this));
}

VSyncGenerator::~VSyncGenerator()
{
    vsyncThreadRunning_ = false;
    if (thread_.joinable()) {
        thread_.join();
    }
}

void VSyncGenerator::ThreadLoop()
{
    vsyncThreadRunning_ = true;
    int64_t occurTimestamp = GetSysTimeNs();
    while (vsyncThreadRunning_ == true) {
        std::vector<Listener> listeners;
        {
            std::unique_lock<std::mutex> locker(mutex_);
            if (period_ == 0) {
                con_.wait(locker);
                continue;
            }

            int64_t nextTimeStamp = ComputeNextVSyncTimeStamp(occurTimestamp);
            if (nextTimeStamp == INT64_MAX) {
                con_.wait(locker);
                continue;
            }

            // check
            bool isWakeUp = false;
            if (occurTimestamp < nextTimeStamp) {
                auto err = con_.wait_for(locker, std::chrono::nanoseconds(nextTimeStamp - occurTimestamp));
                if (err == std::cv_status::timeout) {
                    isWakeUp = true;
                } else {
                    continue;
                }
            }

            occurTimestamp = GetSysTimeNs();
            listeners = GetListenerTimeouted(occurTimestamp);
        }
        for (uint32_t i = 0; i < listeners.size(); i++) {
            listeners[i].callback_->OnVSyncEvent(listeners[i].lastTime_);
        }
    }
}

int64_t VSyncGenerator::ComputeNextVSyncTimeStamp(int64_t now)
{
    int64_t nextVSyncTime = INT64_MAX;
    for (uint32_t i = 0; i < listeners_.size(); i++) {
        int64_t t = ComputeListenerNextVSyncTimeStamp(listeners_[i], now);
        if (t < nextVSyncTime) {
            nextVSyncTime = t;
        }
    }

    return nextVSyncTime;
}

int64_t VSyncGenerator::ComputeListenerNextVSyncTimeStamp(const Listener& listener, int64_t now)
{
    int64_t lastVSyncTime = listener.lastTime_ + wakeupDelay_;
    if (now < lastVSyncTime) {
        now = lastVSyncTime;
    }

    now -= refrenceTime_;
    int64_t phase = phase_ + listener.phase_;
    now -= phase;
    if (now < 0) {
        now = -period_;
    }
    int64_t numPeriod = now / period_;
    int64_t nextTime = (numPeriod + 1) * period_ + phase;
    nextTime += refrenceTime_;

    // 3/5 just empirical value
    if (nextTime - listener.lastTime_ < (3 * period_ / 5)) {
        nextTime += period_;
    }

    // check wakeupDelay_
    nextTime -= wakeupDelay_;
    return nextTime;
}

std::vector<VSyncGenerator::Listener> VSyncGenerator::GetListenerTimeouted(int64_t now)
{
    std::vector<VSyncGenerator::Listener> ret;
    int64_t onePeriodAgo = now - period_;

    for (uint32_t i = 0; i < listeners_.size(); i++) {
        int64_t t = ComputeListenerNextVSyncTimeStamp(listeners_[i], onePeriodAgo);
        if (t < now) {
            listeners_[i].lastTime_ = t;
            ret.push_back(listeners_[i]);
        }
    }
    return ret;
}

void VSyncGenerator::UpdateMode(int64_t period, int64_t phase, int64_t refrenceTime)
{
    std::lock_guard<std::mutex> locker(mutex_);
    period_ = period;
    phase_ = phase;
    refrenceTime_ = refrenceTime;
    con_.notify_all();
}

void VSyncGenerator::AddListener(int64_t phase, const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb)
{
    std::lock_guard<std::mutex> locker(mutex_);
    Listener listener;
    listener.phase_ = phase;
    listener.callback_ = cb;
    listener.lastTime_ = 0;

    listeners_.push_back(listener);
    con_.notify_all();
}

void VSyncGenerator::RemoveListener(const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb)
{
    std::lock_guard<std::mutex> locker(mutex_);
    for (auto it = listeners_.begin(); it < listeners_.end(); it++) {
        if (it->callback_ == cb) {
            listeners_.erase(it);
            break;
        }
    }
    con_.notify_all();
}

void VSyncGenerator::ChangePhaseOffset(const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb, int64_t offset)
{
    std::lock_guard<std::mutex> locker(mutex_);
    auto it = listeners_.begin();
    for (; it < listeners_.end(); it++) {
        if (it->callback_ == cb) {
            break;
        }
    }
    it->phase_ = offset;
}
} // namespace impl
sptr<VSyncGenerator> CreateVSyncGenerator()
{
    return impl::VSyncGenerator::GetInstance();
}
}
}
