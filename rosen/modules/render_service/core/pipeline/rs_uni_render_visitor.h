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
#include <parameters.h>
#include <set>

#include "rs_base_render_engine.h"
#include "system/rs_system_parameters.h"

#include "params/rs_render_thread_params.h"
#include "pipeline/round_corner_display/rs_rcd_render_manager.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_uni_hwc_prevalidate_util.h"
#include "platform/ohos/overdraw/rs_overdraw_controller.h"
#include "screen_manager/rs_screen_manager.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;
class RSUniRenderVisitor : public RSNodeVisitor {
public:
    using SurfaceDirtyMgrPair = std::pair<std::shared_ptr<RSSurfaceRenderNode>, std::shared_ptr<RSSurfaceRenderNode>>;
    RSUniRenderVisitor();
    explicit RSUniRenderVisitor(const RSUniRenderVisitor& visitor);
    ~RSUniRenderVisitor() override;

    // To prepare nodes between displayRenderNode and app nodes.
    void QuickPrepareEffectRenderNode(RSEffectRenderNode& node) override;
    void QuickPrepareCanvasRenderNode(RSCanvasRenderNode& node) override;
    void QuickPrepareDisplayRenderNode(RSDisplayRenderNode& node) override;
    void QuickPrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override;
    void QuickPrepareChildren(RSRenderNode& node) override;

    void PrepareChildren(RSRenderNode& node) override {};
    void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override {};
    void PrepareDisplayRenderNode(RSDisplayRenderNode& node) override {};
    void PrepareProxyRenderNode(RSProxyRenderNode& node) override {};
    // prepareroot also used for quickprepare
    void PrepareRootRenderNode(RSRootRenderNode& node) override;
    void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override {};
    void PrepareEffectRenderNode(RSEffectRenderNode& node) override {};

    void ProcessChildren(RSRenderNode& node) override {};
    void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override {};
    void ProcessDisplayRenderNode(RSDisplayRenderNode& node) override {};
    void ProcessProxyRenderNode(RSProxyRenderNode& node) override {};
    void ProcessRootRenderNode(RSRootRenderNode& node) override {};
    void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override {};
    void ProcessEffectRenderNode(RSEffectRenderNode& node) override {};

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

    bool GetIsPartialRenderEnabled() const
    {
        return isPartialRenderEnabled_;
    }
    bool GetIsOpDropped() const
    {
        return isOpDropped_;
    }
    bool GetIsRegionDebugEnabled() const
    {
        return isRegionDebugEnabled_;
    }
    // Use in vulkan parallel rendering
    GraphicColorGamut GetColorGamut() const
    {
        return newColorSpace_;
    }

    void SetAppWindowNum(uint32_t num);

    void SetScreenInfo(ScreenInfo screenInfo)
    {
        screenInfo_ = screenInfo;
    }

    // Use in updating hwcnode hardware state with background alpha
    void UpdateHardwareStateByHwcNodeBackgroundAlpha(const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& hwcNodes);

    bool IsNodeAboveInsideOfNodeBelow(const RectI& rectAbove, std::list<RectI>& hwcNodeRectList);
    // Use end

    void SurfaceOcclusionCallbackToWMS();

    static void ClearRenderGroupCache();

    using RenderParam = std::tuple<std::shared_ptr<RSRenderNode>, RSPaintFilterCanvas::CanvasStatus>;
private:
    /* Prepare relevant calculation */
    // considering occlusion info for app surface as well as widget
    bool IsSubTreeOccluded(RSRenderNode& node) const;
    // restore node's flag and filter dirty collection
    void PostPrepare(RSRenderNode& node, bool subTreeSkipped = false);
    void UpdateNodeVisibleRegion(RSSurfaceRenderNode& node);
    void CalculateOcclusion(RSSurfaceRenderNode& node);

