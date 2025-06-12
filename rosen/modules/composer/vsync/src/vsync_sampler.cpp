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

#include "vsync_sampler.h"
#include <cmath>
#include "vsync_generator.h"
#include "vsync_log.h"
#include <cstdint>
#include <mutex>
#include <scoped_bytrace.h>
#include <string>
#include <rs_trace.h>

namespace OHOS {
namespace Rosen {
namespace impl {
std::once_flag VSyncSampler::createFlag_;
sptr<OHOS::Rosen::VSyncSampler> VSyncSampler::instance_ = nullptr;

namespace {
constexpr double PI = 3.1415926;
constexpr double ERROR_THRESHOLD = 160000000000.0; // 400 usec squared
constexpr int32_t INVALID_TIMESTAMP = -1;
constexpr uint32_t MINES_SAMPLE_NUMS = 3;
constexpr uint32_t SAMPLES_INTERVAL_DIFF_NUMS = 2;
constexpr int64_t MAX_IDLE_TIME_THRESHOLD = 900000000; // 900000000ns == 900ms
constexpr double SAMPLE_VARIANCE_THRESHOLD = 250000000000.0; // 500 usec squared
constexpr int64_t INSPECTION_PERIOD = 5000000000;

static int64_t SystemTime()
{
    timespec t = {};
    clock_gettime(CLOCK_MONOTONIC, &t);
    return int64_t(t.tv_sec) * 1000000000LL + t.tv_nsec; // 1000000000ns == 1s
}
}
sptr<OHOS::Rosen::VSyncSampler> VSyncSampler::GetInstance() noexcept
{
    std::call_once(createFlag_, []() {
        auto vsyncSampler = new VSyncSampler();
        instance_ = vsyncSampler;
    });

    return instance_;
}

VSyncSampler::VSyncSampler()
    : period_(0), phase_(0), referenceTime_(0),
    error_(0), firstSampleIndex_(0), numSamples_(0),
    modeUpdated_(false)
{
}

void VSyncSampler::ResetErrorLocked()
{
    presentFenceTimeOffset_ = 0;
    error_ = 0;
    for (uint32_t i = 0; i < NUM_PRESENT; i++) {
        presentFenceTime_[i] = INVALID_TIMESTAMP;
    }
}

void VSyncSampler::SetAdaptive(bool isAdaptive)
{
    if (isAdaptive_.load() == isAdaptive) {
        return;
    }
    lastAdaptiveTime_ = 0;
    isAdaptive_.store(isAdaptive);
}

void VSyncSampler::SetVsyncEnabledScreenId(uint64_t vsyncEnabledScreenId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RS_TRACE_NAME_FMT("SetVsyncEnabledScreenId:%lu", vsyncEnabledScreenId);
    VLOGI("SetVsyncEnabledScreenId:" VPUBU64, vsyncEnabledScreenId);
    vsyncEnabledScreenId_ = vsyncEnabledScreenId;
}

uint64_t VSyncSampler::GetVsyncEnabledScreenId()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return vsyncEnabledScreenId_;
}

void VSyncSampler::SetVsyncSamplerEnabled(bool enabled)
{
    RS_TRACE_NAME_FMT("HdiOutput::SetVsyncSamplerEnabled, enableVsyncSample_:%d", enabled);
    VLOGI("Change enableVsyncSample_, value is %{public}d", enabled);
    enableVsyncSample_.store(enabled);
}

bool VSyncSampler::GetVsyncSamplerEnabled()
{
    return enableVsyncSample_.load();
}

int32_t VSyncSampler::StartSample(bool forceReSample)
{
    RS_TRACE_NAME_FMT("HdiOutput::StartVSyncSampler, forceReSample:%d", forceReSample);
    if (!enableVsyncSample_.load()) {
        RS_TRACE_NAME_FMT("disabled vsyncSample");
        return VSYNC_ERROR_API_FAILED;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (!forceReSample && hardwareVSyncStatus_) {
        RS_TRACE_NAME("Already Start Sample.");
        VLOGD("Already Start Sample.");
        return VSYNC_ERROR_OK;
    }
    VLOGD("Enable Screen Vsync");
    numSamples_ = 0;
    modeUpdated_ = false;
    SetScreenVsyncEnabledInRSMainThreadLocked(vsyncEnabledScreenId_, true);
    return VSYNC_ERROR_OK;
}

void VSyncSampler::ClearAllSamples()
{
    ScopedBytrace func("ClearAllSamples");
    std::lock_guard<std::mutex> lock(mutex_);
    numSamples_ = 0;
}

void VSyncSampler::SetHardwareVSyncStatus(bool enabled)
{
    std::lock_guard<std::mutex> lock(mutex_);
    hardwareVSyncStatus_ = enabled;
}

void VSyncSampler::RecordDisplayVSyncStatus(bool enabled)
{
    std::lock_guard<std::mutex> lock(mutex_);
    displayVSyncStatus_ = enabled;
}

void VSyncSampler::RollbackHardwareVSyncStatus()
{
    std::lock_guard<std::mutex> lock(mutex_);
    hardwareVSyncStatus_ = displayVSyncStatus_;
}

void VSyncSampler::RegSetScreenVsyncEnabledCallback(VSyncSampler::SetScreenVsyncEnabledCallback cb)
{
    std::lock_guard<std::mutex> lock(mutex_);
    setScreenVsyncEnabledCallback_ = cb;
}

void VSyncSampler::SetScreenVsyncEnabledInRSMainThread(uint64_t screenId, bool enabled)
{
    std::lock_guard<std::mutex> lock(mutex_);
    SetScreenVsyncEnabledInRSMainThreadLocked(screenId, enabled);
}

void VSyncSampler::SetScreenVsyncEnabledInRSMainThreadLocked(uint64_t screenId, bool enabled)
{
    if (setScreenVsyncEnabledCallback_ == nullptr) {
        VLOGE("SetScreenVsyncEnabled:%{public}d failed, cb is null", enabled);
        return;
    }
    hardwareVSyncStatus_ = enabled;
    setScreenVsyncEnabledCallback_(screenId, enabled);
}

bool VSyncSampler::AddSample(int64_t timeStamp)
{
    if (timeStamp < 0) {
        return true;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (!hardwareVSyncStatus_) {
        return true;
    }
    if (numSamples_ > 0) {
        auto preSample = samples_[(firstSampleIndex_ + numSamples_ - 1) % MAX_SAMPLES];
        auto intervalStamp = timeStamp - preSample;

        if (intervalStamp <= 0) {
            RS_TRACE_NAME_FMT("VSyncSampler::AddSample, invalid sample, preSample is larger");
            numSamples_ = 0;
            return true;
        }
        
        if (isAdaptive_.load() && CreateVSyncGenerator()->CheckSampleIsAdaptive(intervalStamp)) {
            RS_TRACE_NAME_FMT("VSyncSampler::AddSample, adaptive sample, intervalStamp:%ld", intervalStamp);
            numSamples_ = 0;
            lastAdaptiveTime_.store(SystemTime());
            return true;
        }
    }

    if (isAdaptive_.load() && SystemTime() - lastAdaptiveTime_.load() < INSPECTION_PERIOD) {
        return true;
    }

    if (numSamples_ < MAX_SAMPLES - 1) {
        numSamples_++;
    } else {
        firstSampleIndex_ = (firstSampleIndex_ + 1) % MAX_SAMPLES;
    }

    if (firstSampleIndex_ + numSamples_ >= 1) {
        uint32_t index = (firstSampleIndex_ + numSamples_ - 1) % MAX_SAMPLES;
        samples_[index] = timeStamp;
    }
    
    UpdateReferenceTimeLocked();
    UpdateModeLocked();

    if (numResyncSamplesSincePresent_++ > MAX_SAMPLES_WITHOUT_PRESENT) {
        ResetErrorLocked();
    }

    // 1/2 just a empirical value
    bool shouldDisableScreenVsync = modeUpdated_ && (error_ < ERROR_THRESHOLD / 2);
    if (shouldDisableScreenVsync) {
        // disabled screen vsync in rsMainThread
        VLOGD("Disable Screen Vsync");
        SetScreenVsyncEnabledInRSMainThreadLocked(vsyncEnabledScreenId_, false);
    }

    return !shouldDisableScreenVsync;
}

void VSyncSampler::UpdateReferenceTimeLocked()
{
    bool isFrameRateChanging = CreateVSyncGenerator()->GetFrameRateChaingStatus();
    // update referenceTime at the first sample, unless in adaptive sync mode
    if (!isFrameRateChanging && (numSamples_ == 1) && !isAdaptive_.load()) {
        phase_ = 0;
        referenceTime_ = samples_[firstSampleIndex_];
        CheckIfFirstRefreshAfterIdleLocked();
        CreateVSyncGenerator()->UpdateMode(0, phase_, referenceTime_);
    } else if (isFrameRateChanging && (numSamples_ >= 2)) { // at least 2 samples
        int64_t prevSample = samples_[(firstSampleIndex_ + numSamples_ - 2) % MAX_SAMPLES]; // at least 2 samples
        int64_t latestSample = samples_[(firstSampleIndex_ + numSamples_ - 1) % MAX_SAMPLES];
        CheckIfFirstRefreshAfterIdleLocked();
        CreateVSyncGenerator()->CheckAndUpdateReferenceTime(latestSample - prevSample, prevSample);
    }
}

void VSyncSampler::UpdateModeLocked()
{
    if (!CreateVSyncGenerator()->GetFrameRateChaingStatus() && (numSamples_ >= MIN_SAMPLES_FOR_UPDATE)) {
        int64_t sum = 0;
        int64_t min = INT64_MAX;
        int64_t max = 0;
        int64_t diffPrev = 0;
        int64_t diff = 0;
        double variance = 0;
        for (uint32_t i = 1; i < numSamples_; i++) {
            int64_t prevSample = samples_[(firstSampleIndex_ + i - 1 + MAX_SAMPLES) % MAX_SAMPLES];
            int64_t currentSample = samples_[(firstSampleIndex_ + i) % MAX_SAMPLES];
            diffPrev = diff;
            diff = currentSample - prevSample;
            if (diffPrev != 0) {
                int64_t delta = diff - diffPrev;
                variance += pow(static_cast<double>(delta), 2); // the 2nd power of delta
            }
            min = min < diff ? min : diff;
            max = max > diff ? max : diff;
            sum += diff;
        }
        variance /= (numSamples_ - SAMPLES_INTERVAL_DIFF_NUMS);
        if (variance > SAMPLE_VARIANCE_THRESHOLD) {
            // keep only the latest 5 samples, and sample the next timestamp.
            firstSampleIndex_ = (firstSampleIndex_ + numSamples_ - MIN_SAMPLES_FOR_UPDATE + 1) % MAX_SAMPLES;
            numSamples_ = MIN_SAMPLES_FOR_UPDATE - 1;
            referenceTime_ = samples_[firstSampleIndex_];
            return;
        }

        sum -= min;
        sum -= max;

        period_ = sum / (int64_t)(numSamples_ - MINES_SAMPLE_NUMS);
        if (period_ <= 0) {
            return;
        }

        referenceTime_ = samples_[firstSampleIndex_];

        ComputePhaseLocked();

        modeUpdated_ = true;
        CheckIfFirstRefreshAfterIdleLocked();
        CreateVSyncGenerator()->UpdateMode(period_, phase_, referenceTime_);
        pendingPeriod_ = period_;
    }
}

void VSyncSampler::UpdateErrorLocked()
{
    if (!modeUpdated_ || (period_ <= 0)) {
        return;
    }

    int numErrSamples = 0;
    double sqErrSum = 0;

    for (uint32_t i = 0; i < NUM_PRESENT; i++) {
        int64_t t = presentFenceTime_[i];
        if (t <= 0) {
            continue;
        }

        int64_t sample = t - referenceTime_;
        if (sample <= phase_) {
            continue;
        }

        int64_t sampleErr = (sample - phase_) % period_;
        // 1/2 just a empirical value
        if (sampleErr > period_ / 2) {
            sampleErr -= period_;
        }
        sqErrSum += pow(static_cast<double>(sampleErr), 2); // the 2nd power of sampleErr
        numErrSamples++;
    }

    if (numErrSamples > 0) {
        error_ = sqErrSum / numErrSamples;
    } else {
        error_ = 0;
    }
}

bool VSyncSampler::AddPresentFenceTime(uint32_t screenId, int64_t timestamp)
{
    if (timestamp < 0) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (screenId != vsyncEnabledScreenId_) {
        return false;
    }
    presentFenceTime_[presentFenceTimeOffset_] = timestamp;

    presentFenceTimeOffset_ = (presentFenceTimeOffset_ + 1) % NUM_PRESENT;
    numResyncSamplesSincePresent_ = 0;

    UpdateErrorLocked();
    if (error_ > ERROR_THRESHOLD) {
        RS_TRACE_NAME_FMT("PresentFenceTime error_:%lf", error_);
    }

    return !modeUpdated_ || error_ > ERROR_THRESHOLD;
}

void VSyncSampler::CheckIfFirstRefreshAfterIdleLocked()
{
    if (presentFenceTimeOffset_ + NUM_PRESENT < 1) {
        return;
    }
    int64_t curFenceTimeStamp = presentFenceTime_[presentFenceTimeOffset_];
    int64_t prevFenceTimeStamp = presentFenceTime_[(presentFenceTimeOffset_ + NUM_PRESENT - 1) % NUM_PRESENT];
    if ((curFenceTimeStamp != INVALID_TIMESTAMP) && (prevFenceTimeStamp != INVALID_TIMESTAMP) &&
        (curFenceTimeStamp - prevFenceTimeStamp > MAX_IDLE_TIME_THRESHOLD)) {
        CreateVSyncGenerator()->StartRefresh();
    }
}

void VSyncSampler::ComputePhaseLocked()
{
    double scale = 2.0 * PI / period_;
    double deltaAvgX = 0;
    double deltaAvgY = 0;
    for (uint32_t i = 1; i < numSamples_; i++) {
        double delta = (samples_[(firstSampleIndex_ + i) % MAX_SAMPLES] - referenceTime_) % period_ * scale;
        deltaAvgX += cos(delta);
        deltaAvgY += sin(delta);
    }

    deltaAvgX /= double(numSamples_ - 1);
    deltaAvgY /= double(numSamples_ - 1);

    phase_ = int64_t(::atan2(deltaAvgY, deltaAvgX) / scale);
}

int64_t VSyncSampler::GetPeriod() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return period_;
}

int64_t VSyncSampler::GetPhase() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return phase_;
}

int64_t VSyncSampler::GetRefrenceTime() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return referenceTime_;
}

int64_t VSyncSampler::GetHardwarePeriod() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    int64_t period = period_;
    if (!modeUpdated_ && pendingPeriod_ != 0) {
        period = pendingPeriod_;
    }
    return period;
}

