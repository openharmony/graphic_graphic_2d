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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_H

#include <atomic>
#include <bitset>
#include <cstdint>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "animation/rs_animation_manager.h"
#include "animation/rs_frame_rate_range.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "draw/surface.h"
#include "drawable/rs_drawable.h"
#include "image/gpu_context.h"
#include "memory/rs_dfx_string.h"
#include "modifier/rs_render_modifier.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_content.h"
#include "pipeline/rs_render_display_sync.h"
#include "pipeline/rs_single_frame_composer.h"
#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DrawCmdList;
}
class RSContext;
class RSNodeVisitor;
class RSCommand;
namespace NativeBufferUtils {
class VulkanCleanupHelper;
}
class RSB_EXPORT RSRenderNode : public std::enable_shared_from_this<RSRenderNode>  {
public:
    using WeakPtr = std::weak_ptr<RSRenderNode>;
    using SharedPtr = std::shared_ptr<RSRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::RS_NODE;
    virtual RSRenderNodeType GetType() const
    {
        return Type;
    }

    explicit RSRenderNode(NodeId id, const std::weak_ptr<RSContext>& context = {}, bool isTextureExportNode = false);
    explicit RSRenderNode(NodeId id, bool isOnTheTree, const std::weak_ptr<RSContext>& context = {},
        bool isTextureExportNode = false);
    RSRenderNode(const RSRenderNode&) = delete;
    RSRenderNode(const RSRenderNode&&) = delete;
    RSRenderNode& operator=(const RSRenderNode&) = delete;
    RSRenderNode& operator=(const RSRenderNode&&) = delete;
    virtual ~RSRenderNode();

    void AddChild(SharedPtr child, int index = -1);
    void SetContainBootAnimation(bool isContainBootAnimation);
    bool GetContainBootAnimation() const;

    virtual void SetBootAnimation(bool isBootAnimation);
    virtual bool GetBootAnimation() const;

    void MoveChild(SharedPtr child, int index);
    void RemoveChild(SharedPtr child, bool skipTransition = false);
    void ClearChildren();
    void RemoveFromTree(bool skipTransition = false);

    // Add/RemoveCrossParentChild only used as: the child is under multiple parents(e.g. a window cross multi-screens)
    void AddCrossParentChild(const SharedPtr& child, int32_t index = -1);
    void RemoveCrossParentChild(const SharedPtr& child, const WeakPtr& newParent);

