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
namespace {
    constexpr float MARGIN = 0.00001;
    constexpr int MIN_DRAWING_FPS = 10;
    constexpr int REFRESH_RATE_OUTLIER = -1;
}

namespace OHOS {
namespace Rosen {
void HgmFrameRateManager::UpdateFrameRateRange(ScreenId id, FrameRateRange range)
{
    if (!range.IsValid()) {
        return;
    }
    screenIdToFrameRateRange_[id] = range;
}

void HgmFrameRateManager::FindAndSendRefreshRate()
{
    for (auto idToRange : screenIdToFrameRateRange_) {
        ScreenId id = idToRange.first;
        auto& instance = HgmCore::Instance();

        // Get supported refreshRates
        auto& refreshRateMap = screenIdToSupportedRefreshRates_;
        if (refreshRateMap.find(id) == refreshRateMap.end()) {
            refreshRateMap[id] = instance.GetScreenSupportedRefreshRates(id);
            std::sort(refreshRateMap[id].begin(), refreshRateMap[id].end());
        }
        auto& refreshRates = refreshRateMap[id];

        // Find current refreshRate by FrameRateRange. For example:
        // 1. FrameRateRange[min, max, preferred] is [24, 48, 48], supported refreshRates
        // of current screen are {30, 60, 90}, the result should be 30, not 60.
        // 2. FrameRateRange[min, max, preferred] is [150, 150, 150], supported refreshRates
        // of current screen are {30, 60, 90}, the result will be 90.
        uint32_t currRefreshRate = 0;
        FrameRateRange range = screenIdToFrameRateRange_[id];
        auto iter = std::lower_bound(refreshRates.begin(), refreshRates.end(), range.preferred_);
        uint32_t pos = static_cast<uint32_t>(iter - refreshRates.begin());
        if (pos < refreshRates.size()) {
            if (refreshRates[pos] > static_cast<uint32_t>(range.max_) && pos > 0 &&
                refreshRates[pos - 1] >= static_cast<uint32_t>(range.min_) &&
                refreshRates[pos - 1] <= static_cast<uint32_t>(range.max_)) {
                currRefreshRate = refreshRates[pos - 1];
            } else {
                currRefreshRate = refreshRates[pos];
            }
        } else {
            currRefreshRate = refreshRates.back(); // preferred fps >= biggest supported refreshRate
        }
        screenIdToLCDRefreshRates_[id] = currRefreshRate;

        // Send RefreshRate
        static bool refreshRateSwitch = system::GetBoolParameter("persist.hgm.refreshrate.enabled", false);
        if (!refreshRateSwitch) {
            HGM_LOGD("HgmFrameRateManager: refreshRateSwitch is off, currRefreshRate is %{public}d", currRefreshRate);
            return;
        }
        uint32_t lcdRefreshRate = instance.GetScreenCurrentRefreshRate(id);
        if (currRefreshRate != lcdRefreshRate) {
            HGM_LOGD("HgmFrameRateManager: current refreshRate is %{public}d",
                static_cast<int>(currRefreshRate));
            int status = instance.SetScreenRefreshRate(id, 0, currRefreshRate);
            if (status != EXEC_SUCCESS) {
                screenIdToLCDRefreshRates_[id] = lcdRefreshRate;
                HGM_LOGE("HgmFrameRateManager: failed to set refreshRate %{public}d, screenId %{public}d",
                    static_cast<int>(currRefreshRate), static_cast<int>(id));
            }
        }
    }
}

void HgmFrameRateManager::ResetFrameRateRangeMap()
{
    screenIdToFrameRateRange_.clear();
    screenIdToLCDRefreshRates_.clear();
    drawingFrameRateMap_.clear();
}

void HgmFrameRateManager::CalcSurfaceDrawingFrameRate(NodeId surfaceNodeId,
    ScreenId screenId, FrameRateRange range)
{
    if (!range.IsValid()) {
        return;
    }
    int refreshRate = GetRefreshRateByScreenId(screenId);
    if (GetRefreshRateByScreenId(screenId) == REFRESH_RATE_OUTLIER) {
        return;
    }

    float drawingFps = GetDrawingFps(static_cast<float>(refreshRate), range);
    drawingFrameRateMap_[surfaceNodeId] = drawingFps;
    HGM_LOGD("HgmFrameRateManager:: surfaceNodeId - %{public}d, Drawing FrameRate %{public}.1f",
        static_cast<int>(surfaceNodeId), drawingFps);
}

int HgmFrameRateManager::GetRefreshRateByScreenId(ScreenId screenId)
{
    auto& refreshRatesMap = screenIdToLCDRefreshRates_;
    if (refreshRatesMap.find(screenId) == refreshRatesMap.end()) {
        HGM_LOGE("HgmFrameRateManager:: Failed to find ScreenId - %{public}d",
            static_cast<int>(screenId));
        return REFRESH_RATE_OUTLIER;
    }
    return refreshRatesMap[screenId];
}

float HgmFrameRateManager::GetDrawingFps(float refreshRate, FrameRateRange range)
{
    // We will find a drawing fps, which is divisible by refreshRate.
    // If the refreshRate is 60, the options of drawing fps are 60, 30, 15, 12, etc.
    // 1. The preferred fps is divisible by refreshRate.
    const float preferredFps = static_cast<float>(range.preferred_);
    if (std::fmodf(refreshRate, range.preferred_) < MARGIN) {
        return preferredFps;
    }

    // 2. FrameRateRange is not dynamic, we will find the closest drawing fps to preferredFps.
    // e.g. If the FrameRateRange of a surfaceNode is [50, 50, 50], the refreshRate is
    // 90, the drawing fps of the surfaceNode should be 45.
    if (!range.IsDynamic()) {
        return refreshRate / std::round(refreshRate / preferredFps);
    }

    // 3. FrameRateRange is dynamic. We will find a divisible result in the range if possible.
    // If several divisible options are in the range, the smoother, the better.
    // The KPI of "smooth" is the ratio of lack.
    // e.g. The preferred fps is 58, the refreshRate is 60. When the drawing fps is 60,
    // we lack the least(the ratio is 2/60).
    // The preferred fps is 34, the refreshRate is 60, the drawing fps will be 30(the ratio is 4/30).
    int divisor = 1;
    float drawingFps = refreshRate;
    float dividedFps = refreshRate;
    float currRatio = std::abs(dividedFps - preferredFps) / preferredFps;
    float ratio = currRatio;
    while (dividedFps > MIN_DRAWING_FPS - MARGIN) {
        if (dividedFps < range.min_) {
            break;
        }
        if (dividedFps > range.max_) {
            divisor++;
            float preDividedFps = dividedFps;
            dividedFps = refreshRate / static_cast<float>(divisor);
            // If we cannot find a divisible result, the closer to the preferred, the better.
            // e.g.FrameRateRange is [50, 80, 80], refreshrate is
            // 90, the drawing frame rate is 90.
            if (dividedFps < range.min_ && (preferredFps - dividedFps) >
                (preDividedFps - preferredFps)) {
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
            ratio = currRatio;
            drawingFps = dividedFps;
            break;
        }

        if (currRatio < ratio) {
            ratio = currRatio;
            drawingFps = dividedFps;
        }
        divisor++;
        dividedFps = refreshRate / static_cast<float>(divisor);
    }
    return drawingFps;
}
} // namespace Rosen
} // namespace OHOS