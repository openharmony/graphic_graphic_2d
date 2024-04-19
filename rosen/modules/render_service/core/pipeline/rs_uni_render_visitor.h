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

#include <cstdint>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <parameters.h>
#include <set>

#include "rs_base_render_engine.h"
#include "system/rs_system_parameters.h"

#include "params/rs_render_thread_params.h"
#include "pipeline/round_corner_display/rs_rcd_render_manager.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_processor.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_uni_hwc_prevalidate_util.h"
#include "platform/ohos/overdraw/rs_cpu_overdraw_canvas_listener.h"
#include "platform/ohos/overdraw/rs_gpu_overdraw_canvas_listener.h"
#include "platform/ohos/overdraw/rs_overdraw_controller.h"
#include "screen_manager/rs_screen_manager.h"
#include "visitor/rs_node_visitor.h"

#ifdef DDGR_ENABLE_FEATURE_OPINC
#include "rs_auto_cache.h"
#endif
#ifdef RS_PROFILER_ENABLED
#include "rs_profiler_capture_recorder.h"
#endif

class SkPicture;
namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;
class RSUniRenderVisitor : public RSNodeVisitor {
public:
    using SurfaceDirtyMgrPair = std::pair<std::shared_ptr<RSSurfaceRenderNode>, std::shared_ptr<RSSurfaceRenderNode>>;
    RSUniRenderVisitor();
    RSUniRenderVisitor(std::shared_ptr<RSPaintFilterCanvas> canvas, uint32_t surfaceIndex);
    explicit RSUniRenderVisitor(const RSUniRenderVisitor& visitor);
    ~RSUniRenderVisitor() override;

    // To prepare nodes between displayRenderNode and app nodes.
    void QuickPrepareEffectRenderNode(RSEffectRenderNode& node) override;
    void QuickPrepareCanvasRenderNode(RSCanvasRenderNode& node) override;
    void QuickPrepareDisplayRenderNode(RSDisplayRenderNode& node) override;
    void QuickPrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override;
    void QuickPrepareChildren(RSRenderNode& node) override;
    /* Prepare relevant calculation */
    // considering occlusion info for app surface as well as widget
    bool IsSubTreeOccluded(RSRenderNode& node) const;
    // restore node's flag and filter dirty collection
    void PostPrepare(RSRenderNode& node, bool subTreeSkipped = false);
    void CalculateOcclusion(RSSurfaceRenderNode& node);

    void PrepareChildren(RSRenderNode& node) override;
    void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override;
    void PrepareDisplayRenderNode(RSDisplayRenderNode& node) override;
    void PrepareProxyRenderNode(RSProxyRenderNode& node) override;
    // prepareroot also used for quickprepare
    void PrepareRootRenderNode(RSRootRenderNode& node) override;
    void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override;
    void PrepareEffectRenderNode(RSEffectRenderNode& node) override;

    void ProcessChildren(RSRenderNode& node) override;
    void ProcessChildInner(RSRenderNode& node, const RSRenderNode::SharedPtr child);
    void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override;
    void ProcessDisplayRenderNode(RSDisplayRenderNode& node) override;
    void ProcessProxyRenderNode(RSProxyRenderNode& node) override;
    void ProcessRootRenderNode(RSRootRenderNode& node) override;
    void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override;
    void ProcessEffectRenderNode(RSEffectRenderNode& node) override;

    bool DoDirectComposition(std::shared_ptr<RSBaseRenderNode> rootNode);
    void ChangeCacheRenderNodeMap(RSRenderNode& node, const uint32_t count = 0);
    void UpdateCacheRenderNodeMap(RSRenderNode& node);
    bool GenerateNodeContentCache(RSRenderNode& node);
    bool InitNodeCache(RSRenderNode& node);
    void CopyVisitorInfos(std::shared_ptr<RSUniRenderVisitor> visitor);
    void CheckSkipRepeatShadow(RSRenderNode& node, const bool resetStatus);
    void SetNodeSkipShadow(std::shared_ptr<RSRenderNode> node, const bool resetStatus);
    void SetProcessorRenderEngine(std::shared_ptr<RSBaseRenderEngine> renderEngine)
    {
        renderEngine_ = renderEngine;
    }

