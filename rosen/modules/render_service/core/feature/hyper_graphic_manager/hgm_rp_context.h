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
#include "rp_frame_rate_policy.h"
#include "feature/hyper_graphic_manager/hgm_rp_energy.h"
#include "feature/hyper_graphic_manager/hgm_info_parcel.h"
#include "feature/hyper_graphic_manager/rs_render_frame_rate_linker_map.h"

namespace OHOS {
namespace Rosen {
class HgmRPContext {
public:
    HgmRPContext();
    ~HgmRPContext() noexcept = default;

    int32_t InitHgmConfig(std::unordered_map<std::string, std::string>& sourceTuningConfig,
        std::unordered_map<std::string, std::string>& solidLayerConfig, std::vector<std::string>& appBufferList);
    void SetServiceToProcessInfo(sptr<HgmServiceToProcessInfo> hgmServiceToProcessInfo,
        uint32_t *pendingScreenRefreshRate, uint64_t *pendingConstraintRelativeTime);

    FrameRateRange& GetRSCurrRangeRef()
    {
        return rsCurrRange_;
    }

    const FrameRateRange& GetRSCurrRange()
    {
        return rsCurrRange_;
    }

    void UpdateSurfaceData(const std::string& surfaceName, pid_t pid)
    {
        surfaceData_.emplace_back(std::tuple<std::string, pid_t>({surfaceName, pid}));
    }

    void ClearSurfaceData()
    {
        surfaceData_.clear();
    }

    std::vector<std::tuple<std::string, pid_t>>& GetMutableSurfaceData()
    {
        return surfaceData_;
    }

    const std::string& GetGameNodeName()
    {
        return gameNodeName_;
    }

    bool AdaptiveStatus()
    {
        return isAdaptive_;
    }

    bool GetLtpoEnabled()
    {
        return ltpoEnabled_;
    }

    bool IsDelayMode()
    {
        return isDelayMode_;
    }

    int32_t GetPipelineOffsetPulseNum()
    {
        return pipelineOffsetPulseNum_;
    }

    void SetIsGameNodeOnTree(bool isGameNodeOnTree)
    {
        isGameNodeOnTree_ = isGameNodeOnTree;
    }

    bool IsGameNodeOnTree()
    {
        return isGameNodeOnTree_;
    }

    const std::vector<std::string>& GetAppBufferList() const
    {
        return appBufferList_;
    }

    std::shared_ptr<HgmRPEnergy> GetHgmRPEnergy()
    {
        return hgmRPEnergy_;
    }

    const std::function<int32_t(RSPropertyUnit, float, int32_t, int32_t)>& GetConvertFrameRateFunc() const
    {
        return convertFrameRateFunc_;
    }

private:
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
    std::vector<std::string> appBufferList_;
    std::function<int32_t(RSPropertyUnit, float, int32_t, int32_t)> convertFrameRateFunc_ = nullptr;
};
} // namespace OHOS
} // namespace Rosen

#endif // HGM_RP_CONTEXT_H