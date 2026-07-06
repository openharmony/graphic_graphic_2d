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
#ifndef RS_LAYER_SPLITTER_PLANNER_H
#define RS_LAYER_SPLITTER_PLANNER_H

#include "pipeline/layer_split/common/rs_layer_split_types.h"

namespace OHOS::Rosen {
class RSLayerSplitterProcessor;
class RSLayerSplitterPlanner {
public:
    virtual ~RSLayerSplitterPlanner() = default;

    virtual void Reset() = 0;
    virtual void MoveSplitSurfaceNode() = 0;
    virtual void InitSplitSurface(const ScreenInfo& screenInfo) = 0;
    virtual void CheckNeedLeave() = 0;
    virtual void CheckSplitNodeIntersectFilter(const std::shared_ptr<RSSurfaceRenderNode>& hwcNode);
    virtual void UpdateSplitPlan() = 0;
    virtual void UpdateScreenDirtyRegion(std::shared_ptr<RSDirtyRegionManager> curScreenDirtyManager) = 0;

    // for direct composition
    virtual bool CheckOpIncNodeFromCommand(NodeId nodeId) = 0;
    virtual std::pair<bool, Vector4f> GetBoundsFromModifier(const std::shared_ptr<RSRenderNode>& node) = 0;
    virtual bool CheckCanDoDirectComposition() = 0;

    virtual SurfaceStatus GetSurfaceStatus() = 0;
    virtual void UnregisterSplitSurfaceNode() = 0;

    virtual void ClearAllChildrenLayerObjects() = 0;
    virtual void Sync(std::shared_ptr<RSLayerSplitterProcessor> processor_) = 0;
    virtual void UpdateChildren(const std::shared_ptr<RSRenderNode>& parent) = 0;
    virtual std::string GetDfxString() = 0;
    virtual std::shared_ptr<RSRenderNode> GetOpIncParentNode() const = 0;

    virtual PlanStatus GetPlanStatus() const = 0;
    virtual void SetOpIncParentNode(const std::shared_ptr<RSRenderNode>& parentNode) = 0;
    virtual std::shared_ptr<SplitSurface> GetSplitSurface() const = 0;
    virtual bool GetNeedLeave() const = 0;
    virtual const RectF& GetSrcRect() const = 0;
    virtual const RectF& GetDstRect() const = 0;
    virtual std::shared_ptr<RequestController> GetRequestController() const = 0;
    virtual const std::vector<std::pair<NodeId, Vector4f>>& GetOpIncNodes() const = 0;
    virtual const std::vector<std::pair<NodeId, Vector4f>>& GetLastOpIncNodes() const = 0;
};

} // namespace OHOS::Rosen

#endif
