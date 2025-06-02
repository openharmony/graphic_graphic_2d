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

#ifndef HGM_SOFT_VSYNC_MANAGER_H
#define HGM_SOFT_VSYNC_MANAGER_H

#include <unordered_map>
#include <vector>

#include "common/rs_common_def.h"
#include "hgm_command.h"
#include "hgm_energy_consumption_policy.h"
#include "hgm_voter.h"
#include "hgm_vsync_generator_controller.h"
#include "pipeline/rs_render_frame_rate_linker.h"
#include "variable_frame_rate/rs_variable_frame_rate.h"
#include "vsync_distributor.h"

namespace OHOS {
namespace Rosen {
using WindowId = uint64_t;
using VsyncName = std::string;
using FrameRateLinkerMap = std::unordered_map<FrameRateLinkerId, std::shared_ptr<RSRenderFrameRateLinker>>;

class HgmSoftVSyncManager {
public:
    HgmSoftVSyncManager();
    ~HgmSoftVSyncManager() = default;

    void SetWindowExpectedRefreshRate(pid_t pid,
                                      const std::unordered_map<WindowId, EventInfo>& voters);
    void SetWindowExpectedRefreshRate(pid_t pid,
                                      const std::unordered_map<VsyncName, EventInfo>& voters);
    bool CollectFrameRateChange(FrameRateRange finalRange, std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker,
        const FrameRateLinkerMap& appFrameRateLinkers, const uint32_t currRefreshRate);
    std::vector<std::pair<FrameRateLinkerId, uint32_t>> GetSoftAppChangeData();
    void UniProcessDataForLtpo(const std::map<uint64_t, int>& vRatesMap,
                               const FrameRateLinkerMap& appFrameRateLinkers);
    void InitController(std::weak_ptr<HgmVSyncGeneratorController> controller,
                        sptr<VSyncDistributor> appDistributor);
    void ChangePerformanceFirst(bool isPerformanceFirst)
    {
        isPerformanceFirst_.store(isPerformanceFirst);
    }
    uint32_t GetControllerRate() const
    {
        return controllerRate_;
    }
    // called by hgmFrameRateManager
    void SetVsyncRateDiscountLTPO(const std::vector<uint64_t>& linkerIds, uint32_t rateDiscount);
    void SetQosVSyncRate(const uint32_t currRefreshRate, const FrameRateLinkerMap& appFrameRateLinkers);
    // Vrate
    void GetVRateMiniFPS(const std::shared_ptr<PolicyConfigData>& configData);
    void EraseGameRateDiscountMap(pid_t pid);
private:
    void Reset();
    void HandleLinkers();
    void DeliverSoftVote(FrameRateLinkerId linkerId, const VoteInfo& voteInfo, bool eventStatus);
    // vrate voting to hgm linkerId means that frameLinkerid, appFrameRate means that vrate
    bool CollectVRateChange(uint64_t linkerId, FrameRateRange& appFrameRate);
    bool CollectGameRateDiscountChange(uint64_t linkerId, FrameRateRange& expectedRange,
        const uint32_t currRefreshRate);
    static uint32_t GetDrawingFrameRate(const uint32_t refreshRate, const FrameRateRange& range);
    void CalcAppFrameRate(
        const std::pair<FrameRateLinkerId, std::shared_ptr<RSRenderFrameRateLinker>>& linker,
        FrameRateRange& expectedRange,
        bool& frameRateChanged,
        bool controllerRateChanged,
        const uint32_t currRefreshRate);

    // FORMAT: "frameRateLinkerId, <voterName, <<pid, <min, max>>, effective>>"
    std::unordered_map<uint64_t, std::shared_ptr<HgmVoter>> linkerVoteMap_;
    std::unordered_map<WindowId, FrameRateLinkerId> winLinkerMap_;
    std::unordered_map<VsyncName, std::vector<FrameRateLinkerId>> vsyncLinkerMap_;
    std::unordered_map<FrameRateLinkerId, uint32_t> appVoteData_;
    std::unordered_map<FrameRateLinkerId, uint32_t> appChangeData_;
    std::weak_ptr<HgmVSyncGeneratorController> controller_;
    // linkerid is key, vrate is value
    std::map<uint64_t, int> vRatesMap_;
    // Vrate
    //defalut value is 1, visiable lower than 10%.
    int32_t vrateControlMinifpsValue_ = 1;
    sptr<VSyncDistributor> appDistributor_ = nullptr;
    FrameRateLinkerMap appFrameRateLinkers_;
    // FORMAT: <linkerid, rateDiscount>
    std::unordered_map<uint64_t, uint32_t> gameRateDiscountMap_;

    uint32_t controllerRate_ = 0;

    std::atomic<bool> isPerformanceFirst_;
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_SOFT_VSYNC_MANAGER_H