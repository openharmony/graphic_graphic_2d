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

#include <cstdint>
#include <memory>
#include <mutex>
#include <set>
#include <parameters.h>

#include "overdraw/rs_cpu_overdraw_canvas_listener.h"
#include "overdraw/rs_gpu_overdraw_canvas_listener.h"
#include "overdraw/rs_overdraw_controller.h"
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
    RSUniRenderVisitor(std::shared_ptr<RSPaintFilterCanvas> canvas, uint32_t surfaceIndex);
    explicit RSUniRenderVisitor(const RSUniRenderVisitor& visitor);
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

    bool DoDirectComposition(std::shared_ptr<RSBaseRenderNode> rootNode);

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

    bool GetAnimateState() const
    {
        return doAnimate_;
    }

    void MarkHardwareForcedDisabled()
    {
        isHardwareForcedDisabled_ = true;
    }

    void SetHardwareEnabledNodes(const std::vector<std::shared_ptr<RSSurfaceRenderNode>>& hardwareEnabledNodes);
    void AdjustZOrderAndCreateLayer();

    void CopyForParallelPrepare(std::shared_ptr<RSUniRenderVisitor> visitor);
    // Some properties defiend before ProcessSurfaceRenderNode() may be used in
    // ProcessSurfaceRenderNode() method. We should copy these properties in parallel render.
    void CopyPropertyForParallelVisitor(RSUniRenderVisitor *mainVisitor);

private:
    void DrawDirtyRectForDFX(const RectI& dirtyRect, const SkColor color,
        const SkPaint::Style fillType, float alpha, int edgeWidth);
    void DrawDirtyRegionForDFX(std::vector<RectI> dirtyRects);
    void DrawAllSurfaceDirtyRegionForDFX(RSDisplayRenderNode& node, const Occlusion::Region& region);
    void DrawTargetSurfaceDirtyRegionForDFX(RSDisplayRenderNode& node);
    void DrawAllSurfaceOpaqueRegionForDFX(RSDisplayRenderNode& node);
    void DrawSurfaceOpaqueRegionForDFX(RSSurfaceRenderNode& node);
    std::vector<RectI> GetDirtyRects(const Occlusion::Region &region);
    /* calculate display/global (between windows) level dirty region, current include:
     * 1. window move/add/remove 2. transparent dirty region
     * when process canvas culling, canvas intersect with surface's visibledirty region or
     * global dirty region will be skipped
     */
    void CalcDirtyDisplayRegion(std::shared_ptr<RSDisplayRenderNode>& node) const;
    void CalcDirtyRegionForFilterNode(std::shared_ptr<RSDisplayRenderNode>& node);
    // set global dirty region to each surface node
    void SetSurfaceGlobalDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node);
    void SetSurfaceGlobalAlignedDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node,
        const Occlusion::Region alignedDirtyRegion);

    void InitCacheSurface(RSRenderNode& node, int width, int height);
    void DrawChildRenderNode(RSRenderNode& node);
    void CheckColorSpace(RSSurfaceRenderNode& node);
    void AddOverDrawListener(std::unique_ptr<RSRenderFrame>& renderFrame,
        std::shared_ptr<RSCanvasListener>& overdrawListener);
    /* Judge if surface render node could skip preparation:
     * 1. not leash window
     * 2. parent not dirty
     * 3. no processWithCommands_ of node's corresponding pid
     * If so, reset status flag and stop traversal
     */
    bool CheckIfSurfaceRenderNodeStatic(RSSurfaceRenderNode& node);
    bool IsHardwareComposerEnabled();

    void ClearTransparentBeforeSaveLayer();
    // mark parentNode's child surfaceView nodes hardware forced disabled
    void MarkSubHardwareEnableNodeState(RSSurfaceRenderNode& parentNode);

    void RecordAppWindowNodeAndPostTask(RSSurfaceRenderNode& node, float width, float height);
    // offscreen render related
    void PrepareOffscreenRender(RSRenderNode& node);
    void FinishOffscreenRender();
    void ParallelPrepareDisplayRenderNodeChildrens(RSDisplayRenderNode& node);
    bool AdaptiveSubRenderThreadMode(uint32_t renderNodeNum);
    void ParallelRenderEnableHardwareComposer(RSSurfaceRenderNode& node);
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
    bool isOpaqueRegionDfxEnabled_ = false;
    bool isQuickSkipPreparationEnabled_ = false;
    bool isHardwareComposerEnabled_ = false;
    std::vector<std::string> dfxTargetSurfaceNames_;
    PartialRenderType partialRenderType_;
    bool isDirty_ = false;
    bool needFilter_ = false;
    ColorGamut newColorSpace_ = ColorGamut::COLOR_GAMUT_SRGB;
    std::vector<ScreenColorGamut> colorGamutmodes_;
    ContainerWindowConfigType containerWindowConfig_;
    pid_t currentFocusedPid_ = -1;

    bool needColdStartThread_ = false; // flag used for cold start app window
    bool needCheckFirstFrame_ = false; // flag used for avoiding notifying first frame repeatedly

    bool isDirtyRegionAlignedEnable_ = false;
    bool isParallel_ = false;
    std::shared_ptr<std::mutex> surfaceNodePrepareMutex_;
    uint32_t parallelRenderVisitorIndex_ = 0;
    ParallelRenderingType parallelRenderType_;

    RectI prepareClipRect_{0, 0, 0, 0}; // renderNode clip rect used in Prepare

    // count prepared and processed canvasnode numbers per app
    // unirender visitor resets every frame, no overflow risk here
    unsigned int preparedCanvasNodeInCurrentSurface_ = 0;
    unsigned int processedCanvasNodeInCurrentSurface_ = 0;

    float globalZOrder_ = 0.0f;
    bool isFreeze_ = false;
    bool isHardwareForcedDisabled_ = false; // indicates if hardware composer is totally disabled
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_VISITOR_H
