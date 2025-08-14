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

#include "hgm_one_shot_timer.h"
#include "hgm_log.h"
#include "hgm_task_handle_thread.h"

namespace OHOS::Rosen {
namespace {
constexpr auto ZERO = std::chrono::steady_clock::duration::zero();
} // namespace

HgmSimpleTimer::HgmSimpleTimer(std::string name, const Interval& interval,
    const StartCallback& startCallback, const ExpiredCallback& expiredCallback,
    std::unique_ptr<ChronoSteadyClock> clock)
    : name_(std::move(name)),
      interval_(interval),
      startCallback_(startCallback),
      expiredCallback_(expiredCallback),
      clock_(std::move(clock))
{}

void HgmSimpleTimer::Start()
{
    if (HgmTaskHandleThread::Instance().GetQueue() == nullptr) {
        return;
    }

    bool isRunning = running_.exchange(true);
    Reset();    // Reset() only take effect when running

    // start
    if (!isRunning) {
        if (startCallback_) {
            HgmTaskHandleThread::Instance().PostTask(startCallback_);
        }
        auto time = interval_.load();
        HgmTaskHandleThread::Instance().PostEvent(name_, [this]() { Loop(); }, time.count());
    }
}

void HgmSimpleTimer::Stop()
{
    if (running_.exchange(false) && HgmTaskHandleThread::Instance().GetQueue() != nullptr) {
        HgmTaskHandleThread::Instance().RemoveEvent(name_);
    }
}

void HgmSimpleTimer::Reset()
{
    if (running_.load() && clock_ != nullptr) {
        resetTimePoint_.store(clock_->Now());
    }
}

void HgmSimpleTimer::SetInterval(Interval valueMs)
{
    if (interval_.load() != valueMs) {
        interval_.store(valueMs);
    }
}

void HgmSimpleTimer::Loop()
{
    auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(
        resetTimePoint_.load() + interval_.load() - clock_->Now());
    if (delay > ZERO) {
        // reset
        if (running_.load() && HgmTaskHandleThread::Instance().GetQueue() != nullptr) {
            HgmTaskHandleThread::Instance().PostEvent(name_, [this]() { Loop(); }, delay.count());
            return;
        }
    } else {
        // cb
        if (expiredCallback_ != nullptr) {
            HgmTaskHandleThread::Instance().PostTask(expiredCallback_);
        }
    }
    running_.store(false);
}
} // namespace OHOS::Rosen