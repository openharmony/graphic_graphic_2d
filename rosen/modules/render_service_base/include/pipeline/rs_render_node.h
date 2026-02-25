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
#include "modifier_ng/rs_modifier_ng_type.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_display_sync.h"
#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {
namespace DrawableV2 {
class RSChildrenDrawable;
class RSColorPickerDrawable;
class RSFilterDrawable;
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
    uint16_t curFramePrepareSeqNum = 0;
    uint16_t curFramePostPrepareSeqNum = 0;
    uint32_t curFrameVsyncId = 0;
    bool curFrameSubTreeSkipped = false;
    bool curFrameReverseChildren = false;
};

class RSB_EXPORT RSRenderNode : public std::enable_shared_from_this<RSRenderNode> {
public:
    using WeakPtr = std::weak_ptr<RSRenderNode>;
    using WeakPtrSet = std::set<WeakPtr, std::owner_less<WeakPtr>>;
    using SharedPtr = std::shared_ptr<RSRenderNode>;
    using ModifierNGContainer = std::vector<std::shared_ptr<ModifierNG::RSRenderModifier>>;
    using ModifiersNGMap = std::map<ModifierNG::RSModifierType, ModifierNGContainer>;
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

    virtual void SetBootAnimation(bool isBootAnimation);
    virtual bool GetBootAnimation() const;

    virtual bool GetGlobalPositionEnabled() const;

    void MoveChild(SharedPtr child, int index);
    void RemoveChild(SharedPtr child, bool skipTransition = false);
    void ClearChildren();
    void SetUIContextToken(uint64_t token)
    {
        uiContextToken_ = token;
        if (std::find(uiContextTokenList_.begin(), uiContextTokenList_.end(), token) == uiContextTokenList_.end()) {
            uiContextTokenList_.emplace_back(token);
        }
    }
    uint64_t GetUIContextToken() const
    {
        return uiContextToken_;
    }
    std::vector<uint64_t> GetUIContextTokenList() const
    {
        return uiContextTokenList_;
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

    bool IsSubTreeDirty() const
    {
        return isSubTreeDirty_;
    }
    void SetSubTreeDirty(bool val)
    {
        isSubTreeDirty_ = val;
    }
    // Only used in quick skip prepare phase
    void SetForcePrepare(bool isForcePrepare)
    {
        isForcePrepare_ = isForcePrepare;
    }
    bool IsForcePrepare() const
    {
        return isForcePrepare_;
    }
    void SetParentTreeDirty(bool isParentTreeDirty)
    {
        isParentTreeDirty_ = isParentTreeDirty;
    }
    bool IsParentTreeDirty() const
    {
        return isParentTreeDirty_;
    }
    bool IsTreeStateChangeDirty() const
    {
        return isTreeStateChangeDirty_;
    }
    void SetTreeStateChangeDirty(bool val)
    {
        isTreeStateChangeDirty_ = val;
    }
    void SetParentSubTreeDirty();
    // set when tree state changed or in uicapture task for each parent node recursively
    void SetParentTreeStateChangeDirty(bool isUpdateAllParentNode = false);
    // set in uicapture task for each child node recursively
    void SetChildrenTreeStateChangeDirty();
    // attention: current all base node's dirty ops causing content dirty
    // if there is any new dirty op, check it
    bool IsContentDirty() const;
    void SetContentDirty();
    void ResetIsOnlyBasicGeoTransform()
    {
        isOnlyBasicGeoTransform_ = true;
    }
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

    bool IsFirstLevelNode()
    {
        return id_ == firstLevelNodeId_;
    }
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
        return filterRegionInfo_ != nullptr ? filterRegionInfo_->filterRegion_ : RectI();
    }

    inline RectI GetDefaultFilterRegion() const
    {
        return filterRegionInfo_ != nullptr ? filterRegionInfo_->defaultFilterRegion_ : RectI();
    }

