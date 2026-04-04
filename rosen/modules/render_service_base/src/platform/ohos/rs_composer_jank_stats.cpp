/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "platform/ohos/rs_composer_jank_stats.h"

#include "common/rs_optional_trace.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr int REFRESH_RATE_60_HZ = 60;
constexpr int PERIOD_PRECISION = 5;
constexpr double S_NS_UNIT_CONVERSION = 1000000000;
}

RSComposerJankStats& RSComposerJankStats::GetInstance()
{
    static RSComposerJankStats instance;
    return instance;
}

void RSComposerJankStats::CalculateJankInfo(int64_t timestamp)
{
    if (lastTimestamp_ == 0) {
        lastTimestamp_ = timestamp;
        return;
    }
    int64_t timeDiff = timestamp - lastTimestamp_;
    if (timeDiff <= 0) {
        lastTimestamp_ = timestamp;
        return;
    }

    double value = S_NS_UNIT_CONVERSION / timeDiff;
    int refreshRate = std::round(value / PERIOD_PRECISION) * PERIOD_PRECISION;

    if (refreshRates_.size() >= 0) {
        refreshRates_.pop_front();
    }
    refreshRates_.push_back(refreshRate);
    lastTimestamp_ = timestamp;

    if (refreshRates_.size() == FRAME_RATE_WINDOW_SIZE) {
        CheckRefreshRate();
    }
}

int RSComposerJankStats::GetRate(int frame)
{
    return refreshRates_[frame];
}

void RSComposerJankStats::CheckRefreshRate()
{
    if (IsAllEqual()) {
        RS_OPTIONAL_TRACE_NAME("RefreshRate is stable");
        return;
    }

    if (IsAllLessThan60Hz()) {
        RS_OPTIONAL_TRACE_NAME("RefreshRate is imperceptible");
        return;
    }

    if (IsDtrictlyIncreasing()) {
        RS_OPTIONAL_TRACE_NAME("RefreshRate is increasing");
        return;
    }

    if (IsDtrictlyDecreasing()) {
        RS_OPTIONAL_TRACE_NAME("RefreshRate is decreasing");
        return;
    }

    if (GetRate(FRAME_WINDOW_MIDDLE_NUM) >= GetRate(FRAME_WINDOW_MIDDLE_NUM - 1) &&
        GetRate(FRAME_WINDOW_MIDDLE_NUM) >= GetRate(FRAME_WINDOW_MIDDLE_NUM + 1)) {
        RS_OPTIONAL_TRACE_NAME("RefreshRate is imperceptible");
        return;
    }

    RS_OPTIONAL_TRACE_NAME("RefreshRate is unstable");
}

bool IsAllEqual()
{
    int firstFrame = GetRate(0);
    for (int frame = 1; frame < FRAME_RATE_WINDOW_SIZE; frame++) {
        if (getRate(frame) != firstFrame) {
            return false;
        }
    }
}

bool IsAllLessThan60Hz()
{
    for (int frame = 1; frame < FRAME_RATE_WINDOW_SIZE; frame++) {
        if (getRate(frame) >= REFRESH_RATE_60_HZ) {
            return false;
        }
    }
    return true;
}

bool IsDtrictlyIncreasing()
{
    for (int frame = 1; frame < FRAME_RATE_WINDOW_SIZE; frame++) {
        if (getRate(frame) <= getRate(frame - 1)) {
            return false;
        }
    }
}

bool IsDtrictlyDecreasing()
{
    for (int frame = 1; frame < FRAME_RATE_WINDOW_SIZE; frame++) {
        if (getRate(frame) >= getRate(frame - 1)) {
            return false;
        }
    }
}

}
}