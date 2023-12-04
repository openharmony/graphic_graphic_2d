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

#ifndef HGM_FRAME_RATE_MANAGER_H
#define HGM_FRAME_RATE_MANAGER_H

#include <unordered_map>
#include <vector>

#include "animation/rs_frame_rate_range.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_render_frame_rate_linker.h"
#include "screen_manager/screen_types.h"
#include "hgm_one_shot_timer.h"
#include "hgm_command.h"
#include "hgm_screen.h"
#include "hgm_task_handle_thread.h"
#include "hgm_vsync_generator_controller.h"

namespace OHOS {
namespace Rosen {
using FrameRateLinkerMap = std::unordered_map<FrameRateLinkerId, std::shared_ptr<RSRenderFrameRateLinker>>;
class HgmFrameRateManager {
public:
    HgmFrameRateManager() = default;
    ~HgmFrameRateManager() = default;

    void UniProcessData(ScreenId screenId, uint64_t timestamp,
        std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker,
        const FrameRateLinkerMap& appFrameRateLinkers, bool idleTimerExpired);
    int32_t CalModifierPreferred(const HgmModifierProfile &hgmModifierProfile);
    std::shared_ptr<HgmOneShotTimer> GetScreenTimer(ScreenId screenId) const;
    void ResetScreenTimer(ScreenId screenId) const;
    void StartScreenTimer(ScreenId screenId, int32_t interval,
        std::function<void()> resetCallback, std::function<void()> expiredCallback);
    void StopScreenTimer(ScreenId screenId);
    void SetForceUpdateCallback(std::function<void(bool, bool)> forceUpdateCallback)
    {
        forceUpdateCallback_ = forceUpdateCallback;
    }

    void Init(sptr<VSyncController> rsController,
        sptr<VSyncController> appController, sptr<VSyncGenerator> vsyncGenerator);
    std::shared_ptr<uint32_t> GetPendingRefreshRate();
    void ResetPendingRefreshRate();
private:
    static void UpdateVSyncMode(sptr<VSyncController> rsController, sptr<VSyncController> appController);
    void Reset();
    bool CollectFrameRateChange(FrameRateRange finalRange, std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker,
        const FrameRateLinkerMap& appFrameRateLinkers);
    void HandleFrameRateChangeForLTPO(uint64_t timestamp);
    void CalcRefreshRate(const ScreenId id, const FrameRateRange& range);
    uint32_t GetDrawingFrameRate(const uint32_t refreshRate, const FrameRateRange& range);
    std::pair<float, float> applyDimension(
        SpeedTransType speedTransType, float xSpeed, float ySpeed, sptr<HgmScreen> hgmScreen);

    uint32_t currRefreshRate_ = 0;
    uint32_t controllerRate_ = 0;
    std::shared_ptr<uint32_t> pendingRefreshRate_;
    std::shared_ptr<HgmVSyncGeneratorController> controller_;
    std::vector<std::pair<FrameRateLinkerId, uint32_t>> appChangeData_;

    std::function<void(bool, bool)> forceUpdateCallback_;
    std::unordered_map<ScreenId, std::shared_ptr<HgmOneShotTimer>> screenTimerMap_;
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_FRAME_RATE_MANAGER_H