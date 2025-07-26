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

#include "display_engine/rs_luminance_control.h"
#include "feature/opinc/rs_opinc_cache.h"
#include "feature/single_frame_composer/rs_single_frame_composer.h"
#include "hwc/rs_hwc_recorder.h"

#include "animation/rs_animation_manager.h"
#include "animation/rs_frame_rate_range.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "draw/surface.h"
#include "drawable/rs_drawable.h"
#include "drawable/rs_property_drawable.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "image/gpu_context.h"
#include "memory/rs_dfx_string.h"
#include "memory/rs_memory_snapshot.h"
#include "modifier/rs_render_modifier.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_display_sync.h"
#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {
namespace DrawableV2 {
class RSChildrenDrawable;
class RSRenderNodeDrawableAdapter;
class RSRenderNodeShadowDrawable;
}
class RSRenderParams;
class RSContext;
class RSNodeVisitor;
class RSCommand;
namespace NativeBufferUtils {
class VulkanCleanupHelper;
}
namespace ModifierNG {
class RSRenderModifier;
class RSForegroundFilterRenderModifier;
class RSBackgroundFilterRenderModifier;
enum class RSModifierType : uint16_t;
}
struct SharedTransitionParam;

struct CurFrameInfoDetail {
    uint32_t curFramePrepareSeqNum = 0;
    uint32_t curFramePostPrepareSeqNum = 0;
    uint64_t curFrameVsyncId = 0;
    bool curFrameSubTreeSkipped = false;
    bool curFrameReverseChildren = false;
};

class RSB_EXPORT RSRenderNode : public std::enable_shared_from_this<RSRenderNode> {
public:
    using WeakPtr = std::weak_ptr<RSRenderNode>;
    using SharedPtr = std::shared_ptr<RSRenderNode>;
    using ModifierNGContainer = std::vector<std::shared_ptr<ModifierNG::RSRenderModifier>>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::RS_NODE;
    std::atomic<int32_t> cacheCnt_ = -1;
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

    virtual void SetBootAnimation(bool isBootAnimation);
    virtual bool GetBootAnimation() const;

    virtual bool GetGlobalPositionEnabled() const;

    void MoveChild(SharedPtr child, int index);
    void RemoveChild(SharedPtr child, bool skipTransition = false);
    void ClearChildren();
    void SetUIContextToken(uint64_t token)
    {
        uiContextToken_ = token;
    }
    void RemoveFromTree(bool skipTransition = false);

    // Add/RemoveCrossParentChild only used as: the child is under multiple parents(e.g. a window cross multi-screens)
    void AddCrossParentChild(const SharedPtr& child, int32_t index = -1);
    void RemoveCrossParentChild(const SharedPtr& child, const WeakPtr& newParent);
    void SetIsCrossNode(bool isCrossNode);

    // Only used in PC extend screen
    void AddCrossScreenChild(const SharedPtr& child, NodeId cloneNodeId, int32_t index = -1,
        bool autoClearCloneNode = false);
    void RemoveCrossScreenChild(const SharedPtr& child);
    void ClearCloneCrossNode();

    WeakPtr GetSourceCrossNode() const
    {
        return sourceCrossNode_;
    }

    bool IsCloneCrossNode() const
    {
        return isCloneCrossNode_;
    }

    bool HasVisitedCrossNode() const;

    void SetCrossNodeVisitedStatus(bool hasVisited);

    void SetCurCloneNodeParent(SharedPtr node)
    {
        curCloneNodeParent_ = node;
    }
    WeakPtr GetCurCloneNodeParent() const
    {
        return curCloneNodeParent_;
    }

