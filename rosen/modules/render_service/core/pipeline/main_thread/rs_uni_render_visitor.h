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

#include "pipeline/render_thread/rs_base_render_engine.h"
#include "system/rs_system_parameters.h"

#include "feature/hwc/rs_uni_hwc_prevalidate_util.h"
#include "feature/window_keyframe/rs_window_keyframe_node_info.h"
#include "common/rs_special_layer_manager.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_pointer_window_manager.h"
#include "pipeline/rs_render_node.h"
#include "platform/ohos/overdraw/rs_overdraw_controller.h"
#include "screen_manager/rs_screen_manager.h"
#include "visitor/rs_node_visitor.h"
#include "info_collection/rs_hdr_collection.h"

namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;
class RSUniHwcVisitor;
class RSOcclusionHandler;
class RSUniRenderVisitor : public RSNodeVisitor {
public:
    using SurfaceDirtyMgrPair = std::pair<std::shared_ptr<RSSurfaceRenderNode>, std::shared_ptr<RSSurfaceRenderNode>>;
    RSUniRenderVisitor();
    RSUniRenderVisitor(const RSUniRenderVisitor& visitor) = delete;
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

    bool GetAnimateState() const
    {
        return doAnimate_;
    }

    // Enable HWCompose
    void MarkHardwareForcedDisabled();

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
    
    uint32_t GetLayerNum() const
    {
        return layerNum_;
    }

    void SurfaceOcclusionCallbackToWMS();

    using RenderParam = std::tuple<std::shared_ptr<RSRenderNode>, RSPaintFilterCanvas::CanvasStatus>;

    bool GetDumpRsTreeDetailEnabled() { return isDumpRsTreeDetailEnabled_; }

    uint32_t IncreasePrepareSeq() { return ++nodePreparedSeqNum_; }

    uint32_t IncreasePostPrepareSeq() { return ++nodePostPreparedSeqNum_; }

    void UpdateCurFrameInfoDetail(RSRenderNode& node, bool subTreeSkipped = false, bool isPostPrepare = false);

    void ResetCrossNodesVisitedStatus();

    void MarkFilterInForegroundFilterAndCheckNeedForceClearCache(RSRenderNode& node);

    void UpdateDrawingCacheInfoBeforeChildren(RSCanvasRenderNode& node);

private:
    /* Prepare relevant calculation */
    // considering occlusion info for app surface as well as widget
    bool IsSubTreeOccluded(RSRenderNode& node) const;
    // restore node's flag and filter dirty collection
    void PostPrepare(RSRenderNode& node, bool subTreeSkipped = false);
    void UpdateNodeVisibleRegion(RSSurfaceRenderNode& node);
    void CalculateOpaqueAndTransparentRegion(RSSurfaceRenderNode& node);

