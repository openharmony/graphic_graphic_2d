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

#ifndef HGM_DIMMING_MANAGER_H
#define HGM_DIMMING_MANAGER_H

#include <chrono>
#include <functional>
#include <vector>

#include "hgm_command.h"

namespace OHOS::Rosen {
enum DimmingStatus : int32_t {
    DIMMING_DOWN = -1,
    NOT_DIMMING = 0,
    DIMMING_UP = 1,
};

class HgmDimmingManager final {
public:
    using DimmingEventCallback = std::function<void(int32_t)>;

    HgmDimmingManager() = default;
    ~HgmDimmingManager() = default;

    void SetLightFactorStatus(int32_t state) { lightFactorStatus_ = state; }
    void SetRefreshRateVec(std::vector<uint32_t> vec);
    void SetDimmingTimeoutConfig(const std::shared_ptr<PolicyConfigData>& configData);
    void RegisterDimmingEventCallback(DimmingEventCallback callback);
    uint32_t CalcDimmingRefreshRate(uint32_t voteFps);

private:
    int32_t lightFactorStatus_ = 0;
    std::vector<uint32_t> refreshRateVec_ = {};
    int32_t dimmingUpTimeoutMs_ = 0;
    int32_t dimmingDownTimeoutMs_ = 0;
    DimmingEventCallback dimmingEventCallback_ = nullptr;
    uint32_t currRefreshRate_ = 0;
    int32_t dimmingStatus_ = DimmingStatus::NOT_DIMMING;
    std::chrono::steady_clock::time_point dimmingEndTime_ = std::chrono::steady_clock::now();
};
} // OHOS::Rosen
#endif // HGM_DIMMING_MANAGER_H
