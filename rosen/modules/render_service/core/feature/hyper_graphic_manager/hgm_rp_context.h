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

#ifndef HGM_RP_CONTEXT_H
#define HGM_RP_CONTEXT_H

#include <set>
#include <unordered_map>
#include "animation/rs_frame_rate_range.h"
#include "feature/hyper_graphic_manager/hgm_rp_energy.h"
#include "feature/hyper_graphic_manager/hgm_info_parcel.h"
#include "feature/hyper_graphic_manager/rs_render_frame_rate_linker_map.h"
#include "render_server/transaction/zidl/rs_irender_to_service_connection.h"
#include "rp_frame_rate_policy.h"

namespace OHOS {
namespace Rosen {
class RSContext;
class RSIRenderToServiceConnection;
class RSRenderNodeMap;
struct PipelineParam;

class HgmRPContext {
public:
    HgmRPContext(const sptr<RSIRenderToServiceConnection>& renderToServiceConnection);
    ~HgmRPContext() noexcept = default;

    int32_t InitHgmConfig(std::unordered_map<std::string, std::string>& sourceTuningConfig,
        std::unordered_map<std::string, std::string>& solidLayerConfig, std::vector<std::string>& appBufferList);
    void NotifyRpHgmFrameRate(uint64_t vsyncId,
        const std::shared_ptr<RSContext>& rsContext, std::map<NodeId, int> vRateMap, PipelineParam& pipelineParam);
    void AddScreenId(ScreenId screenId);
    void RemoveScreenId(ScreenId screenId);
    void SetServiceToProcessInfo(sptr<HgmServiceToProcessInfo> hgmServiceToProcessInfo,
        uint32_t *pendingScreenRefreshRate, uint64_t *pendingConstraintRelativeTime);

    bool AdaptiveStatus() const { return isAdaptive_; }

    bool GetLtpoEnabled() const { return ltpoEnabled_; }

    bool IsDelayMode() const { return isDelayMode_; }

    int32_t GetPipelineOffsetPulseNum() const { return pipelineOffsetPulseNum_; }

    std::shared_ptr<HgmRPEnergy> GetHgmRPEnergy() { return hgmRPEnergy_; }

    const std::function<int32_t(RSPropertyUnit, float, int32_t, int32_t)>& GetConvertFrameRateFunc() const
    {
        return convertFrameRateFunc_;
    }

    bool IsGameNodeOnTree() const { return isGameNodeOnTree_; }

    FrameRateRange& GetRSCurrRangeRef() { return rsCurrRange_; }
    
    void UpdateSurfaceData(const std::string& surfaceName, pid_t pid)
    {
        surfaceData_.emplace_back(std::tuple<std::string, pid_t>({surfaceName, pid}));
    }

private:
    void HandleGameNode(const RSRenderNodeMap& nodeMap);

    sptr<RSIRenderToServiceConnection> renderToServiceConnection_ = nullptr;
    std::unordered_set<ScreenId> screenIds_; // Accessed ONLY on main thread

    RPFrameRatePolicy rpFrameRatePolicy_;
    std::shared_ptr<HgmRPEnergy> hgmRPEnergy_;
    bool isAdaptive_ = false;
    std::string gameNodeName_ = "";

    bool ltpoEnabled_ = false;
    bool isDelayMode_ = false;
    int32_t pipelineOffsetPulseNum_ = 0;

    bool isGameNodeOnTree_ = false;
    FrameRateRange rsCurrRange_;
    std::vector<std::tuple<std::string, pid_t>> surfaceData_;
    std::function<int32_t(RSPropertyUnit, float, int32_t, int32_t)> convertFrameRateFunc_ = nullptr;
};
} // namespace OHOS
} // namespace Rosen

#endif // HGM_RP_CONTEXT_H