    virtual void CollectSurface(const std::shared_ptr<RSRenderNode>& node,
                                std::vector<RSRenderNode::SharedPtr>& vec,
                                bool isUniRender,
                                bool onlyFirstLevel);
    virtual void CollectSurfaceForUIFirstSwitch(uint32_t& leashWindowCount, uint32_t minNodeNum);
    virtual void QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor);
    // if subtree dirty or child filter need prepare
    virtual bool IsSubTreeNeedPrepare(bool needMap, bool filterInGlobal, bool isOccluded = false);
    virtual void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor);
    virtual void Process(const std::shared_ptr<RSNodeVisitor>& visitor);
    bool IsDirty() const;
    bool IsSubTreeDirty() const;
    void SetSubTreeDirty(bool val);
    void SetParentSubTreeDirty();
    // attention: current all base node's dirty ops causing content dirty
    // if there is any new dirty op, check it
    bool IsContentDirty() const;
    void SetContentDirty();
    void ResetIsOnlyBasicGeoTransform();
    bool IsOnlyBasicGeoTransform() const;

    WeakPtr GetParent() const;

    inline NodeId GetId() const
    {
        return id_;
    }

    inline const std::map<NodeId, std::vector<WeakPtr>>& GetSubSurfaceNodes() const
    {
        return subSurfaceNodes_;
    }

    bool IsFirstLevelSurfaceNode();
    bool SubSurfaceNodeNeedDraw(PartialRenderType opDropType);
    void AddSubSurfaceNode(SharedPtr child, SharedPtr parent);
    void RemoveSubSurfaceNode(SharedPtr child, SharedPtr parent);
    inline static const bool isSubSurfaceEnabled_ = RSSystemProperties::GetSubSurfaceEnabled();

    // flag: isOnTheTree; instanceRootNodeId: displaynode or leash/appnode attached to
    // firstLevelNodeId: surfacenode for uiFirst to assign task; cacheNodeId: drawing cache rootnode attached to
    virtual void SetIsOnTheTree(bool flag, NodeId instanceRootNodeId = INVALID_NODEID,
        NodeId firstLevelNodeId = INVALID_NODEID, NodeId cacheNodeId = INVALID_NODEID);
    bool IsOnTheTree() const;

    bool IsNewOnTree() const
    {
        return isNewOnTree_;
    }

    bool GetIsTextureExportNode() const;

    using ChildrenListSharedPtr = std::shared_ptr<const std::vector<std::shared_ptr<RSRenderNode>>>;
    // return children and disappeared children, not guaranteed to be sorted by z-index
    ChildrenListSharedPtr GetChildren() const;
    // return children and disappeared children, sorted by z-index
    ChildrenListSharedPtr GetSortedChildren() const;
    uint32_t GetChildrenCount() const;
    std::shared_ptr<RSRenderNode> GetFirstChild() const;

    void DumpTree(int32_t depth, std::string& ou) const;
    void DumpNodeInfo(DfxString& log);

    virtual bool HasDisappearingTransition(bool recursive = true) const;

    void SetTunnelHandleChange(bool change);
    bool GetTunnelHandleChange() const;

    // type-safe reinterpret_cast
    template<typename T>
    bool IsInstanceOf() const
    {
        constexpr auto targetType = static_cast<uint32_t>(T::Type);
        return (static_cast<uint32_t>(GetType()) & targetType) == targetType;
    }
    template<typename T>
    static std::shared_ptr<T> ReinterpretCast(std::shared_ptr<RSRenderNode> node)
    {
        return node ? node->ReinterpretCastTo<T>() : nullptr;
    }
    template<typename T>
    std::shared_ptr<T> ReinterpretCastTo()
    {
        return (IsInstanceOf<T>()) ? std::static_pointer_cast<T>(shared_from_this()) : nullptr;
    }
    template<typename T>
    std::shared_ptr<const T> ReinterpretCastTo() const
    {
        return (IsInstanceOf<T>()) ? std::static_pointer_cast<const T>(shared_from_this()) : nullptr;
    }

    bool HasChildrenOutOfRect() const;
    void UpdateChildrenOutOfRectFlag(bool flag);

    void ResetHasRemovedChild();
    bool HasRemovedChild() const;
    void ResetChildrenRect();
    RectI GetChildrenRect() const;

    bool ChildHasVisibleFilter() const;
    void SetChildHasVisibleFilter(bool val);
    bool ChildHasVisibleEffect() const;
    void SetChildHasVisibleEffect(bool val);

    NodeId GetInstanceRootNodeId() const;
    const std::shared_ptr<RSRenderNode> GetInstanceRootNode() const;
    NodeId GetFirstLevelNodeId() const;

    // accumulate all valid children's area
    void UpdateChildrenRect(const RectI& subRect);
    void MapChildrenRect();
    void SetDirty(bool forceAddToActiveList = false);

    virtual void AddDirtyType(RSModifierType type)
    {
        dirtyTypes_.set(static_cast<int>(type), true);
    }

    std::tuple<bool, bool, bool> Animate(int64_t timestamp, int64_t period = 0, bool isDisplaySyncEnabled = false);

    bool IsClipBound() const;
    // clipRect has value in UniRender when calling PrepareCanvasRenderNode, else it is nullopt
    const RectI& GetSelfDrawRect() const;
    const RectI& GetAbsDrawRect() const;

    bool UpdateDrawRectAndDirtyRegion(RSDirtyRegionManager& dirtyManager, const std::shared_ptr<RSRenderNode>& parent, bool accumGeoDirty,
        std::optional<RectI> clipRect = std::nullopt);
    // update node's local draw region (based on node itself, including childrenRect)
    void UpdateLocalDrawRect();

    bool Update(RSDirtyRegionManager& dirtyManager, const std::shared_ptr<RSRenderNode>& parent, bool parentDirty,
        std::optional<RectI> clipRect = std::nullopt);
    virtual std::optional<Drawing::Rect> GetContextClipRegion() const { return std::nullopt; }

    RSProperties& GetMutableRenderProperties();
    const RSProperties& GetRenderProperties() const;
    void UpdateRenderStatus(RectI& dirtyRegion, bool isPartialRenderEnabled);
    bool IsRenderUpdateIgnored() const;

    // used for animation test
    RSAnimationManager& GetAnimationManager();

    void ApplyBoundsGeometry(RSPaintFilterCanvas& canvas);
    void ApplyAlpha(RSPaintFilterCanvas& canvas);
    virtual void ProcessTransitionBeforeChildren(RSPaintFilterCanvas& canvas);
    virtual void ProcessAnimatePropertyBeforeChildren(RSPaintFilterCanvas& canvas, bool includeProperty = true) {}
    virtual void ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas);

    virtual void ProcessRenderContents(RSPaintFilterCanvas& canvas) {}

    virtual void ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas);
    virtual void ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas) {}
    virtual void ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas);

    void RenderTraceDebug() const;
    bool ShouldPaint() const;

    RectI GetOldDirty() const;
    RectI GetOldDirtyInSurface() const;
    bool IsDirtyRegionUpdated() const;
    void CleanDirtyRegionUpdated();

    void AddModifier(const std::shared_ptr<RSRenderModifier>& modifier, bool isSingleFrameComposer = false);
    void RemoveModifier(const PropertyId& id);
    std::shared_ptr<RSRenderModifier> GetModifier(const PropertyId& id);

    bool IsShadowValidLastFrame() const;
    void SetShadowValidLastFrame(bool isShadowValidLastFrame)
    {
        isShadowValidLastFrame_ = isShadowValidLastFrame;
    }

    // update parent's children rect including childRect and itself
    void UpdateParentChildrenRect(std::shared_ptr<RSRenderNode> parentNode) const;
    virtual void UpdateFilterCacheManagerWithCacheRegion(
        RSDirtyRegionManager& dirtyManager, const std::optional<RectI>& clipRect = std::nullopt);

    void SetStaticCached(bool isStaticCached);
    bool IsStaticCached() const;
    // store prev surface subtree's must-renewed info that need prepare
    virtual void StoreMustRenewedInfo();
    bool HasMustRenewedInfo() const;
    bool HasSubSurface() const;

    bool NeedInitCacheSurface() const;
    bool NeedInitCacheCompletedSurface() const;
    bool IsPureContainer() const;
    bool IsContentNode() const;

    inline const RSRenderContent::DrawCmdContainer& GetDrawCmdModifiers() const
    {
        return renderContent_->drawCmdModifiers_;
    }

    using ClearCacheSurfaceFunc =
        std::function<void(std::shared_ptr<Drawing::Surface>&&,
        std::shared_ptr<Drawing::Surface>&&, uint32_t, uint32_t)>;
    void InitCacheSurface(Drawing::GPUContext* grContext, ClearCacheSurfaceFunc func = nullptr,
        uint32_t threadIndex = UNI_MAIN_THREAD_INDEX);

    Vector2f GetOptionalBufferSize() const;

    std::shared_ptr<Drawing::Surface> GetCacheSurface() const
    {
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        return cacheSurface_;
    }

