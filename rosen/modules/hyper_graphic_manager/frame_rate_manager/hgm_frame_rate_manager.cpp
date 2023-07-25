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
    constexpr int MIN_DRAWING_FPS = 1;
    constexpr int DUPLATION = 2;
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
        uint32_t pos = iter - refreshRates.begin();
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
    auto& refreshRatesMap = screenIdToLCDRefreshRates_;
    if (refreshRatesMap.find(screenId) == refreshRatesMap.end()) {
        HGM_LOGE("HgmFrameRateManager:: Failed to find ScreenId - %{public}d",
            static_cast<int>(screenId));
        return;
    }

    int refreshRate = static_cast<int>(refreshRatesMap[screenId]);
    float drawingFps = static_cast<float>(refreshRate);
    if (range.preferred_ == refreshRate || refreshRate % range.preferred_ == 0) {
        drawingFps = static_cast<float>(range.preferred_);
    } else if (!range.IsDynamic()) {
        // if the FrameRateRange of a surfaceNode is [50, 50, 50], the refreshRate is
        // 90, the drawing fps of the surfaceNode should be 45.
        int divisor = refreshRate / range.preferred_;
        drawingFps = static_cast<float>(refreshRate) / static_cast<float>(divisor);
    } else {
        // if the FrameRateRange of a surfaceNode is [24, 48, 48], the refreshRate is
        // 60, the drawing fps of the surfaceNode should be 30.
        float ratio = 1.0f;
        int divisor = 1;
        float dividedFps = static_cast<float>(refreshRate);
        while (dividedFps > MIN_DRAWING_FPS - MARGIN) {
            if (dividedFps < range.min_) {
                break;
            }
            if (dividedFps > range.max_) {
                divisor++;
                float preDividedFps = dividedFps;
                dividedFps = static_cast<float>(refreshRate) / static_cast<float>(divisor);
                // FrameRateRange is [50, 80, 80], refreshrate is
                // 90, the drawing frame rate is 90.
                // FrameRateRange is [40, 80, 80], refreshrate is
                // 90, the drawing frame rate is 45.
                if (dividedFps < range.min_ && (range.preferred_ - dividedFps) >
                    (preDividedFps - range.preferred_)) {
                    drawingFps = preDividedFps;
                    break;
                }
                drawingFps = dividedFps;
                continue;
            }
            // We want to measure the satisfaction of current drawing fps.
            // The KPI of satisfaction is the ratio of lack.
            // e.g. Preferred fps is 58, refreshRate is 60. When the
            // drawing fps is 60, we lack the least(the ratio is 2/60).
            // Preferred fps is 34, refreshRate is 60. When the
            // drawing fps is 34, we lack the least(the ratio is 4/30).
            int remainder = std::min(range.preferred_ % static_cast<int>(dividedFps),
                std::abs(static_cast<int>(DUPLATION * dividedFps) - range.preferred_) %
                static_cast<int>(dividedFps));
            float currRatio = static_cast<float>(remainder) / dividedFps;
            // dividedFps is the perfect result, currRatio is almost zero.
            if (currRatio < MARGIN) {
                ratio = currRatio;
                drawingFps = dividedFps;
                break;
            }
            // dividedFps is better than previous result
            if (currRatio < ratio) {
                ratio = currRatio;
                drawingFps = dividedFps;
            }
            divisor++;
            dividedFps = static_cast<float>(refreshRate) / static_cast<float>(divisor);
        }
    }
    drawingFrameRateMap_[surfaceNodeId] = drawingFps;
    HGM_LOGD("HgmFrameRateManager:: surfaceNodeId - %{public}d, Drawing FrameRate %{public}.1f",
        static_cast<int>(surfaceNodeId), drawingFps);
}
} // namespace Rosen
} // namespace OHOS