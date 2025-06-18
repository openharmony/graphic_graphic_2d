/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H

#include <climits>
#include <functional>
#include <limits>
#include <memory>
#include <tuple>
#include <queue>

#include "surface_type.h"

#include "common/rs_macros.h"
#include "common/rs_occlusion_region.h"
#include "common/rs_special_layer_manager.h"
#include "common/rs_vector4.h"
#include "display_engine/rs_luminance_control.h"
#include "ipc_callbacks/buffer_available_callback.h"
#include "ipc_callbacks/buffer_clear_callback.h"
#include "memory/rs_memory_track.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_surface_ext.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties_painter.h"
#include "screen_manager/screen_types.h"
#include "transaction/rs_occlusion_data.h"

#ifndef ROSEN_CROSS_PLATFORM
#include "surface_buffer.h"
#include "sync_fence.h"
#endif
#include "ipc_security/rs_ipc_interface_code_access_verifier_base.h"
#ifdef ENABLE_FULL_SCREEN_RECONGNIZE
#include "monitor/aps_monitor_impl.h"
#endif

namespace OHOS {
namespace Rosen {
class OcclusionNode;
class RSCommand;
class RSDirtyRegionManager;
class RSOcclusionHandler;
class RSSurfaceHandler;

// Used for control-level occlusion culling scene info and culled nodes transmission.
// Maintained by the Dirty Regions feature team.
class RSB_EXPORT OcclusionParams {
public:
    // Enables/Disables control-level occlusion culling for the node's subtree
    void UpdateOcclusionCullingStatus(bool enable, NodeId keyOcclusionNodeId);

    bool IsOcclusionCullingOn()
    {
        return !keyOcclusionNodeIds_.empty();
    }

    void SetOcclusionHandler(std::shared_ptr<RSOcclusionHandler> occlusionHandler)
    {
        occlusionHandler_ = occlusionHandler;
    }

    std::shared_ptr<RSOcclusionHandler> GetOcclusionHandler() const
    {
        return occlusionHandler_;
    }

    std::unordered_set<NodeId> TakeCulledNodes()
    {
        return std::move(culledNodes_);
    }

    void SetCulledNodes(std::unordered_set<NodeId>&& culledNodes)
    {
        culledNodes_ = std::move(culledNodes);
    }

    std::unordered_set<NodeId> TakeCulledEntireSubtree()
    {
        return std::move(culledEntireSubtree_);
    }

    void SetCulledEntireSubtree(std::unordered_set<NodeId>&& culledEntireSubtree)
    {
        culledEntireSubtree_ = std::move(culledEntireSubtree);
    }

    void CheckKeyOcclusionNodeValidity(
        const std::unordered_map<NodeId, std::shared_ptr<OcclusionNode>>& occlusionNodes);
private:
    std::unordered_multiset<NodeId> keyOcclusionNodeIds_;
    std::shared_ptr<RSOcclusionHandler> occlusionHandler_;
    std::unordered_set<NodeId> culledNodes_;
    std::unordered_set<NodeId> culledEntireSubtree_;
};

class RSB_EXPORT RSSurfaceRenderNode : public RSRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSSurfaceRenderNode>;
    using SharedPtr = std::shared_ptr<RSSurfaceRenderNode>;
    using TreeStateChangeCallback = std::function<void(RSSurfaceRenderNode&)>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::SURFACE_NODE;
    RSRenderNodeType GetType() const override
    {
        return Type;
    }

    ~RSSurfaceRenderNode() override;

    void PrepareRenderBeforeChildren(RSPaintFilterCanvas& canvas);
    void PrepareRenderAfterChildren(RSPaintFilterCanvas& canvas);

    void SetIsOnTheTree(bool onTree, NodeId instanceRootNodeId = INVALID_NODEID,
        NodeId firstLevelNodeId = INVALID_NODEID, NodeId cacheNodeId = INVALID_NODEID,
        NodeId uifirstRootNodeId = INVALID_NODEID, NodeId displayNodeId = INVALID_NODEID) override;
    bool IsAppWindow() const
    {
        return nodeType_ == RSSurfaceNodeType::APP_WINDOW_NODE;
    }

    bool IsStartingWindow() const
    {
        return nodeType_ == RSSurfaceNodeType::STARTING_WINDOW_NODE;
    }

    void ResetRenderParams();

    ScreenId GetScreenId() const
    {
        return screenId_;
    }

    bool IsAbilityComponent() const
    {
        return nodeType_ == RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    }

    bool IsLeashWindow() const
    {
        return nodeType_ == RSSurfaceNodeType::LEASH_WINDOW_NODE;
    }

    bool IsRosenWeb() const
    {
        return GetName().find("RosenWeb") != std::string::npos;
    }

    bool IsSubHighPriorityType() const
    {
        return GetName().find("hipreview") != std::string::npos;
    }

    bool IsScbScreen() const
    {
        return nodeType_ == RSSurfaceNodeType::SCB_SCREEN_NODE;
    }

    void SetNodeDirty(bool isNodeDirty)
    {
        isNodeDirty_ = isNodeDirty || isNodeDirtyInLastFrame_;
        isNodeDirtyInLastFrame_ = isNodeDirty;
    }

    bool IsNodeDirty() const
    {
        return isNodeDirty_;
    }

    bool IsHardwareEnabledTopSurface() const;
    void SetHardCursorStatus(bool status);
    bool GetHardCursorStatus() const;
    bool GetHardCursorLastStatus() const;

    void SetLayerTop(bool isTop);

    bool IsLayerTop() const
    {
        return isLayerTop_;
    }

    void SetForceRefresh(bool isForceRefresh);

    bool IsForceRefresh() const
    {
        return isForceRefresh_;
    }

    void SetHardwareEnableHint(bool enable)
    {
        if (isHardwareEnableHint_ == enable) {
            return;
        }
        isHardwareEnableHint_ = enable;
        SetContentDirty();
    }

    bool IsHardwareEnableHint() const
    {
        return isHardwareEnableHint_;
    }

    void SetSourceDisplayRenderNodeId(NodeId nodeId)
    {
        sourceDisplayRenderNodeId_ = nodeId;
    }

    NodeId GetSourceDisplayRenderNodeId() const
    {
        return sourceDisplayRenderNodeId_;
    }

    void SetSourceDisplayRenderNodeDrawable(DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr drawable);

    void SetExistTransparentHardwareEnabledNode(bool exist)
    {
        existTransparentHardwareEnabledNode_ = exist;
    }
    
    bool ExistTransparentHardwareEnabledNode() const
    {
        return existTransparentHardwareEnabledNode_;
    }

    // indicate if this node type can enable hardware composer
    bool IsHardwareEnabledType() const
    {
        return (nodeType_ == RSSurfaceNodeType::SELF_DRAWING_NODE && isHardwareEnabledNode_) ||
            IsLayerTop();
    }

    void SetPreSubHighPriorityType(bool priorityType);

    bool IsDynamicHardwareEnable() const
    {
        return dynamicHardwareEnable_;
    }
    void SetHardwareEnabled(bool isEnabled, SelfDrawingNodeType selfDrawingType = SelfDrawingNodeType::DEFAULT,
        bool dynamicHardwareEnable = true)
    {
        isHardwareEnabledNode_ = isEnabled;
        selfDrawingType_ = selfDrawingType;
        dynamicHardwareEnable_ = dynamicHardwareEnable;
    }

    void SetForceHardwareAndFixRotation(bool flag);
    bool GetFixRotationByUser() const;
    bool IsInFixedRotation() const;
    void SetInFixedRotation(bool isRotating);

    SelfDrawingNodeType GetSelfDrawingNodeType() const
    {
        return selfDrawingType_;
    }

    bool NeedBilinearInterpolation() const
    {
        return nodeType_ == RSSurfaceNodeType::SELF_DRAWING_NODE && isHardwareEnabledNode_;
    }

    void SetSubNodeShouldPaint()
    {
        hasSubNodeShouldPaint_ = true;
    }

    void ResetSubNodeShouldPaint()
    {
        hasSubNodeShouldPaint_ = false;
    }

    bool HasSubNodeShouldPaint() const
    {
        return hasSubNodeShouldPaint_;
    }

#ifndef ROSEN_CROSS_PLATFORM
    void UpdateBufferInfo(const sptr<SurfaceBuffer>& buffer, const Rect& damageRect,
        const sptr<SyncFence>& acquireFence, const sptr<SurfaceBuffer>& preBuffer);
#endif

    bool IsLastFrameHardwareEnabled() const
    {
        return isLastFrameHardwareEnabled_;
    }

    bool IsCurrentFrameHardwareEnabled() const
    {
        return isCurrentFrameHardwareEnabled_;
    }

    void SetCurrentFrameHardwareEnabled(bool enable)
    {
        isCurrentFrameHardwareEnabled_ = enable;
    }