// use for uni render visitor
    std::shared_ptr<Drawing::Surface> GetCacheSurface(uint32_t threadIndex, bool needCheckThread,
        bool releaseAfterGet = false);

    void UpdateCompletedCacheSurface();
    void SetTextureValidFlag(bool isValid);
    std::shared_ptr<Drawing::Surface> GetCompletedCacheSurface(uint32_t threadIndex = UNI_MAIN_THREAD_INDEX,
        bool needCheckThread = true, bool releaseAfterGet = false);
    void ClearCacheSurfaceInThread();
    void ClearCacheSurface(bool isClearCompletedCacheSurface = true);
    bool IsCacheSurfaceValid() const;

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    void UpdateBackendTexture();
#endif

    void DrawCacheSurface(RSPaintFilterCanvas& canvas, uint32_t threadIndex = UNI_MAIN_THREAD_INDEX,
        bool isUIFirst = false);

    void SetCacheType(CacheType cacheType);
    CacheType GetCacheType() const;

    void SetCacheSurfaceNeedUpdated(bool isCacheSurfaceNeedUpdate)
    {
        isCacheSurfaceNeedUpdate_ = isCacheSurfaceNeedUpdate;
    }

    bool GetCacheSurfaceNeedUpdated() const
    {
        return isCacheSurfaceNeedUpdate_;
    }

