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

#include "event_handler.h"

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

class HgmSimpleTimer {
public:
    using Interval = std::chrono::milliseconds;
    using StartCallback = std::function<void()>;
    using ExpiredCallback = std::function<void()>;

    HgmSimpleTimer(std::string name, const Interval& interval, const StartCallback& startCallback,
        const ExpiredCallback& expiredCallback,
        std::unique_ptr<ChronoSteadyClock> clock = std::make_unique<ChronoSteadyClock>());
    ~HgmSimpleTimer() = default;

    // Initializes and turns on the idle timer.
    void Start();
    // Stops the idle timer and any held resources.
    void Stop();
    // Resets the wakeup time and fires the reset callback.
    void Reset();
    // Set interval value
    void SetInterval(Interval valueMs);
private:
    void Loop();

    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;

    std::string name_;
    std::atomic<Interval> interval_;
    const StartCallback startCallback_ = nullptr;
    const ExpiredCallback expiredCallback_ = nullptr;
    std::unique_ptr<ChronoSteadyClock> clock_ = nullptr;

    std::atomic<std::chrono::steady_clock::time_point> resetTimePoint_;
    std::atomic<bool> running_{ false };
};
} // namespace OHOS::Rosen

#endif // HGM_ONE_SHOT_TIME_H