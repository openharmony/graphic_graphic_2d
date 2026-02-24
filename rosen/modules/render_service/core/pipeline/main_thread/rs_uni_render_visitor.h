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
#include "feature/round_corner_display/rs_rcd_render_manager.h"
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

    // To prepare nodes between ScreenRenderNode and app nodes.
    void QuickPrepareEffectRenderNode(RSEffectRenderNode& node) override;
    void QuickPrepareCanvasRenderNode(RSCanvasRenderNode& node) override;
    void QuickPrepareScreenRenderNode(RSScreenRenderNode& node) override;
    void QuickPrepareLogicalDisplayRenderNode(RSLogicalDisplayRenderNode& node) override;
    void QuickPrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override;
    void QuickPrepareUnionRenderNode(RSUnionRenderNode& node) override;
    void QuickPrepareWindowKeyFrameRenderNode(RSWindowKeyFrameRenderNode& node) override;
    void QuickPrepareChildren(RSRenderNode& node) override;

    void PrepareChildren(RSRenderNode& node) override {};
    void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override {}
    void PrepareScreenRenderNode(RSScreenRenderNode& node) override {}
    void PrepareLogicalDisplayRenderNode(RSLogicalDisplayRenderNode& node) override {}
    void PrepareProxyRenderNode(RSProxyRenderNode& node) override {}
    // prepareroot also used for quickprepare
    void PrepareRootRenderNode(RSRootRenderNode& node) override;
    void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override {}
    void PrepareEffectRenderNode(RSEffectRenderNode& node) override {}

    void ProcessChildren(RSRenderNode& node) override {};
    void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override {}
    void ProcessScreenRenderNode(RSScreenRenderNode& node) override {}
    void ProcessLogicalDisplayRenderNode(RSLogicalDisplayRenderNode& node) override {}
    void ProcessProxyRenderNode(RSProxyRenderNode& node) override {}
    void ProcessRootRenderNode(RSRootRenderNode& node) override {}
    void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override {}
    void ProcessEffectRenderNode(RSEffectRenderNode& node) override {}

    void RegisterHpaeCallback(RSRenderNode& node) override;

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

    void UpdateOffscreenCanvasNodeId(RSCanvasRenderNode& node);