#ifdef DDGR_ENABLE_FEATURE_OPINC
    bool IsOnTreeDirty();
    void SetDirtyByOnTree(bool forceAddToActiveList = false);
    Vector4f GetOptionBufferBound() const;
    Vector2f GetOpincBufferSize() const;
    Drawing::Rect GetOpincBufferBound() const;
#endif

    int GetShadowRectOffsetX() const;
    int GetShadowRectOffsetY() const;

    void SetDrawingCacheType(RSDrawingCacheType cacheType);
    RSDrawingCacheType GetDrawingCacheType() const;
    void ResetFilterRectsInCache(const std::unordered_set<NodeId>& curRects);
    void GetFilterRectsInCache(std::unordered_map<NodeId, std::unordered_set<NodeId>>& allRects) const;
    bool IsFilterRectsInCache() const;
    void SetDrawingCacheChanged(bool cacheChanged);
    bool GetDrawingCacheChanged() const;
    void ResetDrawingCacheNeedUpdate();
    void SetVisitedCacheRootIds(const std::unordered_set<NodeId>& visitedNodes);
    const std::unordered_set<NodeId>& GetVisitedCacheRootIds() const;
    // manage cache root nodeid
    void SetDrawingCacheRootId(NodeId id);
    NodeId GetDrawingCacheRootId() const;
    // record cache geodirty for preparation optimization
    void SetGeoUpdateDelay(bool val);
    void ResetGeoUpdateDelay();
    bool GetGeoUpdateDelay() const;
    bool HasAnimation() const;

    bool HasFilter() const;
    void SetHasFilter(bool hasFilter);

    std::recursive_mutex& GetSurfaceMutex() const;

    bool HasHardwareNode() const;
    void SetHasHardwareNode(bool hasHardwareNode);

    bool HasAbilityComponent() const;
    void SetHasAbilityComponent(bool hasAbilityComponent);

    uint32_t GetCacheSurfaceThreadIndex() const;

    uint32_t GetCompletedSurfaceThreadIndex() const;

    bool IsMainThreadNode() const;
    void SetIsMainThreadNode(bool isMainThreadNode);

    bool IsScale() const;
    void SetIsScale(bool isScale);

    void SetPriority(NodePriorityType priority);
    NodePriorityType GetPriority();

    bool IsAncestorDirty() const;
    void SetIsAncestorDirty(bool isAncestorDirty);

    bool IsParentLeashWindow() const;
    void SetParentLeashWindow();

    bool IsParentScbScreen() const;
    void SetParentScbScreen();

    bool HasCachedTexture() const;

    void SetDrawRegion(const std::shared_ptr<RectF>& rect);
    const std::shared_ptr<RectF>& GetDrawRegion() const;
    void SetOutOfParent(OutOfParentType outOfParent);
    OutOfParentType GetOutOfParent() const;

    void UpdateEffectRegion(std::optional<Drawing::RectI>& region, bool isForced = false);
    bool IsBackgroundFilterCacheValid() const;
    virtual void UpdateFilterCacheWithDirty(RSDirtyRegionManager& dirtyManager, bool isForeground = true);

    void CheckGroupableAnimation(const PropertyId& id, bool isAnimAdd);
    bool IsForcedDrawInGroup() const;
    bool IsSuggestedDrawInGroup() const;
    void CheckDrawingCacheType();
    bool HasCacheableAnim() const { return hasCacheableAnim_; }
    enum NodeGroupType {
        NONE = 0,
#ifdef DDGR_ENABLE_FEATURE_OPINC
        GROUPED_BY_AUTO,
#endif
        GROUPED_BY_ANIM,
        GROUPED_BY_UI,
        GROUPED_BY_USER,
    };
    void MarkNodeGroup(NodeGroupType type, bool isNodeGroup, bool includeProperty);
    NodeGroupType GetNodeGroupType();
    bool IsNodeGroupIncludeProperty() const;

    void MarkNodeSingleFrameComposer(bool isNodeSingleFrameComposer, pid_t pid = 0);
    virtual bool GetNodeIsSingleFrameComposer() const;

    /////////////////////////////////////////////

    // shared transition params, in format <InNodeId, target weakPtr>, nullopt means no transition
    using SharedTransitionParam = std::pair<NodeId, std::weak_ptr<RSRenderNode>>;
    void SetSharedTransitionParam(const std::optional<SharedTransitionParam>&& sharedTransitionParam);
    const std::optional<SharedTransitionParam>& GetSharedTransitionParam() const;

    void SetGlobalAlpha(float alpha);
    float GetGlobalAlpha() const;
    virtual void OnAlphaChanged() {}

    inline const Vector4f& GetGlobalCornerRadius() noexcept
    {
        return globalCornerRadius_;
    }

    inline void SetGlobalCornerRadius(const Vector4f& globalCornerRadius) noexcept
    {
        globalCornerRadius_ = globalCornerRadius;
    }

    void ActivateDisplaySync();
    void InActivateDisplaySync();
    void UpdateDisplaySyncRange();

    void MarkNonGeometryChanged();
    void ApplyModifiers();
    void ApplyPositionZModifier();
    virtual void UpdateRenderParams();

    virtual RectI GetFilterRect() const;
    void SetIsUsedBySubThread(bool isUsedBySubThread);
    bool GetIsUsedBySubThread() const;

    void SetLastIsNeedAssignToSubThread(bool lastIsNeedAssignToSubThread);
    bool GetLastIsNeedAssignToSubThread() const;

    void SetIsTextureExportNode(bool isTextureExportNode)
    {
        isTextureExportNode_ = isTextureExportNode;
    }

