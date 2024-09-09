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
#include "vsync_distributor.h"
#include "common/rs_common_def.h"
#include "common/rs_occlusion_region.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "system/rs_system_parameters.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {

class RSUniRenderVisitor;
class RSVsyncRateReduceManager {
public:
    RSVsyncRateReduceManager() {};
    ~RSVsyncRateReduceManager() = default;
    void CollectVSyncRate(RSSurfaceRenderNode& node, RSVisibleLevel visibleLevel);
    void SetVSyncRateByVisibleLevel(std::map<NodeId, RSVisibleLevel>& pidVisMap,
        std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces);
    void SetUniVSyncRateByVisibleLevel(const std::shared_ptr<RSUniRenderVisitor>& uniVisitor);
    void NotifyVSyncRates(const std::map<NodeId, RSVisibleLevel>& vSyncRates);

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
        lastVisMapForVSyncRate_.clear();
    }
    bool GetIsReduceBySystemAnimatedScenes() const
    {
        return isReduceBySystemAnimatedScenes_;
    }
    void SetIsReduceBySystemAnimatedScenes(bool isReduceBySystemAnimatedScenes)
    {
        isReduceBySystemAnimatedScenes_ = isReduceBySystemAnimatedScenes;
    }
    void ResetFrameValues();
    void Init(const sptr<VSyncDistributor>& appVSyncDistributor);

private:
    void SetVSyncRatesChanged(bool vSyncRatesChanged)
    {
        vSyncRatesChanged_ = vSyncRatesChanged_ || vSyncRatesChanged;
    }
private:
    bool vsyncControlEnabled_ = false;
    bool vSyncRatesChanged_ = false;
    std::vector<NodeId> curAllMainAndLeashWindowNodesIds_;
    std::vector<NodeId> lastAllMainAndLeashWindowNodesIds_;
    std::map<NodeId, RSVisibleLevel> visMapForVSyncRate_;
    std::map<NodeId, RSVisibleLevel> lastVisMapForVSyncRate_;

    NodeId focusedNodeId_ = 0;
    NodeId lastFocusedNodeId_ = 0;
    bool isReduceBySystemAnimatedScenes_ = false;
    sptr<VSyncDistributor> appVSyncDistributor_ = nullptr;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_VSYNC_RATE_REDUCE_H
