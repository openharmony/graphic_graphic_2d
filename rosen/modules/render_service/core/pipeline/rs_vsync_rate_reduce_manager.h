/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_VSYNC_RATE_REDUCE_MANAGER_H
#define RENDER_SERVICE_VSYNC_RATE_REDUCE_MANAGER_H

#include <cstdint>
#include <cstdio>
#include <chrono>
#include <screen_manager/rs_screen_info.h>
#include "common/rs_common_def.h"
#include "common/rs_occlusion_region.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "system/rs_system_parameters.h"
#include "visitor/rs_node_visitor.h"
#include "vsync_distributor.h"

namespace OHOS {
namespace Rosen {

struct SurfaceVRateInfo {
    NodeId nodeId = 0;
    std::string name;
    Occlusion::Region visibleRegion;
    RectI maxVisRect;
    int appWindowArea = 0;
};

class RSVsyncRateReduceManager {
public:
    RSVsyncRateReduceManager() {};
    ~RSVsyncRateReduceManager() = default;

    void SetFocusedNodeId(NodeId focusedNodeId)
    {
        focusedNodeId_ = focusedNodeId;
    }
    void PushWindowNodeId(NodeId nodeId)
    {
        curAllMainAndLeashWindowNodesIds_.emplace_back(nodeId);
    }
    void ClearLastVisMapForVsyncRate()
    {
        lastVisMapForVSyncVisLevel_.clear();
    }
    bool GetVsyncControlEnabled() const
    {
        return vsyncControlEnabled_;
    }

    void FrameDurationBegin()
    {
        curTime_ = Now();
    }
    void FrameDurationEnd()
    {
        if (oneFramePeriod_ > 0) {
            float val = (Now() - curTime_) / static_cast<float>(oneFramePeriod_);
            EnqueueFrameDuration(val);
        }
        curTime_ = 0;
    }

    bool GetIsReduceBySystemAnimatedScenes() const
    {
        return isReduceBySystemAnimatedScenes_;
    }
    void SetIsReduceBySystemAnimatedScenes(bool isReduceBySystemAnimatedScenes)
    {
        isReduceBySystemAnimatedScenes_ = isReduceBySystemAnimatedScenes;
    }
    void Init(const sptr<VSyncDistributor>& appVSyncDistributor);
    void ResetFrameValues(uint32_t refreshRate);
    void CollectSurfaceVsyncInfo(const ScreenInfo& screenInfo, RSSurfaceRenderNode& node);
    void SetUniVsync();

    void SetVSyncRateByVisibleLevel(std::map<NodeId, RSVisibleLevel>& pidVisMap,
        std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces);
    void NotifyVSyncRates(const std::map<NodeId, RSVisibleLevel>& vSyncRates);

private:
    void NotifyVRates();
    int UpdateRatesLevel();
    void CalcRates();
    bool CheckNeedNotify();
    int GetRateByBalanceLevel(double vVal);
    void EnqueueFrameDuration(float duration);
    static inline Occlusion::Rect GetMaxVerticalRect(const Occlusion::Region& region);
    static Occlusion::Rect CalcMaxVisibleRect(const Occlusion::Region& region, int appWindowArea);
    static float CalcVValByAreas(int windowArea, int maxVisRectArea, int visArea);

    uint64_t Now();
    void SetVSyncRatesChanged(bool vSyncRatesChanged)
    {
        vSyncRatesChanged_ = vSyncRatesChanged_ || vSyncRatesChanged;
    }
private:
    bool vsyncControlEnabled_ = false;
    bool vSyncRatesChanged_ = false;
    std::map<NodeId, int> vSyncRateMap_;
    std::map<NodeId, int> lastVSyncRateMap_;
    std::vector<NodeId> curAllMainAndLeashWindowNodesIds_;
    std::vector<NodeId> lastAllMainAndLeashWindowNodesIds_;
    std::map<NodeId, RSVisibleLevel> visMapForVSyncVisLevel_;
    std::map<NodeId, RSVisibleLevel> lastVisMapForVSyncVisLevel_;

    NodeId focusedNodeId_ = 0;
    NodeId lastFocusedNodeId_ = 0;

    uint64_t curTime_ = 0;
    int curRatesLevel_ = 0;
    int64_t oneFramePeriod_ = 0;
    uint32_t rsRefreshRate_ = 0;
    std::mutex mutexFrameDuration_;
    std::deque<float> frameDurations_;

    bool isSystemAnimatedScenes_ = false;
    bool isReduceBySystemAnimatedScenes_ = false;
    DeviceType deviceType_ = DeviceType::PC;
    std::map<NodeId, SurfaceVRateInfo> surfaceVRateMap_;
    sptr<VSyncDistributor> appVSyncDistributor_ = nullptr;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_VSYNC_RATE_REDUCE_H