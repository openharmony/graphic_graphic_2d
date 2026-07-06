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
#ifndef RS_OPINC_LAYER_SPLITTER_PLANNER_H
#define RS_OPINC_LAYER_SPLITTER_PLANNER_H

#include "pipeline/layer_split/splitter/opinc/controller/rs_opinc_split_controller.h"
#include "pipeline/layer_split/surface/rs_split_surface.h"
#include "pipeline/layer_split/splitter/rs_layer_splitter_planner.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
class RSRenderNodeDrawable;
class RSScreenRenderNodeDrawable;
}

class RSOpincLayerSplitterPlanner : public RSLayerSplitterPlanner {
public:
    RSOpincLayerSplitterPlanner();
    ~RSOpincLayerSplitterPlanner();

    void Reset() override;
    void MoveSplitSurfaceNode() override;
    void InitSplitSurface(const ScreenInfo& screenInfo) override;
    void CheckNeedLeave() override;
    void CheckSplitNodeIntersectFilter(const std::shared_ptr<RSSurfaceRenderNode>& hwcNode) override;
    void UpdateSplitPlan() override;
    void UpdateScreenDirtyRegion(std::shared_ptr<RSDirtyRegionManager> curScreenDirtyManager) override;

    // for direct composition
    bool CheckOpIncNodeFromCommand(NodeId nodeId) override;
    std::pair<bool, Vector4f> GetBoundsFromModifier(const std::shared_ptr<RSRenderNode>& node) override;
    bool CheckCanDoDirectComposition() override;

    SurfaceStatus GetSurfaceStatus() override;
    void UnregisterSplitSurfaceNode() override;

    void ClearAllChildrenLayerObjects() override;
    void Sync(std::shared_ptr<RSLayerSplitterProcessor> processor_) override;
    void UpdateChildren(const std::shared_ptr<RSRenderNode>& parent) override;
    std::string GetDfxString() override;
    void SetOpIncParentNode(const std::shared_ptr<RSRenderNode>& parentNode) override;

    std::shared_ptr<RSRenderNode> GetOpIncParentNode() const override { return opIncParentNode_; }
    PlanStatus GetPlanStatus() const override { return planStatus_; }
    std::shared_ptr<SplitSurface> GetSplitSurface() const override { return splitSurface_; }
    bool GetNeedLeave() const override { return needLeave_; }
    const RectF& GetSrcRect() const override { return srcRect_; }
    const RectF& GetDstRect() const override { return dstRect_; }
    std::shared_ptr<RequestController> GetRequestController() const override { return requestController_; }
    const std::vector<std::pair<NodeId, Vector4f>>& GetOpIncNodes() const override { return opIncNodes_; }
    const std::vector<std::pair<NodeId, Vector4f>>& GetLastOpIncNodes() const override { return lastOpIncNodes_; }

private:
    static bool Vector2fNearEqual(const Vector2f& x, const Vector2f& y)
    {
        constexpr float eps = 2.f;
        return std::abs(x[0] - y[0]) < eps && std::abs(x[1] - y[1]) < eps;
    }
    void SetSrcAndDstRect();
    bool CollectOpIncNodes();
    bool IsOpIncNodesChanged();
    bool CheckRectOverlap(const RectI& rect, const std::vector<RectI>& nonOpIncNodeRects) const;
    bool UpdateBufferBounds();
    void SetColorSpaceInfo();
    void ProcessPlanStatusAction();
private:
    std::vector<std::pair<NodeId, Vector4f>> opIncNodes_;
    std::vector<std::pair<NodeId, Vector4f>> lastOpIncNodes_;
    std::shared_ptr<RSRenderNode> opIncParentNode_ = nullptr;

    RectI bufferBounds_;
    RectF srcRect_;
    RectF dstRect_;
    RectI itemBounds_;
    bool isUpdateBuffer_;
    Vector2f currentOffset_;

    bool needLeave_ = false;
    std::shared_ptr<RequestController> requestController_ = nullptr;

    uint32_t screenWidth_;
    uint32_t screenHeight_;
    std::shared_ptr<SplitSurface> splitSurface_ = nullptr;
    bool layerSplitterEnabled_ = false;
    PlanStatus planStatus_ = PlanStatus::OFF;

    // for direct composition
    std::set<NodeId> visitedNodeId_;
    std::unordered_set<NodeId> lastOpIncNodeIds_;
    bool canDoDirectComposition_ = false;

    std::unordered_set<std::shared_ptr<RSRenderNode>> lastFrameChildSet_;
    std::unordered_set<std::shared_ptr<RSRenderNode>> currFrameChildSet_;
};

} // namespace OHOS::Rosen

#endif // RS_LAYER_SPLITTER_PLANNER_H