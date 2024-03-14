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

#include "memory/rs_memory_track.h"

#include "common/rs_macros.h"
#include "common/rs_occlusion_region.h"
#include "common/rs_vector4.h"
#include "ipc_callbacks/buffer_available_callback.h"
#include "ipc_callbacks/buffer_clear_callback.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_system_properties.h"
#include "platform/common/rs_surface_ext.h"
#include "property/rs_properties_painter.h"
#include "screen_manager/screen_types.h"
#include "surface_type.h"
#include "transaction/rs_occlusion_data.h"


namespace OHOS {
namespace Rosen {
class RSCommand;
class RSDirtyRegionManager;
class RSB_EXPORT RSSurfaceRenderNode : public RSRenderNode, public RSSurfaceHandler {
public:
    using WeakPtr = std::weak_ptr<RSSurfaceRenderNode>;
    using SharedPtr = std::shared_ptr<RSSurfaceRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::SURFACE_NODE;
    RSRenderNodeType GetType() const override
    {
        return Type;
    }

    explicit RSSurfaceRenderNode(NodeId id, const std::weak_ptr<RSContext>& context = {},
        bool isTextureExportNode = false);
    explicit RSSurfaceRenderNode(const RSSurfaceRenderNodeConfig& config, const std::weak_ptr<RSContext>& context = {});
    ~RSSurfaceRenderNode() override;

    void PrepareRenderBeforeChildren(RSPaintFilterCanvas& canvas);
    void PrepareRenderAfterChildren(RSPaintFilterCanvas& canvas);

    void SetIsOnTheTree(bool flag, NodeId instanceRootNodeId = INVALID_NODEID,
        NodeId firstLevelNodeId = INVALID_NODEID, NodeId cacheNodeId = INVALID_NODEID) override;
    bool IsAppWindow() const
    {
        return nodeType_ == RSSurfaceNodeType::APP_WINDOW_NODE;
    }