#ifdef DDGR_ENABLE_FEATURE_OPINC
    class RSAutoCache;
    const std::shared_ptr<RSAutoCache>& GetAutoCache();
    bool isOpincRectOutParent_ = false;
#endif

    const std::shared_ptr<RSRenderContent> GetRenderContent() const;

    void MarkParentNeedRegenerateChildren() const;

    std::unique_ptr<RSRenderParams>& GetStagingRenderParams();

    const std::unique_ptr<RSRenderParams>& GetRenderParams() const;

    void UpdateStagingDrawCmdList(std::shared_ptr<Drawing::DrawCmdList> drawCmdList);

    void SetNeedSyncFlag(bool needSync);

    void Sync()
    {
        OnSync();
    }

protected:
    virtual void OnApplyModifiers() {}

    enum class NodeDirty {
        CLEAN = 0,
    #ifdef DDGR_ENABLE_FEATURE_OPINC
        ON_TREE_DIRTY,
    #endif
        DIRTY,
    };
    void SetClean();

    void DumpNodeType(std::string& out) const;

    void DumpSubClassNode(std::string& out) const;
    void DumpDrawCmdModifiers(std::string& out) const;
    void DumpDrawCmdModifier(std::string& propertyDesc, RSModifierType type,
        std::shared_ptr<RSRenderModifier>& modifier) const;

    const std::weak_ptr<RSContext> GetContext() const
    {
        return context_;
    }
    virtual void OnTreeStateChanged();
    // recursive update subSurfaceCnt
    void UpdateSubSurfaceCnt(SharedPtr curParent, SharedPtr preParent);

    static void SendCommandFromRT(std::unique_ptr<RSCommand>& command, NodeId nodeId);
    void AddGeometryModifier(const std::shared_ptr<RSRenderModifier>& modifier);

    virtual void InitRenderParams();
    virtual void OnSync();

    std::unique_ptr<RSRenderParams> renderParams_;
    std::unique_ptr<RSRenderParams> stagingRenderParams_;

    RSPaintFilterCanvas::SaveStatus renderNodeSaveCount_;
    std::shared_ptr<RSSingleFrameComposer> singleFrameComposer_ = nullptr;
    bool isNodeSingleFrameComposer_ = false;
    // if true, it means currently it's in partial render mode and this node is intersect with dirtyRegion
    bool isRenderUpdateIgnored_ = false;
    bool isShadowValidLastFrame_ = false;

    bool IsSelfDrawingNode() const;
    bool isOnTheTree_ = false;
    NodeId drawingCacheRootId_ = INVALID_NODEID;
    bool mustRenewedInfo_ = false;

    ModifierDirtyTypes dirtyTypes_;
    bool isBootAnimation_ = false;

