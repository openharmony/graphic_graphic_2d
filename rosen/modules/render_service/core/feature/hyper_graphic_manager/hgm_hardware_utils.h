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
#ifndef HGM_HARDWARE_UTILS_H
#define HGM_HARDWARE_UTILS_H

#include "feature/hyper_graphic_manager/rs_vblank_idle_corrector.h"
#include "hdi_backend.h"
#include "hgm_frame_rate_manager.h"
#include "hgm_core.h"

namespace OHOS {
namespace Rosen {
struct RefreshRateParam {
    uint32_t rate = 0;
    uint64_t frameTimestamp = 0;
    int64_t actualTimestamp = 0;
    uint64_t vsyncId = 0;
    uint64_t constraintRelativeTime = 0;
    bool isForceRefresh = false;
    uint64_t fastComposeTimeStampDiff = 0;
};

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
    int32_t curIndex = 0;

    void ClearTimestamp()
    {
        if (!frameTimestamps.empty()) {
            std::vector<int64_t>().swap(frameTimestamps);
            highFpsFrameCount = 0;
            curIndex = 0;
        }
    }

    bool RecordTimestamp(int64_t timestamp)
    {
        if (frameTimestamps.empty()) {
            highFpsFrameCount = 0;
            curIndex = 0;
        }

        if (curIndex < frameTimestamps.size()) {
            frameTimestamps[curIndex] = timestamp;
        } else {
            frameTimestamps.push_back(timestamp);
        }

        if (frameTimestamps.size() < TIMESTAMP_BUFFER_SIZE) {
            curIndex++;
            return false;
        }

        int32_t nextIndex = (curIndex + 1) % TIMESTAMP_BUFFER_SIZE;
        int32_t avgInterval = (frameTimestamps[curIndex] -
                               frameTimestamps[nextIndex]) / (TIMESTAMP_BUFFER_SIZE - 1);
        curIndex = nextIndex;

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

    void ExecuteSwitchRefreshRate(ScreenId screenId);
    void PerformSetActiveMode(const std::shared_ptr<HdiOutput>& output);
    void UpdateRefreshRateParam();
    void ResetRetryCount(ScreenPowerStatus status);
    void SetScreenVBlankIdle() { vblankIdleCorrector_.SetScreenVBlankIdle(); }

    const RefreshRateParam& GetRefreshRateParam() const
    {
        return refreshRateParam_;
    }

    void SwitchRefreshRate(const std::shared_ptr<HdiOutput>& hdiOutput, int64_t timestamp);

    void TransactRefreshRateParam(uint32_t& currentRate, RefreshRateParam& param);

private:
    void UpdateRetrySetRateStatus(ScreenId id, int32_t modeId, uint32_t setRateRet);
    void ReportRetryOverLimit(uint64_t vsyncId, uint32_t rate);
    void RecordTimestampForAS(int64_t timestamp);

    HgmCore& hgmCore_ = HgmCore::Instance();
    HgmRefreshRates hgmRefreshRates_ = HgmRefreshRates::SET_RATE_NULL;
    RefreshRateParam refreshRateParam_;
    SetRateRetryParam setRateRetryParam_;
    ASRecordRateParam asRecordRateParam_;
    RSVBlankIdleCorrector vblankIdleCorrector_;
};
} // namespace OHOS
} // namespace Rosen
#endif // HGM_HARDWARE_UTILS_H