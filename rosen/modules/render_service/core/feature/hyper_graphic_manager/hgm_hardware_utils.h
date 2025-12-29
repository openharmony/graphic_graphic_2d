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
#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"

namespace OHOS {
namespace Rosen {
class HgmHardwareUtils {
public:
    HgmHardwareUtils() = default;
    ~HgmHardwareUtils() noexcept = default;

    void SetScreenVBlankIdle() { vblankIdleCorrector_.SetScreenVBlankIdle(); }
    void TransactRefreshRateParam(uint32_t& currentRate,
        uint32_t pendingScreenRefreshRate, uint64_t frameTimestamp, uint64_t pendingConstraintRelativeTime);
    void SwitchRefreshRate(const std::shared_ptr<HdiOutput>& hdiOutput);

    void RefreshRateCounts(std::string& dumpString);
    void ClearRefreshRateCounts(std::string& dumpString);

private:
    struct RefreshRateParam {
        uint32_t rate = 0;
        uint64_t frameTimestamp = 0;
        uint64_t constraintRelativeTime = 0;
    };

    void ExecuteSwitchRefreshRate(ScreenId screenId);
    void UpdateRetrySetRateStatus(ScreenId id, int32_t modeId, uint32_t setRateRet);
    void PerformSetActiveMode(const std::shared_ptr<HdiOutput>& output, RSScreenManager* screenManager);

    void AddRefreshRateCount(ScreenId screenId);

    HgmCore& hgmCore_ = HgmCore::Instance();
    // key: ScreenId, value: <needRetrySetRate, retryCount>
    std::unordered_map<ScreenId, std::pair<bool, int32_t>> setRateRetryMap_;
    HgmRefreshRates hgmRefreshRates_ = HgmRefreshRates::SET_RATE_NULL;
    RefreshRateParam refreshRateParam_;
    RSVBlankIdleCorrector vblankIdleCorrector_;

    std::map<uint32_t, uint64_t> refreshRateCounts_;
};
} // namespace OHOS
} // namespace Rosen
#endif // HGM_HARDWARE_UTILS_H