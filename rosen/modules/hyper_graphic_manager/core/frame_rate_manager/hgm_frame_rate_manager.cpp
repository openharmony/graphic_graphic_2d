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

#include "hgm_frame_rate_manager.h"

#include "hgm_core.h"
#include "hgm_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr float MARGIN = 0.00001;
    constexpr float MIN_DRAWING_DIVISOR = 10.0f;
    constexpr float DIVISOR_TWO = 2.0f;
    constexpr int32_t DEFAULT_PREFERRED = 60;
    constexpr int32_t IDLE_TIMER_EXPIRED = 200; // ms
}

void HgmFrameRateManager::UniProcessData(const FrameRateRangeData& data)
{
    auto screenId = data.screenId;
    if (screenId == INVALID_SCREEN_ID) {
        return;
    }

    FrameRateRange finalRange;
    finalRange.Merge(data.rsRange);
    for (auto appRange : data.multiAppRange) {
        finalRange.Merge(appRange.second);
    }
    if (!finalRange.IsValid()) {
        if (data.forceUpdateFlag) {
            finalRange.max_ = DEFAULT_PREFERRED;
            finalRange.preferred_ = DEFAULT_PREFERRED;
        } else {
            HgmCore::Instance().InsertAndStartScreenTimer(screenId, IDLE_TIMER_EXPIRED, nullptr, expiredCallback_);
            return;
        }
    } else {
        HgmCore::Instance().ResetScreenTimer(screenId);
    }

    CalcRefreshRate(screenId, finalRange);
    rsFrameRate_ = GetDrawingFrameRate(currRefreshRate_, finalRange);
    RS_TRACE_NAME("HgmFrameRateManager:UniProcessData refreshRate:" +
        std::to_string(static_cast<int>(currRefreshRate_)) + ", rsFrameRate:" +
        std::to_string(static_cast<int>(rsFrameRate_)) + ", finalRange=(" +
        std::to_string(static_cast<int>(finalRange.min_)) + ", " +
        std::to_string(static_cast<int>(finalRange.max_)) + ", " +
        std::to_string(static_cast<int>(finalRange.preferred_)) + ")");

    for (auto appRange : data.multiAppRange) {
        multiAppFrameRate_[appRange.first] = GetDrawingFrameRate(currRefreshRate_, finalRange);
        RS_TRACE_NAME("multiAppFrameRate:pid=" +
            std::to_string(static_cast<int>(appRange.first)) + ", appRange=(" +
            std::to_string(static_cast<int>(appRange.second.min_)) + ", " +
            std::to_string(static_cast<int>(appRange.second.max_)) + ", " +
            std::to_string(static_cast<int>(appRange.second.preferred_)) + "), frameRate=" +
            std::to_string(static_cast<int>(multiAppFrameRate_[appRange.first])));
    }
    // [Temporary func]: Switch refresh rate immediately, func will be removed in the future.
    ExecuteSwitchRefreshRate(screenId);
}

void HgmFrameRateManager::CalcRefreshRate(const ScreenId id, const FrameRateRange& range)
{
    // Find current refreshRate by FrameRateRange. For example:
    // 1. FrameRateRange[min, max, preferred] is [24, 48, 48], supported refreshRates
    // of current screen are {30, 60, 90}, the result should be 30, not 60.
    // 2. FrameRateRange[min, max, preferred] is [150, 150, 150], supported refreshRates
    // of current screen are {30, 60, 90}, the result will be 90.
    auto supportRefreshRateVec = HgmCore::Instance().GetScreenSupportedRefreshRates(id);
    std::sort(supportRefreshRateVec.begin(), supportRefreshRateVec.end());
    auto iter = std::lower_bound(supportRefreshRateVec.begin(), supportRefreshRateVec.end(), range.preferred_);
    if (iter != supportRefreshRateVec.end()) {
        currRefreshRate_ = *iter;
        if (currRefreshRate_ > static_cast<uint32_t>(range.max_) &&
            (iter - supportRefreshRateVec.begin()) > 0) {
            iter--;
            if (*iter >= static_cast<uint32_t>(range.min_) &&
                *iter <= static_cast<uint32_t>(range.max_)) {
                currRefreshRate_ = *iter;
            }
        }
    } else {
        currRefreshRate_ = supportRefreshRateVec.back();
    }
}

