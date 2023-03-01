/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include <parameters.h>

#include "platform/ohos/overdraw/rs_cpu_overdraw_canvas_listener.h"
#include "platform/ohos/overdraw/rs_gpu_overdraw_canvas_listener.h"
#include "platform/ohos/overdraw/rs_overdraw_controller.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_processor.h"
#include "screen_manager/rs_screen_manager.h"
#include "visitor/rs_node_visitor.h"

class SkPicture;
namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;
class RSUniRenderVisitor : public RSNodeVisitor {
public:
    RSUniRenderVisitor();
    ~RSUniRenderVisitor() override;

    void PrepareBaseRenderNode(RSBaseRenderNode& node) override;
    void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override;
    void PrepareDisplayRenderNode(RSDisplayRenderNode& node) override;
    void PrepareProxyRenderNode(RSProxyRenderNode& node) override;
    void PrepareRootRenderNode(RSRootRenderNode& node) override;
    void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override;

    void ProcessBaseRenderNode(RSBaseRenderNode& node) override;
    void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override;
    void ProcessDisplayRenderNode(RSDisplayRenderNode& node) override;
    void ProcessProxyRenderNode(RSProxyRenderNode& node) override;
    void ProcessRootRenderNode(RSRootRenderNode& node) override;
    void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override;

    void SetAnimateState(bool doAnimate)
    {
        doAnimate_ = doAnimate;
    }

    void SetDirtyFlag(bool isDirty)
    {
        isDirty_ = isDirty;
    }

    void SetFocusedWindowPid(pid_t pid)
    {
        currentFocusedPid_ = pid;
    }
private:
    void DrawDirtyRectForDFX(const RectI& dirtyRect, const SkColor color,
        const SkPaint::Style fillType, float alpha);
    void DrawDirtyRegionForDFX(std::vector<RectI> dirtyRects);
    void DrawAllSurfaceDirtyRegionForDFX(RSDisplayRenderNode& node, const Occlusion::Region& region);
    void DrawTargetSurfaceDirtyRegionForDFX(RSDisplayRenderNode& node);
    std::vector<RectI> GetDirtyRects(const Occlusion::Region &region);
    /* calculate display/global (between windows) level dirty region, current include:
     * 1. window move/add/remove 2. transparent dirty region
     * when process canvas culling, canvas intersect with surface's visibledirty region or
     * global dirty region will be skipped
     */
    void CalcDirtyDisplayRegion(std::shared_ptr<RSDisplayRenderNode>& node) const;
    void CalcDirtyRegionForFilterNode(std::shared_ptr<RSDisplayRenderNode>& node) const;
    // set global dirty region to each surface node
    void SetSurfaceGlobalDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node);

    void InitCacheSurface(RSSurfaceRenderNode& node, int width, int height);
    void SetPaintOutOfParentFlag(RSBaseRenderNode& node);
    void CheckColorSpace(RSSurfaceRenderNode& node);
    void AddOverDrawListener(std::unique_ptr<RSRenderFrame>& renderFrame,
        std::shared_ptr<RSCanvasListener>& overdrawListener);

    void RecordAppWindowNodeAndPostTask(RSSurfaceRenderNode& node, float width, float height);
    
    // offscreen render related
    void PrepareOffscreenRender(RSRenderNode& node);
    void FinishOffscreenRender();
    sk_sp<SkSurface> offscreenSurface_;                 // temporary holds offscreen surface
    std::shared_ptr<RSPaintFilterCanvas> canvasBackup_; // backup current canvas before offscreen render

    ScreenInfo screenInfo_;
    std::shared_ptr<RSDirtyRegionManager> curSurfaceDirtyManager_;
    std::shared_ptr<RSSurfaceRenderNode> curSurfaceNode_;
    float curAlpha_ = 1.f;
    bool dirtyFlag_ { false };
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
    std::map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> dirtySurfaceNodeMap_;
    SkRect boundsRect_;
    Gravity frameGravity_ = Gravity::DEFAULT;

    int32_t offsetX_ { 0 };
    int32_t offsetY_ { 0 };
    std::shared_ptr<RSProcessor> processor_;
    SkMatrix parentSurfaceNodeMatrix_;

    ScreenId currentVisitDisplay_;
    std::map<ScreenId, bool> displayHasSecSurface_;
    std::set<ScreenId> mirroredDisplays_;
    bool isSecurityDisplay_ = false;

    std::shared_ptr<RSBaseRenderEngine> renderEngine_;

    std::shared_ptr<RSDirtyRegionManager> curDisplayDirtyManager_;
    std::shared_ptr<RSDisplayRenderNode> curDisplayNode_;
    bool doAnimate_ = false;
    bool isPartialRenderEnabled_ = false;
    bool isOpDropped_ = false;
    bool isDirtyRegionDfxEnabled_ = false; // dirtyRegion DFX visualization
    bool isTargetDirtyRegionDfxEnabled_ = false;
    bool isOcclusionEnabled_ = false;
    std::vector<std::string> dfxTargetSurfaceNames_;
    PartialRenderType partialRenderType_;
    bool isDirty_ = false;
    bool needFilter_ = false;
    std::unordered_map<NodeId, std::vector<RectI>> filterRects_;
    ColorGamut newColorSpace_ = ColorGamut::COLOR_GAMUT_SRGB;
    std::vector<ScreenColorGamut> colorGamutmodes_;
    ContainerWindowConfigType containerWindowConfig_;
    pid_t currentFocusedPid_ = -1;

    bool needColdStartThread_ = false; // flag used for cold start app window
    bool needDrawStartingWindow_ = true; // flag used for avoiding drawing both app and starting window
    bool needCheckFirstFrame_ = false; // flag used for avoiding notifying first frame repeatedly
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_VISITOR_H