    void MarkCurrentFrameHardwareEnabled()
    {
        isCurrentFrameHardwareEnabled_ = true;
    }

    void SetIsRotating(bool isRotating)
    {
        isRotating_ = isRotating;
    }

    bool IsRotating() const
    {
        return isRotating_;
    }

    void ResetRotateState()
    {
        isRotating_ = false;
    }

    void ResetCurrentFrameHardwareEnabledState()
    {
        isLastFrameHardwareEnabled_ = isCurrentFrameHardwareEnabled_;
        isCurrentFrameHardwareEnabled_ = false;
    }

    void ResetHardwareEnabledStates()
    {
        isLastFrameHardwareEnabled_ = false;
        isCurrentFrameHardwareEnabled_ = false;
    }

    void SetHardwareForcedDisabledState(bool forcesDisabled)
    {
        isHardwareForcedDisabled_ = forcesDisabled;
    }

    void SetNodeHasBackgroundColorAlpha(bool forcesDisabled)
    {
        isHardwareForcedByBackgroundAlpha_ = forcesDisabled;
    }

    bool IsNodeHasBackgroundColorAlpha() const
    {
        return isHardwareForcedByBackgroundAlpha_;
    }

    void SetHardwareForcedDisabledStateByFilter(bool forcesDisabled)
    {
        isHardwareForcedDisabledByFilter_ = forcesDisabled;
    }

    void ResetMakeImageState()
    {
        intersectWithAIBar_ = false;
        hardwareNeedMakeImage_ = false;
    }

    void SetHardwareNeedMakeImage(bool needMakeImage)
    {
        hardwareNeedMakeImage_ = needMakeImage;
    }

    bool IsHardwareNeedMakeImage() const
    {
        if (specialLayerManager_.Find(SpecialLayerType::PROTECTED)) {
            return false;
        }
        return hardwareNeedMakeImage_;
    }

    void SetIntersectWithAIBar(bool flag)
    {
        intersectWithAIBar_ = flag;
    }

    bool GetIntersectWithAIBar() const
    {
        return intersectWithAIBar_;
    }

    bool IsHardwareForcedDisabledByFilter() const
    {
        return isHardwareForcedDisabledByFilter_;
    }

    void ResetHardwareForcedDisabledBySrcRect()
    {
        isHardwareForcedDisabledBySrcRect_ = false;
    }

    bool IsHardwareForcedDisabled() const
    {
        if (GetTunnelLayerId()) {
            return false;
        }
        // a protected node not on the tree need to release buffer when producer produce buffers
        // release buffer in ReleaseSelfDrawingNodeBuffer function
        // isForcedClipHole: for tv product, force clip hole in tvplayer
        if ((specialLayerManager_.Find(SpecialLayerType::PROTECTED) || isForcedClipHole()) && IsOnTheTree()) {
            constexpr float DRM_MIN_ALPHA = 0.1f;
            return GetGlobalAlpha() < DRM_MIN_ALPHA; // if alpha less than 0.1, drm layer display black background.
        }
        return isHardwareForcedDisabled_;
    }

    bool IsLeashOrMainWindow() const
    {
        return nodeType_ <= RSSurfaceNodeType::LEASH_WINDOW_NODE || nodeType_ == RSSurfaceNodeType::CURSOR_NODE ||
               nodeType_ == RSSurfaceNodeType::ABILITY_MAGNIFICATION_NODE;
    }

    bool IsMainWindowType() const
    {
        // a mainWindowType surfacenode will not mounted under another mainWindowType surfacenode
        // including app main window, starting window, and selfdrawing window
        return nodeType_ <= RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE ||
               nodeType_ == RSSurfaceNodeType::CURSOR_NODE ||
               nodeType_ == RSSurfaceNodeType::ABILITY_MAGNIFICATION_NODE;
    }

    bool GetIsLastFrameHwcEnabled() const
    {
        return isLastFrameHwcEnabled_;
    }

    void SetIsLastFrameHwcEnabled(bool enable)
    {
        isLastFrameHwcEnabled_ = enable;
    }

    bool GetNeedCollectHwcNode() const
    {
        return needCollectHwcNode_;
    }

    void SetNeedCollectHwcNode(bool needCollect)
    {
        needCollectHwcNode_ = needCollect;
    }

    void ResetNeedCollectHwcNode()
    {
        needCollectHwcNode_ = false;
    }

    bool GetCalcRectInPrepare() const
    {
        return calcRectInPrepare_;
    }

    void SetCalcRectInPrepare(bool calc)
    {
        calcRectInPrepare_ = calc;
    }

    bool IsSelfDrawingType() const
    {
        // self drawing surfacenode has its own buffer, and rendered in its own progress/thread
        // such as surfaceview (web/videos) and self draw windows (such as mouse pointer and boot animation)
        return nodeType_ == RSSurfaceNodeType::SELF_DRAWING_NODE ||
                nodeType_ == RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE ||
                nodeType_ == RSSurfaceNodeType::CURSOR_NODE;
    }

    bool IsUIFirstSelfDrawCheck();
    void UpdateCacheSurfaceDirtyManager(int bufferAge = 2);

    bool GetNeedSubmitSubThread() const
    {
        return isNeedSubmitSubThread_;
    }

    void SetNeedSubmitSubThread(bool needSubmitSubThread)
    {
        isNeedSubmitSubThread_ = needSubmitSubThread;
    }

    RSSurfaceNodeType GetSurfaceNodeType() const
    {
        return nodeType_;
    }

    void SetSurfaceNodeType(RSSurfaceNodeType nodeType);

    void MarkUIHidden(bool isHidden);
    bool IsUIHidden() const;

    bool IsLeashWindowSurfaceNodeVisible();

    const std::string& GetName() const
    {
        return name_;
    }

    const std::string& GetBundleName() const
    {
        return bundleName_;
    }

    void SetOffSetX(int32_t offset)
    {
        offsetX_ = offset;
    }

    enum SurfaceWindowType GetSurfaceWindowType() const
    {
        return surfaceWindowType_;
    }

    int32_t GetOffSetX() const
    {
        return offsetX_;
    }

    void SetOffSetY(int32_t offset)
    {
        offsetY_ = offset;
    }

    int32_t GetOffSetY() const
    {
        return offsetY_;
    }

    void SetOffset(int32_t offsetX, int32_t offsetY)
    {
        offsetX_ = offsetX;
        offsetY_ = offsetY;
    }

    bool GetArsrTag() const
    {
        return arsrTag_;
    }

    void SetArsrTag(bool arsrTag)
    {
        arsrTag_ = arsrTag;
    }

    bool GetCopybitTag() const
    {
        return copybitTag_;
    }

    void SetCopybitTag(bool copybitTag)
    {
        copybitTag_ = copybitTag;
    }

    void CollectSurface(const std::shared_ptr<RSBaseRenderNode>& node, std::vector<RSBaseRenderNode::SharedPtr>& vec,
        bool isUniRender, bool onlyFirstLevel) override;
    void CollectSelfDrawingChild(const std::shared_ptr<RSBaseRenderNode>& node, std::vector<NodeId>& vec) override;
    void QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    // keep specified nodetype preparation
    virtual bool IsSubTreeNeedPrepare(bool filterInGloba, bool isOccluded = false) override;
    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    void ProcessTransitionBeforeChildren(RSPaintFilterCanvas& canvas) override {}
    void ProcessAnimatePropertyBeforeChildren(RSPaintFilterCanvas& canvas, bool includeProperty) override;
    void ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas) override;

