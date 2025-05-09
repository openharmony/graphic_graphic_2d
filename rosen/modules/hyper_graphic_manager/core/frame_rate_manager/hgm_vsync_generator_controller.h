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

#ifndef HGM_VSYNC_GENERATOR_CONTROLLER_H
#define HGM_VSYNC_GENERATOR_CONTROLLER_H

#include <unordered_map>
#include <vector>

#include "common/rs_common_def.h"
#include "vsync_controller.h"
#include "vsync_generator.h"

namespace OHOS {
namespace Rosen {
class HgmVSyncGeneratorController {
public:
    HgmVSyncGeneratorController(sptr<VSyncController> rsController,
        sptr<VSyncController> appController, sptr<VSyncGenerator> vsyncGenerator);
    ~HgmVSyncGeneratorController();

    static int32_t GetAppOffset(const uint32_t controllerRate);
    uint64_t CalcVSyncQuickTriggerTime(uint64_t lastVSyncTime, uint32_t lastRate);
    int64_t ChangeGeneratorRate(const uint32_t controllerRate,
                                const std::vector<std::pair<FrameRateLinkerId, uint32_t>>& appData,
                                uint64_t targetTime = 0,
                                bool isNeedUpdateAppOffset = false);
    uint32_t GetCurrentRate() const { return currentRate_; }
    int64_t GetCurrentOffset() const { return currentOffset_; }
    int32_t GetPulseNum() const { return pulseNum_; }
private:
    sptr<VSyncController> rsController_;
    sptr<VSyncController> appController_;
    sptr<VSyncGenerator> vsyncGenerator_;
    uint32_t currentRate_ = 0;
    int32_t currentOffset_ = 0;
    int32_t pulseNum_ = -1;
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_VSYNC_GENERATOR_CONTROLLER_H