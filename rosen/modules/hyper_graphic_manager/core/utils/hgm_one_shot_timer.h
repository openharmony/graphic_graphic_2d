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
#ifndef HGM_ONE_SHOT_TIME_H
#define HGM_ONE_SHOT_TIME_H
#include <semaphore.h>
#include <chrono>
#include <condition_variable>
#include <thread>

namespace OHOS::Rosen {
class ChronoSteadyClock {
public:
    ChronoSteadyClock() = default;
    ~ChronoSteadyClock() = default;

    static std::chrono::steady_clock::time_point Now()
    {
        return std::chrono::steady_clock::now();
    }
};

class HgmOneShotTimer {
public:
    using Interval = std::chrono::milliseconds;
    using ResetCallback = std::function<void()>;
    using ExpiredCallback = std::function<void()>;

    HgmOneShotTimer(std::string name, const Interval& interval, const ResetCallback& resetCallback,
        const ExpiredCallback& expiredCallback,
        std::unique_ptr<ChronoSteadyClock> clock = std::make_unique<ChronoSteadyClock>());
    ~HgmOneShotTimer();

    // Initializes and turns on the idle timer.
    void Start();
    // Stops the idle timer and any held resources.
    void Stop();
    // Resets the wakeup time and fires the reset callback.
    void Reset();

    std::string Dump() const;

private:
    enum class HgmTimerState {
        STOP = 0,
        RESET = 1,
        WAITING = 2,
        IDLE = 3
    };

    void Loop();
    HgmTimerState CheckForResetAndStop(HgmTimerState state);
    bool CheckTimerExpired(std::chrono::steady_clock::time_point expireTime) const;

    std::thread thread_;
    std::unique_ptr<ChronoSteadyClock> clock_;

    sem_t semaphone_;
    std::string name_;

    const Interval interval_;
    const ResetCallback resetCallback_;
    const ExpiredCallback expiredCallback_;

    std::atomic<bool> resetFlag_ = false;
    std::atomic<bool> stopFlag_ = false;
};
} //namespace OHOS::Rosen

#endif //HGM_ONE_SHOT_TIME_H