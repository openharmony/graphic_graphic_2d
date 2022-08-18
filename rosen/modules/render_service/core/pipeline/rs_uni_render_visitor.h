/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_VISITOR_H
#define RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_VISITOR_H

#include <set>

#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_processor.h"
#include "screen_manager/rs_screen_manager.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;

class RSUniRenderVisitor : public RSNodeVisitor {
public:
    RSUniRenderVisitor();
    ~RSUniRenderVisitor() override;

    void PrepareBaseRenderNode(RSBaseRenderNode& node) override;
    void PrepareDisplayRenderNode(RSDisplayRenderNode& node) override;
    void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override;
    void PrepareRootRenderNode(RSRootRenderNode& node) override;
    void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override;

    void ProcessBaseRenderNode(RSBaseRenderNode& node) override;
    void ProcessDisplayRenderNode(RSDisplayRenderNode& node) override;
    void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override;
    void ProcessRootRenderNode(RSRootRenderNode& node) override;
    void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override;

    void SetAnimateState(bool doAnimate)
    {
        doAnimate_ = doAnimate;
    }

private:
    void DrawRectOnCanvas(const RectI& dirtyRect, const SkColor color,
        const SkPaint::Style fillType, float alpha);
    void DrawDirtyRegion();
    std::vector<RectI> GetDirtyRects(const Occlusion::Region &region);
    RectI CoordinateTransform(const RectI& rect);
    inline bool GetSurfaceViewDirtyEnabled()
    {
        return std::atoi((system::GetParameter("rosen.uni.surfaceviewdirty.enabled", "0")).c_str()) != 0;
    }

    ScreenInfo screenInfo_;
    std::shared_ptr<RSDirtyRegionManager> curSurfaceDirtyManager_;
    bool dirtyFlag_ { false };
    std::unique_ptr<RSPaintFilterCanvas> canvas_;
    std::map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> dirtySurfaceNodeMap_;
    SkRect boundsRect_;
    Gravity frameGravity_ = Gravity::DEFAULT;

    int32_t offsetX_ { 0 };
    int32_t offsetY_ { 0 };
    std::shared_ptr<RSProcessor> processor_;

    ScreenId currentVisitDisplay_;
    std::map<ScreenId, bool> displayHasSecSurface_;
    std::set<ScreenId> mirroredDisplays_;
    bool isSecurityDisplay_ = false;

    std::shared_ptr<RSRenderEngine> renderEngine_;

    std::shared_ptr<RSDirtyRegionManager> curDisplayDirtyManager_;
    std::shared_ptr<RSDisplayRenderNode> curDisplayNode_;
    bool doAnimate_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_VISITOR_H