    void ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas) override {}
    void ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas) override;
    void ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas) override;
    bool IsSCBNode() const;
    void UpdateHwcNodeLayerInfo(GraphicTransformType transform, bool isHardCursorEnable = false);
    void UpdateHardwareDisabledState(bool disabled);
    void SetHwcChildrenDisabledState();

    void SetContextBounds(const Vector4f bounds);
    bool CheckParticipateInOcclusion();

    void OnApplyModifiers() override;

    void SetTotalMatrix(const Drawing::Matrix& totalMatrix);
    const Drawing::Matrix& GetTotalMatrix() const
    {
        return totalMatrix_;
    }

    // Transfer the rendering context variables (matrix, alpha, and clipRegion) from the
    // source node (in the render thread) to the
    // target node (in the render service). Note that:
    // - All three variables are relative to their parent node.
    // - Alpha can be processed as an absolute value, as its parent (surface) node's alpha should always be 1.0f.
    // - The matrix and clipRegion should be applied according to the parent node's matrix.
    void SetContextMatrix(const std::optional<Drawing::Matrix>& transform, bool sendMsg = true);
    void SetContextAlpha(float alpha, bool sendMsg = true);
    void SetContextClipRegion(const std::optional<Drawing::Rect>& clipRegion, bool sendMsg = true);
    std::optional<Drawing::Rect> GetContextClipRegion() const override;

    void SetBootAnimation(bool isBootAnimation) override;
    bool GetBootAnimation() const override;

    void SetGlobalPositionEnabled(bool isEnabled);
    bool GetGlobalPositionEnabled() const override;

    void SetHwcGlobalPositionEnabled(bool isEnabled);
    bool GetHwcGlobalPositionEnabled() const;

    void SetHwcCrossNode(bool isCrossNode);
    bool IsDRMCrossNode() const;

    void SetSecurityLayer(bool isSecurityLayer);
    void SetLeashPersistentId(uint64_t leashPersistentId);
    void SetSkipLayer(bool isSkipLayer);
    void SetSnapshotSkipLayer(bool isSnapshotSkipLayer);
    void SetProtectedLayer(bool isProtectedLayer);
    void SetIsOutOfScreen(bool isOutOfScreen);
    void UpdateBlackListStatus(ScreenId virtualScreenId, bool isBlackList);

    // get whether it is a security/skip layer itself
    LeashPersistentId GetLeashPersistentId() const;

    // set ability state that surfaceNode belongs to as foreground or background
    void SetAbilityState(RSSurfaceNodeAbilityState abilityState);
    RSSurfaceNodeAbilityState GetAbilityState() const override;

    // get whether it and it's subtree contain security layer
    bool GetHasPrivacyContentLayer() const;

    void ResetSpecialLayerChangedFlag()
    {
        specialLayerChanged_ = false;
    }

    bool IsSpecialLayerChanged() const
    {
        return specialLayerChanged_;
    }

    void UpdateSpecialLayerInfoByTypeChange(uint32_t type, bool isSpecialLayer);
    void UpdateSpecialLayerInfoByOnTreeStateChange();
    void SyncBlackListInfoToFirstLevelNode();
    void SyncPrivacyContentInfoToFirstLevelNode();
    void SyncColorGamutInfoToFirstLevelNode();

    void SetFingerprint(bool hasFingerprint);
    bool GetFingerprint() const;

    // [Attention] The function only used for unlocking screen for PC currently
    void SetClonedNodeRenderDrawable(DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr clonedNodeRenderDrawable);
    bool IsCloneNode() const;
    void SetClonedNodeInfo(NodeId id, bool needOffscreen);
    void SetIsCloned(bool isCloned);
    void SetIsClonedNodeOnTheTree(bool isOnTheTree);

    void SetForceUIFirst(bool forceUIFirst);
    bool GetForceUIFirst() const;

    bool GetForceDrawWithSkipped() const;
    void SetForceDrawWithSkipped(bool GetForceDrawWithSkipped);

    void SetUIFirstIsPurge(bool IsPurge)
    {
        UIFirstIsPurge_ = IsPurge;
    }

    bool GetUIFirstIsPurge() const
    {
        return UIFirstIsPurge_;
    }
    void SetUifirstUseStarting(NodeId id); // only cache app window, first frame not wait

    void SetForceUIFirstChanged(bool forceUIFirstChanged);
    bool GetForceUIFirstChanged();

    // Unified DSS synthesis switch for Anco nodes
    static void SetAncoForceDoDirect(bool direct);
    // Obtain whether the Anco node is using the DSS synthesis flag
    static bool GetOriAncoForceDoDirect();
    // Whether to use DSS synthesis to obtain anco nodes. only anco node can be true.
    bool GetAncoForceDoDirect() const;
    // Used to distinguish whether this node is an Anco node
    void SetAncoFlags(uint32_t flags);
    // Determine whether it is an Anco node
    uint32_t GetAncoFlags() const;
    // Set the buffer srcRect of the anco node. Only used on anco nodes.
    void SetAncoSrcCrop(const Rect& srcCrop);

    void SetHDRPresent(bool hasHdrPresent);
    bool GetHDRPresent() const;

    void IncreaseHDRNum(HDRComponentType hdrType);
    void ReduceHDRNum(HDRComponentType hdrType);

    bool GetIsWideColorGamut() const;

    void IncreaseWideColorGamutNum();
    void ReduceWideColorGamutNum();

    bool IsHdrEffectColorGamut() const;

    const std::shared_ptr<RSDirtyRegionManager>& GetDirtyManager() const;
    std::shared_ptr<RSDirtyRegionManager> GetCacheSurfaceDirtyManager() const;

    void SetSrcRect(const RectI& rect)
    {
        srcRect_ = rect;
    }

    void NeedClearBufferCache(std::set<uint32_t>& bufferCacheSet);

    void NeedClearPreBuffer(std::set<uint32_t>& bufferCacheSet);

    const RectI& GetSrcRect() const
    {
        return srcRect_;
    }

    void SetDstRect(const RectI& dstRect)
    {
        if (dstRect_ != dstRect) {
            dstRectChanged_ = true;
        }
        dstRect_ = dstRect;
    }

    const RectI& GetDstRect() const
    {
        return dstRect_;
    }

    void SetDstRectWithoutRenderFit(const RectI& rect)
    {
        dstRectWithoutRenderFit_ = Drawing::Rect(rect.left_, rect.top_, rect.GetRight(), rect.GetBottom());
    }

    void SetRegionToBeMagnified(Vector4f regionToBeMagnified)
    {
        regionToBeMagnified_ = regionToBeMagnified;
    }

    Drawing::Rect GetDstRectWithoutRenderFit() const
    {
        return dstRectWithoutRenderFit_;
    }

    const RectI& GetOriginalDstRect() const
    {
        return originalDstRect_;
    }

    const RectI& GetOriginalSrcRect() const
    {
        return originalSrcRect_;
    }

    Occlusion::Region& GetTransparentRegion()
    {
        return transparentRegion_;
    }

    const Occlusion::Region& GetRoundedCornerRegion() const
    {
        return roundedCornerRegion_;
    }

    const Occlusion::Region& GetOpaqueRegion() const
    {
        return opaqueRegion_;
    }

    Occlusion::Region& GetOpaqueRegion()
    {
        return opaqueRegion_;
    }

    Occlusion::Region& GetContainerRegion()
    {
        return containerRegion_;
    }

    void OnAlphaChanged() override {
        alphaChanged_ = true;
    }

    void SetStencilVal(int64_t stencilVal);

    void SetOcclusionVisible(bool visible);

    bool GetOcclusionVisible() const
    {
        return isOcclusionVisible_;
    }

    void SetOcclusionVisibleWithoutFilter(bool visible)
    {
        isOcclusionVisibleWithoutFilter_ = visible;
    }

    void SetOcclusionInSpecificScenes(bool isOcclusionInSpecificScenes)
    {
        isOcclusionInSpecificScenes_ = isOcclusionInSpecificScenes;
    }

    bool GetOcclusionInSpecificScenes() const
    {
        return isOcclusionInSpecificScenes_;
    }

    const Occlusion::Region& GetVisibleRegion() const
    {
        return visibleRegion_;
    }

    const Occlusion::Region& GetVisibleRegionBehindWindow() const
    {
        return visibleRegionBehindWindow_;
    }

    const Occlusion::Region& GetVisibleRegionInVirtual() const
    {
        return visibleRegionInVirtual_;
    }

    const Occlusion::Region& GetVisibleRegionForCallBack() const
    {
        return visibleRegionForCallBack_;
    }

    void SetAbilityBGAlpha(uint8_t alpha)
    {
        alphaChanged_ = (alpha == 255 && abilityBgAlpha_ != 255) ||
            (alpha != 255 && abilityBgAlpha_ == 255);
        abilityBgAlpha_ = alpha;
    }

    uint8_t GetAbilityBgAlpha() const
    {
        return abilityBgAlpha_;
    }

    void setQosCal(bool qosPidCal)
    {
        qosPidCal_ = qosPidCal;
    }

    bool IsSurfaceInStartingWindowStage() const;

    WINDOW_LAYER_INFO_TYPE GetVisibleLevelForWMS(RSVisibleLevel visibleLevel);

    void SetVisibleRegionRecursive(
        const Occlusion::Region& region,
        VisibleData& visibleVec,
        std::map<NodeId, RSVisibleLevel>& pidVisMap,
        bool needSetVisibleRegion = true,
        RSVisibleLevel visibleLevel = RSVisibleLevel::RS_UNKNOW_VISIBLE_LEVEL,
        bool isSystemAnimatedScenes = false);

    void SetLeashWindowVisibleRegionEmpty(bool isLeashWindowVisibleRegionEmpty)
    {
        if (!IsLeashWindow()) {
            return;
        }
        isLeashWindowVisibleRegionEmpty_ = isLeashWindowVisibleRegionEmpty;
        SetLeashWindowVisibleRegionEmptyParam();
    }

    bool GetLeashWindowVisibleRegionEmpty() const
    {
        return isLeashWindowVisibleRegionEmpty_;
    }

    void SetLeashWindowVisibleRegionEmptyParam();

    void SetExtraDirtyRegionAfterAlignment(const Occlusion::Region& region)
    {
        extraDirtyRegionAfterAlignment_ = region;
        extraDirtyRegionAfterAlignmentIsEmpty_ = extraDirtyRegionAfterAlignment_.IsEmpty();
    }

    bool GetDstRectChanged() const
    {
        return dstRectChanged_;
    }

    void CleanDstRectChanged()
    {
        dstRectChanged_ = false;
    }

    bool GetAlphaChanged() const
    {
        return alphaChanged_;
    }

    void CleanAlphaChanged()
    {
        alphaChanged_ = false;
    }

    void SetLocalZOrder(float localZOrder);
    float GetLocalZOrder() const;

    void SetColorSpace(GraphicColorGamut colorSpace);
    GraphicColorGamut GetColorSpace() const;
    // Only call this if the node is first level node.
    GraphicColorGamut GetFirstLevelNodeColorGamut() const;
    void SetFirstLevelNodeColorGamutByResource(bool changeToP3);
    void SetFirstLevelNodeColorGamutByWindow(bool changeToP3);

    // Only call this if the node is self-drawing surface node.
    void UpdateColorSpaceWithMetadata();