    bool IsStartingWindow() const
    {
        return nodeType_ == RSSurfaceNodeType::STARTING_WINDOW_NODE;
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

    bool IsHardwareEnabledTopSurface() const
    {
        return nodeType_ == RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE && GetName() == "pointer window";
    }

    // indicate if this node type can enable hardware composer
    bool IsHardwareEnabledType() const
    {
        if (IsRosenWeb() && !RSSystemProperties::IsPhoneType()) {
            return false;
        }
        return (nodeType_ == RSSurfaceNodeType::SELF_DRAWING_NODE && isHardwareEnabledNode_) ||
            IsHardwareEnabledTopSurface();
    }

    void SetHardwareEnabled(bool isEnabled, SelfDrawingNodeType selfDrawingType = SelfDrawingNodeType::DEFAULT)
    {
        isHardwareEnabledNode_ = isEnabled;
        selfDrawingType_ = selfDrawingType;
    }

    SelfDrawingNodeType GetSelfDrawingNodeType() const
    {
        return selfDrawingType_;
    }

    bool NeedBilinearInterpolation() const
    {
        return nodeType_ == RSSurfaceNodeType::SELF_DRAWING_NODE && isHardwareEnabledNode_ &&
            name_ == "SceneViewer Model0";
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

    bool IsLastFrameHardwareEnabled() const
    {
        return isLastFrameHardwareEnabled_;
    }

    bool IsCurrentFrameHardwareEnabled() const
    {
        return isCurrentFrameHardwareEnabled_;
    }

    void MarkCurrentFrameHardwareEnabled()
    {
        isCurrentFrameHardwareEnabled_ = true;
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

    void SetHardwareDisabledByCache(bool disabledByCache)
    {
        isHardwareDisabledByCache_ = disabledByCache;
    }

    void SetHardwareForcedDisabledStateByFilter(bool forcesDisabled)
    {
        isHardwareForcedDisabledByFilter_ = forcesDisabled;
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
        return isHardwareForcedDisabled_ || isHardwareDisabledByCache_ || isHardwareForcedDisabledBySrcRect_ ||
            GetDstRect().GetWidth() <= 1 || GetDstRect().GetHeight() <= 1; // avoid fallback by composer
    }

    bool IsMainWindowType() const
    {
        // a mainWindowType surfacenode will not mounted under another mainWindowType surfacenode
        // including app main window, starting window, and selfdrawing window
        return nodeType_ == RSSurfaceNodeType::APP_WINDOW_NODE ||
               nodeType_ == RSSurfaceNodeType::DEFAULT ||
               nodeType_ == RSSurfaceNodeType::STARTING_WINDOW_NODE ||
               nodeType_ == RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    }

    bool IsSelfDrawingType() const
    {
        // self drawing surfacenode has its own buffer, and rendered in its own progress/thread
        // such as surfaceview (web/videos) and self draw windows (such as mouse pointer and boot animation)
        return nodeType_ == RSSurfaceNodeType::SELF_DRAWING_NODE ||
               nodeType_ == RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    }

    bool IsUIFirstSelfDrawCheck();
    bool IsVisibleDirtyEmpty(DeviceType deviceType);
    bool IsCurFrameStatic(DeviceType deviceType);
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

    void SetSurfaceNodeType(RSSurfaceNodeType nodeType)
    {
        if (nodeType_ != RSSurfaceNodeType::ABILITY_COMPONENT_NODE) {
            nodeType_ = nodeType;
        }
    }

    void MarkUIHidden(bool isHidden);
    bool IsUIHidden() const;

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

    int32_t GetOffSetX()
    {
        return offsetX_;
    }

    void SetOffSetY(int32_t offset)
    {
        offsetY_ = offset;
    }

    int32_t GetOffSetY()
    {
        return offsetY_;
    }

    void SetOffset(int32_t offsetX, int32_t offsetY)
    {
        offsetX_ = offsetX;
        offsetY_ = offsetY;
    }

    void CollectSurface(const std::shared_ptr<RSBaseRenderNode>& node, std::vector<RSBaseRenderNode::SharedPtr>& vec,
        bool isUniRender, bool onlyFirstLevel) override;
    void CollectSurfaceForUIFirstSwitch(uint32_t& leashWindowCount, uint32_t minNodeNum) override;
    void QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    // keep specified nodetype preparation
    virtual bool IsSubTreeNeedPrepare(bool needMap, bool filterInGloba, bool isOccluded = false) override;
    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    void ProcessTransitionBeforeChildren(RSPaintFilterCanvas& canvas) override {}
    void ProcessAnimatePropertyBeforeChildren(RSPaintFilterCanvas& canvas, bool includeProperty) override;
    void ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas) override;

    void ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas) override {}
    void ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas) override;
    void ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas) override;
    bool IsNeedSetVSync();

    void SetContextBounds(const Vector4f bounds);
    bool CheckParticipateInOcclusion() const;

    void OnApplyModifiers() override;

    void SetTotalMatrix(const Drawing::Matrix& totalMatrix)
    {
        totalMatrix_ = totalMatrix;
    }
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

    void SetSecurityLayer(bool isSecurityLayer);
    bool GetSecurityLayer() const;

    void SetSkipLayer(bool isSkipLayer);
    bool GetSkipLayer() const;

    void SetFingerprint(bool hasFingerprint);
    bool GetFingerprint() const;

    void SetForceUIFirst(bool forceUIFirst);
    bool GetForceUIFirst() const;

    void SetForceUIFirstChanged(bool forceUIFirstChanged);
    bool GetForceUIFirstChanged();

    std::shared_ptr<RSDirtyRegionManager> GetDirtyManager() const;
    std::shared_ptr<RSDirtyRegionManager> GetCacheSurfaceDirtyManager() const;

    void SetSrcRect(const RectI& rect)
    {
        srcRect_ = rect;
    }

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