private:
    NodeId id_;
    NodeId instanceRootNodeId_ = INVALID_NODEID;
    NodeId firstLevelNodeId_ = INVALID_NODEID;

    WeakPtr parent_;
    void SetParent(WeakPtr parent);
    void ResetParent();
    virtual void OnResetParent() {}

    std::list<WeakPtr> children_;
    std::list<std::pair<SharedPtr, uint32_t>> disappearingChildren_;

    // Note: Make sure that fullChildrenList_ is never nullptr. Otherwise, the caller using
    // `for (auto child : *GetSortedChildren()) { ... }` will crash.
    // When an empty list is needed, use EmptyChildrenList instead.
    static const inline auto EmptyChildrenList = std::make_shared<const std::vector<std::shared_ptr<RSRenderNode>>>();
    ChildrenListSharedPtr fullChildrenList_ = EmptyChildrenList ;
    bool isFullChildrenListValid_ = true;
    bool isChildrenSorted_ = true;

    void UpdateFullChildrenListIfNeeded();
    void GenerateFullChildrenList();
    void ResortChildren();

    std::weak_ptr<RSContext> context_ = {};
    NodeDirty dirtyStatus_ = NodeDirty::CLEAN;
    bool isContentDirty_ = false;
    bool isNewOnTree_ = false;
    bool isOnlyBasicGeoTransform_ = true;
    friend class RSRenderPropertyBase;
    friend class RSRenderTransition;
    std::atomic<bool> isTunnelHandleChange_ = false;
    // accumulate all children's region rect for dirty merging when any child has been removed
    bool hasRemovedChild_ = false;
    bool hasChildrenOutOfRect_ = false;
    RectI childrenRect_;
    bool childHasVisibleFilter_ = false;  // only collect visible children filter status
    bool childHasVisibleEffect_ = false;  // only collect visible children has useeffect

    void InternalRemoveSelfFromDisappearingChildren();
    void FallbackAnimationsToRoot();
    void FilterModifiersByPid(pid_t pid);

    void CollectAndUpdateLocalShadowRect();
    void CollectAndUpdateLocalOutlineRect();
    void CollectAndUpdateLocalPixelStretchRect();
    // update drawrect based on self's info
    void UpdateSelfDrawRect();
    void UpdateAbsDirtyRegion(RSDirtyRegionManager& dirtyManager, std::optional<RectI> clipRect = std::nullopt);
    
    void UpdateDirtyRegion(RSDirtyRegionManager& dirtyManager, bool geoDirty, std::optional<RectI> clipRect);
    void UpdateFullScreenFilterCacheRect(RSDirtyRegionManager& dirtyManager, bool isForeground) const;

    void UpdateShouldPaint(); // update node should paint state in apply modifier stage
    bool shouldPaint_ = true;
    bool isSubTreeDirty_ = false;

    bool isDirtyRegionUpdated_ = false;
    bool isContainBootAnimation_ = false;
    bool isLastVisible_ = false;
    bool fallbackAnimationOnDestroy_ = true;
    uint32_t disappearingTransitionCount_ = 0;
    RectI oldDirty_;
    RectI oldDirtyInSurface_;
    RSAnimationManager animationManager_;
    std::map<PropertyId, std::shared_ptr<RSRenderModifier>> modifiers_;
    // bounds and frame modifiers must be unique
    std::shared_ptr<RSRenderModifier> boundsModifier_;
    std::shared_ptr<RSRenderModifier> frameModifier_;
#ifdef DDGR_ENABLE_FEATURE_OPINC
    std::shared_ptr<RSAutoCache> autoCache_;
#endif

    std::shared_ptr<Drawing::Image> GetCompletedImage(
        RSPaintFilterCanvas& canvas, uint32_t threadIndex, bool isUIFirst);
    std::shared_ptr<Drawing::Surface> cacheSurface_ = nullptr;
    std::shared_ptr<Drawing::Surface> cacheCompletedSurface_ = nullptr;
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::BackendTexture cacheBackendTexture_;
    Drawing::BackendTexture cacheCompletedBackendTexture_;
#ifdef RS_ENABLE_VK
    NativeBufferUtils::VulkanCleanupHelper* cacheCleanupHelper_ = nullptr;
    NativeBufferUtils::VulkanCleanupHelper* cacheCompletedCleanupHelper_ = nullptr;
#endif
    bool isTextureValid_ = false;