    void CheckFilterCacheNeedForceClearOrSave(RSRenderNode& node);
    Occlusion::Region GetSurfaceTransparentFilterRegion(NodeId surfaceNodeId) const;
    void CollectTopOcclusionSurfacesInfo(RSSurfaceRenderNode& node, bool isParticipateInOcclusion);
    void UpdateOccludedStatusWithFilterNode(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const;
    void PartialRenderOptionInit();
    RSVisibleLevel GetRegionVisibleLevel(const Occlusion::Region& visibleRegion,
        const Occlusion::Region& selfDrawRegion);
    void UpdateSurfaceOcclusionInfo();
    enum class RSPaintStyle {
        FILL,
        STROKE
    };
    // check if surface name is in UIFirst dfx target list
    inline bool CheckIfSurfaceForUIFirstDFX(std::string nodeName)
    {
        return std::find_if(dfxUIFirstSurfaceNames_.begin(), dfxUIFirstSurfaceNames_.end(),
            [&](const std::string& str) {
                return nodeName.find(str) != std::string::npos;
            }) != dfxUIFirstSurfaceNames_.end();
    }
    bool InitDisplayInfo(RSDisplayRenderNode& node);

    bool BeforeUpdateSurfaceDirtyCalc(RSSurfaceRenderNode& node);
    bool NeedPrepareChindrenInReverseOrder(RSRenderNode& node) const;
    bool IsLeashAndHasMainSubNode(RSRenderNode& node) const;
    bool AfterUpdateSurfaceDirtyCalc(RSSurfaceRenderNode& node);
    void UpdateLeashWindowVisibleRegionEmpty(RSSurfaceRenderNode& node);
    void UpdateSurfaceRenderNodeRotate(RSSurfaceRenderNode& node);
    void UpdateSurfaceDirtyAndGlobalDirty();
    // should be removed due to rcd node will be handled by RS tree in OH 6.0 rcd refactoring
    void UpdateDisplayRcdRenderNode();
    // should ensure that the surface size of dirty region manager has been set
    void ResetDisplayDirtyRegion();
    bool CheckScreenPowerChange() const;
    bool CheckCurtainScreenUsingStatusChange() const;
    bool CheckLuminanceStatusChange(ScreenId id);
    bool CheckSkipCrossNode(RSSurfaceRenderNode& node);
    bool CheckSkipAndPrepareForCrossNode(RSSurfaceRenderNode& node);
    bool IsFirstFrameOfPartialRender() const;
    bool IsFirstFrameOfOverdrawSwitch() const;
    bool IsFirstFrameOfDrawingCacheDfxSwitch() const;
    bool IsAccessibilityConfigChanged() const;
    bool IsWatermarkFlagChanged() const;
    void UpdateDisplayZoomState();
    void CollectFilterInfoAndUpdateDirty(RSRenderNode& node,
        RSDirtyRegionManager& dirtyManager, const RectI& globalFilterRect, const RectI& globalHwcFilterRect);
    RectI GetVisibleEffectDirty(RSRenderNode& node) const;

    // This function is used for solving display problems caused by dirty blurfilter node half-obscured.
    void UpdateDisplayDirtyAndExtendVisibleRegion();
    // This function is used to update global dirty and visibleRegion
    // by processing dirty blurfilter node obscured.
    void ProcessFilterNodeObscured(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
        Occlusion::Region& extendRegion, const RSRenderNodeMap& nodeMap);

    void UpdateHwcNodeInfoForAppNode(RSSurfaceRenderNode& node);
    void UpdateAncoPrepareClip(RSSurfaceRenderNode& node);
    void ProcessAncoNode(std::shared_ptr<RSSurfaceRenderNode>& hwcNodePtr, bool& ancoHasGpu);
    void UpdateAncoNodeHWCDisabledState(std::unordered_set<std::shared_ptr<RSSurfaceRenderNode>>& ancoNodes);
    void UpdateHwcNodeDirtyRegionAndCreateLayer(std::shared_ptr<RSSurfaceRenderNode>& node);
    void AllSurfacesDrawnInUniRender(const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& hwcNodes);
    void UpdatePointWindowDirtyStatus(std::shared_ptr<RSSurfaceRenderNode>& pointWindow);
    void UpdateTopLayersDirtyStatus(const std::vector<std::shared_ptr<RSSurfaceRenderNode>>& topLayers);
    void UpdateCornerRadiusInfoForDRM(std::shared_ptr<RSSurfaceRenderNode> hwcNode, std::vector<RectI>& hwcRects);
    bool CheckIfRoundCornerIntersectDRM(const float& ratio, std::vector<float>& ratioVector,
        const Vector4f& instanceCornerRadius, const RectI& instanceAbsRect, const RectI& hwcAbsRect);
    void UpdateIfHwcNodesHaveVisibleRegion(std::vector<RSBaseRenderNode::SharedPtr>& curMainAndLeashSurfaces);
    void UpdateHwcNodesIfVisibleForApp(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
        const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& hwcNodes,
        bool& hasVisibleHwcNodes, bool& needForceUpdateHwcNodes);
    void PrevalidateHwcNode();
    bool PrepareForCloneNode(RSSurfaceRenderNode& node);
    void PrepareForCrossNode(RSSurfaceRenderNode& node);

    // use in QuickPrepareSurfaceRenderNode, update SurfaceRenderNode's uiFirst status
    void PrepareForUIFirstNode(RSSurfaceRenderNode& node);

    void PrepareForMultiScreenViewSurfaceNode(RSSurfaceRenderNode& node);
    void PrepareForMultiScreenViewDisplayNode(RSDisplayRenderNode& node);

    void UpdateHwcNodeDirtyRegionForApp(std::shared_ptr<RSSurfaceRenderNode>& appNode,
        std::shared_ptr<RSSurfaceRenderNode>& hwcNode);

    void AccumulateSurfaceDirtyRegion(
        std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, Occlusion::Region& accumulatedDirtyRegion) const;
    void CheckMergeDisplayDirtyByCrossDisplayWindow(RSSurfaceRenderNode& surfaceNode) const;
    void PrepareForSkippedCrossNode(RSSurfaceRenderNode& surfaceNode);
    void CollectFilterInCrossDisplayWindow(
        std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, Occlusion::Region& accumulatedDirtyRegion);
    void CheckMergeDisplayDirtyByTransparent(RSSurfaceRenderNode& surfaceNode) const;
    void CheckMergeDisplayDirtyByZorderChanged(RSSurfaceRenderNode& surfaceNode) const;
    void CheckMergeDisplayDirtyByPosChanged(RSSurfaceRenderNode& surfaceNode) const;
    void CheckMergeDisplayDirtyByShadowChanged(RSSurfaceRenderNode& surfaceNode) const;
    void CheckMergeDisplayDirtyBySurfaceChanged() const;
    void CheckMergeDisplayDirtyByAttraction(RSSurfaceRenderNode& surfaceNode) const;
    void CheckMergeSurfaceDirtysForDisplay(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const;
    void CheckMergeDisplayDirtyByTransparentRegions(RSSurfaceRenderNode& surfaceNode) const;
    void CheckMergeFilterDirtyByIntersectWithDirty(OcclusionRectISet& filterSet, bool isGlobalDirty);

    void CheckMergeDisplayDirtyByTransparentFilter(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
        Occlusion::Region& accumulatedDirtyRegion);
    void CheckMergeGlobalFilterForDisplay(Occlusion::Region& accumulatedDirtyRegion);
    void CheckMergeDebugRectforRefreshRate(std::vector<RSBaseRenderNode::SharedPtr>& surfaces);
    void CheckMergeDisplayDirtyByRoundCornerDisplay() const;

    // merge last childRect as dirty if any child has been removed
    void MergeRemovedChildDirtyRegion(RSRenderNode& node, bool needMap = false);
    // Reset curSurface info as upper surfaceParent in case surfaceParent has multi children
    void ResetCurSurfaceInfoAsUpperSurfaceParent(RSSurfaceRenderNode& node);

    void CheckColorSpace(RSSurfaceRenderNode& node);
    void CheckColorSpaceWithSelfDrawingNode(RSSurfaceRenderNode& node, GraphicColorGamut& newColorSpace);
    void UpdateColorSpaceAfterHwcCalc(RSDisplayRenderNode& node);
    void HandleColorGamuts(RSDisplayRenderNode& node, const sptr<RSScreenManager>& screenManager);
    void CheckPixelFormat(RSSurfaceRenderNode& node);
    void HandlePixelFormat(RSDisplayRenderNode& node, const sptr<RSScreenManager>& screenManager);

    bool IsHardwareComposerEnabled();
    void UpdateSpecialLayersRecord(RSSurfaceRenderNode& node);
    void SendRcdMessage(RSDisplayRenderNode& node);

    bool ForcePrepareSubTree()
    {
        return (curSurfaceNode_ && curSurfaceNode_->GetNeedCollectHwcNode()) || IsAccessibilityConfigChanged() ||
               isFirstFrameAfterScreenRotation_;
    }

    inline bool IsValidInVirtualScreen(const RSSurfaceRenderNode& node) const
    {
        const auto& specialLayerMgr = node.GetSpecialLayerMgr();
        if (specialLayerMgr.Find(IS_GENERAL_SPECIAL)) {
            return false; // surface is special layer
        }
        if (!allWhiteList_.empty() || allBlackList_.count(node.GetId()) != 0) {
            return false; // white list is not empty, or surface is in black list
        }
        return true;
    }

    void UpdateRotationStatusForEffectNode(RSEffectRenderNode& node);
    void CheckFilterNodeInSkippedSubTreeNeedClearCache(const RSRenderNode& node, RSDirtyRegionManager& dirtyManager);
    void UpdateSubSurfaceNodeRectInSkippedSubTree(const RSRenderNode& rootNode);
    void CollectOcclusionInfoForWMS(RSSurfaceRenderNode& node);
    void CollectEffectInfo(RSRenderNode& node);

    void UpdateVirtualScreenInfo(RSDisplayRenderNode& node);
    void UpdateVirtualScreenSecurityExemption(RSDisplayRenderNode& node, RSDisplayRenderNode& mirrorNode);
    void UpdateVirtualScreenVisibleRectSecurity(RSDisplayRenderNode& node, RSDisplayRenderNode& mirrorNode);

    /* Check whether gpu overdraw buffer feature can be enabled on the RenderNode
     * 1. is leash window
     * 2. window has scale, radius, no transparency and no animation
     * 3. find the child background node, which is no transparency and completely filling the window
     */
    void CheckIsGpuOverDrawBufferOptimizeNode(RSSurfaceRenderNode& node);
    void MarkBlurIntersectWithDRM(std::shared_ptr<RSRenderNode> node) const;

    // Used for closing HDR in PC multidisplay becauseof performance and open when singledisplay
    void SetHdrWhenMultiDisplayChange();

    void TryNotifyUIBufferAvailable();
    bool IsHdrUseUnirender(bool hasUniRenderHdrSurface, std::shared_ptr<RSSurfaceRenderNode>& hwcNodePtr);

    void CollectSelfDrawingNodeRectInfo(RSSurfaceRenderNode& node);

    // Used to collect prepared subtree into the control-level occlusion culling handler.
    // For the root node, trigger occlusion detection.
    void CollectSubTreeAndProcessOcclusion(RSRenderNode& node, bool subTreeSkipped);

    // Used to collect prepared node into the control-level occlusion culling handler.
    void CollectNodeForOcclusion(RSRenderNode& node);

    // Used to initialize the handler of control-level occlusion culling.
    void InitializeOcclusionHandler(RSRootRenderNode& node);

    friend class RSUniHwcVisitor;
    std::unique_ptr<RSUniHwcVisitor> hwcVisitor_;

    bool isCompleteRenderEnabled_ = false;
    std::shared_ptr<RSBaseRenderEngine> renderEngine_;
    bool doAnimate_ = false;
    bool isDirty_ = false;
    bool dirtyFlag_ { false };
    bool isPartialRenderEnabled_ = false;
    bool isAdvancedDirtyRegionEnabled_ = false;
    bool isRegionDebugEnabled_ = false;
    bool ancestorNodeHasAnimation_ = false;
    bool curDirty_ = false;
    uint64_t currentFocusedNodeId_ = 0;
    uint64_t focusedLeashWindowId_ = 0;
    std::shared_ptr<RSDirtyRegionManager> curSurfaceDirtyManager_;
    std::shared_ptr<RSDirtyRegionManager> curDisplayDirtyManager_;
    std::shared_ptr<RSSurfaceRenderNode> curSurfaceNode_;
    ScreenId currentVisitDisplay_ = INVALID_SCREEN_ID;
    RSSpecialLayerManager specialLayerManager_;
    std::map<ScreenId, bool> displaySpecailSurfaceChanged_;
    std::map<ScreenId, bool> hasCaptureWindow_;
    std::map<ScreenId, bool> hasFingerprint_;
    std::shared_ptr<RSDisplayRenderNode> curDisplayNode_;
    // record nodes which has transparent clean filter
    std::unordered_map<NodeId, std::vector<std::pair<NodeId, RectI>>> transparentCleanFilter_;
    // record nodes which has transparent dirty filter
    std::unordered_map<NodeId, std::vector<std::pair<NodeId, RectI>>> transparentDirtyFilter_;
    // record DRM nodes
    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes_;
    int16_t occlusionSurfaceOrder_ = DEFAULT_OCCLUSION_SURFACE_ORDER;
    sptr<RSScreenManager> screenManager_;
    ScreenInfo screenInfo_;
    RectI screenRect_;
    std::unordered_set<NodeId> allBlackList_; // The collection of blacklist for all screens
    std::unordered_set<NodeId> allWhiteList_; // The collection of whitelist for all screens
    Occlusion::Region accumulatedOcclusionRegion_;
    Occlusion::Region occlusionRegionWithoutSkipLayer_;
    // variable for occlusion
    bool needRecalculateOcclusion_ = false;
    bool displayNodeRotationChanged_ = false;
    bool hasAccumulatedClip_ = false;
    // if display node has skip layer except capsule window
    bool hasSkipLayer_ = false;
    float curAlpha_ = 1.f;
    float globalZOrder_ = 0.0f;
    bool isScreenRotationAnimating_ = false;
    // use for not skip subtree prepare in first frame after screen rotation
    bool isFirstFrameAfterScreenRotation_ = false;
    static bool isLastFrameRotating_;
    // added for judge if drawing cache changes
    bool isDrawingCacheEnabled_ = false;
    bool unchangeMarkEnable_ = false;
    bool unchangeMarkInApp_ = false;
    // vector of Appwindow nodes ids not contain subAppWindow nodes ids in current frame
    std::queue<NodeId> curMainAndLeashWindowNodesIds_;
    RectI prepareClipRect_{0, 0, 0, 0}; // renderNode clip rect used in Prepare
    Vector4f curCornerRadius_{ 0.f, 0.f, 0.f, 0.f };
    Drawing::Matrix parentSurfaceNodeMatrix_;
    bool isSwitchToSourceCrossNodePrepare_ = false;
    // visible filter in transparent surface or display must prepare
    bool filterInGlobal_ = true;
    // opinc feature
    bool autoCacheEnable_ = false;

    // to record and pass container node dirty to leash node.
    bool curContainerDirty_ = false;
    bool isOcclusionEnabled_ = false;
    bool hasMirrorDisplay_ = false;
    Drawing::Rect boundsRect_ {};
    Gravity frameGravity_ = Gravity::DEFAULT;
    // vector of current displaynode mainwindow surface visible info
    VisibleData dstCurVisVec_;
    // record container nodes which need filter
    FilterRectISet containerFilter_;
    // record nodes in surface which has filter may influence globalDirty
    OcclusionRectISet globalFilter_;
    // record filter in current surface when there is no below dirty
    OcclusionRectISet curSurfaceNoBelowDirtyFilter_;
    // vector of current frame mainwindow surface visible info
    VisibleData allDstCurVisVec_;
    bool hasDisplayHdrOn_ = false;
    std::vector<NodeId> hasVisitedCrossNodeIds_;
    bool isDirtyRegionDfxEnabled_ = false; // dirtyRegion DFX visualization
    bool isTargetDirtyRegionDfxEnabled_ = false;
    bool isOpaqueRegionDfxEnabled_ = false;
    bool isVisibleRegionDfxEnabled_ = false;
    bool isAllSurfaceVisibleDebugEnabled_ = false;
    bool isDisplayDirtyDfxEnabled_ = false;
    bool isOpDropped_ = false;
    bool isDirtyAlignEnabled_ = false;
    bool isStencilPixelOcclusionCullingEnabled_ = false;
    bool isUIFirstDebugEnable_ = false;
    bool isVirtualDirtyEnabled_ = false;
    bool isVirtualDirtyDfxEnabled_ = false;
    bool isExpandScreenDirtyEnabled_ = false;
    bool needRequestNextVsync_ = true;
    CrossNodeOffScreenRenderDebugType isCrossNodeOffscreenOn_ = CrossNodeOffScreenRenderDebugType::ENABLE;
    DirtyRegionDebugType dirtyRegionDebugType_;
    AdvancedDirtyRegionType advancedDirtyType_;
    std::vector<std::string> dfxTargetSurfaceNames_;

    std::stack<std::shared_ptr<RSDirtyRegionManager>> surfaceDirtyManager_;
    int32_t offsetX_ { 0 };
    int32_t offsetY_ { 0 };
    bool isTargetUIFirstDfxEnabled_ = false;
    std::vector<std::string> dfxUIFirstSurfaceNames_;
    std::vector<NodeId> uiBufferAvailableId_;
    PartialRenderType partialRenderType_;
    SurfaceRegionDebugType surfaceRegionDebugType_;
    // vector of Appwindow nodes ids not contain subAppWindow nodes ids in last frame
    static inline std::queue<NodeId> preMainAndLeashWindowNodesIds_;
    // vector of last frame mainwindow surface visible info
    static inline VisibleData allLastVisVec_;
    std::mutex occlusionMutex_;
    static void ProcessUnpairedSharedTransitionNode();
    NodeId FindInstanceChildOfDisplay(std::shared_ptr<RSRenderNode> node);
    void UpdateSurfaceRenderNodeScale(RSSurfaceRenderNode& node);

    std::shared_ptr<RsHdrCollection> rsHdrCollection_ = RsHdrCollection::GetInstance();

    bool zoomStateChange_ = false;

    // anco RSSurfaceNode process
    bool ancoHasGpu_ = false;
    std::unordered_set<std::shared_ptr<RSSurfaceRenderNode>> ancoNodes_;
    uint32_t layerNum_ = 0;

    NodeId clonedSourceNodeId_ = INVALID_NODEID;

    bool isDumpRsTreeDetailEnabled_ = false;
    uint32_t nodePreparedSeqNum_ = 0;
    uint32_t nodePostPreparedSeqNum_ = 0;

    // Used for PC window resize scene
    RSWindowKeyframeNodeInfo windowKeyFrameNodeInf_;

    // used in uifirst for checking whether leashwindow or its parent should paint or not
    bool globalShouldPaint_ = true;

    uint32_t rsDisplayNodeChildNum_ = 0;

    int32_t appWindowZOrder_ = 0;

    // Used for control-level occlusion culling.
    std::shared_ptr<RSOcclusionHandler> curOcclusionHandler_;

    // in foregroundFilter subtree
    bool inForegroundFilter_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_VISITOR_H