    virtual void CollectSurface(const std::shared_ptr<RSRenderNode>& node,
                                std::vector<RSRenderNode::SharedPtr>& vec,
                                bool isUniRender,
                                bool onlyFirstLevel);
    virtual void CollectSelfDrawingChild(const std::shared_ptr<RSRenderNode>& node, std::vector<NodeId>& vec);
    virtual void QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor);
    void PrepareSelfNodeForApplyModifiers();
    void PrepareChildrenForApplyModifiers();
    // if subtree dirty or child filter need prepare
    virtual bool IsSubTreeNeedPrepare(bool filterInGlobal, bool isOccluded = false);
    virtual void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor);
    virtual void Process(const std::shared_ptr<RSNodeVisitor>& visitor);
    bool SetAccumulatedClipFlag(bool clipChange);
    bool GetAccumulatedClipFlagChange() const
    {
        return isAccumulatedClipFlagChanged_;
    }
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
    void ForceMergeSubTreeDirtyRegion(RSDirtyRegionManager& dirtyManager, const RectI& clipRect);
    void SubTreeSkipPrepare(RSDirtyRegionManager& dirtymanager, bool isDirty, bool accumGeoDirty,
        const RectI& clipRect);
    inline bool LastFrameSubTreeSkipped() const
    {
        return lastFrameSubTreeSkipped_;
    }

    inline WeakPtr GetParent() const
    {
        return parent_;
    }

    inline NodeId GetId() const
    {
        return id_;
    }

    bool IsFirstLevelNode();
    void UpdateChildSubSurfaceNode();
    bool GetAbsMatrixReverse(const RSRenderNode& rootNode, Drawing::Matrix& absMatrix);

    // flag: isOnTheTree; instanceRootNodeId: displaynode or leash/appnode attached to
    // firstLevelNodeId: surfacenode for uiFirst to assign task; cacheNodeId: drawing cache rootnode attached to
    virtual void SetIsOnTheTree(bool flag, NodeId instanceRootNodeId = INVALID_NODEID,
        NodeId firstLevelNodeId = INVALID_NODEID, NodeId cacheNodeId = INVALID_NODEID,
        NodeId uifirstRootNodeId = INVALID_NODEID, NodeId screenNodeId = INVALID_NODEID,
        NodeId logicalDisplayNodeId = INVALID_NODEID);
    void SetIsOntheTreeOnlyFlag(bool flag)
    {
        SetIsOnTheTree(flag, instanceRootNodeId_, firstLevelNodeId_, drawingCacheRootId_,
            uifirstRootNodeId_, screenNodeId_, logicalDisplayNodeId_);
    }
    inline bool IsOnTheTree() const
    {
        return isOnTheTree_;
    }

    inline bool IsNewOnTree() const
    {
        return isNewOnTree_;
    }

    inline bool GetIsTextureExportNode() const
    {
        return isTextureExportNode_;
    }

    inline RectI GetFilterRegion() const
    {
        return filterRegion_;
    }

    inline bool IsRepaintBoundary() const
    {
        return isRepaintBoundary_;
    }

    inline void MarkRepaintBoundary(bool isRepaintBoundary)
    {
        isRepaintBoundary_ = isRepaintBoundary;
    }

    inline bool IsWaitSync() const
    {
        return waitSync_;
    }

    using ChildrenListSharedPtr = std::shared_ptr<const std::vector<std::shared_ptr<RSRenderNode>>>;
    // return children and disappeared children, not guaranteed to be sorted by z-index
    ChildrenListSharedPtr GetChildren() const;
    // return children and disappeared children, sorted by z-index
    virtual ChildrenListSharedPtr GetSortedChildren() const;
    void CollectAllChildren(const std::shared_ptr<RSRenderNode>& node, std::vector<NodeId>& vec);
    uint32_t GetChildrenCount() const;
    std::shared_ptr<RSRenderNode> GetFirstChild() const;
    std::list<WeakPtr> GetChildrenList() const;

    void DumpTree(int32_t depth, std::string& out) const;
    void DumpNodeInfo(DfxString& log);

    virtual bool HasDisappearingTransition(bool recursive = true) const;

    void SetTunnelHandleChange(bool change);
    bool GetTunnelHandleChange() const;

    void SetChildHasSharedTransition(bool val);
    bool ChildHasSharedTransition() const;

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
    inline void ResetChildrenRect()
    {
        childrenRect_.Clear();
    }
    RectI GetChildrenRect() const;
    RectI GetRemovedChildrenRect() const;

    bool ChildHasVisibleFilter() const;
    void SetChildHasVisibleFilter(bool val);
    bool ChildHasVisibleEffect() const;
    void SetChildHasVisibleEffect(bool val);
    const std::vector<NodeId>& GetVisibleFilterChild() const;
    void UpdateVisibleFilterChild(RSRenderNode& childNode);
    const std::unordered_set<NodeId>& GetVisibleEffectChild() const;
    void UpdateVisibleEffectChild(RSRenderNode& childNode);

    inline NodeId GetInstanceRootNodeId() const
    {
        return instanceRootNodeId_;
    }

    const std::shared_ptr<RSRenderNode> GetInstanceRootNode() const;
    inline NodeId GetFirstLevelNodeId() const
    {
        return firstLevelNodeId_;
    }
    const std::shared_ptr<RSRenderNode> GetFirstLevelNode() const;

    inline const std::set<NodeId>& GetPreFirstLevelNodeIdSet()
    {
        return preFirstLevelNodeIdSet_;
    }

    inline std::set<NodeId>& GetMutablePreFirstLevelNodeIdSet()
    {
        return preFirstLevelNodeIdSet_;
    }

    inline void AddPreFirstLevelNodeIdSet(const std::set<NodeId>& preSet)
    {
        preFirstLevelNodeIdSet_.insert(preSet.begin(), preSet.end());
    }

    // only use for ARKTS_CARD
    inline NodeId GetUifirstRootNodeId() const
    {
        return uifirstRootNodeId_;
    }
    const std::shared_ptr<RSRenderNode> GetUifirstRootNode() const;
    void UpdateTreeUifirstRootNodeId(NodeId id);

    // reset accumulated vals before traverses children
    void ResetChildRelevantFlags();
    // accumulate all valid children's area
    void UpdateChildrenRect(const RectI& subRect);
    void UpdateCurCornerRadius(Vector4f& curCornerRadius);
    void SetDirty(bool forceAddToActiveList = false);

    void ResetDirtyFlag()
    {
        SetClean();
        GetMutableRenderProperties().ResetDirty();
    }

    virtual void AddDirtyType(RSModifierType type)
    {
        dirtyTypes_.set(static_cast<int>(type), true);
    }

    virtual void AddDirtyType(ModifierNG::RSModifierType type)
    {
        dirtyTypesNG_.set(static_cast<int>(type), true);
    }

    std::tuple<bool, bool, bool> Animate(
        int64_t timestamp, int64_t& minLeftDelayTime, int64_t period = 0, bool isDisplaySyncEnabled = false);

    bool IsClipBound() const;
    // clipRect has value in UniRender when calling PrepareCanvasRenderNode, else it is nullopt
    const RectF& GetSelfDrawRect() const;
    const RectI& GetAbsDrawRect() const;
    void UpdateAbsDrawRect();

    void ResetChangeState();
    void NodeDrawLargeAreaBlur(std::pair<bool, bool>& nodeDrawLargeAreaBlur);
    bool UpdateDrawRectAndDirtyRegion(RSDirtyRegionManager& dirtyManager, bool accumGeoDirty, const RectI& clipRect,
        const Drawing::Matrix& parentSurfaceMatrix);
    void UpdateDirtyRegionInfoForDFX(RSDirtyRegionManager& dirtyManager);
    void UpdateSubTreeSkipDirtyForDFX(RSDirtyRegionManager& dirtyManager, const RectI& rect);
    // update node's local draw region (based on node itself, including childrenRect)
    bool UpdateLocalDrawRect();

    bool Update(RSDirtyRegionManager& dirtyManager, const std::shared_ptr<RSRenderNode>& parent, bool parentDirty,
        std::optional<RectI> clipRect = std::nullopt);
    virtual std::optional<Drawing::Rect> GetContextClipRegion() const { return std::nullopt; }

    inline RSProperties& GetMutableRenderProperties()
    {
        return renderProperties_;
    }
    inline const RSProperties& GetRenderProperties() const
    {
        return renderProperties_;
    }
    void UpdateRenderStatus(RectI& dirtyRegion, bool isPartialRenderEnabled);
    bool IsRenderUpdateIgnored() const;

    // used for animation test
    RSAnimationManager& GetAnimationManager();

    void ApplyAlphaAndBoundsGeometry(RSPaintFilterCanvas& canvas);
    virtual void ProcessTransitionBeforeChildren(RSPaintFilterCanvas& canvas);
    virtual void ProcessAnimatePropertyBeforeChildren(RSPaintFilterCanvas& canvas, bool includeProperty = true) {}
    virtual void ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas);

    virtual void ProcessRenderContents(RSPaintFilterCanvas& canvas) {}

    virtual void ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas);
    virtual void ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas) {}
    virtual void ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas);

    void RenderTraceDebug() const;
    inline bool ShouldPaint() const
    {
        return shouldPaint_;
    }

    inline RectI GetInnerAbsDrawRect() const noexcept
    {
        return innerAbsDrawRect_;
    }

    // dirty rect of current frame after update dirty, last frame before update
    RectI GetOldDirty() const;
    // dirty rect in display of current frame after update dirty, last frame before update
    RectI GetOldDirtyInSurface() const;
    // clip rect of last frame before post prepare, current frame after post prepare
    RectI GetOldClipRect() const;

    const Drawing::Matrix& GetOldAbsMatrix() const
    {
        return oldAbsMatrix_;
    }

    bool IsDirtyRegionUpdated() const;
    void CleanDirtyRegionUpdated();
    
    std::shared_ptr<RSRenderPropertyBase> GetProperty(PropertyId id);
    void RegisterProperty(const std::shared_ptr<RSRenderPropertyBase>& property);
    void UnregisterProperty(const std::shared_ptr<RSRenderPropertyBase>& property);
    void UnregisterProperty(PropertyId id);

    void AddModifier(const std::shared_ptr<RSRenderModifier>& modifier, bool isSingleFrameComposer = false);
    void RemoveModifier(const PropertyId& id);
    void RemoveAllModifiers();
    std::shared_ptr<RSRenderModifier> GetModifier(const PropertyId& id);

    void AddModifier(const std::shared_ptr<ModifierNG::RSRenderModifier>& modifier, bool isSingleFrameComposer = false);
    void RemoveModifier(ModifierNG::RSModifierType type, ModifierId id);
    void RemoveModifierNG(ModifierId id);
    void RemoveAllModifiersNG();
    std::shared_ptr<ModifierNG::RSRenderModifier> GetModifierNG(
        ModifierNG::RSModifierType type, ModifierId id = 0) const;
    const ModifierNGContainer& GetModifiersNG(ModifierNG::RSModifierType type) const;
    bool HasDrawCmdModifiers() const;
    bool HasContentStyleModifierOnly() const;

    size_t GetAllModifierSize();

    bool IsShadowValidLastFrame() const;
    void SetShadowValidLastFrame(bool isShadowValidLastFrame)
    {
        isShadowValidLastFrame_ = isShadowValidLastFrame;
    }

    // update parent's children rect including childRect and itself
    void MapAndUpdateChildrenRect();
    void UpdateSubTreeInfo(const RectI& clipRect);
    void UpdateParentChildrenRect(std::shared_ptr<RSRenderNode> parentNode) const;
    void NodePostPrepare(
        std::shared_ptr<RSSurfaceRenderNode> curSurfaceNode, const RectI& clipRect);

    void SetStaticCached(bool isStaticCached);
    virtual bool IsStaticCached() const;
    void SetNodeName(const std::string& nodeName);
    const std::string& GetNodeName() const;
    bool HasSubSurface() const;

    bool NeedInitCacheSurface();
    bool NeedInitCacheCompletedSurface();
    bool IsPureContainer() const;
    bool IsContentNode() const;
    bool IsPureBackgroundColor() const;
    void SetDrawNodeType(DrawNodeType nodeType);
    DrawNodeType GetDrawNodeType() const;

    using DrawCmdContainer = std::map<RSModifierType, std::list<std::shared_ptr<RSRenderModifier>>>;
    inline const DrawCmdContainer& GetDrawCmdModifiers() const
    {
        return drawCmdModifiers_;
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
    bool IsCacheCompletedSurfaceValid() const;
    bool IsCacheSurfaceValid() const;

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    void UpdateBackendTexture();
#endif

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
    bool GetCurFrameHasAnimation() const
    {
        return curFrameHasAnimation_;
    }
    void SetCurFrameHasAnimation(bool b)
    {
        curFrameHasAnimation_ = b;
    }

    float GetHDRBrightness() const;
    bool HasFilter() const;
    void SetHasFilter(bool hasFilter);
    bool GetCommandExecuted() const
    {
        return commandExecuted_;
    }

    void SetCommandExecuted(bool commandExecuted)
    {
        commandExecuted_ = commandExecuted;
    }

    std::recursive_mutex& GetSurfaceMutex() const;

    bool HasAbilityComponent() const;
    void SetHasAbilityComponent(bool hasAbilityComponent);

    uint32_t GetCacheSurfaceThreadIndex() const;

    uint32_t GetCompletedSurfaceThreadIndex() const;

    bool IsMainThreadNode() const;
    void SetIsMainThreadNode(bool isMainThreadNode);

    bool IsScale() const;
    void SetIsScale(bool isScale);

    bool IsScaleInPreFrame() const;
    void SetIsScaleInPreFrame(bool isScale);

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
    virtual void MarkFilterHasEffectChildren() {};
    virtual void OnFilterCacheStateChanged() {};

    // for blur filter cache
    virtual void CheckBlurFilterCacheNeedForceClearOrSave(bool rotationChanged = false,
        bool rotationStatusChanged = false);
    void UpdateLastFilterCacheRegion();
    void UpdateFilterRegionInSkippedSubTree(RSDirtyRegionManager& dirtyManager,
        const RSRenderNode& subTreeRoot, RectI& filterRect, const RectI& clipRect);
    void MarkFilterStatusChanged(bool isForeground, bool isFilterRegionChanged);
    void UpdateFilterCacheWithBackgroundDirty();
    virtual bool UpdateFilterCacheWithBelowDirty(const Occlusion::Region& belowDirty, bool isForeground = false);
    virtual void UpdateFilterCacheWithSelfDirty();
    bool IsBackgroundInAppOrNodeSelfDirty() const;
    void PostPrepareForBlurFilterNode(RSDirtyRegionManager& dirtyManager, bool needRequestNextVsync);
#ifdef RS_ENABLE_GPU
    void CheckFilterCacheAndUpdateDirtySlots(
        std::shared_ptr<DrawableV2::RSFilterDrawable>& filterDrawable, RSDrawableSlot slot);
#endif
    bool IsFilterCacheValid() const;
    bool IsAIBarFilter() const;
    bool CheckAndUpdateAIBarCacheStatus(bool intersectHwcDamage) const;
    void MarkForceClearFilterCacheWithInvisible();
    void MarkFilterInForegroundFilterAndCheckNeedForceClearCache(NodeId offscreenCanvasNodeId);

    void CheckGroupableAnimation(const PropertyId& id, bool isAnimAdd);
    bool IsForcedDrawInGroup() const;
    bool IsSuggestedDrawInGroup() const;
    void CheckDrawingCacheType();
    bool HasCacheableAnim() const { return hasCacheableAnim_; }
    enum NodeGroupType : uint8_t {
        NONE = 0,
        GROUPED_BY_ANIM = 1,
        GROUPED_BY_UI = GROUPED_BY_ANIM << 1,
        GROUPED_BY_USER = GROUPED_BY_UI << 1,
        GROUPED_BY_FOREGROUND_FILTER = GROUPED_BY_USER << 1,
        GROUP_TYPE_BUTT = GROUPED_BY_FOREGROUND_FILTER,
    };
    void MarkNodeGroup(NodeGroupType type, bool isNodeGroup, bool includeProperty);
    void MarkForegroundFilterCache();
    NodeGroupType GetNodeGroupType() const;
    bool IsNodeGroupIncludeProperty() const;

    void MarkNodeSingleFrameComposer(bool isNodeSingleFrameComposer, pid_t pid = 0);
    virtual bool GetNodeIsSingleFrameComposer() const;

    // mark cross node in physical extended screen model
    bool IsCrossNode() const;

    std::string QuickGetNodeDebugInfo();

    // arkui mark
    void MarkSuggestOpincNode(bool isOpincNode, bool isNeedCalculate);

    void UpdateOpincParam();

    /////////////////////////////////////////////

    void SetSharedTransitionParam(const std::shared_ptr<SharedTransitionParam>& sharedTransitionParam);
    const std::shared_ptr<SharedTransitionParam>& GetSharedTransitionParam() const;

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
    inline void InActivateDisplaySync()
    {
        displaySync_ = nullptr;
    }
    void UpdateDisplaySyncRange();

    void MarkNonGeometryChanged();

    void ApplyModifier(RSModifierContext& context, std::shared_ptr<RSRenderModifier> modifier);
    void ApplyModifiers();
    void ApplyPositionZModifier();
    virtual void UpdateRenderParams();
    void SetCrossNodeOffScreenStatus(CrossNodeOffScreenRenderDebugType isCrossNodeOffscreenOn_);
    void UpdateDrawingCacheInfoBeforeChildren(bool isScreenRotation);
    void UpdateDrawingCacheInfoAfterChildren(bool isInBlackList = false);

    virtual RectI GetFilterRect() const;
    void CalVisibleFilterRect(const std::optional<RectI>& clipRect);
    void SetIsUsedBySubThread(bool isUsedBySubThread);
    bool GetIsUsedBySubThread() const;

    void SetLastIsNeedAssignToSubThread(bool lastIsNeedAssignToSubThread);
    bool GetLastIsNeedAssignToSubThread() const;

    void SetIsTextureExportNode(bool isTextureExportNode)
    {
        isTextureExportNode_ = isTextureExportNode;
    }

    bool HasHpaeBackgroundFilter() const;

#ifdef RS_ENABLE_STACK_CULLING
    void SetFullSurfaceOpaqueMarks(const std::shared_ptr<RSRenderNode> curSurfaceNodeParam);
    void SetSubNodesCovered();
    void ResetSubNodesCovered();
    bool isFullSurfaceOpaquCanvasNode_ = false;
    bool hasChildFullSurfaceOpaquCanvasNode_ = false;
    bool isCoveredByOtherNode_ = false;
#define MAX_COLD_DOWN_NUM 20
    int32_t coldDownCounter_ = 0;
#endif

    void MarkParentNeedRegenerateChildren() const;

    void ResetChildUifirstSupportFlag()
    {
        isChildSupportUifirst_ = true;
    }

    void UpdateChildUifirstSupportFlag(bool b)
    {
        isChildSupportUifirst_ = isChildSupportUifirst_ && b;
    }

    virtual bool GetUifirstSupportFlag();

    virtual void MergeOldDirtyRect()
    {
        return;
    }
#ifdef RS_ENABLE_GPU
    std::unique_ptr<RSRenderParams>& GetStagingRenderParams();

    // Deprecated! Do not use this interface.
    // This interface has crash risks and will be deleted in later versions.
    const std::unique_ptr<RSRenderParams>& GetRenderParams() const;
#endif
    void UpdatePointLightDirtySlot();
    void AccumulateDirtyInOcclusion(bool isOccluded);
    void RecordCurDirtyStatus();
    void AccumulateDirtyStatus();
    void ResetAccumulateDirtyStatus();
    void RecordCurDirtyTypes();
    void AccumulateDirtyTypes();
    void ResetAccumulateDirtyTypes();
    void SetUifirstSyncFlag(bool needSync);
    void SetUifirstSkipPartialSync(bool skip)
    {
        uifirstSkipPartialSync_ = skip;
    }

    void SetForceUpdateByUifirst(bool b)
    {
        forceUpdateByUifirst_ = b;
    }

    bool GetForceUpdateByUifirst() const
    {
        return forceUpdateByUifirst_;
    }

    MultiThreadCacheType GetLastFrameUifirstFlag()
    {
        return lastFrameUifirstFlag_;
    }

    void SetLastFrameUifirstFlag(MultiThreadCacheType b)
    {
        lastFrameUifirstFlag_ = b;
    }

    void SkipSync()
    {
        OnSkipSync();
    }
    void Sync()
    {
        OnSync();
    }
    void AddToPendingSyncList();
    const std::weak_ptr<RSContext> GetContext() const
    {
        return context_;
    }

    // will be abandoned
    void MarkUifirstNode(bool isUifirstNode);
    // Mark uifirst leash node
    void MarkUifirstNode(bool isForceFlag, bool isUifirstEnable);
    bool GetUifirstNodeForceFlag() const;

    void SetUIFirstSwitch(RSUIFirstSwitch uiFirstSwitch);
    RSUIFirstSwitch GetUIFirstSwitch() const;

    void SetOccludedStatus(bool occluded);
    const RectI GetFilterCachedRegion() const;
    virtual bool EffectNodeShouldPaint() const { return true; };
    virtual bool FirstFrameHasEffectChildren() const { return false; }
    virtual void MarkClearFilterCacheIfEffectChildrenChanged() {}
    bool HasBlurFilter() const;
    void SetChildrenHasSharedTransition(bool hasSharedTransition);
    virtual bool SkipFrame(uint32_t refreshRate, uint32_t skipFrameInterval) { return false; }
    void RemoveChildFromFulllist(NodeId nodeId);
    void SetStartingWindowFlag(bool startingFlag);
    bool GetStartingWindowFlag() const
    {
        return startingWindowFlag_;
    }
    void SetChildrenHasUIExtension(bool SetChildrenHasUIExtension);
    bool ChildrenHasUIExtension() const
    {
        return childrenHasUIExtension_;
    }

    // temporary used for dfx/surfaceHandler/canvas drawing render node
    DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr GetRenderDrawable() const
    {
        return renderDrawable_;
    }
    void MarkBlurIntersectWithDRM(bool intersectWithDRM, bool isDark);
    bool GetIsFullChildrenListValid() const
    {
        return isFullChildrenListValid_;
    }

    virtual RSSurfaceNodeAbilityState GetAbilityState() const { return RSSurfaceNodeAbilityState::FOREGROUND; }

    int32_t GetPreparedDisplayOffsetX() const
    {
        return preparedDisplayOffsetX_;
    }
    void SetPreparedDisplayOffsetX(int32_t offsetX)
    {
        preparedDisplayOffsetX_ = offsetX;
    }
    int32_t GetPreparedDisplayOffsetY() const
    {
        return preparedDisplayOffsetY_;
    }
    void SetPreparedDisplayOffsetY(int32_t offsetY)
    {
        preparedDisplayOffsetY_ = offsetY;
    }
    bool IsFirstLevelCrossNode() const
    {
        return isFirstLevelCrossNode_;
    }
    void SetFirstLevelCrossNode(bool isFirstLevelCrossNode)
    {
        isFirstLevelCrossNode_ = isFirstLevelCrossNode;
    }
    void SetHdrNum(bool flag, NodeId instanceRootNodeId, HDRComponentType hdrType);

    void SetEnableHdrEffect(bool enableHdrEffect);

    void SetIsAccessibilityConfigChanged(bool isAccessibilityConfigChanged)
    {
        isAccessibilityConfigChanged_ = isAccessibilityConfigChanged;
    }

    bool IsAccessibilityConfigChanged() const
    {
        return isAccessibilityConfigChanged_;
    }

    // recursive update subSurfaceCnt
    void UpdateSubSurfaceCnt(int updateCnt);

    NodeId GetScreenNodeId() const
    {
        return screenNodeId_;
    }

    NodeId GetLogicalDisplayNodeId() const
    {
        return logicalDisplayNodeId_;
    }

    void ProcessBehindWindowOnTreeStateChanged();
    void ProcessBehindWindowAfterApplyModifiers();
    void UpdateDrawableBehindWindow();
    virtual bool NeedUpdateDrawableBehindWindow() const { return false; }
    virtual bool NeedDrawBehindWindow() const { return false; }
    virtual void AddChildBlurBehindWindow(NodeId id) {}
    virtual void RemoveChildBlurBehindWindow(NodeId id) {}
    virtual void CalDrawBehindWindowRegion() {}
    virtual RectI GetBehindWindowRegion() const { return {}; };

    void SetAbsRotation(float degree)
    {
        absRotation_ = degree;
    }

    float GetAbsRotation() const
    {
        return absRotation_;
    }

    CurFrameInfoDetail& GetCurFrameInfoDetail() { return curFrameInfoDetail_; }

    bool HasUnobscuredUEC() const;
    void SetHasUnobscuredUEC();
    void SetOldDirtyInSurface(RectI oldDirtyInSurface);

    // Enable HWCompose
    RSHwcRecorder& GetHwcRecorder() { return hwcRecorder_; }

    RSOpincCache& GetOpincCache()
    {
        return opincCache_;
    }

    void SetHasWhiteListNode(ScreenId screenId, bool hasWhiteListNode)
    {
        hasVirtualScreenWhiteList_[screenId] |= hasWhiteListNode;
    }

    void UpdateVirtualScreenWhiteListInfo();

    bool IsForegroundFilterEnable();
    void ResetPixelStretchSlot();
    bool CanFuzePixelStretch();

    void ResetRepaintBoundaryInfo();
    void UpdateRepaintBoundaryInfo(RSRenderNode& node);
    uint32_t GetRepaintBoundaryWeight();

    void ClearSubtreeParallelNodes();
    void UpdateSubTreeParallelNodes();
    void MergeSubtreeParallelNodes(RSRenderNode& childNode);
    std::unordered_set<NodeId>& GetSubtreeParallelNodes();

    void SetNeedUseCmdlistDrawRegion(bool needUseCmdlistDrawRegion);
    bool GetNeedUseCmdlistDrawRegion();

protected:
    void ResetDirtyStatus();

    // only be called in node's constructor if it's generated by render service
    static NodeId GenerateId();

    virtual void OnApplyModifiers() {}

    enum class NodeDirty {
        CLEAN = 0,
        DIRTY,
    };
    inline void SetClean()
    {
        isNewOnTree_ = false;
        isContentDirty_ = false;
        dirtyStatus_ = NodeDirty::CLEAN;
    }

    static void DumpNodeType(RSRenderNodeType nodeType, std::string& out);

    void DumpSubClassNode(std::string& out) const;
    void DumpDrawCmdModifiers(std::string& out) const;
    void DumpModifiers(std::string& out) const;

    virtual void OnTreeStateChanged();
    void AddSubSurfaceUpdateInfo(SharedPtr curParent, SharedPtr preParent);

    static void SendCommandFromRT(std::unique_ptr<RSCommand>& command, NodeId nodeId);
    void AddGeometryModifier(const std::shared_ptr<RSRenderModifier>& modifier);
    void AddUIFilterModifier(const std::shared_ptr<RSRenderModifier>& modifier);

    virtual void InitRenderParams();
    virtual void OnSync();
    virtual void OnSkipSync();
    virtual void ClearResource() {};
    virtual void ClearNeverOnTree() {};

    void AddUIExtensionChild(SharedPtr child);
    void MoveUIExtensionChild(SharedPtr child);
    void RemoveUIExtensionChild(SharedPtr child);
    bool NeedRoutedBasedOnUIExtension(SharedPtr child);

    void UpdateDrawableVecV2();
    void ClearDrawableVec2();

    void DrawPropertyDrawable(RSDrawableSlot slot, RSPaintFilterCanvas& canvas);
    void DrawPropertyDrawableRange(RSDrawableSlot begin, RSDrawableSlot end, RSPaintFilterCanvas& canvas);

#ifdef RS_ENABLE_GPU
    std::shared_ptr<DrawableV2::RSFilterDrawable> GetFilterDrawable(bool isForeground) const;
    virtual void MarkFilterCacheFlags(std::shared_ptr<DrawableV2::RSFilterDrawable>& filterDrawable,
        RSDirtyRegionManager& dirtyManager, bool needRequestNextVsync);
    bool IsForceClearOrUseFilterCache(std::shared_ptr<DrawableV2::RSFilterDrawable>& filterDrawable);
#endif
    void UpdateDirtySlotsAndPendingNodes(RSDrawableSlot slot);
    // if true, it means currently it's in partial render mode and this node is intersect with dirtyRegion
    bool isRenderUpdateIgnored_ = false;
    bool isShadowValidLastFrame_ = false;
    bool IsSelfDrawingNode() const;
    bool lastFrameHasAnimation_ = false;
    bool needClearSurface_ = false;
    bool isBootAnimation_ = false;
    bool lastFrameHasVisibleEffect_ = false;
    bool waitSync_ = false;
    mutable bool isFullChildrenListValid_ = true;
    mutable bool isChildrenSorted_ = true;
    mutable bool childrenHasSharedTransition_ = false;
    bool isOnTheTree_ = false;
    bool isChildSupportUifirst_ = true;
    bool isUifirstNode_ = true;
    bool isForceFlag_ = false;
    bool isUifirstEnable_ = false;
    bool lastFrameSynced_ = true;
    bool srcOrClipedAbsDrawRectChangeFlag_ = false;
    bool startingWindowFlag_ = false;
    bool isNodeSingleFrameComposer_ = false;
    bool childHasSharedTransition_ = false;
    bool flagIntersectWithDRM_ = false;
    std::atomic<bool> isStaticCached_ = false;
    RSUIFirstSwitch uiFirstSwitch_ = RSUIFirstSwitch::NONE;
    NodeDirty dirtyStatus_ = NodeDirty::CLEAN;
    NodeDirty curDirtyStatus_ = NodeDirty::CLEAN;
    int isUifirstDelay_ = 0;
    NodeId drawingCacheRootId_ = INVALID_NODEID;
    mutable std::shared_ptr<DrawableV2::RSRenderNodeDrawableAdapter> renderDrawable_;
    std::shared_ptr<RSSingleFrameComposer> singleFrameComposer_ = nullptr;
    std::unique_ptr<RSRenderParams> stagingRenderParams_;
    RSPaintFilterCanvas::SaveStatus renderNodeSaveCount_;
    RectI filterRegion_;
    ModifierDirtyTypes dirtyTypes_;
    ModifierDirtyTypes curDirtyTypes_;

    ModifierNG::ModifierDirtyTypes dirtyTypesNG_;
    ModifierNG::ModifierDirtyTypes curDirtyTypesNG_;

    CurFrameInfoDetail curFrameInfoDetail_;

    // Enable HWCompose
    RSHwcRecorder hwcRecorder_;

private:
    // mark cross node in physical extended screen model
    bool isRepaintBoundary_ = false;
    bool isCrossNode_ = false;
    bool isCloneCrossNode_ = false;
    bool isFirstLevelCrossNode_ = false;
    bool autoClearCloneNode_ = false;
    uint8_t nodeGroupType_ = NodeGroupType::NONE;
    bool shouldPaint_ = true;
    bool isAccumulatedClipFlagChanged_ = false;
    bool geoUpdateDelay_ = false;
    int subSurfaceCnt_ = 0;
    bool selfAddForSubSurfaceCnt_ = false;
    bool visitedForSubSurfaceCnt_ = false;
    // drawing group cache
    RSDrawingCacheType drawingCacheType_ = RSDrawingCacheType::DISABLED_CACHE;
    bool isTextureExportNode_ = false;
    uint8_t drawableVecStatus_ = 0;
    bool isOnlyBasicGeoTransform_ = true;
    // Test pipeline
    bool addedToPendingSyncList_ = false;
    bool drawCmdListNeedSync_ = false;
    bool drawableVecNeedClear_ = false;
    bool unobscuredUECChildrenNeedSync_ = false;
    // accumulate all children's region rect for dirty merging when any child has been removed
    bool hasRemovedChild_ = false;
    bool lastFrameSubTreeSkipped_ = false;
    bool curFrameHasAnimation_ = false;
    bool childHasVisibleFilter_ = false;  // only collect visible children filter status
    bool childHasVisibleEffect_ = false;  // only collect visible children has useeffect
    bool hasChildrenOutOfRect_ = false;

    bool isSubTreeDirty_ = false;
    bool isContentDirty_ = false;
    bool nodeGroupIncludeProperty_ = false;
    bool isNewOnTree_ = false;
    bool geometryChangeNotPerceived_ = false;
    // node region, only used in selfdrawing node dirty
    bool isSelfDrawingNode_ = false;
    bool isDirtyRegionUpdated_ = false;
    bool isLastVisible_ = false;
    bool hasAccumulatedClipFlag_ = false;
    // since preparation optimization would skip child's dirtyFlag(geoDirty) update
    // it should be recorded and update if marked dirty again
    bool lastFrameHasChildrenOutOfRect_ = false;
    MultiThreadCacheType lastFrameUifirstFlag_ = MultiThreadCacheType::NONE;
    bool isContainBootAnimation_ = false;
    CacheType cacheType_ = CacheType::NONE;
    bool isDrawingCacheChanged_ = false;
    bool drawingCacheNeedUpdate_ = false;
    bool isMainThreadNode_ = true;
    bool isScale_ = false;
    bool isScaleInPreFrame_ = false;
    bool hasFilter_ = false;
    bool hasAbilityComponent_ = false;
    bool isAncestorDirty_ = false;
    bool isParentLeashWindow_ = false;
    bool isParentScbScreen_ = false;
    bool hasCacheableAnim_ = false;
    NodePriorityType priority_ = NodePriorityType::MAIN_PRIORITY;
    bool lastIsNeedAssignToSubThread_ = false;
    uint8_t drawableVecStatusV1_ = 0;
    bool uifirstNeedSync_ = false; // both cmdlist&param
    bool uifirstSkipPartialSync_ = false;
    bool forceUpdateByUifirst_ = false;
    bool backgroundFilterRegionChanged_ = false;
    bool backgroundFilterInteractWithDirty_ = false;
    bool foregroundFilterRegionChanged_ = false;
    bool foregroundFilterInteractWithDirty_ = false;
    bool isOccluded_ = false;
    // for UIExtension info collection
    bool childrenHasUIExtension_ = false;
    bool isAccessibilityConfigChanged_ = false;
    const bool isPurgeable_;
    DrawNodeType drawNodeType_ = DrawNodeType::PureContainerType;
    std::atomic<bool> isTunnelHandleChange_ = false;
    std::atomic<bool> isCacheSurfaceNeedUpdate_ = false;
    std::atomic<bool> commandExecuted_ = false;
    std::atomic_bool isUsedBySubThread_ = false;
    int32_t crossScreenNum_ = 0;
    // shadowRectOffset means offset between shadowRect and absRect of node
    int shadowRectOffsetX_ = 0;
    int shadowRectOffsetY_ = 0;
    int32_t preparedDisplayOffsetX_ = 0;
    int32_t preparedDisplayOffsetY_ = 0;
    uint32_t disappearingTransitionCount_ = 0;
    float globalAlpha_ = 1.0f;
    // collect subtree's surfaceNode including itself
    uint32_t cacheSurfaceThreadIndex_ = UNI_MAIN_THREAD_INDEX;
    uint32_t completedSurfaceThreadIndex_ = UNI_MAIN_THREAD_INDEX;
    OutOfParentType outOfParent_ = OutOfParentType::UNKNOWN;
    // Only use in RSRenderNode::DrawCacheSurface to calculate scale factor
    float boundsWidth_ = 0.0f;
    float boundsHeight_ = 0.0f;
    pid_t appPid_ = 0;
    uint64_t uiContextToken_ = 0;
    NodeId id_;
    NodeId instanceRootNodeId_ = INVALID_NODEID;
    NodeId firstLevelNodeId_ = INVALID_NODEID;
    NodeId uifirstRootNodeId_ = INVALID_NODEID;
    NodeId screenNodeId_ = INVALID_NODEID;
    NodeId logicalDisplayNodeId_ = INVALID_NODEID;
    std::shared_ptr<SharedTransitionParam> sharedTransitionParam_;
    // bounds and frame modifiers must be unique
    std::shared_ptr<RSRenderModifier> boundsModifier_;
    std::shared_ptr<RSRenderModifier> frameModifier_;

    std::shared_ptr<ModifierNG::RSRenderModifier> boundsModifierNG_;
    std::shared_ptr<ModifierNG::RSRenderModifier> frameModifierNG_;

    // Note: Make sure that fullChildrenList_ is never nullptr. Otherwise, the caller using
    // `for (auto child : *GetSortedChildren()) { ... }` will crash.
    // When an empty list is needed, use EmptyChildrenList instead.
    static const inline auto EmptyChildrenList = std::make_shared<const std::vector<std::shared_ptr<RSRenderNode>>>();
    ChildrenListSharedPtr fullChildrenList_ = EmptyChildrenList ;
    std::shared_ptr<RSRenderDisplaySync> displaySync_ = nullptr;
    std::shared_ptr<Drawing::Surface> cacheSurface_ = nullptr;
    std::shared_ptr<Drawing::Surface> cacheCompletedSurface_ = nullptr;
    std::shared_ptr<RectF> drawRegion_ = nullptr;
    std::shared_ptr<std::unordered_set<std::shared_ptr<RSRenderNode>>> stagingUECChildren_ =
        std::make_shared<std::unordered_set<std::shared_ptr<RSRenderNode>>>();
    WeakPtr sourceCrossNode_;
    WeakPtr curCloneNodeParent_;
    std::weak_ptr<RSContext> context_ = {};
    WeakPtr parent_;
    // including enlarged draw region
    RectF selfDrawRect_;
    RectI localShadowRect_;
    RectI localOutlineRect_;
    RectI localPixelStretchRect_;
    RectI localForegroundEffectRect_;
    RectI localDistortionEffectRect_;
    // map parentMatrix
    RectI absDrawRect_;
    RectF absDrawRectF_;
    RectI oldAbsDrawRect_;
    // round in by absDrawRectF_, only used for opaque region calculations
    RectI innerAbsDrawRect_;
    // map parentMatrix by cmdlist draw region
    RectI absCmdlistDrawRect_;
    RectF absCmdlistDrawRectF_;
    RectI oldAbsCmdlistDrawRect_;
    RectI oldDirty_;
    RectI oldDirtyInSurface_;
    RectI childrenRect_;
    RectI oldChildrenRect_;
    RectI removedChildrenRect_;
    RectI oldClipRect_;
    // aim to record children rect in abs coords, without considering clip
    RectI absChildrenRect_;
    // aim to record current frame clipped children dirty region, in abs coords
    RectI subTreeDirtyRegion_;
    Vector4f globalCornerRadius_{ 0.f, 0.f, 0.f, 0.f };
    RectF selfDrawingNodeDirtyRect_;
    RectI selfDrawingNodeAbsDirtyRect_;
    RectF selfDrawingNodeAbsDirtyRectF_;
    // used in old pipline
    RectI oldRectFromRenderProperties_;
    // for blur cache
    RectI lastFilterRegion_;
    std::vector<SharedPtr> cloneCrossNodeVec_;
    bool hasVisitedCrossNode_ = false;
    std::map<PropertyId, std::shared_ptr<RSRenderModifier>> modifiers_;

    std::array<std::vector<std::shared_ptr<ModifierNG::RSRenderModifier>>, ModifierNG::MODIFIER_TYPE_COUNT>
        modifiersNG_;
    std::map<PropertyId, std::shared_ptr<RSRenderPropertyBase>> properties_;

    std::unordered_set<RSDrawableSlot> dirtySlots_;
    DrawCmdIndex stagingDrawCmdIndex_;
    std::vector<Drawing::RecordingCanvas::DrawFunc> stagingDrawCmdList_;
    std::vector<NodeId> visibleFilterChild_;
    std::unordered_set<NodeId> visibleEffectChild_;
    Drawing::Matrix oldMatrix_;
    Drawing::Matrix oldAbsMatrix_;
    RSDrawable::Vec drawableVec_;
    RSAnimationManager animationManager_;
    RSOpincCache opincCache_;
    std::unordered_set<NodeId> subtreeParallelNodes_;
    bool isAllChildRepaintBoundary_ = false;
    uint32_t repaintBoundaryWeight_ = 0;

    std::list<WeakPtr> children_;
    std::set<NodeId> preFirstLevelNodeIdSet_ = {};
    std::list<std::pair<SharedPtr, uint32_t>> disappearingChildren_;
    std::mutex childrenMutex_;
    friend class RSRenderPropertyBase;
    friend class RSRenderTransition;
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::BackendTexture cacheBackendTexture_;
    Drawing::BackendTexture cacheCompletedBackendTexture_;
#ifdef RS_ENABLE_VK
    NativeBufferUtils::VulkanCleanupHelper* cacheCleanupHelper_ = nullptr;
    NativeBufferUtils::VulkanCleanupHelper* cacheCompletedCleanupHelper_ = nullptr;
#endif
    bool isTextureValid_ = false;
#endif
    std::string nodeName_ = "";
    std::unordered_set<NodeId> curCacheFilterRects_ = {};
    std::unordered_set<NodeId> visitedCacheRoots_ = {};
    mutable std::recursive_mutex surfaceMutex_;
    ClearCacheSurfaceFunc clearCacheSurfaceFunc_ = nullptr;

    std::unordered_map<ScreenId, bool> hasVirtualScreenWhiteList_;

    RSProperties renderProperties_;
    DrawCmdContainer drawCmdModifiers_;

    // for blur effct count
    static std::unordered_map<pid_t, size_t> blurEffectCounter_;
    // The angle at which the node rotates about the Z-axis
    float absRotation_ = 0.f;
    void UpdateBlurEffectCounter(int deltaCount);
    int GetBlurEffectDrawbleCount();

    bool enableHdrEffect_ = false;

    bool needUseCmdlistDrawRegion_ = false;
    RectF cmdlistDrawRegion_;

    void SetParent(WeakPtr parent);
    void ResetParent();
    void UpdateSrcOrClipedAbsDrawRectChangeState(const RectI& clipRect);
    bool IsUifirstArkTsCardNode();
    virtual void OnResetParent() {}

    void GenerateFullChildrenList();
    void ResortChildren();
    bool ShouldClearSurface();

    void InternalRemoveSelfFromDisappearingChildren();
    void FallbackAnimationsToRoot();
    void FilterModifiersByPid(pid_t pid);

    bool UpdateBufferDirtyRegion(RectI& dirtyRect, const RectI& drawRegion);
    void CollectAndUpdateLocalShadowRect();
    void CollectAndUpdateLocalOutlineRect();
    void CollectAndUpdateLocalPixelStretchRect();
    void CollectAndUpdateLocalForegroundEffectRect();
    void CollectAndUpdateLocalDistortionEffectRect();
    // update drawrect based on self's info
    void UpdateBufferDirtyRegion();
    bool UpdateSelfDrawRect();
    bool CheckAndUpdateGeoTrans(std::shared_ptr<RSObjAbsGeometry>& geoPtr);
    void UpdateAbsDirtyRegion(RSDirtyRegionManager& dirtyManager, const RectI& clipRect);
    void UpdateDirtyRegion(RSDirtyRegionManager& dirtyManager, bool geoDirty, const std::optional<RectI>& clipRect);
    void UpdateDrawRect(bool& accumGeoDirty, const RectI& clipRect, const Drawing::Matrix& parentSurfaceMatrix);
    void UpdateFullScreenFilterCacheRect(RSDirtyRegionManager& dirtyManager, bool isForeground) const;
    void ValidateLightResources();
    void UpdateShouldPaint(); // update node should paint state in apply modifier stage

    std::shared_ptr<Drawing::Image> GetCompletedImage(
        RSPaintFilterCanvas& canvas, uint32_t threadIndex, bool isUIFirst);

    void UpdateDisplayList();
    void UpdateShadowRect();

    void RecordCloneCrossNode(SharedPtr node);

    void OnRegister(const std::weak_ptr<RSContext>& context);

    void ChildrenListDump(std::string& out) const;

    void ResetAndApplyModifiers();

    void CalcCmdlistDrawRegionFromOpItem(std::shared_ptr<ModifierNG::RSRenderModifier> modifier);

    friend class DrawFuncOpItem;
    friend class RSContext;
    friend class RSMainThread;
    friend class RSPointerWindowManager;
    friend class RSModifierDrawable;
    friend class RSProxyRenderNode;
    friend class RSRenderNodeMap;
    friend class RSRenderThread;
    friend class RSRenderTransition;
    friend class DrawableV2::RSRenderNodeDrawableAdapter;
    friend class DrawableV2::RSChildrenDrawable;
    friend class DrawableV2::RSRenderNodeShadowDrawable;
    friend class ModifierNG::RSRenderModifier;
    friend class ModifierNG::RSForegroundFilterRenderModifier;
    friend class ModifierNG::RSBackgroundFilterRenderModifier;
#ifdef RS_PROFILER_ENABLED
    friend class RSProfiler;
#endif
};
// backward compatibility
using RSBaseRenderNode = RSRenderNode;