void VSyncSampler::SetPendingPeriod(int64_t period)
{
    if (period <= 0) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    pendingPeriod_ = period;
    CreateVSyncGenerator()->SetFrameRateChangingStatus(true);
}

void VSyncSampler::Dump(std::string &result)
{
    std::lock_guard<std::mutex> lock(mutex_);
    result.append("\n-- VSyncSampler --");
    result += "\nperiod:" + std::to_string(period_);
    result += "\nphase:" + std::to_string(phase_);
    result += "\nreferenceTime:" + std::to_string(referenceTime_);
    result += "\nmodeUpdated:" + std::to_string(modeUpdated_);
    result += "\nhardwareVSyncStatus:" + std::to_string(hardwareVSyncStatus_);
    result += "\nnumSamples:" + std::to_string(numSamples_);
    result += "\nsamples:[";
    for (uint32_t i = 0; i < numSamples_; i++) {
        result += std::to_string(samples_[(firstSampleIndex_ + i) % MAX_SAMPLES]) + ",";
    }
    result += "]";
    result += "\npresentFenceTime:[";
    for (uint32_t i = 0; i < NUM_PRESENT; i++) {
        result += std::to_string(presentFenceTime_[i]) + ",";
    }
    result += "]";
    result += "\npresentFenceTimeOffset:" + std::to_string(presentFenceTimeOffset_);
    result += "\nvsyncEnabledScreenId:" + std::to_string(vsyncEnabledScreenId_);
}

VSyncSampler::~VSyncSampler()
{
}
} // namespace impl

sptr<VSyncSampler> CreateVSyncSampler()
{
    return impl::VSyncSampler::GetInstance();
}
}
}