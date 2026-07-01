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
#ifndef RS_LAYER_SPLIT_MANAGER_H
#define RS_LAYER_SPLIT_MANAGER_H

#include "pipeline/layer_split/buffer/rs_layer_split_surface_buffer.h"
#include "pipeline/layer_split/selector/opinc/rs_opinc_split_node_selector.h"
#include "pipeline/layer_split/splitter/opinc/controller/rs_opinc_split_controller.h"
#include "pipeline/layer_split/splitter/opinc/rs_opinc_layer_splitter_planner.h"
#include "pipeline/layer_split/splitter/opinc/rs_opinc_layer_splitter_processor.h"
#include "pipeline/layer_split/surface/rs_split_surface.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_processor.h"
#include "system/rs_system_parameters.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
class RSRenderNodeDrawable;
class RSScreenRenderNodeDrawable;
}

class RSLayerSplitterPlanner;
class RSLayerSplitterProcessor;

class RSLayerSplitManager {
public:
    static RSLayerSplitManager* GetInstance();

    RSLayerSplitManager(const RSLayerSplitManager&) = delete;
    RSLayerSplitManager& operator=(const RSLayerSplitManager&) = delete;
    RSLayerSplitManager(RSLayerSplitManager&&) = delete;
    RSLayerSplitManager& operator=(RSLayerSplitManager&&) = delete;

    void Reset(uint64_t vsyncId);
    void MoveSplitSurfaceNode();
    void RecordSplitNode(std::shared_ptr<RSRenderNode> node);
    void CheckNeedLeave();
    void CheckSplitNodeIntersectFilter(const std::shared_ptr<RSSurfaceRenderNode>& hwcNode);
    void UpdatePlanAndDirtyRegion(std::shared_ptr<RSDirtyRegionManager> dirtyManager);
    void Sync(uint64_t vsyncId);
    void DrawDfx(std::shared_ptr<RSPaintFilterCanvas> canvas, uint64_t vsyncId);
    bool CheckDoDirectCompositionWithSplitLayer(
        std::shared_ptr<TransactionDataMap> transactionDataEffective, bool doDirectComposition);
    void InitSplitSurface(const ScreenInfo& screenInfo);
    void SetEnabled(bool isEnabled);

private:
    RSLayerSplitManager();

    bool isEnabled_ = false;
    bool isGlobalEnabled_ = false;
    std::vector<std::shared_ptr<SplitNodeSelector>> selectorVec_;
    std::unordered_map<NodeId, std::shared_ptr<RSLayerSplitterPlanner>> plannerMap_;
    std::unordered_map<NodeId, std::shared_ptr<RSLayerSplitterProcessor>> processorMap_;
    std::unordered_map<NodeId, std::shared_ptr<RSRenderNode>> parentNodeMap_;
};
} // namespace OHOS::Rosen

#endif // RS_LAYER_SPLIT_MANAGER_H