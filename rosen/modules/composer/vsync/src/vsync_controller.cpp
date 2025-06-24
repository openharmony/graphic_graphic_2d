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

#include "vsync_controller.h"
#include <scoped_bytrace.h>
#include "vsync_log.h"

namespace OHOS {
namespace Rosen {
VSyncController::VSyncController(const sptr<VSyncGenerator> &geng, int64_t offset)
    : generator_(geng), callbackMutex_(), callback_(nullptr),
    offsetMutex_(), phaseOffset_(offset), normalPhaseOffset_(offset), enabled_(false), isDirectly_(false)
{
}

VSyncController::~VSyncController()
{
}

void VSyncController::ChangeAdaptiveStatus(bool isAdaptive)
{
    if (!isAdaptive) {
        ResetOffset();
    }
}
 
void VSyncController::AdjustAdaptiveOffset(int64_t period, int64_t offset)
{
    if (period <= 0) {
        VLOGE("VSyncController::AdjustAdaptiveOffset: period is illegal.");
        return;
    }
    std::lock_guard<std::mutex> locker(offsetMutex_);
    auto remainder = (phaseOffset_ + offset) % period;
 
    phaseOffset_ = remainder;
}
 
void VSyncController::ResetOffset()
{
    std::lock_guard<std::mutex> locker(offsetMutex_);
    phaseOffset_ = normalPhaseOffset_;
}

bool VSyncController::NeedPreexecuteAndUpdateTs(int64_t& timestamp, int64_t& period)
{
    if (generator_ == nullptr) {
        return false;
    }
    const sptr<VSyncGenerator> generator = generator_.promote();
    if (generator == nullptr) {
        return false;
    }
    int64_t offset = 0;
    bool flag = generator->NeedPreexecuteAndUpdateTs(timestamp, period, offset, lastVsyncTime_.load());
    if (flag && offset > 0) {
        AdjustAdaptiveOffset(period, offset);
    }
 
    return flag;
}

VsyncError VSyncController::SetEnable(bool enable, bool& isGeneratorEnable)
{
    if (generator_ == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    const sptr<VSyncGenerator> generator = generator_.promote();
    if (generator == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    int64_t phaseOffset;
    {
        std::lock_guard<std::mutex> locker(offsetMutex_);
        phaseOffset = phaseOffset_;
    }
    VsyncError ret = VSYNC_ERROR_OK;
    if (enable) {
        // If the sampler does not complete the sampling work, the generator does not work
        // We need to tell the distributor to use the software vsync
        isGeneratorEnable = generator->IsEnable();
        if (isGeneratorEnable) {
            ret = generator->AddListener(phaseOffset, this, isRS_, isDirectly_);
            if (ret != VSYNC_ERROR_OK) {
                isGeneratorEnable = false;
            }
        } else {
            ret = VSYNC_ERROR_API_FAILED;
        }
    } else {
        ret = generator->RemoveListener(this);
        if (ret == VSYNC_ERROR_OK) {
            isGeneratorEnable = false;
        }
    }

    isRS_ = false;
    isDirectly_ = false;
    enabled_ = isGeneratorEnable;
    return ret;
}

VsyncError VSyncController::SetCallback(Callback *cb)
{
    if (cb == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    std::lock_guard<std::mutex> locker(callbackMutex_);
    callback_ = cb;
    return VSYNC_ERROR_OK;
}

VsyncError VSyncController::SetPhaseOffset(int64_t offset)
{
    if (generator_ == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    const sptr<VSyncGenerator> generator = generator_.promote();
    if (generator == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    {
        std::lock_guard<std::mutex> locker(offsetMutex_);
        phaseOffset_ = offset;
        normalPhaseOffset_ = offset;
    }
    return generator->ChangePhaseOffset(this, offset);
}

void VSyncController::OnVSyncEvent(int64_t now, int64_t period,
    uint32_t refreshRate, VSyncMode vsyncMode, uint32_t vsyncMaxRefreshRate)
{
    Callback *cb = nullptr;
    {
        std::lock_guard<std::mutex> locker(callbackMutex_);
        cb = callback_;
    }
    if (cb != nullptr) {
        lastVsyncTime_ = now;
        cb->OnVSyncEvent(now, period, refreshRate, vsyncMode, vsyncMaxRefreshRate);
    }
}

void VSyncController::OnPhaseOffsetChanged(int64_t phaseOffset)
{
    std::lock_guard<std::mutex> locker(offsetMutex_);
    phaseOffset_ = phaseOffset;
    normalPhaseOffset_ = phaseOffset;
}

/* std::pair<id, refresh rate> */
void VSyncController::OnConnsRefreshRateChanged(const std::vector<std::pair<uint64_t, uint32_t>> &refreshRates)
{
    Callback *cb = nullptr;
    {
        std::lock_guard<std::mutex> locker(callbackMutex_);
        cb = callback_;
    }
    if (cb != nullptr) {
        cb->OnConnsRefreshRateChanged(refreshRates);
    }
}
}
}