    void SetAnimateState(bool doAnimate)
    {
        doAnimate_ = doAnimate;
    }

    void SetDirtyFlag(bool isDirty)
    {
        isDirty_ = isDirty;
    }

    void SetFocusedNodeId(uint64_t nodeId, uint64_t leashId)
    {
        currentFocusedNodeId_ = nodeId;
        focusedLeashWindowId_ = leashId;
    }

    void SetSubThreadConfig(uint32_t threadIndex)
    {
        isSubThread_ = true;
        isHardwareForcedDisabled_ = true;
        threadIndex_ = threadIndex;
    }

    bool GetAnimateState() const
    {
        return doAnimate_;
    }

    void MarkHardwareForcedDisabled()
    {
        isHardwareForcedDisabled_ = true;
    }

    void SetUniRenderThreadParam(std::unique_ptr<RSRenderThreadParams>& renderThreadParams);
    void SetHardwareEnabledNodes(const std::vector<std::shared_ptr<RSSurfaceRenderNode>>& hardwareEnabledNodes);
    void AssignGlobalZOrderAndCreateLayer(std::vector<std::shared_ptr<RSSurfaceRenderNode>>& nodesInZOrder);
    void RotateMirrorCanvasIfNeed(RSDisplayRenderNode& node, bool canvasRotation = false);
    void ScaleMirrorIfNeed(RSDisplayRenderNode& node, bool canvasRotation = false);
    void RotateMirrorCanvasIfNeedForWiredScreen(RSDisplayRenderNode& node);
    void ScaleMirrorIfNeedForWiredScreen(RSDisplayRenderNode& node, bool canvasRotation = false);

    bool GetIsPartialRenderEnabled() const
    {
        return isPartialRenderEnabled_;
    }
    bool GetIsOpDropped() const
    {
        return isOpDropped_;
    }
    // Use in vulkan parallel rendering
    GraphicColorGamut GetColorGamut() const
    {
        return newColorSpace_;
    }

    std::shared_ptr<RSProcessor> GetProcessor() const
    {
        return processor_;
    }

    void SetRenderFrame(std::unique_ptr<RSRenderFrame> renderFrame)
    {
        renderFrame_ = std::move(renderFrame);
    }
    void SetAppWindowNum(uint32_t num);

    void SetForceUpdateFlag(bool flag)
    {
        forceUpdateFlag_ = flag;
    }

    void SetScreenInfo(ScreenInfo screenInfo)
    {
        screenInfo_ = screenInfo;
    }

