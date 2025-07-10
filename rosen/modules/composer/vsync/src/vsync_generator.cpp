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
#include <parameters.h>
#include "vsync_log.h"
#include <ctime>
#include <vsync_sampler.h>
#include <rs_trace.h>
#include "scoped_trace_fmt.h"

#ifdef COMPOSER_SCHED_ENABLE
#include "if_system_ability_manager.h"
#include <iservice_registry.h>
#include "system_ability_definition.h"
#endif

#if defined(RS_ENABLE_DVSYNC_2)
#include "dvsync.h"
#endif

namespace OHOS {
namespace Rosen {
namespace impl {
namespace {
static int64_t SystemTime()
{
    timespec t = {};
    clock_gettime(CLOCK_MONOTONIC, &t);
    return int64_t(t.tv_sec) * 1000000000LL + t.tv_nsec; // 1000000000ns == 1s
}

// 1.5ms
constexpr int64_t MAX_WALEUP_DELAY = 1500000;
constexpr int32_t THREAD_PRIORTY = -6;
constexpr int32_t SCHED_PRIORITY = 2;
constexpr int64_t ERROR_THRESHOLD = 500000;
constexpr int32_t MAX_REFRESHRATE_DEVIATION = 5; // ±5Hz
constexpr int64_t PERIOD_CHECK_THRESHOLD = 1000000; // 1000000ns == 1.0ms
constexpr int64_t DEFAULT_SOFT_VSYNC_PERIOD = 16000000; // 16000000ns == 16ms
constexpr int64_t REMAINING_TIME_THRESHOLD = 100000; // 100000ns == 0.1ms
constexpr int64_t REMAINING_TIME_THRESHOLD_FOR_LISTENER = 1000000; // 1000000ns == 1ms
constexpr int64_t ONE_SECOND_FOR_CALCUTE_FREQUENCY = 1000000000; // 1000000000ns == 1s
constexpr uint32_t MAX_LISTENERS_AMOUNT = 2;

// minimum ratio of dvsync thread
constexpr double DVSYNC_PERIOD_MIN_INTERVAL = 0.6;

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

uint32_t CalculateRefreshRate(int64_t period)
{
    static struct {
        int min;
        int max;
        int refreshRate;
    } rateSections[] = {
        {30000000, 35000000, 30}, // 30000000ns, 35000000ns
        {15000000, 18000000, 60}, // 15000000ns, 18000000ns
        {13000000, 15000000, 72}, // 13000000ns, 15000000ns
        {10000000, 12000000, 90}, // 10000000ns, 12000000ns
        {12000000, 13000000, 80}, // 12000000ns, 13000000ns
        {7500000, 9000000, 120}, // 7500000ns, 9000000ns
        {6000000, 7500000, 144}}; // 6000000ns, 7500000ns
    for (const auto& rateSection : rateSections) {
        if (period > rateSection.min && period < rateSection.max) {
            return rateSection.refreshRate;
        }
    }
    return 0;
}

sptr<OHOS::Rosen::VSyncGenerator> VSyncGenerator::GetInstance() noexcept
{
    std::call_once(createFlag_, []() {
        instance_ = new VSyncGenerator();
    });

    return instance_;
}

void VSyncGenerator::DeleteInstance() noexcept
{
    instance_ = nullptr;
}

VSyncGenerator::VSyncGenerator()
{
    period_ = DEFAULT_SOFT_VSYNC_PERIOD;
    vsyncThreadRunning_ = true;
    thread_ = std::thread([this] { this->ThreadLoop(); });
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
    SCOPED_DEBUG_TRACE_FMT("occurTimestamp:%ld, nextTimeStamp:%ld", occurTimestamp, nextTimeStamp);
    std::vector<Listener> listeners;
    uint32_t vsyncMaxRefreshRate = 360;
    {
        std::unique_lock<std::mutex> locker(mutex_);
        vsyncMaxRefreshRate = vsyncMaxRefreshRate_;
        int64_t newOccurTimestamp = SystemTime();
        if (isWakeup) {
            UpdateWakeupDelay(newOccurTimestamp, nextTimeStamp);
        }
        if (vsyncMode_ == VSYNC_MODE_LTPO) {
            listeners = GetListenerTimeoutedLTPO(occurTimestamp, occurReferenceTime);
        } else {
            listeners = GetListenerTimeouted(newOccurTimestamp, occurTimestamp, occurReferenceTime);
        }
        expectTimeFlag_ = false;
    }
    RS_TRACE_NAME_FMT("GenerateVsyncCount:%lu, period:%ld, currRefreshRate_:%u, vsyncMode_:%d",
        listeners.size(), periodRecord_, currRefreshRate_, vsyncMode_);
    for (uint32_t i = 0; i < listeners.size(); i++) {
        RS_TRACE_NAME_FMT("listener phase is %ld", listeners[i].phase_);
        if (listeners[i].callback_ != nullptr) {
            listeners[i].callback_->OnVSyncEvent(listeners[i].lastTime_,
                periodRecord_, currRefreshRate_, vsyncMode_, vsyncMaxRefreshRate);
        }
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
    while (true) {
        {
            std::unique_lock<std::mutex> locker(mutex_);
            if (vsyncThreadRunning_ == false) {
                break;
            }
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
            occurTimestamp = SystemTime();
            nextTimeStamp = ComputeNextVSyncTimeStamp(occurTimestamp, occurReferenceTime);
            if (nextTimeStamp == INT64_MAX) {
                ScopedBytrace func("VSyncGenerator: there has no request to be processed");
                if (vsyncThreadRunning_ == true) {
                    con_.wait(locker);
                }
                continue;
            } else if (vsyncMode_ == VSYNC_MODE_LTPO &&
                UpdateChangeDataLocked(occurTimestamp, occurReferenceTime, nextTimeStamp)) {
                ScopedBytrace func("VSyncGenerator: LTPO mode change");
                bool clearAllSamplesFlag = clearAllSamplesFlag_;
                clearAllSamplesFlag_ = false;
                locker.unlock();
                ClearAllSamplesInternal(clearAllSamplesFlag);
                if (appVSyncDistributor_ != nullptr) {
                    appVSyncDistributor_->RecordVsyncModeChange(currRefreshRate_, period_);
                }
                if (rsVSyncDistributor_ != nullptr) {
                    rsVSyncDistributor_->RecordVsyncModeChange(currRefreshRate_, period_);
                }
                continue;
            }
        }

        WaitForTimeout(occurTimestamp, nextTimeStamp, occurReferenceTime);
    }
}

void VSyncGenerator::WaitForTimeout(int64_t occurTimestamp, int64_t nextTimeStamp, int64_t occurReferenceTime)
{
    bool isWakeup = false;
    if (occurTimestamp < nextTimeStamp) {
        if (nextTimeStamp - occurTimestamp > periodRecord_ * 3 / 2) { // 3/2 means no more than 1.5 period
            RS_TRACE_NAME_FMT("WaitForTimeout occurTimestamp:%ld, nextTimeStamp:%ld", occurTimestamp, nextTimeStamp);
        }
        std::unique_lock<std::mutex> lck(waitForTimeoutMtx_);
        nextTimeStamp_ = nextTimeStamp;
        auto err = waitForTimeoutCon_.wait_for(lck, std::chrono::nanoseconds(nextTimeStamp - occurTimestamp));
        if (err == std::cv_status::timeout) {
            isWakeup = true;
        } else {
            ScopedBytrace func("VSyncGenerator::ThreadLoop : vsync generator was interrupted while waitting");
            return;
        }
    }
    ListenerVsyncEventCB(occurTimestamp, nextTimeStamp, occurReferenceTime, isWakeup);
}

void VSyncGenerator::WaitForTimeoutConNotifyLocked()
{
    int64_t curTime = SystemTime();
    if (curTime <= 0 || nextTimeStamp_ <= 0) {
        return;
    }
    int64_t remainingTime = nextTimeStamp_ - curTime;
    if (remainingTime > REMAINING_TIME_THRESHOLD) {
        waitForTimeoutCon_.notify_all();
    }
}

void VSyncGenerator::WaitForTimeoutConNotifyLockedForListener()
{
    int64_t curTime = SystemTime();
    if (curTime <= 0 || nextTimeStamp_ <= 0) {
        return;
    }
    int64_t remainingTime = nextTimeStamp_ - curTime;
    if (remainingTime > REMAINING_TIME_THRESHOLD_FOR_LISTENER) {
        waitForTimeoutCon_.notify_all();
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
    if (it->callback_ != nullptr) {
        it->callback_->OnPhaseOffsetChanged(phaseOffset);
    }
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
    if (it->callback_ != nullptr) {
        it->callback_->OnConnsRefreshRateChanged(changingRefreshRates_.refreshRates);
    }
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
    wakeupDelay_ = wakeupDelay_ > MAX_WALEUP_DELAY ? MAX_WALEUP_DELAY : wakeupDelay_;
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
    // Start of DVSync
    ComputeDVSyncListenerTimeStamp(dvsyncListener_, now, nextVSyncTime);
    // End of DVSync
    return nextVSyncTime;
}

// Start of DVSync
void VSyncGenerator::ComputeDVSyncListenerTimeStamp(const Listener& listener, int64_t now, int64_t &nextVSyncTime)
{
#if defined(RS_ENABLE_DVSYNC_2)
    int64_t t = INT64_MAX;
    DVSync::Instance().UpdateReferenceTimeAndPeriod(isLtpoNeedChange_, occurDvsyncReferenceTime_, dvsyncPeriodRecord_);
    if (dvsyncPeriodRecord_ != 0 && listener.callback_ != nullptr) {
        t = ComputeDVSyncListenerNextVSyncTimeStamp(listener, now, occurDvsyncReferenceTime_, dvsyncPeriodRecord_);
        nextVSyncTime = t < nextVSyncTime? t : nextVSyncTime;
        RS_TRACE_NAME_FMT("DVSync::UiDVSync ComputeNextVSyncTimeStamp t:%ld, dvsyncPeriod:%ld, dvsyncReferenceTime:%ld",
            t, dvsyncPeriodRecord_, occurDvsyncReferenceTime_);
    }
#endif
}

int64_t VSyncGenerator::SetCurrentRefreshRate(uint32_t currRefreshRate, uint32_t lastRefreshRate)
{
    int64_t delayTime = 0;
#if defined(RS_ENABLE_DVSYNC_2)
    std::lock_guard<std::mutex> locker(mutex_);
    delayTime = DVSync::Instance().SetCurrentRefreshRate(currRefreshRate, lastRefreshRate);
    if (currRefreshRate != 0 && delayTime != 0) {
        WaitForTimeoutConNotifyLocked();
        isLtpoNeedChange_ = true;
    }
#endif
    RS_TRACE_NAME_FMT("DVSync::UiDVSync setCurrentRefreshRate isLtpoNeedChange:%d, currRefreshRate:%u, delayTime:%ld",
        isLtpoNeedChange_, currRefreshRate, delayTime);
    return delayTime;
}

void VSyncGenerator::DVSyncRateChanged(uint32_t currRefreshRate, bool &frameRateChanged)
{
#if defined(RS_ENABLE_DVSYNC_2)
    uint32_t dvsyncRate = 0;
    bool dvsyncRateChanged = DVSync::Instance().DVSyncRateChanged(currRefreshRate, dvsyncRate);
    if (dvsyncRate != 0) {
        frameRateChanged = dvsyncRateChanged;
    }
#endif
}

int64_t VSyncGenerator::CollectDVSyncListener(const Listener &listener, int64_t now,
    std::vector<VSyncGenerator::Listener> &ret)
{
    int64_t t = INT64_MAX;
    if (dvsyncPeriodRecord_!= 0 && listener.callback_!= nullptr) {
        t = ComputeDVSyncListenerNextVSyncTimeStamp(listener, now, occurDvsyncReferenceTime_, dvsyncPeriodRecord_);
        if (t - SystemTime() < ERROR_THRESHOLD) {
            dvsyncListener_.lastTime_ = t;
            ret.push_back(dvsyncListener_);
#if defined(RS_ENABLE_DVSYNC_2)
            DVSync::Instance().SetToCurrentPeriod();
#endif
            RS_TRACE_NAME_FMT("DVSync::UiDVSync CollectDVSyncListener t:%ld, dvsyncPeriod:%ld, "
                "dvsyncReferenceTime:%ld", t, dvsyncPeriodRecord_, occurDvsyncReferenceTime_);
        }
    }
    return t;
}

int64_t VSyncGenerator::ComputeDVSyncListenerNextVSyncTimeStamp(const Listener &listener, int64_t now,
    int64_t referenceTime, int64_t period)
{
    if (period == 0) {
        return INT64_MAX;
    }
    int64_t lastVSyncTime = listener.lastTime_ + wakeupDelay_;
    if (now < lastVSyncTime) {
        now = lastVSyncTime;
    }
    now -= referenceTime;
    int64_t phase = phaseRecord_ + listener.phase_;
    now -= phase;
    if (now < 0) {
        if (vsyncMode_ == VSYNC_MODE_LTPO) {
            if (expectTimeFlag_ || refreshRateIsChanged_) { // Ensure that nextTime is not earlier than referenceTime.
                now += ((-now) / period) * period;
            }
            now -= period;
        } else {
            now = -period;
        }
    }
    int64_t numPeriod = now / period;
    int64_t nextTime = (numPeriod + 1) * period + phase;
    nextTime += referenceTime;
    int64_t threshold = static_cast<int64_t>(DVSYNC_PERIOD_MIN_INTERVAL * static_cast<double>(period));
    if (nextTime - listener.lastTime_ < threshold) {
        RS_TRACE_NAME_FMT("VSyncGenerator::ComputeDVSyncListenerNextVSyncTimeStamp "
            "add one more period:%ld, threshold:%ld", period, threshold);
        nextTime += period;
    }
    nextTime -= wakeupDelay_;
    return nextTime;
}

VsyncError VSyncGenerator::AddDVSyncListener(int64_t phase, const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb)
{
    ScopedBytrace func("AddDVSyncListener");
    std::lock_guard<std::mutex> locker(mutex_);
    if (cb == nullptr) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    Listener listener;
    listener.phase_ = phase;
    listener.callback_ = cb;
    listener.lastTime_ = SystemTime() - period_ + phase_;
    dvsyncListener_ = listener;
    con_.notify_all();
    WaitForTimeoutConNotifyLocked();
    return VSYNC_ERROR_OK;
}

bool VSyncGenerator::IsUiDvsyncOn()
{
    if (rsVSyncDistributor_ != nullptr) {
        return rsVSyncDistributor_->IsUiDvsyncOn();
    }
    return false;
}

VsyncError VSyncGenerator::RemoveDVSyncListener(const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb)
{
    ScopedBytrace func("RemoveDVSyncListener");
    std::lock_guard<std::mutex> locker(mutex_);
    if (cb == nullptr) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    dvsyncListener_ = {0, nullptr, 0};
    return VSYNC_ERROR_OK;
}
// End of DVSync

bool VSyncGenerator::CheckTimingCorrect(int64_t now, int64_t referenceTime, int64_t nextVSyncTime)
{
    bool isTimingCorrect = false;
    for (uint32_t i = 0; i < listeners_.size(); i++) {
        int64_t t = ComputeListenerNextVSyncTimeStamp(listeners_[i], now, referenceTime);
        if ((t - nextVSyncTime < ERROR_THRESHOLD) && (listeners_[i].isRS_ || listeners_[i].phase_ == 0)) {
            isTimingCorrect = true;
        }
    }
    return isTimingCorrect;
}

bool VSyncGenerator::UpdateChangeDataLocked(int64_t now, int64_t referenceTime, int64_t nextVSyncTime)
{
    bool modelChanged = false;

    // change referenceTime
    if (expectNextVsyncTime_ > 0) {
        RS_TRACE_NAME_FMT("UpdateChangeDataLocked, expectNextVsyncTime_:%ld", expectNextVsyncTime_);
        nextVSyncTime = expectNextVsyncTime_;
        expectNextVsyncTime_ = 0;
        referenceTime_ = nextVSyncTime;
        modelChanged = true;
        expectTimeFlag_ = true;
    } else {
        if (!CheckTimingCorrect(now, referenceTime, nextVSyncTime)) {
            return false;
        }
    }

    // update generate refreshRate
    if (needChangeGeneratorRefreshRate_) {
        currRefreshRate_ = changingGeneratorRefreshRate_;
        period_ = pulse_ * static_cast<int64_t>(vsyncMaxRefreshRate_ / currRefreshRate_);
        referenceTime_ = nextVSyncTime;
        changingGeneratorRefreshRate_ = 0; // reset
        needChangeGeneratorRefreshRate_ = false;
        refreshRateIsChanged_ = true;
        frameRateChanging_ = true;
        ScopedBytrace trace("frameRateChanging_ = true");
        targetPeriod_ = period_;
        clearAllSamplesFlag_ = true;
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

void VSyncGenerator::ClearAllSamplesInternal(bool clearAllSamplesFlag)
{
    if (clearAllSamplesFlag) {
        CreateVSyncSampler()->ClearAllSamples();
    }
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
            if (expectTimeFlag_ || refreshRateIsChanged_) { // Ensure that nextTime is not earlier than referenceTime.
                now += ((-now) / periodRecord_) * periodRecord_;
            }
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
    if (!refreshRateIsChanged_ && frameRateChanging_ && periodRecord_ > 8000000 && periodRecord_ < 8500000) {
        threshold = 4 * periodRecord_ / 5; // 4 / 5 is an empirical value
    }
    // 3 / 5 just empirical value
    if (((vsyncMode_ == VSYNC_MODE_LTPS) && (nextTime - listener.lastTime_ < (3 * periodRecord_ / 5))) ||
        ((vsyncMode_ == VSYNC_MODE_LTPO) && (nextTime - listener.lastTime_ < threshold))) {
        RS_TRACE_NAME_FMT("ComputeListenerNextVSyncTimeStamp add one more period:%ld, threshold:%ld",
            periodRecord_, threshold);
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
        if (t < now || (t - now < ERROR_THRESHOLD)) {
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
        if (t - SystemTime() < ERROR_THRESHOLD) {
            listeners_[i].lastTime_ = t;
            ret.push_back(listeners_[i]);
        }
    }
    // Start of DVSync
    CollectDVSyncListener(dvsyncListener_, now, ret);
    // End of DVSync
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
            RS_TRACE_NAME_FMT("update period failed, refreshRate:%u, currRefreshRate_:%d",
                refreshRate, currRefreshRate_);
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
    VLOGI("%{public}s", __func__);
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

    saStatusChangeListener_ = new VSyncSystemAbilityListener(threadName, strUid, strPid, strTid);
    int32_t ret = systemAbilityManager->SubscribeSystemAbility(RES_SCHED_SYS_ABILITY_ID, saStatusChangeListener_);
    if (ret != ERR_OK) {
        VLOGE("%{public}s subscribe system ability %{public}d failed.", __func__, RES_SCHED_SYS_ABILITY_ID);
        saStatusChangeListener_ = nullptr;
    }
}

VsyncError VSyncGenerator::UpdateMode(int64_t period, int64_t phase, int64_t referenceTime)
{
    std::lock_guard<std::mutex> locker(mutex_);
    RS_TRACE_NAME_FMT("UpdateMode, period:%ld, phase:%ld, referenceTime:%ld, referenceTimeOffsetPulseNum_:%d",
        period, phase, referenceTime, referenceTimeOffsetPulseNum_);
    if (period < 0 || referenceTime < 0) {
        VLOGE("wrong parameter, period:" VPUBI64 ", referenceTime:" VPUBI64, period, referenceTime);
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    phase_ = phase;
    if (period == 0 || UpdatePeriodLocked(period) == VSYNC_ERROR_OK) {
        UpdateReferenceTimeLocked(referenceTime);
    }
    startRefresh_ = false;
    con_.notify_all();
    return VSYNC_ERROR_OK;
}

bool VSyncGenerator::NeedPreexecuteAndUpdateTs(
    int64_t& timestamp, int64_t& period, int64_t& offset, int64_t lastVsyncTime)
{
    std::lock_guard<std::mutex> locker(mutex_);
    int64_t now = SystemTime();
    offset = (now - lastVsyncTime) % period_;
    if (period_ - offset > PERIOD_CHECK_THRESHOLD) {
        timestamp = now;
        period = period_;
        RS_TRACE_NAME_FMT("NeedPreexecuteAndUpdateTs, new referenceTime:%ld, timestamp:%ld, period:%ld,",
            referenceTime_, timestamp, period);
        return true;
    }
    return false;
}

VsyncError VSyncGenerator::AddListener(
    const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb, bool isRS, bool isUrgent)
{
    ScopedBytrace func("AddListener");
    std::lock_guard<std::mutex> locker(mutex_);
    if (cb == nullptr) {
        VLOGE("AddListener failed, cb is null.");
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    for (auto it = listeners_.begin(); it < listeners_.end(); ++it) {
        if (it->callback_ == cb) {
            VLOGI("this listener has been added.");
            return VSYNC_ERROR_OK;
        }
    }
    Listener listener;
    listener.phase_ = cb->GetPhaseOffset();
    listener.callback_ = cb;
    listener.lastTime_ = isUrgent ? SystemTime() : SystemTime() - period_ + phase_;
    listener.isRS_ = isRS;

    listeners_.push_back(listener);

    if (listeners_.size() > MAX_LISTENERS_AMOUNT) {
        VLOGE("AddListener, listeners size is out of range, size = %{public}zu", listeners_.size());
    }

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
    WaitForTimeoutConNotifyLockedForListener();
    return VSYNC_ERROR_OK;
}

uint32_t VSyncGenerator::JudgeRefreshRateLocked(int64_t period)
{
    if (period <= 0) {
        return 0;
    }
    int32_t actualRefreshRate = round(1.0/((double)period/1000000000.0)); // 1.0s == 1000000000.0ns
    if (actualRefreshRate == 0) { // actualRefreshRate is greater than 0
        return 0;
    }
    int32_t refreshRate = actualRefreshRate;
    int32_t diff = 0;
    // 在actualRefreshRate附近找一个能被vsyncMaxRefreshRate_整除的刷新率作为训练pulse的参考刷新率
    // refreshRate is greater than 0, and the value is in following range:
    // [max(1, actualRefreshRate - MAX_REFRESHRATE_DEVIATION), actualRefreshRate + MAX_REFRESHRATE_DEVIATION]
    while ((abs(refreshRate - actualRefreshRate) < MAX_REFRESHRATE_DEVIATION) &&
           (vsyncMaxRefreshRate_ % static_cast<uint32_t>(refreshRate) != 0)) {
        if (diff < 0) {
            diff = -diff;
        } else {
            diff = -diff - 1;
        }
        refreshRate = actualRefreshRate + diff;
    }
    if (vsyncMaxRefreshRate_ % static_cast<uint32_t>(refreshRate) != 0) {
        VLOGE("Not Support this refresh rate: %{public}d, update pulse failed.", actualRefreshRate);
        return 0;
    }
    pulse_ = period / static_cast<int64_t>(vsyncMaxRefreshRate_ / static_cast<uint32_t>(refreshRate));
    return static_cast<uint32_t>(refreshRate);
}

VsyncError VSyncGenerator::SetExpectNextVsyncTimeInternal(int64_t expectNextVsyncTime)
{
    if (expectNextVsyncTime <= 0) {
        return VSYNC_ERROR_OK;
    }
    auto now = SystemTime();
    int64_t expectTime = 0;
    if (expectNextVsyncTime - referenceTime_ > 0) {
        if (((expectNextVsyncTime - referenceTime_) % pulse_) < (pulse_ / 2)) { // check with 1/2 pulse
            expectTime = ((expectNextVsyncTime - referenceTime_) / pulse_) * pulse_ + referenceTime_;
        } else {
            expectTime = ((expectNextVsyncTime - referenceTime_) / pulse_ + 1) * pulse_ + referenceTime_;
        }
    }
    if (expectTime == 0 || expectTime - now > 100000000) { // 100ms == 100000000ns
        RS_TRACE_NAME_FMT("SetExpectNextVsyncTime Failed, expectTime:%ld, now:%ld, expectNextVsyncTime:%ld,"
            " referenceTime_:%ld", expectTime, now, expectNextVsyncTime, referenceTime_);
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    expectNextVsyncTime_ = expectTime;
    RS_TRACE_NAME_FMT("expectNextVsyncTime:%ld, expectNextVsyncTime_:%ld, diff:%ld", expectNextVsyncTime,
        expectNextVsyncTime_, (expectNextVsyncTime_ - expectNextVsyncTime));
    return VSYNC_ERROR_OK;
}

VsyncError VSyncGenerator::ChangeGeneratorRefreshRateModel(const ListenerRefreshRateData &listenerRefreshRates,
                                                           const ListenerPhaseOffsetData &listenerPhaseOffset,
                                                           uint32_t generatorRefreshRate,
                                                           int64_t &rsVsyncCount,
                                                           int64_t expectNextVsyncTime)
{
    if (rsVSyncDistributor_ != nullptr) {
        rsVsyncCount = rsVSyncDistributor_->GetVsyncCount();
    }
    RS_TRACE_NAME_FMT("ChangeGeneratorRefreshRateModel:%u, phaseByPulseNum:%d, expectNextVsyncTime:%ld",
        generatorRefreshRate, listenerPhaseOffset.phaseByPulseNum, expectNextVsyncTime);
    for (std::pair<uint64_t, uint32_t> rateVec : listenerRefreshRates.refreshRates) {
        uint64_t linkerId = rateVec.first;
        uint32_t refreshrate = rateVec.second;
        RS_TRACE_NAME_FMT("linkerId:%lu, refreshrate:%u", linkerId, refreshrate);
    }
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

    VsyncError ret = SetExpectNextVsyncTimeInternal(expectNextVsyncTime);

    if ((generatorRefreshRate <= 0 || (vsyncMaxRefreshRate_ % generatorRefreshRate != 0))) {
        RS_TRACE_NAME_FMT("Not support this refresh rate: %u", generatorRefreshRate);
        VLOGE("Not support this refresh rate: %{public}u", generatorRefreshRate);
        return VSYNC_ERROR_NOT_SUPPORT;
    }

    if (changingRefreshRates_.cb == nullptr) {
        changingRefreshRates_ = listenerRefreshRates;
    } else {
        UpdateChangeRefreshRatesLocked(listenerRefreshRates);
    }
    needChangeRefreshRates_ = true;

    changingPhaseOffset_ = listenerPhaseOffset;
    needChangePhaseOffset_ = true;

    if (changingGeneratorRefreshRate_ != 0 || generatorRefreshRate != currRefreshRate_) {
        changingGeneratorRefreshRate_ = generatorRefreshRate;
        needChangeGeneratorRefreshRate_ = true;
    } else {
        RS_TRACE_NAME_FMT("refreshRateNotChanged, generatorRefreshRate:%u, currRefreshRate_:%u",
            generatorRefreshRate, currRefreshRate_);
    }

    WaitForTimeoutConNotifyLocked();
    return ret;
}

void VSyncGenerator::UpdateChangeRefreshRatesLocked(const ListenerRefreshRateData &listenerRefreshRates)
{
    for (auto refreshRate : listenerRefreshRates.refreshRates) {
        bool found = false;
        for (auto it = changingRefreshRates_.refreshRates.begin();
             it != changingRefreshRates_.refreshRates.end(); it++) {
            if ((*it).first == refreshRate.first) { // first is linkerId
                (*it).second = refreshRate.second; // second is refreshRate
                found = true;
                break;
            }
        }
        if (!found) {
            changingRefreshRates_.refreshRates.push_back(refreshRate);
        }
    }
}

int64_t VSyncGenerator::GetVSyncPulse()
{
    std::lock_guard<std::mutex> locker(mutex_);
    return pulse_;
}

VsyncError VSyncGenerator::SetVSyncMode(VSyncMode vsyncMode)
{
    RS_TRACE_NAME_FMT("SetVSyncMode:%d", vsyncMode);
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

uint32_t VSyncGenerator::GetVsyncRefreshRate()
{
    std::lock_guard<std::mutex> locker(mutex_);
    if (period_ == 0) {
        return UINT32_MAX;
    }
    uint32_t refreshRate = CalculateRefreshRate(period_);
    if (refreshRate == 0) {
        refreshRate = std::round(static_cast<double>(ONE_SECOND_FOR_CALCUTE_FREQUENCY)
            / static_cast<double>(period_));
    }
    return refreshRate;
}

uint32_t VSyncGenerator::GetVSyncMaxRefreshRate()
{
    return vsyncMaxRefreshRate_;
}

VsyncError VSyncGenerator::SetVSyncMaxRefreshRate(uint32_t refreshRate)
{
    std::lock_guard<std::mutex> locker(mutex_);
    if (refreshRate < VSYNC_MAX_REFRESHRATE_RANGE_MIN ||
        refreshRate > VSYNC_MAX_REFRESHRATE_RANGE_MAX) {
        VLOGE("Not support max refresh rate: %{public}u", refreshRate);
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    vsyncMaxRefreshRate_ = refreshRate;
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
    RS_TRACE_NAME("StartRefresh");
    std::lock_guard<std::mutex> lock(mutex_);
    startRefresh_ = true;
    referenceTimeOffsetPulseNum_ = defaultReferenceTimeOffsetPulseNum_;
    return VSYNC_ERROR_OK;
}

void VSyncGenerator::SetRSDistributor(sptr<VSyncDistributor> &rsVSyncDistributor)
{
    std::lock_guard<std::mutex> lock(mutex_);
    rsVSyncDistributor_ = rsVSyncDistributor;
}

void VSyncGenerator::PeriodCheckLocked(int64_t hardwareVsyncInterval)
{
    if (lastPeriod_ == period_) {
        if (abs(hardwareVsyncInterval - period_) > PERIOD_CHECK_THRESHOLD) {
            // if software period not changed, and hardwareVsyncInterval,
            // and software period is not the same, accumulate counter
            periodCheckCounter_++;
            RS_TRACE_NAME_FMT("CounterAccumulated, lastPeriod_:%ld, period_:%ld, hardwareVsyncInterval:%ld,"
                " periodCheckCounter_:%d", lastPeriod_, period_, hardwareVsyncInterval, periodCheckCounter_);
        }
    } else {
        // if period changed, record this period as lastPeriod_ and clear periodCheckCounter_
        lastPeriod_ = period_;
        periodCheckCounter_ = 0;
        RS_TRACE_NAME("periodCheckCounter_ = 0");
    }
    // exit frameRateChanging status when the frame rate is inconsistent for 10 consecutive times.
    if (periodCheckCounter_ > 10) {
        RS_TRACE_NAME_FMT("samePeriodCounter ERROR, period_:%ld, hardwareVsyncInterval:%ld, pendingReferenceTime_:%ld"
            ", referenceTime_:%ld, referenceTimeDiff:%ld", period_, hardwareVsyncInterval, pendingReferenceTime_,
            referenceTime_, abs(pendingReferenceTime_ - referenceTime_));
        VLOGE("samePeriodCounter ERROR, period_:" VPUBI64 ", hardwareVsyncInterval:" VPUBI64
            ", pendingReferenceTime_:" VPUBI64 ", referenceTime_:" VPUBI64 ", referenceTimeDiff:" VPUBI64,
            period_, hardwareVsyncInterval, pendingReferenceTime_, referenceTime_,
            abs(pendingReferenceTime_ - referenceTime_));
        // end the frameRateChanging status
        frameRateChanging_ = false;
        ScopedBytrace forceEnd("frameRateChanging_ = false, forceEnd");
    }
}

void VSyncGenerator::SetAppDistributor(sptr<VSyncDistributor> &appVSyncDistributor)
{
    std::lock_guard<std::mutex> lock(mutex_);
    appVSyncDistributor_ = appVSyncDistributor;
}

void VSyncGenerator::CalculateReferenceTimeOffsetPulseNumLocked(int64_t referenceTime)
{
    int64_t actualOffset = referenceTime - pendingReferenceTime_;
    int32_t actualOffsetPulseNum = round((double)actualOffset/(double)pulse_);
    if (startRefresh_ || (defaultReferenceTimeOffsetPulseNum_ == 0)) {
        referenceTimeOffsetPulseNum_ = defaultReferenceTimeOffsetPulseNum_;
    } else {
        referenceTimeOffsetPulseNum_ = std::max(actualOffsetPulseNum, defaultReferenceTimeOffsetPulseNum_);
    }
    RS_TRACE_NAME_FMT("UpdateMode, referenceTime:%ld, actualOffsetPulseNum:%d, referenceTimeOffsetPulseNum_:%d"
        ", startRefresh_:%d, period:%ld", referenceTime, actualOffsetPulseNum, referenceTimeOffsetPulseNum_,
        startRefresh_, pendingPeriod_);
}

int64_t VSyncGenerator::GetVSyncOffset()
{
    std::lock_guard<std::mutex> locker(mutex_);
    return vsyncOffset_;
}

VsyncError VSyncGenerator::CheckAndUpdateReferenceTime(int64_t hardwareVsyncInterval, int64_t referenceTime)
{
    if (hardwareVsyncInterval < 0 || referenceTime < 0) {
        VLOGE("wrong parameter, hardwareVsyncInterval:" VPUBI64 ", referenceTime:" VPUBI64,
                hardwareVsyncInterval, referenceTime);
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    std::lock_guard<std::mutex> locker(mutex_);
    if ((pendingPeriod_ <= 0 && targetPeriod_ <= 0) || pulse_ == 0) {
        frameRateChanging_ = false;
        VLOGE("[%{public}s] Failed, pendingPeriod_:" VPUBI64 ", targetPeriod_:" VPUBI64 ", pulse_:" VPUBI64,
            __func__, pendingPeriod_, targetPeriod_, pulse_);
        return VSYNC_ERROR_API_FAILED;
    }

    PeriodCheckLocked(hardwareVsyncInterval);

    if (((abs(hardwareVsyncInterval - pendingPeriod_) < PERIOD_CHECK_THRESHOLD) &&
        (abs(hardwareVsyncInterval - targetPeriod_) < PERIOD_CHECK_THRESHOLD || targetPeriod_ == 0))) {
        // framerate has changed
        frameRateChanging_ = false;
        ScopedBytrace changeEnd("frameRateChanging_ = false");
        CalculateReferenceTimeOffsetPulseNumLocked(referenceTime);
        UpdateReferenceTimeLocked(referenceTime);
        bool needNotify = true;
        uint32_t periodRefreshRate = CalculateRefreshRate(period_);
        uint32_t pendingPeriodRefreshRate = CalculateRefreshRate(pendingPeriod_);
        if (pendingPeriodRefreshRate != 0) {
            uint32_t periodPulseNum = vsyncMaxRefreshRate_ / pendingPeriodRefreshRate;
            vsyncOffset_ = (referenceTimeOffsetPulseNum_ % static_cast<int32_t>(periodPulseNum)) * pulse_;
            RS_TRACE_NAME_FMT("vsyncOffset_:%ld", vsyncOffset_);
        }
        // 120hz, 90hz, 60hz
        if (((periodRefreshRate == 120) || (periodRefreshRate == 90)) && (pendingPeriodRefreshRate == 60)) {
            needNotify = false;
        }
        if ((periodRefreshRate != 0) && (periodRefreshRate == pendingPeriodRefreshRate)) {
            RS_TRACE_NAME_FMT("period not changed, period:%ld", period_);
            needNotify = false;
        } else {
            UpdatePeriodLocked(pendingPeriod_);
        }
        if (needNotify) {
            WaitForTimeoutConNotifyLocked();
        }
        pendingPeriod_ = 0;
        targetPeriod_ = 0;
        startRefresh_ = false;
    }
    return VSYNC_ERROR_OK;
}

VsyncError VSyncGenerator::RemoveListener(const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb)
{
    ScopedBytrace func("RemoveListener");
    std::lock_guard<std::mutex> locker(mutex_);
    if (cb == nullptr) {
        VLOGE("RemoveListener failed, cb is null.");
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
        VLOGE("RemoveListener, not found, size = %{public}zu", listeners_.size());
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

void VSyncGenerator::SetFrameRateChangingStatus(bool frameRateChanging)
{
    std::lock_guard<std::mutex> locker(mutex_);
    frameRateChanging_ = frameRateChanging;
}

void VSyncGenerator::SetPendingMode(int64_t period, int64_t timestamp)
{
    if (period <= 0) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    pendingPeriod_ = period;
    pendingReferenceTime_ = timestamp;
    if (rsVSyncDistributor_ != nullptr) {
        rsVSyncDistributor_->UpdatePendingReferenceTime(pendingReferenceTime_);
    }
}

void VSyncGenerator::Dump(std::string &result)
{
    std::unique_lock<std::mutex> lock(mutex_);
    result.append("\n-- VSyncGenerator --");
    result += "\nperiod:" + std::to_string(period_);
    result += "\nphase:" + std::to_string(phase_);
    result += "\nreferenceTime:" + std::to_string(referenceTime_);
    result += "\nvsyncMode:" + std::to_string(vsyncMode_);
    result += "\nperiodCheckCounter_:" + std::to_string(periodCheckCounter_);
}

bool VSyncGenerator::CheckSampleIsAdaptive(int64_t hardwareVsyncInterval)
{
    std::unique_lock<std::mutex> lock(mutex_);
    return hardwareVsyncInterval > period_ + PERIOD_CHECK_THRESHOLD;
}

void VSyncGenerator::PrintGeneratorStatus()
{
    std::unique_lock<std::mutex> lock(mutex_);
    VLOGW("[Info]PrintGeneratorStatus, period:" VPUBI64 ", phase:" VPUBI64 ", referenceTime:" VPUBI64
        ", vsyncMode:%{public}d, listeners size:%{public}u", period_, phase_, referenceTime_, vsyncMode_,
        static_cast<uint32_t>(listeners_.size()));
    for (uint32_t i = 0; i < listeners_.size(); i++) {
        VLOGW("[Info]i:%{public}u, listener phase is " VPUBI64 ", timeStamp is " VPUBI64,
            i, listeners_[i].phase_, listeners_[i].lastTime_);
    }
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