#ifndef ROSEN_CROSS_PLATFORM
    void SetConsumer(const sptr<IConsumerSurface>& consumer);
    void SetBlendType(GraphicBlendType blendType);
    GraphicBlendType GetBlendType();
#endif

    void UpdateSurfaceDefaultSize(float width, float height);

    void UpdateInfoForClonedNode(NodeId nodeId);

    // Only SurfaceNode in RS calls "RegisterBufferAvailableListener"
    // to save callback method sent by RT or UI which depends on the value of "isFromRenderThread".
    void RegisterBufferAvailableListener(sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread);

    void RegisterBufferClearListener(sptr<RSIBufferClearCallback> callback);

    // Only SurfaceNode in RT calls "ConnectToNodeInRenderService" to send callback method to RS
    void ConnectToNodeInRenderService();

    void NotifyRTBufferAvailable(bool isTextureExportNode = false);
    bool IsNotifyRTBufferAvailable() const;
    bool IsNotifyRTBufferAvailablePre() const;

    void NotifyUIBufferAvailable();
    bool IsNotifyUIBufferAvailable() const;
    void SetIsNotifyUIBufferAvailable(bool available);

    // UI Thread would not be notified when SurfaceNode created by Video/Camera in RenderService has available buffer.
    // And RenderThread does not call mainFunc_ if nothing in UI thread is changed
    // which would cause callback for "clip" on parent SurfaceNode cannot be triggered
    // for "clip" is executed in RenderThreadVisitor::ProcessSurfaceRenderNode.
    // To fix this bug, we set callback which would call RSRenderThread::RequestNextVSync() to forcedly "refresh"
    // RenderThread when SurfaceNode in RenderService has available buffer and execute RSIBufferAvailableCallback.
    void SetCallbackForRenderThreadRefresh(bool isRefresh);
    bool NeedSetCallbackForRenderThreadRefresh();

    void ParallelVisitLock()
    {
        parallelVisitMutex_.lock();
    }

    void ParallelVisitUnlock()
    {
        parallelVisitMutex_.unlock();
    }

    bool SubNodeVisible(const RectI& r) const
    {
        Occlusion::Rect nodeRect { r.left_, r.top_, r.GetRight(), r.GetBottom() };
        // if current node is in occluded region of the surface, it could be skipped in process step
        return visibleRegion_.IsIntersectWith(nodeRect);
    }

    bool CheckIfOcclusionReusable(std::queue<NodeId>& surfaceNodesIds) const;
    bool CheckIfOcclusionChanged() const;

    void SetVisibleRegion(const Occlusion::Region& region)
    {
        visibleRegion_ = region;
    }

    void SetVisibleRegionBehindWindow(const Occlusion::Region& region)
    {
        visibleRegionBehindWindow_ = region;
    }

    void SetVisibleRegionInVirtual(const Occlusion::Region& region)
    {
        visibleRegionInVirtual_ = region;
    }

    inline bool IsEmptyAppWindow() const
    {
        return IsAppWindow() && (GetChildrenCount() == 0 || HasOnlyOneRootNode());
    }

    // Due to the BehindWindowFilter enabling ui-first, the condition is excluded.
    // This condition is now only used by ui-first
    inline bool IsAlphaTransparent() const
    {
        const uint8_t opacity = 255;
        return !(GetAbilityBgAlpha() == opacity && ROSEN_EQ(GetGlobalAlpha(), 1.0f)) ||
            (IsEmptyAppWindow() && RSUniRenderJudgement::IsUniRender());
    }

    inline bool IsTransparent() const
    {
        return IsAlphaTransparent() || NeedDrawBehindWindow();
    }

    inline bool IsCurrentNodeInTransparentRegion(const Occlusion::Rect& nodeRect) const
    {
        return transparentRegion_.IsIntersectWith(nodeRect);
    }

    // Used when the node is opaque, but not calculate in occlusion
    void SetTreatedAsTransparent(bool isOcclusion)
    {
        isTreatedAsTransparent_ = isOcclusion;
    }

    bool IsTreatedAsTransparent() const
    {
        return isTreatedAsTransparent_;
    }

    bool GetZorderChanged() const
    {
        return zOrderChanged_;
    }

    bool IsZOrderPromoted() const
    {
        return GetRenderProperties().GetPositionZ() > positionZ_;
    }

    void UpdatePositionZ()
    {
        const auto& zProperties = GetRenderProperties().GetPositionZ();
        zOrderChanged_ = !ROSEN_EQ(zProperties, positionZ_);
        positionZ_ = zProperties;
    }

    inline bool HasContainerWindow() const
    {
        return containerConfig_.hasContainerWindow_;
    }

    void SetContainerWindow(bool hasContainerWindow, RRect rrect);

    std::string GetContainerConfigDump() const
    {
        return "[outR: " + std::to_string(containerConfig_.outR_) +
               " inR: " + std::to_string(containerConfig_.inR_) +
               " x: " + std::to_string(containerConfig_.innerRect_.left_) +
               " y: " + std::to_string(containerConfig_.innerRect_.top_) +
               " w: " + std::to_string(containerConfig_.innerRect_.width_) +
               " h: " + std::to_string(containerConfig_.innerRect_.height_) + "]";
    }

    bool IsOpaqueRegionChanged() const
    {
        return opaqueRegionChanged_;
    }

    void CleanOpaqueRegionChanged()
    {
        opaqueRegionChanged_ = false;
    }

    bool IsBehindWindowOcclusionChanged() const
    {
        return behindWindowOcclusionChanged_;
    }

    // [planning] Remove this after skia is upgraded, the clipRegion is supported
    void ResetChildrenFilterRects();
    void UpdateChildrenFilterRects(std::shared_ptr<RSRenderNode> filternode, const RectI& rect, bool cacheValid);
    const std::vector<RectI>& GetChildrenNeedFilterRects() const;
    const std::vector<bool>& GetChildrenNeedFilterRectsCacheValid() const;
    const std::vector<std::shared_ptr<RSRenderNode>>& GetChildrenFilterNodes() const;
    std::vector<RectI> GetChildrenNeedFilterRectsWithoutCacheValid();

    // manage abilities' nodeid info
    void UpdateAbilityNodeIds(NodeId id, bool isAdded);
    const std::unordered_set<NodeId>& GetAbilityNodeIds() const;
    void AddAbilityComponentNodeIds(std::unordered_set<NodeId>& nodeIds);
    void ResetAbilityNodeIds();

    // manage appWindowNode's child hardware enabled nodes info
    void ResetChildHardwareEnabledNodes();
    void AddChildHardwareEnabledNode(WeakPtr childNode);
    const std::vector<WeakPtr>& GetChildHardwareEnabledNodes() const;

    bool IsFocusedNode(uint64_t focusedNodeId)
    {
        return GetId() == focusedNodeId;
    }

    void CheckAndUpdateOpaqueRegion(const RectI& screeninfo, const ScreenRotation screenRotation,
        const bool isFocusWindow);

    void ResetSurfaceOpaqueRegion(const RectI& screeninfo, const RectI& absRect, const ScreenRotation screenRotation,
        const bool isFocusWindow, const Vector4<int>& cornerRadius);
    Occlusion::Region ResetOpaqueRegion(
        const RectI& absRect, const ScreenRotation screenRotation, const bool isFocusWindow) const;
    Occlusion::Region SetUnfocusedWindowOpaqueRegion(const RectI& absRect, const ScreenRotation screenRotation) const;
    Occlusion::Region SetFocusedWindowOpaqueRegion(const RectI& absRect, const ScreenRotation screenRotation) const;
    void SetCornerRadiusOpaqueRegion(const RectI& absRect, const Vector4<int>& cornerRadius);
    void ResetSurfaceContainerRegion(const RectI& screeninfo, const RectI& absRect,
        const ScreenRotation screenRotation);
    bool CheckOpaqueRegionBaseInfo(const RectI& screeninfo, const RectI& absRect, const ScreenRotation screenRotation,
        const bool isFocusWindow, const Vector4<int>& cornerRadius) const;
    void SetOpaqueRegionBaseInfo(const RectI& screeninfo, const RectI& absRect, const ScreenRotation screenRotation,
        const bool isFocusWindow, const Vector4<int>& cornerRadius);
    void DealWithDrawBehindWindowTransparentRegion();

    bool IsStartAnimationFinished() const;
    void SetStartAnimationFinished();
    // if surfacenode's buffer has been consumed, it should be set dirty
    bool UpdateDirtyIfFrameBufferConsumed();

    void UpdateSrcRect(const Drawing::Canvas& canvas, const Drawing::RectI& dstRect, bool hasRotation = false);

    // if a surfacenode's dstrect is empty, its subnodes' prepare stage can be skipped
    bool ShouldPrepareSubnodes();

    void SetNodeCost(int32_t cost)
    {
        nodeCost_ = cost;
    }

    int32_t GetNodeCost() const
    {
        return nodeCost_;
    }

    std::string DirtyRegionDump() const;
    void SetAnimateState() {
        animateState_ = true;
    }
    void ResetAnimateState() {
        animateState_ = false;
    }
    bool GetAnimateState() const
    {
        return animateState_;
    }
    bool IsParentLeashWindowInScale() const;

    Occlusion::Rect GetSurfaceOcclusionRect(bool isUniRender);

    void AccumulateOcclusionRegion(Occlusion::Region& accumulatedRegion,
        Occlusion::Region& curRegion,
        bool& hasFilterCacheOcclusion,
        bool isUniRender,
        bool filterCacheOcclusionEnabled);

    bool LeashWindowRelatedAppWindowOccluded(std::vector<std::shared_ptr<RSSurfaceRenderNode>>& appNode);

    void FindScreenId();

    void OnTreeStateChanged() override;

    void SetDrawingGPUContext(Drawing::GPUContext* grContext)
    {
        grContext_ = grContext;
    }
    // UIFirst
    void UpdateUIFirstFrameGravity();

    void SetSubmittedSubThreadIndex(uint32_t index)
    {
        submittedSubThreadIndex_ = index;
    }

    uint32_t GetSubmittedSubThreadIndex() const
    {
        return submittedSubThreadIndex_;
    }

    bool IsWaitUifirstFirstFrame() const;
    void SetWaitUifirstFirstFrame(bool wait);

    void SetCacheSurfaceProcessedStatus(CacheProcessStatus cacheProcessStatus);
    CacheProcessStatus GetCacheSurfaceProcessedStatus() const;

    void RegisterTreeStateChangeCallback(TreeStateChangeCallback callback);
    void NotifyTreeStateChange();

    /* For filter cache occlusion calculation */
    bool GetFilterCacheFullyCovered() const
    {
        return isFilterCacheFullyCovered_;
    }

    void SetFilterCacheFullyCovered(bool val)
    {
        isFilterCacheFullyCovered_ = val;
    }

    bool GetFilterCacheValidForOcclusion() const
    {
        return isFilterCacheValidForOcclusion_;
    }

    // mark if any valid filter cache within surface fully cover targer range
    void CheckValidFilterCacheFullyCoverTarget(const RSRenderNode& filterNode, const RectI& targetRect);
    void CalcFilterCacheValidForOcclusion();
    // mark occluded by upper filtercache
    void UpdateOccludedByFilterCache(bool val);
    bool IsOccludedByFilterCache() const;

    bool IsFilterCacheStatusChanged() const
    {
        return isFilterCacheStatusChanged_;
    }

    void ResetFilterNodes()
    {
        filterNodes_.clear();
    }
    void UpdateFilterNodes(const std::shared_ptr<RSRenderNode>& nodePtr);
    // update static node's back&front-ground filter cache status
    void UpdateFilterCacheStatusWithVisible(bool visible);
    void UpdateFilterCacheStatusIfNodeStatic(const RectI& clipRect, bool isRotationChanged);
    void UpdateDrawingCacheNodes(const std::shared_ptr<RSRenderNode>& nodePtr);
    // reset static node's drawing cache status as not changed and get filter rects
    void ResetDrawingCacheStatusIfNodeStatic(std::unordered_map<NodeId, std::unordered_set<NodeId>>& allRects);

    void SetNotifyRTBufferAvailable(bool isNotifyRTBufferAvailable);

    // whether the subtree has only one root node
    bool HasOnlyOneRootNode() const;

    bool GetHwcDelayDirtyFlag() const noexcept
    {
        return hwcDelayDirtyFlag_;
    }

    void SetHwcDelayDirtyFlag(bool hwcDelayDirtyFlag)
    {
        hwcDelayDirtyFlag_ = hwcDelayDirtyFlag;
    }

    bool GetSurfaceCacheContentStatic()
    {
        return surfaceCacheContentStatic_;
    }

    bool GetUifirstContentDirty()
    {
        bool uifirstContentDirty = uifirstContentDirty_;
        uifirstContentDirty_ = false;
        return uifirstContentDirty;
    }

    void UpdateSurfaceCacheContentStatic();

    void UpdateSurfaceCacheContentStatic(
        const std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>>& activeNodeIds);
    // temperory limit situation:
    // subtree no drawingcache and geodirty
    // contentdirty 1 specifically for buffer update
    bool IsContentDirtyNodeLimited() const
    {
        return drawingCacheNodes_.empty() && dirtyGeoNodeNum_ == 0 && dirtyContentNodeNum_ <= 1;
    }

    size_t GetLastFrameChildrenCnt()
    {
        return lastFrameChildrenCnt_;
    }

    void SetLastFrameChildrenCnt(size_t childrenCnt)
    {
        lastFrameChildrenCnt_ = childrenCnt;
    }

    bool GetUifirstSupportFlag() override
    {
        return RSRenderNode::GetUifirstSupportFlag();
    }

    bool OpincGetNodeSupportFlag() override
    {
        return false;
    }

    void UpdateSurfaceCacheContentStaticFlag(bool isAccessibilityChanged);

    void UpdateSurfaceSubTreeDirtyFlag();

    void MergeOldDirtyRect() override
    {
        if (IsAppWindow()) {
            this->GetDirtyManager()->MergeDirtyRect(this->GetOldDirtyInSurface());
        }
    }

