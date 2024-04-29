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
#include "vsync_distributor.h"
#include <cstdint>
#include <mutex>
#include <scoped_bytrace.h>
#include <sched.h>
#include <sys/resource.h>
#include <string>
#include "vsync_log.h"
#include <ctime>

#ifdef COMPOSER_SCHED_ENABLE
#include "if_system_ability_manager.h"
#include <iservice_registry.h>
#include "system_ability_definition.h"
#endif

namespace OHOS {
namespace Rosen {
namespace impl {
namespace {
static int64_t systemTime()
{
    timespec t = {};
    clock_gettime(CLOCK_MONOTONIC, &t);
    return int64_t(t.tv_sec) * 1000000000LL + t.tv_nsec; // 1000000000ns == 1s
}

// 1.5ms
constexpr int64_t maxWaleupDelay = 1500000;
constexpr int32_t THREAD_PRIORTY = -6;
constexpr int32_t SCHED_PRIORITY = 2;
constexpr int64_t errorThreshold = 500000;
constexpr int32_t MAX_REFRESHRATE_DEVIATION = 5; // ±5Hz
constexpr int64_t REFERENCETIME_CHECK_THRESHOLD = 2000000; // 2000000ns == 2.0ms
constexpr int64_t PERIOD_CHECK_THRESHOLD = 1000000; // 1000000ns == 1.0ms
constexpr int64_t DEFAULT_SOFT_VSYNC_PERIOD = 16000000; // 16000000ns == 16ms

static void SetThreadHighPriority()
{
    setpriority(PRIO_PROCESS, 0, THREAD_PRIORTY);
    struct sched_param param = {0};
    param.sched_priority = SCHED_PRIORITY;
    sched_setscheduler(0, SCHED_FIFO, &param);
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

void VSyncGenerator::DeleteInstance() noexcept
{
    instance_ = nullptr;
}

VSyncGenerator::VSyncGenerator()
    : period_(DEFAULT_SOFT_VSYNC_PERIOD), phase_(0), referenceTime_(0), wakeupDelay_(0),
      pulse_(0), currRefreshRate_(0), referenceTimeOffsetPulseNum_(0), defaultReferenceTimeOffsetPulseNum_(0)
{
    vsyncThreadRunning_ = true;
    thread_ = std::thread(std::bind(&VSyncGenerator::ThreadLoop, this));
    pthread_setname_np(thread_.native_handle(), "VSyncGenerator");
}

VSyncGenerator::~VSyncGenerator()
{
    {
        std::unique_lock<std::mutex> locker(mutex_);
        vsyncThreadRunning_ = false;
    }
    if (thread_.joinable()) {
        con_.notify_all();
        thread_.join();
    }
}

void VSyncGenerator::ListenerVsyncEventCB(int64_t occurTimestamp, int64_t nextTimeStamp,
    int64_t occurReferenceTime, bool isWakeup)
{
    ScopedBytrace trace("occurTimestamp:" + std::to_string(occurTimestamp) +
                        ", nextTimeStamp:" + std::to_string(nextTimeStamp));
    std::vector<Listener> listeners;
    {
        std::unique_lock<std::mutex> locker(mutex_);
        int64_t newOccurTimestamp = systemTime();
        if (isWakeup) {
            UpdateWakeupDelay(newOccurTimestamp, nextTimeStamp);
        }
        if (vsyncMode_ == VSYNC_MODE_LTPO) {
            listeners = GetListenerTimeoutedLTPO(occurTimestamp, occurReferenceTime);
        } else {
            listeners = GetListenerTimeouted(newOccurTimestamp, occurTimestamp, occurReferenceTime);
        }
    }
    ScopedBytrace func("GenerateVsyncCount:" + std::to_string(listeners.size()) +
        ", period:" + std::to_string(periodRecord_) + ", currRefreshRate_:" + std::to_string(currRefreshRate_) +
        ", vsyncMode_:" + std::to_string(vsyncMode_));
    for (uint32_t i = 0; i < listeners.size(); i++) {
        ScopedBytrace func("listener phase is " + std::to_string(listeners[i].phase_));
        listeners[i].callback_->OnVSyncEvent(listeners[i].lastTime_, periodRecord_, currRefreshRate_, vsyncMode_);
    }
}

void VSyncGenerator::ThreadLoop()
{
#ifdef COMPOSER_SCHED_ENABLE
    SubScribeSystemAbility();
#endif
    // set thread priorty
    SetThreadHighPriority();

    int64_t occurTimestamp = 0;
    int64_t nextTimeStamp = 0;
    int64_t occurReferenceTime = 0;
    while (vsyncThreadRunning_ == true) {
        {
            std::unique_lock<std::mutex> locker(mutex_);
            UpdateVSyncModeLocked();
            occurReferenceTime = referenceTime_;
            phaseRecord_ = phase_;
            periodRecord_ = period_;
            if (period_ == 0) {
                ScopedBytrace func("VSyncGenerator: period not valid");
                if (vsyncThreadRunning_ == true) {
                    con_.wait(locker);
                }
                continue;
            }
            occurTimestamp = systemTime();
            nextTimeStamp = ComputeNextVSyncTimeStamp(occurTimestamp, occurReferenceTime);
            if (nextTimeStamp == INT64_MAX) {
                ScopedBytrace func("VSyncGenerator: there has no listener");
                if (vsyncThreadRunning_ == true) {
                    con_.wait(locker);
                }
                continue;
            } else if (vsyncMode_ == VSYNC_MODE_LTPO) {
                bool modelChanged = UpdateChangeDataLocked(occurTimestamp, occurReferenceTime, nextTimeStamp);
                if (modelChanged) {
                    ScopedBytrace func("VSyncGenerator: LTPO mode change");
                    continue;
                }
            }
        }

        bool isWakeup = false;
        if (occurTimestamp < nextTimeStamp) {
            std::unique_lock<std::mutex> lck(waitForTimeoutMtx_);
            auto err = waitForTimeoutCon_.wait_for(lck, std::chrono::nanoseconds(nextTimeStamp - occurTimestamp));
            if (err == std::cv_status::timeout) {
                isWakeup = true;
            } else {
                ScopedBytrace func("VSyncGenerator::ThreadLoop::Continue");
                continue;
            }
        }
        ListenerVsyncEventCB(occurTimestamp, nextTimeStamp, occurReferenceTime, isWakeup);
    }
}

bool VSyncGenerator::ChangeListenerOffsetInternal()
{
    if (changingPhaseOffset_.cb == nullptr) {
        return true;
    }
    auto it = listeners_.begin();
    for (; it < listeners_.end(); it++) {
        if (it->callback_ == changingPhaseOffset_.cb) {
            break;
        }
    }
    int64_t phaseOffset = pulse_ * changingPhaseOffset_.phaseByPulseNum;
    if (it != listeners_.end()) {
        it->phase_ = phaseOffset;
    }

    it = listenersRecord_.begin();
    for (; it < listenersRecord_.end(); it++) {
        if (it->callback_ == changingPhaseOffset_.cb) {
            break;
        }
    }
    if (it == listenersRecord_.end()) {
        return false;
    }
    it->callback_->OnPhaseOffsetChanged(phaseOffset);
    changingPhaseOffset_ = {}; // reset
    return true;
}

bool VSyncGenerator::ChangeListenerRefreshRatesInternal()
{
    if (changingRefreshRates_.cb == nullptr) {
        return true;
    }
    auto it = listenersRecord_.begin();
    for (; it < listenersRecord_.end(); it++) {
        if (it->callback_ == changingRefreshRates_.cb) {
            break;
        }
    }
    if (it == listenersRecord_.end()) {
        return false;
    }
    it->callback_->OnConnsRefreshRateChanged(changingRefreshRates_.refreshRates);
    // reset
    changingRefreshRates_.cb = nullptr;
    changingRefreshRates_.refreshRates.clear();
    changingRefreshRates_ = {};
    return true;
}

void VSyncGenerator::UpdateWakeupDelay(int64_t occurTimestamp, int64_t nextTimeStamp)
{
    // 63, 1 / 64
    wakeupDelay_ = ((wakeupDelay_ * 63) + (occurTimestamp - nextTimeStamp)) / 64;
    wakeupDelay_ = wakeupDelay_ > maxWaleupDelay ? maxWaleupDelay : wakeupDelay_;
}

int64_t VSyncGenerator::ComputeNextVSyncTimeStamp(int64_t now, int64_t referenceTime)
{
    int64_t nextVSyncTime = INT64_MAX;
    for (uint32_t i = 0; i < listeners_.size(); i++) {
        int64_t t = ComputeListenerNextVSyncTimeStamp(listeners_[i], now, referenceTime);
        if (t < nextVSyncTime) {
            nextVSyncTime = t;
        }
    }

    return nextVSyncTime;
}

bool VSyncGenerator::CheckTimingCorrect(int64_t now, int64_t referenceTime, int64_t nextVSyncTime)
{
    bool isTimingCorrect = false;
    for (uint32_t i = 0; i < listeners_.size(); i++) {
        int64_t t = ComputeListenerNextVSyncTimeStamp(listeners_[i], now, referenceTime);
        if ((t - nextVSyncTime < errorThreshold) && (listeners_[i].phase_ == 0)) {
            isTimingCorrect = true;
        }
    }
    return isTimingCorrect;
}

bool VSyncGenerator::UpdateChangeDataLocked(int64_t now, int64_t referenceTime, int64_t nextVSyncTime)
{
    if (!CheckTimingCorrect(now, referenceTime, nextVSyncTime)) {
        ScopedBytrace trace("Timing not Correct");
        return false;
    }

    bool modelChanged = false;

    // update generate refreshRate
    if (needChangeGeneratorRefreshRate_) {
        currRefreshRate_ = changingGeneratorRefreshRate_;
        period_ = pulse_ * static_cast<int64_t>(VSYNC_MAX_REFRESHRATE / currRefreshRate_);
        referenceTime_ = nextVSyncTime;
        changingGeneratorRefreshRate_ = 0; // reset
        needChangeGeneratorRefreshRate_ = false;
        refreshRateIsChanged_ = true;
        frameRateChanging_ = true;
        ScopedBytrace trace("frameRateChanging_ = true");
        modelChanged = true;
    }

    // update phaseOffset
    if (needChangePhaseOffset_) {
        bool offsetChangedSucceed = ChangeListenerOffsetInternal();
        if (offsetChangedSucceed) {
            needChangePhaseOffset_ = false;
            modelChanged = true;
        }
    }

    // update VSyncConnections refreshRates
    if (needChangeRefreshRates_) {
        bool refreshRatesChangedSucceed = ChangeListenerRefreshRatesInternal();
        if (refreshRatesChangedSucceed) {
            needChangeRefreshRates_ = false;
            modelChanged = true;
        }
    }

    return modelChanged;
}

void VSyncGenerator::UpdateVSyncModeLocked()
{
    if (pendingVsyncMode_ != VSYNC_MODE_INVALID) {
        vsyncMode_ = pendingVsyncMode_;
        pendingVsyncMode_ = VSYNC_MODE_INVALID;
    }
}

int64_t VSyncGenerator::ComputeListenerNextVSyncTimeStamp(const Listener& listener, int64_t now, int64_t referenceTime)
{
    int64_t lastVSyncTime = listener.lastTime_ + wakeupDelay_;
    if (now < lastVSyncTime) {
        now = lastVSyncTime;
    }

    now -= referenceTime;
    int64_t phase = phaseRecord_ + listener.phase_;
    now -= phase;
    if (now < 0) {
        if (vsyncMode_ == VSYNC_MODE_LTPO) {
            now -= periodRecord_;
        } else {
            now = -periodRecord_;
        }
    }
    int64_t numPeriod = now / periodRecord_;
    int64_t nextTime = (numPeriod + 1) * periodRecord_ + phase;
    nextTime += referenceTime;

    // 3 / 5 and 1 / 10 are just empirical value
    int64_t threshold = refreshRateIsChanged_ ? (1 * periodRecord_ / 10) : (3 * periodRecord_ / 5);
    // between 8000000(8ms) and 8500000(8.5ms)
    if (!refreshRateIsChanged_ && periodRecord_ > 8000000 && periodRecord_ < 8500000) {
        threshold = 4 * periodRecord_ / 5; // 4 / 5 is an empirical value
    }
    // 3 / 5 just empirical value
    if (((vsyncMode_ == VSYNC_MODE_LTPS) && (nextTime - listener.lastTime_ < (3 * periodRecord_ / 5))) ||
        ((vsyncMode_ == VSYNC_MODE_LTPO) && (nextTime - listener.lastTime_ < threshold))) {
        nextTime += periodRecord_;
    }

    nextTime -= wakeupDelay_;
    return nextTime;
}

std::vector<VSyncGenerator::Listener> VSyncGenerator::GetListenerTimeouted(
    int64_t now, int64_t occurTimestamp, int64_t referenceTime)
{
    std::vector<VSyncGenerator::Listener> ret;
    for (uint32_t i = 0; i < listeners_.size(); i++) {
        int64_t t = ComputeListenerNextVSyncTimeStamp(listeners_[i], occurTimestamp, referenceTime);
        if (t < now || (t - now < errorThreshold)) {
            listeners_[i].lastTime_ = t;
            ret.push_back(listeners_[i]);
        }
    }
    return ret;
}

std::vector<VSyncGenerator::Listener> VSyncGenerator::GetListenerTimeoutedLTPO(int64_t now, int64_t referenceTime)
{
    std::vector<VSyncGenerator::Listener> ret;
    for (uint32_t i = 0; i < listeners_.size(); i++) {
        int64_t t = ComputeListenerNextVSyncTimeStamp(listeners_[i], now, referenceTime);
        if (t - systemTime() < errorThreshold) {
            listeners_[i].lastTime_ = t;
            ret.push_back(listeners_[i]);
        }
    }
    refreshRateIsChanged_ = false;
    return ret;
}

VsyncError VSyncGenerator::UpdatePeriodLocked(int64_t period)
{
    VsyncError ret = VSYNC_ERROR_OK;
    uint32_t refreshRate = JudgeRefreshRateLocked(period);
    if ((pendingVsyncMode_ == VSYNC_MODE_LTPO) || (vsyncMode_ == VSYNC_MODE_LTPO)) {
        if ((refreshRate != 0) && ((currRefreshRate_ == refreshRate) || currRefreshRate_ == 0)) {
            period_ = period;
        } else {
            ScopedBytrace failedTrace("update period failed, refreshRate:" + std::to_string(refreshRate) +
                                    ", currRefreshRate_:" + std::to_string(currRefreshRate_));
            VLOGE("update period failed, refreshRate:%{public}u, currRefreshRate_:%{public}u, period:" VPUBI64,
                                    refreshRate, currRefreshRate_, period);
            ret = VSYNC_ERROR_API_FAILED;
        }
    } else {
        if (period != 0) {
            period_ = period;
        } else {
            ret = VSYNC_ERROR_API_FAILED;
        }
    }
    return ret;
}

VsyncError VSyncGenerator::UpdateReferenceTimeLocked(int64_t referenceTime)
{
    if ((pendingVsyncMode_ == VSYNC_MODE_LTPO) || (vsyncMode_ == VSYNC_MODE_LTPO)) {
        referenceTime_ = referenceTime - referenceTimeOffsetPulseNum_ * pulse_;
    } else {
        referenceTime_ = referenceTime;
    }
    return VSYNC_ERROR_OK;
}

void VSyncGenerator::SubScribeSystemAbility()
{
    VLOGD("%{public}s", __func__);
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        VLOGE("%{public}s failed to get system ability manager client", __func__);
        return;
    }
    std::string threadName = "VSyncGenerator";
    std::string strUid = std::to_string(getuid());
    std::string strPid = std::to_string(getpid());
    std::string strTid = std::to_string(gettid());

    saStatusChangeListener_ = new (std::nothrow)VSyncSystemAbilityListener(threadName, strUid, strPid, strTid);
    int32_t ret = systemAbilityManager->SubscribeSystemAbility(RES_SCHED_SYS_ABILITY_ID, saStatusChangeListener_);
    if (ret != ERR_OK) {
        VLOGE("%{public}s subscribe system ability %{public}d failed.", __func__, RES_SCHED_SYS_ABILITY_ID);
        saStatusChangeListener_ = nullptr;
    }
}

VsyncError VSyncGenerator::UpdateMode(int64_t period, int64_t phase, int64_t referenceTime)
{
    ScopedBytrace func("UpdateMode, period:" + std::to_string(period) +
                        ", phase:" + std::to_string(phase) +
                        ", referenceTime:" + std::to_string((referenceTime)) +
                        ", referenceTimeOffsetPulseNum_:" + std::to_string(referenceTimeOffsetPulseNum_));
    if (period < 0 || referenceTime < 0) {
        VLOGE("wrong parameter, period:" VPUBI64 ", referenceTime:" VPUBI64, period, referenceTime);
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    std::lock_guard<std::mutex> locker(mutex_);
    phase_ = phase;
    if (period != 0) {
        UpdatePeriodLocked(period);
    }
    UpdateReferenceTimeLocked(referenceTime);
    startRefresh_ = false;
    con_.notify_all();
    return VSYNC_ERROR_OK;
}

VsyncError VSyncGenerator::AddListener(int64_t phase, const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb)
{
    ScopedBytrace func("AddListener");
    std::lock_guard<std::mutex> locker(mutex_);
    if (cb == nullptr) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    Listener listener;
    listener.phase_ = phase;
    listener.callback_ = cb;
    listener.lastTime_ = systemTime() - period_ + phase_;

    listeners_.push_back(listener);

    size_t i = 0;
    for (; i < listenersRecord_.size(); i++) {
        if (listener.callback_ == listenersRecord_[i].callback_) {
            break;
        }
    }
    if (i == listenersRecord_.size()) {
        listenersRecord_.push_back(listener);
    }
    con_.notify_all();
    waitForTimeoutCon_.notify_all();
    return VSYNC_ERROR_OK;
}

uint32_t VSyncGenerator::JudgeRefreshRateLocked(int64_t period)
{
    if (period <= 0) {
        return 0;
    }
    int32_t actualRefreshRate = round(1.0/((double)period/1000000000.0)); // 1.0s == 1000000000.0ns
    int32_t refreshRate = actualRefreshRate;
    int32_t diff = 0;
    // 在actualRefreshRate附近找一个能被VSYNC_MAX_REFRESHRATE整除的刷新率作为训练pulse的参考刷新率
    while ((abs(refreshRate - actualRefreshRate) < MAX_REFRESHRATE_DEVIATION) &&
           (VSYNC_MAX_REFRESHRATE % refreshRate != 0)) {
        if (diff < 0) {
            diff = -diff;
        } else {
            diff = -diff - 1;
        }
        refreshRate = actualRefreshRate + diff;
    }
    if (VSYNC_MAX_REFRESHRATE % refreshRate != 0) {
        VLOGE("Not Support this refresh rate: %{public}d, update pulse failed.", actualRefreshRate);
        return 0;
    }
    pulse_ = period / (VSYNC_MAX_REFRESHRATE / refreshRate);
    return static_cast<uint32_t>(refreshRate);
}

VsyncError VSyncGenerator::ChangeGeneratorRefreshRateModel(const ListenerRefreshRateData &listenerRefreshRates,
                                                           const ListenerPhaseOffsetData &listenerPhaseOffset,
                                                           uint32_t generatorRefreshRate)
{
    ScopedBytrace func("ChangeGeneratorRefreshRateModel:" + std::to_string(generatorRefreshRate));
    std::lock_guard<std::mutex> locker(mutex_);
    if ((vsyncMode_ != VSYNC_MODE_LTPO) && (pendingVsyncMode_ != VSYNC_MODE_LTPO)) {
        ScopedBytrace trace("it's not ltpo mode.");
        return VSYNC_ERROR_NOT_SUPPORT;
    }
    if (pulse_ == 0) {
        ScopedBytrace trace("pulse is not ready!!!");
        VLOGE("pulse is not ready!!!");
        return VSYNC_ERROR_API_FAILED;
    }
    if ((generatorRefreshRate <= 0 || (VSYNC_MAX_REFRESHRATE % generatorRefreshRate != 0))) {
        ScopedBytrace trace("Not support this refresh rate: " + std::to_string(generatorRefreshRate));
        VLOGE("Not support this refresh rate: %{public}u", generatorRefreshRate);
        return VSYNC_ERROR_NOT_SUPPORT;
    }

    changingRefreshRates_ = listenerRefreshRates;
    needChangeRefreshRates_ = true;

    changingPhaseOffset_ = listenerPhaseOffset;
    needChangePhaseOffset_ = true;

    if (generatorRefreshRate != currRefreshRate_) {
        changingGeneratorRefreshRate_ = generatorRefreshRate;
        needChangeGeneratorRefreshRate_ = true;
    } else {
        ScopedBytrace trace("refreshRateNotChanged, generatorRefreshRate:" + std::to_string(generatorRefreshRate) +
                            ", currRefreshRate_:" + std::to_string(currRefreshRate_));
    }

    waitForTimeoutCon_.notify_all();
    return VSYNC_ERROR_OK;
}

int64_t VSyncGenerator::GetVSyncPulse()
{
    std::lock_guard<std::mutex> locker(mutex_);
    return pulse_;
}

VsyncError VSyncGenerator::SetVSyncMode(VSyncMode vsyncMode)
{
    ScopedBytrace func("SetVSyncMode:" + std::to_string(vsyncMode));
    std::lock_guard<std::mutex> locker(mutex_);
    pendingVsyncMode_ = vsyncMode;
    return VSYNC_ERROR_OK;
}

VSyncMode VSyncGenerator::GetVSyncMode()
{
    std::lock_guard<std::mutex> locker(mutex_);
    return vsyncMode_;
}

VsyncError VSyncGenerator::SetVSyncPhaseByPulseNum(int32_t phaseByPulseNum)
{
    std::lock_guard<std::mutex> locker(mutex_);
    referenceTimeOffsetPulseNum_ = phaseByPulseNum;
    defaultReferenceTimeOffsetPulseNum_ = phaseByPulseNum;
    return VSYNC_ERROR_OK;
}

VsyncError VSyncGenerator::SetReferenceTimeOffset(int32_t offsetByPulseNum)
{
    std::lock_guard<std::mutex> locker(mutex_);
    referenceTimeOffsetPulseNum_ = offsetByPulseNum;
    return VSYNC_ERROR_OK;
}

VsyncError VSyncGenerator::StartRefresh()
{
    std::lock_guard<std::mutex> lock(mutex_);
    startRefresh_ = true;
    referenceTimeOffsetPulseNum_ = defaultReferenceTimeOffsetPulseNum_;
    return VSYNC_ERROR_OK;
}

void VSyncGenerator::SetRSDistributor(sptr<VSyncDistributor> &rsVSyncDistributor)
{
    rsVSyncDistributor_ = rsVSyncDistributor;
}

void VSyncGenerator::PeriodCheckLocked(int64_t hardwareVsyncInterval)
{
    if (lastPeriod_ == period_) {
        if (abs(hardwareVsyncInterval - period_) > PERIOD_CHECK_THRESHOLD) {
            // if software period not changed, and hardwareVsyncInterval,
            // and software period is not the same, accumulate counter
            periodCheckCounter_++;
            ScopedBytrace trace("CounterAccumulated, lastPeriod_:" + std::to_string(lastPeriod_) +
                                ", period_:" + std::to_string(period_) +
                                ", hardwareVsyncInterval:" + std::to_string(hardwareVsyncInterval) +
                                ", periodCheckCounter_:" + std::to_string(periodCheckCounter_));
        }
    } else {
        // if period changed, record this period as lastPeriod_ and clear periodCheckCounter_
        lastPeriod_ = period_;
        periodCheckCounter_ = 0;
        ScopedBytrace trace("periodCheckCounter_ = 0");
    }
    // exit frameRateChanging status when the frame rate is inconsistent for 10 consecutive times.
    if (periodCheckCounter_ > 10) {
        ScopedBytrace trace("samePeriodCounter ERROR, period_:" + std::to_string(period_) +
                            ", hardwareVsyncInterval:" + std::to_string(hardwareVsyncInterval) +
                            ", pendingReferenceTime_:" + std::to_string(pendingReferenceTime_) +
                            ", referenceTime_:" + std::to_string(referenceTime_) +
                            ", referenceTimeDiff:" + std::to_string(abs(pendingReferenceTime_ - referenceTime_)));
        VLOGE("samePeriodCounter ERROR, period_:" VPUBI64 ", hardwareVsyncInterval:" VPUBI64
            ", pendingReferenceTime_:" VPUBI64 ", referenceTime_:" VPUBI64 ", referenceTimeDiff:" VPUBI64,
            period_, hardwareVsyncInterval, pendingReferenceTime_, referenceTime_,
            abs(pendingReferenceTime_ - referenceTime_));
        // end the frameRateChanging status
        frameRateChanging_ = false;
        ScopedBytrace forceEnd("frameRateChanging_ = false, forceEnd");
    }
}

VsyncError VSyncGenerator::CheckAndUpdateReferenceTime(int64_t hardwareVsyncInterval, int64_t referenceTime)
{
    if (hardwareVsyncInterval < 0 || referenceTime < 0) {
        VLOGE("wrong parameter, hardwareVsyncInterval:" VPUBI64 ", referenceTime:" VPUBI64,
                hardwareVsyncInterval, referenceTime);
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    std::lock_guard<std::mutex> locker(mutex_);
    if (pendingPeriod_ <= 0) {
        return VSYNC_ERROR_API_FAILED;
    }

    PeriodCheckLocked(hardwareVsyncInterval);

    if (rsVSyncDistributor_->IsDVsyncOn() ||
        ((abs(pendingReferenceTime_ - referenceTime_) < REFERENCETIME_CHECK_THRESHOLD) &&
        (abs(hardwareVsyncInterval - pendingPeriod_) < PERIOD_CHECK_THRESHOLD))) {
        // framerate has changed
        frameRateChanging_ = false;
        ScopedBytrace changeEnd("frameRateChanging_ = false");
        pendingPeriod_ = 0;
        int64_t actualOffset = referenceTime - pendingReferenceTime_;
        if (pulse_ == 0) {
            VLOGI("[%{public}s] pulse is not ready.", __func__);
            return VSYNC_ERROR_API_FAILED;
        }
        int32_t actualOffsetPulseNum = round((double)actualOffset/(double)pulse_);
        if (startRefresh_) {
            referenceTimeOffsetPulseNum_ = defaultReferenceTimeOffsetPulseNum_;
        } else {
            referenceTimeOffsetPulseNum_ = std::max(actualOffsetPulseNum, defaultReferenceTimeOffsetPulseNum_);
        }
        ScopedBytrace func("UpdateMode, referenceTime:" + std::to_string((referenceTime)) +
                        ", actualOffsetPulseNum:" + std::to_string((actualOffsetPulseNum)) +
                        ", referenceTimeOffsetPulseNum_:" + std::to_string(referenceTimeOffsetPulseNum_) +
                        ", startRefresh_:" + std::to_string(startRefresh_));
        UpdateReferenceTimeLocked(referenceTime);
        startRefresh_ = false;
    }
    return VSYNC_ERROR_OK;
}

VsyncError VSyncGenerator::RemoveListener(const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb)
{
    ScopedBytrace func("RemoveListener");
    std::lock_guard<std::mutex> locker(mutex_);
    if (cb == nullptr) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    bool removeFlag = false;
    auto it = listeners_.begin();
    for (; it < listeners_.end(); it++) {
        if (it->callback_ == cb) {
            listeners_.erase(it);
            removeFlag = true;
            break;
        }
    }
    if (!removeFlag) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    return VSYNC_ERROR_OK;
}

VsyncError VSyncGenerator::ChangePhaseOffset(const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb, int64_t offset)
{
    std::lock_guard<std::mutex> locker(mutex_);
    if (cb == nullptr) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    auto it = listeners_.begin();
    for (; it < listeners_.end(); it++) {
        if (it->callback_ == cb) {
            break;
        }
    }
    if (it != listeners_.end()) {
        it->phase_ = offset;
    } else {
        return VSYNC_ERROR_INVALID_OPERATING;
    }
    return VSYNC_ERROR_OK;
}

bool VSyncGenerator::IsEnable()
{
    std::lock_guard<std::mutex> locker(mutex_);
    return period_ > 0;
}

bool VSyncGenerator::GetFrameRateChaingStatus()
{
    std::lock_guard<std::mutex> locker(mutex_);
    return frameRateChanging_;
}

void VSyncGenerator::SetPendingMode(int64_t period, int64_t timestamp)
{
    if (period <= 0) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    pendingPeriod_ = period;
    pendingReferenceTime_ = timestamp;
}

void VSyncGenerator::Dump(std::string &result)
{
    result.append("\n-- VSyncGenerator --");
    result += "\nperiod:" + std::to_string(period_);
    result += "\nphase:" + std::to_string(phase_);
    result += "\nreferenceTime:" + std::to_string(referenceTime_);
    result += "\nvsyncMode:" + std::to_string(vsyncMode_);
    result += "\nperiodCheckCounter_:" + std::to_string(periodCheckCounter_);
}
} // namespace impl
sptr<VSyncGenerator> CreateVSyncGenerator()
{
    return impl::VSyncGenerator::GetInstance();
}

void DestroyVSyncGenerator()
{
    impl::VSyncGenerator::DeleteInstance();
}
}
}