    void UpdateOccludedStatusWithFilterNode(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const;

    static void ClearRenderGroupCache();

    using RenderParam = std::tuple<std::shared_ptr<RSRenderNode>, RSPaintFilterCanvas::CanvasStatus>;
private:
    void PartialRenderOptionInit();
    RSVisibleLevel GetRegionVisibleLevel(const Occlusion::Region& visibleRegion,
        const Occlusion::Region& selfDrawRegion);
    void SurfaceOcclusionCallbackToWMS();
    void CalcChildFilterNodeDirtyRegion(std::shared_ptr<RSSurfaceRenderNode>& currentSurfaceNode,
        std::shared_ptr<RSDisplayRenderNode>& displayNode);
    void CalcSurfaceFilterNodeDirtyRegion(std::shared_ptr<RSSurfaceRenderNode>& currentSurfaceNode,
        std::shared_ptr<RSDisplayRenderNode>& displayNode);
    void DrawWatermarkIfNeed(RSDisplayRenderNode& node, bool isMirror = false);
    enum class RSPaintStyle {
        FILL,
        STROKE
    };
    void DrawDirtyRectForDFX(const RectI& dirtyRect, const Drawing::Color color,
        const RSPaintStyle fillType, float alpha, int edgeWidth, std::string extra = "");
    void DrawDirtyRegionForDFX(std::vector<RectI> dirtyRects);
    void DrawCacheRegionForDFX(std::map<NodeId, RectI>& cacheRects);
    void DrawHwcRegionForDFX(std::vector<std::shared_ptr<RSSurfaceRenderNode>>& hwcNodes);
#ifdef DDGR_ENABLE_FEATURE_OPINC
    void DrawAutoCacheRegionForDFX(std::vector<RectI, std::string> cacheRegionInfo);
#endif
    void DrawAllSurfaceDirtyRegionForDFX(RSDisplayRenderNode& node, const Occlusion::Region& region);
    void DrawTargetSurfaceDirtyRegionForDFX(RSDisplayRenderNode& node);
    void DrawAllSurfaceOpaqueRegionForDFX(RSDisplayRenderNode& node);
    void DrawSurfaceOpaqueRegionForDFX(RSSurfaceRenderNode& node);
    void DrawTargetSurfaceVisibleRegionForDFX(RSDisplayRenderNode& node);
    void DrawCurrentRefreshRate(uint32_t currentRefreshRate, uint32_t realtimeRefreshRate, RSDisplayRenderNode& node);
    // check if surface name is in dfx target list
    inline bool CheckIfSurfaceTargetedForDFX(std::string nodeName)
    {
        return (std::find(dfxTargetSurfaceNames_.begin(), dfxTargetSurfaceNames_.end(),
            nodeName) != dfxTargetSurfaceNames_.end());
    }

    bool DrawDetailedTypesOfDirtyRegionForDFX(RSSurfaceRenderNode& node);
    void DrawAndTraceSingleDirtyRegionTypeForDFX(RSSurfaceRenderNode& node,
        DirtyRegionType dirtyType, bool isDrawn = true);

    bool InitDisplayInfo(RSDisplayRenderNode& node);

    bool BeforeUpdateSurfaceDirtyCalc(RSSurfaceRenderNode& node);
    bool NeedPrepareChindrenInReverseOrder(RSRenderNode& node) const;
    bool IsLeashAndHasMainSubNode(RSRenderNode& node) const;
    bool AfterUpdateSurfaceDirtyCalc(RSSurfaceRenderNode& node);
    void UpdateSurfaceRenderNodeRotate(RSSurfaceRenderNode& node);
    void UpdateSurfaceDirtyAndGlobalDirty();
    void ResetDisplayDirtyRegionForColorFilterSwitch();
    void CollectFilterInfoAndUpdateDirty(RSRenderNode& node);
    RectI GetVisibleEffectDirty(RSRenderNode& node) const;

    void UpdateHwcNodeEnableByGlobalFilter(std::shared_ptr<RSSurfaceRenderNode>& node);
    void UpdateHwcNodeEnableByFilterRect(std::shared_ptr<RSSurfaceRenderNode>& node, const RectI& filterRect);
    void UpdateHwcNodeEnableByBackgroundAlpha(RSSurfaceRenderNode& node);
    void UpdateHwcNodeEnableBySrcRect(RSSurfaceRenderNode& node);
    void UpdateHwcNodeInfoForAppNode(RSSurfaceRenderNode& node);
    void UpdateSrcRect(RSSurfaceRenderNode& node,
        const Drawing::Matrix& absMatrix, const RectI& clipRect);
    void UpdateDstRect(RSSurfaceRenderNode& node, const RectI& absRect, const RectI& clipRect);
    void UpdateHwcNodeByTransform(RSSurfaceRenderNode& node);
    void UpdateHwcNodeEnableByRotateAndAlpha(std::shared_ptr<RSSurfaceRenderNode>& node);
    void UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(std::vector<RectI>& hwcRects,
        std::shared_ptr<RSSurfaceRenderNode>& hwcNode);
    void UpdateHwcNodeDirtyRegionAndCreateLayer(std::shared_ptr<RSSurfaceRenderNode>& node);
    void UpdateHwcNodeEnable();
    void PrevalidateHwcNode();
    void AccumulateMatrixAndAlpha(std::shared_ptr<RSSurfaceRenderNode>& node, Drawing::Matrix& matrix, float& alpha);

    void UpdateHwcNodeDirtyRegionForApp(std::shared_ptr<RSSurfaceRenderNode>& appNode,
        std::shared_ptr<RSSurfaceRenderNode>& hwcNode);

    void UpdatePrepareClip(RSRenderNode& node);

    void CheckMergeSurfaceDirtysForDisplay(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const;
    void CheckMergeTransparentDirtysForDisplay(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const;

    void CheckMergeTransparentFilterForDisplay(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
        Occlusion::Region& accumulatedDirtyRegion);
    // If reusable filter cache covers whole screen, mark lower layer to skip process
    void CheckAndUpdateFilterCacheOcclusion(std::vector<RSBaseRenderNode::SharedPtr>& curMainAndLeashSurfaces) const;
    void CheckMergeGlobalFilterForDisplay(Occlusion::Region& accumulatedDirtyRegion);

    bool IsNotDirtyHardwareEnabledTopSurface(std::shared_ptr<RSSurfaceRenderNode>& node) const;
    void ClipRegion(std::shared_ptr<Drawing::Canvas> canvas, const Drawing::Region& region) const;
    /* calculate display/global (between windows) level dirty region, current include:
     * 1. window move/add/remove 2. transparent dirty region
     * when process canvas culling, canvas intersect with surface's visibledirty region or
     * global dirty region will be skipped
     */
    void CalcDirtyDisplayRegion(std::shared_ptr<RSDisplayRenderNode>& node);
    void CalcDirtyRegionForFilterNode(const RectI& filterRect,
        std::shared_ptr<RSSurfaceRenderNode>& currentSurfaceNode,
        std::shared_ptr<RSDisplayRenderNode>& displayNode);

    // remove functions below when dirty region is enabled for foldable device
    void UpdateHardwareNodeStatusBasedOnFilterRegion(RSDisplayRenderNode& displayNode);
    void UpdateHardwareNodeStatusBasedOnFilter(std::shared_ptr<RSSurfaceRenderNode>& node,
        std::vector<std::shared_ptr<RSSurfaceRenderNode>>& prevHwcEnabledNodes);
    void UpdateHardwareChildNodeStatus(std::shared_ptr<RSSurfaceRenderNode>& node,
        std::vector<SurfaceDirtyMgrPair>& curHwcEnabledNodes);
 
    void UpdateHardwareEnableList(std::vector<RectI>& filterRects,
        std::vector<std::shared_ptr<RSSurfaceRenderNode>>& validHwcNodes);
    // remove functions above when dirty region is enabled for foldable device

    void CalcDirtyFilterRegion(std::shared_ptr<RSDisplayRenderNode>& node);
    /* Disable visible hwc surface if it intersects with filter region
     * Save rest validNodes in prevHwcEnabledNodes
     * [planning] Update hwc surface dirty status at the same time
     */
    void UpdateHardwareNodeStatusBasedOnFilter(std::shared_ptr<RSSurfaceRenderNode>& node,
        std::vector<SurfaceDirtyMgrPair>& prevHwcEnabledNodes,
        std::shared_ptr<RSDirtyRegionManager>& displayDirtyManager);
    /* Disable hwc surface intersect with filter rects and merge dirty filter region
     * [planning] If invisible filterRects could be removed
     */
    RectI UpdateHardwareEnableList(std::vector<RectI>& filterRects,
        std::vector<SurfaceDirtyMgrPair>& validHwcNodes);
    void MergeDirtyRectIfNeed(std::shared_ptr<RSSurfaceRenderNode> appNode,
        std::shared_ptr<RSSurfaceRenderNode> hwcNode);
    void AddContainerDirtyToGlobalDirty(std::shared_ptr<RSDisplayRenderNode>& node) const;
    // merge last childRect as dirty if any child has been removed
    void MergeRemovedChildDirtyRegion(RSRenderNode& node, bool needMap = false);
    // Reset curSurface info as upper surfaceParent in case surfaceParent has multi children
    void ResetCurSurfaceInfoAsUpperSurfaceParent(RSSurfaceRenderNode& node);

    // set global dirty region to each surface node
    void SetSurfaceGlobalDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node);
    void SetSurfaceGlobalAlignedDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node,
        const Occlusion::Region alignedDirtyRegion);
    void AlignGlobalAndSurfaceDirtyRegions(std::shared_ptr<RSDisplayRenderNode>& node);

    void CheckAndSetNodeCacheType(RSRenderNode& node);
    bool UpdateCacheSurface(RSRenderNode& node);
    void DrawSpherize(RSRenderNode& node);
    void DrawChildRenderNode(RSRenderNode& node);
    void DrawChildCanvasRenderNode(RSRenderNode& node);

    void CheckColorSpace(RSSurfaceRenderNode& node);
    void HandleColorGamuts(RSDisplayRenderNode& node, const sptr<RSScreenManager>& screenManager);
    void CheckPixelFormat(RSSurfaceRenderNode& node);
    void HandlePixelFormat(RSDisplayRenderNode& node, const sptr<RSScreenManager>& screenManager);
    void AddOverDrawListener(std::unique_ptr<RSRenderFrame>& renderFrame,
        std::shared_ptr<RSCanvasListener>& overdrawListener);
    /* Judge if surface render node could skip preparation:
     * 1. not leash window
     * 2. parent not dirty
     * 3. no processWithCommands_ of node's corresponding pid
     * If so, reset status flag and stop traversal
     */
    bool CheckIfSurfaceRenderNodeStatic(RSSurfaceRenderNode& node);
    /* Judge if uifirst surface render node could skip subtree preparation:
     * mainwindow check if it has leashwindow parent
     * If so, check parent or check itself
     */
    bool CheckIfUIFirstSurfaceContentReusable(std::shared_ptr<RSSurfaceRenderNode>& node, bool& isAssigned);
    // currently classify surface assigned subthread specific dirty case for preparation
    void ClassifyUIFirstSurfaceDirtyStatus(RSSurfaceRenderNode& node);

    void PrepareTypesOfSurfaceRenderNodeBeforeUpdate(RSSurfaceRenderNode& node);
    void PrepareTypesOfSurfaceRenderNodeAfterUpdate(RSSurfaceRenderNode& node);
    // judge if node's cache changes
    // return false if cache static and simplify its subtree traversal
    bool UpdateCacheChangeStatus(RSRenderNode& node);
    bool IsDrawingCacheStatic(RSRenderNode& node);
    // if cache root reuses, update its subtree
    // [attention] check curSurfaceDirtyManager_ before function calls
    void UpdateSubTreeInCache(const std::shared_ptr<RSRenderNode>& cacheRootNode,
        const std::vector<RSRenderNode::SharedPtr>& children);
    // set node cacheable animation after checking whold child tree
    void SetNodeCacheChangeStatus(RSRenderNode& node);
    void DisableNodeCacheInSetting(RSRenderNode& node);
    // update rendernode's cache status and collect valid cache rect
    void UpdateForegroundFilterCacheWithDirty(RSRenderNode& node,
        RSDirtyRegionManager& dirtyManager, bool isForeground = true);

    bool IsHardwareComposerEnabled();

    // choose to keep filter cache if node is filter occluded
    bool CheckIfSurfaceRenderNodeNeedProcess(RSSurfaceRenderNode& node, bool& keepFilterCache);

    void ClearTransparentBeforeSaveLayer();
    // mark surfaceNode's child surfaceView nodes hardware forced disabled
    void MarkSubHardwareEnableNodeState(RSSurfaceRenderNode& surfaceNode);
    void CollectAppNodeForHwc(std::shared_ptr<RSSurfaceRenderNode> surfaceNode);
    void UpdateSecurityAndSkipLayerRecord(RSSurfaceRenderNode& node);
    void PrepareEffectNodeIfCacheReuse(const std::shared_ptr<RSRenderNode>& cacheRootNode,
        std::shared_ptr<RSEffectRenderNode> effectNode);

    // offscreen render related
    void PrepareOffscreenRender(RSRenderNode& node);
    void FinishOffscreenRender(bool isMirror = false);
    // close partialrender when perform window animation
    void ClosePartialRenderWhenAnimatingWindows(std::shared_ptr<RSDisplayRenderNode>& node);
    bool DrawBlurInCache(RSRenderNode& node);
    void UpdateCacheRenderNodeMapWithBlur(RSRenderNode& node);
    bool IsFirstVisitedCacheForced() const;
    bool IsRosenWebHardwareDisabled(RSSurfaceRenderNode& node, int rotation) const;
    bool ForceHardwareComposer(RSSurfaceRenderNode& node) const;
    bool UpdateSrcRectForHwcNode(RSSurfaceRenderNode& node); // return if srcRect is allowed by dss restriction
    std::shared_ptr<Drawing::Image> GetCacheImageFromMirrorNode(std::shared_ptr<RSDisplayRenderNode> mirrorNode);

    void SwitchColorFilterDrawing(int currentSaveCount);
    void ProcessShadowFirst(RSRenderNode& node, bool inSubThread);
    void SaveCurSurface(std::shared_ptr<RSDirtyRegionManager> dirtyManager,
        std::shared_ptr<RSSurfaceRenderNode> surfaceNode);
    void RestoreCurSurface(std::shared_ptr<RSDirtyRegionManager> &dirtyManager,
        std::shared_ptr<RSSurfaceRenderNode> &surfaceNode);
    void PrepareSubSurfaceNodes(RSSurfaceRenderNode& node);
    void ProcessSubSurfaceNodes(RSSurfaceRenderNode& node);

    // used to catch overdraw
    void StartOverDraw();
    void FinishOverDraw();

    void SendRcdMessage(RSDisplayRenderNode& node);

    bool ForcePrepareSubTree()
    {
        return curSurfaceNode_ && curSurfaceNode_->GetNeedCollectHwcNode();
    }
    void UpdateRotationStatusForEffectNode(RSEffectRenderNode& node);
    void CheckFilterNodeInSkippedSubTreeNeedClearCache(const RSRenderNode& node);

    std::shared_ptr<Drawing::Surface> offscreenSurface_;                 // temporary holds offscreen surface
    std::shared_ptr<RSPaintFilterCanvas> canvasBackup_; // backup current canvas before offscreen render

    // Use in vulkan parallel rendering
    bool IsOutOfScreenRegion(RectI rect);

    // used to catch overdraw
    std::shared_ptr<Drawing::Surface> overdrawSurface_ = nullptr;
    std::shared_ptr<Drawing::OverDrawCanvas> overdrawCanvas_ = nullptr;

    sptr<RSScreenManager> screenManager_;
    ScreenInfo screenInfo_;
    RectI screenRect_;
    std::shared_ptr<RSDirtyRegionManager> curSurfaceDirtyManager_;
    std::shared_ptr<RSSurfaceRenderNode> curSurfaceNode_;
    std::stack<std::shared_ptr<RSDirtyRegionManager>> surfaceDirtyManager_;
    std::stack<std::shared_ptr<RSSurfaceRenderNode>> surfaceNode_;
    float curAlpha_ = 1.f;
    Vector4f curCornerRadius_{ 0.f, 0.f, 0.f, 0.f };
    bool dirtyFlag_ { false };
    std::unique_ptr<RSRenderFrame> renderFrame_;
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
    Drawing::Region clipRegion_;
    std::map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> dirtySurfaceNodeMap_;
    std::map<NodeId, RectI> cacheRenderNodeMapRects_;
    std::map<NodeId, bool> cacheRenderNodeIsUpdateMap_;
    Drawing::Rect boundsRect_ {};
    Gravity frameGravity_ = Gravity::DEFAULT;

    int32_t offsetX_ { 0 };
    int32_t offsetY_ { 0 };
    std::shared_ptr<RSProcessor> processor_;
    Drawing::Matrix parentSurfaceNodeMatrix_;

    ScreenId currentVisitDisplay_ = INVALID_SCREEN_ID;
    std::map<ScreenId, bool> displayHasSecSurface_;
    std::map<ScreenId, bool> displayHasSkipSurface_;
    std::map<ScreenId, bool> hasCaptureWindow_;
    std::set<ScreenId> mirroredDisplays_;
    bool isSecurityDisplay_ = false;

    bool hasFingerprint_ = false;
    bool mirrorAutoRotate_ = false;

    std::shared_ptr<RSBaseRenderEngine> renderEngine_;

    std::shared_ptr<RSDirtyRegionManager> curDisplayDirtyManager_;
    std::shared_ptr<RSDisplayRenderNode> curDisplayNode_;
    bool doAnimate_ = false;
    bool isSurfaceRotationChanged_ = false;
    bool isPartialRenderEnabled_ = false;
    bool isOpDropped_ = false;
    bool isDirtyRegionDfxEnabled_ = false; // dirtyRegion DFX visualization
    bool isTargetDirtyRegionDfxEnabled_ = false;
    bool isOpaqueRegionDfxEnabled_ = false;
    bool isVisibleRegionDfxEnabled_ = false;
    bool isDisplayDirtyDfxEnabled_ = false;
    bool isCanvasNodeSkipDfxEnabled_ = false;
    bool isQuickSkipPreparationEnabled_ = false;
    bool isOcclusionEnabled_ = false;
    bool isSkipCanvasNodeOutOfScreen_ = false;
    bool isScreenRotationAnimating_ = false;
    std::vector<std::string> dfxTargetSurfaceNames_;
    PartialRenderType partialRenderType_;
    QuickSkipPrepareType quickSkipPrepareType_;
    DirtyRegionDebugType dirtyRegionDebugType_;
    SurfaceRegionDebugType surfaceRegionDebugType_;
    bool isRegionDebugEnabled_ = false;
    bool isDirty_ = false;
    // added for judge if drawing cache changes
    bool isDrawingCacheEnabled_ = false;
    std::stack<bool> isDrawingCacheChanged_ = {};
    std::vector<RectI> accumulatedDirtyRegions_ = {};
    bool isSubSurfaceEnabled_ = false;
#ifdef DDGR_ENABLE_FEATURE_OPINC
    bool autoCacheEnable_ = false;
    bool autoCacheDrawingEnable_ = false;
    RSRenderNode::RSAutoCache::NodeStragyType nodeCacheType_ = RSRenderNode::RSAutoCache::CACHE_NONE;
    bool unchangeMark_ = false;
    bool unchangeMarkEnable_ = false;
    bool isDiscardSurface_ = true;
    std::vector<std::pair<RectI, std::string>> autoCacheRenderNodeInfos_;
    bool isOpincDropNodeExt_ = true;
#endif

    bool needFilter_ = false;
    GraphicColorGamut newColorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    GraphicPixelFormat newPixelFormat_ = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    std::vector<ScreenColorGamut> colorGamutModes_;
    uint64_t currentFocusedNodeId_ = 0;
    uint64_t focusedLeashWindowId_ = 0;

    bool isSubThread_ = false;
    bool isUIFirst_ = false;
    bool isUIFirstDebugEnable_ = false;
    bool hasSelfDraw_ = false;
    bool ancestorNodeHasAnimation_ = false;
    uint32_t threadIndex_ = UNI_MAIN_THREAD_INDEX;
    // check each surface could be reused per frame
    // currently available to uiFirst
    bool isCachedSurfaceReuse_ = false;
    bool isSurfaceDirtyNodeLimited_ = false;

    bool isDirtyRegionAlignedEnable_ = false;
    bool isPrevalidateHwcNodeEnable_ = false;

    RectI prepareClipRect_{0, 0, 0, 0}; // renderNode clip rect used in Prepare

    // count prepared and processed canvasnode numbers per app
    // unirender visitor resets every frame, no overflow risk here
    unsigned int preparedCanvasNodeInCurrentSurface_ = 0;
    unsigned int processedCanvasNodeInCurrentSurface_ = 0;
    unsigned int processedPureContainerNode_ = 0;

    float globalZOrder_ = 0.0f;
    bool isUpdateCachedSurface_ = false;
    bool isHardwareForcedDisabled_ = false; // indicates if hardware composer is totally disabled
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes_;
    // vector of all app window nodes with surfaceView, sorted by zOrder
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> appWindowNodesInZOrder_;
    // vector of hardwareEnabled nodes above displayNodeSurface like pointer window
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledTopNodes_;
    // vector of Appwindow nodes ids not contain subAppWindow nodes ids in current frame
    std::queue<NodeId> curMainAndLeashWindowNodesIds_;
    // vector of current frame mainwindow surface visible info
    VisibleData dstCurVisVec_;
    std::mutex occlusionMutex_;
    float localZOrder_ = 0.0f; // local zOrder for surfaceView under same app window node

    std::unique_ptr<RcdInfo> rcdInfo_ = nullptr;
    static void ProcessUnpairedSharedTransitionNode();
    std::stack<RenderParam> curGroupedNodes_;

    // adapt to sceneboard, mark if the canvasNode within the scope of surfaceNode
    bool isSubNodeOfSurfaceInPrepare_ = false;
    bool isSubNodeOfSurfaceInProcess_ = false;

    uint32_t appWindowNum_ = 0;

    // displayNodeMatrix only used in offScreen render case to ensure correct composer layer info when with rotation,
    // displayNodeMatrix indicates display node's matrix info
    std::optional<Drawing::Matrix> displayNodeMatrix_;
    mutable std::mutex copyVisitorInfosMutex_;
    bool resetRotate_ = false;
    std::optional<Drawing::RectI> effectRegion_ = std::nullopt;
    // variable for occlusion
    bool needRecalculateOcclusion_ = false;
    Occlusion::Region accumulatedOcclusionRegion_;

    bool curDirty_ = false;
    bool curContentDirty_ = false;
    bool isPhone_ = false;
    bool isPc_ = false;
    bool isCacheBlurPartialRenderEnabled_ = false;
    bool drawCacheWithBlur_ = false;
    bool notRunCheckAndSetNodeCacheType_ = false;
    bool noNeedTodrawShadowAgain_ = false;
    int updateCacheProcessCnt_ = 0;
    std::vector<std::string> windowsName_;

    NodeId firstVisitedCache_ = INVALID_NODEID;
    std::unordered_set<NodeId> visitedCacheNodeIds_ = {};
    std::unordered_map<NodeId, std::unordered_set<NodeId>> allCacheFilterRects_ = {};
    std::stack<std::unordered_set<NodeId>> curCacheFilterRects_ = {};

    // record nodes in surface which has filter may influence golbalDirty
    OcclusionRectISet globalFilter_;
    // record container nodes which need filter
    OcclusionRectISet containerFilter_;
    // record nodes which has transparent clean filter
    std::unordered_map<NodeId, std::vector<std::pair<NodeId, RectI>>> transparentCleanFilter_;
    // record nodes which has transparent dirty filter
    std::unordered_map<NodeId, std::vector<std::pair<NodeId, RectI>>> transparentDirtyFilter_;

    std::vector<RectI> globalFilterRects_;
    // visible filter in transparent surface or display must prepare
    bool filterInGlobal_ = true;

    bool forceUpdateFlag_ = false;
#ifdef ENABLE_RECORDING_DCL
    void tryCapture(float width, float height);
    void endCapture() const;
    std::shared_ptr<ExtendRecordingCanvas> recordingCanvas_;
#endif
#ifdef RS_PROFILER_ENABLED
    RSCaptureRecorder captureRecorder_;
#endif
    bool isNodeSingleFrameComposer_ = false;
    // use for screen recording optimization
    std::shared_ptr<Drawing::Image> cacheImgForCapture_ = nullptr;
    std::shared_ptr<Drawing::Image> offScreenCacheImgForCapture_ = nullptr;

    void SetHasSharedTransitionNode(RSSurfaceRenderNode& surfaceNode, bool hasSharedTransitionNode);

    // attention: please synchronize the change of RSUniRenderVisitor::ProcessChildren to this func
    void ProcessChildrenForScreenRecordingOptimization(RSDisplayRenderNode& node, NodeId rootIdOfCaptureWindow);
    NodeId FindInstanceChildOfDisplay(std::shared_ptr<RSRenderNode> node);
    bool CheckIfNeedResetRotate();

    // use for virtual screen app/window filtering ability
    NodeId virtualScreenFilterAppRootId_ = INVALID_NODEID;
    void UpdateVirtualScreenFilterAppRootId(const RSRenderNode::SharedPtr& node);

    void UpdateSurfaceRenderNodeScale(RSSurfaceRenderNode& node);

    // dfx for effect render node
    void DrawEffectRenderNodeForDFX();
    std::vector<RectI> nodesUseEffectFallbackForDfx_;
    std::vector<RectI> nodesUseEffectForDfx_;
    // pair<ApplyBackgroundEffectNodeList, ApplyBackgroundEffectFallbackNodeList>
    std::unordered_map<NodeId, std::pair<std::vector<RectI>, std::vector<RectI>>> effectNodeMapForDfx_;

    // use for curtain screen
    void DrawCurtainScreen();
    bool isCurtainScreenOn_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_VISITOR_H
