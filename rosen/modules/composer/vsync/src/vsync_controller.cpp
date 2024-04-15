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
    offsetMutex_(), phaseOffset_(offset), enabled_(false)
{
}

VSyncController::~VSyncController()
{
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
            ret = generator->AddListener(phaseOffset, this);
        } else {
            ret = VSYNC_ERROR_API_FAILED;
        }
    } else {
        ret = generator->RemoveListener(this);
        isGeneratorEnable = enable;
    }

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
    }
    return generator->ChangePhaseOffset(this, offset);
}

void VSyncController::OnVSyncEvent(int64_t now, int64_t period, uint32_t refreshRate, VSyncMode vsyncMode)
{
    Callback *cb = nullptr;
    {
        std::lock_guard<std::mutex> locker(callbackMutex_);
        cb = callback_;
    }
    if (cb != nullptr) {
        cb->OnVSyncEvent(now, period, refreshRate, vsyncMode);
    }
}

void VSyncController::OnPhaseOffsetChanged(int64_t phaseOffset)
{
    std::lock_guard<std::mutex> locker(offsetMutex_);
    phaseOffset_ = phaseOffset;
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