#ifdef USE_SURFACE_TEXTURE
    std::shared_ptr<RSSurfaceTexture> GetSurfaceTexture() const { return surfaceTexture_; };
    void SetSurfaceTexture(const std::shared_ptr<RSSurfaceTexture> &texture) { surfaceTexture_ = texture; }
#endif

    void SetForeground(bool isForeground)
    {
        isForeground_ = isForeground;
    }

    void SetSurfaceId(SurfaceId surfaceId)
    {
        surfaceId_ = surfaceId;
    }

    SurfaceId GetSurfaceId() const
    {
        return surfaceId_;
    }

    void SetTunnelLayerId(SurfaceId tunnelLayerId)
    {
        tunnelLayerId_ = tunnelLayerId;
    }

    SurfaceId GetTunnelLayerId() const
    {
        return tunnelLayerId_;
    }

    bool GetIsForeground() const
    {
        return isForeground_;
    }
    bool GetNodeIsSingleFrameComposer() const override;

    void SetAncestorDisplayNode(const RSBaseRenderNode::WeakPtr& ancestorDisplayNode)
    {
        ancestorDisplayNode_ = ancestorDisplayNode;
    }

    bool SetUifirstNodeEnableParam(MultiThreadCacheType b);

    void SetIsParentUifirstNodeEnableParam(bool b);

    void SetUifirstChildrenDirtyRectParam(RectI rect);

    void SetUifirstStartTime(int64_t startTime)
    {
        uifirstStartTime_ = startTime;
    }

    int64_t GetUifirstStartTime() const
    {
        return uifirstStartTime_;
    }

    RSBaseRenderNode::WeakPtr GetAncestorDisplayNode() const
    {
        return ancestorDisplayNode_;
    }
    bool QuerySubAssignable(bool isRotation);
    bool QueryIfAllHwcChildrenForceDisabledByFilter();
    bool GetHasSharedTransitionNode() const;
    void SetHasSharedTransitionNode(bool hasSharedTransitionNode);
    Vector2f GetGravityTranslate(float imgWidth, float imgHeight);
    void UpdateTransparentSurface();
    bool GetHasTransparentSurface() const;
    void UpdatePartialRenderParams();
    // This function is used for extending visibleRegion by dirty blurfilter node half-obscured
    void UpdateExtendVisibleRegion(Occlusion::Region& region);
    void UpdateAncestorDisplayNodeInRenderParams();

    void SetNeedDrawFocusChange(bool needDrawFocusChange)
    {
        needDrawFocusChange_ = needDrawFocusChange;
    }

    bool GetNeedDrawFocusChange() const
    {
        return needDrawFocusChange_;
    }

    bool HasWindowCorner()
    {
        Vector4f cornerRadius;
        Vector4f::Max(GetWindowCornerRadius(), GetGlobalCornerRadius(), cornerRadius);
        return !cornerRadius.IsZero();
    }
    void SetBufferRelMatrix(const Drawing::Matrix& matrix)
    {
        bufferRelMatrix_ = matrix;
    }

    const Drawing::Matrix& GetBufferRelMatrix() const
    {
        return bufferRelMatrix_;
    }

    void SetGpuOverDrawBufferOptimizeNode(bool overDrawNode)
    {
        isGpuOverDrawBufferOptimizeNode_ = overDrawNode;
    }
    bool IsGpuOverDrawBufferOptimizeNode() const
    {
        return isGpuOverDrawBufferOptimizeNode_;
    }

    void SetOverDrawBufferNodeCornerRadius(const Vector4f& radius)
    {
        overDrawBufferNodeCornerRadius_ = radius;
    }
    const Vector4f& GetOverDrawBufferNodeCornerRadius() const
    {
        return overDrawBufferNodeCornerRadius_;
    }

    bool HasSubSurfaceNodes() const;
    void SetIsSubSurfaceNode(bool isSubSurfaceNode);
    bool IsSubSurfaceNode() const;
    const std::map<NodeId, RSSurfaceRenderNode::WeakPtr>& GetChildSubSurfaceNodes() const;
    void GetAllSubSurfaceNodes(std::vector<std::pair<NodeId, RSSurfaceRenderNode::WeakPtr>>& allSubSurfaceNodes) const;
    std::string SubSurfaceNodesDump() const;

    void SetDoDirectComposition(bool flag)
    {
        doDirectComposition_ = flag;
    }

    bool GetDoDirectComposition() const
    {
        return doDirectComposition_;
    }

    void SetSkipDraw(bool skip);
    bool GetSkipDraw() const;
    void SetHidePrivacyContent(bool needHidePrivacyContent);
    void SetNeedOffscreen(bool needOffscreen);
    void SetSdrNit(float sdrNit);
    void SetDisplayNit(float displayNit);
    void SetColorFollow(bool colorFollow);
    void SetBrightnessRatio(float brightnessRatio);
    void SetLayerLinearMatrix(const std::vector<float>& layerLinearMatrix);
    void SetSdrHasMetadata(bool hasMetadata);
    bool GetSdrHasMetadata() const;
    static const std::unordered_map<NodeId, NodeId>& GetSecUIExtensionNodes();
    bool IsSecureUIExtension() const
    {
        return nodeType_ == RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE;
    }

    bool IsUnobscuredUIExtensionNode() const
    {
        return nodeType_ == RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE && GetUIExtensionUnobscured();
    }

    bool IsUIExtension() const
    {
        return nodeType_ == RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE ||
               nodeType_ == RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE;
    }

    void SetCornerRadiusInfoForDRM(const std::vector<float>& drmCornerRadius);
    void SetForceDisableClipHoleForDRM(bool isForceDisable);
    const std::vector<float>& GetCornerRadiusInfoForDRM() const
    {
        return drmCornerRadiusInfo_;
    }

    // [Attention] The function only used for unlocking screen for PC currently
    NodeId GetClonedNodeId() const
    {
        return clonedSourceNodeId_;
    }

    const std::shared_ptr<RSSurfaceHandler> GetRSSurfaceHandler() const
    {
        return surfaceHandler_;
    }

    std::shared_ptr<RSSurfaceHandler> GetMutableRSSurfaceHandler()
    {
        return surfaceHandler_;
    }

    void CheckContainerDirtyStatusAndUpdateDirty(bool& containerDirty)
    {
        if (!containerDirty || !IsLeashOrMainWindow()) {
            return;
        }
        dirtyStatus_ = NodeDirty::DIRTY;
        containerDirty = false;
    }

    void SetWatermarkEnabled(const std::string& name, bool isEnabled);
    const std::unordered_map<std::string, bool>& GetWatermark() const;
    bool IsWatermarkEmpty() const;

    template<class... Args>
    void SetIntersectedRoundCornerAABBs(Args&& ...args)
    {
        std::vector<RectI>(std::forward<Args>(args)...).swap(intersectedRoundCornerAABBs_);
    }

    const std::vector<RectI>& GetIntersectedRoundCornerAABBs() const
    {
        return intersectedRoundCornerAABBs_;
    }

    size_t GetIntersectedRoundCornerAABBsSize() const
    {
        return intersectedRoundCornerAABBs_.size();
    }

    void SetNeedCacheSurface(bool needCacheSurface);
    bool GetSubThreadAssignable() const
    {
        return subThreadAssignable_;
    }
    void SetSubThreadAssignable(bool subThreadAssignable)
    {
        subThreadAssignable_ = subThreadAssignable;
    }
    RSSpecialLayerManager& GetMultableSpecialLayerMgr()
    {
        return specialLayerManager_;
    }

    const RSSpecialLayerManager& GetSpecialLayerMgr() const
    {
        return specialLayerManager_;
    }

    bool NeedUpdateDrawableBehindWindow() const override;
    void SetOldNeedDrawBehindWindow(bool val);
    bool NeedDrawBehindWindow() const override;
    void AddChildBlurBehindWindow(NodeId id) override;
    void RemoveChildBlurBehindWindow(NodeId id) override;
    void CalDrawBehindWindowRegion() override;
    RectI GetFilterRect() const override;
    RectI GetBehindWindowRegion() const override;
    void UpdateCrossNodeSkipDisplayConversionMatrices(NodeId displayId, const Drawing::Matrix& matrix)
    {
        crossNodeSkipDisplayConversionMatrices_[displayId] = matrix;
    }
    const Drawing::Matrix& GetCrossNodeSkipDisplayConversionMatrix(NodeId displayId)
    {
        return crossNodeSkipDisplayConversionMatrices_[displayId];
    }
    void ClearCrossNodeSkipDisplayConversionMatrices()
    {
        crossNodeSkipDisplayConversionMatrices_.clear();
    }
    HdrStatus GetVideoHdrStatus() const
    {
        return hdrVideoSurface_;
    }

    void SetVideoHdrStatus(HdrStatus hasHdrVideoSurface)
    {
        hdrVideoSurface_ = hasHdrVideoSurface;
    }

    void SetApiCompatibleVersion(uint32_t apiCompatibleVersion);
    uint32_t GetApiCompatibleVersion()
    {
        return apiCompatibleVersion_;
    }

    void ResetIsBufferFlushed();

    void ResetSurfaceNodeStates();

    bool IsUIBufferAvailable();

    bool GetUIExtensionUnobscured() const;

    std::shared_ptr<RSDirtyRegionManager>& GetDirtyManagerForUifirst()
    {
        return dirtyManager_;
    }

    // [Attention] Used in uifirst for checking whether node and parent should paint or not
    void SetSelfAndParentShouldPaint(bool selfAndParentShouldPaint)
    {
        selfAndParentShouldPaint_ = selfAndParentShouldPaint;
    }

    bool GetSelfAndParentShouldPaint() const
    {
        return selfAndParentShouldPaint_;
    }

    inline bool IsHardwareDisabledBySrcRect() const
    {
        return isHardwareForcedDisabledBySrcRect_;
    }

    void SetAppWindowZOrder(int32_t appWindowZOrder)
    {
        appWindowZOrder_ = appWindowZOrder;
    }

    int32_t GetAppWindowZOrder() const
    {
        return appWindowZOrder_;
    }

    // Enable HWCompose
    RSHwcSurfaceRecorder& HwcSurfaceRecorder() { return hwcSurfaceRecorder_; }

    void SetFrameGravityNewVersionEnabled(bool isEnabled);
    bool GetFrameGravityNewVersionEnabled() const;

    // Used for control-level occlusion culling scene info and culled nodes transmission.
    std::shared_ptr<OcclusionParams> GetOcclusionParams()
    {
        if (occlusionParams_ == nullptr) {
            occlusionParams_ = std::make_shared<OcclusionParams>();
        }
        return occlusionParams_;
    }

