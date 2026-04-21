/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_COMPOSER_SERVICE_EXTERNER_DEPEND_HYPER_GRAPHIC_MANAGER_HGM_HARDWARE_UTILS_H
#define RENDER_SERVICE_COMPOSER_SERVICE_EXTERNER_DEPEND_HYPER_GRAPHIC_MANAGER_HGM_HARDWARE_UTILS_H

#include "hdi_backend.h"
#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "hyper_graphic_manager/rs_vblank_idle_corrector.h"

namespace OHOS {
namespace Rosen {
struct PipelineParam;

struct SetRateRetryParam {
    int32_t retryCount = 0; // Number of retries attempted for frame switching
    bool needRetrySetRate = false;  // whether a retry is needed for next frame switching
    bool isRetryOverLimit = false;  // Determines if the maximum retry limit has been exceeded
};

struct ASRecordRateParam {
    static constexpr int32_t TIMESTAMP_BUFFER_SIZE = 6;
    static constexpr int32_t BELOW_THRESHOLD_LIMIT = 50;
    static constexpr int64_t AS_INTERVAL_THRESHOLD = 25000; // 25ms, 40Hz frame rate (in microseconds)
    int32_t highFpsFrameCount = 0;
    std::vector<int64_t> frameTimestamps;
    int32_t newIndex = 0;

    void ClearTimestamp()
    {
        if (!frameTimestamps.empty()) {
            std::vector<int64_t>().swap(frameTimestamps);
            highFpsFrameCount = 0;
            newIndex = 0;
        }
    }

    bool IsBufferNotFull()
    {
        return frameTimestamps.size() < static_cast<size_t>(TIMESTAMP_BUFFER_SIZE);
    }

    bool RecordTimestamp(int64_t timestamp)
    {
        if (frameTimestamps.empty()) {
            highFpsFrameCount = 0;
            newIndex = 0;
        }

        if (IsBufferNotFull()) {
            frameTimestamps.push_back(timestamp);
        } else {
            frameTimestamps[newIndex] = timestamp;
        }
        int32_t curIndex = newIndex;
        newIndex = (newIndex + 1) % TIMESTAMP_BUFFER_SIZE;

        if (IsBufferNotFull()) {
            return false;
        }

        int64_t avgInterval = (frameTimestamps[curIndex] - frameTimestamps[newIndex]) /
                              static_cast<int64_t>(TIMESTAMP_BUFFER_SIZE - 1);

        if (avgInterval > AS_INTERVAL_THRESHOLD) {
            highFpsFrameCount = 0;
        } else if (highFpsFrameCount < BELOW_THRESHOLD_LIMIT) {
            highFpsFrameCount++;
        }
        return highFpsFrameCount >= BELOW_THRESHOLD_LIMIT;
    }
};

class HgmHardwareUtils {
public:
    HgmHardwareUtils() = default;
    ~HgmHardwareUtils() noexcept = default;

    void ResetRetryCount(ScreenPowerStatus status);
    void SetScreenVBlankIdle() { vblankIdleCorrector_.SetScreenVBlankIdle(); }
    void SwitchRefreshRate(const std::shared_ptr<HdiOutput>& hdiOutput, int64_t timestamp,
        const PipelineParam& pipelineParam);

    void RefreshRateCounts(std::string& dumpString);
    void ClearRefreshRateCounts(std::string& dumpString);

private:
    struct RefreshRateParam {
        uint32_t rate = 0;
        uint64_t frameTimestamp = 0;
        uint64_t constraintRelativeTime = 0;
        void setParam(uint32_t pendingRefreshRate, uint64_t timestamp, uint64_t pendingConstraintRelativeTime)
        {
            rate = pendingRefreshRate;
            frameTimestamp = timestamp;
            constraintRelativeTime = pendingConstraintRelativeTime;
        }
    };

    void ExecuteSwitchRefreshRate(ScreenId screenId);
    void UpdateRetrySetRateStatus(ScreenId id, int32_t modeId, uint32_t setRateRet);
    void PerformSetActiveMode(const std::shared_ptr<HdiOutput>& output);

    void AddRefreshRateCount(ScreenId screenId);

    void ReportRetryOverLimit(uint64_t vsyncId, uint32_t rate);
    void RecordTimestampForAS(int64_t timestamp);

    HgmCore& hgmCore_ = HgmCore::Instance();
    // key: ScreenId, value: <needRetrySetRate, retryCount>
    std::unordered_map<ScreenId, std::pair<bool, int32_t>> setRateRetryMap_;
    HgmRefreshRates hgmRefreshRates_ = HgmRefreshRates::SET_RATE_NULL;
    RefreshRateParam refreshRateParam_;
    SetRateRetryParam setRateRetryParam_;
    ASRecordRateParam asRecordRateParam_;
    RSVBlankIdleCorrector vblankIdleCorrector_;

    std::map<uint32_t, uint64_t> refreshRateCounts_;
};
} // namespace OHOS
} // namespace Rosen
#endif // RENDER_SERVICE_COMPOSER_SERVICE_EXTERNER_DEPEND_HYPER_GRAPHIC_MANAGER_HGM_HARDWARE_UTILS_H