private:
    /* Prepare relevant calculation */
    // considering occlusion info for app surface as well as widget
    bool IsSubTreeOccluded(RSRenderNode& node) const;
    // restore node's flag and filter dirty collection
    void PostPrepare(RSRenderNode& node, bool subTreeSkipped = false);
    void UpdateNodeVisibleRegion(RSSurfaceRenderNode& node);
    void CalculateOpaqueAndTransparentRegion(RSSurfaceRenderNode& node);
    void CheckResetAccumulatedOcclusionRegion(RSSurfaceRenderNode& node);
    bool IsParticipateInOcclusion(RSSurfaceRenderNode& node);

    void CheckFilterCacheNeedForceClearOrSave(RSRenderNode& node);
    Occlusion::Region GetSurfaceTransparentFilterRegion(const RSSurfaceRenderNode& surfaceNode) const;
    void CollectTopOcclusionSurfacesInfo(RSSurfaceRenderNode& node, bool isParticipateInOcclusion);
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
    bool InitScreenInfo(RSScreenRenderNode& node);
    bool InitLogicalDisplayInfo(RSLogicalDisplayRenderNode& node);

    void UpdateCompositeType(RSScreenRenderNode& node);
    bool BeforeUpdateSurfaceDirtyCalc(RSSurfaceRenderNode& node);
    bool NeedPrepareChindrenInReverseOrder(RSRenderNode& node) const;
    bool IsLeashAndHasMainSubNode(RSRenderNode& node) const;
    bool AfterUpdateSurfaceDirtyCalc(RSSurfaceRenderNode& node);
    void UpdateSurfaceRenderNodeRotate(RSSurfaceRenderNode& node);
    void UpdateSurfaceDirtyAndGlobalDirty();
    // should ensure that the surface size of dirty region manager has been set
    void ResetDisplayDirtyRegion();
    bool CheckScreenPowerChange() const;
    bool CheckCurtainScreenUsingStatusChange() const;
    bool CheckLuminanceStatusChange(ScreenId id);
    bool CheckSkipCrossNode(RSSurfaceRenderNode& node);
    bool CheckSkipAndPrepareForCrossNode(RSSurfaceRenderNode& node);
    bool IsFirstFrameOfOverdrawSwitch() const;
    bool IsFirstFrameOfPartialRender() const;
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
    void ProcessAncoNode(const std::shared_ptr<RSSurfaceRenderNode>& hwcNodePtr, bool& ancoHasGpu);
    void UpdateAncoNodeHWCDisabledState(const std::vector<std::shared_ptr<RSSurfaceRenderNode>>& ancoNodes);
    void UpdateHwcNodeDirtyRegionAndCreateLayer(
        std::shared_ptr<RSSurfaceRenderNode>& node, std::vector<std::shared_ptr<RSSurfaceRenderNode>>& topLayers);
    void AllSurfacesDrawnInUniRender(const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& hwcNodes);
    void UpdatePointWindowDirtyStatus(std::shared_ptr<RSSurfaceRenderNode>& pointWindow);
    void UpdateTopLayersDirtyStatus(const std::vector<std::shared_ptr<RSSurfaceRenderNode>>& topLayers);
    void UpdateCornerRadiusInfoForDRM(std::shared_ptr<RSSurfaceRenderNode> hwcNode, std::vector<RectI>& hwcRects);
    bool CheckIfRoundCornerIntersectDRM(const float ratio, std::vector<float>& ratioVector,
        const Vector4f& instanceCornerRadius, const RectI& instanceAbsRect, const RectI& hwcAbsRect);
    void UpdateIfHwcNodesHaveVisibleRegion(std::vector<RSBaseRenderNode::SharedPtr>& curMainAndLeashSurfaces);
    void UpdateHwcNodesIfVisibleForApp(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
        const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& hwcNodes,
        bool& hasVisibleHwcNodes, bool& needForceUpdateHwcNodes);
    void PrevalidateHwcNode();
    bool PrepareForCloneNode(RSSurfaceRenderNode& node);
    void UpdateInfoForClonedNode(RSSurfaceRenderNode& node);
    void PrepareForCrossNode(RSSurfaceRenderNode& node);

    // use in QuickPrepareSurfaceRenderNode, update SurfaceRenderNode's uiFirst status
    void PrepareForUIFirstNode(RSSurfaceRenderNode& node);

    void PrepareForMultiScreenViewSurfaceNode(RSSurfaceRenderNode& node);
    void PrepareForMultiScreenViewDisplayNode(RSScreenRenderNode& node);

    bool CheckSkipAndUpdateForegroundSurfaceRenderNode(RSSurfaceRenderNode& node);
    bool CheckSkipBackgroundSurfaceRenderNode(RSSurfaceRenderNode& node);
    bool CheckQuickSkipSurfaceRenderNode(RSSurfaceRenderNode& node);

    void UpdateHwcNodeDirtyRegionForApp(std::shared_ptr<RSSurfaceRenderNode>& appNode,
        std::shared_ptr<RSSurfaceRenderNode>& hwcNode);

    void SubSurfaceOpaqueRegionFromAccumulatedDirtyRegion(
        const RSSurfaceRenderNode& surfaceNode, Occlusion::Region& accumulatedDirtyRegion) const;
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
    void CheckMergeDisplayDirtyByAttractionChanged(RSSurfaceRenderNode& surfaceNode) const;
    void CheckMergeDisplayDirtyBySurfaceChanged() const;
    void CheckMergeDisplayDirtyByAttraction(RSSurfaceRenderNode& surfaceNode) const;
    void CheckMergeSurfaceDirtysForDisplay(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const;
    void CheckMergeDisplayDirtyByTransparentRegions(RSSurfaceRenderNode& surfaceNode) const;
    void CheckMergeFilterDirtyWithPreDirty(const std::shared_ptr<RSDirtyRegionManager>& dirtyManager,
        Occlusion::Region accumulatedDirtyRegion, FilterDirtyType filterDirtyType);

    void CheckMergeDebugRectforRefreshRate(std::vector<RSBaseRenderNode::SharedPtr>& surfaces);
    void CheckMergeScreenDirtyByRoundCornerDisplay() const;

    // merge last childRect as dirty if any child has been removed
    void MergeRemovedChildDirtyRegion(RSRenderNode& node, bool needMap = false);
    // Reset curSurface info as upper surfaceParent in case surfaceParent has multi children
    void ResetCurSurfaceInfoAsUpperSurfaceParent(RSSurfaceRenderNode& node);
    bool CheckIfSkipDrawInVirtualScreen(RSSurfaceRenderNode& node);

    void HandleVirtualScreenColorGamut(RSScreenRenderNode& node);
    bool IsWiredMirrorScreen(RSScreenRenderNode& node);
    void HandleWiredMirrorScreenColorGamut(RSScreenRenderNode& node);
    bool IsWiredExtendedScreen(RSScreenRenderNode& node);
    void HandleWiredExtendedScreenColorGamut(RSScreenRenderNode& node);
    void HandleMainScreenColorGamut(RSScreenRenderNode& node);

    void CheckColorSpace(RSSurfaceRenderNode& node);
    void CheckColorSpaceWithSelfDrawingNode(RSSurfaceRenderNode& node);
    void UpdateColorSpaceAfterHwcCalc(RSScreenRenderNode& node);
    void HandleColorGamuts(RSScreenRenderNode& node);
    void CheckPixelFormat(RSSurfaceRenderNode& node);
    void HandlePixelFormat(RSScreenRenderNode& node);
    bool IsHardwareComposerEnabled();

    void UpdateSpecialLayersRecord(RSSurfaceRenderNode& node);
    void UpdateBlackListRecord(RSSurfaceRenderNode& node);
    void DealWithSpecialLayer(RSSurfaceRenderNode& node);
    void SendRcdMessage(RSScreenRenderNode& node);

    bool ForcePrepareSubTree()
    {
        return (curSurfaceNode_ && curSurfaceNode_->GetNeedCollectHwcNode()) || IsAccessibilityConfigChanged() ||
               isFirstFrameAfterScreenRotation_ || isCurSubTreeForcePrepare_;
    }
    // Wish to force prepare specific subtrees? Add conditions here
    bool IsCurrentSubTreeForcePrepare(RSRenderNode& node);

    inline bool IsValidInVirtualScreen(const RSSurfaceRenderNode& node) const
    {
        const auto& specialLayerMgr = node.GetSpecialLayerMgr();
        if (specialLayerMgr.Find(IS_GENERAL_SPECIAL) || isSkipDrawInVirtualScreen_) {
            return false; // surface is special layer
        }
        if (!allWhiteList_.empty() || allBlackList_.count(node.GetId()) != 0) {
            return false; // white list is not empty, or surface is in black list
        }
        return true;
    }

    void UpdateRotationStatusForEffectNode(RSEffectRenderNode& node);
    void UpdateFilterRegionInSkippedSurfaceNode(const RSRenderNode& rootNode, RSDirtyRegionManager& dirtyManager);
    void CheckFilterNodeInSkippedSubTreeNeedClearCache(const RSRenderNode& node, RSDirtyRegionManager& dirtyManager);
    void UpdateVisibleEffectChildrenStatus(const RSRenderNode& rootNode);
    void CheckFilterNodeInOccludedSkippedSubTreeNeedClearCache(const RSRenderNode& node,
        RSDirtyRegionManager& dirtyManager);
    void UpdateSubSurfaceNodeRectInSkippedSubTree(const RSRenderNode& rootNode);
    void CollectOcclusionInfoForWMS(RSSurfaceRenderNode& node);
    void CollectEffectInfo(RSRenderNode& node);

    void UpdateVirtualDisplayInfo(RSLogicalDisplayRenderNode& node);
    void UpdateVirtualDisplaySecurityExemption(
        RSLogicalDisplayRenderNode& node, RSLogicalDisplayRenderNode& mirrorNode);
    void UpdateVirtualDisplayVisibleRectSecurity(
        RSLogicalDisplayRenderNode& node, RSLogicalDisplayRenderNode& mirrorNode);

    // used for renderGroup
    void UpdateDrawingCacheInfoBeforeChildren(RSCanvasRenderNode& node);
    void UpdateDrawingCacheInfoAfterChildren(RSRenderNode& node);
    void AddRenderGroupCacheRoot(RSCanvasRenderNode& node);
    void PopRenderGroupCacheRoot(const RSCanvasRenderNode& node);
    void SetRenderGroupSubTreeDirtyIfNeed(const RSRenderNode& node);
    bool IsOnRenderGroupExcludedSubTree() const;
    // !used for renderGroup

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

    void CollectSelfDrawingNodeRectInfo(RSSurfaceRenderNode& node);

    // Used to collect prepared subtree into the control-level occlusion culling handler.
    // For the root node, trigger occlusion detection.
    void CollectSubTreeAndProcessOcclusion(RSRenderNode& node, bool subTreeSkipped);

    // Used to collect prepared node into the control-level occlusion culling handler.
    void CollectNodeForOcclusion(RSRenderNode& node);

    // Used to initialize the handler of control-level occlusion culling.
    void InitializeOcclusionHandler(RSSurfaceRenderNode& node);
    void HandleTunnelLayerId(RSSurfaceRenderNode& node);

    void UpdateChildBlurBehindWindowAbsMatrix(RSRenderNode& node);

    void CollectSurfaceLockLayer(RSSurfaceRenderNode& node);

    void CheckFilterNeedEnableDebug(RSEffectRenderNode& node, bool hasEffectNodeInParent);

    void UpdateFixedSize(RSLogicalDisplayRenderNode& node);

    void DisableOccludedHwcNodeInSkippedSubTree(const RSRenderNode& node) const;

    void PrepareColorPickerDrawable(RSRenderNode& node);

    friend class RSUniHwcVisitor;
    std::unique_ptr<RSUniHwcVisitor> hwcVisitor_;

    bool isBgWindowTraversalStarted_ = false;
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
    std::shared_ptr<RSDirtyRegionManager> curScreenDirtyManager_;
    std::shared_ptr<RSSurfaceRenderNode> curSurfaceNode_;
    std::shared_ptr<RSUnionRenderNode> curUnionNode_;
    RSSpecialLayerManager specialLayerManager_;

    bool hasFingerprint_ = false;
    std::shared_ptr<RSScreenRenderNode> curScreenNode_;
    std::shared_ptr<RSLogicalDisplayRenderNode> curLogicalDisplayNode_;
    // record nodes which ......
    std::unordered_map<NodeId, std::vector<std::pair<NodeId, Rect>>> transparntHwcCleanFilter_;
    // record nodes which ......
    std::unordered_map<NodeId, std::vector<std::pair<NodeId, Rect>>> transparntHwcDirtyFilter_;
    // record DRM nodes
    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes_;
    int16_t occlusionSurfaceOrder_ = DEFAULT_OCCLUSION_SURFACE_ORDER;
    sptr<RSScreenManager> screenManager_;
    static std::unordered_set<NodeId> allBlackList_; // The collection of blacklist for all screens
    static std::unordered_set<NodeId> allWhiteList_; // The collection of whitelist for all screens
    // The info of whitelist contains screenId
    std::unordered_map<ScreenId, std::unordered_set<uint64_t>> screenWhiteList_;
    std::unordered_set<NodeId> childHasProtectedNodeSet_;

    Occlusion::Region accumulatedOcclusionRegion_;
    Occlusion::Region accumulatedOcclusionRegionBehindWindow_; // Accumulate transparent area
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
    // used for renderGroup
    bool isDrawingCacheEnabled_ = false;
    std::unordered_map<NodeId, std::shared_ptr<RSCanvasRenderNode>> renderGroupCacheRoots_;
    bool hasMarkedRenderGroupSubTreeDirty_ = false;
    NodeId curExcludedRootNodeId_ = INVALID_NODEID;
    // !used for renderGroup
    bool unchangeMarkEnable_ = false;
    bool unchangeMarkInApp_ = false;
    // vector of Appwindow nodes ids not contain subAppWindow nodes ids in current frame
    std::queue<NodeId> curMainAndLeashWindowNodesIds_;
    RectI prepareClipRect_{0, 0, 0, 0}; // renderNode clip rect used in Prepare
    /*
     * surfaceRenderNode clip rect used in Prepare.
     * use as the clip bounds of the filter with a custom snapshot/drawing rect.
     */
    std::optional<RectI> prepareFilterClipRect_ = std::nullopt;
    Vector4f curCornerRadius_{ 0.f, 0.f, 0.f, 0.f };
    RectI curCornerRect_;
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
    // vector of current frame mainwindow surface visible info
    VisibleData allDstCurVisVec_;
    bool hasDisplayHdrOn_ = false;
    std::vector<NodeId> hasVisitedCrossNodeIds_;
    bool isDirtyRegionDfxEnabled_ = false; // dirtyRegion DFX visualization
    bool isMergedDirtyRegionDfxEnabled_ = false;
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
    bool isVirtualExpandScreenDirtyEnabled_ = false;
    bool needRequestNextVsync_ = true;
    bool isTargetUIFirstDfxEnabled_ = false;
#ifdef SUBTREE_PARALLEL_ENABLE
    bool isInFocusSurface_ = false;
#endif
    CrossNodeOffScreenRenderDebugType isCrossNodeOffscreenOn_ = CrossNodeOffScreenRenderDebugType::ENABLE;
    DirtyRegionDebugType dirtyRegionDebugType_;
    AdvancedDirtyRegionType advancedDirtyType_;
    std::vector<std::string> dfxTargetSurfaceNames_;
    std::vector<std::string> dfxUIFirstSurfaceNames_;
    std::vector<NodeId> uiBufferAvailableId_;

    std::stack<std::shared_ptr<RSDirtyRegionManager>> surfaceDirtyManager_;
    int32_t offsetX_ { 0 };
    int32_t offsetY_ { 0 };
    PartialRenderType partialRenderType_;
    SurfaceRegionDebugType surfaceRegionDebugType_;
    // vector of Appwindow nodes ids not contain subAppWindow nodes ids in last frame
    static inline std::queue<NodeId> preMainAndLeashWindowNodesIds_;

    // vector of last frame mainwindow surface visible info
    static inline VisibleData allLastVisVec_;
    std::mutex occlusionMutex_;
    static void ProcessUnpairedSharedTransitionNode();
    void UpdateSurfaceRenderNodeScale(RSSurfaceRenderNode& node);
    std::shared_ptr<RsHdrCollection> rsHdrCollection_ = RsHdrCollection::GetInstance();

    bool zoomStateChange_ = false;

    uint32_t layerNum_ = 0;

    // first: cloneSource id; second: cloneSource drawable
    std::map<NodeId, DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr> cloneNodeMap_;

    bool isDumpRsTreeDetailEnabled_ = false;
    uint32_t nodePreparedSeqNum_ = 0;
    uint32_t nodePostPreparedSeqNum_ = 0;

    // used in uifirst for checking whether leashwindow or its parent should paint or not
    bool globalShouldPaint_ = true;

    int32_t appWindowZOrder_ = 0;

    // Used for control-level occlusion culling.
    std::shared_ptr<RSOcclusionHandler> curOcclusionHandler_;

    // record OffscreenCanvas's NodeId
    NodeId offscreenCanvasNodeId_ = INVALID_NODEID;

    int32_t rsScreenNodeChildNum_ = 0;
    size_t rsScreenNodeNum_ = 0;

    ScreenState screenState_ = ScreenState::UNKNOWN;
    
    bool isSkipDrawInVirtualScreen_ = false;

    // used for finding the first effect render node to check to need to enabled debug
    bool hasEffectNodeInParent_ = false;

    // used for force prepare current subtree this frame
    bool isCurSubTreeForcePrepare_ = false;
};

class RSSubTreePrepareController {
public:
    /**
     * @brief Constructor with force-prepare state reference and trigger condition
     * @param isCurSubTreeForcePrepare
     *        Reference of the RSUniRenderVisitor::isCurSubTreeForcePrepare_, depends current subtree need force-prepare
     * @param condition Trigger condition for enabling force-prepare
     */
    RSSubTreePrepareController(bool& isCurSubTreeForcePrepare, bool condition);
    /**
     * @brief RAII resource release, restore RSUniRenderVisitor::isCurSubTreeForcePrepare_ to false
     */
    ~RSSubTreePrepareController();

    RSSubTreePrepareController() = delete;
    RSSubTreePrepareController(const RSSubTreePrepareController& other) = delete;
    RSSubTreePrepareController(RSSubTreePrepareController&& other) = delete;
    RSSubTreePrepareController& operator=(const RSSubTreePrepareController& other) = delete;
    RSSubTreePrepareController& operator=(RSSubTreePrepareController&& other) = delete;

private:
    bool& isSubTreeForcePrepare_;
    bool condition_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_VISITOR_H