protected:
    void OnSync() override;
    void OnSkipSync() override;
    // rotate corner by rotation degreee. Every 90 degrees clockwise rotation, the vector
    // of corner radius loops one element to the right
    void RotateCorner(int rotationDegree, Vector4<int>& cornerRadius) const;

private:
    explicit RSSurfaceRenderNode(NodeId id, const std::weak_ptr<RSContext>& context = {},
        bool isTextureExportNode = false);
    explicit RSSurfaceRenderNode(const RSSurfaceRenderNodeConfig& config, const std::weak_ptr<RSContext>& context = {});
    void OnResetParent() override;
    void ClearChildrenCache();
    Vector4f GetWindowCornerRadius();
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> GetLeashWindowNestedSurfaces();
    bool IsHistoryOccludedDirtyRegionNeedSubmit();
    void ClearHistoryUnSubmittedDirtyInfo();
    void UpdateHistoryUnsubmittedDirtyInfo();
    void SetUIExtensionUnobscured(bool obscured);
    void OnSubSurfaceChanged();
    void UpdateChildSubSurfaceNodes(RSSurfaceRenderNode::SharedPtr node, bool isOnTheTree);
    bool IsYUVBufferFormat() const;
    void InitRenderParams() override;
    void UpdateRenderParams() override;
    void UpdateChildHardwareEnabledNode(NodeId id, bool isOnTree);
    std::unordered_set<NodeId> GetAllSubSurfaceNodeIds() const;
    bool IsCurFrameSwitchToPaint();

    bool isForcedClipHole() const;

