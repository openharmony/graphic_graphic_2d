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

#include "pipeline/rs_render_display_sync.h"

#include <algorithm>
#include <vector>

#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t MAX_DIVISOR_NUM = 15;
constexpr int32_t MIN_DIVISOR_FRAME_RATE = 10;
constexpr int32_t FRAME_RATE_THRESHOLD = 5;
const std::vector<int32_t> SOURCE_FRAME_RATES = {30, 60, 72, 90, 120}; // sorted
}

RSRenderDisplaySync::RSRenderDisplaySync(NodeId id) : id_(id) {}

RSRenderDisplaySync::RSRenderDisplaySync(std::weak_ptr<RSRenderAnimation> renderAnimation)
    : renderAnimation_(renderAnimation) {}

uint64_t RSRenderDisplaySync::GetId() const
{
    auto renderAnimation = renderAnimation_.lock();
    if (renderAnimation) {
        return renderAnimation->GetAnimationId();
    }

    return id_;
}

void RSRenderDisplaySync::SetExpectedFrameRateRange(const FrameRateRange& range)
{
    auto renderAnimation = renderAnimation_.lock();
    if (renderAnimation) {
        renderAnimation->SetFrameRateRange(range);
    }

    if (expectedFrameRateRange_ != range) {
        expectedFrameRateRange_ = range;
        isSkipCountUpdate_ = true;
    }
}

const FrameRateRange& RSRenderDisplaySync::GetExpectedFrameRange() const
{
    return expectedFrameRateRange_;
}

void RSRenderDisplaySync::SetAnimateResult(std::tuple<bool, bool, bool>& result)
{
    animateResult_ = result;
}

std::tuple<bool, bool, bool> RSRenderDisplaySync::GetAnimateResult() const
{
    return animateResult_;
}

bool RSRenderDisplaySync::OnFrameSkip(uint64_t timestamp, int64_t period, bool isDisplaySyncEnabled)
{
    if (!isDisplaySyncEnabled) {
        referenceCount_ = 0;
        return false;
    }
    if (period <= 0 || timestamp_ == timestamp || expectedFrameRateRange_.preferred_ == 0) {
        return false;
    }
    bool isFrameSkip = false;
    referenceCount_++;
    timestamp_ = timestamp;

    if (currentPeriod_ != period) {
        currentPeriod_ = period;
        int32_t frameRate = round(1.0 / (static_cast<double>(currentPeriod_) / NS_TO_S));
        frameRate = GetNearestFrameRate(frameRate, SOURCE_FRAME_RATES);
        if (currentFrameRate_ != frameRate) {
            currentFrameRate_ = frameRate;
            referenceCount_ = 0;
            isSkipCountUpdate_ = true;
        }
    }

    if (isSkipCountUpdate_) {
        skipRateCount_ = CalcSkipRateCount(currentFrameRate_);
        isSkipCountUpdate_ = false;
    }
    if (skipRateCount_ == 0) {
        skipRateCount_ = 1; // default value
    }

    if (referenceCount_ % skipRateCount_ != 0) {
        isFrameSkip = true;
    }
    RS_OPTIONAL_TRACE_NAME_FMT(
        "RSRenderDisplaySync::OnFrameSkip preferred: [%d] currentPeroid: [%d] isFrameSkip:[%d]",
        expectedFrameRateRange_.preferred_, currentPeriod_, isFrameSkip);
    return isFrameSkip;
}

int32_t RSRenderDisplaySync::CalcSkipRateCount(int32_t frameRate)
{
    int32_t count = 0;
    int32_t preferred = expectedFrameRateRange_.preferred_;
    if (preferred == 0 || frameRate == 0) {
        return count;
    }
    std::vector<int32_t> divisorRates;
    for (int i = 1; i <= MAX_DIVISOR_NUM; i++) {
        if (frameRate % i == 0 && frameRate / i >= MIN_DIVISOR_FRAME_RATE) {
            divisorRates.emplace_back(frameRate / i);
        }
    }
    std::sort(divisorRates.begin(), divisorRates.end());
    auto divisorFrameRate = GetNearestFrameRate(preferred, divisorRates);
    if (divisorFrameRate != 0) {
        count = frameRate / divisorFrameRate;
    }
    return count;
}

int32_t RSRenderDisplaySync::GetNearestFrameRate(int32_t num, const std::vector<int32_t>& rates)
{
    int32_t rate = 0;
    if (!rates.empty()) {
        auto iter = std::lower_bound(rates.begin(), rates.end(), num);
        if (iter == rates.end()) {
            if (num - rates.back() <= FRAME_RATE_THRESHOLD) {
                rate = rates.back();
            }
        } else if (iter == rates.begin()) {
            if (rates.front() - num <= FRAME_RATE_THRESHOLD) {
                rate = rates.front();
            }
        } else if (*iter == num) {
            rate = *iter;
        } else {
            int32_t index = iter - rates.begin();
            // compare the difference between the number of rates[index - 1] and rates[index]
            int32_t lastInterval = num - rates[index - 1];
            int32_t nextInterval = rates[index] - num;
            if (lastInterval < nextInterval && lastInterval <= FRAME_RATE_THRESHOLD) {
                rate = rates[index - 1];
            } else {
                rate = rates[index];
            }
        }
    }
    return rate;
}
} // namespace Rosen
} // namespace OHOS
