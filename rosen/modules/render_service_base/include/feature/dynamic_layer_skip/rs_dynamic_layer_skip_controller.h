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

#ifndef RENDER_SERVICE_BASE_FEATURE_RS_DYNAMIC_LAYER_SKIP_CONTROLLER_H
#define RENDER_SERVICE_BASE_FEATURE_RS_DYNAMIC_LAYER_SKIP_CONTROLLER_H

#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS {
namespace Rosen {
class RSDynamicLayerSkipController;
struct RSB_EXPORT LayerSkipContext {
    bool screenLayerInvalid_ = false;
    std::vector<NodeId> relevantSurfaceNodeIds_ = {};
    void SyncFrom(const RSDynamicLayerSkipController& controller);
    void Reset();
};

class RSB_EXPORT RSDynamicLayerSkipController {
public:
    RSDynamicLayerSkipController() = default;

    ~RSDynamicLayerSkipController() noexcept = default;

    bool IsScreenLayerInvalid() const;

    void Init(const RectI& screenRect, bool globalDisabled);

    void CheckNodeDrawProperty(RSRenderNode& node);

    void MarkParentSubTreeHasDrawContent(RSRenderNode& node) const;

    bool HasDrawContentDrawableInRange(const RSRenderNode& node, RSDrawableSlot begin, RSDrawableSlot end) const;

    void VisitRenderNode(std::shared_ptr<RSSurfaceRenderNode> surfaceNode, RSRenderNode& node);

    void DetectScreenLayerValidity(RSSurfaceRenderNode& rootNode);

    bool HasFullScreenSelfDrawingSurface(RSSurfaceRenderNode& rootNode) const;

    void VerifyScreenLayerValidity(float screenNodeGlobalZOrder);
private:
    RectI screenRect_;
    bool screenLayerInvalid_ = false;
    bool globalOccluderDetected_ = false;
    NodeId occluderInstanceRootNodeId_ = INVALID_NODEID;
    std::vector<RSSurfaceRenderNode::WeakPtr> fullScreenSelfDrawingSurface_ = {};
    bool globalDisabled_ = false;
    int32_t visitedRenderNodeCount_ = 0;

    friend struct LayerSkipContext;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_FEATURE_RS_DYNAMIC_LAYER_SKIP_CONTROLLER_H
