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

#include "rs_uni_render_frame_rate_manager.h"

#include "hgm_core.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
void RSUniRenderFrameRateManager::UpdateFrameRateRange(ScreenId id, FrameRateRange range)
{
    if (!range.IsValid()) {
        return;
    }
    screenIdToFrameRateRange_[id] = range;
}

void RSUniRenderFrameRateManager::FindAndSendRefreshRate()
{
    for (auto idToRange : screenIdToFrameRateRange_) {
        ScreenId id = idToRange.first;
        auto& instance = HgmCore::Instance();

        // Get supportted refreshRates
        auto& refreshRateMap = screenIdToSupportedRefreshRates_;
        if (refreshRateMap.find(id) == refreshRateMap.end()) {
            refreshRateMap[id] = instance.GetScreenSupportedRefreshRates(id);
            std::sort(refreshRateMap[id].begin(), refreshRateMap[id].end());
        }
        auto& refreshRates = refreshRateMap[id];

        // Find current refreshRate by FrameRateRange. For example, 
        // 1. FrameRateRange[min, max, preferred] = [24, 48, 48], supportted refreshRates
        // of current screen are {30, 60, 90}. The good refreshRate should be 30, not 60.
        // 2. FrameRateRange[150, 150, 150], supportted refreshRates are {30, 60, 90}, the
        // result is 90.
        int currRefreshRate = 0;
        FrameRateRange range = screenIdToFrameRateRange_[id];
        auto iter = std::lower_bound(refreshRates.begin(), refreshRates.end(), range.preferred_);
        int pos = iter - refreshRates.begin();
        if (pos < refreshRates.size()) {
            if (refreshRates[pos] > range.max_ && pos > 0 && refreshRates[pos - 1] >= range.min_ &&
                refreshRates[pos - 1] <= range.max_) {
                currRefreshRate = refreshRates[pos - 1];
            } else {
                currRefreshRate = refreshRates[pos];
            }
        } else {
            currRefreshRate = refreshRates.back(); // preferred fps >= biggest supportted refreshRate
        }

        // Send RefreshRate
        if (currRefreshRate != instance.GetScreenCurrentRefreshRate(id)) {
            ROSEN_LOGD("RSUniRenderFrameRateManager::FindAndSendRefreshRate current refreshRate is %d",
                currRefreshRate);
            int status = instance.SetScreenRefreshRate(id, 0, currRefreshRate);
            if (status != EXEC_SUCCESS) {
                ROSEN_LOGE("RSUniRenderFrameRateManager::FindAndSendRefreshRate failed to set refreshRate %d, \
                    screenId " PRIu64 "", currRefreshRate, id);
            }
        }
    }
}

void RSUniRenderFrameRateManager::ResetFrameRateRangeMap()
{
    screenIdToFrameRateRange_.clear();
}
} // namespace Rosen
} // namespace OHOS