struct SharedTransitionParam {
    SharedTransitionParam(RSRenderNode::SharedPtr inNode, RSRenderNode::SharedPtr outNode, bool isInSameWindow);

    RSRenderNode::SharedPtr GetPairedNode(const NodeId nodeId) const;
    bool IsLower(const NodeId nodeId) const;
    void UpdateHierarchy(const NodeId nodeId);
    bool IsInAppTranSition() const
    {
        return !crossApplication_;
    }
    void InternalUnregisterSelf();
    bool HasRelation();
    void SetNeedGenerateDrawable(const bool needGenerateDrawable);
    static void UpdateUnpairedSharedTransitionMap(const std::shared_ptr<SharedTransitionParam>& param);
    RSB_EXPORT std::string Dump() const;
    RSB_EXPORT void ResetRelation();
    RSB_EXPORT void GenerateDrawable(const RSRenderNode& node);

    std::weak_ptr<RSRenderNode> inNode_;
    std::weak_ptr<RSRenderNode> outNode_;
    NodeId inNodeId_;
    NodeId outNodeId_;
    bool needGenerateDrawable_ = false;

    RSB_EXPORT static std::map<NodeId, std::weak_ptr<SharedTransitionParam>> unpairedShareTransitions_;
    bool paired_ = true; // treated as paired by default, until we fail to pair them

private:
    enum class NodeHierarchyRelation : uint8_t {
        UNKNOWN = -1,
        IN_NODE_BELOW_OUT_NODE = 0,
        IN_NODE_ABOVE_OUT_NODE = 1,
    };
    NodeHierarchyRelation relation_ = NodeHierarchyRelation::UNKNOWN;
    bool crossApplication_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_H