    Occlusion::Region& GetTransparentRegion()
    {
        return transparentRegion_;
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

    const Occlusion::Region& GetVisibleRegion() const
    {
        return visibleRegion_;
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
        std::map<uint32_t, RSVisibleLevel>& pidVisMap,
        bool needSetVisibleRegion = true,
        RSVisibleLevel visibleLevel = RSVisibleLevel::RS_UNKNOW_VISIBLE_LEVEL,
        bool isSystemAnimatedScenes = false);

    const Occlusion::Region& GetVisibleDirtyRegion() const
    {
        return visibleDirtyRegion_;
    }

    void SetVisibleDirtyRegion(const Occlusion::Region& region)
    {
        visibleDirtyRegion_ = region;
    }

    void SetAlignedVisibleDirtyRegion(const Occlusion::Region& alignedRegion)
    {
        alignedVisibleDirtyRegion_ = alignedRegion;
    }

    const Occlusion::Region& GetAlignedVisibleDirtyRegion()
    {
        return alignedVisibleDirtyRegion_;
    }

    void SetExtraDirtyRegionAfterAlignment(const Occlusion::Region& region)
    {
        extraDirtyRegionAfterAlignment_ = region;
        extraDirtyRegionAfterAlignmentIsEmpty_ = extraDirtyRegionAfterAlignment_.IsEmpty();
    }

    void SetDirtyRegionAlignedEnable(bool enable)
    {
        isDirtyRegionAlignedEnable_ = enable;
    }

    const Occlusion::Region& GetDirtyRegionBelowCurrentLayer() const
    {
        return dirtyRegionBelowCurrentLayer_;
    }

    void SetDirtyRegionBelowCurrentLayer(Occlusion::Region& region)
    {
#ifndef ROSEN_CROSS_PLATFORM
        Occlusion::Rect dirtyRect { GetOldDirtyInSurface() };
        Occlusion::Region dirtyRegion { dirtyRect };
        dirtyRegionBelowCurrentLayer_ = dirtyRegion.And(region);
        dirtyRegionBelowCurrentLayerIsEmpty_ = dirtyRegionBelowCurrentLayer_.IsEmpty();
#endif
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

    void SetGlobalDirtyRegion(const RectI& rect)
    {
        Occlusion::Rect tmpRect { rect.left_, rect.top_, rect.GetRight(), rect.GetBottom() };
        Occlusion::Region region { tmpRect };
        globalDirtyRegion_ = visibleRegion_.And(region);
        globalDirtyRegionIsEmpty_ = globalDirtyRegion_.IsEmpty();
    }

    const Occlusion::Region& GetGlobalDirtyRegion() const
    {
        return globalDirtyRegion_;
    }

    void SetLocalZOrder(float localZOrder);
    float GetLocalZOrder() const;

    void SetColorSpace(GraphicColorGamut colorSpace);
    GraphicColorGamut GetColorSpace() const;
#ifndef ROSEN_CROSS_PLATFORM
    void SetConsumer(const sptr<IConsumerSurface>& consumer);
    void SetBlendType(GraphicBlendType blendType);
    GraphicBlendType GetBlendType();
#endif

    void UpdateSurfaceDefaultSize(float width, float height);

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

    void SetVisibleRegion(Occlusion::Region region)
    {
        visibleRegion_ = region;
    }

    inline bool IsEmptyAppWindow() const
    {
        return IsAppWindow() && (GetChildrenCount() == 0 || HasOnlyOneRootNode());
    }

    inline bool IsTransparent() const
    {
        const uint8_t opacity = 255;
        return !(GetAbilityBgAlpha() == opacity && ROSEN_EQ(GetGlobalAlpha(), 1.0f)) ||
            (IsEmptyAppWindow() && RSUniRenderJudgement::IsUniRender());
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

    bool SubNodeIntersectWithDirty(const RectI& r) const;

    // judge if a rect r is intersect with existing dirtyregion, include current surfacenode's dirtyregion, display
    // dirtyregion, and dirtyregion from other surfacenode because of 32/64 bits alignment.
    bool SubNodeNeedDraw(const RectI& r, PartialRenderType opDropType) const;

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
        zOrderChanged_ = !ROSEN_EQ(GetRenderProperties().GetPositionZ(), positionZ_);
        positionZ_ = GetRenderProperties().GetPositionZ();
    }

    inline bool HasContainerWindow() const
    {
        return containerConfig_.hasContainerWindow_;
    }

    void SetContainerWindow(bool hasContainerWindow, float density)
    {
        containerConfig_.Update(hasContainerWindow, density);
    }

    bool IsOpaqueRegionChanged() const
    {
        return opaqueRegionChanged_;
    }

    void CleanOpaqueRegionChanged()
    {
        opaqueRegionChanged_ = false;
    }

    // [planning] Remove this after skia is upgraded, the clipRegion is supported
    void ResetChildrenFilterRects();
    void UpdateChildrenFilterRects(std::shared_ptr<RSRenderNode> filternode, const RectI& rect, bool cacheValid);
    const std::vector<RectI>& GetChildrenNeedFilterRects() const;
    const std::vector<bool>& GetChildrenNeedFilterRectsCacheValid() const;
    const std::vector<std::shared_ptr<RSRenderNode>>& GetChildrenFilterNodes() const;

    // manage abilities' nodeid info
    void UpdateAbilityNodeIds(NodeId id, bool isAdded);
    const std::unordered_set<NodeId>& GetAbilityNodeIds() const;
    void AddAbilityComponentNodeIds(std::unordered_set<NodeId> nodeIds);
    void ResetAbilityNodeIds();

    // manage appWindowNode's child hardware enabled nodes info
    void ResetChildHardwareEnabledNodes();
    void AddChildHardwareEnabledNode(WeakPtr childNode);
    const std::vector<WeakPtr>& GetChildHardwareEnabledNodes() const;

    bool IsFocusedNode(uint64_t focusedNodeId)
    {
        return GetNodeId() == focusedNodeId;
    }


    void CheckAndUpdateOpaqueRegion(const RectI& screeninfo, const ScreenRotation screenRotation);

    void ResetSurfaceOpaqueRegion(const RectI& screeninfo, const RectI& absRect, const ScreenRotation screenRotation,
        const bool isFocusWindow, const Vector4<int>& cornerRadius);
    Occlusion::Region ResetOpaqueRegion(
        const RectI& absRect, const ScreenRotation screenRotation, const bool isFocusWindow) const;
    Occlusion::Region SetUnfocusedWindowOpaqueRegion(const RectI& absRect, const ScreenRotation screenRotation) const;
    Occlusion::Region SetFocusedWindowOpaqueRegion(const RectI& absRect, const ScreenRotation screenRotation) const;
    Occlusion::Region SetCornerRadiusOpaqueRegion(const RectI& absRect, const Vector4<int>& cornerRadius) const;
    void ResetSurfaceContainerRegion(const RectI& screeninfo, const RectI& absRect,
        const ScreenRotation screenRotation);
    bool CheckOpaqueRegionBaseInfo(const RectI& screeninfo, const RectI& absRect, const ScreenRotation screenRotation,
        const bool isFocusWindow, const Vector4<int>& cornerRadius);
    void SetOpaqueRegionBaseInfo(const RectI& screeninfo, const RectI& absRect, const ScreenRotation screenRotation,
        const bool isFocusWindow, const Vector4<int>& cornerRadius);

    bool IsStartAnimationFinished() const;
    void SetStartAnimationFinished();
    // if surfacenode's buffer has been consumed, it should be set dirty
    bool UpdateDirtyIfFrameBufferConsumed();

    void UpdateSrcRect(const RSPaintFilterCanvas& canvas, const Drawing::RectI& dstRect, bool hasRotation = false);

    // if a surfacenode's dstrect is empty, its subnodes' prepare stage can be skipped
    bool ShouldPrepareSubnodes();
    void StoreMustRenewedInfo() override;

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

    bool LeashWindowRelatedAppWindowOccluded(std::shared_ptr<RSSurfaceRenderNode>& appNode);

    void OnTreeStateChanged() override;

    void SetDrawingGPUContext(Drawing::GPUContext* grContext)
    {
        grContext_ = grContext;
    }
    // UIFirst
    void SetSubmittedSubThreadIndex(uint32_t index)
    {
        submittedSubThreadIndex_ = index;
    }

    uint32_t GetSubmittedSubThreadIndex() const
    {
        return submittedSubThreadIndex_;
    }

    void SetCacheSurfaceProcessedStatus(CacheProcessStatus cacheProcessStatus);
    CacheProcessStatus GetCacheSurfaceProcessedStatus() const;

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

    void CalcFilterCacheValidForOcclusion();

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

    bool GetHasSecurityLayer()
    {
        return hasSecurityLayer_;
    }

    void SetHasSecurityLayer(bool hasSecurityLayer)
    {
        hasSecurityLayer_ = hasSecurityLayer;
    }
    bool GetHasSkipLayer()
    {
        return hasSkipLayer_;
    }

    void SetHasSkipLayer(bool hasSkipLayer)
    {
        hasSkipLayer_ = hasSkipLayer;
    }

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

    bool IsUIFirstCacheReusable(DeviceType deviceType);

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

    bool GetIsForeground() const
    {
        return isForeground_;
    }
    bool GetNodeIsSingleFrameComposer() const override;

    void SetAncestorDisplayNode(const RSBaseRenderNode::WeakPtr& ancestorDisplayNode)
    {
        ancestorDisplayNode_ = ancestorDisplayNode;
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
    bool GetHasTransparentSurface() const;
    void UpdatePartialRenderParams();
    void UpdateAncestorDisplayNodeInRenderParams();

    bool HasWindowCorner()
    {
        Vector4f cornerRadius;
        Vector4f::Max(GetWindowCornerRadius(), GetGlobalCornerRadius(), cornerRadius);
        return !cornerRadius.IsZero();
    }
protected:
    void OnSync() override;
private:
    void OnResetParent() override;
    void ClearChildrenCache();
    bool SubNodeIntersectWithExtraDirtyRegion(const RectI& r) const;
    Vector4f GetWindowCornerRadius();
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> GetLeashWindowNestedSurfaces();
    bool IsHistoryOccludedDirtyRegionNeedSubmit();
    void ClearHistoryUnSubmittedDirtyInfo();
    void UpdateHistoryUnsubmittedDirtyInfo();
    bool IsHardwareDisabledBySrcRect() const;
    bool IsYUVBufferFormat() const;
    void InitRenderParams() override;
    std::mutex mutexRT_;
    std::mutex mutexUI_;
    std::mutex mutexClear_;
    std::mutex mutex_;
    Drawing::GPUContext* grContext_ = nullptr;
    std::mutex parallelVisitMutex_;

    float contextAlpha_ = 1.0f;
    std::optional<Drawing::Matrix> contextMatrix_;
    std::optional<Drawing::Rect> contextClipRect_;

    bool isSecurityLayer_ = false;
    bool isSkipLayer_ = false;
    bool hasFingerprint_ = false;
    RectI srcRect_;
    Drawing::Matrix totalMatrix_;
    int32_t offsetX_ = 0;
    int32_t offsetY_ = 0;
    float positionZ_ = 0.0f;
    bool zOrderChanged_ = false;
    bool qosPidCal_ = false;
    SurfaceId surfaceId_ = 0;

    std::string name_;
    std::string bundleName_;
    RSSurfaceNodeType nodeType_ = RSSurfaceNodeType::DEFAULT;
    GraphicColorGamut colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
#ifndef ROSEN_CROSS_PLATFORM
    GraphicBlendType blendType_ = GraphicBlendType::GRAPHIC_BLEND_SRCOVER;
#endif
    bool isNotifyRTBufferAvailablePre_ = false;
    std::atomic<bool> isNotifyRTBufferAvailable_ = false;
    std::atomic<bool> isNotifyUIBufferAvailable_ = true;
    std::atomic_bool isBufferAvailable_ = false;
    sptr<RSIBufferAvailableCallback> callbackFromRT_;
    sptr<RSIBufferAvailableCallback> callbackFromUI_;
    sptr<RSIBufferClearCallback> clearBufferCallback_;
    bool isRefresh_ = false;
    std::vector<NodeId> childSurfaceNodeIds_;
    friend class RSRenderThreadVisitor;
    RectI clipRegionFromParent_;
    /*
        visibleRegion: appwindow visible region after occlusion, used for rs opdrop and other optimization.
        visibleRegionForCallBack: appwindow visible region after occlusion (no filtercache occlusion), used in
    windowmanager, qos, and web surfacenode visibility callback.
        These two values are the same in most cases. If there are filter cache occlusion, this two values will be
    different under filter cache surfacenode layer.
    */
    Occlusion::Region visibleRegion_;
    Occlusion::Region visibleRegionForCallBack_;
    Occlusion::Region visibleDirtyRegion_;
    bool isDirtyRegionAlignedEnable_ = false;
    Occlusion::Region alignedVisibleDirtyRegion_;
    bool isOcclusionVisible_ = true;
    bool isOcclusionVisibleWithoutFilter_ = true;
    bool isOcclusionInSpecificScenes_ = false;
    std::shared_ptr<RSDirtyRegionManager> dirtyManager_ = nullptr;
    std::shared_ptr<RSDirtyRegionManager> cacheSurfaceDirtyManager_ = nullptr;
    RectI dstRect_;
    bool dstRectChanged_ = false;
    uint8_t abilityBgAlpha_ = 0;
    bool alphaChanged_ = false;
    bool isUIHidden_ = false;
    Occlusion::Region globalDirtyRegion_;
    // dirtyRegion caused by surfaceNode visible region after alignment
    Occlusion::Region extraDirtyRegionAfterAlignment_;
    bool extraDirtyRegionAfterAlignmentIsEmpty_ = true;

    bool globalDirtyRegionIsEmpty_ = false;
    // if a there a dirty layer under transparent clean layer, transparent layer should refreshed
    Occlusion::Region dirtyRegionBelowCurrentLayer_;
    bool dirtyRegionBelowCurrentLayerIsEmpty_ = false;

    // opaque region of the surface
    Occlusion::Region opaqueRegion_;
    bool opaqueRegionChanged_ = false;
    // [planning] Remove this after skia is upgraded, the clipRegion is supported
    std::vector<RectI> childrenFilterRects_;
    std::vector<bool> childrenFilterRectsCacheValid_;
    std::vector<std::shared_ptr<RSRenderNode>> childrenFilterNodes_;
    std::unordered_set<NodeId> abilityNodeIds_;
    size_t dirtyContentNodeNum_ = 0;
    size_t dirtyGeoNodeNum_ = 0;
    size_t dirtynodeNum_ = 0;
    // transparent region of the surface, floating window's container window is always treated as transparent
    Occlusion::Region transparentRegion_;

    Occlusion::Region containerRegion_;
    bool isFilterCacheFullyCovered_ = false;
    bool isFilterCacheValidForOcclusion_ = false;
    bool isFilterCacheStatusChanged_ = false;
    bool isTreatedAsTransparent_ = false;
    // valid filter nodes within, including itself
    std::vector<std::shared_ptr<RSRenderNode>> filterNodes_;
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> drawingCacheNodes_;

    struct OpaqueRegionBaseInfo
    {
        RectI screenRect_;
        RectI absRect_;
        ScreenRotation screenRotation_;
        bool isFocusWindow_;
        bool isTransparent_;
        bool hasContainerWindow_;
        Vector4<int> cornerRadius_;
    };
    
    //<screenRect, absRect, screenRotation, isFocusWindow, isTransparent, hasContainerWindow>
    OpaqueRegionBaseInfo opaqueRegionBaseInfo_;

    /*
        ContainerWindow configs acquired from arkui, including container window state, screen density, container border
        width, padding width, inner/outer radius, etc.
    */
    class ContainerConfig {
    public:
        void Update(bool hasContainer, float density);
    private:
        inline int RoundFloor(float length)
        {
            // if a float value is very close to a integer (< 0.05f), return round value
            return std::abs(length - std::round(length)) < 0.05f ? std::round(length) : std::floor(length);
        }
    public:
        // temporary const value from ACE container_modal_constants.h, will be replaced by uniform interface
        const static int CONTAINER_TITLE_HEIGHT = 37;   // container title height = 37 vp
        const static int CONTENT_PADDING = 4;           // container <--> content distance 4 vp
        const static int CONTAINER_BORDER_WIDTH = 1;    // container border width 2 vp
        const static int CONTAINER_OUTER_RADIUS = 16;   // container outer radius 16 vp
        const static int CONTAINER_INNER_RADIUS = 14;   // container inner radius 14 vp

        bool hasContainerWindow_ = false;               // set to false as default, set by arkui
        float density = 2.0f;                           // The density default value is 2
        int outR = 32;                                  // outer radius (int value)
        int inR = 28;                                   // inner radius (int value)
        int bp = 10;                                    // border width + padding (int value)
        int bt = 76;                                    // border width + title (int value)
    };

    ContainerConfig containerConfig_;

    bool startAnimationFinished_ = false;

    // only used in hardware enabled pointer window, when gpu -> hardware composer
    bool isNodeDirtyInLastFrame_ = true;
    bool isNodeDirty_ = true;
    // used for hardware enabled nodes
    bool isHardwareEnabledNode_ = false;
    SelfDrawingNodeType selfDrawingType_ = SelfDrawingNodeType::DEFAULT;
    bool isCurrentFrameHardwareEnabled_ = false;
    bool isLastFrameHardwareEnabled_ = false;
    bool hasSubNodeShouldPaint_ = false;
    // mark if this self-drawing node is forced not to use hardware composer
    // in case where this node's parent window node is occluded or is appFreeze, this variable will be marked true
    bool isHardwareForcedDisabled_ = false;
    bool isHardwareForcedDisabledByFilter_ = false;
    // For certain buffer format(YUV), dss restriction on src : srcRect % 2 == 0
    // To avoid switch between gpu and dss during sliding, we disable dss when srcHeight != bufferHeight
    bool isHardwareForcedDisabledBySrcRect_ = false;
    bool isHardwareDisabledByCache_ = false;
    float localZOrder_ = 0.0f;
    std::vector<WeakPtr> childHardwareEnabledNodes_;
    int32_t nodeCost_ = 0;

    bool animateState_ = false;

    bool needDrawAnimateProperty_ = false;
    bool prevVisible_ = false;
    bool hasSecurityLayer_ = false;
    bool hasSkipLayer_ = false;

    uint32_t processZOrder_ = -1;

    // mark if this self-drawing node do not consume buffer when gpu -> hwc
    bool hwcDelayDirtyFlag_ = false;

    // UIFirst
    uint32_t submittedSubThreadIndex_ = INT_MAX;
    std::atomic<CacheProcessStatus> cacheProcessStatus_ = CacheProcessStatus::WAITING;
    std::atomic<bool> isNeedSubmitSubThread_ = true;
#ifdef USE_SURFACE_TEXTURE
    std::shared_ptr<RSSurfaceTexture> surfaceTexture_ {};
#endif
    bool isForeground_ = false;

    RSBaseRenderNode::WeakPtr ancestorDisplayNode_;
    bool hasSharedTransitionNode_ = false;
    size_t lastFrameChildrenCnt_ = 0;
    // node only have translate and scale changes
    bool surfaceCacheContentStatic_ = false;

    std::atomic<bool> hasUnSubmittedOccludedDirtyRegion_ = false;
    RectI historyUnSubmittedOccludedDirtyRegion_;
    bool hasTransparentSurface_ = false;
    bool forceUIFirst_ = false;
    bool forceUIFirstChanged_ = false;

    friend class RSUniRenderVisitor;
    friend class RSRenderNode;
    friend class RSRenderService;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H
