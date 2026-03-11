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

#ifndef HGM_RENDER_CONTEXT_H
#define HGM_RENDER_CONTEXT_H

#include <set>
#include <unordered_map>

#include "animation/rs_frame_rate_range.h"
#include "feature/hyper_graphic_manager/hgm_info_parcel.h"
#include "feature/hyper_graphic_manager/hgm_rp_energy.h"
#include "feature/hyper_graphic_manager/rs_render_frame_rate_linker_map.h"
#include "render_server/transaction/zidl/rs_irender_to_service_connection.h"
#include "rp_frame_rate_policy.h"

namespace OHOS {
namespace Rosen {
class RSContext;
class RSIRenderToServiceConnection;
class RSRenderNodeMap;
struct PipelineParam;

class HgmRenderContext {
public:
    using ConvertFrameRateFunc = std::function<int32_t(RSPropertyUnit, float, int32_t, int32_t)>;

    HgmRenderContext(const sptr<RSIRenderToServiceConnection>& renderToServiceConnection);
    ~HgmRenderContext() noexcept = default;

    int32_t InitHgmConfig(std::unordered_map<std::string, std::string>& sourceTuningConfig,
        std::unordered_map<std::string, std::string>& solidLayerConfig, std::vector<std::string>& appBufferList);

    const ConvertFrameRateFunc& GetConvertFrameRateFunc() const { return convertFrameRateFunc_; }

    void NotifyRpHgmFrameRate(uint64_t vsyncId, const std::shared_ptr<RSContext>& rsContext,
        const std::unordered_map<NodeId, int>& vRateMap, bool isNeedRefreshVRate, PipelineParam& pipelineParam);
    void SetServiceToProcessInfo(sptr<HgmServiceToProcessInfo> hgmServiceToProcessInfo,
        uint32_t& pendingScreenRefreshRate, uint64_t& pendingConstraintRelativeTime);

    std::shared_ptr<HgmRPEnergy> GetHgmRPEnergy() { return hgmRPEnergy_; }

    bool AdaptiveStatus() const { return isAdaptive_.load(); }
    bool IsGameNodeOnTree() const { return isGameNodeOnTree_.load(); }
    bool IsAdaptiveVsyncReady() const { return isAdaptiveVsyncReaddy_.load(); }

    bool GetLtpoEnabled() const { return ltpoEnabled_; }
    bool IsDelayMode() const { return isDelayMode_; }
    int32_t GetPipelineOffsetPulseNum() const { return pipelineOffsetPulseNum_; }

    FrameRateRange& GetRSCurrRangeRef() { return rsCurrRange_; }
    void UpdateSurfaceData(const std::shared_ptr<RSSurfaceHandler>& surfaceHandler,
        const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode);

private:
    void HandleGameNode(const RSRenderNodeMap& nodeMap);

    const sptr<RSIRenderToServiceConnection> renderToServiceConnection_;
    std::unordered_set<ScreenId> screenIds_; // Accessed ONLY on main thread

    const std::shared_ptr<RPFrameRatePolicy> rpFrameRatePolicy_;
    ConvertFrameRateFunc convertFrameRateFunc_ = nullptr;

    const std::shared_ptr<HgmRPEnergy> hgmRPEnergy_;

    std::atomic<bool> isAdaptive_ = false;
    std::string gameNodeName_ = "";
    std::atomic<bool> isGameNodeOnTree_ = false;
    std::atomic<bool> isAdaptiveVsyncReaddy_ = false;

    bool ltpoEnabled_ = false;
    bool isDelayMode_ = false;
    int32_t pipelineOffsetPulseNum_ = 0;

    FrameRateRange rsCurrRange_;
    std::vector<std::tuple<std::string, pid_t>> surfaceData_;
};
} // namespace OHOS
} // namespace Rosen
#endif // HGM_RENDER_CONTEXT_H