#ifdef ENABLE_FULL_SCREEN_RECONGNIZE
    void SendSurfaceNodeTreeStatus(bool onTree);
    void SendSurfaceNodeBoundChange();
#endif
#ifndef ROSEN_CROSS_PLATFORM
    void UpdatePropertyFromConsumer();
#endif

    RSSpecialLayerManager specialLayerManager_;
    bool specialLayerChanged_ = false;
    std::unordered_map<ScreenId, std::unordered_set<NodeId>> blackListIds_ = {};
    bool isGlobalPositionEnabled_ = false;
    bool isHwcGlobalPositionEnabled_ = false;
    bool isHwcCrossNode_ = false;
    bool hasFingerprint_ = false;
    // hdr video
    HdrStatus hdrVideoSurface_ = HdrStatus::NO_HDR;
    bool zOrderChanged_ = false;
    bool qosPidCal_ = false;
    RSSurfaceNodeAbilityState abilityState_ = RSSurfaceNodeAbilityState::FOREGROUND;
    RSSurfaceNodeType nodeType_ = RSSurfaceNodeType::DEFAULT;
    bool isLayerTop_ = false;
    bool isForceRefresh_ = false; // the self-drawing node need force refresh
    // Specifying hardware enable is only a 'hint' to RS that
    // the self-drawing node use hardware composer in some condition,
    // such as transparent background.
    bool isHardwareEnableHint_ = false;
    NodeId sourceDisplayRenderNodeId_ = INVALID_NODEID;
    const enum SurfaceWindowType surfaceWindowType_ = SurfaceWindowType::DEFAULT_WINDOW;
    bool isNotifyRTBufferAvailablePre_ = false;
    bool isRefresh_ = false;
    bool isLeashWindowVisibleRegionEmpty_ = false;
    bool isOcclusionVisible_ = true;
    bool isOcclusionVisibleWithoutFilter_ = true;
    bool isOcclusionInSpecificScenes_ = false;
    bool dstRectChanged_ = false;
    uint8_t abilityBgAlpha_ = 0;
    bool alphaChanged_ = false;
    bool isUIHidden_ = false;
    bool extraDirtyRegionAfterAlignmentIsEmpty_ = true;
    bool opaqueRegionChanged_ = false;
    bool behindWindowOcclusionChanged_ = false;
    bool isFilterCacheFullyCovered_ = false;
    bool isFilterCacheValidForOcclusion_ = false;
    bool isOccludedByFilterCache_ = false;
    bool isFilterCacheStatusChanged_ = false;
    bool isTreatedAsTransparent_ = false;
    bool startAnimationFinished_ = false;
    // only used in hardware enabled pointer window, when gpu -> hardware composer
    bool isNodeDirtyInLastFrame_ = true;
    bool isNodeDirty_ = true;
    // used for hardware enabled nodes
    bool isHardwareEnabledNode_ = false;
    bool dynamicHardwareEnable_ = true;
    bool isFixRotationByUser_ = false;
    bool isInFixedRotation_ = false;
    SelfDrawingNodeType selfDrawingType_ = SelfDrawingNodeType::DEFAULT;
    bool isCurrentFrameHardwareEnabled_ = false;
    bool isLastFrameHardwareEnabled_ = false;
    bool isLastFrameHwcEnabled_ = false;
    bool needCollectHwcNode_ = false;
    bool intersectByFilterInApp_ = false;
    bool calcRectInPrepare_ = false;
    bool hasSubNodeShouldPaint_ = false;
    // mark if this self-drawing node is forced not to use hardware composer
    // in case where this node's parent window node is occluded or is appFreeze, this variable will be marked true
    bool isHardwareForcedDisabled_ = false;
    bool hardwareNeedMakeImage_ = false;
    bool intersectWithAIBar_ = false;
    bool isHardwareForcedDisabledByFilter_ = false;
    // For certain buffer format(YUV), dss restriction on src : srcRect % 2 == 0
    // To avoid switch between gpu and dss during sliding, we disable dss when srcHeight != bufferHeight
    bool isHardwareForcedDisabledBySrcRect_ = false;
    // Mark if the leash or main window node has transparent self-drawing node
    bool existTransparentHardwareEnabledNode_ = false;
    bool animateState_ = false;
    bool isRotating_ = false;
    bool isParentScaling_ = false;
    bool needDrawAnimateProperty_ = false;
    bool prevVisible_ = false;

    // mark if this self-drawing node do not consume buffer when gpu -> hwc
    bool hwcDelayDirtyFlag_ = false;
    bool isForeground_ = false;
    bool UIFirstIsPurge_ = false;
    bool isTargetUIFirstDfxEnabled_ = false;
    bool hasSharedTransitionNode_ = false;
    bool lastFrameShouldPaint_ = true;
    // node only have translate and scale changes
    bool surfaceCacheContentStatic_ = false;
    bool uifirstContentDirty_ = false;
    // point window
    bool isHardCursor_ = false;
    bool isLastHardCursor_ = false;
    bool needDrawFocusChange_ = false;
    bool forceUIFirstChanged_ = false;
    bool forceUIFirst_ = false;
    bool uifirstForceDrawWithSkipped_ = false;
    bool hasTransparentSurface_ = false;
    bool isGpuOverDrawBufferOptimizeNode_ = false;
    bool isSubSurfaceNode_ = false;
    bool doDirectComposition_ = true;
    bool isSkipDraw_ = false;
    bool needHidePrivacyContent_ = false;
    bool isHardwareForcedByBackgroundAlpha_ = false;
    bool arsrTag_ = true;
    bool copybitTag_ = false;
    bool subThreadAssignable_ = false;
    bool oldNeedDrawBehindWindow_ = false;
    RectI skipFrameDirtyRect_;
    bool UIExtensionUnobscured_ = false;
    std::atomic<bool> isNotifyRTBufferAvailable_ = false;
    std::atomic<bool> isNotifyUIBufferAvailable_ = true;
    std::atomic_bool isBufferAvailable_ = false;
    std::atomic<CacheProcessStatus> cacheProcessStatus_ = CacheProcessStatus::WAITING;
    std::atomic<bool> isNeedSubmitSubThread_ = true;
    // whether to wait uifirst first frame finished when buffer available callback invoked.
    std::atomic<bool> isWaitUifirstFirstFrame_ = false;
    std::atomic<bool> hasUnSubmittedOccludedDirtyRegion_ = false;
    static inline std::atomic<bool> ancoForceDoDirect_ = false;
    float contextAlpha_ = 1.0f;
    // Count the number of hdr pictures. If hdrPhotoNum_ > 0, it means there are hdr pictures
    int hdrPhotoNum_ = 0;
    // Count the number of hdr UI components. If hdrUIComponentNum_ > 0, it means there are hdr UI components
    int hdrUIComponentNum_ = 0;
    int hdrEffectNum_ = 0;
    int wideColorGamutNum_ = 0;
    int32_t offsetX_ = 0;
    int32_t offsetY_ = 0;
    int64_t stencilVal_ = -1;
    float positionZ_ = 0.0f;
    // This variable can be set in two cases:
    // 1. The upper-layer IPC interface directly sets window colorspace.
    // 2. If it is a self-drawing node, the colorspace will be refreshed after hardware-enable calculation.
    GraphicColorGamut colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
#ifndef ROSEN_CROSS_PLATFORM
    GraphicBlendType blendType_ = GraphicBlendType::GRAPHIC_BLEND_SRCOVER;