uint32_t HgmFrameRateManager::GetDrawingFrameRate(const uint32_t refreshRate, const FrameRateRange& range)
{
    // We will find a drawing fps, which is divisible by refreshRate.
    // If the refreshRate is 60, the options of drawing fps are 60, 30, 15, 12, etc.
    // 1. The preferred fps is divisible by refreshRate.
    const float currRefreshRate = static_cast<float>(refreshRate);
    const float preferredFps = static_cast<float>(range.preferred_);
    if (std::fmodf(currRefreshRate, range.preferred_) < MARGIN) {
        return static_cast<uint32_t>(preferredFps);
    }

    // 2. FrameRateRange is not dynamic, we will find the closest drawing fps to preferredFps.
    // e.g. If the FrameRateRange of a surfaceNode is [50, 50, 50], the refreshRate is
    // 90, the drawing fps of the surfaceNode should be 45.
    if (!range.IsDynamic()) {
        return static_cast<uint32_t>(currRefreshRate / std::round(refreshRate / preferredFps));
    }

    // 3. FrameRateRange is dynamic. We will find a divisible result in the range if possible.
    // If several divisible options are in the range, the smoother, the better.
    // The KPI of "smooth" is the ratio of lack.
    // e.g. The preferred fps is 58, the refreshRate is 60. When the drawing fps is 60,
    // we lack the least(the ratio is 2/60).
    // The preferred fps is 34, the refreshRate is 60, the drawing fps will be 30(the ratio is 4/30).
    int divisor = 1;
    float drawingFps = currRefreshRate;
    float dividedFps = currRefreshRate;
    float currRatio = std::abs(dividedFps - preferredFps) / preferredFps;
    float ratio = currRatio;
    const float minDrawingFps = currRefreshRate / MIN_DRAWING_DIVISOR;
    while (dividedFps > minDrawingFps - MARGIN) {
        if (dividedFps < range.min_ || dividedFps <= static_cast<float>(range.preferred_) / DIVISOR_TWO) {
            break;
        }
        if (dividedFps > range.max_) {
            divisor++;
            float preDividedFps = dividedFps;
            dividedFps = currRefreshRate / static_cast<float>(divisor);
            // If we cannot find a divisible result, the closer to the preferred, the better.
            // e.g.FrameRateRange is [50, 80, 80], refreshrate is
            // 90, the drawing frame rate is 90.
            if (dividedFps < range.min_ && (preferredFps - dividedFps) > (preDividedFps - preferredFps)) {
                drawingFps = preDividedFps;
                break;
            }
            currRatio = std::abs(dividedFps - preferredFps) / preferredFps;
            if (currRatio < ratio) {
                ratio = currRatio;
                drawingFps = dividedFps;
            }
            continue;
        }
        float remainder = std::min(std::fmodf(preferredFps, dividedFps),
            std::fmodf(std::abs(dividedFps - preferredFps), dividedFps));
        currRatio = remainder / dividedFps;
        // When currRatio is almost zero, dividedFps is the perfect result
        if (currRatio < MARGIN) {
            drawingFps = dividedFps;
            break;
        }

        if (currRatio < ratio) {
            ratio = currRatio;
            drawingFps = dividedFps;
        }
        divisor++;
        dividedFps = currRefreshRate / static_cast<float>(divisor);
    }
    return static_cast<uint32_t>(std::round(drawingFps));
}

void HgmFrameRateManager::ExecuteSwitchRefreshRate(const ScreenId id)
{
    static bool refreshRateSwitch = system::GetBoolParameter("persist.hgm.refreshrate.enabled", true);
    if (!refreshRateSwitch) {
        HGM_LOGD("HgmFrameRateManager: refreshRateSwitch is off, currRefreshRate is %{public}d", currRefreshRate_);
        return;
    }
    uint32_t lcdRefreshRate = HgmCore::Instance().GetScreenCurrentRefreshRate(id);
    if (currRefreshRate_ != lcdRefreshRate) {
        HGM_LOGD("HgmFrameRateManager: current refreshRate is %{public}d",
            static_cast<int>(currRefreshRate_));
        int status = HgmCore::Instance().SetScreenRefreshRate(id, 0, currRefreshRate_);
        if (status < EXEC_SUCCESS) {
            currRefreshRate_ = lcdRefreshRate;
            HGM_LOGE("HgmFrameRateManager: failed to set refreshRate %{public}d, screenId %{public}d",
                static_cast<int>(currRefreshRate_), static_cast<int>(id));
        }
    }
}

void HgmFrameRateManager::Reset()
{
    currRefreshRate_ = -1;
    rsFrameRate_ = -1;
    multiAppFrameRate_.clear();
}
} // namespace Rosen
} // namespace OHOS