    inline bool HasForceSubmit() const
    {
        return hasForceSubmit_;
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
    ChildrenListSharedPtr GetSortedChildren() const;
    void CollectAllChildren(const std::shared_ptr<RSRenderNode>& node, std::vector<NodeId>& vec);
    uint32_t GetChildrenCount() const
    {
        return children_.size();
    }
    std::shared_ptr<RSRenderNode> GetFirstChild() const
    {
        return children_.empty() ? nullptr : children_.front().lock();
    }
    std::list<WeakPtr> GetChildrenList() const;

    void DumpTree(int32_t depth, std::string& out) const;

    virtual bool HasDisappearingTransition(bool recursive = true) const;

    void SetTunnelHandleChange(bool change)
    {
        isTunnelHandleChange_ = change;
    }
    bool GetTunnelHandleChange() const
    {
        return isTunnelHandleChange_;
    }

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

    void ResetHasRemovedChild()
    {
        hasRemovedChild_ = false;
    }
    bool HasRemovedChild() const
    {
        return hasRemovedChild_;
    }
    inline void ResetChildrenRect()
    {
        childrenRect_.Clear();
    }
    RectI GetChildrenRect() const
    {
        return childrenRect_;
    }
    RectI GetRemovedChildrenRect() const;

    bool ChildHasVisibleFilter() const
    {
        return childHasVisibleFilter_;
    }
    void SetChildHasVisibleFilter(bool val);
    bool ChildHasVisibleEffect() const
    {
        return childHasVisibleEffect_;
    }
    void SetChildHasVisibleEffect(bool val);
    const std::vector<NodeId>& GetVisibleFilterChild() const
    {
        return visibleFilterChild_;
    }
    void UpdateVisibleFilterChild(RSRenderNode& childNode);
    const std::unordered_set<NodeId>& GetVisibleEffectChild() const
    {
        return visibleEffectChild_;
    }
    /* 1. add childNode to visibleEffectChild_ when it has useEffect and non-empty oldDirtySurface
     * (refer to GetVisibleEffectRegion)
     * 2. add childNode's visibleEffectChild_ to visibleEffectChild_
     */
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
    virtual void ResetChildRelevantFlags();
    // accumulate all valid children's area
    void UpdateChildrenRect(const RectI& subRect);
    void UpdateCurCornerInfo(Vector4f& curCornerRadius, RectI& curCornerRect);
    void SetDirty(bool forceAddToActiveList = false);

    void ResetDirtyFlag()
    {
        SetClean();
        GetMutableRenderProperties().ResetDirty();
    }

    virtual void AddDirtyType(ModifierNG::RSModifierType type)
    {
        dirtyTypesNG_.set(static_cast<int>(type), true);
    }

    std::tuple<bool, bool, bool> Animate(
        int64_t timestamp, int64_t& minLeftDelayTime, int64_t period = 0, bool isDisplaySyncEnabled = false);

    // check if all clip properties are enabled
    bool IsClipBound() const;
    // clipRect has value in UniRender when calling PrepareCanvasRenderNode, else it is nullopt
    const RectF& GetSelfDrawRect() const
    {
        return selfDrawRect_;
    }
    const RectI& GetAbsDrawRect() const
    {
        return absDrawRect_;
    }
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
    RectI GetOldDirtyInSurface() const
    {
        return oldDirtyInSurface_;
    }
    // clip rect of last frame before post prepare, current frame after post prepare
    RectI GetOldClipRect() const
    {
        return oldClipRect_;
    }

    const Drawing::Matrix& GetOldAbsMatrix() const
    {
        return oldAbsMatrix_;
    }

    bool IsDirtyRegionUpdated() const
    {
        return isDirtyRegionUpdated_;
    }
    void CleanDirtyRegionUpdated()
    {
        isDirtyRegionUpdated_ = false;
    }

    std::shared_ptr<RSRenderPropertyBase> GetProperty(PropertyId id);
    void RegisterProperty(const std::shared_ptr<RSRenderPropertyBase>& property);
    void UnregisterProperty(const std::shared_ptr<RSRenderPropertyBase>& property);
    void UnregisterProperty(PropertyId id);

    void RemoveModifier(const PropertyId& id);

    void AddModifier(const std::shared_ptr<ModifierNG::RSRenderModifier>& modifier, bool isSingleFrameComposer = false);
    void RemoveModifier(ModifierNG::RSModifierType type, ModifierId id);
    void RemoveModifierNG(ModifierId id);
    void RemoveAllModifiersNG();
    std::shared_ptr<ModifierNG::RSRenderModifier> GetModifierNG(
        ModifierNG::RSModifierType type, ModifierId id = 0) const;
    ModifierNGContainer GetModifiersNG(ModifierNG::RSModifierType type) const;
    const ModifiersNGMap& GetAllModifiers() const;
    bool HasDrawCmdModifiers() const;
    bool HasContentStyleModifierOnly() const;

    size_t GetAllModifierSize();

    bool IsShadowValidLastFrame() const
    {
        return isShadowValidLastFrame_;
    }
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

    bool IsPureContainer() const;
    bool IsContentNode() const;
    bool IsPureBackgroundColor() const;
    void SetDrawNodeType(DrawNodeType nodeType);
    DrawNodeType GetDrawNodeType() const;

    using ClearCacheSurfaceFunc =
        std::function<void(std::shared_ptr<Drawing::Surface>&&,
        std::shared_ptr<Drawing::Surface>&&, uint32_t, uint32_t)>;

    Vector2f GetOptionalBufferSize() const;

    void SetCacheType(CacheType cacheType);
    CacheType GetCacheType() const
    {
        return cacheType_;
    }

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
    void SetHDRUIBrightness(float brightness);

    bool GetCommandExecuted() const
    {
        return commandExecuted_;
    }

    void SetCommandExecuted(bool commandExecuted)
    {
        commandExecuted_ = commandExecuted;
    }

    bool IsScale() const
    {
        return isScale_;
    }
    void SetIsScale(bool isScale)
    {
        isScale_ = isScale;
    }

    void SetDrawRegion(const std::shared_ptr<RectF>& rect);
    const std::shared_ptr<RectF>& GetDrawRegion() const;
    void SetOutOfParent(OutOfParentType outOfParent);
    OutOfParentType GetOutOfParent() const;

    void UpdateEffectRegion(std::optional<Drawing::RectI>& region, bool isForced = false);
    virtual void MarkFilterHasEffectChildren() {}
    virtual void OnFilterCacheStateChanged() {}

    // for blur filter cache
    virtual void CheckBlurFilterCacheNeedForceClearOrSave(bool rotationChanged = false,
        bool rotationStatusChanged = false);
    void UpdateLastFilterCacheRegion();
    void UpdateFilterRegionInSkippedSubTree(RSDirtyRegionManager& dirtyManager,
        const RSRenderNode& subTreeRoot, RectI& filterRect, const RectI& clipRect,
        const std::optional<RectI>& surfaceClipRect);
    void FilterRectMergeDirtyRectInSkippedSubtree(RSDirtyRegionManager& dirtyManager,
        const RectI& filterRect);
    void MarkFilterStatusChanged(std::shared_ptr<DrawableV2::RSFilterDrawable>& filterDrawable,
        bool isForeground, bool isFilterRegionChanged);
    void UpdateFilterCacheWithBackgroundDirty();
    virtual bool UpdateFilterCacheWithBelowDirty(const Occlusion::Region& belowDirty,
        RSDrawableSlot slot = RSDrawableSlot::BACKGROUND_FILTER);
    virtual void UpdateFilterCacheWithSelfDirty();
    void UpdatePendingPurgeFilterDirtyRect(RSDirtyRegionManager& dirtyManager, RSDrawableSlot slot);
    bool IsBackgroundInAppOrNodeSelfDirty() const
    {
        return backgroundFilterInteractWithDirty_ || backgroundFilterRegionChanged_;
    }
    void PostPrepareForBlurFilterNode(RSDirtyRegionManager& dirtyManager, bool needRequestNextVsync);
#ifdef RS_ENABLE_GPU
    void CheckFilterCacheAndUpdateDirtySlots(
        std::shared_ptr<DrawableV2::RSFilterDrawable>& filterDrawable, RSDrawableSlot slot);
#endif
    bool IsFilterCacheValid() const;
    bool IsAIBarFilter() const;
    bool CheckAndUpdateAIBarCacheStatus(bool intersectHwcDamage) const;
    // Return true if the cache interval of aibar has been successfully reduced; otherwise, return false.
    bool ForceReduceAIBarCacheInterval(bool intersectHwcDamage);
    void MarkForceClearFilterCacheWithInvisible();
    void MarkFilterInForegroundFilterAndCheckNeedForceClearCache(NodeId offscreenCanvasNodeId);

    // used for renderGroup
    void SetDrawingCacheType(RSDrawingCacheType cacheType);
    RSDrawingCacheType GetDrawingCacheType() const
    {
        return drawingCacheType_;
    }
    void SetDrawingCacheChanged(bool cacheChanged);
    bool GetDrawingCacheChanged() const;
    void SetForceDisableNodeGroup(bool forceDisable);
    bool IsForceDisableNodeGroup() const;
    bool IsForcedDrawInGroup() const;
    bool IsSuggestedDrawInGroup() const;
    void CheckDrawingCacheType();
    enum NodeGroupType : uint8_t {
        NONE = 0,
        GROUPED_BY_ANIM = 1,
        GROUPED_BY_UI = GROUPED_BY_ANIM << 1,
        GROUPED_BY_USER = GROUPED_BY_UI << 1,
        GROUPED_BY_FOREGROUND_FILTER = GROUPED_BY_USER << 1,
        GROUP_TYPE_BUTT = GROUPED_BY_FOREGROUND_FILTER,
    };
    void MarkNodeGroup(NodeGroupType type, bool isNodeGroup, bool includeProperty);
    void ExcludedFromNodeGroup(bool isExcluded);
    bool IsExcludedFromNodeGroup() const;
    void SetHasChildExcludedFromNodeGroup(bool isExcluded);
    bool HasChildExcludedFromNodeGroup() const;
    void SetRenderGroupExcludedStateChanged(bool isChanged);
    bool IsRenderGroupExcludedStateChanged() const;
    void SetRenderGroupSubTreeDirty(bool isDirty);
    bool IsRenderGroupSubTreeDirty() const;
    void MarkForegroundFilterCache();
    NodeGroupType GetNodeGroupType() const;
    void SetChildHasTranslateOnSqueeze(bool val);
    bool IsNodeGroupIncludeProperty() const;
    void UpdateDrawingCacheInfoBeforeChildren(bool isScreenRotation, bool isOnExcludedSubTree);
    void UpdateDrawingCacheInfoAfterChildren(
        bool isInBlackList = false, const std::unordered_set<NodeId>& childHasProtectedNodeSet = {});
    // !used for renderGroup

    void MarkNodeSingleFrameComposer(bool isNodeSingleFrameComposer, pid_t pid = 0);
    virtual bool GetNodeIsSingleFrameComposer() const;

    // mark cross node in physical extended screen model
    bool IsCrossNode() const;

    // arkui mark
    void MarkSuggestOpincNode(bool isOpincNode, bool isNeedCalculate);

    /////////////////////////////////////////////

    void SetSharedTransitionParam(const std::shared_ptr<SharedTransitionParam>& sharedTransitionParam);
    const std::shared_ptr<SharedTransitionParam>& GetSharedTransitionParam() const
    {
        return sharedTransitionParam_;
    }

    void SetGlobalAlpha(float alpha);
    float GetGlobalAlpha() const
    {
        return globalAlpha_;
    }
    virtual void OnAlphaChanged() {}

    inline const Vector4f& GetGlobalCornerRadius() const noexcept
    {
        return globalCornerRadius_;
    }

    inline const RectI& GetGlobalCornerRect() const noexcept
    {
        return globalCornerRect_;
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

    virtual void ApplyModifiers();
    void ApplyPositionZModifier();
    virtual void UpdateRenderParams();
    void SetCrossNodeOffScreenStatus(CrossNodeOffScreenRenderDebugType isCrossNodeOffscreenOn);

    virtual RectI GetFilterRect() const;
    RectI GetAbsRect() const;
    void CalVisibleFilterRect(const std::optional<RectI>& clipRect, const std::optional<RectI>& surfaceClipRect);
    void CalVisibleFilterRect(const RectI& absRect, const Drawing::Matrix& matrix,
        const std::optional<RectI>& clipRect, const std::optional<RectI>& surfaceClipRect);
    void UpdateFilterRectInfo();
    std::shared_ptr<RSFilter> GetRSFilterWithSlot(RSDrawableSlot slot) const;

    void SetIsTextureExportNode(bool isTextureExportNode)
    {
        isTextureExportNode_ = isTextureExportNode;
    }

    bool HasHpaeBackgroundFilter() const;

    bool GetLastFrameSync() const { return lastFrameSynced_; }

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
    std::unique_ptr<RSRenderParams>& GetStagingRenderParams()
    {
        return stagingRenderParams_;
    }

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
    RSUIFirstSwitch GetUIFirstSwitch() const
    {
        return uiFirstSwitch_;
    }

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

    void UpdateHDRStatus(HdrStatus hdrStatus, bool isAdd);

    void ClearHDRVideoStatus();

    HdrStatus GetHDRStatus() const;

    void SetChildHasVisibleHDRContent(bool val);

    bool ChildHasVisibleHDRContent() const;

    void SetHdrNum(bool flag, NodeId instanceRootNodeId, NodeId screenNodeId, HDRComponentType hdrType);

    virtual void UpdateNodeColorSpace() {}
    void ResetNodeColorSpace();
    void SetNodeColorSpace(GraphicColorGamut colorSpace);
    GraphicColorGamut GetNodeColorSpace() const;
    virtual void MarkNodeColorSpace(bool isP3) {}

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
    void UpdateDrawableAfterPostPrepare(ModifierNG::RSModifierType type);
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
    void SetOldDirtyInSurface(RectI oldDirtyInSurface)
    {
        oldDirtyInSurface_ = oldDirtyInSurface;
    }

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
    bool IsPixelStretchValid() const;

    void ResetRepaintBoundaryInfo();
    void UpdateRepaintBoundaryInfo(RSRenderNode& node);
    uint32_t GetRepaintBoundaryWeight();

    void ClearSubtreeParallelNodes();
    void UpdateSubTreeParallelNodes();
    void MergeSubtreeParallelNodes(RSRenderNode& childNode);
    const std::unordered_set<NodeId>& GetSubtreeParallelNodes();

    void SetNeedUseCmdlistDrawRegion(bool needUseCmdlistDrawRegion);
    bool GetNeedUseCmdlistDrawRegion();
    void ReleaseNodeMem();
    bool IsNodeMemClearEnable();
    virtual void AfterTreeStatueChanged() {}

    RectI GetFilterDrawableSnapshotRegion() const;

    std::shared_ptr<DrawableV2::RSColorPickerDrawable> GetColorPickerDrawable() const;
    // returns true if color picker will execute this frame
    bool PrepareColorPickerForExecution(uint64_t vsyncTime, bool darkMode);

protected:
    void ResetDirtyStatus();

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
    void HandleNodeRemovedFromTree();
    void AddSubSurfaceUpdateInfo(SharedPtr curParent, SharedPtr preParent);

    static void SendCommandFromRT(std::unique_ptr<RSCommand>& command, NodeId nodeId);

    virtual void InitRenderParams();
    virtual void OnSync();
    virtual void OnSkipSync();
    virtual void AccumulateLastDirtyTypes() {}
    virtual void ClearNeverOnTree() {}

    void AddUIExtensionChild(SharedPtr child);
    void MoveUIExtensionChild(SharedPtr child);
    void RemoveUIExtensionChild(SharedPtr child);
    bool NeedRoutedBasedOnUIExtension(SharedPtr child);

    void UpdateDrawableVecV2();
    void ClearDrawableVec2();
    void UpdateDrawableEnableEDR();

    void SetHdrPhotoHeadroom(uint32_t headroom);
    void SetHdrEffectHeadroom(uint32_t headroom);
    void SetHdrUIComponentHeadroom(uint32_t headroom);
    uint32_t GetHdrPhotoHeadroom() const;
    uint32_t GetHdrEffectHeadroom() const;
    uint32_t GetHdrUIComponentHeadroom() const;

    void DrawPropertyDrawable(RSDrawableSlot slot, RSPaintFilterCanvas& canvas);
    void DrawPropertyDrawableRange(RSDrawableSlot begin, RSDrawableSlot end, RSPaintFilterCanvas& canvas);

#ifdef RS_ENABLE_GPU
    std::shared_ptr<DrawableV2::RSFilterDrawable> GetFilterDrawable(bool isForeground) const;
    std::shared_ptr<DrawableV2::RSFilterDrawable> GetFilterDrawable(RSDrawableSlot slot) const;

    bool InvokeFilterDrawable(RSDrawableSlot slot,
        std::function<void(std::shared_ptr<DrawableV2::RSFilterDrawable>)> checkMethodInvokeFunc);

    virtual void MarkFilterCacheFlags(std::shared_ptr<DrawableV2::RSFilterDrawable>& filterDrawable,
        RSDirtyRegionManager& dirtyManager, bool needRequestNextVsync);
    bool IsForceClearOrUseFilterCache(std::shared_ptr<DrawableV2::RSFilterDrawable>& filterDrawable);
#endif
    void UpdateDirtySlotsAndPendingNodes(RSDrawableSlot slot);
    void ExpandDirtyRegionWithFilterRegion(RSDirtyRegionManager& dirtyManager)
    {
        dirtyManager.MergeDirtyRect(GetFilterRegion());
        isDirtyRegionUpdated_ = true;
    }
    // if true, it means currently it's in partial render mode and this node is intersect with dirtyRegion
    bool isRenderUpdateIgnored_ = false;
    bool isShadowValidLastFrame_ = false;
    bool IsSelfDrawingNode() const;
    bool needClearSurface_ = false;
    bool isBootAnimation_ = false;
    bool lastFrameHasVisibleEffectWithoutEmptyRect_ = false;
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

    ModifierNG::ModifierDirtyTypes dirtyTypesNG_;
    ModifierNG::ModifierDirtyTypes curDirtyTypesNG_;

    CurFrameInfoDetail curFrameInfoDetail_;

    // Enable HWCompose
    RSHwcRecorder hwcRecorder_;

private:
    // mark cross node in physical extended screen model
    bool isRepaintBoundary_ = false;
    bool hasForceSubmit_ = false;
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
    bool isTreeStateChangeDirty_ = false;
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
    bool isScale_ = false;
    bool uifirstNeedSync_ = false; // both cmdlist&param
    bool uifirstSkipPartialSync_ = false;
    bool forceUpdateByUifirst_ = false;
    bool backgroundFilterRegionChanged_ = false;
    bool backgroundFilterInteractWithDirty_ = false;
    // for UIExtension info collection
    bool childrenHasUIExtension_ = false;
    bool isAccessibilityConfigChanged_ = false;
    const bool isPurgeable_;
    bool isForcePrepare_ = false;
    bool isParentTreeDirty_ = false;
    DrawNodeType drawNodeType_ = DrawNodeType::PureContainerType;
    std::atomic<bool> isTunnelHandleChange_ = false;
    std::atomic<bool> commandExecuted_ = false;
    int32_t preparedDisplayOffsetX_ = 0;
    int32_t preparedDisplayOffsetY_ = 0;
    uint32_t disappearingTransitionCount_ = 0;
    float globalAlpha_ = 1.0f;
    // collect subtree's surfaceNode including itself
    OutOfParentType outOfParent_ = OutOfParentType::UNKNOWN;
    pid_t appPid_ = 0;
    uint64_t uiContextToken_ = 0;
    std::vector<uint64_t> uiContextTokenList_;
    NodeId id_;
    NodeId instanceRootNodeId_ = INVALID_NODEID;
    NodeId firstLevelNodeId_ = INVALID_NODEID;
    NodeId uifirstRootNodeId_ = INVALID_NODEID;
    NodeId screenNodeId_ = INVALID_NODEID;
    NodeId logicalDisplayNodeId_ = INVALID_NODEID;
    std::shared_ptr<SharedTransitionParam> sharedTransitionParam_;

    bool isBoundsModifierAdded_ = false;
    bool isFrameModifierAdded_= false;

    // Note: Make sure that fullChildrenList_ is never nullptr. Otherwise, the caller using
    // `for (auto child : *GetSortedChildren()) { ... }` will crash.
    // When an empty list is needed, use EmptyChildrenList instead.
    static const inline auto EmptyChildrenList = std::make_shared<const std::vector<std::shared_ptr<RSRenderNode>>>();
    ChildrenListSharedPtr fullChildrenList_ = EmptyChildrenList ;
    std::unique_ptr<RSRenderDisplaySync> displaySync_ = nullptr;
    std::shared_ptr<RectF> drawRegion_ = nullptr;
    std::shared_ptr<std::unordered_set<std::shared_ptr<RSRenderNode>>> stagingUECChildren_ =
        std::make_shared<std::unordered_set<std::shared_ptr<RSRenderNode>>>();
    WeakPtr sourceCrossNode_;
    WeakPtr curCloneNodeParent_;
    std::weak_ptr<RSContext> context_ = {};
    WeakPtr parent_;
    // including enlarged draw region
    RectF selfDrawRect_;
    // map parentMatrix
    RectI absDrawRect_;
    RectF absDrawRectF_;
    RectI oldAbsDrawRect_;
    // round in by absDrawRectF_, only used for opaque region calculations
    RectI innerAbsDrawRect_;
    // map parentMatrix by cmdlist draw region
    RectI absCmdlistDrawRect_;
    RectI oldDirty_;
    RectI oldDirtyInSurface_;
    RectI childrenRect_;
    RectI oldChildrenRect_;
    RectI removedChildrenRect_;
    RectI oldClipRect_;
    // aim to record children rect in abs coords, without considering clip
    // aim to record current frame clipped children dirty region, in abs coords
    RectI subTreeDirtyRegion_;
    Vector4f globalCornerRadius_{ 0.f, 0.f, 0.f, 0.f };
    RectI globalCornerRect_;
    RectF selfDrawingNodeDirtyRect_;
    RectI selfDrawingNodeAbsDirtyRect_;
    // used in old pipline
    RectI oldRectFromRenderProperties_;
    // for blur cache
    struct FilterRegionInfo {
        RectI filterRegion_;
        RectI defaultFilterRegion_;
    };
    std::unique_ptr<FilterRegionInfo> filterRegionInfo_;
    RectI lastFilterRegion_;
    std::vector<SharedPtr> cloneCrossNodeVec_;
    bool hasVisitedCrossNode_ = false;

    ModifiersNGMap modifiersNG_;
    std::map<PropertyId, std::shared_ptr<RSRenderPropertyBase>> properties_;

    std::unordered_set<RSDrawableSlot> dirtySlots_;
    DrawCmdIndex stagingDrawCmdIndex_;
    RSDrawable::DrawList stagingDrawCmdList_;
    std::vector<NodeId> visibleFilterChild_;
    std::unordered_set<NodeId> visibleEffectChild_;
    Drawing::Matrix oldMatrix_;
    Drawing::Matrix oldAbsMatrix_;
#ifdef RS_ENABLE_MEMORY_DOWNTREE
    mutable std::unique_ptr<RSDrawable::Vec> drawableVec_;
    bool released_ = false;
#else
    mutable RSDrawable::Vec drawableVec_;
#endif
    RSAnimationManager animationManager_;
    RSOpincCache opincCache_;
    std::unordered_set<NodeId> subtreeParallelNodes_;
    bool isAllChildRepaintBoundary_ = false;
    uint32_t repaintBoundaryWeight_ = 0;

    std::list<WeakPtr> children_;
    std::set<NodeId> preFirstLevelNodeIdSet_ = {};
    std::list<std::pair<SharedPtr, uint32_t>> disappearingChildren_;
    friend class RSRenderPropertyBase;
    friend class RSRenderTransition;
    std::string nodeName_ = "";
    std::unordered_set<NodeId> curCacheFilterRects_ = {};
    std::unordered_set<NodeId> visitedCacheRoots_ = {};

    std::unordered_map<ScreenId, bool> hasVirtualScreenWhiteList_;

    RSProperties renderProperties_;

    // The angle at which the node rotates about the Z-axis
    float absRotation_ = 0.f;
    void ShowSetIsOnetheTreeCntIfNeed(const std::string& funcName, NodeId nodeId, const std::string& nodeName);

    bool enableHdrEffect_ = false;
    static constexpr uint32_t DEFAULT_HEADROOM_VALUE = 0U;
    struct HeadroomInfo {
        uint32_t hdrPhotoHeadroom = DEFAULT_HEADROOM_VALUE;
        uint32_t hdrEffectHeadroom = DEFAULT_HEADROOM_VALUE;
        uint32_t hdrUIComponentHeadroom = DEFAULT_HEADROOM_VALUE;
    };
    std::unique_ptr<HeadroomInfo> headroomInfo_ = nullptr;
    void CheckHdrHeadroomInfoPointer();

    bool needUseCmdlistDrawRegion_ = false;
    RectF cmdlistDrawRegion_;

    FilterRegionInfo& GetFilterRegionInfo()
    {
        if (filterRegionInfo_ == nullptr) {
            filterRegionInfo_ = std::make_unique<FilterRegionInfo>();
        }

        return *filterRegionInfo_;
    }

    void SetParent(WeakPtr parent);
    void ResetParent();
    void UpdateSrcOrClipedAbsDrawRectChangeState(const RectI& clipRect);
    virtual void OnResetParent() {}

    void GenerateFullChildrenList();
    void ResortChildren();
    bool ShouldClearSurface();

    void InternalRemoveSelfFromDisappearingChildren();
    void FallbackAnimationsToRoot();
    void FilterModifiersByPid(pid_t pid);

    bool UpdateBufferDirtyRegion(RectI& dirtyRect, const RectI& drawRegion);
    RectI GetDrawCmdListRect() const;
    void CollectAndUpdateRenderFitRect();
    void CollectAndUpdateLocalShadowRect();
    void CollectAndUpdateLocalOutlineRect();
    void CollectAndUpdateLocalPixelStretchRect();
    void CollectAndUpdateLocalForegroundEffectRect();
    void CollectAndUpdateLocalDistortionEffectRect();
    void CollectAndUpdateLocalMagnifierEffectRect();
    void CollectAndUpdateLocalEffectRect();
    // update drawrect based on self's info
    void UpdateBufferDirtyRegion();
    bool UpdateSelfDrawRect();
    void UpdateAbsDirtyRegion(RSDirtyRegionManager& dirtyManager, const RectI& clipRect);
    void UpdateDirtyRegion(RSDirtyRegionManager& dirtyManager, bool geoDirty, const std::optional<RectI>& clipRect);
    void UpdateDrawRect(bool& accumGeoDirty, const RectI& clipRect, const Drawing::Matrix& parentSurfaceMatrix);
    void UpdateFullScreenFilterCacheRect(RSDirtyRegionManager& dirtyManager, bool isForeground) const;
    void ValidateLightResources();
    void UpdateShouldPaint(); // update node should paint state in apply modifier stage

    void UpdateDisplayList();
    void UpdateShadowRect();

    void RecordCloneCrossNode(SharedPtr node);

    void OnRegister(const std::weak_ptr<RSContext>& context);

    void ChildrenListDump(std::string& out) const;

    void ResetAndApplyModifiers();

    void InitRenderDrawableAndDrawableVec();
    RSDrawable::Vec& GetDrawableVec(const char*) const;
    void ResetFilterInfo();
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

struct RSB_EXPORT SharedTransitionParam {
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