#endif
    // hdr
    int32_t displayNit_ = 500; // default sdr luminance
    float brightnessRatio_ = 1.0f; // no ratio by default
    float localZOrder_ = 0.0f;
    uint32_t processZOrder_ = -1;
    int32_t nodeCost_ = 0;
    uint32_t submittedSubThreadIndex_ = INT_MAX;
    uint32_t wideColorGamutWindowCount_ = 0;
    uint32_t wideColorGamutResourceWindowCount_ = 0;
    uint32_t apiCompatibleVersion_ = 0;
    std::atomic<uint32_t> ancoFlags_ = 0;
    Drawing::GPUContext* grContext_ = nullptr;
    ScreenId screenId_ = INVALID_SCREEN_ID;
    SurfaceId surfaceId_ = 0;
    SurfaceId tunnelLayerId_ = 0;
    uint64_t leashPersistentId_ = INVALID_LEASH_PERSISTENTID;
    size_t dirtyContentNodeNum_ = 0;
    size_t dirtyGeoNodeNum_ = 0;
    size_t dirtynodeNum_ = 0;
    // UIFirst
    int64_t uifirstStartTime_ = -1;
    size_t lastFrameChildrenCnt_ = 0;
    sptr<RSIBufferAvailableCallback> callbackFromRT_ = nullptr;
    sptr<RSIBufferAvailableCallback> callbackFromUI_ = nullptr;
    sptr<RSIBufferClearCallback> clearBufferCallback_ = nullptr;
    std::shared_ptr<RSDirtyRegionManager> dirtyManager_ = nullptr;
    std::shared_ptr<RSDirtyRegionManager> cacheSurfaceDirtyManager_ = nullptr;
    std::shared_ptr<RSSurfaceHandler> surfaceHandler_;
#ifdef USE_SURFACE_TEXTURE
    std::shared_ptr<RSSurfaceTexture> surfaceTexture_ {};
#endif
    RSBaseRenderNode::WeakPtr ancestorDisplayNode_;
    RectI clipRegionFromParent_;
    RectI dstRect_;
    RectI srcRect_;
    RectI originalDstRect_;
    RectI originalSrcRect_;
    Vector4f regionToBeMagnified_;
    Drawing::Rect dstRectWithoutRenderFit_;
    RectI historyUnSubmittedOccludedDirtyRegion_;
    Vector4f overDrawBufferNodeCornerRadius_;
    RectI drawBehindWindowRegion_;

    std::mutex mutexRT_;
    std::mutex mutexUI_;
    std::mutex mutexClear_;
    std::mutex mutex_;
    std::mutex mutexHDR_;
    std::mutex parallelVisitMutex_;
    std::optional<Drawing::Matrix> contextMatrix_;
    std::optional<Drawing::Rect> contextClipRect_;

    std::set<NodeId> privacyContentLayerIds_ = {};
    Drawing::Matrix totalMatrix_;
    std::vector<RectI> intersectedRoundCornerAABBs_;
    std::vector<float> drmCornerRadiusInfo_;

    std::string name_;
    std::string bundleName_;
    std::vector<NodeId> childSurfaceNodeIds_;
    friend class RSRenderThreadVisitor;
    /*
        visibleRegion: appwindow visible region after occlusion, used for rs opdrop and other optimization.
        visibleRegionForCallBack: appwindow visible region after occlusion (no filtercache occlusion), used in
    windowmanager, and web surfacenode visibility callback.
        These two values are the same in most cases. If there are filter cache occlusion, this two values will be
    different under filter cache surfacenode layer.
    */
    Occlusion::Region visibleRegion_;
    Occlusion::Region visibleRegionBehindWindow_;
    Occlusion::Region extendVisibleRegion_;
    Occlusion::Region visibleRegionInVirtual_;
    Occlusion::Region visibleRegionForCallBack_;

    // dirtyRegion caused by surfaceNode visible region after alignment
    Occlusion::Region extraDirtyRegionAfterAlignment_;

    // region of rounded corner
    Occlusion::Region roundedCornerRegion_;
    // opaque region of the surface
    Occlusion::Region opaqueRegion_;
    // [planning] Remove this after skia is upgraded, the clipRegion is supported
    std::vector<RectI> childrenFilterRects_;
    std::vector<bool> childrenFilterRectsCacheValid_;
    std::vector<std::shared_ptr<RSRenderNode>> childrenFilterNodes_;
    // transparent region of the surface, floating window's container window is always treated as transparent
    Occlusion::Region transparentRegion_;
    Occlusion::Region occlusionRegionBehindWindow_;

    Occlusion::Region containerRegion_;
    std::unordered_set<NodeId> abilityNodeIds_;
    // valid filter nodes within, including itself
    std::vector<std::shared_ptr<RSRenderNode>> filterNodes_;
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> drawingCacheNodes_;
    int32_t appWindowZOrder_ = 0;

    // Enable HWCompose
    RSHwcSurfaceRecorder hwcSurfaceRecorder_;

    // previous self-Drawing Node Bound
#ifdef ENABLE_FULL_SCREEN_RECONGNIZE
    float prevSelfDrawHeight_ = 0.0f;
    float prevSelfDrawWidth_ = 0.0f;
#endif

    /*
        ContainerWindow configs acquired from arkui, including container window state, screen density, container border
        width, padding width, inner/outer radius, etc.
    */
    class ContainerConfig {
    public:
        // rrect means content region, including padding to left and top, inner radius;
        void Update(bool hasContainer, RRect rrect);

        bool operator==(const ContainerConfig& config) const
        {
            return hasContainerWindow_ == config.hasContainerWindow_ &&
                outR_ == config.outR_ &&
                inR_ == config.inR_ &&
                innerRect_ == config.innerRect_;
        }
    private:
        inline int RoundFloor(float length)
        {
            // if a float value is very close to a integer (< 0.05f), return round value
            return std::abs(length - std::round(length)) < 0.05f ? std::round(length) : std::floor(length);
        }
    public:
        bool hasContainerWindow_ = false;                // set to false as default, set by arkui
        int outR_ = 0;                                   // outer radius (int value)
        int inR_ = 0;                                    // inner radius (int value)
        RectI innerRect_ = {};                           // inner rect, value relative to outerRect
    };

    ContainerConfig containerConfig_;
    ContainerConfig GetAbsContainerConfig() const;

    struct OpaqueRegionBaseInfo
    {
        RectI screenRect_;
        RectI absRect_;
        RectI oldDirty_;
        ScreenRotation screenRotation_ = ScreenRotation::INVALID_SCREEN_ROTATION;
        bool isFocusWindow_ = false;
        bool isTransparent_ = false;
        bool hasContainerWindow_ = false;
        Vector4<int> cornerRadius_;
        ContainerConfig containerConfig_;
        bool needDrawBehindWindow_ = false;
        RectI absDrawBehindWindowRegion_ = RectI();
    };

    //<screenRect, absRect, screenRotation, isFocusWindow, isTransparent, hasContainerWindow>
    OpaqueRegionBaseInfo opaqueRegionBaseInfo_;

    std::vector<WeakPtr> childHardwareEnabledNodes_;

    TreeStateChangeCallback treeStateChangeCallback_;
    Drawing::Matrix bufferRelMatrix_ = Drawing::Matrix();

    // [Attention] The variable only used for unlocking screen for PC currently
    bool isCloneNode_ = false;
    NodeId clonedSourceNodeId_ = INVALID_NODEID;
    bool isClonedNodeOnTheTree_ = false;
    bool clonedSourceNodeNeedOffscreen_ = true;

    std::map<NodeId, RSSurfaceRenderNode::WeakPtr> childSubSurfaceNodes_;
    std::unordered_map<std::string, bool> watermarkHandles_ = {};
    std::unordered_set<NodeId> childrenBlurBehindWindow_ = {};
    std::unordered_map<NodeId, Drawing::Matrix> crossNodeSkipDisplayConversionMatrices_ = {};

    // used in uifirst for checking whether node and parents should paint or not
    bool selfAndParentShouldPaint_ = true;

    bool isFrameGravityNewVersionEnabled_ = false;

    // Used for control-level occlusion culling scene info and culled nodes transmission.
    std::shared_ptr<OcclusionParams> occlusionParams_ = nullptr;

    // UIExtension record, <UIExtension, hostAPP>
    inline static std::unordered_map<NodeId, NodeId> secUIExtensionNodes_ = {};
    friend class SurfaceNodeCommandHelper;
    friend class RSUifirstManager;
    friend class RSUniRenderVisitor;
    friend class RSRenderNode;
    friend class RSRenderService;
    friend class RSHdrUtil;
#ifdef RS_PROFILER_ENABLED
    friend class RSProfiler;
#endif
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H
