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
#include <sstream>
#include "hgm_log.h"
namespace OHOS::Rosen {
namespace {
using namespace std::chrono_literals;
using nsecs_t =  int64_t;

constexpr int64_t NS_TO_SECONDS = std::chrono::duration_cast<std::chrono::nanoseconds>(1s).count();
constexpr auto ZERO = std::chrono::steady_clock::duration::zero();

static inline nsecs_t SystemTime()
{
    struct timespec t;
    t.tv_sec = t.tv_nsec = 0;
    clock_gettime(CLOCK_REALTIME, &t);
    return nsecs_t(t.tv_sec) * NS_TO_SECONDS + t.tv_nsec;
}

void CalculateTimeoutTime(std::chrono::nanoseconds timestamp, timespec* spec)
{
    const nsecs_t timeout = SystemTime() + timestamp.count();
    spec->tv_sec = static_cast<time_t>(timeout / NS_TO_SECONDS);
    spec->tv_nsec = timeout % NS_TO_SECONDS;
}
} // namespace

HgmOneShotTimer::HgmOneShotTimer(std::string name, const Interval& interval,
    const ResetCallback& resetCallback, const ExpiredCallback& expiredCallback,
    std::unique_ptr<ChronoSteadyClock> clock)
    : clock_(std::move(clock)),
      name_(std::move(name)),
      interval_(interval),
      resetCallback_(resetCallback),
      expiredCallback_(expiredCallback)
{
    int result = sem_init(&semaphone_, 0, 0);
    HGM_LOGD("HgmOneShotTimer::sem_init result: %{public}d", result);
};

HgmOneShotTimer::~HgmOneShotTimer()
{
    Stop();
    int result = sem_destroy(&semaphone_);
    HGM_LOGD("HgmOneShotTimer::sem_destroy result: %{public}d", result);
}

void HgmOneShotTimer::Start()
{
    if (!thread_.joinable()) {
        thread_ = std::thread(&HgmOneShotTimer::Loop, this);
    }
}

void HgmOneShotTimer::Stop()
{
    stopFlag_ = true;
    int result = sem_post(&semaphone_);
    HGM_LOGD("HgmOneShotTimer::sem_post result: %{public}d", result);
    if (thread_.joinable()) {
        thread_.join();
    }
}

void HgmOneShotTimer::Loop()
{
    pthread_setname_np(pthread_self(), name_.c_str());
    HgmTimerState state = HgmTimerState::RESET;
    while (true) {
        bool resetFlag = false;
        bool expiredFlag = false;
        state = CheckForResetAndStop(state);
        if (state == HgmTimerState::STOP) {
            break;
        }
        if (state == HgmTimerState::IDLE) {
            int result = sem_wait(&semaphone_);
            if (result && errno != EINTR) {
                HGM_LOGE("HgmOneShotTimer::sem_wait failed (%{public}s)", std::to_string(errno).c_str());
            }
            continue;
        }
        if (state == HgmTimerState::RESET) {
            resetFlag = true;
        }
        if (resetFlag && resetCallback_) {
            resetCallback_();
        }
        state = CheckForResetAndStop(state);
        if (state == HgmTimerState::STOP) {
            break;
        }
        auto expireTime = clock_->Now() + interval_;
        state = HgmTimerState::WAITING;
        while (state == HgmTimerState::WAITING) {
            struct timespec ts;
            CalculateTimeoutTime(std::chrono::nanoseconds(interval_), &ts);
            int result = sem_timedwait(&semaphone_, &ts);
            if (result && errno != ETIMEDOUT && errno != EINTR) {
                HGM_LOGE("HgmOneShotTimer::sem_timedwait failed (%{public}s)", std::to_string(errno).c_str());
            }
            state = CheckForResetAndStop(state);
            if (state == HgmTimerState::RESET) {
                expireTime = clock_->Now() + interval_;
                state = HgmTimerState::WAITING;
            } else if (state == HgmTimerState::WAITING && CheckTimerExpired(expireTime)) {
                expiredFlag = true;
                state = HgmTimerState::IDLE;
            }
        }
        if (expiredFlag && expiredCallback_) {
            expiredCallback_();
        }
    }
}

bool HgmOneShotTimer::CheckTimerExpired(std::chrono::steady_clock::time_point expireTime) const
{
    return (expireTime - clock_->Now()) <= ZERO;
}

HgmOneShotTimer::HgmTimerState HgmOneShotTimer::CheckForResetAndStop(HgmTimerState state)
{
    if (stopFlag_.exchange(false)) {
        return HgmTimerState::STOP;
    }
    if (state != HgmTimerState::STOP && resetFlag_.exchange(false)) {
        return HgmTimerState::RESET;
    }
    return state;
}

void HgmOneShotTimer::Reset()
{
    resetFlag_ = true;
    int result = sem_post(&semaphone_);
    HGM_LOGD("HgmOneShotTimer::sem_post result: %{public}d", result);
}

std::string HgmOneShotTimer::Dump() const
{
    std::ostringstream stream;
    stream << interval_.count() << "ms";
    return stream.str();
}
} // namespace OHOS::Rosen