#endif
    std::atomic<bool> isCacheSurfaceNeedUpdate_ = false;
    std::atomic<bool> isStaticCached_ = false;
    CacheType cacheType_ = CacheType::NONE;
    // drawing group cache
    RSDrawingCacheType drawingCacheType_ = RSDrawingCacheType::DISABLED_CACHE;
    bool isDrawingCacheChanged_ = false;
    bool drawingCacheNeedUpdate_ = false;
    // since cache preparation optimization would skip child's dirtyFlag(geoDirty) update
    // it should be recorded and update if marked dirty again
    bool cacheGeoPreparationDelay_ = false;

    std::unordered_set<NodeId> curCacheFilterRects_ = {};
    std::unordered_set<NodeId> visitedCacheRoots_ = {};
    // collect subtree's surfaceNode including itself
    uint32_t subSurfaceCnt_ = 0;

    mutable std::recursive_mutex surfaceMutex_;
    ClearCacheSurfaceFunc clearCacheSurfaceFunc_ = nullptr;
    uint32_t cacheSurfaceThreadIndex_ = UNI_MAIN_THREAD_INDEX;
    uint32_t completedSurfaceThreadIndex_ = UNI_MAIN_THREAD_INDEX;
    bool isMainThreadNode_ = true;
    bool isScale_ = false;
    bool hasFilter_ = false;
    bool hasHardwareNode_ = false;
    bool hasAbilityComponent_ = false;
    bool isAncestorDirty_ = false;
    bool isParentLeashWindow_ = false;
    bool isParentScbScreen_ = false;
    NodePriorityType priority_ = NodePriorityType::MAIN_PRIORITY;

    OutOfParentType outOfParent_ = OutOfParentType::UNKNOWN;
    float globalAlpha_ = 1.0f;
    Vector4f globalCornerRadius_{ 0.f, 0.f, 0.f, 0.f };
    std::optional<SharedTransitionParam> sharedTransitionParam_;

    std::shared_ptr<RectF> drawRegion_ = nullptr;
    NodeGroupType nodeGroupType_ = NodeGroupType::NONE;
    bool nodeGroupIncludeProperty_ = false;

    // shadowRectOffset means offset between shadowRect and absRect of node
    int shadowRectOffsetX_ = 0;
    int shadowRectOffsetY_ = 0;
    // Only use in RSRenderNode::DrawCacheSurface to calculate scale factor
    float boundsWidth_ = 0.0f;
    float boundsHeight_ = 0.0f;
    bool hasCacheableAnim_ = false;
    bool geometryChangeNotPerceived_ = false;
    // including enlarged draw region
    RectI selfDrawRect_;
    RectI localShadowRect_;
    RectI localOutlineRect_;
    RectI localPixelStretchRect_;
    // map parentMatrix
    RectI absDrawRect_;

    bool isTextureExportNode_ = false;

    std::atomic_bool isUsedBySubThread_ = false;
    bool lastIsNeedAssignToSubThread_ = false;

    std::shared_ptr<RSRenderDisplaySync> displaySync_ = nullptr;

    uint8_t drawableVecStatus_ = 0;
    void UpdateDrawableVec();
    void UpdateDisplayList();
    std::map<NodeId, std::vector<WeakPtr>> subSurfaceNodes_;
    pid_t appPid_ = 0;

    const std::shared_ptr<RSRenderContent> renderContent_ = std::make_shared<RSRenderContent>();

    void OnRegister(const std::weak_ptr<RSContext>& context);

    // Test pipeline
    struct DrawCmdIndex {
        int8_t shadowIndex_          = -1;
        int8_t backgroundEndIndex_   = -1;
        int8_t childrenIndex_        = -1;
        int8_t contentIndex_         = -1;
        int8_t foregroundBeginIndex_ = -1;
        int8_t endIndex_             = -1;
    };
    bool drawCmdListNeedSync_ = false;
    DrawCmdIndex drawCmdIndex_;
    std::vector<Drawing::RecordingCanvas::DrawFunc> drawCmdList_;
    DrawCmdIndex stagingDrawCmdIndex_;
    std::vector<Drawing::RecordingCanvas::DrawFunc> stagingDrawCmdList_;

    std::unordered_set<RSDrawableSlot> dirtySlots_;
    RSDrawable::Vec drawableVec_;

    friend class DrawFuncOpItem;
    friend class RSContext;
    friend class RSMainThread;
    friend class RSModifierDrawable;
    friend class RSProxyRenderNode;
    friend class RSRenderNodeMap;
    friend class RSRenderThread;
    friend class RSRenderTransition;
    friend class RSRenderNodeDrawable;
};
// backward compatibility
using RSBaseRenderNode = RSRenderNode;
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_H