    void CheckFilterCacheNeedForceClearOrSave(RSRenderNode& node);
    void UpdateOccludedStatusWithFilterNode(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const;
    void PartialRenderOptionInit();
    RSVisibleLevel GetRegionVisibleLevel(const Occlusion::Region& visibleRegion,
        const Occlusion::Region& selfDrawRegion);
    void UpdateSurfaceOcclusionInfo();
    enum class RSPaintStyle {
        FILL,
        STROKE
    };

    bool InitDisplayInfo(RSDisplayRenderNode& node);

    bool BeforeUpdateSurfaceDirtyCalc(RSSurfaceRenderNode& node);
    bool NeedPrepareChindrenInReverseOrder(RSRenderNode& node) const;
    bool IsLeashAndHasMainSubNode(RSRenderNode& node) const;
    bool AfterUpdateSurfaceDirtyCalc(RSSurfaceRenderNode& node);
    void UpdateLeashWindowVisibleRegionEmpty(RSSurfaceRenderNode& node);
    void UpdateSurfaceRenderNodeRotate(RSSurfaceRenderNode& node);
    void UpdateSurfaceDirtyAndGlobalDirty();
    // should ensure that the surface size of dirty region manager has been set
    void ResetDisplayDirtyRegion();
    bool CheckScreenPowerChange() const;
    bool CheckColorFilterChange() const;
    bool CheckCurtainScreenUsingStatusChange() const;
    bool CheckLuminanceStatusChange();
    bool IsFirstFrameOfPartialRender() const;
    bool IsFirstFrameOfOverdrawSwitch() const;
    bool IsFirstFrameOfDrawingCacheDfxSwitch() const;
    bool IsWatermarkFlagChanged() const;
    void UpdateDisplayZoomState();
    void CollectFilterInfoAndUpdateDirty(RSRenderNode& node,
        RSDirtyRegionManager& dirtyManager, const RectI& globalFilterRect);
    RectI GetVisibleEffectDirty(RSRenderNode& node) const;

    void UpdateHwcNodeEnableByGlobalFilter(std::shared_ptr<RSSurfaceRenderNode>& node);
    void UpdateHwcNodeEnableByGlobalCleanFilter(const std::vector<std::pair<NodeId, RectI>>& cleanFilter,
        RSSurfaceRenderNode& hwcNodePtr);
    void UpdateHwcNodeEnableByFilterRect(
        std::shared_ptr<RSSurfaceRenderNode>& node, const RectI& filterRect, bool isReverseOrder = false);
    void CalcHwcNodeEnableByFilterRect(
        std::shared_ptr<RSSurfaceRenderNode>& node, const RectI& filterRect, bool isReverseOrder = false);
    // This function is used for solving display problems caused by dirty blurfilter node half-obscured.
    void UpdateDisplayDirtyAndExtendVisibleRegion();
    // This function is used to update global dirty and visibleRegion
    // by processing dirty blurfilter node obscured.
    void ProcessFilterNodeObscured(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
        Occlusion::Region& extendRegion, const RSRenderNodeMap& nodeMap);
    void UpdateHwcNodeEnableByBackgroundAlpha(RSSurfaceRenderNode& node);
    void UpdateHwcNodeEnableBySrcRect(RSSurfaceRenderNode& node);
    void UpdateHwcNodeEnableByBufferSize(RSSurfaceRenderNode& node);
    void UpdateHwcNodeInfoForAppNode(RSSurfaceRenderNode& node);
    void UpdateSrcRect(RSSurfaceRenderNode& node,
        const Drawing::Matrix& absMatrix, const RectI& clipRect);
    void UpdateDstRect(RSSurfaceRenderNode& node, const RectI& absRect, const RectI& clipRect);
    void UpdateHwcNodeProperty(std::shared_ptr<RSSurfaceRenderNode> hwcNode);
    void UpdateHwcNodeByTransform(RSSurfaceRenderNode& node);
    void UpdateHwcNodeEnableByRotateAndAlpha(std::shared_ptr<RSSurfaceRenderNode>& node);
    void ProcessAncoNode(std::shared_ptr<RSSurfaceRenderNode>& hwcNodePtr);
    void InitAncoStatus();
    void UpdateAncoNodeHWCDisabledState();
    void UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(std::vector<RectI>& hwcRects,
        std::shared_ptr<RSSurfaceRenderNode>& hwcNode);
    void UpdateChildHwcNodeEnableByHwcNodeBelow(std::vector<RectI>& hwcRects,
        std::shared_ptr<RSSurfaceRenderNode>& appNode);
    void UpdateHwcNodeEnableByHwcNodeBelowSelf(std::vector<RectI>& hwcRects,
        std::shared_ptr<RSSurfaceRenderNode>& hwcNode, bool isIntersectWithRoundCorner);
    void UpdateHwcNodeDirtyRegionAndCreateLayer(std::shared_ptr<RSSurfaceRenderNode>& node);
    void AllSurfacesDrawnInUniRender(const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& hwcNodes);
    void UpdatePointWindowDirtyStatus(std::shared_ptr<RSSurfaceRenderNode>& pointWindow);
    void UpdateTopLayersDirtyStatus(const std::vector<std::shared_ptr<RSSurfaceRenderNode>>& topLayers);
    void UpdateCornerRadiusInfoForDRM(std::shared_ptr<RSSurfaceRenderNode> hwcNode, std::vector<RectI>& hwcRects);
    bool CheckIfRoundCornerIntersectDRM(const float& ratio, std::vector<float>& ratioVector,
        const Vector4f& instanceCornerRadius, const RectI& instanceAbsRect, const RectI& hwcAbsRect);
    void UpdateHwcNodeEnable();
    void UpdateHwcNodeEnableByNodeBelow();
    void PrevalidateHwcNode();

    // use in QuickPrepareSurfaceRenderNode, update SurfaceRenderNode's uiFirst status
    void PrepareForUIFirstNode(RSSurfaceRenderNode& node);

    void UpdateHwcNodeDirtyRegionForApp(std::shared_ptr<RSSurfaceRenderNode>& appNode,
        std::shared_ptr<RSSurfaceRenderNode>& hwcNode);

    void UpdatePrepareClip(RSRenderNode& node);

    void CheckMergeDisplayDirtyByTransparent(RSSurfaceRenderNode& surfaceNode) const;
    void CheckMergeDisplayDirtyByZorderChanged(RSSurfaceRenderNode& surfaceNode) const;
    void CheckMergeDisplayDirtyByPosChanged(RSSurfaceRenderNode& surfaceNode) const;
    void CheckMergeDisplayDirtyByShadowChanged(RSSurfaceRenderNode& surfaceNode) const;
    void CheckMergeDisplayDirtyBySurfaceChanged() const;
    void CheckMergeDisplayDirtyByAttractionChanged(RSSurfaceRenderNode& surfaceNode) const;
    void CheckMergeSurfaceDirtysForDisplay(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const;
    void CheckMergeDisplayDirtyByTransparentRegions(RSSurfaceRenderNode& surfaceNode) const;
    void CheckMergeFilterDirtyByIntersectWithDirty(OcclusionRectISet& filterSet, bool isGlobalDirty);

    void CheckMergeDisplayDirtyByTransparentFilter(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
        Occlusion::Region& accumulatedDirtyRegion);
    void CheckMergeGlobalFilterForDisplay(Occlusion::Region& accumulatedDirtyRegion);
    void CheckMergeDebugRectforRefreshRate(std::vector<RSBaseRenderNode::SharedPtr>& surfaces);

    // merge last childRect as dirty if any child has been removed
    void MergeRemovedChildDirtyRegion(RSRenderNode& node, bool needMap = false);
    // Reset curSurface info as upper surfaceParent in case surfaceParent has multi children
    void ResetCurSurfaceInfoAsUpperSurfaceParent(RSSurfaceRenderNode& node);

    void CheckColorSpace(RSSurfaceRenderNode& node);
    void CheckColorSpaceWithSelfDrawingNode(RSSurfaceRenderNode& node);
    void UpdateColorSpaceAfterHwcCalc(RSDisplayRenderNode& node);
    void HandleColorGamuts(RSDisplayRenderNode& node, const sptr<RSScreenManager>& screenManager);
    void CheckPixelFormat(RSSurfaceRenderNode& node);
    void HandlePixelFormat(RSDisplayRenderNode& node, const sptr<RSScreenManager>& screenManager);
    bool IsHardwareComposerEnabled();

    void UpdateSecuritySkipAndProtectedLayersRecord(RSSurfaceRenderNode& node);

    void SendRcdMessage(RSDisplayRenderNode& node);

    bool ForcePrepareSubTree()
    {
        return curSurfaceNode_ && curSurfaceNode_->GetNeedCollectHwcNode();
    }

    inline bool IsValidInVirtualScreen(const RSSurfaceRenderNode& node) const
    {
        if (node.GetSkipLayer() || node.GetSecurityLayer() ||
            node.GetProtectedLayer() || node.GetSnapshotSkipLayer()) {
            return false; // surface is special layer
        }
        if (!allWhiteList_.empty() || allBlackList_.count(node.GetId()) != 0) {
            return false; // white list is not empty, or surface is in black list
        }
        return true;
    }

    void UpdateRotationStatusForEffectNode(RSEffectRenderNode& node);
    void CheckFilterNodeInSkippedSubTreeNeedClearCache(const RSRenderNode& node, RSDirtyRegionManager& dirtyManager);
    void UpdateHwcNodeRectInSkippedSubTree(const RSRenderNode& node);
    void UpdateSubSurfaceNodeRectInSkippedSubTree(const RSRenderNode& rootNode);
    void CollectOcclusionInfoForWMS(RSSurfaceRenderNode& node);
    void CollectEffectInfo(RSRenderNode& node);

    void UpdateVirtualScreenSecurityExemption(RSDisplayRenderNode& node);

    /* Check whether gpu overdraw buffer feature can be enabled on the RenderNode
     * 1. is leash window
     * 2. window has scale, radius, no transparency and no animation
     * 3. find the child background node, which is no transparency and completely filling the window
     */
    void CheckIsGpuOverDrawBufferOptimizeNode(RSSurfaceRenderNode& node);

    void MarkBlurIntersectWithDRM(std::shared_ptr<RSRenderNode> node) const;

    void TryNotifyUIBufferAvailable();

    // Used for closing HDR in PC multidisplay becauseof performance and open when singledisplay
    void SetHdrWhenMultiDisplayChangeInPC();

    // record DRM nodes
    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes_;
    sptr<RSScreenManager> screenManager_;
    ScreenInfo screenInfo_;
    RectI screenRect_;
    std::unordered_set<NodeId> allBlackList_; // The collection of blacklist for all screens
    std::unordered_set<NodeId> allWhiteList_; // The collection of whitelist for all screens
    std::shared_ptr<RSDirtyRegionManager> curSurfaceDirtyManager_;
    std::shared_ptr<RSSurfaceRenderNode> curSurfaceNode_;
    std::stack<std::shared_ptr<RSDirtyRegionManager>> surfaceDirtyManager_;
    std::stack<std::shared_ptr<RSSurfaceRenderNode>> surfaceNode_;
    float curAlpha_ = 1.f;
    Vector4f curCornerRadius_{ 0.f, 0.f, 0.f, 0.f };
    bool dirtyFlag_ { false };
    Drawing::Rect boundsRect_ {};
    Gravity frameGravity_ = Gravity::DEFAULT;

    int32_t offsetX_ { 0 };
    int32_t offsetY_ { 0 };
    Drawing::Matrix parentSurfaceNodeMatrix_;

    ScreenId currentVisitDisplay_ = INVALID_SCREEN_ID;
    std::map<ScreenId, bool> displayHasSecSurface_;
    std::map<ScreenId, bool> displayHasSkipSurface_;
    std::map<ScreenId, bool> displayHasSnapshotSkipSurface_;
    std::map<ScreenId, bool> displayHasProtectedSurface_;
    std::map<ScreenId, bool> displaySpecailSurfaceChanged_;
    std::map<ScreenId, bool> hasCaptureWindow_;

    bool hasFingerprint_ = false;
    bool hasUniRenderHdrSurface_ = false;

    std::shared_ptr<RSBaseRenderEngine> renderEngine_;

    std::shared_ptr<RSDirtyRegionManager> curDisplayDirtyManager_;
    std::shared_ptr<RSDisplayRenderNode> curDisplayNode_;
    bool doAnimate_ = false;
    bool isSurfaceRotationChanged_ = false;
    bool isPartialRenderEnabled_ = false;
    bool isCompleteRenderEnabled_ = false;
    bool isOpDropped_ = false;
    bool isDirtyRegionDfxEnabled_ = false; // dirtyRegion DFX visualization
    bool isTargetDirtyRegionDfxEnabled_ = false;
    bool isOpaqueRegionDfxEnabled_ = false;
    bool isVisibleRegionDfxEnabled_ = false;
    bool isAllSurfaceVisibleDebugEnabled_ = false;
    bool isDisplayDirtyDfxEnabled_ = false;
    bool isCanvasNodeSkipDfxEnabled_ = false;
    bool isVirtualDirtyEnabled_ = false;
    bool isVirtualDirtyDfxEnabled_ = false;
    bool isExpandScreenDirtyEnabled_ = false;
    bool hasMirrorDisplay_ = false;
    // if display node has skip layer except capsule window
    bool hasSkipLayer_ = false;
    bool isOcclusionEnabled_ = false;
    bool isSkipCanvasNodeOutOfScreen_ = false;
    bool isScreenRotationAnimating_ = false;
    bool displayNodeRotationChanged_ = false;
    std::vector<std::string> dfxTargetSurfaceNames_;
    PartialRenderType partialRenderType_;
    DirtyRegionDebugType dirtyRegionDebugType_;
    SurfaceRegionDebugType surfaceRegionDebugType_;
    bool isRegionDebugEnabled_ = false;
    bool isDirty_ = false;
    // added for judge if drawing cache changes
    bool isDrawingCacheEnabled_ = false;
    // opinc feature
    bool autoCacheEnable_ = false;
    bool unchangeMarkEnable_ = false;
    bool unchangeMarkInApp_ = false;

    GraphicColorGamut newColorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    GraphicPixelFormat newPixelFormat_ = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    uint64_t currentFocusedNodeId_ = 0;
    uint64_t focusedLeashWindowId_ = 0;

    bool isSubThread_ = false;
    bool isUIFirstDebugEnable_ = false;
    bool ancestorNodeHasAnimation_ = false;
    bool hasAccumulatedClip_ = false;
    uint32_t threadIndex_ = UNI_MAIN_THREAD_INDEX;

    bool isPrevalidateHwcNodeEnable_ = false;

    RectI prepareClipRect_{0, 0, 0, 0}; // renderNode clip rect used in Prepare

    float globalZOrder_ = 0.0f;
    bool isHardwareForcedDisabled_ = false; // indicates if hardware composer is totally disabled
    std::vector<NodeId> uiBufferAvailableId_;
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes_;
    // vector of all app window nodes with surfaceView, sorted by zOrder
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> appWindowNodesInZOrder_;
    // vector of hardwareEnabled nodes above displayNodeSurface like pointer window
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledTopNodes_;
    // vector of Appwindow nodes ids not contain subAppWindow nodes ids in current frame
    std::queue<NodeId> curMainAndLeashWindowNodesIds_;
    // vector of Appwindow nodes ids not contain subAppWindow nodes ids in last frame
    static inline std::queue<NodeId> preMainAndLeashWindowNodesIds_;
    // vector of current dsiplaynode mainwindow surface visible info
    VisibleData dstCurVisVec_;
    // vector of current frame mainwindow surface visible info
    VisibleData allDstCurVisVec_;
    // vector of last frame mainwindow surface visible info
    static inline VisibleData allLastVisVec_;
    std::mutex occlusionMutex_;
    float localZOrder_ = 0.0f; // local zOrder for surfaceView under same app window node

    static void ProcessUnpairedSharedTransitionNode();

    uint32_t appWindowNum_ = 0;
    // variable for occlusion
    bool needRecalculateOcclusion_ = false;
    Occlusion::Region accumulatedOcclusionRegion_;
    Occlusion::Region occlusionRegionWithoutSkipLayer_;

    bool curDirty_ = false;
    // to record and pass container node dirty to leash node.
    bool curContainerDirty_ = false;

    // record nodes in surface which has filter may influence globalDirty
    OcclusionRectISet globalFilter_;
    // record filter in current surface when there is no below dirty
    OcclusionRectISet curSurfaceNoBelowDirtyFilter_;
    // record container nodes which need filter
    FilterRectISet containerFilter_;
    // record nodes which has transparent clean filter
    std::unordered_map<NodeId, std::vector<std::pair<NodeId, RectI>>> transparentCleanFilter_;
    // record nodes which has transparent dirty filter
    std::unordered_map<NodeId, std::vector<std::pair<NodeId, RectI>>> transparentDirtyFilter_;

    std::vector<RectI> globalFilterRects_;
    // visible filter in transparent surface or display must prepare
    bool filterInGlobal_ = true;
    bool needRequestNextVsync_ = true;

    NodeId FindInstanceChildOfDisplay(std::shared_ptr<RSRenderNode> node);

    void UpdateSurfaceRenderNodeScale(RSSurfaceRenderNode& node);

    // use for hardware compose disabled reason collection
    HwcDisabledReasonCollection& hwcDisabledReasonCollection_ = HwcDisabledReasonCollection::GetInstance();

    bool zoomStateChange_ = false;

    // anco RSSurfaceNode process
    bool ancoHasGpu_ = false;
    std::unordered_set<std::shared_ptr<RSSurfaceRenderNode>> ancoNodes_;

    // used in uifirst for checking whether leashwindow or its parent should paint or not
    bool globalShouldPaint_ = true;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_VISITOR_H
