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

#include "pipeline/rs_uni_render_visitor.h"
#include <memory>
#include "rs_trace.h"
#include "screen_manager/rs_screen_manager.h"

#ifdef RS_ENABLE_OLD_VK
#include <vulkan_window.h>
#endif

#include "draw/color.h"
#include "recording/recording_canvas.h"
#include "skia_adapter/skia_canvas.h"

#include "src/core/SkCanvasPriv.h"

#include "common/rs_background_thread.h"
#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "common/rs_singleton.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_display_render_params.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_draw_cmd.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_realtime_refresh_rate_manager.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_listener.h"
#include "pipeline/rs_uni_render_virtual_processor.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_uifirst_manager.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "platform/ohos/rs_jank_stats.h"
#include "property/rs_properties_painter.h"
#include "property/rs_point_light_manager.h"
#include "render/rs_skia_filter.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "system/rs_system_parameters.h"
#include "scene_board_judgement.h"
#include "hgm_core.h"
#include "benchmarks/rs_recording_thread.h"
#include "scene_board_judgement.h"
#include "metadata_helper.h"
#include <v1_0/buffer_handle_meta_key_type.h>
#include <v1_0/cm_color_space.h>
#ifdef DDGR_ENABLE_FEATURE_OPINC
#include "rs_auto_cache.h"
#include "text/font_mgr.h"
#include "text/font_style_set.h"
#endif

#include "pipeline/round_corner_display/rs_round_corner_display.h"
#include "pipeline/round_corner_display/rs_message_bus.h"
namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t PHONE_MAX_APP_WINDOW_NUM = 1;
constexpr uint32_t CACHE_MAX_UPDATE_TIME = 2;
constexpr int32_t VISIBLEAREARATIO_FORQOS = 3;
constexpr int ROTATION_90 = 90;
constexpr int ROTATION_270 = 270;
constexpr float EPSILON_SCALE = 0.00001f;
static const std::string CAPTURE_WINDOW_NAME = "CapsuleWindow";
constexpr const char* CLEAR_GPU_CACHE = "ClearGpuCache";
static std::map<NodeId, uint32_t> cacheRenderNodeMap = {};
static uint32_t cacheReuseTimes = 0;
static std::mutex cacheRenderNodeMapMutex;
// vector of Appwindow nodes ids not contain subAppWindow nodes ids in last frame
static std::queue<NodeId> preMainAndLeashWindowNodesIds_;
static VisibleData lastVisVec_;
static const std::map<DirtyRegionType, std::string> DIRTY_REGION_TYPE_MAP {
    { DirtyRegionType::UPDATE_DIRTY_REGION, "UPDATE_DIRTY_REGION" },
    { DirtyRegionType::OVERLAY_RECT, "OVERLAY_RECT" },
    { DirtyRegionType::FILTER_RECT, "FILTER_RECT" },
    { DirtyRegionType::SHADOW_RECT, "SHADOW_RECT" },
    { DirtyRegionType::PREPARE_CLIP_RECT, "PREPARE_CLIP_RECT" },
    { DirtyRegionType::REMOVE_CHILD_RECT, "REMOVE_CHILD_RECT" },
    { DirtyRegionType::RENDER_PROPERTIES_RECT, "RENDER_PROPERTIES_RECT" },
    { DirtyRegionType::CANVAS_NODE_SKIP_RECT, "CANVAS_NODE_SKIP_RECT" },
    { DirtyRegionType::OUTLINE_RECT, "OUTLINE_RECT" },
};

bool CheckRootNodeReadyToDraw(const std::shared_ptr<RSBaseRenderNode>& child)
{
    if (child != nullptr && child->IsInstanceOf<RSRootRenderNode>()) {
        auto rootNode = child->ReinterpretCastTo<RSRootRenderNode>();
        const auto& property = rootNode->GetRenderProperties();
        if (property.GetFrameWidth() > 0 && property.GetFrameHeight() > 0 && rootNode->GetEnableRender()) {
            return true;
        }
    }
    return false;
}

bool CheckScbReadyToDraw(const std::shared_ptr<RSBaseRenderNode>& child)
{
    if (child != nullptr && child->IsInstanceOf<RSCanvasRenderNode>()) {
        auto canvasRenderNode = child->ReinterpretCastTo<RSCanvasRenderNode>();
        const auto& property = canvasRenderNode->GetRenderProperties();
        if (property.GetFrameWidth() > 0 && property.GetFrameHeight() > 0) {
            return true;
        }
    }
    return false;
}

bool IsFirstFrameReadyToDraw(RSSurfaceRenderNode& node)
{
    bool result = false;
    auto sortedChildren = node.GetSortedChildren();
    if (node.IsScbScreen()) {
        for (const auto& child : *sortedChildren) {
            result = CheckScbReadyToDraw(child);
        }
        return result;
    }
    for (auto& child : *sortedChildren) {
        result = CheckRootNodeReadyToDraw(child);
        // when appWindow has abilityComponent node
        if (child != nullptr && child->IsInstanceOf<RSSurfaceRenderNode>()) {
            for (const auto& surfaceNodeChild : *child->GetSortedChildren()) {
                result = CheckRootNodeReadyToDraw(surfaceNodeChild);
            }
        }
    }
    return result;
}
}

void DoScreenRcdTask(std::shared_ptr<RSProcessor>& processor, std::unique_ptr<RcdInfo>& rcdInfo,
    ScreenInfo& screenInfo_)
{
    if (screenInfo_.state != ScreenState::HDI_OUTPUT_ENABLE) {
        RS_LOGD("DoScreenRcdTask is not at HDI_OUPUT mode");
        return;
    }
    if (RSSingleton<RoundCornerDisplay>::GetInstance().GetRcdEnable()) {
        RSSingleton<RoundCornerDisplay>::GetInstance().RunHardwareTask(
            [&processor, &rcdInfo]() {
                auto hardInfo = RSSingleton<RoundCornerDisplay>::GetInstance().GetHardwareInfo();
                rcdInfo->processInfo = {processor, hardInfo.topLayer, hardInfo.bottomLayer,
                    hardInfo.resourceChanged};
                RSRcdRenderManager::GetInstance().DoProcessRenderTask(rcdInfo->processInfo);
            }
        );
    }
}

RSUniRenderVisitor::RSUniRenderVisitor()
    : curSurfaceDirtyManager_(std::make_shared<RSDirtyRegionManager>())
{
    PartialRenderOptionInit();
    auto mainThread = RSMainThread::Instance();
    renderEngine_ = mainThread->GetRenderEngine();
    quickSkipPrepareType_ = RSSystemParameters::GetQuickSkipPrepareType();
    isOcclusionEnabled_ = RSSystemProperties::GetOcclusionEnabled();
    isQuickSkipPreparationEnabled_ = (quickSkipPrepareType_ != QuickSkipPrepareType::DISABLED);
    isDrawingCacheEnabled_ = RSSystemParameters::GetDrawingCacheEnabled();
#ifdef DDGR_ENABLE_FEATURE_OPINC
    autoCacheEnable_ = RSSystemProperties::IsDdgrOpincEnable();
    autoCacheDrawingEnable_ = RSSystemProperties::GetAutoCacheDebugEnabled() && autoCacheEnable_;
#endif
    RSTagTracker::UpdateReleaseResourceEnabled(RSSystemProperties::GetReleaseResourceEnabled());
    isScreenRotationAnimating_ = RSSystemProperties::GetCacheEnabledForRotation();
    isSubSurfaceEnabled_ = RSSystemProperties::GetSubSurfaceEnabled();
    isSkipCanvasNodeOutOfScreen_ = RSSystemParameters::GetSkipCanvasNodeOutofScreenEnabled();
    if (RSRcdRenderManager::GetInstance().GetRcdRenderEnabled()) {
        rcdInfo_ = std::make_unique<RcdInfo>();
    }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (renderEngine_ && renderEngine_->GetRenderContext()) {
        auto subThreadManager = RSSubThreadManager::Instance();
        subThreadManager->Start(renderEngine_->GetRenderContext().get());
    }
#endif
    isUIFirst_ = RSMainThread::Instance()->IsUIFirstOn();
    isPhone_ = RSMainThread::Instance()->GetDeviceType() == DeviceType::PHONE;
    isPc_ = RSMainThread::Instance()->GetDeviceType() == DeviceType::PC;
}

void RSUniRenderVisitor::PartialRenderOptionInit()
{
    partialRenderType_ = RSSystemProperties::GetUniPartialRenderEnabled();
    isPartialRenderEnabled_ = (partialRenderType_ != PartialRenderType::DISABLED) &&
        RSMainThread::Instance()->IsSingleDisplay();
    dirtyRegionDebugType_ = RSSystemProperties::GetDirtyRegionDebugType();
    surfaceRegionDebugType_ = RSSystemProperties::GetSurfaceRegionDfxType();
    isRegionDebugEnabled_ = (dirtyRegionDebugType_ != DirtyRegionDebugType::DISABLED) ||
        (surfaceRegionDebugType_ != SurfaceRegionDebugType::DISABLED);
    isVisibleRegionDfxEnabled_ = (surfaceRegionDebugType_ == SurfaceRegionDebugType::VISIBLE_REGION);
    isOpaqueRegionDfxEnabled_ = (surfaceRegionDebugType_ == SurfaceRegionDebugType::OPAQUE_REGION);
    isTargetDirtyRegionDfxEnabled_ = RSSystemProperties::GetTargetDirtyRegionDfxEnabled(dfxTargetSurfaceNames_) &&
        (surfaceRegionDebugType_ == SurfaceRegionDebugType::DISABLED);
    isDirtyRegionDfxEnabled_ = !isTargetDirtyRegionDfxEnabled_ &&
        (dirtyRegionDebugType_ == DirtyRegionDebugType::EGL_DAMAGE);
    isDisplayDirtyDfxEnabled_ = !isTargetDirtyRegionDfxEnabled_ &&
        (dirtyRegionDebugType_ == DirtyRegionDebugType::DISPLAY_DIRTY);
    isCanvasNodeSkipDfxEnabled_ = (dirtyRegionDebugType_ == DirtyRegionDebugType::CANVAS_NODE_SKIP_RECT);
    isOpDropped_ = isPartialRenderEnabled_ &&
        (partialRenderType_ != PartialRenderType::SET_DAMAGE) && !isRegionDebugEnabled_;
    isCacheBlurPartialRenderEnabled_ = RSSystemProperties::GetCachedBlurPartialRenderEnabled();
}

RSUniRenderVisitor::RSUniRenderVisitor(const RSUniRenderVisitor& visitor) : RSUniRenderVisitor()
{
    currentVisitDisplay_ = visitor.currentVisitDisplay_;
    screenInfo_ = visitor.screenInfo_;
    displayHasSecSurface_ = visitor.displayHasSecSurface_;
    displayHasSkipSurface_ = visitor.displayHasSkipSurface_;
    hasCaptureWindow_ = visitor.hasCaptureWindow_;
    parentSurfaceNodeMatrix_ = visitor.parentSurfaceNodeMatrix_;
    curAlpha_ = visitor.curAlpha_;
    dirtyFlag_ = visitor.dirtyFlag_;
    curDisplayNode_ = visitor.curDisplayNode_;
    currentFocusedNodeId_ = visitor.currentFocusedNodeId_;
    prepareClipRect_ = visitor.prepareClipRect_;
    isOpDropped_ = visitor.isOpDropped_;
    isPartialRenderEnabled_ = visitor.isPartialRenderEnabled_;
    isHardwareForcedDisabled_ = visitor.isHardwareForcedDisabled_;
    doAnimate_ = visitor.doAnimate_;
    isDirty_ = visitor.isDirty_;
}

RSUniRenderVisitor::~RSUniRenderVisitor() {}

void RSUniRenderVisitor::CopyVisitorInfos(std::shared_ptr<RSUniRenderVisitor> visitor)
{
    std::unique_lock<std::mutex> lock(copyVisitorInfosMutex_);
    currentVisitDisplay_ = visitor->currentVisitDisplay_;
    screenInfo_ = visitor->screenInfo_;
    displayHasSecSurface_ = visitor->displayHasSecSurface_;
    displayHasSkipSurface_ = visitor->displayHasSkipSurface_;
    hasCaptureWindow_ = visitor->hasCaptureWindow_;
    parentSurfaceNodeMatrix_ = visitor->parentSurfaceNodeMatrix_;
    curAlpha_ = visitor->curAlpha_;
    dirtyFlag_ = visitor->dirtyFlag_;
    curDisplayNode_ = visitor->curDisplayNode_;
    currentFocusedNodeId_ = visitor->currentFocusedNodeId_;
    prepareClipRect_ = visitor->prepareClipRect_;
    isOpDropped_ = visitor->isOpDropped_;
    isPartialRenderEnabled_ = visitor->isPartialRenderEnabled_;
    isHardwareForcedDisabled_ = visitor->isHardwareForcedDisabled_;
    doAnimate_ = visitor->doAnimate_;
    isDirty_ = visitor->isDirty_;
}

void RSUniRenderVisitor::UpdateSubTreeInCache(const std::shared_ptr<RSRenderNode>& cacheRootNode,
    const std::vector<RSRenderNode::SharedPtr>& children)
{
    for (auto& child : children) {
        if (child == nullptr) {
            continue;
        }
#ifdef DDGR_ENABLE_FEATURE_OPINC
        bool isPrepareAutoCache = autoCacheEnable_ && !isDiscardSurface_ && unchangeMarkEnable_;
        if (isPrepareAutoCache) {
            child->GetAutoCache()->OpincStartMarkStableNode(unchangeMark_, true);
        }
#endif
        if (child->IsDirty()) {
            curDirty_ = child->IsDirty();
            child->Prepare(shared_from_this());
#ifdef DDGR_ENABLE_FEATURE_OPINC
            if (isPrepareAutoCache) {
                child->GetAutoCache()->OpincEndMarkStableNode(unchangeMark_);
            }
#endif
            continue;
        }
        // set flag for surface node whose children contain shared transition node
        if (child->GetSharedTransitionParam() && curSurfaceNode_) {
            SetHasSharedTransitionNode(*curSurfaceNode_, true);
        }
        // [planning] pay attention to outofparent case
        if (auto surfaceNode = child->ReinterpretCastTo<RSSurfaceRenderNode>()) {
            // fully prepare hwcLayer Layer and its subnodes
            if (surfaceNode->IsHardwareEnabledType()) {
                PrepareSurfaceRenderNode(*surfaceNode);
#ifdef DDGR_ENABLE_FEATURE_OPINC
                if (isPrepareAutoCache) {
                    child->GetAutoCache()->OpincEndMarkStableNode(unchangeMark_);
                }
#endif
                return;
            }
        } else if (auto effectNode = child->ReinterpretCastTo<RSEffectRenderNode>()) {
            // effectNode need to update effectRegion so effectNode and use-effect child should be updated
            PrepareEffectNodeIfCacheReuse(cacheRootNode, effectNode);
#ifdef DDGR_ENABLE_FEATURE_OPINC
            if (isPrepareAutoCache) {
                child->GetAutoCache()->OpincEndMarkStableNode(unchangeMark_);
            }
#endif
            return;
        }
        if (child->GetRenderProperties().NeedFilter() || child->GetRenderProperties().GetUseEffect()) {
            child->Update(*curSurfaceDirtyManager_, cacheRootNode, dirtyFlag_, prepareClipRect_);
        }
        if (child->GetRenderProperties().NeedFilter()) {
            UpdateForegroundFilterCacheWithDirty(*child, *curSurfaceDirtyManager_);
            if (curSurfaceNode_ && curSurfaceNode_->GetId() == child->GetInstanceRootNodeId()) {
                curSurfaceNode_->UpdateChildrenFilterRects(child, child->GetOldDirtyInSurface(),
                    child->IsBackgroundFilterCacheValid());
            }
        }
        UpdateSubTreeInCache(child, *child->GetSortedChildren());
#ifdef DDGR_ENABLE_FEATURE_OPINC
        if (isPrepareAutoCache) {
            child->GetAutoCache()->OpincEndMarkStableNode(unchangeMark_);
        }
#endif
    }
}

void RSUniRenderVisitor::PrepareEffectNodeIfCacheReuse(const std::shared_ptr<RSRenderNode>& cacheRootNode,
    std::shared_ptr<RSEffectRenderNode> effectNode)
{
    if (effectNode == nullptr || curSurfaceDirtyManager_ == nullptr) {
        return;
    }
    // set rotationChanged true when screen is rotating or folding/expanding screen.
    if (curDisplayNode_->IsRotationChanged() || (!curDisplayNode_->IsRotationChanged() && doAnimate_)) {
        effectNode->SetRotationChanged(true);
        int invalidateTimes = 2; // node call invalidate cache 3 times in one frame.
        effectNode->SetInvalidateTimesForRotation(invalidateTimes);
    } else {
        effectNode->SetRotationChanged(false);
    }
    effectNode->SetVisitedFilterCacheStatus(curSurfaceDirtyManager_->IsCacheableFilterRectEmpty());
    effectNode->Update(*curSurfaceDirtyManager_, cacheRootNode, dirtyFlag_, prepareClipRect_);
    UpdateSubTreeInCache(effectNode, *effectNode->GetSortedChildren());
    if (effectNode->GetRenderProperties().NeedFilter()) {
        UpdateForegroundFilterCacheWithDirty(*effectNode, *curSurfaceDirtyManager_);
        if (curSurfaceNode_ && curSurfaceNode_->GetId() == effectNode->GetInstanceRootNodeId()) {
            curSurfaceNode_->UpdateChildrenFilterRects(effectNode, effectNode->GetOldDirtyInSurface(),
                effectNode->IsBackgroundFilterCacheValid());
        }
    }
}

void RSUniRenderVisitor::PrepareChildren(RSRenderNode& node)
{
    // GetSortedChildren() may remove disappearingChildren_ when transition animation end.
    // So the judgement whether node has removed child should be executed after this.
    // NOTE: removal of transition node is moved to RSMainThread::Animate
    MergeRemovedChildDirtyRegion(node);
    // backup environment variables.
    node.ResetChildrenRect();

    auto tempCornerRadius = curCornerRadius_;
    if (!isSubNodeOfSurfaceInPrepare_) {
        Vector4f::Max(node.GetRenderProperties().GetCornerRadius(), curCornerRadius_, curCornerRadius_);
    }
    if (node.GetType() == RSRenderNodeType::SURFACE_NODE) {
        node.SetGlobalCornerRadius(curCornerRadius_);
    }

    float alpha = curAlpha_;
    curAlpha_ *= node.GetRenderProperties().GetAlpha();
    node.SetGlobalAlpha(curAlpha_);
    auto children = node.GetSortedChildren();
    // check curSurfaceDirtyManager_ for SubTree updates
    if (curSurfaceDirtyManager_ != nullptr && isCachedSurfaceReuse_ && !node.HasMustRenewedInfo()) {
        RS_OPTIONAL_TRACE_NAME_FMT("CachedSurfaceReuse node %llu quickSkip subtree", node.GetId());
    } else if (curSurfaceDirtyManager_ != nullptr && curDisplayNode_ != nullptr &&
        (isCachedSurfaceReuse_ || isSurfaceDirtyNodeLimited_ || !UpdateCacheChangeStatus(node))) {
        RS_OPTIONAL_TRACE_NAME_FMT("UpdateCacheChangeStatus node %llu simply update subtree, isCachedSurfaceReuse_ %d,"
            " isSurfaceDirtyNodeLimited_ %d, hasUseEffect %d", node.GetId(), isCachedSurfaceReuse_,
            isSurfaceDirtyNodeLimited_, node.ChildHasVisibleEffect());
        UpdateSubTreeInCache(node.ReinterpretCastTo<RSRenderNode>(), *children);
        node.UpdateEffectRegion(effectRegion_,
            (node.GetDrawingCacheType() != RSDrawingCacheType::DISABLED_CACHE && node.ChildHasVisibleEffect()));
    } else {
        // Get delay flag to restore geo changes
        if (node.GetGeoUpdateDelay()) {
            dirtyFlag_ = true;
        }
        node.SetChildHasVisibleEffect(false);
        for (auto& child : *children) {
            SaveCurSurface(curSurfaceDirtyManager_, curSurfaceNode_);
            curDirty_ = child->IsDirty();
            child->Prepare(shared_from_this());
            if (child->ChildHasVisibleEffect()) {
                node.SetChildHasVisibleEffect(true);
            }
            RestoreCurSurface(curSurfaceDirtyManager_, curSurfaceNode_);
        }
        // Reset delay flag
        node.ResetGeoUpdateDelay();
        SetNodeCacheChangeStatus(node);
    }

    node.SetSubTreeDirty(false);
    curCornerRadius_ = std::move(tempCornerRadius);
    curAlpha_ = alpha;
    // restore environment variables
}

void RSUniRenderVisitor::MergeRemovedChildDirtyRegion(RSRenderNode& node, bool needMap)
{
    if (!node.HasRemovedChild()) {
        return;
    }
    RectI dirtyRect = node.GetChildrenRect();
    auto dirtyManager = curSurfaceNode_ ? curSurfaceDirtyManager_ : curDisplayDirtyManager_;
    if (dirtyManager == nullptr || dirtyRect.IsEmpty()) {
        node.ResetHasRemovedChild();
        return;
    }

    // [planning] merge removed child's rect instead
    if (needMap && curSurfaceNode_) {
        if (auto geoPtr = curSurfaceNode_->GetRenderProperties().GetBoundsGeometry()) {
            dirtyRect = geoPtr->MapAbsRect(dirtyRect.ConvertTo<float>());
        }
    } else {
        dirtyRect = prepareClipRect_.IntersectRect(node.GetChildrenRect());
    }
    dirtyManager->MergeDirtyRect(dirtyRect);
    if (dirtyManager->IsTargetForDfx()) {
        // since childRect includes multiple rects, defaultly marked as canvas_node
        dirtyManager->UpdateDirtyRegionInfoForDfx(node.GetId(), RSRenderNodeType::CANVAS_NODE,
            DirtyRegionType::REMOVE_CHILD_RECT, dirtyRect);
    }
    node.ResetHasRemovedChild();
}

bool RSUniRenderVisitor::IsDrawingCacheStatic(RSRenderNode& node)
{
    // since this function only called by drawing group root
    // if cache valid, cacheRenderNodeMapCnt > 0
    // check all dirtynodes of app instance if there's any in cache subtree
    if (curContentDirty_ || node.GetDrawingCacheChanged() || !node.IsCacheSurfaceValid() ||
        curSurfaceNode_ == nullptr || curSurfaceNode_->GetId() != node.GetInstanceRootNodeId() ||
        RSMainThread::Instance()->IsDrawingGroupChanged(node)) {
        return false;
    }
    // skip targeted cache node when first visited node is forced cache
    if (node.GetDrawingCacheType() == RSDrawingCacheType::TARGETED_CACHE && firstVisitedCache_ != node.GetId() &&
        IsFirstVisitedCacheForced()) {
        return false;
    }
    // simplify Cache status reset
    node.GetFilterRectsInCache(allCacheFilterRects_);
    node.SetDrawingCacheChanged(false);
    node.SetGeoUpdateDelay(dirtyFlag_);
    if (allCacheFilterRects_.count(node.GetId())) {
        node.SetChildHasVisibleFilter(true);
        if (const auto directParent = node.GetParent().lock()) {
            directParent->SetChildHasVisibleFilter(true);
        }
    }
    return true;
}

bool RSUniRenderVisitor::UpdateCacheChangeStatus(RSRenderNode& node)
{
    node.SetChildHasVisibleFilter(false);
    if (!isDrawingCacheEnabled_) {
        return true;
    }
    node.CheckDrawingCacheType();
    if (node.GetDrawingCacheType() != RSDrawingCacheType::DISABLED_CACHE && !node.IsStaticCached() &&
        (!node.ShouldPaint() || isScreenRotationAnimating_)) {
        node.SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
    }
    // skip status check if there is no upper cache mark
    if (node.GetDrawingCacheType() == RSDrawingCacheType::DISABLED_CACHE && firstVisitedCache_ == INVALID_NODEID) {
        return true;
    }
    // subroot's dirty and cached filter should be count for parent root
    if (!isDrawingCacheChanged_.empty()) {
        // Any child node dirty causes cache change
        RS_OPTIONAL_TRACE_NAME_FMT("UpdateCacheChangeStatus child:%" PRIu64 "", node.GetId());
        isDrawingCacheChanged_.top() = isDrawingCacheChanged_.top() || curDirty_;
    }
    if (!curCacheFilterRects_.empty() && !node.IsInstanceOf<RSEffectRenderNode>() &&
        (node.GetRenderProperties().GetBackgroundFilter() || node.GetRenderProperties().GetUseEffect() ||
        node.GetRenderProperties().GetShadowColorStrategy() != SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE)) {
        curCacheFilterRects_.top().emplace(node.GetId());
    }
    // drawing group root node
    if (node.GetDrawingCacheType() != RSDrawingCacheType::DISABLED_CACHE) {
        // if firstVisitedCache_ valid, upper cache should be updated so sub cache shouldn't skip
        // [planning] static subcache could be skip and reuse
        if ((quickSkipPrepareType_ >= QuickSkipPrepareType::STATIC_CACHE) &&
            (firstVisitedCache_ == INVALID_NODEID) && IsDrawingCacheStatic(node)) {
            return false;
        }
        // For rootnode, init drawing changes only if there is any content dirty
        isDrawingCacheChanged_.push(curContentDirty_);
        RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderVisitor::UpdateCacheChangeStatus: cachable node %" PRIu64 ""
            "contentDirty(cacheChanged): %d", node.GetId(), static_cast<int>(isDrawingCacheChanged_.top()));
        curCacheFilterRects_.push({});
        if (firstVisitedCache_ == INVALID_NODEID) {
            firstVisitedCache_ = node.GetId();
        }
    }
    return true;
}

void RSUniRenderVisitor::DisableNodeCacheInSetting(RSRenderNode& node)
{
    if (node.IsStaticCached()) {
        return;
    }
    // Attention: filter node should be marked. Only enable lowest suggested cached node
    if (node.GetDrawingCacheType() == RSDrawingCacheType::TARGETED_CACHE) {
        // if target cached is reused, keep enable -- prepareskip
        // disable cache if it has outOfParent -- cannot cache right
        // [planning] if there is dirty subcache, disable upper targetcache
        // disable targeted cache node when first visited node is forced cache to avoid problem in case with blur
        if (node.HasChildrenOutOfRect() || (firstVisitedCache_ != node.GetId() && IsFirstVisitedCacheForced())) {
            node.SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
            allCacheFilterRects_[firstVisitedCache_].insert(allCacheFilterRects_[node.GetId()].begin(),
                allCacheFilterRects_[node.GetId()].end());
            allCacheFilterRects_.erase(node.GetId());
            RS_OPTIONAL_TRACE_NAME_FMT("Disable cache %llu: outofparent %d, firstVisitedCache_ %llu & cacheforce %d",
                node.GetId(), node.HasChildrenOutOfRect(), firstVisitedCache_, IsFirstVisitedCacheForced());
        }
    }
    if (firstVisitedCache_ == INVALID_NODEID) {
        node.SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
        std::stack<bool>().swap(isDrawingCacheChanged_);
        visitedCacheNodeIds_.clear();
    }
}

bool RSUniRenderVisitor::IsFirstVisitedCacheForced() const
{
    if (firstVisitedCache_ != INVALID_NODEID) {
        auto firstVisitedNode = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode<RSRenderNode>(
            firstVisitedCache_);
        if (firstVisitedNode && firstVisitedNode->GetDrawingCacheType() == RSDrawingCacheType::FORCED_CACHE) {
            return true;
        }
    }
    return false;
}

void RSUniRenderVisitor::SaveCurSurface(std::shared_ptr<RSDirtyRegionManager> dirtyManager,
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode)
{
    if (!isSubSurfaceEnabled_) {
        return;
    }
    surfaceDirtyManager_.push(dirtyManager);
    surfaceNode_.push(surfaceNode);
}

void RSUniRenderVisitor::RestoreCurSurface(std::shared_ptr<RSDirtyRegionManager> &dirtyManager,
    std::shared_ptr<RSSurfaceRenderNode> &surfaceNode)
{
    if (!isSubSurfaceEnabled_) {
        return;
    }
    dirtyManager = surfaceDirtyManager_.top();
    surfaceNode = surfaceNode_.top();
    surfaceDirtyManager_.pop();
    surfaceNode_.pop();
}

void RSUniRenderVisitor::PrepareSubSurfaceNodes(RSSurfaceRenderNode& node)
{
    if (!isSubSurfaceEnabled_) {
        return;
    }
    for (auto &nodes : node.GetSubSurfaceNodes()) {
        for (auto &node : nodes.second) {
            auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.lock());
            if (surfaceNode != nullptr) {
                SaveCurSurface(curSurfaceDirtyManager_, curSurfaceNode_);
                PrepareSurfaceRenderNode(*surfaceNode);
                RestoreCurSurface(curSurfaceDirtyManager_, curSurfaceNode_);
            }
        }
    }
}

void RSUniRenderVisitor::ProcessSubSurfaceNodes(RSSurfaceRenderNode& node)
{
    if (!isSubSurfaceEnabled_) {
        return;
    }
    for (auto &nodes : node.GetSubSurfaceNodes()) {
        for (auto &node : nodes.second) {
            auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.lock());
            if (surfaceNode != nullptr) {
                SaveCurSurface(curSurfaceDirtyManager_, curSurfaceNode_);
                ProcessSurfaceRenderNode(*surfaceNode);
                RestoreCurSurface(curSurfaceDirtyManager_, curSurfaceNode_);
            }
        }
    }
}

void RSUniRenderVisitor::SetNodeCacheChangeStatus(RSRenderNode& node)
{
    auto directParent = node.GetParent().lock();
    if (directParent != nullptr && node.ChildHasVisibleFilter()) {
        directParent->SetChildHasVisibleFilter(true);
    }
    if (!isDrawingCacheEnabled_ ||
        node.GetDrawingCacheType() == RSDrawingCacheType::DISABLED_CACHE) {
        return;
    }
    if (!curCacheFilterRects_.empty()) {
        allCacheFilterRects_[node.GetId()].insert(curCacheFilterRects_.top().begin(),
            curCacheFilterRects_.top().end());
        node.ResetFilterRectsInCache(allCacheFilterRects_[node.GetId()]);
        curCacheFilterRects_.pop();
    }
    DisableNodeCacheInSetting(node);
    if (node.GetDrawingCacheType() != RSDrawingCacheType::DISABLED_CACHE) {
        // update visited cache roots including itself
        visitedCacheNodeIds_.emplace(node.GetId());
        node.SetVisitedCacheRootIds(visitedCacheNodeIds_);
        if (curSurfaceNode_) {
            curSurfaceNode_->UpdateDrawingCacheNodes(node.ReinterpretCastTo<RSRenderNode>());
        }
    }
    bool isDrawingCacheChanged = isDrawingCacheChanged_.empty() ? true : isDrawingCacheChanged_.top();
    RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderVisitor::SetNodeCacheChangeStatus: node %" PRIu64 " drawingtype %d, "
        "staticCache %d, cacheChange %d, childHasVisibleFilter|effect: %d|%d, outofparent: %d, visitedCacheNodeIds num: %lu",
        node.GetId(), static_cast<int>(node.GetDrawingCacheType()), node.IsStaticCached(),
        static_cast<int>(isDrawingCacheChanged), node.ChildHasVisibleFilter(), node.ChildHasVisibleEffect(),
        static_cast<int>(node.HasChildrenOutOfRect()), visitedCacheNodeIds_.size());
    if (node.IsStaticCached()) {
        node.SetDrawingCacheChanged(false);
    } else {
        node.SetDrawingCacheChanged(isDrawingCacheChanged);
    }
    // reset counter after executing the very first marked node
    if (firstVisitedCache_ == node.GetId()) {
        std::stack<bool>().swap(isDrawingCacheChanged_);
        firstVisitedCache_ = INVALID_NODEID;
        visitedCacheNodeIds_.clear();
    } else if (!isDrawingCacheChanged_.empty()) {
        bool isChildChanged = isDrawingCacheChanged_.top();
        isDrawingCacheChanged_.pop();
        if (!isDrawingCacheChanged_.empty()) {
            isDrawingCacheChanged_.top() = isDrawingCacheChanged_.top() || isChildChanged;
        }
    }
}

void RSUniRenderVisitor::CheckColorSpace(RSSurfaceRenderNode& node)
{
    const sptr<SurfaceBuffer>& buffer = node.GetBuffer();
    if (buffer != nullptr) {
        using namespace HDI::Display::Graphic::Common::V1_0;
        CM_ColorSpaceInfo colorSpaceInfo;
        if (MetadataHelper::GetColorSpaceInfo(buffer, colorSpaceInfo) != GSERROR_OK) {
            RS_LOGD("RSUniRenderVisitor::CheckColorSpace: get color space info failed.");
            return;
        }

        if (colorSpaceInfo.primaries != COLORPRIMARIES_SRGB && !IsHardwareComposerEnabled()) {
            newColorSpace_ = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
            RS_LOGD("RSUniRenderVisitor::CheckColorSpace: node(%{public}s) set new colorspace primaries %{public}d",
                    node.GetName().c_str(), colorSpaceInfo.primaries);
        }
        return;
    }

    if (node.IsAppWindow()) {
        auto colorspace = node.GetColorSpace();
        if (colorspace != GRAPHIC_COLOR_GAMUT_SRGB) {
            newColorSpace_ = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
            RS_LOGD("RSUniRenderVisitor::CheckColorSpace: node(%{public}s) set new colorgamut %{public}d",
                    node.GetName().c_str(), colorspace);
        }
    }
}

void RSUniRenderVisitor::HandleColorGamuts(RSDisplayRenderNode& node, const sptr<RSScreenManager>& screenManager)
{
    RSScreenType screenType = BUILT_IN_TYPE_SCREEN;
    if (screenManager->GetScreenType(node.GetScreenId(), screenType) != SUCCESS) {
        RS_LOGD("RSUniRenderVisitor::HandleColorGamuts get screen type failed.");
        return;
    }

    if (screenType == VIRTUAL_TYPE_SCREEN) {
        ScreenColorGamut screenColorGamut;
        if (screenManager->GetScreenColorGamut(node.GetScreenId(), screenColorGamut) != SUCCESS) {
            RS_LOGD("RSUniRenderVisitor::HandleColorGamuts get screen color gamut failed.");
            return;
        }
        newColorSpace_ = static_cast<GraphicColorGamut>(screenColorGamut);
    }
}

void RSUniRenderVisitor::CheckPixelFormat(RSSurfaceRenderNode& node)
{
    if (hasFingerprint_) {
        RS_LOGD("RSUniRenderVisitor::CheckPixelFormat hasFingerprint is true.");
        return;
    }
    const sptr<SurfaceBuffer>& buffer = node.GetBuffer();
    if (buffer == nullptr) {
        RS_LOGD("RSUniRenderVisitor::CheckPixelFormat node(%{public}s) did not have buffer.", node.GetName().c_str());
        return;
    }

    if (node.GetFingerprint()) {
        hasFingerprint_ = true;
        newPixelFormat_ = GRAPHIC_PIXEL_FMT_RGBA_1010102;
        RS_LOGD("RSUniRenderVisitor::CheckPixelFormat newPixelFormat_ is set 1010102 for fingerprint.");
        return;
    }

    auto bufferPixelFormat = buffer->GetFormat();
    if ((bufferPixelFormat == GRAPHIC_PIXEL_FMT_RGBA_1010102 ||
        bufferPixelFormat == GRAPHIC_PIXEL_FMT_YCBCR_P010 ||
        bufferPixelFormat == GRAPHIC_PIXEL_FMT_YCRCB_P010) && !IsHardwareComposerEnabled()) {
        newPixelFormat_ = GRAPHIC_PIXEL_FMT_RGBA_1010102;
        RS_LOGD("RSUniRenderVisitor::CheckPixelFormat pixelformat is set to 1010102 for 10bit buffer");
    }
}

void RSUniRenderVisitor::HandlePixelFormat(RSDisplayRenderNode& node, const sptr<RSScreenManager>& screenManager)
{
    RSScreenType screenType = BUILT_IN_TYPE_SCREEN;
    if (screenManager->GetScreenType(node.GetScreenId(), screenType) != SUCCESS) {
        RS_LOGD("RSUniRenderVisitor::HandlePixelFormat get screen type failed.");
        return;
    }

    if (screenType == VIRTUAL_TYPE_SCREEN) {
        if (screenManager->GetPixelFormat(node.GetScreenId(), newPixelFormat_) != SUCCESS) {
            RS_LOGD("RSUniRenderVisitor::HandlePixelFormat get screen color gamut failed.");
        }
    }
}

void RSUniRenderVisitor::PrepareDisplayRenderNode(RSDisplayRenderNode& node)
{
    currentVisitDisplay_ = node.GetScreenId();
    displayHasSecSurface_.emplace(currentVisitDisplay_, false);
    displayHasSkipSurface_.emplace(currentVisitDisplay_, false);
    hasCaptureWindow_.emplace(currentVisitDisplay_, false);
    dirtySurfaceNodeMap_.clear();

    RS_TRACE_NAME("RSUniRender:PrepareDisplay " + std::to_string(currentVisitDisplay_));
    curDisplayDirtyManager_ = node.GetDirtyManager();
    if (!curDisplayDirtyManager_) {
        return;
    }
    // set 1st elem for display dirty
    accumulatedDirtyRegions_.emplace_back(RectI());
    curDisplayDirtyManager_->Clear();
    curDisplayNode_ = node.shared_from_this()->ReinterpretCastTo<RSDisplayRenderNode>();

    dirtyFlag_ = isDirty_;
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSUniRenderVisitor::PrepareDisplayRenderNode ScreenManager is nullptr");
        return;
    }
    screenInfo_ = screenManager->QueryScreenInfo(node.GetScreenId());
    prepareClipRect_.SetAll(0, 0, screenInfo_.width, screenInfo_.height);
    // rcd message send
    if ((screenInfo_.state == ScreenState::HDI_OUTPUT_ENABLE) &&
        RSSingleton<RoundCornerDisplay>::GetInstance().GetRcdEnable()) {
        using rcd_msg = RSSingleton<RsMessageBus>;
        rcd_msg::GetInstance().SendMsg<uint32_t, uint32_t>(TOPIC_RCD_DISPLAY_SIZE,
            screenInfo_.width, screenInfo_.height);
        rcd_msg::GetInstance().SendMsg<ScreenRotation>(TOPIC_RCD_DISPLAY_ROTATION,
            node.GetScreenRotation());
    }

    parentSurfaceNodeMatrix_ = Drawing::Matrix();
    auto geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr != nullptr) {
        geoPtr->UpdateByMatrixFromSelf();
        parentSurfaceNodeMatrix_ = geoPtr->GetAbsMatrix();
        if (geoPtr->IsNeedClientCompose()) {
            isHardwareForcedDisabled_ = true;
        }
    }
    dirtyFlag_ = dirtyFlag_ || node.IsRotationChanged();
    // when display is in rotation state, occlusion relationship will be ruined,
    // hence partial-render quick-reject should be disabled.
    if(node.IsRotationChanged()) {
        isOpDropped_ = false;
        RS_TRACE_NAME("ClosePartialRender 1 RotationChanged");
    }
    node.UpdateRotation();
    curAlpha_ = node.GetRenderProperties().GetAlpha();
    newColorSpace_ = GRAPHIC_COLOR_GAMUT_SRGB;
    hasFingerprint_ = false;
    newPixelFormat_ = GRAPHIC_PIXEL_FMT_RGBA_8888;
    PrepareChildren(node);
    auto mirrorNode = node.GetMirrorSource().lock();
    if (mirrorNode) {
        mirroredDisplays_.insert(mirrorNode->GetScreenId());
    }

    node.GetCurAllSurfaces().clear();
    node.CollectSurface(node.shared_from_this(), node.GetCurAllSurfaces(), true, false);

    HandleColorGamuts(node, screenManager);
    HandlePixelFormat(node, screenManager);
    RSRcdRenderManager::GetInstance().DoPrepareRenderTask(rcdInfo_->prepareInfo);
}

bool RSUniRenderVisitor::CheckIfSurfaceRenderNodeStatic(RSSurfaceRenderNode& node)
{
    // dirtyFlag_ includes leashWindow dirty
    // window layout change(e.g. move or zooming) | proxyRenderNode's cmd
    // temporary cannot deal with leashWindow and scbScreen, restricted to mainwindow
    if (dirtyFlag_ || node.IsDirty() || !node.IsMainWindowType() || curDisplayNode_ == nullptr) {
        return false;
    }
    if (curDisplayDirtyManager_) {
        accumulatedDirtyRegions_[0] = curDisplayDirtyManager_->GetCurrentFrameDirtyRegion();
    }
    // if node has to be prepared, it's not static
    bool isClassifyByRootNode = (quickSkipPrepareType_ >= QuickSkipPrepareType::STATIC_APP_INSTANCE);
    NodeId rootId = node.GetInstanceRootNodeId();
    if (RSMainThread::Instance()->CheckNodeHasToBePreparedByPid(
        isClassifyByRootNode ? rootId : node.GetId(), isClassifyByRootNode)) {
        return false;
    }
    // [Attention] node's ability pid could be different but should have same rootId
    auto abilityNodeIds = node.GetAbilityNodeIds();
    bool result = isClassifyByRootNode
        ? RSMainThread::Instance()->CheckNodeHasToBePreparedByPid(rootId, true)
        : std::any_of(abilityNodeIds.begin(), abilityNodeIds.end(), [&](uint64_t nodeId) {
            return RSMainThread::Instance()->CheckNodeHasToBePreparedByPid(nodeId, false);
        });
    if (result) {
        return false;
    }
    node.UpdateSurfaceCacheContentStatic({});
    RS_OPTIONAL_TRACE_NAME("Skip static surface " + node.GetName() + " nodeid - pid: " +
        std::to_string(node.GetId()) + " - " + std::to_string(ExtractPid(node.GetId())));
    // static node's dirty region is empty
    auto dirtyManager = node.GetDirtyManager();
    if (dirtyManager) {
        dirtyManager->Clear();
        if (node.IsTransparent()) {
            dirtyManager->UpdateVisitedDirtyRects(accumulatedDirtyRegions_);
        }
        node.UpdateFilterCacheStatusIfNodeStatic(prepareClipRect_, curDisplayNode_->IsRotationChanged());
    }
    node.ResetDrawingCacheStatusIfNodeStatic(allCacheFilterRects_);
    // Attention: curSurface info would be reset as upper surfaceParent if it has
    ResetCurSurfaceInfoAsUpperSurfaceParent(node);
    // static surface keeps same position
    curDisplayNode_->UpdateSurfaceNodePos(node.GetId(), curDisplayNode_->GetLastFrameSurfacePos(node.GetId()));
    return true;
}

void RSUniRenderVisitor::ResetCurSurfaceInfoAsUpperSurfaceParent(RSSurfaceRenderNode& node)
{
    // record current frame mainwindow or leashwindow node
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        curMainAndLeashWindowNodesIds_.push(node.GetId());
        curDisplayNode_->RecordMainAndLeashSurfaces(node.shared_from_this());
    }
    // only reset for instance node
    if (curSurfaceNode_ == nullptr || curSurfaceNode_->GetId() != node.GetId()) {
        return;
    }
    if (auto directParent = node.GetParent().lock()) {
        if (auto parentInstance = directParent->GetInstanceRootNode()) {
            // in case leashwindow is not directParent
            auto surfaceParent = parentInstance->ReinterpretCastTo<RSSurfaceRenderNode>();
            if (surfaceParent && (surfaceParent->IsLeashWindow() || surfaceParent->IsMainWindowType())) {
                curSurfaceNode_ = surfaceParent;
                curSurfaceDirtyManager_ = surfaceParent->GetDirtyManager();
                filterInGlobal_ = surfaceParent->IsTransparent();
                return;
            }
            curSurfaceNode_ = nullptr;
            curSurfaceDirtyManager_ = nullptr;
            filterInGlobal_ = true;
        }
    }
}

bool RSUniRenderVisitor::IsHardwareComposerEnabled()
{
    return !isHardwareForcedDisabled_;
}

void RSUniRenderVisitor::ClearTransparentBeforeSaveLayer()
{
    RS_TRACE_NAME("ClearTransparentBeforeSaveLayer");
    if (!IsHardwareComposerEnabled()) {
        return;
    }
    for (auto& node : hardwareEnabledNodes_) {
        if (!node->ShouldPaint()) {
            continue;
        }
        auto dstRect = node->GetDstRect();
        if (dstRect.IsEmpty()) {
            continue;
        }
        canvas_->Save();
        canvas_->ClipRect({ static_cast<float>(dstRect.GetLeft()), static_cast<float>(dstRect.GetTop()),
                              static_cast<float>(dstRect.GetRight()), static_cast<float>(dstRect.GetBottom()) },
            Drawing::ClipOp::INTERSECT, false);
        canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
        canvas_->Restore();
    }
}

void RSUniRenderVisitor::MarkSubHardwareEnableNodeState(RSSurfaceRenderNode& surfaceNode)
{
    if (!IsHardwareComposerEnabled()) {
        return;
    }

    // hardware enabled type case: mark self
    if (surfaceNode.IsHardwareEnabledType()) {
        surfaceNode.SetHardwareForcedDisabledState(true);
        return;
    }

    if (!surfaceNode.IsAppWindow() && !surfaceNode.IsAbilityComponent() && !surfaceNode.IsLeashWindow()) {
        return;
    }

    // ability component type case: check pid
    if (surfaceNode.IsAbilityComponent()) {
        pid_t pid = ExtractPid(surfaceNode.GetId());
        for (auto& childNode : hardwareEnabledNodes_) {
            pid_t childPid = ExtractPid(childNode->GetId());
            if (pid == childPid) {
                childNode->SetHardwareForcedDisabledState(true);
            }
        }
        return;
    }
    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes;
    if (surfaceNode.IsAppWindow()) {
        hardwareEnabledNodes = surfaceNode.GetChildHardwareEnabledNodes();
    } else {
        for (auto& child : *surfaceNode.GetChildren()) {
            auto appNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
            if (appNode && appNode->IsAppWindow()) {
                hardwareEnabledNodes = appNode->GetChildHardwareEnabledNodes();
                break;
            }
        }
    }
    // app window type case: mark all child hardware enabled nodes
    for (auto& node : hardwareEnabledNodes) {
        auto childNode = node.lock();
        if (childNode) {
            childNode->SetHardwareForcedDisabledState(true);
        }
    }
}

void RSUniRenderVisitor::CollectAppNodeForHwc(std::shared_ptr<RSSurfaceRenderNode> surfaceNode)
{
    if (!IsHardwareComposerEnabled() || !surfaceNode || surfaceNode->GetChildHardwareEnabledNodes().empty()) {
        return;
    }
    if (surfaceNode->IsHardwareEnabledTopSurface()) {
        hardwareEnabledTopNodes_.emplace_back(surfaceNode);
    } else {
        appWindowNodesInZOrder_.emplace_back(surfaceNode);
    }
}

void RSUniRenderVisitor::PrepareTypesOfSurfaceRenderNodeBeforeUpdate(RSSurfaceRenderNode& node)
{
    // if current surfacenode is a main window type, reset the curSurfaceDirtyManager
    // reset leash window's dirtyManager pointer to avoid curSurfaceDirtyManager mis-pointing
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        node.SetFilterCacheFullyCovered(false);
        node.ResetFilterNodes();
        // [planning] check if it is not reset recursively
        firstVisitedCache_ = INVALID_NODEID;
        curSurfaceNode_ = node.ReinterpretCastTo<RSSurfaceRenderNode>();
        RSMainThread::Instance()->CheckAndUpdateInstanceContentStaticStatus(curSurfaceNode_);
        curSurfaceDirtyManager_ = node.GetDirtyManager();
        if (curSurfaceDirtyManager_ == nullptr) {
            RS_LOGE("RSUniRenderVisitor::PrepareTypesOfSurfaceRenderNodeBeforeUpdate %{public}s has no"
                " SurfaceDirtyManager", node.GetName().c_str());
            return;
        }
        curSurfaceDirtyManager_->Clear();
        if (node.IsTransparent()) {
            curSurfaceDirtyManager_->UpdateVisitedDirtyRects(accumulatedDirtyRegions_);
        }
        curSurfaceDirtyManager_->SetSurfaceSize(screenInfo_.width, screenInfo_.height);
        if (isTargetDirtyRegionDfxEnabled_ && CheckIfSurfaceTargetedForDFX(node.GetName())) {
            curSurfaceDirtyManager_->MarkAsTargetForDfx();
        }
        ClassifyUIFirstSurfaceDirtyStatus(node);
    }

    // collect app window node's child hardware enabled node
    if (node.IsHardwareEnabledType() && curSurfaceNode_) {
        curSurfaceNode_->AddChildHardwareEnabledNode(node.ReinterpretCastTo<RSSurfaceRenderNode>());
        node.SetLocalZOrder(localZOrder_++);
    }
}

void RSUniRenderVisitor::ClassifyUIFirstSurfaceDirtyStatus(RSSurfaceRenderNode& node)
{
    if (node.IsMainWindowType()) {
        isCachedSurfaceReuse_ = (quickSkipPrepareType_ >= QuickSkipPrepareType::STATIC_CACHE_SURFACE) &&
            (RSMainThread::Instance()->GetDeviceType() == DeviceType::PC) &&
            CheckIfUIFirstSurfaceContentReusable(curSurfaceNode_, isSurfaceDirtyNodeLimited_);
        // The condition of childHasVisibleFilter in QuerySubAssignable can not be used here
        // Because child node's filter is not collected yet, so disable prepare optimization when node is transparent
        // [planning]: detect the filter change before prepare, and use the last frame result
        isSurfaceDirtyNodeLimited_ = (quickSkipPrepareType_ == QuickSkipPrepareType::CONTENT_DIRTY_CACHE_SURFACE) &&
            !node.IsTransparent() && isSurfaceDirtyNodeLimited_ &&
            node.IsOnlyBasicGeoTransform() && node.IsContentDirtyNodeLimited();
        if (isCachedSurfaceReuse_) {
            node.SetGeoUpdateDelay(dirtyFlag_);
        }
    }
}

bool RSUniRenderVisitor::CheckIfUIFirstSurfaceContentReusable(std::shared_ptr<RSSurfaceRenderNode>& node,
    bool& isAssigned)
{
    if (!isUIFirst_ || node == nullptr) {
        return false;
    }
    auto deviceType = RSMainThread::Instance()->GetDeviceType();
    if (auto directParent = node->GetParent().lock()) {
        if (auto parentInstance = directParent->GetInstanceRootNode()) {
            auto surfaceParent = parentInstance->ReinterpretCastTo<RSSurfaceRenderNode>();
            if (surfaceParent && surfaceParent->IsLeashWindow()) {
                isAssigned =
                    RSUniRenderUtil::IsNodeAssignSubThread(surfaceParent, curDisplayNode_->IsRotationChanged());
                RS_OPTIONAL_TRACE_NAME_FMT("%s CheckIfUIFirstSurfaceContentReusable(leash): %d, isAssigned %d",
                    surfaceParent->GetName().c_str(), surfaceParent->IsUIFirstCacheReusable(deviceType), isAssigned);
                return isAssigned && surfaceParent->IsUIFirstCacheReusable(deviceType);
            }
        }
    }
    isAssigned = RSUniRenderUtil::IsNodeAssignSubThread(node, curDisplayNode_->IsRotationChanged());
    RS_OPTIONAL_TRACE_NAME_FMT("%s CheckIfUIFirstSurfaceContentReusable(mainwindow): %d, isAssigned %d",
        node->GetName().c_str(), node->IsUIFirstCacheReusable(deviceType), isAssigned);
    return isAssigned && node->IsUIFirstCacheReusable(deviceType);
}

void RSUniRenderVisitor::PrepareTypesOfSurfaceRenderNodeAfterUpdate(RSSurfaceRenderNode& node)
{
    if (!curSurfaceDirtyManager_) {
        return;
    }
    const auto& properties = node.GetRenderProperties();
    if (properties.NeedFilter()) {
        UpdateForegroundFilterCacheWithDirty(node, *curSurfaceDirtyManager_);
        if (auto parentNode = node.GetParent().lock()) {
            parentNode->SetChildHasVisibleFilter(true);
        }
        if (curSurfaceNode_) {
            curSurfaceNode_->UpdateFilterNodes(node.shared_from_this());
        }
    }
    if (node.IsLeashWindow()) {
        auto children = node.GetSortedChildren();
        for (auto& child : *children) {
            if (child->ChildHasVisibleFilter()) {
                node.SetChildHasVisibleFilter(true);
                break;
            }
        }
    } else if (node.IsMainWindowType()) {
        isCachedSurfaceReuse_ = false;
        isSurfaceDirtyNodeLimited_ = false;
        bool hasFilter = node.IsTransparent() && properties.NeedFilter();
        bool hasHardwareNode = !node.GetChildHardwareEnabledNodes().empty();
        bool hasAbilityComponent = !node.GetAbilityNodeIds().empty();
        auto rsParent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.GetParent().lock());
        if (rsParent && rsParent->IsLeashWindow()) {
            rsParent->SetHasFilter(hasFilter);
            rsParent->SetHasHardwareNode(hasHardwareNode);
            rsParent->SetHasAbilityComponent(hasAbilityComponent);
            // [uifirst] leashWindow add or clear abilityNodeId for checking subthread node status
            if (hasAbilityComponent) {
                rsParent->AddAbilityComponentNodeIds(node.GetAbilityNodeIds());
            } else {
                rsParent->ResetAbilityNodeIds();
            }
        } else {
            node.SetHasFilter(hasFilter);
            node.SetHasHardwareNode(hasHardwareNode);
            node.SetHasAbilityComponent(hasAbilityComponent);
        }
        if (node.IsTransparent() &&
            curSurfaceDirtyManager_->IfCacheableFilterRectFullyCover(node.GetOldDirtyInSurface())) {
            node.SetFilterCacheFullyCovered(true);
            RS_LOGD("SetFilterCacheFullyCovered surfacenode %{public}" PRIu64 " [%{public}s]",
                node.GetId(), node.GetName().c_str());
        }
        node.CalcFilterCacheValidForOcclusion();
        RS_OPTIONAL_TRACE_NAME(node.GetName() + " PreparedNodes: " +
            std::to_string(preparedCanvasNodeInCurrentSurface_));
        preparedCanvasNodeInCurrentSurface_ = 0;
    }
    // accumulate all visited dirty rects including leash window's shadow dirty
    if ((node.IsMainWindowType() || node.IsLeashWindow()) && curSurfaceDirtyManager_->IsCurrentFrameDirty()) {
        accumulatedDirtyRegions_.emplace_back(curSurfaceDirtyManager_->GetCurrentFrameDirtyRegion());
    }
}

void RSUniRenderVisitor::UpdateSecurityAndSkipLayerRecord(RSSurfaceRenderNode& node)
{
    if (node.GetHasSecurityLayer()) {
        displayHasSecSurface_[currentVisitDisplay_] = true;
    }
    if (node.GetHasSkipLayer() && node.GetName().find(CAPTURE_WINDOW_NAME) == std::string::npos) {
        displayHasSkipSurface_[currentVisitDisplay_] = true;
    }
}

void RSUniRenderVisitor::UpdateForegroundFilterCacheWithDirty(RSRenderNode& node,
    RSDirtyRegionManager& dirtyManager, bool isForegound)
{
    node.UpdateFilterCacheWithDirty(dirtyManager, isForegound);
    node.UpdateFilterCacheManagerWithCacheRegion(dirtyManager, prepareClipRect_, isForegound);
}

bool RSUniRenderVisitor::IsSubTreeOccluded(RSRenderNode& node) const
{
    if (!isOcclusionEnabled_) {
        return false;
    }
    // step1. apply occlusion info for surfacenode and skip fully covered subtree
    if (node.GetType() == RSRenderNodeType::SURFACE_NODE) {
        auto& surfaceNode = static_cast<RSSurfaceRenderNode&>(node);
        if (surfaceNode.IsMainWindowType()) {
            RS_TRACE_NAME_FMT("RSUniRenderVisitor::IsSubTreeOccluded node[%s]"
            "name:[%s] visibleRegionIsEmpty[%d]", std::to_string(node.GetId()).c_str(),
            surfaceNode.GetName().c_str(), surfaceNode.GetVisibleRegion().IsEmpty());
            return surfaceNode.GetVisibleRegion().IsEmpty();
        }
    }
    // step2.1 For partial visible surface, intersected region->rects in surface
    // step2.2 check if clean subtree in occlusion rects
    return false;
}

void RSUniRenderVisitor::QuickPrepareDisplayRenderNode(RSDisplayRenderNode& node)
{
    // 0. init display info
    RS_TRACE_NAME("RSUniRender:QuickPrepareDisplayRenderNode " + std::to_string(node.GetScreenId()));
    if (!InitDisplayInfo(node)) {
        RS_LOGE("RSUniRenderVisitor::QuickPrepareDisplayRenderNode InitDisplayInfo fail");
        return;
    }
    ancestorNodeHasAnimation_ = false;

    dirtyFlag_ = isDirty_ || node.IsRotationChanged();
    prepareClipRect_ = screenRect_;
    curAlpha_ = 1.0f;
    node.UpdateRotation();

    if (node.IsSubTreeDirty()) {
        QuickPrepareChildren(node);
    }
    PostPrepare(node);

    UpdateSurfaceDirtyAndGlobalDirty();
    SurfaceOcclusionCallbackToWMS();
    curDisplayNode_->UpdatePartialRenderParams();
    curDisplayNode_->UpdateScreenRenderParams(screenInfo_, displayHasSecSurface_, displayHasSkipSurface_,
        hasCaptureWindow_);
    HandleColorGamuts(node, screenManager_);
    HandlePixelFormat(node, screenManager_);
    if (UNLIKELY(!SharedTransitionParam::unpairedShareTransitions_.empty())) {
        ProcessUnpairedSharedTransitionNode();
    }
}

void RSUniRenderVisitor::QuickPrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    RS_TRACE_NAME_FMT("RSUniRender::QuickPrepareSurfaceRenderNode: node[%llu] name[%s] pid[%d] nodeType[%d] "
        "subTreeDirty[%d]", node.GetId(), node.GetName().c_str(), ExtractPid(node.GetId()),
        static_cast<int>(node.GetSurfaceNodeType()), node.IsSubTreeDirty());
    RS_LOGD("RSUniRender::QuickPrepareSurfaceRenderNode:[%{public}s] nodeid:[%{public}" PRIu64 "]"
        "pid:[%{public}d] nodeType:[%{public}d] subTreeDirty[%{public}d]",
        node.GetName().c_str(), node.GetId(), ExtractPid(node.GetId()),
        static_cast<int>(node.GetSurfaceNodeType()), node.IsSubTreeDirty());

    // 0. init curSurface* info and check current node need to tranverse
    if (!BeforeUpdateSurfaceDirtyCalc(node)) {
        RS_LOGE("RSUniRenderVisitor::QuickPrepareSurfaceRenderNode BeforeUpdateSurfaceDirtyCalc fail");
        RSUifirstManager::Instance().DisableUifirstNode(node);
        return;
    }

    // 1. Update matrix and collect dirty region
    auto dirtyFlag = dirtyFlag_;
    auto prepareClipRect = prepareClipRect_;
    dirtyFlag_ = node.UpdateDrawRectAndDirtyRegion(*curSurfaceDirtyManager_, node.GetParent().lock(),
        dirtyFlag_, prepareClipRect_);
    if (!AfterUpdateSurfaceDirtyCalc(node)) {
        RS_LOGE("RSUniRenderVisitor::QuickPrepareSurfaceRenderNode AfterUpdateSurfaceDirtyCalc fail");
        RSUifirstManager::Instance().DisableUifirstNode(node);
        return;
    }
    auto prevAlpha = curAlpha_;
    curAlpha_ *= std::clamp(node.GetRenderProperties().GetAlpha(), 0.f, 1.f);

    UpdateHwcNodeInfoForAppNode(node);

    // 2. Recursively traverse child nodes
    bool firstlevelBackup = traversalFirstLevelSruface_;
    traversalFirstLevelSruface_ = true;
    bool IsSubTreeNeedPrepare = node.IsSubTreeNeedPrepare(filterInGlobal_, IsSubTreeOccluded(node)) ||
        ForcePrepareSubTree();
    IsSubTreeNeedPrepare ? QuickPrepareChildren(node) :
        node.SubTreeSkipPrepare(*curSurfaceDirtyManager_, curDirty_, dirtyFlag_);

    node.SetGlobalAlpha(curAlpha_);
    PostPrepare(node);
    prepareClipRect_ = prepareClipRect;
    dirtyFlag_ = dirtyFlag;

    if (!firstlevelBackup && node.GetUifirstSupportFlag()) {
        RSUifirstManager::Instance().PrepareUifirstNode(node, ancestorNodeHasAnimation_);
    } else {
        RSUifirstManager::Instance().DisableUifirstNode(node);
    }
    traversalFirstLevelSruface_ = firstlevelBackup;

    ResetCurSurfaceInfoAsUpperSurfaceParent(node);
    curAlpha_ = prevAlpha;
}

void RSUniRenderVisitor::CalculateOcclusion(RSSurfaceRenderNode& node)
{
    // CheckAndUpdateOpaqueRegion only in mainWindow
    node.CheckAndUpdateOpaqueRegion(screenRect_, curDisplayNode_->GetRotation());
    if (!needRecalculateOcclusion_) {
        needRecalculateOcclusion_ = node.CheckIfOcclusionChanged();
    }
    // Update node visbleRegion
    Occlusion::Rect selfDrawRect = node.GetSurfaceOcclusionRect(true);
    Occlusion::Region selfDrawRegion { selfDrawRect };
    if (needRecalculateOcclusion_) {
        Occlusion::Region subResult = selfDrawRegion.Sub(accumulatedOcclusionRegion_);
        node.SetVisibleRegion(subResult);
    }
    // check current surface Participate In Occlusion
    if (node.CheckParticipateInOcclusion()) {
        accumulatedOcclusionRegion_.OrSelf(node.GetOpaqueRegion());
    }

    // collect surface occlusion visibleLevel
    auto visibleLevel = GetRegionVisibleLevel(node.GetVisibleRegion(), selfDrawRegion);
    // wms default all visible about sefdrawing node and AbilityComponent node
    if (node.IsSelfDrawingNode() || node.IsAbilityComponent()) {
        dstCurVisVec_.emplace_back(std::make_pair(node.GetId(),
            WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE));
        return;
    }
    if (visibleLevel != RSVisibleLevel::RS_INVISIBLE) {
        dstCurVisVec_.emplace_back(std::make_pair(node.GetId(),
            node.GetVisibleLevelForWMS(visibleLevel)));
    }
}

RSVisibleLevel RSUniRenderVisitor::GetRegionVisibleLevel(const Occlusion::Region& visibleRegion,
    const Occlusion::Region& selfDrawRegion)
{
    if (visibleRegion.IsEmpty()) {
        return RSVisibleLevel::RS_INVISIBLE;
    } else if (visibleRegion.Area() == selfDrawRegion.Area()) {
        return RSVisibleLevel::RS_ALL_VISIBLE;
    } else if (static_cast<uint>(visibleRegion.Area()) <
        (static_cast<uint>(selfDrawRegion.Area()) >> VISIBLEAREARATIO_FORQOS)) {
        return RSVisibleLevel::RS_SEMI_DEFAULT_VISIBLE;
    }
    return RSVisibleLevel::RS_SEMI_NONDEFAULT_VISIBLE;
}

void RSUniRenderVisitor::RecordDrawCmdList(RSRenderNode& node)
{
    auto extendRecodingCanvas = new ExtendRecordingCanvas(node.GetRenderProperties().GetBoundsWidth(),
        node.GetRenderProperties().GetBoundsHeight(), true);
    std::unique_ptr<RSPaintFilterCanvas> recordingCanvas = std::make_unique<RSPaintFilterCanvas>(extendRecodingCanvas);
    CacheType cacheType = node.GetCacheType();
    node.ProcessTransitionBeforeChildren(*recordingCanvas);
    switch (cacheType) {
        case CacheType::NONE: {
            auto preCache = recordingCanvas->GetCacheType();
            if (node.HasCacheableAnim() && isDrawingCacheEnabled_) {
                recordingCanvas->SetCacheType(RSPaintFilterCanvas::CacheType::ENABLED);
            }
            node.ProcessAnimatePropertyBeforeChildren(*recordingCanvas);
            node.ProcessRenderContents(*recordingCanvas);
            ProcessChildren(node);
            node.ProcessAnimatePropertyAfterChildren(*recordingCanvas);
            if (node.HasCacheableAnim() && isDrawingCacheEnabled_) {
                recordingCanvas->SetCacheType(preCache);
            }
            break;
        }
        case CacheType::CONTENT: {
            if (node.IsNodeGroupIncludeProperty()) {
                node.ProcessAnimatePropertyBeforeChildren(*recordingCanvas, false);
            } else {
                node.ProcessAnimatePropertyBeforeChildren(*recordingCanvas);
            }
            node.DrawCacheSurface(*recordingCanvas, threadIndex_, false);
            node.ProcessAnimatePropertyAfterChildren(*recordingCanvas);
            cacheRenderNodeMapRects_.push_back(node.GetOldDirtyInSurface());
            break;
        }
        case CacheType::ANIMATE_PROPERTY: {
            node.DrawCacheSurface(*recordingCanvas, threadIndex_, false);
            break;
        }
        default:
            break;
    }
    node.ProcessTransitionAfterChildren(*recordingCanvas);
    // node.UpdateStagingDrawCmdList(extendRecodingCanvas->GetDrawCmdList());
    node.SetNeedSyncFlag(true);
    recordingCanvas.reset();
}

void RSUniRenderVisitor::QuickPrepareEffectRenderNode(RSEffectRenderNode& node)
{
    // 0. check current node need to tranverse
    auto nodeParent = node.GetParent().lock();
    auto dirtyManager = curSurfaceNode_ ? curSurfaceDirtyManager_ : curDisplayDirtyManager_;
    auto dirtyFlag = dirtyFlag_;
    auto prevAlpha = curAlpha_;
    curAlpha_ *= std::clamp(node.GetRenderProperties().GetAlpha(), 0.f, 1.f);

    RectI prepareClipRect = prepareClipRect_;
    dirtyFlag_ = node.UpdateDrawRectAndDirtyRegion(*dirtyManager, nodeParent, dirtyFlag_, prepareClipRect_);

    // 1. Recursively traverse child nodes
    bool IsSubTreeNeedPrepare = node.IsSubTreeNeedPrepare(filterInGlobal_) || ForcePrepareSubTree();
    IsSubTreeNeedPrepare ? QuickPrepareChildren(node) :
        node.SubTreeSkipPrepare(*dirtyManager, curDirty_, dirtyFlag_);

    PostPrepare(node);
    prepareClipRect_ = prepareClipRect;
    dirtyFlag_ = dirtyFlag;
    curAlpha_ = prevAlpha;
}

void RSUniRenderVisitor::QuickPrepareCanvasRenderNode(RSCanvasRenderNode& node)
{
    // 0. check current node need to traverse
    auto nodeParent = node.GetParent().lock();
    auto dirtyManager = curSurfaceNode_ ? curSurfaceDirtyManager_ : curDisplayDirtyManager_;
    auto dirtyFlag = dirtyFlag_;
    auto prevAlpha = curAlpha_;
    curAlpha_ *= std::clamp(node.GetRenderProperties().GetAlpha(), 0.f, 1.f);

    if (isDrawingCacheEnabled_) {
        node.UpdateDrawingCacheInfoBeforeChildren();
    }

    RectI prepareClipRect = prepareClipRect_;
    dirtyFlag_ = node.UpdateDrawRectAndDirtyRegion(*dirtyManager, nodeParent, dirtyFlag_, prepareClipRect_);
    // update prepare clip before children
    UpdatePrepareClip(node);

    // 1. Recursively traverse child nodes if above curSurfaceNode and subnode need draw
    bool IsSubTreeNeedPrepare = !curSurfaceNode_ || node.IsSubTreeNeedPrepare(filterInGlobal_) ||
        ForcePrepareSubTree();
    IsSubTreeNeedPrepare ? QuickPrepareChildren(node) :
        node.SubTreeSkipPrepare(*dirtyManager, curDirty_, dirtyFlag_);

    PostPrepare(node);
    prepareClipRect_ = prepareClipRect;
    dirtyFlag_ = dirtyFlag;
    curAlpha_ = prevAlpha;
}

void RSUniRenderVisitor::UpdatePrepareClip(RSRenderNode& node)
{
    const auto& property = node.GetRenderProperties();
    auto geoPtr = property.GetBoundsGeometry();
    if (geoPtr == nullptr) {
        return;
    }
    // Dirty Region use abstract coordinate, property of node use relative coordinate
    // BoundsRect(if exists) is mapped to absRect_ of RSObjAbsGeometry.
    if (property.GetClipToBounds()) {
        prepareClipRect_ = prepareClipRect_.IntersectRect(geoPtr->GetAbsRect());
    }
    // FrameRect(if exists) is mapped to rect using abstract coordinate explicitly by calling MapAbsRect.
    if (property.GetClipToFrame()) {
        // MapAbsRect do not handle the translation of OffsetX and OffsetY
        RectF frameRect{
            property.GetFrameOffsetX() * geoPtr->GetAbsMatrix().Get(Drawing::Matrix::SCALE_X),
            property.GetFrameOffsetY() * geoPtr->GetAbsMatrix().Get(Drawing::Matrix::SCALE_Y),
            property.GetFrameWidth(), property.GetFrameHeight()};
        prepareClipRect_ = prepareClipRect_.IntersectRect(geoPtr->MapAbsRect(frameRect));
    }
}

void RSUniRenderVisitor::QuickPrepareChildren(RSRenderNode& node)
{
    MergeRemovedChildDirtyRegion(node);
    bool animationBackup = ancestorNodeHasAnimation_;
    ancestorNodeHasAnimation_ = ancestorNodeHasAnimation_ || node.HasAnimation();
    node.ResetChildRelevantFlags();
    node.ResetChildUifirstSupportFlag();
    auto children = node.GetSortedChildren();
    // leashwindow should not include multi mainwindow
    if (curSurfaceNode_) {
        std::for_each((*children).begin(), (*children).end(), [this](const std::shared_ptr<RSRenderNode>& node) {
            curDirty_ = node->IsDirty();
            node->QuickPrepare(shared_from_this());
        });
    } else {
        std::for_each((*children).rbegin(), (*children).rend(), [this](const std::shared_ptr<RSRenderNode>& node) {
            curDirty_ = node->IsDirty();
            node->QuickPrepare(shared_from_this());
        });
    }
    ancestorNodeHasAnimation_ = animationBackup;
    node.ResetGeoUpdateDelay();
}

bool RSUniRenderVisitor::InitDisplayInfo(RSDisplayRenderNode& node)
{
    // 1 init curDisplay and curDisplayDirtyManager
    currentVisitDisplay_ = node.GetScreenId();
    displayHasSecSurface_.emplace(currentVisitDisplay_, false);
    displayHasSkipSurface_.emplace(currentVisitDisplay_, false);
    hasCaptureWindow_.emplace(currentVisitDisplay_, false);
    curDisplayDirtyManager_ = node.GetDirtyManager();
    curDisplayNode_ = node.shared_from_this()->ReinterpretCastTo<RSDisplayRenderNode>();
    if (!curDisplayDirtyManager_ || !curDisplayNode_) {
        RS_LOGE("RSUniRenderVisitor::InitDisplayInfo dirtyMgr or node ptr is nullptr");
        return false;
    }
    curDisplayDirtyManager_->Clear();

    // 2 init screenManager info
    screenManager_ = CreateOrGetScreenManager();
    if (!screenManager_) {
        RS_LOGE("RSUniRenderVisitor::InitDisplayInfo screenManager_ is nullptr");
        return false;
    }
    screenInfo_ = screenManager_->QueryScreenInfo(node.GetScreenId());
    curDisplayDirtyManager_->SetSurfaceSize(screenInfo_.width, screenInfo_.height);
    screenRect_ = RectI{0, 0, screenInfo_.width, screenInfo_.height};

    // 3 init Occlusion info
    needRecalculateOcclusion_ = false;
    accumulatedOcclusionRegion_.Reset();
    if (!curMainAndLeashWindowNodesIds_.empty()) {
        std::queue<NodeId>().swap(curMainAndLeashWindowNodesIds_);
    }

    // 4. check isHardwareForcedDisabled
    auto geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        RS_LOGE("RSUniRenderVisitor::InitDisplayInfo geoPtr is nullptr");
        return false;
    }
    if (geoPtr->IsNeedClientCompose()) {
        isHardwareForcedDisabled_ = true;
    }

    // 5. check compositeType
    auto mirrorNode = node.GetMirrorSource().lock();
    switch (screenInfo_.state) {
        case ScreenState::PRODUCER_SURFACE_ENABLE:
            node.SetCompositeType(mirrorNode ?
                RSDisplayRenderNode::CompositeType::UNI_RENDER_MIRROR_COMPOSITE :
                RSDisplayRenderNode::CompositeType::UNI_RENDER_EXPAND_COMPOSITE);
            break;
        case ScreenState::HDI_OUTPUT_ENABLE:
            node.SetCompositeType(node.IsForceSoftComposite() ?
                RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE :
                RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE);
            break;
        default:
            RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode ScreenState unsupported");
            return false;
    }

    return true;
}

bool RSUniRenderVisitor::BeforeUpdateSurfaceDirtyCalc(RSSurfaceRenderNode& node)
{
    // 1. init and record surface info
    if (node.GetName().find(CAPTURE_WINDOW_NAME) != std::string::npos) {
        hasCaptureWindow_[currentVisitDisplay_] = true;
    }
    // only need collect first level node's security & skip layer info
    if (node.GetId() == node.GetFirstLevelNodeId()) {
        UpdateSecurityAndSkipLayerRecord(node);
    }
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        curSurfaceNode_ = node.ReinterpretCastTo<RSSurfaceRenderNode>();
        curSurfaceDirtyManager_ = node.GetDirtyManager();
        if (!curSurfaceDirtyManager_ || !curSurfaceNode_) {
            RS_LOGE("RSUniRenderVisitor::BeforeUpdateSurfaceDirtyCalc %{public}s has invalid"
                " SurfaceDirtyManager or node ptr", node.GetName().c_str());
            return false;
        }
        curSurfaceDirtyManager_->Clear();
        curSurfaceDirtyManager_->SetSurfaceSize(screenInfo_.width, screenInfo_.height);
        filterInGlobal_ = curSurfaceNode_->IsTransparent();
    }
    // 2. update surface info and CheckIfOcclusionReusable
    node.SetAncestorDisplayNode(curDisplayNode_); // set for boot animation
    node.UpdateAncestorDisplayNodeInRenderParams();
    node.CleanDstRectChanged();
    // [planning] check node isDirty can be optimized.
    needRecalculateOcclusion_ = needRecalculateOcclusion_ || node.IsDirty() ||
        node.CheckIfOcclusionReusable(preMainAndLeashWindowNodesIds_);
    return true;
}

bool RSUniRenderVisitor::AfterUpdateSurfaceDirtyCalc(RSSurfaceRenderNode& node)
{
    // 1 update surface info
    auto geoPtr = node.GetMutableRenderProperties().GetBoundsGeometry();
    if (geoPtr == nullptr) {
        return false;
    }
    UpdateDstRect(node, geoPtr->GetAbsRect(), prepareClipRect_);
    node.UpdatePositionZ();
    UpdateSurfaceRenderNodeScale(node);
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        curDisplayNode_->UpdateSurfaceNodePos(node.GetId(), node.GetOldDirtyInSurface());
    }
    // 2 Update Occlusion info before children preparation
    if (node.IsMainWindowType()) {
        CalculateOcclusion(node);
        RS_TRACE_NAME_FMT("RSUniRenderVisitor::AfterUpdateSurfaceDirtyCalc visibleRegion:%s",
            node.GetVisibleRegion().GetRegionInfo().c_str());
    }
    return true;
}

void RSUniRenderVisitor::UpdateHwcNodeInfoForAppNode(RSSurfaceRenderNode& node)
{
    // app node
    if (node.GetNeedCollectHwcNode()) {
        node.ResetChildHardwareEnabledNodes();
    }
    // hwc node
    if (node.IsHardwareEnabledType() && curSurfaceNode_) {
        if (curSurfaceNode_->GetNeedCollectHwcNode()) {
            curSurfaceNode_->AddChildHardwareEnabledNode(node.ReinterpretCastTo<RSSurfaceRenderNode>());
        }
        node.SetHardwareForcedDisabledState(false);
        if (!IsHardwareComposerEnabled() || curSurfaceNode_->GetVisibleRegion().IsEmpty() || !node.GetBuffer()) {
            node.SetHardwareForcedDisabledState(true);
            return;
        }
        auto geo = node.GetRenderProperties().GetBoundsGeometry();
        UpdateSrcRect(node, geo->GetAbsMatrix(), geo->GetAbsRect());
        UpdateHwcNodeByTransform(node);
        UpdateHwcNodeEnableByBackgroundAlpha(node);
        UpdateHwcNodeEnableBySrcRect(node);
        if (!node.IsHardwareForcedDisabled()) {
            node.SetIntersectByFilterInApp(true);
        }
    }
}

void RSUniRenderVisitor::UpdateSrcRect(RSSurfaceRenderNode& node,
    const Drawing::Matrix& absMatrix, const RectI& absRect)
{
    auto canvas = std::make_unique<Rosen::Drawing::Canvas>(screenInfo_.phyWidth, screenInfo_.phyHeight);
    canvas->ConcatMatrix(absMatrix);

    auto dstRect = node.GetDstRect();
    Drawing::RectI dst = { dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(),
                           dstRect.GetBottom() };
    bool hasRotation = false;
    if (node.GetConsumer() != nullptr) {
        auto rotation = RSBaseRenderUtil::GetRotateTransform(node.GetConsumer()->GetTransform());
        hasRotation = rotation == GRAPHIC_ROTATE_90 || rotation == GRAPHIC_ROTATE_270;
    }
    node.UpdateSrcRect(*canvas.get(), dst);
    if (node.GetBuffer()) {
        RSUniRenderUtil::UpdateRealSrcRect(node, absRect);
    }
}

void RSUniRenderVisitor::UpdateDstRect(RSSurfaceRenderNode& node, const RectI& absRect, const RectI& clipRect)
{
    auto geoPtr = node.GetRenderProperties().GetBoundsGeometry();
    if (geoPtr == nullptr) {
        return;
    }
    auto dstRect = absRect;
    // If the screen is expanded, intersect the destination rectangle with the screen rectangle
    dstRect = dstRect.IntersectRect(RectI(curDisplayNode_->GetDisplayOffsetX(), curDisplayNode_->GetDisplayOffsetY(),
        screenInfo_.width, screenInfo_.height));
    // Remove the offset of the screen
    dstRect = RectI(dstRect.left_ - curDisplayNode_->GetDisplayOffsetX(),
        dstRect.top_ - curDisplayNode_->GetDisplayOffsetY(), dstRect.GetWidth(), dstRect.GetHeight());
    // If the node is a hardware-enabled type, intersect its destination rectangle with the prepare clip rectangle
    if (node.IsHardwareEnabledType() && !clipRect.IsEmpty()) {
        dstRect = dstRect.IntersectRect(clipRect);
    }
    // Set the destination rectangle of the node
    node.SetDstRect(dstRect);
}

void RSUniRenderVisitor::UpdateHwcNodeByTransform(RSSurfaceRenderNode& node)
{
    if (!node.GetBuffer()) {
        return;
    }
    RSUniRenderUtil::DealWithNodeGravity(node, screenInfo_);
    RSUniRenderUtil::LayerRotate(node, screenInfo_);
    RSUniRenderUtil::LayerCrop(node, screenInfo_);
    RSUniRenderUtil::LayerScaleDown(node);
    node.SetCalcRectInPrepare(true);
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByBackgroundAlpha(RSSurfaceRenderNode& node)
{
    if (node.IsHardwareForcedDisabled()) {
        return;
    }
    bool bgTransport =
        static_cast<uint8_t>(node.GetRenderProperties().GetBackgroundColor().GetAlpha()) < UINT8_MAX;
    if (bgTransport) {
        node.SetHardwareForcedDisabledState(true);
    }
}

void RSUniRenderVisitor::UpdateHwcNodeEnableBySrcRect(RSSurfaceRenderNode& node)
{
    if (node.IsHardwareForcedDisabled()) {
        return;
    }
    bool hasRotation = false;
    const auto& consumer = node.GetConsumer();
    if (consumer != nullptr) {
        auto rotation = RSBaseRenderUtil::GetRotateTransform(consumer->GetTransform());
        hasRotation = rotation == GRAPHIC_ROTATE_90 || rotation == GRAPHIC_ROTATE_270;
    }
    node.UpdateHwcDisabledBySrcRect(hasRotation);
    if (node.IsHardwareDisabledBySrcRect()) {
        node.SetHardwareForcedDisabledState(true);
    }
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(std::vector<RectI>& hwcRects,
    std::shared_ptr<RSSurfaceRenderNode>& hwcNode)
{
    if (hwcNode->IsHardwareForcedDisabled()) {
        return;
    }
    auto dst = hwcNode->GetDstRect();
    for (auto rect : hwcRects) {
        if (dst.Intersect(rect)) {
            hwcNode->SetHardwareForcedDisabledState(true);
            return;
        }
    }
    hwcRects.emplace_back(dst);
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByRotateAndAlpha(std::shared_ptr<RSSurfaceRenderNode>& hwcNode)
{
    Drawing::Matrix totalMatrix;
    float alpha = 1.f;
    AccumulateMatrixAndAlpha(hwcNode, totalMatrix, alpha);
    if (!ROSEN_EQ(alpha, 1.f)) {
        hwcNode->SetHardwareForcedDisabledState(true);
        return;
    }
    // [planning] degree only multiples of 90 now
    int degree = RSUniRenderUtil::GetRotationDegreeFromMatrix(totalMatrix);
    bool hasRotate = degree % ROTATION_90 != 0;
    if (hasRotate) {
        hwcNode->SetHardwareForcedDisabledState(true);
        return;
    }
    if (!hwcNode->GetCalcRectInPrepare() &&
        !(hwcNode->GetTotalMatrix() == totalMatrix)) {
        const auto& properties = hwcNode->GetRenderProperties();
        Drawing::Rect bounds = Drawing::Rect(0, 0, properties.GetBoundsWidth(), properties.GetBoundsHeight());
        Drawing::Rect absRect;
        totalMatrix.MapRect(absRect, bounds);
        RectI rect = {absRect.left_, absRect.top_, absRect.GetWidth(), absRect.GetHeight()};
        UpdateDstRect(*hwcNode, rect, RectI());
        UpdateSrcRect(*hwcNode, totalMatrix, rect);
        UpdateHwcNodeByTransform(*hwcNode);
    }
    hwcNode->SetTotalMatrix(totalMatrix);
}

void RSUniRenderVisitor::AccumulateMatrixAndAlpha(std::shared_ptr<RSSurfaceRenderNode>& hwcNode,
    Drawing::Matrix& matrix, float& alpha)
{
    const auto& property = hwcNode->GetRenderProperties();
    alpha = property.GetAlpha();
    matrix = property.GetBoundsGeometry()->GetMatrix();
    auto parent = hwcNode->GetParent().lock();
    while (parent && parent->GetType() != RSRenderNodeType::DISPLAY_NODE) {
        const auto& property = parent->GetRenderProperties();
        alpha *= property.GetAlpha();
        matrix.PostConcat(property.GetBoundsGeometry()->GetMatrix());
        parent = parent->GetParent().lock();
    }
    if (!parent) {
        return;
    }
    const auto& parentProperty = parent->GetRenderProperties();
    alpha *= parentProperty.GetAlpha();
    matrix.PostConcat(parentProperty.GetBoundsGeometry()->GetMatrix());
}

void RSUniRenderVisitor::UpdateHwcNodeEnableAndCreateLayer(std::shared_ptr<RSSurfaceRenderNode>& node)
{
    if (!node) {
        return;
    }
    UpdateHwcNodeEnableByGlobalFilter(node);
    node->ResetNeedCollectHwcNode();
    const auto& hwcNodes = node->GetChildHardwareEnabledNodes();
    if (hwcNodes.empty()) {
        return;
    }
    std::shared_ptr<RSSurfaceRenderNode> pointWindow;
    std::vector<RectI> hwcRects;
    for(auto hwcNode : hwcNodes) {
        auto hwcNodePtr = hwcNode.lock();
        if (!hwcNodePtr || !hwcNodePtr->IsOnTheTree()) {
            continue;
        }
        if (node->IsHardwareEnabledTopSurface()) {
            pointWindow = hwcNodePtr;
            continue;
        }
        UpdateHwcNodeEnableByRotateAndAlpha(hwcNodePtr);
        UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(hwcRects, hwcNodePtr);
        UpdateHwcNodeDirtyRegionForApp(node, hwcNodePtr);
        hwcNodePtr->SetIntersectByFilterInApp(false);
        hwcNodePtr->SetCalcRectInPrepare(false);
        auto transform = RSUniRenderUtil::GetLayerTransform(*hwcNodePtr, screenInfo_);
        hwcNodePtr->SetGlobalZOrder(hwcNodePtr->IsHardwareForcedDisabled() ? -1.f : globalZOrder_++);
        hwcNodePtr->UpdateHwcNodeLayerInfo(transform);
    }
    if (pointWindow) {
        // globalZOrder_ + 1 is displayNode layer, point window must be at the top.
        pointWindow->SetGlobalZOrder(globalZOrder_ + 2);
        pointWindow->SetHardwareForcedDisabledState(!IsHardwareComposerEnabled());
        pointWindow->UpdateHwcNodeLayerInfo(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    }
}

void RSUniRenderVisitor::UpdateHwcNodeDirtyRegionForApp(std::shared_ptr<RSSurfaceRenderNode>& appNode,
    std::shared_ptr<RSSurfaceRenderNode>& hwcNode)
{
    // if current frame hwc enable status not equal with last frame
    // or current frame do gpu composistion and has buffer consumed,
    // we need merge hwc node dst rect to dirty region.
    bool updateDirty = ((!hwcNode->IsHardwareForcedDisabled()) != hwcNode->GetIsLastFrameHwcEnabled()) ||
        (hwcNode->IsHardwareForcedDisabled() && hwcNode->IsCurrentFrameBufferConsumed());
    if (updateDirty) {
        appNode->GetDirtyManager()->MergeDirtyRect(hwcNode->GetDstRect());
    }
}

void RSUniRenderVisitor::UpdateSurfaceDirtyAndGlobalDirty()
{
    auto& curMainAndLeashSurfaces = curDisplayNode_->GetAllMainAndLeashSurfaces();
    // this is used to record mainAndLeash surface accumulatedDirtyRegion by Pre-order traversal
    Occlusion::Region accumulatedDirtyRegion;
    bool hasMainAndLeashSurfaceDirty = false;
    std::for_each(curMainAndLeashSurfaces.rbegin(), curMainAndLeashSurfaces.rend(),
        [this, &accumulatedDirtyRegion, &hasMainAndLeashSurfaceDirty](RSBaseRenderNode::SharedPtr& nodePtr) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodePtr);
        RSMainThread::Instance()->GetContext().AddPendingSyncNode(nodePtr);
        auto dirtyManager = surfaceNode->GetDirtyManager();
        // 0. update hwc node enable status and create layer
        UpdateHwcNodeEnableAndCreateLayer(surfaceNode);
        // 1. calculate abs dirtyrect and update partialRenderParams
        // currently only sync visible region info
        surfaceNode->UpdatePartialRenderParams();
        if (dirtyManager->IsCurrentFrameDirty()) {
            hasMainAndLeashSurfaceDirty = true;
        }
        // 2. check surface node dirtyrect need merge into displayDirtyManager
        CheckMergeSurfaceDirtysForDisplay(surfaceNode);
        // 3. check surface node's transparent dirtyrect need merge into displayDirtyManager
        CheckMergeTransparentDirtysForDisplay(surfaceNode);
        // 4. check filter node need merge into displayDirtyManager
        CheckMergeTransparentFilterForDisplay(surfaceNode, accumulatedDirtyRegion);
    });
    curDisplayNode_->SetMainAndLeashSurfaceDirty(hasMainAndLeashSurfaceDirty);
    CheckAndUpdateFilterCacheOcclusion(curMainAndLeashSurfaces);
    CheckMergeGlobalFilterForDisplay(accumulatedDirtyRegion);
    curDisplayNode_->ClearCurrentSurfacePos();
    std::swap(preMainAndLeashWindowNodesIds_, curMainAndLeashWindowNodesIds_);
}

void RSUniRenderVisitor::SurfaceOcclusionCallbackToWMS()
{
    if (!needRecalculateOcclusion_) {
        RS_LOGD("RSUniRenderVisitor::SurfaceOcclusionCallbackToWMS no need to callback");
        return;
    }
    bool visibleChanged = dstCurVisVec_.size() != lastVisVec_.size();
    if (!visibleChanged) {
        for (uint32_t i = 0; i < dstCurVisVec_.size(); i++) {
            if ((dstCurVisVec_[i].first != lastVisVec_[i].first) ||
                (dstCurVisVec_[i].second != lastVisVec_[i].second)) {
                visibleChanged = true;
                break;
            }
        }
    }
    if (visibleChanged) {
        RSMainThread::Instance()->SurfaceOcclusionChangeCallback(dstCurVisVec_);
    }
    lastVisVec_.clear();
    std::swap(lastVisVec_, dstCurVisVec_);
}

void RSUniRenderVisitor::CheckMergeSurfaceDirtysForDisplay(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const
{
    RS_OPTIONAL_TRACE_FUNC();
    const auto& dirtyRect = surfaceNode->GetDirtyManager()->GetCurrentFrameDirtyRegion();
    auto oldDirtyInSurface = surfaceNode->GetOldDirtyInSurface();
    if (surfaceNode->IsTransparent()) {
        // 1 Handles the case of transparent surface, merge transparent dirty rect
        RectI transparentDirtyRect = oldDirtyInSurface.IntersectRect(dirtyRect);
        if (!transparentDirtyRect.IsEmpty()) {
            RS_LOGD("CheckMergeSurfaceDirtysForDisplay merge transparent dirty %{public}s rect %{public}s",
                surfaceNode->GetName().c_str(), transparentDirtyRect.ToString().c_str());
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(transparentDirtyRect);
        }
    }
    if (surfaceNode->GetZorderChanged()) {
        // 2 Zorder changed case, merge surface dest Rect
        RS_LOGD("CheckMergeSurfaceDirtysForDisplay merge GetZorderChanged %{public}s rect %{public}s",
            surfaceNode->GetName().c_str(), oldDirtyInSurface.ToString().c_str());
        curDisplayNode_->GetDirtyManager()->MergeDirtyRect(oldDirtyInSurface);
    }
    // 3 surfacePos chanded case, merge surface lastframe pos or curframe pos
    RectI lastFrameSurfacePos = curDisplayNode_->GetLastFrameSurfacePos(surfaceNode->GetId());
    RectI currentFrameSurfacePos = curDisplayNode_->GetCurrentFrameSurfacePos(surfaceNode->GetId());
    if (surfaceNode->GetAnimateState() || lastFrameSurfacePos != currentFrameSurfacePos) {
        RS_LOGD("CheckMergeSurfaceDirtysForDisplay merge surface pos changed %{public}s lastFrameRect %{public}s"
            " currentFrameRect %{public}s", surfaceNode->GetName().c_str(), lastFrameSurfacePos.ToString().c_str(),
            currentFrameSurfacePos.ToString().c_str());
        if (!lastFrameSurfacePos.IsEmpty()) {
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(lastFrameSurfacePos);
        }
        if (!currentFrameSurfacePos.IsEmpty()) {
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(currentFrameSurfacePos);
        }
    }
    // 4 shadow disappear and appear case.
    bool isShadowDisappear = !surfaceNode->GetRenderProperties().IsShadowValid() &&
        surfaceNode->IsShadowValidLastFrame();
    if (surfaceNode->GetRenderProperties().IsShadowValid() || isShadowDisappear) {
        RectI shadowDirtyRect = oldDirtyInSurface.IntersectRect(dirtyRect);
        // There are two situation here:
        // 1. SurfaceNode first has shadow or shadow radius is larger than the last frame,
        // dirtyRect == surfaceNode->GetOldDirtyInSurface()
        // 2. SurfaceNode remove shadow or shadow radius is smaller than the last frame,
        // dirtyRect > surfaceNode->GetOldDirtyInSurface()
        // So we should always merge dirtyRect here.
        if (!shadowDirtyRect.IsEmpty()) {
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(dirtyRect);
            RS_LOGD("CheckMergeSurfaceDirtysForDisplay merge ShadowValid %{public}s rect %{public}s",
                surfaceNode->GetName().c_str(), dirtyRect.ToString().c_str());
        }
        if (isShadowDisappear) {
            surfaceNode->SetShadowValidLastFrame(false);
        }
    }
    // 5 handle last and curframe surfaces which appear or disappear case
    std::vector<RectI> surfaceChangedRects = curDisplayNode_->GetSurfaceChangedRects();
    for (auto& surfaceChangedRect : surfaceChangedRects) {
        RS_LOGD("CheckMergeSurfaceDirtysForDisplay merge Surface closed %{public}s", surfaceChangedRect.ToString().c_str());
        if (!surfaceChangedRect.IsEmpty()) {
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(surfaceChangedRect);
        }
    }
}

void RSUniRenderVisitor::CheckMergeTransparentDirtysForDisplay(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const
{
    RS_OPTIONAL_TRACE_FUNC();
    const auto& dirtyRect = surfaceNode->GetDirtyManager()->GetCurrentFrameDirtyRegion();
    if (surfaceNode->HasContainerWindow()) {
        // If a surface's dirty is intersect with container region (which can be considered transparent)
        // should be added to display dirty region.
        // Note: we use containerRegion rather transparentRegion to bypass inner corner dirty problem.
        auto containerRegion = surfaceNode->GetContainerRegion();
        auto surfaceDirtyRegion = Occlusion::Region{ Occlusion::Rect{ dirtyRect } };
        auto containerDirtyRegion = containerRegion.And(surfaceDirtyRegion);
        if (!containerDirtyRegion.IsEmpty()) {
            RS_LOGD("CheckMergeContainerDirtysForDisplay merge containerDirtyRegion %{public}s region %{public}s",
                surfaceNode->GetName().c_str(), containerDirtyRegion.GetRegionInfo().c_str());
            // plan: we can use surfacenode's absrect as containerRegion's bound
            const auto& rect = containerRegion.GetBoundRef();
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(
                RectI{ rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_ });
        }
    } else {
        // warning: if a surfacenode has transparent region and opaque region, and its dirty pattern appears in
        // transparent region and opaque region in adjacent frame, may cause displaydirty region incomplete after
        // merge history (as surfacenode's dirty region merging opaque region will enlarge surface dirty region
        // which include transparent region but not counted in display dirtyregion)
        if (!surfaceNode->IsNodeDirty()) {
            return;
        }
        auto transparentRegion = surfaceNode->GetTransparentRegion();
        auto surfaceDirtyRegion = Occlusion::Region{ Occlusion::Rect{ dirtyRect } };
        Occlusion::Region transparentDirtyRegion = transparentRegion.And(surfaceDirtyRegion);
        if (!transparentDirtyRegion.IsEmpty()) {
            RS_LOGD("CheckMergeContainerDirtysForDisplay merge TransparentDirtyRegion %{public}s region %{public}s",
                surfaceNode->GetName().c_str(), transparentDirtyRegion.GetRegionInfo().c_str());
            const std::vector<Occlusion::Rect>& rects = transparentDirtyRegion.GetRegionRects();
            for (const auto& rect : rects) {
                curDisplayNode_->GetDirtyManager()->MergeDirtyRect(
                    RectI{ rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_ });
            }
        }
    }
}

void RSUniRenderVisitor::CheckMergeTransparentFilterForDisplay(
    std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
    Occlusion::Region& accumulatedDirtyRegion)
{
    if (surfaceNode->IsMainWindowType() && surfaceNode->GetVisibleRegion().IsEmpty()) {
        RS_LOGD("RSUniRenderVisitor::CheckMergeTransparentFilterForDisplay surface:%{public}s "
            "which is occluded don't need to process filter", surfaceNode->GetName().c_str());
        return;
    }
    surfaceNode->SetFilterCacheFullyCovered(false);
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    auto filterVecIter = transparentCleanFilter_.find(surfaceNode->GetId());
    if (filterVecIter != transparentCleanFilter_.end()) {
        RS_LOGD("RSUniRenderVisitor::CheckMergeTransparentFilterForDisplay surface:%{public}s "
            "has transparentCleanFilter", surfaceNode->GetName().c_str());
        // check accumulatedDirtyRegion influence filter nodes which in the current surface
        for (auto it = filterVecIter->second.begin(); it != filterVecIter->second.end(); ++it) {
            auto filterNode = nodeMap.GetRenderNode<RSRenderNode>(it->first);
            if (filterNode == nullptr) {
                continue;
            }
            auto filterRegion = Occlusion::Region{ Occlusion::Rect{ it->second } };
            auto filterDirtyRegion = filterRegion.And(accumulatedDirtyRegion);
            if (!filterDirtyRegion.IsEmpty()) {
                if (filterNode->GetRenderProperties().GetBackgroundFilter()) {
                    // backgroundfilter affected by below dirty
                    filterNode->UpdateFilterCacheWithDirty(*(curDisplayNode_->GetDirtyManager()));
                }
                RS_LOGD("RSUniRenderVisitor::CheckMergeTransparentFilterForDisplay merge "
                    "filterRegion %{public}s region %{public}s",
                    surfaceNode->GetName().c_str(), it->second.ToString().c_str());
                curDisplayNode_->GetDirtyManager()->MergeDirtyRect(it->second);
                if (filterNode->GetRenderProperties().GetFilter()) {
                    // foregroundfilter affected by below dirty
                    filterNode->UpdateFilterCacheWithDirty(*(curDisplayNode_->GetDirtyManager()), true);
                }
            } else {
                globalFilter_.insert(it->second);
            }
            filterNode->MarkAndUpdateFilterNodeDirtySlotsAfterPrepare();
            // [attention] make sure filter valid check useful
            surfaceNode->CheckValidFilterCacheFullyCoverTarget(*filterNode, screenRect_);
        }
    }
    auto surfaceDirtyRegion = Occlusion::Region{
        Occlusion::Rect{ surfaceNode->GetDirtyManager()->GetCurrentFrameDirtyRegion() } };
    accumulatedDirtyRegion.OrSelf(surfaceDirtyRegion);
}

void RSUniRenderVisitor::CheckAndUpdateFilterCacheOcclusion(
    std::vector<RSBaseRenderNode::SharedPtr>& curMainAndLeashSurfaces) const
{
    if (!RSSystemParameters::GetFilterCacheOcculusionEnabled()) {
        return;
    }
    bool isScreenOccluded = false;
    // top-down traversal all mainsurface
    // if upper surface reuse filter cache which fully cover whole screen
    // mark lower layers for process skip
    std::for_each(curMainAndLeashSurfaces.begin(), curMainAndLeashSurfaces.end(),
        [this, &isScreenOccluded](RSBaseRenderNode::SharedPtr& nodePtr) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodePtr);
        if (surfaceNode->IsMainWindowType()) {
            // reset occluded status for all mainwindow
            surfaceNode->UpdateOccludedByFilterCache(isScreenOccluded);
        }
        isScreenOccluded = isScreenOccluded || surfaceNode->GetFilterCacheFullyCovered();
    });
}

void RSUniRenderVisitor::CheckMergeGlobalFilterForDisplay(Occlusion::Region& accumulatedDirtyRegion)
{
    // [planning] if not allowed containerNode filter, The following processing logic can be removed
    // Recursively traverses container nodes need filter
    for (auto it = containerFilter_.begin(); it != containerFilter_.end(); ++it) {
        auto filterRegion = Occlusion::Region{ Occlusion::Rect{ *it } };
        auto filterDirtyRegion = filterRegion.And(accumulatedDirtyRegion);
        if (!filterDirtyRegion.IsEmpty()) {
            RS_LOGD("RSUniRenderVisitor::CheckMergeGlobalFilterForDisplay merge "
                "container filterRegion %{public}s", (*it).ToString().c_str());
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(*it);
        } else {
            globalFilter_.insert(*it);
        }
    }

    // Recursively traverses until the globalDirty do not change
    for (auto it = globalFilter_.begin(); it != globalFilter_.end();) {
        auto lastGlobalDirtyRect = curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion();
        auto filterRegion = Occlusion::Region{ Occlusion::Rect{ *it } };
        auto lastGlobalDirtyRegion = Occlusion::Region{ Occlusion::Rect{ lastGlobalDirtyRect } };
        auto filterDirtyRegion = filterRegion.And(lastGlobalDirtyRegion);
        if (!filterDirtyRegion.IsEmpty()) {
            RS_LOGD("RSUniRenderVisitor::CheckMergeGlobalFilterForDisplay merge "
                "filterRegion %{public}s", (*it).ToString().c_str());
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(*it);
            it = globalFilter_.erase(it);
            if (lastGlobalDirtyRect != curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion()) {
                // When DisplayDirtyRegion is changed, collect dirty filter region from begin.
                // After all filter region is added, the cycle will definitely stop.
                it = globalFilter_.begin();
            }
        } else {
            ++it;
        }
    }
}

void RSUniRenderVisitor::PostPrepare(RSRenderNode& node)
{
    if (node.GetRenderProperties().NeedFilter()) {
        UpdateHwcNodeEnableByFilterRect(curSurfaceNode_, node.GetOldDirtyInSurface());
        CollectFilterInfoAndUpdateDirty(node);
    }

    if (auto nodeParent = node.GetParent().lock()) {
        if (node.GetRenderProperties().NeedFilter() || node.ChildHasVisibleFilter()) {
            nodeParent->SetChildHasVisibleFilter(true);
            nodeParent->UpdateVisibleFilterChild(node);
        }
        if (node.GetRenderProperties().GetUseEffect() || node.ChildHasVisibleEffect()) {
            nodeParent->SetChildHasVisibleEffect(true);
            nodeParent->UpdateVisibleEffectChild(node);
        }
    }
    node.MapAndUpdateChildrenRect();
    node.UpdateLocalDrawRect();
    if (isDrawingCacheEnabled_) {
        node.UpdateDrawingCacheInfoAfterChildren();
    }
    if (auto nodeParent = node.GetParent().lock()) {
        nodeParent->UpdateChildUifirstSupportFlag(node.GetUifirstSupportFlag());
    }
    if (node.GetRenderProperties().GetSandBox()) {
        node.GetStagingRenderParams()->SetAlpha(curAlpha_);
    } else {
        node.GetStagingRenderParams()->SetAlpha(node.GetRenderProperties().GetAlpha());
    }

    // TODO: only do this if node is dirty
    node.UpdateRenderParams();

    // add if node is dirty
    node.AddToPendingSyncList();
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByFilterRect(
    std::shared_ptr<RSSurfaceRenderNode>& node, const RectI& filterRect)
{
    if (!node || filterRect.IsEmpty()) {
        return;
    }
    const auto& hwcNodes = node->GetChildHardwareEnabledNodes();
    if (hwcNodes.empty()) {
        return;
    }
    // [planning]: Has opaque control exists between the filter and hwc.
    for (auto hwcNode : hwcNodes) {
        auto hwcNodePtr = hwcNode.lock();
        if (!hwcNodePtr || !hwcNodePtr->GetIntersectByFilterInApp()) {
            continue;
        }
        auto dstRect = hwcNodePtr->GetDstRect();
        bool isIntersect = dstRect.Intersect(filterRect);
        if (isIntersect) {
            hwcNodePtr->SetHardwareForcedDisabledState(true);
        }
    }
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByGlobalFilter(std::shared_ptr<RSSurfaceRenderNode>& node)
{
    auto filterVecIter = transparentCleanFilter_.find(node->GetId());
    if (filterVecIter == transparentCleanFilter_.end()) {
        return;
    }
    auto& curMainAndLeashSurfaces = curDisplayNode_->GetAllMainAndLeashSurfaces();
    for(auto it = curMainAndLeashSurfaces.rbegin(); it != curMainAndLeashSurfaces.rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr) {
            continue;
        }
        if (surfaceNode->GetId() == node->GetId()) {
            return;
        }
        const auto& hwcNodes = surfaceNode->GetChildHardwareEnabledNodes();
        if (hwcNodes.empty()) {
            continue;
        }
        for(auto hwcNode : hwcNodes) {
            auto hwcNodePtr = hwcNode.lock();
            if (!hwcNodePtr || hwcNodePtr->IsHardwareForcedDisabled()) {
                continue;
            }
            for (auto filter = filterVecIter->second.begin(); filter != filterVecIter->second.end(); ++filter) {
                if (hwcNodePtr->GetDstRect().Intersect(filter->second)) {
                    hwcNodePtr->SetHardwareForcedDisabledState(true);
                    break;
                }
            }
        }
    }
}

RectI RSUniRenderVisitor::GetVisibleEffectDirty(RSRenderNode& node) const
{
    RectI childEffectRect = RectI();
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (auto& nodeId : node.GetVisibleEffectChild()) {
        if (auto& subnode = nodeMap.GetRenderNode<RSRenderNode>(nodeId)) {
            childEffectRect = childEffectRect.JoinRect(subnode->GetOldDirtyInSurface());
        }
    }
    return childEffectRect;
}

void RSUniRenderVisitor::CollectFilterInfoAndUpdateDirty(RSRenderNode& node)
{
    auto curDirtyManager = curSurfaceNode_ ? curSurfaceDirtyManager_ : curDisplayDirtyManager_;
    auto globalFilterRect = node.IsInstanceOf<RSEffectRenderNode>() ?
        GetVisibleEffectDirty(node) : node.GetOldDirtyInSurface();
    bool isNodeAddedToTransparentCleanFilters = false;
    if (curSurfaceNode_) {
        bool isIntersect = curDirtyManager->GetCurrentFrameDirtyRegion().Intersect(globalFilterRect);
        if (isIntersect) {
            curDirtyManager->MergeDirtyRect(globalFilterRect);
            UpdateForegroundFilterCacheWithDirty(node, *curDirtyManager, true);
        }
        if (curSurfaceNode_->IsTransparent()) {
            globalFilterRects_.emplace_back(globalFilterRect);
            if (!isIntersect || (isIntersect && node.GetRenderProperties().GetBackgroundFilter() &&
                !node.IsBackgroundInAppOrNodeSelfDirty())) {
                // record nodes which has transparent clean filter
                transparentCleanFilter_[curSurfaceNode_->GetId()].push_back({node.GetId(), globalFilterRect});
                isNodeAddedToTransparentCleanFilters = true;
            }
            if (isIntersect) {
                curDisplayDirtyManager_->MergeDirtyRect(globalFilterRect);
            }
        } else {
            // record surface nodes and nodes in surface which has clean filter
            globalFilter_.insert(globalFilterRect);
        }
    } else {
        globalFilterRects_.emplace_back(globalFilterRect);
        // record container nodes which need filter
        containerFilter_.insert(globalFilterRect);
    }
    if (curSurfaceNode_ && !isNodeAddedToTransparentCleanFilters) {
        node.MarkAndUpdateFilterNodeDirtySlotsAfterPrepare();
    }
}

void RSUniRenderVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    RS_OPTIONAL_TRACE_NAME("RSUniRender::Prepare:[" + node.GetName() + "] nodeid: " +
        std::to_string(node.GetId()) +  " pid: " + std::to_string(ExtractPid(node.GetId())) +
        ", nodeType " + std::to_string(static_cast<uint>(node.GetSurfaceNodeType())));
    if (curDisplayNode_ == nullptr) {
        ROSEN_LOGE("RSUniRenderVisitor::PrepareSurfaceRenderNode, curDisplayNode_ is nullptr.");
        return;
    }
    // avoid EntryView upload texture while screen rotation
    if (node.GetName() == "EntryView") {
        node.SetStaticCached(curDisplayNode_->IsRotationChanged());
    }
    node.UpdatePositionZ();
    if (node.GetName().find(CAPTURE_WINDOW_NAME) != std::string::npos) {
        hasCaptureWindow_[currentVisitDisplay_] = true;
    }

    node.SetAncestorDisplayNode(curDisplayNode_);
    node.UpdateAncestorDisplayNodeInRenderParams();
    CheckColorSpace(node);
    CheckPixelFormat(node);
    // only need collect first level node's security & skip layer info
    if (node.GetId() == node.GetFirstLevelNodeId()) {
        UpdateSecurityAndSkipLayerRecord(node);
    }
    // stop traversal if node keeps static
    if (isQuickSkipPreparationEnabled_ && CheckIfSurfaceRenderNodeStatic(node)) {
        // node type is mainwindow.
        PrepareSubSurfaceNodes(node);
        return;
    }
    // Attension: Updateinfo before info reset
    node.StoreMustRenewedInfo();
    SetHasSharedTransitionNode(node, false);
    node.CleanDstRectChanged();
    if (node.IsHardwareEnabledTopSurface()) {
        node.ResetSubNodeShouldPaint();
        node.ResetChildHardwareEnabledNodes();
    }
    curContentDirty_ = node.IsContentDirty();
    bool dirtyFlag = dirtyFlag_;

    RectI prepareClipRect = prepareClipRect_;
    bool isQuickSkipPreparationEnabled = isQuickSkipPreparationEnabled_;

    // update geoptr with ContextMatrix
    auto parentSurfaceNodeMatrix = parentSurfaceNodeMatrix_;
    auto& property = node.GetMutableRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());
    if (geoPtr == nullptr) {
        return;
    }
    // before node update, prepare node's setting by types
    PrepareTypesOfSurfaceRenderNodeBeforeUpdate(node);

    if (curSurfaceDirtyManager_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::PrepareSurfaceRenderNode %{public}s curSurfaceDirtyManager is nullptr",
            node.GetName().c_str());
        return;
    }
    auto skipNodeMap = RSMainThread::Instance()->GetCacheCmdSkippedNodes();
    if (skipNodeMap.count(node.GetId()) != 0) {
        auto parentNode = node.GetParent().lock();
        auto rsParent = (parentNode);
        dirtyFlag_ = node.Update(*curSurfaceDirtyManager_, rsParent, dirtyFlag_, prepareClipRect_);
        dirtyFlag_ = dirtyFlag;
        RS_TRACE_NAME(node.GetName() + " PreparedNodes cacheCmdSkiped");
        return;
    }

#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (autoCacheEnable_ && node.IsappWindow()) {
        node->GetAutoCache()->OpincSetInAppStateStart(unchangeMarkEnable_, unchangeMark_, isDiscardSurface_);
    }
#endif
    // Update node properties, including position (dstrect), OldDirty()
    auto parentNode = node.GetParent().lock();
    auto rsParent = (parentNode);
    auto rsSurfaceParent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(parentNode);
    if (node.IsAppWindow() && rsSurfaceParent && rsSurfaceParent->IsLeashWindow()
        && rsSurfaceParent->GetDstRect().IsEmpty()) {
            prepareClipRect_ = RectI {0, 0, 0, 0};
    }
    dirtyFlag_ = node.Update(*curSurfaceDirtyManager_, rsParent, dirtyFlag_, prepareClipRect_);

    // Calculate the absolute destination rectangle of the node, initialize with absolute bounds rect
    auto dstRect = geoPtr->GetAbsRect();
    // If the screen is expanded, intersect the destination rectangle with the screen rectangle
    dstRect = dstRect.IntersectRect(RectI(curDisplayNode_->GetDisplayOffsetX(), curDisplayNode_->GetDisplayOffsetY(),
        screenInfo_.width, screenInfo_.height));
    // Remove the offset of the screen
    dstRect = RectI(dstRect.left_ - curDisplayNode_->GetDisplayOffsetX(),
        dstRect.top_ - curDisplayNode_->GetDisplayOffsetY(), dstRect.GetWidth(), dstRect.GetHeight());
    // If the node is a hardware-enabled type, intersect its destination rectangle with the prepare clip rectangle
    if (node.IsHardwareEnabledType()) {
        dstRect = dstRect.IntersectRect(prepareClipRect_);
    }
    // Set the destination rectangle of the node
    node.SetDstRect(dstRect);

    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        // record visible node position for display render node dirtyManager
        if (node.ShouldPaint()) {
            curDisplayNode_->UpdateSurfaceNodePos(node.GetId(), node.GetOldDirty());
        }

        if (node.IsAppWindow()) {
            // if update appwindow, its children should not skip
            localZOrder_ = 0.0f;
            isQuickSkipPreparationEnabled_ = false;
            if (isSubSurfaceEnabled_) {
                isQuickSkipPreparationEnabled_ = true;
            }
            node.ResetChildHardwareEnabledNodes();
            boundsRect_ = Drawing::Rect(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
            frameGravity_ = property.GetFrameGravity();
        }
    }

    // [planning] Remove this after skia is upgraded, the clipRegion is supported
    // reset childrenFilterRects
    node.ResetChildrenFilterRects();

    dirtyFlag_ = dirtyFlag_ || node.GetDstRectChanged();
    parentSurfaceNodeMatrix_ = geoPtr->GetAbsMatrix();
    if (RSUniRenderUtil::GetRotationDegreeFromMatrix(parentSurfaceNodeMatrix_) % ROTATION_90 != 0) {
        isSurfaceRotationChanged_ = true;
        doAnimate_ = true;
        node.SetAnimateState();
    }

    bool isSubNodeOfSurfaceInPrepare = isSubNodeOfSurfaceInPrepare_;
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        isSubNodeOfSurfaceInPrepare_ = true;
    }
    node.UpdateChildrenOutOfRectFlag(false);
    // [planning] ShouldPrepareSubnodes would be applied again if condition constraint ensures
    PrepareChildren(node);
    node.UpdateParentChildrenRect(rsParent);

    // restore flags
    parentSurfaceNodeMatrix_ = parentSurfaceNodeMatrix;
    dirtyFlag_ = dirtyFlag;
    isQuickSkipPreparationEnabled_ = isQuickSkipPreparationEnabled;
    prepareClipRect_ = prepareClipRect;
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        isSubNodeOfSurfaceInPrepare_ = isSubNodeOfSurfaceInPrepare;
    }

    PrepareTypesOfSurfaceRenderNodeAfterUpdate(node);
    if (node.GetDstRectChanged() || (node.GetDirtyManager() && node.GetDirtyManager()->IsCurrentFrameDirty())) {
        dirtySurfaceNodeMap_.emplace(node.GetId(), node.ReinterpretCastTo<RSSurfaceRenderNode>());
    }
    UpdateSurfaceRenderNodeScale(node);
    // Due to the alpha is updated in PrepareChildren, so PrepareChildren
    // needs to be done before CheckOpaqueRegionBaseInfo
    auto screenRotation = curDisplayNode_->GetRotation();
    auto screenRect = RectI(0, 0, screenInfo_.width, screenInfo_.height);
    Vector4f cornerRadius;
    Vector4f::Max(node.GetWindowCornerRadius(), node.GetGlobalCornerRadius(), cornerRadius);
    Vector4<int> dstCornerRadius(static_cast<int>(std::ceil(cornerRadius.x_)),
                                 static_cast<int>(std::ceil(cornerRadius.y_)),
                                 static_cast<int>(std::ceil(cornerRadius.z_)),
                                 static_cast<int>(std::ceil(cornerRadius.w_)));
    auto parent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.GetParent().lock());
    auto isFocused = node.IsFocusedNode(currentFocusedNodeId_) ||
        (parent && parent->IsLeashWindow() && parent->IsFocusedNode(focusedLeashWindowId_));
    if (!node.CheckOpaqueRegionBaseInfo(
        screenRect, geoPtr->GetAbsRect(), screenRotation, isFocused, dstCornerRadius)
        && node.GetSurfaceNodeType() != RSSurfaceNodeType::SELF_DRAWING_NODE) {
        node.ResetSurfaceOpaqueRegion(screenRect, geoPtr->GetAbsRect(), screenRotation, isFocused, dstCornerRadius);
    }
    node.SetOpaqueRegionBaseInfo(screenRect, geoPtr->GetAbsRect(), screenRotation, isFocused, dstCornerRadius);
#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (autoCacheEnable_ && node.IsappWindow()) {
        node->GetAutoCache()->OpincSetInAppStateEnd(unchangeMarkEnable_, unchangeMark_, isDiscardSurface_);
    }
#endif
    if (node.IsMainWindowType()) {
        // Attention: curSurface info would be reset as upper surfaceParent if it has
        ResetCurSurfaceInfoAsUpperSurfaceParent(node);
    }
}

void RSUniRenderVisitor::UpdateSurfaceRenderNodeScale(RSSurfaceRenderNode& node)
{
    if (!node.IsLeashWindow()) {
        return;
    }
    auto& property = node.GetMutableRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());
    if (geoPtr == nullptr) {
        return;
    }
    auto absMatrix = geoPtr->GetAbsMatrix();
    bool isScale = false;
    if (RSMainThread::Instance()->GetDeviceType() == DeviceType::PC) {
        isScale = (!ROSEN_EQ(absMatrix.Get(Drawing::Matrix::SCALE_X), 1.f, EPSILON_SCALE) ||
            !ROSEN_EQ(absMatrix.Get(Drawing::Matrix::SCALE_Y), 1.f, EPSILON_SCALE));
    } else {
        bool getMinMaxScales = false;
        // scaleFactors[0]-minimum scaling factor, scaleFactors[1]-maximum scaling factor
        Drawing::scalar scaleFactors[2];
        getMinMaxScales = absMatrix.GetMinMaxScales(scaleFactors);
        if (getMinMaxScales) {
            isScale = !ROSEN_EQ(scaleFactors[0], 1.f, EPSILON_SCALE) || !ROSEN_EQ(scaleFactors[1], 1.f, EPSILON_SCALE);
        }
        if (!getMinMaxScales) {
            RS_LOGD("getMinMaxScales fail, node:%{public}s %{public}" PRIu64 "", node.GetName().c_str(), node.GetId());
            auto dstRect = node.GetDstRect();
            float dstRectWidth = dstRect.GetWidth();
            float dstRectHeight = dstRect.GetHeight();
            float boundsWidth = property.GetBoundsWidth();
            float boundsHeight = property.GetBoundsHeight();
            isScale =
                !ROSEN_EQ(std::min(dstRectWidth, dstRectHeight), std::min(boundsWidth, boundsHeight), EPSILON_SCALE)
                || !ROSEN_EQ(std::max(dstRectWidth, dstRectHeight), std::max(boundsWidth, boundsHeight), EPSILON_SCALE);
        }
    }
    node.SetIsScale(isScale);
}

void RSUniRenderVisitor::PrepareProxyRenderNode(RSProxyRenderNode& node)
{
#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (autoCacheEnable_) {
        node->GetAutoCache()->DisableAutoCacheWithForce(unchangeMark_);
    }
#endif
    // alpha is not affected by dirty flag, always update
    node.SetContextAlpha(curAlpha_);
    // skip matrix & clipRegion update if not dirty
    if (!dirtyFlag_) {
        return;
    }
    auto rsParent = (node.GetParent().lock());
    if (rsParent == nullptr) {
        return;
    }
    auto& property = rsParent->GetMutableRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());

    // Context matrix should be relative to the parent surface node, so we need to revert the parentSurfaceNodeMatrix_.
    Drawing::Matrix invertMatrix;
    Drawing::Matrix contextMatrix = geoPtr->GetAbsMatrix();

    if (parentSurfaceNodeMatrix_.Invert(invertMatrix)) {
        contextMatrix.PreConcat(invertMatrix);
    } else {
        ROSEN_LOGE("RSUniRenderVisitor::PrepareProxyRenderNode, invert parentSurfaceNodeMatrix_ failed");
    }
    node.SetContextMatrix(contextMatrix);

    // For now, we only set the clipRegion if the parent node has ClipToBounds set to true.
    if (!property.GetClipToBounds()) {
        node.SetContextClipRegion(std::nullopt);
    } else {
        // Maybe we should use prepareClipRect_ and make the clipRegion in device coordinate, but it will be more
        // complex to calculate the intersect, and it will make app developers confused.
        auto rect = property.GetBoundsRect();
        // Context clip region is in the parent node coordinate, so we don't need to map it.
        node.SetContextClipRegion(Drawing::Rect(
            rect.GetLeft(), rect.GetTop(), rect.GetWidth() + rect.GetLeft(), rect.GetHeight() + rect.GetTop()));
    }

    // prepare children
    PrepareChildren(node);
}

void RSUniRenderVisitor::PrepareRootRenderNode(RSRootRenderNode& node)
{
    RS_TRACE_NAME_FMT("RSUniRender::PrepareRootRenderNode:node[%llu] pid[%d] subTreeDirty[%d]",
        node.GetId(), ExtractPid(node.GetId()), node.IsSubTreeDirty());
    bool dirtyFlag = dirtyFlag_;
    auto parentSurfaceNodeMatrix = parentSurfaceNodeMatrix_;
    auto prepareClipRect = prepareClipRect_;

    auto nodeParent = (node.GetParent().lock());
    const auto& property = node.GetRenderProperties();
    bool geoDirty = property.IsGeoDirty();
    auto geoPtr = (property.GetBoundsGeometry());
    auto prevAlpha = curAlpha_;
    curAlpha_ *= std::clamp(node.GetRenderProperties().GetAlpha(), 0.f, 1.f);

    if (curSurfaceDirtyManager_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::PrepareRootRenderNode curSurfaceDirtyManager is nullptr");
        return;
    }

    if (RSSystemProperties::GetQuickPrepareEnabled()) {
        dirtyFlag_ = node.UpdateDrawRectAndDirtyRegion(
            *curSurfaceDirtyManager_, nodeParent, dirtyFlag_, prepareClipRect_);
    } else {
        dirtyFlag_ = node.Update(*curSurfaceDirtyManager_, nodeParent, dirtyFlag_, prepareClipRect_);
    }

    if (nodeParent == curSurfaceNode_) {
        const float rootWidth = property.GetFrameWidth() * property.GetScaleX();
        const float rootHeight = property.GetFrameHeight() * property.GetScaleY();
        Drawing::Matrix gravityMatrix;
        (void)RSPropertiesPainter::GetGravityMatrix(frameGravity_,
            RectF { 0.0f, 0.0f, boundsRect_.GetWidth(), boundsRect_.GetHeight() },
            rootWidth, rootHeight, gravityMatrix);
        // Only Apply gravityMatrix when rootNode is dirty
        if (geoPtr != nullptr && (dirtyFlag || geoDirty)) {
            geoPtr->ConcatMatrix(gravityMatrix);
        }
    }

    if (geoPtr != nullptr) {
        parentSurfaceNodeMatrix_ = geoPtr->GetAbsMatrix();
    }

    if (RSSystemProperties::GetQuickPrepareEnabled()) {
        bool IsSubTreeNeedPrepare = node.IsSubTreeNeedPrepare(filterInGlobal_) || ForcePrepareSubTree();
        IsSubTreeNeedPrepare ? QuickPrepareChildren(node) :
            node.SubTreeSkipPrepare(*curSurfaceDirtyManager_, curDirty_, dirtyFlag_);
        PostPrepare(node);
    } else {
        node.UpdateChildrenOutOfRectFlag(false);
        PrepareChildren(node);
        node.UpdateParentChildrenRect(nodeParent);
    }

    curAlpha_ = prevAlpha;
    parentSurfaceNodeMatrix_ = parentSurfaceNodeMatrix;
    dirtyFlag_ = dirtyFlag;
    prepareClipRect_ = prepareClipRect;
}

void RSUniRenderVisitor::PrepareCanvasRenderNode(RSCanvasRenderNode &node)
{
    preparedCanvasNodeInCurrentSurface_++;
    curContentDirty_ = node.IsContentDirty();
    bool dirtyFlag = dirtyFlag_;
    RectI prepareClipRect = prepareClipRect_;

    auto nodeParent = node.GetParent().lock();
    while (nodeParent && nodeParent->ReinterpretCastTo<RSSurfaceRenderNode>() &&
        nodeParent->ReinterpretCastTo<RSSurfaceRenderNode>()->GetSurfaceNodeType() ==
        RSSurfaceNodeType::SELF_DRAWING_NODE) {
        nodeParent = nodeParent->GetParent().lock();
    }
    if (nodeParent) {
        node.SetIsAncestorDirty(nodeParent->IsDirty() || nodeParent->IsAncestorDirty());
        auto parentSurfaceNode = nodeParent->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (parentSurfaceNode && parentSurfaceNode->IsLeashWindow()) {
            node.SetParentLeashWindow();
        }
        if (parentSurfaceNode && parentSurfaceNode->IsScbScreen()) {
            node.SetParentScbScreen();
        }
    }
    if (curSurfaceDirtyManager_ == nullptr && curDisplayDirtyManager_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::PrepareCanvasRenderNode curXDirtyManager is nullptr");
        return;
    }
    if (node.GetSharedTransitionParam()) {
        node.GetMutableRenderProperties().UpdateSandBoxMatrix(parentSurfaceNodeMatrix_);
    }
    if (isSubNodeOfSurfaceInPrepare_ && curSurfaceNode_ &&
        curSurfaceNode_->IsHardwareEnabledTopSurface() && node.ShouldPaint()) {
        curSurfaceNode_->SetSubNodeShouldPaint();
    }
#ifdef DDGR_ENABLE_FEATURE_OPINC
    bool isPrepareAutoCache = autoCacheEnable_ && !isDiscardSurface_ && unchangeMarkEnable_;
    if (isPrepareAutoCache) {
        node->GetAutoCache()->OpincStartMarkStableNode(unchangeMark_, false);
    }
#endif
    // if canvasNode is not sub node of surfaceNode, merge the dirtyRegion to curDisplayDirtyManager_
    auto dirtyManager = isSubNodeOfSurfaceInPrepare_ ? curSurfaceDirtyManager_ : curDisplayDirtyManager_;
    dirtyFlag_ = node.Update(*dirtyManager, nodeParent, dirtyFlag_, prepareClipRect_);

    const auto& property = node.GetRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());
    if (geoPtr == nullptr) {
        return;
    }
    // Dirty Region use abstract coordinate, property of node use relative coordinate
    // BoundsRect(if exists) is mapped to absRect_ of RSObjAbsGeometry.
    if (property.GetClipToBounds()) {
        prepareClipRect_ = prepareClipRect_.IntersectRect(geoPtr->GetAbsRect());
    }
    // FrameRect(if exists) is mapped to rect using abstract coordinate explicitly by calling MapAbsRect.
    if (property.GetClipToFrame()) {
        // MapAbsRect do not handle the translation of OffsetX and OffsetY
        RectF frameRect{
            property.GetFrameOffsetX() * geoPtr->GetAbsMatrix().Get(Drawing::Matrix::SCALE_X),
            property.GetFrameOffsetY() * geoPtr->GetAbsMatrix().Get(Drawing::Matrix::SCALE_Y),
            property.GetFrameWidth(), property.GetFrameHeight()};
        prepareClipRect_ = prepareClipRect_.IntersectRect(geoPtr->MapAbsRect(frameRect));
    }

    node.UpdateChildrenOutOfRectFlag(false);

    if (isSkipCanvasNodeOutOfScreen_ && !isSubNodeOfSurfaceInPrepare_ && !node.HasSubSurface() &&
        IsOutOfScreenRegion(geoPtr->GetAbsRect())) {
        return;
    }

    PrepareChildren(node);
    // attention: accumulate direct parent's childrenRect
    node.UpdateParentChildrenRect(nodeParent);
    if (property.GetUseEffect()) {
        if (auto directParent = node.GetParent().lock()) {
            directParent->SetChildHasVisibleEffect(true);
        }
    }

    node.UpdateEffectRegion(effectRegion_);
    if (property.NeedFilter()) {
        // filterRects_ is used in RSUniRenderVisitor::CalcDirtyFilterRegion
        // When oldDirtyRect of node with filter has intersect with any surfaceNode or displayNode dirtyRegion,
        // the whole oldDirtyRect should be render in this vsync.
        // Partial rendering of node with filter would cause display problem.
        if (auto directParent = node.GetParent().lock()) {
            directParent->SetChildHasVisibleFilter(true);
        }
        if (curSurfaceDirtyManager_->IsTargetForDfx()) {
            curSurfaceDirtyManager_->UpdateDirtyRegionInfoForDfx(node.GetId(), RSRenderNodeType::CANVAS_NODE,
                DirtyRegionType::FILTER_RECT, node.GetOldDirtyInSurface());
        }
        if (curSurfaceNode_) {
            curSurfaceNode_->UpdateChildrenFilterRects(node.shared_from_this(), node.GetOldDirtyInSurface(),
                node.IsBackgroundFilterCacheValid());
            curSurfaceNode_->UpdateFilterNodes(node.shared_from_this());
        }
        UpdateForegroundFilterCacheWithDirty(node, *dirtyManager);
    }
    dirtyFlag_ = dirtyFlag;
    prepareClipRect_ = prepareClipRect;
#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (isPrepareAutoCache) {
        node->GetAutoCache()->OpincEndMarkStableNode(unchangeMark_);
    }
#endif
#if defined(RS_ENABLE_DRIVEN_RENDER)
    // skip content node and its children, calculate dirty contain background and foreground
    if (drivenInfo_ && isContentCanvasNode) {
        drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty = isBeforeContentNodeDirty;
        drivenInfo_->drivenUniTreePrepareMode = DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_AFTER;
    }
#endif

#ifdef RS_ENABLE_STACK_CULLING
    if (RSSystemProperties::GetViewOcclusionCullingEnabled()) {
        node.SetFullSurfaceOpaqueMarks(curSurfaceNode_);
    }
#endif
}

void RSUniRenderVisitor::PrepareEffectRenderNode(RSEffectRenderNode& node)
{
#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (autoCacheEnable_) {
        node->GetAutoCache()->DisableAutoCacheWithForce(unchangeMark_);
    }
#endif
    bool dirtyFlag = dirtyFlag_;
    RectI prepareClipRect = prepareClipRect_;
    auto effectRegion = effectRegion_;
    effectRegion_ = node.InitializeEffectRegion();

    // set rotationChanged true when screen is rotating or folding/expanding screen.
    if (curDisplayNode_->IsRotationChanged() || (!curDisplayNode_->IsRotationChanged() && doAnimate_)) {
        node.SetRotationChanged(true);
        int invalidateTimes = 2; // node call invalidate cache 3 times in one frame.
        node.SetInvalidateTimesForRotation(invalidateTimes);
    } else {
        node.SetRotationChanged(false);
    }
    auto parentNode = node.GetParent().lock();
    node.SetVisitedFilterCacheStatus(curSurfaceDirtyManager_->IsCacheableFilterRectEmpty());
    dirtyFlag_ = node.Update(*curSurfaceDirtyManager_, parentNode, dirtyFlag_, prepareClipRect_);

    node.UpdateChildrenOutOfRectFlag(false);
    PrepareChildren(node);
    node.UpdateParentChildrenRect(node.GetParent().lock());
    node.SetEffectRegion(effectRegion_);

    if (node.GetRenderProperties().NeedFilter()) {
        // filterRects_ is used in RSUniRenderVisitor::CalcDirtyFilterRegion
        // When oldDirtyRect of node with filter has intersect with any surfaceNode or displayNode dirtyRegion,
        // the whole oldDirtyRect should be render in this vsync.
        // Partial rendering of node with filter would cause display problem.
        if (parentNode) {
            parentNode->SetChildHasVisibleFilter(true);
        }
        if (curSurfaceDirtyManager_->IsTargetForDfx()) {
            curSurfaceDirtyManager_->UpdateDirtyRegionInfoForDfx(node.GetId(), RSRenderNodeType::CANVAS_NODE,
                DirtyRegionType::FILTER_RECT, node.GetOldDirtyInSurface());
        }
        if (curSurfaceNode_ && effectRegion_.has_value() && !effectRegion_->IsEmpty()) {
            RectI filterRect(effectRegion_->GetLeft(), effectRegion_->GetTop(),
                effectRegion_->GetWidth(), effectRegion_->GetHeight());
            curSurfaceNode_->UpdateChildrenFilterRects(node.shared_from_this(), filterRect,
                node.IsBackgroundFilterCacheValid());
            curSurfaceNode_->UpdateFilterNodes(node.shared_from_this());
        }
        UpdateForegroundFilterCacheWithDirty(node, *curSurfaceDirtyManager_);
    }

    effectRegion_ = effectRegion;
    dirtyFlag_ = dirtyFlag;
    prepareClipRect_ = prepareClipRect;
}

void RSUniRenderVisitor::DrawDirtyRectForDFX(const RectI& dirtyRect, const Drawing::Color color,
    const RSPaintStyle fillType, float alpha, int edgeWidth = 6, std::string extra)
{
    if (dirtyRect.width_ <= 0 || dirtyRect.height_ <= 0) {
        ROSEN_LOGD("DrawDirtyRectForDFX dirty rect is invalid.");
        return;
    }
    ROSEN_LOGD("DrawDirtyRectForDFX current dirtyRect = %{public}s", dirtyRect.ToString().c_str());
    auto rect = Drawing::Rect(dirtyRect.left_, dirtyRect.top_,
        dirtyRect.left_ + dirtyRect.width_, dirtyRect.top_ + dirtyRect.height_);
#ifdef DDGR_ENABLE_FEATURE_OPINC
    std::string position = ""
    if (autoCacheDrawingEnable_) {
        position = extra;
    } else {
        position = std::to_string(dirtyRect.left_) + ',' + std::to_string(dirtyRect.top_) + ',' +
        std::to_string(dirtyRect.width_) + ',' + std::to_string(dirtyRect.height_) + extra;
    }
#else
    std::string position = std::to_string(dirtyRect.left_) + ',' + std::to_string(dirtyRect.top_) + ',' +
        std::to_string(dirtyRect.width_) + ',' + std::to_string(dirtyRect.height_) + extra;
#endif
    const int defaultTextOffsetX = edgeWidth;
    const int defaultTextOffsetY = 30; // text position has 30 pixelSize under the Rect
    Drawing::Pen rectPen;
    Drawing::Brush rectBrush;
    std::shared_ptr<Drawing::Typeface> typeFace = nullptr;
#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (autoCacheDrawingEnable_) {
        auto fontMgr = Drawing::FontMgr::CreateDefaultFontMgr();
        auto fStyleSet = fontMgr->MatchFamily("HarmonyOS Sans SC");
        if (fStyleSet->Count() <= 0) {
            return;
        }
        typeFace = std::shared_ptr<Drawing::Typeface>(fStyleSet->CreateTypeface(0));
    }
#endif
    // font size: 24
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromString(position.c_str(), Drawing::Font(typeFace, 24.0f, 1.0f, 0.0f));
    if (fillType == RSPaintStyle::STROKE) {
        rectPen.SetColor(color);
        rectPen.SetAntiAlias(true);
        rectPen.SetAlphaF(alpha);
        rectPen.SetWidth(edgeWidth);
        rectPen.SetJoinStyle(Drawing::Pen::JoinStyle::ROUND_JOIN);
        canvas_->AttachPen(rectPen);
    } else {
        rectBrush.SetColor(color);
        rectBrush.SetAntiAlias(true);
        rectBrush.SetAlphaF(alpha);
        canvas_->AttachBrush(rectBrush);
    }
    canvas_->DrawRect(rect);
    canvas_->DetachPen();
    canvas_->DetachBrush();
    canvas_->AttachBrush(Drawing::Brush());
    canvas_->DrawTextBlob(textBlob.get(), dirtyRect.left_ + defaultTextOffsetX, dirtyRect.top_ + defaultTextOffsetY);
    canvas_->DetachBrush();
}

void RSUniRenderVisitor::DrawDirtyRegionForDFX(std::vector<RectI> dirtyRects)
{
    const float fillAlpha = 0.2;
    for (const auto& subRect : dirtyRects) {
        DrawDirtyRectForDFX(subRect, Drawing::Color::COLOR_BLUE, RSPaintStyle::STROKE, fillAlpha);
    }
}

void RSUniRenderVisitor::DrawCacheRegionForDFX(std::vector<RectI> cacheRects)
{
    const float fillAlpha = 0.2;
    for (const auto& subRect : cacheRects) {
        DrawDirtyRectForDFX(subRect, Drawing::Color::COLOR_BLUE, RSPaintStyle::FILL, fillAlpha);
    }
}

void RSUniRenderVisitor::DrawHwcRegionForDFX(std::vector<std::shared_ptr<RSSurfaceRenderNode>>& hwcNodes)
{
    const float fillAlpha = 0.3f;
    for (const auto& node : hwcNodes) {
        if (node->IsHardwareForcedDisabled() || !IsHardwareComposerEnabled()) {
            RSUniRenderUtil::DrawRectForDfx(*canvas_, node->GetDstRect(), Drawing::Color::COLOR_RED, fillAlpha,
                node->GetName());
        } else {
            RSUniRenderUtil::DrawRectForDfx(*canvas_, node->GetDstRect(), Drawing::Color::COLOR_BLUE, fillAlpha,
                node->GetName());
        }
    }
}

#ifdef DDGR_ENABLE_FEATURE_OPINC
constexpr uint32_t DRAW_DFX_EDGE_WIDTH = 6;
void RSUniRenderVisitor::DrawAutoCacheRegionForDFX(std::vector<RectI, std::string> cacheRegionInfo)
{
    const float fillAlpha = 0.2;
    for (const auto& info : cacheRegionInfo) {
        DrawDirtyRectForDFX(info.first, Drawing::Color::COLOR_BLUE, RSPaintStyle::FILL, fillAlpha,
            DRAW_DFX_EDGE_WIDTH, info.second);
    }
}
#endif

void RSUniRenderVisitor::DrawAllSurfaceDirtyRegionForDFX(RSDisplayRenderNode& node, const Occlusion::Region& region)
{
    const auto& visibleDirtyRects = region.GetRegionRects();
    std::vector<RectI> rects;
    for (auto& rect : visibleDirtyRects) {
        rects.emplace_back(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_);
    }
    DrawDirtyRegionForDFX(rects);

    // draw display dirtyregion with red color
    RectI dirtySurfaceRect = node.GetDirtyManager()->GetDirtyRegion();
    const float fillAlpha = 0.2;
    DrawDirtyRectForDFX(dirtySurfaceRect, Drawing::Color::COLOR_RED, RSPaintStyle::STROKE, fillAlpha);
}

void RSUniRenderVisitor::DrawAllSurfaceOpaqueRegionForDFX(RSDisplayRenderNode& node)
{
    for (auto& it : node.GetCurAllSurfaces()) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(it);
        if (surfaceNode->IsMainWindowType()) {
            DrawSurfaceOpaqueRegionForDFX(*surfaceNode);
        }
    }
}

void RSUniRenderVisitor::DrawTargetSurfaceDirtyRegionForDFX(RSDisplayRenderNode& node)
{
    for (auto it = node.GetCurAllSurfaces().rbegin(); it != node.GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        if (CheckIfSurfaceTargetedForDFX(surfaceNode->GetName())) {
            if (DrawDetailedTypesOfDirtyRegionForDFX(*surfaceNode)) {
                continue;
            }
            const auto& visibleDirtyRegions = surfaceNode->GetVisibleDirtyRegion().GetRegionRects();
            std::vector<RectI> rects;
            for (auto& rect : visibleDirtyRegions) {
                rects.emplace_back(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_);
            }
            const auto& visibleRegions = surfaceNode->GetVisibleRegion().GetRegionRects();
            auto displayDirtyRegion = node.GetDirtyManager()->GetDirtyRegion();
            for (auto& rect : visibleRegions) {
                auto visibleRect = RectI(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_);
                auto intersectRegion = displayDirtyRegion.IntersectRect(visibleRect);
                rects.emplace_back(intersectRegion);
            }
            DrawDirtyRegionForDFX(rects);
        }
    }
}

void RSUniRenderVisitor::DrawTargetSurfaceVisibleRegionForDFX(RSDisplayRenderNode& node)
{
    for (auto it = node.GetCurAllSurfaces().rbegin(); it != node.GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        if (CheckIfSurfaceTargetedForDFX(surfaceNode->GetName())) {
            const auto& visibleRegions = surfaceNode->GetVisibleRegion().GetRegionRects();
            std::vector<RectI> rects;
            for (auto& rect : visibleRegions) {
                rects.emplace_back(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_);
            }
            DrawDirtyRegionForDFX(rects);
        }
    }
}

void RSUniRenderVisitor::DrawEffectRenderNodeForDFX()
{
    if (effectNodeMapForDfx_.empty()) {
        return;
    }
    const float strokeAlpha = 0.8;
    const int strokeWidth = 12;
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (const auto& effectNode : effectNodeMapForDfx_) {
        auto node = nodeMap.GetRenderNode<RSRenderNode>(effectNode.first);
        if (!node) {
            continue;
        }
        auto geoPtr = node->GetRenderProperties().GetBoundsGeometry();
        if (geoPtr == nullptr) {
            continue;
        }
        RectI absRect = geoPtr->GetAbsRect();
        // draw effectNode
        DrawDirtyRectForDFX(absRect, Drawing::Color::COLOR_BLUE, RSPaintStyle::STROKE, strokeAlpha, strokeWidth);
        // draw useEffect nodes
        const auto& useEffectRects = effectNode.second.first;
        for (const auto& rect : useEffectRects) {
            DrawDirtyRectForDFX(rect, Drawing::Color::COLOR_GREEN, RSPaintStyle::STROKE, strokeAlpha, strokeWidth);
        }

        // draw fallback nodes
        const auto fallbackRects = effectNode.second.second;
        for (const auto& rect : fallbackRects) {
            DrawDirtyRectForDFX(rect, Drawing::Color::COLOR_RED, RSPaintStyle::STROKE, strokeAlpha, strokeWidth);
        }
    }
}

void RSUniRenderVisitor::DrawCurrentRefreshRate(uint32_t currentRefreshRate, uint32_t realtimeRefreshRate)
{
    std::string info = std::to_string(currentRefreshRate) + " " + std::to_string(realtimeRefreshRate);
    auto color = currentRefreshRate <= 60 ? SK_ColorRED : SK_ColorGREEN;
    std::shared_ptr<Drawing::Typeface> tf = Drawing::Typeface::MakeFromName("HarmonyOS Sans SC", Drawing::FontStyle());
    Drawing::Font font;
    font.SetSize(100);  // 100:Scalar of setting font size
    font.SetTypeface(tf);
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromString(info.c_str(), font);

    Drawing::Brush brush;
    brush.SetColor(color);
    brush.SetAntiAlias(true);
    canvas_->AttachBrush(brush);
    canvas_->DrawTextBlob(
        textBlob.get(), 100.f, 200.f);  // 100.f:Scalar x of drawing TextBlob; 200.f:Scalar y of drawing TextBlob
    canvas_->DetachBrush();
}

void RSUniRenderVisitor::DrawAndTraceSingleDirtyRegionTypeForDFX(RSSurfaceRenderNode& node,
    DirtyRegionType dirtyType, bool isDrawn)
{
    auto dirtyManager = node.GetDirtyManager();
    auto matchType = DIRTY_REGION_TYPE_MAP.find(dirtyType);
    if (dirtyManager == nullptr ||  matchType == DIRTY_REGION_TYPE_MAP.end()) {
        return;
    }
    std::map<NodeId, RectI> dirtyInfo;
    float fillAlpha = 0.2;
    std::map<RSRenderNodeType, std::pair<std::string, SkColor>> nodeConfig = {
        {RSRenderNodeType::CANVAS_NODE, std::make_pair("canvas", SK_ColorRED)},
        {RSRenderNodeType::SURFACE_NODE, std::make_pair("surface", SK_ColorGREEN)},
    };

    std::string subInfo;
    for (const auto& [nodeType, info] : nodeConfig) {
        dirtyManager->GetDirtyRegionInfo(dirtyInfo, nodeType, dirtyType);
        subInfo += (" " + info.first + "node amount: " + std::to_string(dirtyInfo.size()));
        for (const auto& [nid, rect] : dirtyInfo) {
            if (isDrawn) {
                DrawDirtyRectForDFX(rect, info.second, RSPaintStyle::STROKE, fillAlpha);
            }
        }
    }
    RS_TRACE_NAME("DrawAndTraceSingleDirtyRegionTypeForDFX target surface node " + node.GetName() + " - id[" +
        std::to_string(node.GetId()) + "] has dirtytype " + matchType->second + subInfo);
    ROSEN_LOGD("DrawAndTraceSingleDirtyRegionTypeForDFX target surface node %{public}s, id[%{public}" PRIu64 "]"
        "has dirtytype %{public}s%{public}s",
        node.GetName().c_str(), node.GetId(), matchType->second.c_str(), subInfo.c_str());
}

bool RSUniRenderVisitor::DrawDetailedTypesOfDirtyRegionForDFX(RSSurfaceRenderNode& node)
{
    if (dirtyRegionDebugType_ < DirtyRegionDebugType::CUR_DIRTY_DETAIL_ONLY_TRACE) {
        return false;
    }
    if (dirtyRegionDebugType_ == DirtyRegionDebugType::CUR_DIRTY_DETAIL_ONLY_TRACE) {
        auto i = DirtyRegionType::UPDATE_DIRTY_REGION;
        for (; i < DirtyRegionType::TYPE_AMOUNT; i = (DirtyRegionType)(i + 1)) {
            DrawAndTraceSingleDirtyRegionTypeForDFX(node, i, false);
        }
        return true;
    }
    const std::map<DirtyRegionDebugType, DirtyRegionType> DIRTY_REGION_DEBUG_TYPE_MAP {
        { DirtyRegionDebugType::UPDATE_DIRTY_REGION, DirtyRegionType::UPDATE_DIRTY_REGION },
        { DirtyRegionDebugType::OVERLAY_RECT, DirtyRegionType::OVERLAY_RECT },
        { DirtyRegionDebugType::FILTER_RECT, DirtyRegionType::FILTER_RECT },
        { DirtyRegionDebugType::SHADOW_RECT, DirtyRegionType::SHADOW_RECT },
        { DirtyRegionDebugType::PREPARE_CLIP_RECT, DirtyRegionType::PREPARE_CLIP_RECT },
        { DirtyRegionDebugType::REMOVE_CHILD_RECT, DirtyRegionType::REMOVE_CHILD_RECT },
        { DirtyRegionDebugType::RENDER_PROPERTIES_RECT, DirtyRegionType::RENDER_PROPERTIES_RECT },
        { DirtyRegionDebugType::CANVAS_NODE_SKIP_RECT, DirtyRegionType::CANVAS_NODE_SKIP_RECT },
        { DirtyRegionDebugType::OUTLINE_RECT, DirtyRegionType::OUTLINE_RECT },
    };
    auto matchType = DIRTY_REGION_DEBUG_TYPE_MAP.find(dirtyRegionDebugType_);
    if (matchType != DIRTY_REGION_DEBUG_TYPE_MAP.end()) {
        DrawAndTraceSingleDirtyRegionTypeForDFX(node, matchType->second);
    }
    return true;
}

void RSUniRenderVisitor::DrawSurfaceOpaqueRegionForDFX(RSSurfaceRenderNode& node)
{
    const auto& opaqueRegionRects = node.GetOpaqueRegion().GetRegionRects();
    for (const auto &subRect: opaqueRegionRects) {
        DrawDirtyRectForDFX(subRect.ToRectI(), Drawing::Color::COLOR_GREEN,
            RSPaintStyle::FILL, 0.2f, 0);
    }
}

void RSUniRenderVisitor::ProcessShadowFirst(RSRenderNode& node, bool inSubThread)
{
    if (node.GetRenderProperties().GetUseShadowBatching()) {
        auto children = node.GetSortedChildren();
        for (auto& child : *children) {
            if (auto node = child->ReinterpretCastTo<RSCanvasRenderNode>()) {
                node->ProcessShadowBatching(*canvas_);
            }
        }
    }
}

void RSUniRenderVisitor::CheckSkipRepeatShadow(RSRenderNode& node, const bool resetStatus)
{
    // In normal process, if shadow has drawn in drawCacheWithBlur, no need to draw again in children node
    // not comming from drawCacheWithBlur and updateCacheProcess, child has shadow,skip draw shadow child later
    if (!drawCacheWithBlur_ && !notRunCheckAndSetNodeCacheType_ && !allCacheFilterRects_[node.GetId()].empty() &&
        node.ChildHasVisibleFilter() && updateCacheProcessCnt_ == 0) {
        if (resetStatus) {
            RS_TRACE_NAME("status reset");
            noNeedTodrawShadowAgain_ = false;
            return;
        }
        noNeedTodrawShadowAgain_ = true;
    }
}

void RSUniRenderVisitor::SetNodeSkipShadow(std::shared_ptr<RSRenderNode> node, const bool resetStatus)
{
    // skip shadow drawing in updateCacheProcess，it will draw in drawCacheWithBlur
    // and skip shadow repeat drawing in normal process
    if (!drawCacheWithBlur_ && node->GetRenderProperties().GetShadowColorStrategy() !=
        SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE &&
        (updateCacheProcessCnt_ != 0 || noNeedTodrawShadowAgain_)) {
        ROSEN_LOGD("skip draw shadow and text repeatly");
        if (resetStatus) {
            node->GetMutableRenderProperties().SetNeedSkipShadow(false);
            return;
        }
        node->GetMutableRenderProperties().SetNeedSkipShadow(true);
    }
}

void RSUniRenderVisitor::ProcessChildren(RSRenderNode& node)
{
    if (DrawBlurInCache(node)) {
        return;
    }

#ifdef RS_ENABLE_STACK_CULLING
    if (RSSystemProperties::GetViewOcclusionCullingEnabled()) {
        node.SetSubNodesCovered();
    }
#endif

    CheckSkipRepeatShadow(node, false);
    const auto children = node.GetSortedChildren();

    if (isSubThread_) {
        node.SetIsUsedBySubThread(true);
        ProcessShadowFirst(node, isSubThread_);
        for (auto child : *children) {
            ProcessChildInner(node, child);
        }
        node.SetIsUsedBySubThread(false);
    } else {
        ProcessShadowFirst(node, isSubThread_);
        for (auto child : *children) {
            SetNodeSkipShadow(child, false);
            ProcessChildInner(node, child);
            SetNodeSkipShadow(child, true);
        }
    }

    CheckSkipRepeatShadow(node, true);
#ifdef RS_ENABLE_STACK_CULLING
    if (RSSystemProperties::GetViewOcclusionCullingEnabled()) {
        node.ResetSubNodesCovered();
    }
#endif
}

void RSUniRenderVisitor::ProcessChildrenForScreenRecordingOptimization(
    RSDisplayRenderNode& node, NodeId rootIdOfCaptureWindow)
{
    if (DrawBlurInCache(node)) {
        return;
    }
    // just process child above the root of capture window
    bool startVisit = false;
    for (auto child : *node.GetSortedChildren()) {
        if (child->GetId() == rootIdOfCaptureWindow) {
            startVisit = true;
        }
        if (startVisit) {
            ProcessChildInner(node, child);
        } else {
            RS_LOGD("RSUniRenderVisitor::ProcessChildrenForScreenRecordingOptimization "\
                "skip process node %{public}s because of screen recording optimization",
                std::to_string(child->GetId()).c_str());
        }
    }
}

void RSUniRenderVisitor::ProcessChildInner(RSRenderNode& node, const RSRenderNode::SharedPtr child)
{
    if (node.GetDrawingCacheRootId() != INVALID_NODEID) {
        child->SetDrawingCacheRootId(node.GetDrawingCacheRootId());
    }
    SaveCurSurface(curSurfaceDirtyManager_, curSurfaceNode_);
    UpdateVirtualScreenFilterAppRootId(child);
    child->Process(shared_from_this());
    UpdateVirtualScreenFilterAppRootId(child);
    RestoreCurSurface(curSurfaceDirtyManager_, curSurfaceNode_);
}

void RSUniRenderVisitor::UpdateVirtualScreenFilterAppRootId(const RSRenderNode::SharedPtr& node)
{
    if (node->GetType() == RSRenderNodeType::SURFACE_NODE && virtualScreenFilterAppRootId_ == INVALID_NODEID &&
        screenInfo_.filteredAppSet.find(node->GetId()) != screenInfo_.filteredAppSet.end()) {
        // limit surface node is to reduce filteredAppSet set times
        // don't update if node's parent is in filteredAppSet
        virtualScreenFilterAppRootId_ = node->GetId();
    } else if (virtualScreenFilterAppRootId_ == node->GetId()) {
        // restore virtualScreenFilterAppRootId_ only by itself
        virtualScreenFilterAppRootId_ = INVALID_NODEID;
    }
}

std::shared_ptr<Drawing::Image> RSUniRenderVisitor::GetCacheImageFromMirrorNode(
    std::shared_ptr<RSDisplayRenderNode> mirrorNode)
{
    auto image = std::make_shared<Drawing::Image>();
    auto cacheImage = mirrorNode->GetCacheImgForCapture();

    bool parallelComposition = RSMainThread::Instance()->GetParallelCompositionEnabled();
    if (!parallelComposition) {
        return cacheImage;
    }

    if (cacheImage != nullptr) {
        auto renderContext = renderEngine_->GetRenderContext();
        if (renderContext != nullptr) {
            auto grContext = renderContext->GetDrGPUContext();
            auto imageBackendTexure = cacheImage->GetBackendTexture(false, nullptr);
            if (grContext != nullptr && imageBackendTexure.IsValid()) {
                Drawing::BitmapFormat bitmapFormat = {Drawing::ColorType::COLORTYPE_RGBA_8888,
                    Drawing::AlphaType::ALPHATYPE_PREMUL};
                image->BuildFromTexture(*grContext, imageBackendTexure.GetTextureInfo(),
                    Drawing::TextureOrigin::BOTTOM_LEFT, bitmapFormat, nullptr);
            }
        }
    }
    return image;
}

void RSUniRenderVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode& node)
{
    if (mirroredDisplays_.size() == 0) {
        node.SetCacheImgForCapture(nullptr);
    }
    RS_TRACE_NAME("ProcessDisplayRenderNode[" + std::to_string(node.GetScreenId()) + "]" +
        node.GetDirtyManager()->GetDirtyRegion().ToString().c_str());
    RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode node: %{public}" PRIu64 ", child size:%{public}u",
        node.GetId(), node.GetChildrenCount());
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode ScreenManager is nullptr");
        return;
    }
    constexpr int ROTATION_NUM = 4;
    auto screenRotation = node.GetScreenRotation();
    if (RSSystemProperties::IsFoldScreenFlag() && node.GetScreenId() == 0) {
        screenRotation = static_cast<ScreenRotation>((static_cast<int>(screenRotation) + 1) % ROTATION_NUM);
    }
    RSPointLightManager::Instance()->SetScreenRotation(screenRotation);
    screenInfo_ = screenManager->QueryScreenInfo(node.GetScreenId());
    isSecurityDisplay_ = node.GetSecurityDisplay();
    auto mirrorNode = node.GetMirrorSource().lock();
    switch (screenInfo_.state) {
        case ScreenState::PRODUCER_SURFACE_ENABLE:
            node.SetCompositeType(mirrorNode ?
                RSDisplayRenderNode::CompositeType::UNI_RENDER_MIRROR_COMPOSITE :
                RSDisplayRenderNode::CompositeType::UNI_RENDER_EXPAND_COMPOSITE);
            break;
        case ScreenState::HDI_OUTPUT_ENABLE:
            node.SetCompositeType(node.IsForceSoftComposite() ?
                RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE :
                RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE);
            break;
        default:
            RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode ScreenState unsupported");
            return;
    }
    offsetX_ = node.GetDisplayOffsetX();
    offsetY_ = node.GetDisplayOffsetY();
    // in multidisplay scenario, curDisplayDirtyManager_ will be reset in another display's prepare stage
    curDisplayDirtyManager_ = node.GetDirtyManager();
    processor_ = RSProcessorFactory::CreateProcessor(node.GetCompositeType());
    if (processor_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: RSProcessor is null!");
        return;
    }

    if (renderEngine_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: renderEngine is null!");
        return;
    }
    if (!processor_->Init(node, node.GetDisplayOffsetX(), node.GetDisplayOffsetY(),
        mirrorNode ? mirrorNode->GetScreenId() : INVALID_SCREEN_ID, renderEngine_)) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: processor init failed!");
        return;
    }
    std::shared_ptr<RSBaseRenderNode> nodePtr = node.shared_from_this();
    auto displayNodePtr = nodePtr->ReinterpretCastTo<RSDisplayRenderNode>();
    if (!displayNodePtr) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode ReinterpretCastTo fail");
        return;
    }
    if (!node.IsSurfaceCreated()) {
        sptr<IBufferConsumerListener> listener = new RSUniRenderListener(displayNodePtr);
        if (!node.CreateSurface(listener)) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode CreateSurface failed");
            return;
        }
    }

    // Wired screen projection
    if (node.GetCompositeType() == RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE && mirrorNode) {
        RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode wired screen projection");
        auto rsSurface = node.GetRSSurface();
        if (!rsSurface) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode No RSSurface found");
            return;
        }
        // we should request a framebuffer whose size is equals to the physical screen size.
        RS_TRACE_BEGIN("RSUniRender:RequestFrame");
        BufferRequestConfig bufferConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo_, true);
        RS_OPTIONAL_TRACE_BEGIN("RSUniRender::wait for bufferRequest cond");
        if (!RSMainThread::Instance()->WaitUntilDisplayNodeBufferReleased(node)) {
            RS_TRACE_NAME("RSUniRenderVisitor no released buffer");
        }
        RS_OPTIONAL_TRACE_END();
#ifdef NEW_RENDER_CONTEXT
        renderFrame_ = renderEngine_->RequestFrame(std::static_pointer_cast<RSRenderSurfaceOhos>(rsSurface),
            bufferConfig);
#else
        renderFrame_ = renderEngine_->RequestFrame(std::static_pointer_cast<RSSurfaceOhos>(rsSurface), bufferConfig);
#endif
        RS_TRACE_END();

        if (!renderFrame_) {
            RS_LOGE("RSUniRenderVisitor Request Frame Failed");
            return;
        }
        std::shared_ptr<RSCanvasListener> overdrawListener = nullptr;
        AddOverDrawListener(renderFrame_, overdrawListener);

        if (!canvas_) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: failed to create canvas");
            return;
        }
        if (mirrorNode->GetScreenRotation() == ScreenRotation::ROTATION_90 ||
            mirrorNode->GetScreenRotation() == ScreenRotation::ROTATION_270) {
            mirrorAutoRotate_ = true;
        }
        canvas_->Save();
        bool canvasRotation = screenManager->GetCanvasRotation(node.GetScreenId());
        ScaleMirrorIfNeed(node, canvasRotation);
        RotateMirrorCanvasIfNeed(node, canvasRotation);
        bool forceCPU = false;
        auto params = RSUniRenderUtil::CreateBufferDrawParam(*mirrorNode, forceCPU);
        params.isMirror = true;
        renderEngine_->DrawDisplayNodeWithParams(*canvas_, *mirrorNode, params);
        canvas_->Restore();
        mirrorAutoRotate_ = false;
        renderFrame_->Flush();
        processor_->ProcessDisplaySurface(node);
        processor_->PostProcess();
        return;
    }

    if (mirrorNode) {
        auto processor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor_);
        if (mirrorNode->GetSecurityDisplay() != isSecurityDisplay_ && processor &&
            (hasCaptureWindow_[mirrorNode->GetScreenId()] || displayHasSecSurface_[mirrorNode->GetScreenId()] ||
            displayHasSkipSurface_[mirrorNode->GetScreenId()] || !screenInfo_.filteredAppSet.empty())) {
            if (isPc_&& hasCaptureWindow_[mirrorNode->GetScreenId()]) {
                processor->MirrorScenePerf();
            }
            canvas_ = processor->GetCanvas();
            if (canvas_ == nullptr) {
                RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode failed to get canvas.");
                return;
            }
            canvas_->SetDisableFilterCache(true);
            if (displayHasSecSurface_[mirrorNode->GetScreenId()]) {
                canvas_->Clear(Drawing::Color::COLOR_BLACK);
                processor_->PostProcess();
                RS_LOGI("RSUniRenderVisitor::ProcessDisplayRenderNode, set canvas to black because of security layer.");
                canvas_->SetDisableFilterCache(false);
                return;
            }
            std::shared_ptr<Drawing::Image> cacheImageProcessed = GetCacheImageFromMirrorNode(mirrorNode);
            bool canvasRotation = screenManager->GetCanvasRotation(node.GetScreenId());
            if (cacheImageProcessed && !displayHasSkipSurface_[mirrorNode->GetScreenId()] &&
                !displayHasSecSurface_[mirrorNode->GetScreenId()] && screenInfo_.filteredAppSet.empty()) {
                RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode screen recording optimization is enable");
                ScaleMirrorIfNeed(node, canvasRotation);
                RotateMirrorCanvasIfNeed(node, canvasRotation);
                canvas_->Save();
                // If both canvas and skImage have rotated, we need to reset the canvas
                if (resetRotate_) {
                    Drawing::Matrix invertMatrix;
                    if (processor->GetScreenTransformMatrix().Invert(invertMatrix)) {
                        canvas_->ConcatMatrix(invertMatrix);
                    }
                }
                Drawing::Brush brush;
                brush.SetAntiAlias(true);
                canvas_->AttachBrush(brush);
                canvas_->DrawImage(*cacheImageProcessed, 0, 0,
                    Drawing::SamplingOptions(Drawing::CubicResampler::Mitchell()));
                canvas_->DetachBrush();
                canvas_->Restore();
                bool parallelComposition = RSMainThread::Instance()->GetParallelCompositionEnabled();
                if (!parallelComposition) {
                    auto saveCount = canvas_->Save();
                    ProcessChildrenForScreenRecordingOptimization(
                        *mirrorNode, mirrorNode->GetRootIdOfCaptureWindow());
                    canvas_->RestoreToCount(saveCount);
                }
                canvas_->Restore();
                DrawWatermarkIfNeed(*mirrorNode, true);
            } else {
                RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode screen recording optimization is disable");
                mirrorNode->SetCacheImgForCapture(nullptr);
                auto saveCount = canvas_->Save();
                ScaleMirrorIfNeed(node, canvasRotation);
                RotateMirrorCanvasIfNeed(node, canvasRotation);
                PrepareOffscreenRender(*mirrorNode);
                canvas_->SetDisableFilterCache(true);
                ProcessChildren(*mirrorNode);
                FinishOffscreenRender(true);
                DrawWatermarkIfNeed(*mirrorNode, true);
                canvas_->RestoreToCount(saveCount);
            }
            canvas_->SetDisableFilterCache(false);
        } else {
            mirrorNode->SetOriginScreenRotation(node.GetOriginScreenRotation());
            processor_->ProcessDisplaySurface(*mirrorNode);
        }
    } else if (node.GetCompositeType() == RSDisplayRenderNode::CompositeType::UNI_RENDER_EXPAND_COMPOSITE) {
        auto processor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor_);
        canvas_ = processor->GetCanvas();
        if (canvas_ == nullptr) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode failed to get canvas.");
            return;
        }
        ProcessChildren(node);
        DrawWatermarkIfNeed(node);
    } else {
        curDisplayDirtyManager_->SetSurfaceSize(screenInfo_.width, screenInfo_.height);
        if (isSurfaceRotationChanged_) {
            curDisplayDirtyManager_->MergeSurfaceRect();
            isOpDropped_ = false;
            isSurfaceRotationChanged_ = false;
        }
        if (isPartialRenderEnabled_) {
            CalcDirtyDisplayRegion(displayNodePtr);
            AddContainerDirtyToGlobalDirty(displayNodePtr);
            // Aligning displayRenderNode and surfaceRenderNode dirty region before merge dirty filter region
            if (isDirtyRegionAlignedEnable_) {
                AlignGlobalAndSurfaceDirtyRegions(displayNodePtr);
            }
            CalcDirtyFilterRegion(displayNodePtr);
            displayNodePtr->ClearCurrentSurfacePos();
        } else {
            // if isPartialRenderEnabled_ is disabled for some reason (i.e. screen rotation),
            // we should keep a fullscreen dirtyregion history to avoid dirtyregion losses.
            // isPartialRenderEnabled_ should not be disabled after current position.
            curDisplayDirtyManager_->MergeSurfaceRect();
            curDisplayDirtyManager_->UpdateDirty(isDirtyRegionAlignedEnable_);
            UpdateHardwareNodeStatusBasedOnFilterRegion(node);
        }
        if (isOpDropped_ && dirtySurfaceNodeMap_.empty()
            && !curDisplayDirtyManager_->IsCurrentFrameDirty()) {
            RS_LOGD("DisplayNode skip");
            RS_TRACE_NAME("DisplayNode skip");
            GpuDirtyRegion::GetInstance().AddSkipProcessFramesNumberForXpower(node.GetScreenId());
#ifdef OHOS_PLATFORM
            RSJankStats::GetInstance().SetSkipDisplayNode();
#endif
            resetRotate_ = CheckIfNeedResetRotate();
            if (!IsHardwareComposerEnabled()) {
                RSMainThread::Instance()->rsVSyncDistributor_->MarkRSNotRendering();
                return;
            }
            if (!RSMainThread::Instance()->WaitHardwareThreadTaskExcute()) {
                RS_LOGW("RSUniRenderVisitor::ProcessDisplayRenderNode: hardwareThread task has too many to excute");
            }
            if (!RSMainThread::Instance()->CheckIsHardwareEnabledBufferUpdated() && !forceUpdateFlag_) {
                for (auto& surfaceNode: hardwareEnabledNodes_) {
                    if (!surfaceNode->IsHardwareForcedDisabled()) {
                        surfaceNode->MarkCurrentFrameHardwareEnabled();
                    }
                }
                RS_TRACE_NAME("DisplayNodeSkip skip commit");
                RSMainThread::Instance()->rsVSyncDistributor_->MarkRSNotRendering();
                return;
            }
            RSMainThread::Instance()->rsVSyncDistributor_->UnmarkRSNotRendering();
            bool needCreateDisplayNodeLayer = false;
            for (auto& surfaceNode: hardwareEnabledNodes_) {
                if (!surfaceNode->IsHardwareForcedDisabled()) {
                    needCreateDisplayNodeLayer = true;
                    processor_->ProcessSurface(*surfaceNode);
                }
            }
            if (needCreateDisplayNodeLayer || forceUpdateFlag_) {
                processor_->ProcessDisplaySurface(node);
                DoScreenRcdTask(processor_, rcdInfo_, screenInfo_);
                processor_->PostProcess();
            }
            return;
        } else {
            RSMainThread::Instance()->rsVSyncDistributor_->UnmarkRSNotRendering();
        }

        auto rsSurface = node.GetRSSurface();
        if (rsSurface == nullptr) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode No RSSurface found");
            return;
        }
        // we should request a framebuffer whose size is equals to the physical screen size.
        RS_TRACE_BEGIN("RSUniRender:RequestFrame");
        BufferRequestConfig bufferConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo_, true,
            newColorSpace_, newPixelFormat_);
        RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode, colorspace is %{public}d, pixelformat is %{public}d in "\
                "RequestFrame.", newColorSpace_, newPixelFormat_);
        node.SetFingerprint(hasFingerprint_);
        RS_OPTIONAL_TRACE_BEGIN("RSUniRender::wait for bufferRequest cond");
        if (!RSMainThread::Instance()->WaitUntilDisplayNodeBufferReleased(node)) {
            RS_TRACE_NAME("RSUniRenderVisitor no released buffer");
        }
        RS_OPTIONAL_TRACE_END();
#ifdef NEW_RENDER_CONTEXT
        renderFrame_ = renderEngine_->RequestFrame(std::static_pointer_cast<RSRenderSurfaceOhos>(rsSurface),
            bufferConfig);
#else
        renderFrame_ = renderEngine_->RequestFrame(std::static_pointer_cast<RSSurfaceOhos>(rsSurface), bufferConfig);
#endif
        RS_TRACE_END();

        if (renderFrame_ == nullptr) {
            RS_LOGE("RSUniRenderVisitor Request Frame Failed");
            return;
        }
        std::shared_ptr<RSCanvasListener> overdrawListener = nullptr;
        AddOverDrawListener(renderFrame_, overdrawListener);

        if (canvas_ == nullptr) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: failed to create canvas");
            return;
        }

#ifdef ENABLE_RECORDING_DCL
        tryCapture(node.GetRenderProperties().GetBoundsWidth(), node.GetRenderProperties().GetBoundsHeight());
#endif
        StartOverDraw();

        int saveLayerCnt = 0;
        Drawing::Region region;
        Occlusion::Region dirtyRegionTest;
        std::vector<RectI> rects;
        bool clipPath = false;
#ifdef RS_ENABLE_VK
        int saveCountBeforeClip = 0;
        if (RSSystemProperties::IsUseVulkan()) {
            saveCountBeforeClip = canvas_->Save();
        }
#endif
        // Get displayNode buffer age in order to merge visible dirty region for displayNode.
        // And then set egl damage region to improve uni_render efficiency.
        if (isPartialRenderEnabled_) {
            // Early history buffer Merging will have impact on Overdraw display, so we need to
            // set the full screen dirty to avoid this impact.
            if (RSOverdrawController::GetInstance().IsEnabled()) {
                node.GetDirtyManager()->ResetDirtyAsSurfaceSize();
            }
            RS_OPTIONAL_TRACE_BEGIN("RSUniRender::GetBufferAge");
            int bufferAge = renderFrame_->GetBufferAge();
            RS_OPTIONAL_TRACE_END();
            RSUniRenderUtil::MergeDirtyHistory(displayNodePtr, bufferAge, isDirtyRegionAlignedEnable_);
            Occlusion::Region dirtyRegion = RSUniRenderUtil::MergeVisibleDirtyRegion(
                displayNodePtr->GetCurAllSurfaces(), RSMainThread::Instance()->GetDrawStatusVec(), isDirtyRegionAlignedEnable_);
            dirtyRegionTest = dirtyRegion;
            if (isDirtyRegionAlignedEnable_) {
                SetSurfaceGlobalAlignedDirtyRegion(displayNodePtr, dirtyRegion);
            } else {
                SetSurfaceGlobalDirtyRegion(displayNodePtr);
            }
            rects = RSUniRenderUtil::ScreenIntersectDirtyRects(dirtyRegion, screenInfo_);
            if (!rects.empty()) {
                GpuDirtyRegion::GetInstance().UpdateActiveDirtyRegionAreasAndFrameNumberForXpower(node.GetScreenId(),
                                                                                                  rects);
            }
            RectI rect = node.GetDirtyManager()->GetDirtyRegionFlipWithinSurface();
            if (!rect.IsEmpty()) {
                rects.emplace_back(rect);
                GpuDirtyRegion::GetInstance().UpdateGlobalDirtyRegionAreasAndFrameNumberForXpower(node.GetScreenId(),
                                                                                                  rect);
            }
            if (!isDirtyRegionAlignedEnable_) {
                for (auto& r : rects) {
                    int32_t topAfterFlip = 0;
#ifdef RS_ENABLE_VK
                    if (RSSystemProperties::IsUseVulkan()) {
                        topAfterFlip = r.top_;
                    } else {
                        topAfterFlip = static_cast<int32_t>(screenInfo_.GetRotatedHeight()) - r.GetBottom();
                    }
#else
                    topAfterFlip = static_cast<int32_t>(screenInfo_.GetRotatedHeight()) - r.GetBottom();
#endif
                    Drawing::Region tmpRegion;
                    tmpRegion.SetRect(Drawing::RectI(r.left_, topAfterFlip,
                        r.left_ + r.width_, topAfterFlip + r.height_));
                    region.Op(tmpRegion, Drawing::RegionOp::UNION);
                }
            }
            // SetDamageRegion and opDrop will be disabled for dirty region DFX visualization
            if (!isRegionDebugEnabled_) {
                renderFrame_->SetDamageRegion(rects);
            }
        }
        if (isOpDropped_ && !isDirtyRegionAlignedEnable_) {
            if (region.IsEmpty()) {
                // [planning] Remove this after frame buffer can cancel
                canvas_->ClipRect(Drawing::Rect());
            } else if (region.IsRect()) {
                canvas_->ClipRegion(region);
                canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
            } else {
                RS_TRACE_NAME("RSUniRenderVisitor: clipPath");
                clipPath = true;
#ifdef RS_ENABLE_VK
                if (RSSystemProperties::IsUseVulkan()) {
                    canvas_->ClipRegion(region);
                } else {
                    Drawing::Path dirtyPath;
                    region.GetBoundaryPath(&dirtyPath);
                    canvas_->ClipPath(dirtyPath, Drawing::ClipOp::INTERSECT, true);
                }
#else
                Drawing::Path dirtyPath;
                region.GetBoundaryPath(&dirtyPath);
                canvas_->ClipPath(dirtyPath, Drawing::ClipOp::INTERSECT, true);
#endif
            }
        } else {
            canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
        }

        RSPropertiesPainter::SetBgAntiAlias(true);
        if (isUIFirst_) {
            int saveCount = canvas_->Save();
            canvas_->SetHighContrast(renderEngine_->IsHighContrastEnabled());

            bool displayNodeRotationChanged = node.IsRotationChanged();
            // enable cache if screen rotation
            canvas_->SetCacheType((isScreenRotationAnimating_ || displayNodeRotationChanged)
                ? RSPaintFilterCanvas::CacheType::ENABLED
                : RSPaintFilterCanvas::CacheType::DISABLED);
            bool needOffscreen = clipPath || displayNodeRotationChanged;

            if (needOffscreen) {
                ClearTransparentBeforeSaveLayer(); // clear transparent before concat display node's matrix
            }
            auto geoPtr = node.GetRenderProperties().GetBoundsGeometry();
            if (geoPtr != nullptr) {
                canvas_->ConcatMatrix(geoPtr->GetMatrix());
            }
            if (needOffscreen) {
                // we are doing rotation animation, try offscreen render if capable
                displayNodeMatrix_ = canvas_->GetTotalMatrix();
                PrepareOffscreenRender(node);
                ProcessChildren(node);
                FinishOffscreenRender();
            } else {
                // render directly
                ProcessChildren(node);
            }
            SwitchColorFilterDrawing(saveCount);
            canvas_->RestoreToCount(saveCount);
        }
        if (saveLayerCnt > 0) {
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
#ifdef NEW_RENDER_CONTEXT
            RSTagTracker tagTracker(
                renderEngine_->GetDrawingContext()->GetDrawingContext(),
                RSTagTracker::TAGTYPE::TAG_RESTORELAYER_DRAW_NODE);
#else
            RSTagTracker tagTracker(renderEngine_->GetRenderContext()->GetDrGPUContext(),
                RSTagTracker::TAGTYPE::TAG_RESTORELAYER_DRAW_NODE);
            RS_TRACE_NAME("RSUniRender:RestoreLayer");
            canvas_->RestoreToCount(saveLayerCnt);
#endif
#endif
        }
        DrawWatermarkIfNeed(node);
        // the following code makes DirtyRegion visible, enable this method by turning on the dirtyregiondebug property
        if (isPartialRenderEnabled_) {
            if (isDirtyRegionDfxEnabled_) {
                DrawAllSurfaceDirtyRegionForDFX(node, dirtyRegionTest);
            }
            if (isTargetDirtyRegionDfxEnabled_) {
                DrawTargetSurfaceDirtyRegionForDFX(node);
            }
            if (isDisplayDirtyDfxEnabled_) {
                DrawDirtyRegionForDFX(node.GetDirtyManager()->GetMergedDirtyRegions());
            }
        }

        if (isOpaqueRegionDfxEnabled_) {
            DrawAllSurfaceOpaqueRegionForDFX(node);
        }
        if (isVisibleRegionDfxEnabled_) {
            DrawTargetSurfaceVisibleRegionForDFX(node);
        }

        if (isDrawingCacheEnabled_ && RSSystemParameters::GetDrawingCacheEnabledDfx()) {
            DrawCacheRegionForDFX(cacheRenderNodeMapRects_);
        }

        if (RSSystemProperties::GetHwcRegionDfxEnabled()) {
            DrawHwcRegionForDFX(hardwareEnabledNodes_);
        }

#ifdef DDGR_ENABLE_FEATURE_OPINC
        if (autoCacheDrawingEnable_ && !RSSystemParameters::GetDrawingCacheEnableDfx()) {
            DrawAutoCacheRegionForDFX(autoCacheRenderNodeInfos_);
        }
#endif

        if (RSSystemParameters::GetDrawingEffectRegionEnabledDfx()) {
            DrawEffectRenderNodeForDFX();
        }

        if (RSRealtimeRefreshRateManager::Instance().GetShowRefreshRateEnabled()) {
            RS_TRACE_BEGIN("RSUniRender::DrawCurrentRefreshRate");
            uint32_t currentRefreshRate =
                OHOS::Rosen::HgmCore::Instance().GetScreenCurrentRefreshRate(node.GetScreenId());
            uint32_t realtimeRefreshRate = RSRealtimeRefreshRateManager::Instance().GetRealtimeRefreshRate();
            if (realtimeRefreshRate > currentRefreshRate) {
                realtimeRefreshRate = currentRefreshRate;
            }
            DrawCurrentRefreshRate(currentRefreshRate, realtimeRefreshRate);
            RS_TRACE_END();
        }

#ifdef ENABLE_RECORDING_DCL
        endCapture();
#endif
        if ((screenInfo_.state == ScreenState::HDI_OUTPUT_ENABLE) &&
            RSSingleton<RoundCornerDisplay>::GetInstance().GetRcdEnable() &&
            (!RSSingleton<RoundCornerDisplay>::GetInstance().IsSupportHardware())) {
            RSSingleton<RoundCornerDisplay>::GetInstance().DrawRoundCorner(canvas_.get());
        }
        auto mainThread = RSMainThread::Instance();
        if (!mainThread->GetClearMemoryFinished()) {
            mainThread->RemoveTask(CLEAR_GPU_CACHE);
        }
#ifdef RS_ENABLE_VK
        if (RSSystemProperties::IsUseVulkan()) {
            canvas_->RestoreToCount(saveCountBeforeClip);
        }
#endif
        FinishOverDraw();
        RS_TRACE_BEGIN("RSUniRender:FlushFrame");
        renderFrame_->Flush();
        RS_TRACE_END();
        RS_OPTIONAL_TRACE_BEGIN("RSUniRender:WaitUtilUniRenderFinished");
        mainThread->WaitUtilUniRenderFinished();
        RS_OPTIONAL_TRACE_END();
        if (cacheImgForCapture_ != nullptr) {
            node.SetCacheImgForCapture(cacheImgForCapture_);
        }
        if (IsHardwareComposerEnabled() && !hardwareEnabledNodes_.empty()) {
            globalZOrder_ = 0.0f;
        }
        AssignGlobalZOrderAndCreateLayer(appWindowNodesInZOrder_);
        node.SetGlobalZOrder(globalZOrder_++);
        processor_->ProcessDisplaySurface(node);
        AssignGlobalZOrderAndCreateLayer(hardwareEnabledTopNodes_);
    }
    DoScreenRcdTask(processor_, rcdInfo_, screenInfo_);

    if (!RSMainThread::Instance()->WaitHardwareThreadTaskExcute()) {
        RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode: hardwareThread task has too many to excute");
    }
    processor_->PostProcess();
    auto mainThread = RSMainThread::Instance();
    if (!mainThread->GetClearMemoryFinished()) {
        mainThread->ClearMemoryCache(mainThread->GetClearMoment(), mainThread->GetClearMemDeeply());
    }
    RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode end");
#ifdef RS_ENABLE_VK
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN) {
            renderEngine_->GetRenderContext()->GetDrGPUContext()->StoreVkPipelineCacheData();
        }
#endif
}

#ifndef RS_PARALLEL
void RSUniRenderVisitor::DrawSurfaceLayer(const std::shared_ptr<RSDisplayRenderNode>& displayNode,
    const std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes) const
{
    RS_TRACE_NAME_FMT("RSUniRenderVisitor::DrawSurfaceLayer displayNode:%p subThreadNodes:%d", displayNode.get(), int(subThreadNodes.size()));
    auto subThreadManager = RSSubThreadManager::Instance();
    if (RSSingleton<RoundCornerDisplay>::GetInstance().GetRcdEnable()) {
        subThreadManager->StartRCDThread(renderEngine_->GetRenderContext().get());
    }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    subThreadManager->StartColorPickerThread(renderEngine_->GetRenderContext().get());
    subThreadManager->SubmitSubThreadTask(displayNode, subThreadNodes);
#endif
}
#endif

void RSUniRenderVisitor::SwitchColorFilterDrawing(int currentSaveCount)
{
    ColorFilterMode colorFilterMode = renderEngine_->GetColorFilterMode();
    if (colorFilterMode >= ColorFilterMode::INVERT_COLOR_ENABLE_MODE &&
        colorFilterMode <= ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE) {
        RS_LOGD("RsDebug RSBaseRenderEngine::SetColorFilterModeToPaint mode:%{public}d",
            static_cast<int32_t>(colorFilterMode));
        Drawing::Brush brush;
        RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, brush);
#if defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)
#ifdef NEW_RENDER_CONTEXT
        RSTagTracker tagTracker(
            renderEngine_->GetDrawingContext()->GetDrawingContext(),
            RSTagTracker::TAG_SAVELAYER_COLOR_FILTER);
#else
        RSTagTracker tagTracker(
            renderEngine_->GetRenderContext()->GetDrGPUContext(),
            RSTagTracker::TAG_SAVELAYER_COLOR_FILTER);
#endif
#endif
        Drawing::SaveLayerOps slr(nullptr, &brush, Drawing::SaveLayerOps::INIT_WITH_PREVIOUS);
        canvas_->SaveLayer(slr);
        canvas_->RestoreToCount(currentSaveCount);
    }
}

void RSUniRenderVisitor::AssignGlobalZOrderAndCreateLayer(
    std::vector<std::shared_ptr<RSSurfaceRenderNode>>& nodesInZOrder)
{
    if (!IsHardwareComposerEnabled()) {
        return;
    }
    if (hardwareEnabledNodes_.empty()) {
        return;
    }
    for (auto& appWindowNode : nodesInZOrder) {
        // first, sort app window node's child surfaceView by local zOrder
        auto childHardwareEnabledNodes = appWindowNode->GetChildHardwareEnabledNodes();
        for (auto iter = childHardwareEnabledNodes.begin(); iter != childHardwareEnabledNodes.end();) {
            auto childNode = iter->lock();
            if (!childNode || !childNode->IsOnTheTree()) {
                iter = childHardwareEnabledNodes.erase(iter);
                continue;
            }
            if (childNode->GetBuffer() != nullptr && !childNode->IsHardwareForcedDisabled()) {
                // SetGlobalZOrder here to ensure zOrder committed to composer is continuous
                childNode->SetGlobalZOrder(globalZOrder_++);
                RS_LOGD("createLayer: %{public}" PRIu64 "", childNode->GetId());
                processor_->ProcessSurface(*childNode);
            }
            ++iter;
        }
    }
}

void RSUniRenderVisitor::AddOverDrawListener(std::unique_ptr<RSRenderFrame>& renderFrame,
    std::shared_ptr<RSCanvasListener>& overdrawListener)
{
#if defined(NEW_RENDER_CONTEXT)
    if (renderFrame == nullptr) {
        RS_LOGE("RSUniRenderVisitor::AddOverDrawListener: renderFrame is nullptr");
        return;
    }
    auto renderSurface = renderFrame->GetSurface();
    if (renderSurface == nullptr) {
        RS_LOGE("RSUniRenderVisitor::AddOverDrawListener: renderSurface is nullptr");
        return;
    }
    RS_OPTIONAL_TRACE_BEGIN("RSUniRender::GetSurface");
    auto drSurface = renderSurface->GetSurface();
    RS_OPTIONAL_TRACE_END();
    if (drSurface == nullptr) {
        RS_LOGE("RSUniRenderVisitor::AddOverDrawListener: drSurface is null");
        return;
    }
    if (drSurface->GetCanvas() == nullptr) {
        ROSEN_LOGE("drSurface.getCanvas is null.");
        return;
    }
#else
    if (renderFrame->GetFrame() == nullptr) {
        RS_LOGE("RSUniRenderVisitor::AddOverDrawListener: RSSurfaceFrame is nullptr");
        return;
    }
    RS_OPTIONAL_TRACE_BEGIN("RSUniRender::GetSurface");
    auto drSurface = renderFrame->GetFrame()->GetSurface();
    RS_OPTIONAL_TRACE_END();
    if (drSurface == nullptr) {
        RS_LOGE("RSUniRenderVisitor::AddOverDrawListener: drSurface is null");
        return;
    }
    if (drSurface->GetCanvas() == nullptr) {
        ROSEN_LOGE("drSurface.getCanvas is null.");
        return;
    }
#endif
    canvas_ = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
}

bool RSUniRenderVisitor::IsNotDirtyHardwareEnabledTopSurface(std::shared_ptr<RSSurfaceRenderNode>& node) const
{
    if (!node->IsHardwareEnabledTopSurface()) {
        return false;
    }
    // If the pointer is dirty in last frame but not in current, when gpu -> hardware composer.
    // It should also calc global dirty in current frame.
    node->SetNodeDirty(isHardwareForcedDisabled_ || node->HasSubNodeShouldPaint() ||
        !node->IsLastFrameHardwareEnabled());
    return !node->IsNodeDirty();
}

void RSUniRenderVisitor::CalcDirtyDisplayRegion(std::shared_ptr<RSDisplayRenderNode>& node)
{
    RS_OPTIONAL_TRACE_FUNC();
    auto displayDirtyManager = node->GetDirtyManager();
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr) {
            continue;
        }
        if (IsNotDirtyHardwareEnabledTopSurface(surfaceNode)) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNode->GetDirtyManager();
        if (isUIFirst_ && surfaceDirtyManager->IsCurrentFrameDirty()) {
            dirtySurfaceNodeMap_.emplace(surfaceNode->GetId(), surfaceNode->ReinterpretCastTo<RSSurfaceRenderNode>());
        }
        RectI surfaceDirtyRect = surfaceDirtyManager->GetCurrentFrameDirtyRegion();
        if (surfaceNode->IsTransparent()) {
            // Handles the case of transparent surface, merge transparent dirty rect
            RectI transparentDirtyRect = surfaceNode->GetDstRect().IntersectRect(surfaceDirtyRect);
            if (!transparentDirtyRect.IsEmpty()) {
                RS_OPTIONAL_TRACE_NAME_FMT("CalcDirtyDisplayRegion merge transparent dirty rect %s rect %s",
                    surfaceNode->GetName().c_str(), transparentDirtyRect.ToString().c_str());
                displayDirtyManager->MergeDirtyRect(transparentDirtyRect);
            }
        }

        if (surfaceNode->GetZorderChanged()) {
            // Zorder changed case, merge surface dest Rect
            RS_LOGD("CalcDirtyDisplayRegion merge GetZorderChanged %{public}s rect %{public}s",
                surfaceNode->GetName().c_str(), surfaceNode->GetDstRect().ToString().c_str());
            displayDirtyManager->MergeDirtyRect(surfaceNode->GetDstRect());
        }

        RectI lastFrameSurfacePos = node->GetLastFrameSurfacePos(surfaceNode->GetId());
        RectI currentFrameSurfacePos = node->GetCurrentFrameSurfacePos(surfaceNode->GetId());
        if (surfaceNode->GetAnimateState() || lastFrameSurfacePos != currentFrameSurfacePos) {
            RS_LOGD("CalcDirtyDisplayRegion merge surface pos changed %{public}s lastFrameRect %{public}s"
                " currentFrameRect %{public}s", surfaceNode->GetName().c_str(), lastFrameSurfacePos.ToString().c_str(),
                currentFrameSurfacePos.ToString().c_str());
            if (!lastFrameSurfacePos.IsEmpty()) {
                displayDirtyManager->MergeDirtyRect(lastFrameSurfacePos);
            }
            if (!currentFrameSurfacePos.IsEmpty()) {
                displayDirtyManager->MergeDirtyRect(currentFrameSurfacePos);
            }
        }

        bool isShadowDisappear = !surfaceNode->GetRenderProperties().IsShadowValid() && surfaceNode->IsShadowValidLastFrame();
        if (surfaceNode->GetRenderProperties().IsShadowValid() || isShadowDisappear) {
            RectI shadowDirtyRect = surfaceNode->GetOldDirtyInSurface().IntersectRect(surfaceDirtyRect);
            // There are two situation here:
            // 1. SurfaceNode first has shadow or shadow radius is larger than the last frame,
            // surfaceDirtyRect == surfaceNode->GetOldDirtyInSurface()
            // 2. SurfaceNode remove shadow or shadow radius is smaller than the last frame,
            // surfaceDirtyRect > surfaceNode->GetOldDirtyInSurface()
            // So we should always merge surfaceDirtyRect here.
            if (!shadowDirtyRect.IsEmpty()) {
                displayDirtyManager->MergeDirtyRect(surfaceDirtyRect);
                RS_LOGD("CalcDirtyDisplayRegion merge ShadowValid %{public}s rect %{public}s",
                    surfaceNode->GetName().c_str(), surfaceDirtyRect.ToString().c_str());
            }
            if (isShadowDisappear) {
                surfaceNode->SetShadowValidLastFrame(false);
            }
        }
    }
    std::vector<RectI> surfaceChangedRects = node->GetSurfaceChangedRects();
    for (auto& surfaceChangedRect : surfaceChangedRects) {
        RS_LOGD("CalcDirtyDisplayRegion merge Surface closed %{public}s", surfaceChangedRect.ToString().c_str());
        if (!surfaceChangedRect.IsEmpty()) {
            displayDirtyManager->MergeDirtyRect(surfaceChangedRect);
        }
    }
    if (RSRealtimeRefreshRateManager::Instance().GetShowRefreshRateEnabled()) {
        RectI tempRect = {100, 100, 500, 200};   // setDirtyRegion for RealtimeRefreshRate
        displayDirtyManager->MergeDirtyRect(tempRect);
    }
}

void RSUniRenderVisitor::MergeDirtyRectIfNeed(std::shared_ptr<RSSurfaceRenderNode> appNode,
    std::shared_ptr<RSSurfaceRenderNode> hwcNode)
{
    if ((hwcNode->IsLastFrameHardwareEnabled() || hwcNode->IsCurrentFrameBufferConsumed()) &&
        appNode && appNode->GetDirtyManager()) {
        appNode->GetDirtyManager()->MergeDirtyRect(hwcNode->GetDstRect());
        dirtySurfaceNodeMap_.emplace(appNode->GetId(), appNode);
    }
}

RectI RSUniRenderVisitor::UpdateHardwareEnableList(std::vector<RectI>& filterRects,
    std::vector<SurfaceDirtyMgrPair>& validHwcNodes)
{
    if (validHwcNodes.empty() || filterRects.empty()) {
        return RectI();
    }
    // remove invisible surface since occlusion
    // check intersected parts
    RectI filterDirty;
    for (auto iter = validHwcNodes.begin(); iter != validHwcNodes.end(); ++iter) {
        auto childNode = iter->first;
        auto childDirtyRect = childNode->GetDstRect();
        bool isIntersected = false;
        // remove invisible surface since occlusion
        for (auto& filterRect : filterRects) {
            if (!childDirtyRect.IntersectRect(filterRect).IsEmpty()) {
                filterDirty = filterDirty.JoinRect(filterRect);
                isIntersected = true;
            }
        }
        if (isIntersected) {
            childNode->SetHardwareForcedDisabledStateByFilter(true);
            auto node = iter->second;
            MergeDirtyRectIfNeed(iter->second, childNode);
            iter = validHwcNodes.erase(iter);
            iter--;
        }
    }
    return filterDirty;
}

void RSUniRenderVisitor::UpdateHardwareNodeStatusBasedOnFilter(std::shared_ptr<RSSurfaceRenderNode>& node,
    std::vector<SurfaceDirtyMgrPair>& prevHwcEnabledNodes,
    std::shared_ptr<RSDirtyRegionManager>& displayDirtyManager)
{
    if (!IsHardwareComposerEnabled()) {
        return;
    }
    if (node == nullptr || !node->IsAppWindow() || node->GetDirtyManager() == nullptr ||
        displayDirtyManager == nullptr) {
        return;
    }
    auto dirtyManager = node->GetDirtyManager();
    auto filterRects = node->GetChildrenNeedFilterRects();
    // collect valid hwc surface which is not intersected with filterRects
    std::vector<SurfaceDirtyMgrPair> curHwcEnabledNodes;
    // remove invisible surface since occlusion
    auto visibleRegion = node->GetVisibleRegion();
    for (auto subNode : node->GetChildHardwareEnabledNodes()) {
        auto childNode = subNode.lock();
        if (!childNode) {
            continue;
        }
        // recover disabled state before update
        childNode->SetHardwareForcedDisabledStateByFilter(false);
        if (!visibleRegion.IsIntersectWith(Occlusion::Rect(childNode->GetOldDirtyInSurface()))) {
            continue;
        }
        bool isIntersected = false;
        if (isPhone_) {
            for (auto &hwcNode: curHwcEnabledNodes) {
                if (childNode->GetDstRect().Intersect(hwcNode.first->GetDstRect())) {
                    childNode->SetHardwareForcedDisabledStateByFilter(true);
                    isIntersected = true;
                    break;
                }
            }
        }
        if (!isPhone_ || !isIntersected) {
            curHwcEnabledNodes.emplace_back(std::make_pair(subNode, node));
        }
    }
    // Within App: disable hwc if intersect with filterRects
    dirtyManager->MergeDirtyRect(UpdateHardwareEnableList(filterRects, curHwcEnabledNodes));
    // Among App: disable lower hwc layers if intersect with upper transparent appWindow
    if (node->IsTransparent()) {
        if (node->GetRenderProperties().NeedFilter()) {
            // Attention: if transparent appwindow needs filter, only need to check itself
            filterRects = {node->GetDstRect()};
        }
        // In case of transparent window, filterRects need hwc surface's content
        RectI globalTransDirty = UpdateHardwareEnableList(filterRects, prevHwcEnabledNodes);
        displayDirtyManager->MergeDirtyRect(globalTransDirty);
        dirtyManager->MergeDirtyRect(globalTransDirty);
    }
    // erase from curHwcEnabledNodes if app node has no container window and its hwc node intersects with hwc below
    if (!node->HasContainerWindow() && !curHwcEnabledNodes.empty() && !prevHwcEnabledNodes.empty()) {
        for (auto iter = curHwcEnabledNodes.begin(); iter != curHwcEnabledNodes.end(); ++iter) {
            for (auto& prevNode : prevHwcEnabledNodes) {
                if (!iter->first->GetDstRect().IntersectRect(prevNode.first->GetDstRect()).IsEmpty()) {
                    iter->first->SetHardwareForcedDisabledStateByFilter(true);
                    MergeDirtyRectIfNeed(iter->second, iter->first);
                    iter = curHwcEnabledNodes.erase(iter);
                    iter--;
                    break;
                }
            }
        }
    }
    if (!curHwcEnabledNodes.empty()) {
        prevHwcEnabledNodes.insert(prevHwcEnabledNodes.end(), curHwcEnabledNodes.begin(), curHwcEnabledNodes.end());
    }
}

void RSUniRenderVisitor::CalcDirtyRegionForFilterNode(const RectI& filterRect,
    std::shared_ptr<RSSurfaceRenderNode>& currentSurfaceNode,
    std::shared_ptr<RSDisplayRenderNode>& displayNode)
{
    auto displayDirtyManager = displayNode->GetDirtyManager();
    auto currentSurfaceDirtyManager = currentSurfaceNode->GetDirtyManager();
    if (displayDirtyManager == nullptr || currentSurfaceDirtyManager == nullptr) {
        return;
    }

    RectI displayDirtyRect = displayDirtyManager->GetCurrentFrameDirtyRegion();
    RectI currentSurfaceDirtyRect = currentSurfaceDirtyManager->GetCurrentFrameDirtyRegion();
    bool displayDirtyIntersectRectFilter = !displayDirtyRect.IntersectRect(filterRect).IsEmpty();
    bool surfaceDirtyIntersectRectFilter = !currentSurfaceDirtyRect.IntersectRect(filterRect).IsEmpty();
    if (displayDirtyIntersectRectFilter || surfaceDirtyIntersectRectFilter) {
        currentSurfaceDirtyManager->MergeDirtyRect(filterRect);
        if (!currentSurfaceNode->IsTransparent()) {
            Occlusion::Region filterRegion(Occlusion::Rect(filterRect.GetLeft(), filterRect.GetTop(),
                filterRect.GetRight(), filterRect.GetBottom()));
            if (!filterRegion.Sub(currentSurfaceNode->GetOpaqueRegion()).IsEmpty()) {
                displayDirtyManager->MergeDirtyRect(filterRect);
                return;
            }
        }
    }

    if (currentSurfaceNode->IsTransparent()) {
        if (displayDirtyIntersectRectFilter) {
            displayDirtyManager->MergeDirtyRect(filterRect);
            return;
        }
        // If currentSurfaceNode is transparent and displayDirtyRect is not intersect with filterRect,
        // We should check whether window below currentSurfaceNode has dirtyRect intersect with filterRect.
        for (auto belowSurface = displayNode->GetCurAllSurfaces().begin();
            belowSurface != displayNode->GetCurAllSurfaces().end(); ++belowSurface) {
            auto belowSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*belowSurface);
            if (belowSurfaceNode == currentSurfaceNode) {
                break;
            }
            if (belowSurfaceNode == nullptr || !belowSurfaceNode->IsAppWindow()) {
                continue;
            }
            auto belowSurfaceDirtyManager = belowSurfaceNode->GetDirtyManager();
            RectI belowDirtyRect =
                belowSurfaceDirtyManager ? belowSurfaceDirtyManager->GetCurrentFrameDirtyRegion() : RectI{0, 0, 0, 0};
            if (belowDirtyRect.IsEmpty()) {
                continue;
            }
            // To minimize dirtyRect, only filterRect has intersect with both visibleRegion and dirtyRect
            // of window below, we add filterRect to displayDirtyRect and currentSurfaceDirtyRect.
            if (belowSurfaceNode->GetVisibleRegion().IsIntersectWith(filterRect) &&
                !belowDirtyRect.IntersectRect(filterRect).IsEmpty()) {
                displayDirtyManager->MergeDirtyRect(filterRect);
                currentSurfaceDirtyManager->MergeDirtyRect(filterRect);
                break;
            }
        }
    }
}

void RSUniRenderVisitor::CalcChildFilterNodeDirtyRegion(std::shared_ptr<RSSurfaceRenderNode>& currentSurfaceNode,
    std::shared_ptr<RSDisplayRenderNode>& displayNode)
{
    if (currentSurfaceNode == nullptr || displayNode == nullptr) {
        return;
    }
    auto filterRects = currentSurfaceNode->GetChildrenNeedFilterRects();
    auto filterNodes = currentSurfaceNode->GetChildrenFilterNodes();
    if (currentSurfaceNode->IsAppWindow() && !filterRects.empty()) {
        needFilter_ = needFilter_ || !currentSurfaceNode->IsStaticCached();
        for (size_t i = 0; i < filterNodes.size(); i++) {
            auto filterRectsCacheValidNow = filterNodes[i]->IsBackgroundFilterCacheValid();
            // if child filter node has filter cache, no need to be added into dirtyregion
            // only support background filter cache valid and no pixelstretch node now
            if (isCacheBlurPartialRenderEnabled_ && filterRectsCacheValidNow &&
                !filterNodes[i]->GetRenderProperties().GetPixelStretch().has_value()) {
                continue;
            }
            CalcDirtyRegionForFilterNode(filterRects[i], currentSurfaceNode, displayNode);
        }
    }
}

void RSUniRenderVisitor::CalcSurfaceFilterNodeDirtyRegion(std::shared_ptr<RSSurfaceRenderNode>& currentSurfaceNode,
    std::shared_ptr<RSDisplayRenderNode>& displayNode)
{
    if (currentSurfaceNode == nullptr || displayNode == nullptr) {
        return;
    }
    if (currentSurfaceNode->GetRenderProperties().NeedFilter()) {
        needFilter_ = needFilter_ || !currentSurfaceNode->IsStaticCached();
        CalcDirtyRegionForFilterNode(
            currentSurfaceNode->GetOldDirtyInSurface(), currentSurfaceNode, displayNode);
    }
}

void RSUniRenderVisitor::UpdateHardwareNodeStatusBasedOnFilterRegion(RSDisplayRenderNode& displayNode)
{
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> prevHwcEnabledNodes;
    for (auto it = displayNode.GetCurAllSurfaces().begin(); it != displayNode.GetCurAllSurfaces().end(); ++it) {
        auto currentSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (currentSurfaceNode != nullptr) {
            UpdateHardwareNodeStatusBasedOnFilter(currentSurfaceNode, prevHwcEnabledNodes);
        }
    }
}

void RSUniRenderVisitor::UpdateHardwareNodeStatusBasedOnFilter(std::shared_ptr<RSSurfaceRenderNode>& node,
    std::vector<std::shared_ptr<RSSurfaceRenderNode>>& prevHwcEnabledNodes)
{
    if (!IsHardwareComposerEnabled()) {
        return;
    }
    if (node == nullptr || !node->IsAppWindow()) {
        return;
    }
    auto filterRects = node->GetChildrenNeedFilterRects();
    // collect valid hwc surface which is not intersected with filterRects
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> curHwcEnabledNodes;
    for (auto subNode : node->GetChildHardwareEnabledNodes()) {
        auto childNode = subNode.lock();
        if (!childNode) {
            continue;
        }
        childNode->SetHardwareForcedDisabledStateByFilter(false);
        bool isIntersected = false;
        if (isPhone_) {
            for (auto &hwcNode: curHwcEnabledNodes) {
                if (childNode->GetDstRect().Intersect(hwcNode->GetDstRect())) {
                    childNode->SetHardwareForcedDisabledStateByFilter(true);
                    isIntersected = true;
                    break;
                }
            }
        }
        if (!isPhone_ || !isIntersected) {
            curHwcEnabledNodes.emplace_back(childNode);
        }
    }
    // Within App: disable hwc if intersect with filterRects
    UpdateHardwareEnableList(filterRects, curHwcEnabledNodes);
    // Among App: disable lower hwc layers if intersect with upper transparent appWindow
    if (node->IsTransparent()) {
        if (node->GetRenderProperties().NeedFilter()) {
            // Attention: if transparent appwindow needs filter, only need to check itself
            filterRects = {node->GetDstRect()};
        }
        // In case of transparent window, filterRects need hwc surface's content
        UpdateHardwareEnableList(filterRects, prevHwcEnabledNodes);
    }
    if (!curHwcEnabledNodes.empty()) {
        prevHwcEnabledNodes.insert(prevHwcEnabledNodes.end(), curHwcEnabledNodes.begin(), curHwcEnabledNodes.end());
    }
}

void RSUniRenderVisitor::UpdateHardwareEnableList(std::vector<RectI>& filterRects,
    std::vector<std::shared_ptr<RSSurfaceRenderNode>>& validHwcNodes)
{
    if (validHwcNodes.empty() || filterRects.empty()) {
        return;
    }
    for (auto iter = validHwcNodes.begin(); iter != validHwcNodes.end(); ++iter) {
        auto childDirtyRect = (*iter)->GetDstRect();
        bool isIntersected = false;
        for (auto& filterRect : filterRects) {
            if (!childDirtyRect.IntersectRect(filterRect).IsEmpty()) {
                isIntersected = true;
            }
        }
        if (isIntersected) {
            (*iter)->SetHardwareForcedDisabledStateByFilter(true);
            iter = validHwcNodes.erase(iter);
            iter--;
        }
    }
}

void RSUniRenderVisitor::CalcDirtyFilterRegion(std::shared_ptr<RSDisplayRenderNode>& displayNode)
{
    if (displayNode == nullptr || displayNode->GetDirtyManager() == nullptr) {
        return;
    }
    auto displayDirtyManager = displayNode->GetDirtyManager();
    std::vector<SurfaceDirtyMgrPair> prevHwcEnabledNodes;
    for (auto it = displayNode->GetCurAllSurfaces().begin(); it != displayNode->GetCurAllSurfaces().end(); ++it) {
        auto currentSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (currentSurfaceNode) {
            // [planning] Update hwc surface dirty status at the same time
            UpdateHardwareNodeStatusBasedOnFilter(currentSurfaceNode, prevHwcEnabledNodes, displayDirtyManager);
        }
    }

    for (auto it = displayNode->GetCurAllSurfaces().begin(); it != displayNode->GetCurAllSurfaces().end();) {
        auto currentSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (currentSurfaceNode == nullptr) {
            ++it;
            continue;
        }
        if (currentSurfaceNode->GetVisibleRegion().IsEmpty()) {
            ++it;
            continue;
        }
        auto lastDisplayDirtyRegion = displayDirtyManager->GetCurrentFrameDirtyRegion();
        // child node (component) has filter
        CalcChildFilterNodeDirtyRegion(currentSurfaceNode, displayNode);
        // surfaceNode self has filter
        CalcSurfaceFilterNodeDirtyRegion(currentSurfaceNode, displayNode);
        if (lastDisplayDirtyRegion != displayDirtyManager->GetCurrentFrameDirtyRegion()) {
            // When DisplayDirtyRegion is changed, collect dirty filter region from begin.
            // After all filter region is added, the cycle will definitely stop. there is no risk of a dead cycle.
            it = displayNode->GetCurAllSurfaces().begin();
        } else {
            ++it;
        }
    }
}

void RSUniRenderVisitor::AddContainerDirtyToGlobalDirty(std::shared_ptr<RSDisplayRenderNode>& node) const
{
    RS_OPTIONAL_TRACE_FUNC();
    auto displayDirtyManager = node->GetDirtyManager();
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr) {
            continue;
        }
        if (!surfaceNode->IsNodeDirty()) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNode->GetDirtyManager();
        if (surfaceDirtyManager == nullptr) {
            continue;
        }
        RectI surfaceDirtyRect = surfaceDirtyManager->GetCurrentFrameDirtyRegion();
        auto surfaceDirtyRegion = Occlusion::Region{Occlusion::Rect{surfaceDirtyRect}};
        if (surfaceNode->HasContainerWindow()) {
            // If a surface's dirty is intersect with container region (which can be considered transparent)
            // should be added to display dirty region.
            // Note: we use containerRegion rather transparentRegion to bypass inner corner dirty problem.
            auto containerRegion = surfaceNode->GetContainerRegion();
            auto containerDirtyRegion = containerRegion.And(surfaceDirtyRegion);
            if (!containerDirtyRegion.IsEmpty()) {
                RS_LOGD("CalcDirtyDisplayRegion merge containerDirtyRegion %{public}s region %{public}s",
                    surfaceNode->GetName().c_str(), containerDirtyRegion.GetRegionInfo().c_str());
                // plan: we can use surfacenode's absrect as containerRegion's bound
                const auto& rect = containerRegion.GetBoundRef();
                displayDirtyManager->MergeDirtyRect(
                    RectI{ rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_ });
                continue;
            }
        }
        // warning: if a surfacenode has transparent region and opaque region, and its dirty pattern appears in
        // transparent region and opaque region in adjacent frame, may cause displaydirty region incomplete after
        // merge history (as surfacenode's dirty region merging opaque region will enlarge surface dirty region
        // which include transparent region but not counted in display dirtyregion)
        auto transparentRegion = surfaceNode->GetTransparentRegion();
        Occlusion::Region transparentDirtyRegion = transparentRegion.And(surfaceDirtyRegion);
        if (!transparentDirtyRegion.IsEmpty()) {
            RS_LOGD("CalcDirtyDisplayRegion merge TransparentDirtyRegion %{public}s region %{public}s",
                surfaceNode->GetName().c_str(), transparentDirtyRegion.GetRegionInfo().c_str());
            const std::vector<Occlusion::Rect>& rects = transparentDirtyRegion.GetRegionRects();
            for (const auto& rect : rects) {
                displayDirtyManager->MergeDirtyRect(
                    RectI{ rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_ });
            }
        }
    }
}

void RSUniRenderVisitor::SetSurfaceGlobalDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node)
{
    RS_OPTIONAL_TRACE_FUNC();
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsMainWindowType()) {
            continue;
        }
        // set display dirty region to surfaceNode
        surfaceNode->SetGlobalDirtyRegion(node->GetDirtyManager()->GetDirtyRegion());
        surfaceNode->SetDirtyRegionAlignedEnable(false);
    }
    Occlusion::Region curVisibleDirtyRegion;
    for (auto& it : node->GetCurAllSurfaces()) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(it);
        if (surfaceNode == nullptr || !surfaceNode->IsMainWindowType()) {
            continue;
        }
        // set display dirty region to surfaceNode
        surfaceNode->SetDirtyRegionBelowCurrentLayer(curVisibleDirtyRegion);
        auto visibleDirtyRegion = surfaceNode->GetVisibleDirtyRegion();
        curVisibleDirtyRegion = curVisibleDirtyRegion.Or(visibleDirtyRegion);
    }
}

void RSUniRenderVisitor::SetSurfaceGlobalAlignedDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node,
    const Occlusion::Region alignedDirtyRegion)
{
    RS_TRACE_FUNC();
    if (!isDirtyRegionAlignedEnable_) {
        return;
    }
    // calculate extra dirty region after 32 bits alignment
    Occlusion::Region dirtyRegion = alignedDirtyRegion;
    auto globalRectI = node->GetDirtyManager()->GetDirtyRegion();
    Occlusion::Rect globalRect {globalRectI.left_, globalRectI.top_, globalRectI.GetRight(), globalRectI.GetBottom()};
    Occlusion::Region globalRegion{globalRect};
    dirtyRegion.SubSelf(globalRegion);
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsMainWindowType()) {
            continue;
        }
        surfaceNode->SetGlobalDirtyRegion(node->GetDirtyManager()->GetDirtyRegion());
        Occlusion::Region visibleRegion = surfaceNode->GetVisibleRegion();
        Occlusion::Region surfaceAlignedDirtyRegion = surfaceNode->GetAlignedVisibleDirtyRegion();
        if (dirtyRegion.IsEmpty()) {
            surfaceNode->SetExtraDirtyRegionAfterAlignment(dirtyRegion);
        } else {
            auto extraDirtyRegion = (dirtyRegion.Sub(surfaceAlignedDirtyRegion)).And(visibleRegion);
            surfaceNode->SetExtraDirtyRegionAfterAlignment(extraDirtyRegion);
        }
        surfaceNode->SetDirtyRegionAlignedEnable(true);
    }
    Occlusion::Region curVisibleDirtyRegion;
    for (auto& it : node->GetCurAllSurfaces()) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(it);
        if (surfaceNode == nullptr || !surfaceNode->IsMainWindowType()) {
            continue;
        }
        surfaceNode->SetDirtyRegionBelowCurrentLayer(curVisibleDirtyRegion);
        auto alignedVisibleDirtyRegion = surfaceNode->GetAlignedVisibleDirtyRegion();
        curVisibleDirtyRegion.OrSelf(alignedVisibleDirtyRegion);
    }
}

void RSUniRenderVisitor::AlignGlobalAndSurfaceDirtyRegions(std::shared_ptr<RSDisplayRenderNode>& node)
{
    node->GetDirtyManager()->UpdateDirtyByAligned();
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsMainWindowType()) {
            continue;
        }
        surfaceNode->GetDirtyManager()->UpdateDirtyByAligned();
    }
}

void RSUniRenderVisitor::CheckAndSetNodeCacheType(RSRenderNode& node)
{
    if (node.IsStaticCached()) {
        if (node.GetCacheType() != CacheType::CONTENT) {
            node.SetCacheType(CacheType::CONTENT);
            RSUniRenderUtil::ClearCacheSurface(node, threadIndex_);
        }

        if (!node.GetCompletedCacheSurface(threadIndex_, true) && UpdateCacheSurface(node)) {
            node.UpdateCompletedCacheSurface();
        }
    } else if (isDrawingCacheEnabled_ && GenerateNodeContentCache(node)) {
        UpdateCacheRenderNodeMapWithBlur(node);
    } else {
        if (node.GetCacheType() != CacheType::NONE) {
            node.SetCacheType(CacheType::NONE);
            if (node.GetCompletedCacheSurface(threadIndex_, false)) {
                RSUniRenderUtil::ClearCacheSurface(node, threadIndex_);
            }
        }
        // label this frame not run CheckAndSetNodeCacheType, means not run drawCacheWithBlur
        notRunCheckAndSetNodeCacheType_ = true;
    }
}

bool RSUniRenderVisitor::UpdateCacheSurface(RSRenderNode& node)
{
    RS_TRACE_NAME_FMT("UpdateCacheSurface: [%llu]", node.GetId());
    CacheType cacheType = node.GetCacheType();
    if (cacheType == CacheType::NONE) {
        return false;
    }

    if (!node.GetCacheSurface(threadIndex_, true)) {
        RSRenderNode::ClearCacheSurfaceFunc func = std::bind(&RSUniRenderUtil::ClearNodeCacheSurface,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        node.InitCacheSurface(canvas_ ? canvas_->GetGPUContext().get() : nullptr, func, threadIndex_);
    }
    auto surface = node.GetCacheSurface(threadIndex_, true);
    if (!surface) {
        RS_LOGE("Get CacheSurface failed");
        return false;
    }
    auto cacheCanvas = std::make_shared<RSPaintFilterCanvas>(surface.get());
    if (!cacheCanvas) {
        return false;
    }

    // copy current canvas properties into cacheCanvas
    if (renderEngine_) {
        cacheCanvas->SetHighContrast(renderEngine_->IsHighContrastEnabled());
    }
    if (canvas_) {
        cacheCanvas->CopyConfiguration(*canvas_);
    }
    // Using filter cache in multi-thread environment may cause GPU memory leak or invalid textures, so we explicitly
    // disable it in sub-thread.
    cacheCanvas->SetDisableFilterCache(isSubThread_);

    // When drawing CacheSurface, all child node should be drawn.
    // So set isOpDropped_ = false here.
    bool isOpDropped = isOpDropped_;
    isOpDropped_ = false;
    isUpdateCachedSurface_ = true;

    cacheCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);

    swap(cacheCanvas, canvas_);
    // When cacheType == CacheType::ANIMATE_PROPERTY,
    // we should draw AnimateProperty on cacheCanvas
    if (cacheType == CacheType::ANIMATE_PROPERTY) {
        if (node.GetRenderProperties().IsShadowValid()
            && !node.GetRenderProperties().IsSpherizeValid()) {
            canvas_->Save();
            canvas_->Translate(node.GetShadowRectOffsetX(), node.GetShadowRectOffsetY());
        }
        node.ProcessAnimatePropertyBeforeChildren(*canvas_);
    }
    if (node.IsNodeGroupIncludeProperty()) {
        node.ProcessAnimatePropertyBeforeChildren(*canvas_);
    }
    if (node.GetDrawingCacheType() != RSDrawingCacheType::DISABLED_CACHE) {
        node.SetDrawingCacheRootId(node.GetId());
    }

#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (RSSystemParameters::IsOpincRealDrawCacheEnable() && node.GetAutoCache()->isComputeDrawAreaSucc()) {
        auto& unionRect = node.GetAutoCache()->GetOpListUnionArea();
        canvas_->Translate(-unionRect.GetLeft(), -unionRect.GetTop());
    }
#endif

    node.ProcessRenderContents(*canvas_);
    // Set a count to label the ProcessChildren in updateCacheProcess
    updateCacheProcessCnt_++;
    ProcessChildren(node);
    updateCacheProcessCnt_--;

#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (RSSystemParameters::IsOpincRealDrawCacheEnable() && node.GetAutoCache()->isComputeDrawAreaSucc()) {
        auto& unionRect = node.GetAutoCache()->GetOpListUnionArea();
        canvas_->Translate(unionRect.GetLeft(), unionRect.GetTop());
    }
#endif

    if (cacheType == CacheType::ANIMATE_PROPERTY) {
        if (node.GetRenderProperties().IsShadowValid()
            && !node.GetRenderProperties().IsSpherizeValid()) {
            canvas_->Restore();
        }
        node.ProcessAnimatePropertyAfterChildren(*canvas_);
    }
    swap(cacheCanvas, canvas_);

    isUpdateCachedSurface_ = false;
    isOpDropped_ = isOpDropped;

    // To get all FreezeNode
    // execute: "param set rosen.dumpsurfacetype.enabled 2 && setenforce 0"
    // To get specific FreezeNode
    // execute: "param set rosen.dumpsurfacetype.enabled 1 && setenforce 0 && "
    // "param set rosen.dumpsurfaceid "NodeId" "
    // Png file could be found in /data
    RSBaseRenderUtil::WriteCacheRenderNodeToPng(node);
    return true;
}

void RSUniRenderVisitor::DrawSpherize(RSRenderNode& node)
{
    if (node.GetCacheType() != CacheType::ANIMATE_PROPERTY) {
        node.SetCacheType(CacheType::ANIMATE_PROPERTY);
        RSUniRenderUtil::ClearCacheSurface(node, threadIndex_);
    }
    if (!node.GetCompletedCacheSurface(threadIndex_, true) && UpdateCacheSurface(node)) {
        node.UpdateCompletedCacheSurface();
    }
    node.ProcessTransitionBeforeChildren(*canvas_);
    RSPropertiesPainter::DrawSpherize(
        node.GetRenderProperties(), *canvas_, node.GetCompletedCacheSurface(threadIndex_, true));
    node.ProcessTransitionAfterChildren(*canvas_);
}

bool RSUniRenderVisitor::DrawBlurInCache(RSRenderNode& node)
{
    if (LIKELY(curCacheFilterRects_.empty())) {
        return false;
    }
    if (curCacheFilterRects_.top().count(node.GetId())) {
        if (curGroupedNodes_.empty()) {
            // draw filter before drawing cached surface
            curCacheFilterRects_.top().erase(node.GetId());
            if (curCacheFilterRects_.empty() || !node.ChildHasVisibleFilter()) {
                // no filter to draw, return
                return true;
            }
        } else if (node.GetRenderProperties().GetShadowColorStrategy() !=
            SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE) {
            // clear hole while generating cache surface
            Drawing::AutoCanvasRestore arc(*canvas_.get(), true);
            RectI shadowRect;
            auto rrect = node.GetRenderProperties().GetRRect();
            RSPropertiesPainter::GetShadowDirtyRect(shadowRect, node.GetRenderProperties(), &rrect, false, false);
            std::shared_ptr<Drawing::CoreCanvasImpl> coreCanvas = canvas_->GetCanvasData();
            auto skiaCanvas = static_cast<Drawing::SkiaCanvas *>(coreCanvas.get());
            SkCanvasPriv::ResetClip(skiaCanvas->ExportSkCanvas());
            canvas_->ClipRect(Drawing::Rect(shadowRect.left_, shadowRect.top_,
                shadowRect.width_ + shadowRect.left_, shadowRect.height_ + shadowRect.top_));
            canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
        } else if (node.GetRenderProperties().GetBackgroundFilter() || node.GetRenderProperties().GetUseEffect()) {
            // clear hole while generating cache surface
            Drawing::AutoCanvasRestore arc(*canvas_, true);
            if (node.GetRenderProperties().GetClipBounds() != nullptr) {
                canvas_->ClipRect(RSPropertiesPainter::Rect2DrawingRect(node.GetRenderProperties().GetBoundsRect()),
                    Drawing::ClipOp::INTERSECT, false);
            } else {
                canvas_->ClipRoundRect(RSPropertiesPainter::RRect2DrawingRRect(node.GetRenderProperties().GetRRect()),
                    Drawing::ClipOp::INTERSECT, false);
            }
            canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
        }
    } else if (curGroupedNodes_.empty() && !node.ChildHasVisibleFilter()) {
        // no filter to draw, return
        return true;
    }
    return false;
}

void RSUniRenderVisitor::DrawChildCanvasRenderNode(RSRenderNode& node)
{
     if (node.GetCacheType() == CacheType::NONE) {
         if (node.IsPureContainer()) {
             processedPureContainerNode_++;
             node.ApplyBoundsGeometry(*canvas_);
             ProcessChildren(node);
             node.RSRenderNode::ProcessTransitionAfterChildren(*canvas_);
             return;
         } else if (node.IsContentNode()) {
             node.ApplyBoundsGeometry(*canvas_);
             node.ApplyAlpha(*canvas_);
             node.ProcessRenderContents(*canvas_);
             ProcessChildren(node);
             node.RSRenderNode::ProcessTransitionAfterChildren(*canvas_);
             return;
         }
    }
    DrawChildRenderNode(node);
}

void RSUniRenderVisitor::DrawChildRenderNode(RSRenderNode& node)
{
    CacheType cacheType = node.GetCacheType();
    node.ProcessTransitionBeforeChildren(*canvas_);
    switch (cacheType) {
        case CacheType::NONE: {
            auto preCache = canvas_->GetCacheType();
            if (node.HasCacheableAnim() && isDrawingCacheEnabled_) {
                canvas_->SetCacheType(RSPaintFilterCanvas::CacheType::ENABLED);
            }
            node.ProcessAnimatePropertyBeforeChildren(*canvas_);
            node.ProcessRenderContents(*canvas_);
            ProcessChildren(node);
            node.ProcessAnimatePropertyAfterChildren(*canvas_);
            if (node.HasCacheableAnim() && isDrawingCacheEnabled_) {
                canvas_->SetCacheType(preCache);
            }
            break;
        }
        case CacheType::CONTENT: {
            if (node.IsNodeGroupIncludeProperty()) {
                node.ProcessAnimatePropertyBeforeChildren(*canvas_, false);
            } else {
                node.ProcessAnimatePropertyBeforeChildren(*canvas_);
            }
            node.DrawCacheSurface(*canvas_, threadIndex_, false);
            node.ProcessAnimatePropertyAfterChildren(*canvas_);
            cacheRenderNodeMapRects_.push_back(node.GetOldDirtyInSurface());
            break;
        }
        case CacheType::ANIMATE_PROPERTY: {
            node.DrawCacheSurface(*canvas_, threadIndex_, false);
            break;
        }
        default:
            break;
    }
    node.ProcessTransitionAfterChildren(*canvas_);
}

bool RSUniRenderVisitor::CheckIfSurfaceRenderNodeNeedProcess(RSSurfaceRenderNode& node, bool& keepFilterCache)
{
    if (isSubThread_) {
        return true;
    }
    if (RSMainThread::Instance()->GetCacheCmdSkippedNodes().count(node.GetId()) != 0) {
        return true;
    }
    if (isSecurityDisplay_ && node.GetSkipLayer()) {
        RS_PROCESS_TRACE(isPhone_, node.GetName() + " SkipLayer Skip");
        return false;
    }
    if (!node.ShouldPaint()) {
        MarkSubHardwareEnableNodeState(node);
        RS_OPTIONAL_TRACE_NAME(node.GetName() + " Node should not paint Skip");
        RS_LOGD("RSUniRenderVisitor::IfSurfaceRenderNodeNeedProcess node: %{public}" PRIu64 " invisible",
            node.GetId());
        return false;
    }
    if (!node.GetOcclusionVisible() && isOcclusionEnabled_ && !isSecurityDisplay_) {
        MarkSubHardwareEnableNodeState(node);
        if (!node.GetVisibleRegionForCallBack().IsEmpty()) {
            keepFilterCache = true;
        }
        RS_PROCESS_TRACE(isPhone_, node.GetName() + " Occlusion Skip");
        return false;
    }
    if (node.IsAbilityComponent() && node.GetDstRect().IsEmpty() && curGroupedNodes_.empty()) {
        RS_PROCESS_TRACE(isPhone_, node.GetName() + " Empty AbilityComponent Skip");
        return false;
    }
    std::shared_ptr<RSSurfaceRenderNode> appNode;
    if (node.LeashWindowRelatedAppWindowOccluded(appNode) && !isSecurityDisplay_) {
        if (appNode != nullptr) {
            MarkSubHardwareEnableNodeState(*appNode);
        }
        RS_PROCESS_TRACE(isPhone_, node.GetName() + " App Occluded Leashwindow Skip");
        return false;
    }
    if (!screenInfo_.filteredAppSet.empty() && virtualScreenFilterAppRootId_ == INVALID_NODEID) {
        RS_PROCESS_TRACE(isPhone_, node.GetName() + " skip because it isn't filtered App");
        RS_LOGD("RSUniRenderVisitor::CheckIfSurfaceRenderNodeNeedProcess:\
            %{public}s skip because it isn't filtered App", node.GetName().c_str());
        return false;
    }
    return true;
}

bool RSUniRenderVisitor::IsRosenWebHardwareDisabled(RSSurfaceRenderNode& node, int rotation) const
{
    if (node.IsRosenWeb()) {
        return rotation == ROTATION_90 || rotation == ROTATION_270 ||
            RSUniRenderUtil::Is3DRotation(node.GetTotalMatrix()) ||
            node.GetDstRect().width_ > node.GetBuffer()->GetWidth() ||
            node.GetDstRect().height_ > node.GetBuffer()->GetHeight();
    }
    return false;
}

bool RSUniRenderVisitor::ForceHardwareComposer(RSSurfaceRenderNode& node) const
{
    auto bufferPixelFormat = node.GetBuffer()->GetFormat();
    return (bufferPixelFormat == GRAPHIC_PIXEL_FMT_RGBA_1010102 ||
         bufferPixelFormat == GRAPHIC_PIXEL_FMT_YCBCR_P010 ||
         bufferPixelFormat == GRAPHIC_PIXEL_FMT_YCRCB_P010) && !node.IsHardwareForcedDisabledByFilter() &&
         !isUpdateCachedSurface_;
}

bool RSUniRenderVisitor::UpdateSrcRectForHwcNode(RSSurfaceRenderNode& node)
{
    Drawing::AutoCanvasRestore acr(*canvas_.get(), true);

    if (displayNodeMatrix_.has_value()) {
        auto& displayNodeMatrix = displayNodeMatrix_.value();
        canvas_->ConcatMatrix(displayNodeMatrix);
    }
    node.SetTotalMatrix(canvas_->GetTotalMatrix());

    auto dstRect = node.GetDstRect();
    Drawing::RectI dst = { dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(),
                           dstRect.GetBottom() };
    bool hasRotation = false;
    if (node.GetConsumer() != nullptr) {
        auto rotation = RSBaseRenderUtil::GetRotateTransform(node.GetConsumer()->GetTransform());
        hasRotation = rotation == GRAPHIC_ROTATE_90 || rotation == GRAPHIC_ROTATE_270;
    }
    node.UpdateSrcRect(*canvas_, dst, hasRotation);
    return !node.IsHardwareDisabledBySrcRect();
}

void RSUniRenderVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    return;
#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (autoCacheEnable_) {
        node.GetAutoCache->OpIncStateInitWithSurface(nodeCacheType_, isDiscardSurface_, isSubThread_);
    }
#endif
    if (isUIFirst_ && isSubThread_) {
        bool isFirstLevelSurface = !RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.GetParent().lock());
        if (isSubSurfaceEnabled_) {
            isFirstLevelSurface = node.IsFirstLevelSurfaceNode();
        }
        if (auto parentNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(node.GetParent().lock()) ||
            (SceneBoardJudgement::IsSceneBoardEnabled() && (node.IsLeashWindow() || (node.IsAppWindow() &&
            isFirstLevelSurface)))) {
            UpdateCacheSurface(node);
            return;
        }
    }
    if (RSSystemProperties::GetProxyNodeDebugEnabled() && node.contextClipRect_.has_value() && canvas_ != nullptr) {
        // draw transparent red rect to indicate valid clip area
        {
            RSAutoCanvasRestore acr(canvas_);
            canvas_->ConcatMatrix(node.contextMatrix_.value_or(Drawing::Matrix()));
            Drawing::Brush brush;
            brush.SetARGB(0x80, 0xFF, 0, 0); // transparent red
            canvas_->AttachBrush(brush);
            canvas_->DrawRect(node.contextClipRect_.value());
            canvas_->DetachBrush();
        }
        // make this node context transparent
        canvas_->MultiplyAlpha(0.5);
    }
    RS_PROCESS_TRACE(isPhone_ || (!isSubThread_ && node.IsMainThreadNode()),
        "RSUniRender::Process:[" + node.GetName() + "] " +
        node.GetDstRect().ToString() + " Alpha: " + std::to_string(node.GetGlobalAlpha()));
    RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode node:%{public}" PRIu64 ",child size:%{public}u,"
        "name:%{public}s,OcclusionVisible:%{public}d",
        node.GetId(), node.GetChildrenCount(), node.GetName().c_str(), node.GetOcclusionVisible());
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
#ifdef NEW_RENDER_CONTEXT
    Drawing::GPUContext* gpuContext = renderEngine_->GetRenderContext()->GetDrGPUContext();
    RSTagTracker tagTracker(gpuContext, node.GetId(), RSTagTracker::TAGTYPE::TAG_DRAW_SURFACENODE);
    node.SetDrawingGPUContext(gpuContext);
#else
    std::shared_ptr<Drawing::GPUContext> gpuContext = canvas_ != nullptr ? canvas_->GetGPUContext() : nullptr;
    RSTagTracker tagTracker(gpuContext.get(), node.GetId(), RSTagTracker::TAGTYPE::TAG_DRAW_SURFACENODE);
    node.SetDrawingGPUContext(gpuContext.get());
#endif
#endif
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        node.SetDrawingGPUContext(renderEngine_->GetSkContext().get());
    }
#endif
    bool keepFilterCache = false;
    if (!CheckIfSurfaceRenderNodeNeedProcess(node, keepFilterCache)) {
        if (!keepFilterCache) {
            node.UpdateFilterCacheStatusWithVisible(false);
        }
        ProcessSubSurfaceNodes(node);
        return;
    } else {
        node.UpdateFilterCacheStatusWithVisible(true);
    }
    if (node.IsMainWindowType()) {
        curSurfaceNode_ = node.ReinterpretCastTo<RSSurfaceRenderNode>();
        CollectAppNodeForHwc(curSurfaceNode_);
    }
    // skip clean surface node
    if (isOpDropped_ && node.IsAppWindow() &&
        !node.SubNodeNeedDraw(node.GetOldDirtyInSurface(), partialRenderType_)) {
        RS_PROCESS_TRACE(isPhone_, node.GetName() + " QuickReject Skip");
        RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode skip: %{public}s", node.GetName().c_str());
        ProcessSubSurfaceNodes(node);
        return;
    }
    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }
    const auto& property = node.GetRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());
    if (!geoPtr) {
        RS_LOGE("RSUniRenderVisitor::ProcessSurfaceRenderNode node:%{public}" PRIu64 ", get geoPtr failed",
            node.GetId());
        return;
    }

    // when display is in rotation state, occlusion relationship will be ruined,
    // hence visibleRegions cannot be used.
    if (isOpDropped_ && node.IsAppWindow()) {
        const auto& visibleRegions = node.GetVisibleRegion().GetRegionRects();
        if (visibleRegions.size() == 1) {
            auto visibleRect = Drawing::Rect(
                visibleRegions[0].left_, visibleRegions[0].top_, visibleRegions[0].right_, visibleRegions[0].bottom_);
            Drawing::Matrix inverse;
            if (displayNodeMatrix_.has_value() && displayNodeMatrix_.value().Invert(inverse)) {
                inverse.MapRect(visibleRect, visibleRect);
            }
            canvas_->SetVisibleRect(visibleRect);
        }
    }

    // when surfacenode named "CapsuleWindow", cache the current canvas as SkImage for screen recording
    if (!isSecurityDisplay_ && canvas_->GetSurface() != nullptr &&
        node.GetName().find(CAPTURE_WINDOW_NAME) != std::string::npos) {
        resetRotate_ = CheckIfNeedResetRotate();
        cacheImgForCapture_ = canvas_->GetSurface()->GetImageSnapshot();
        auto mirrorNode = curDisplayNode_->GetMirrorSource().lock() ?
            curDisplayNode_->GetMirrorSource().lock() : curDisplayNode_;
        mirrorNode->SetRootIdOfCaptureWindow(FindInstanceChildOfDisplay(node.GetParent().lock()));
    }

    RSAutoCanvasRestore acr(canvas_);
    auto bgAntiAliasState = RSPropertiesPainter::GetBgAntiAlias();
    if (doAnimate_ && (!ROSEN_EQ(geoPtr->GetScaleX(), 1.f) || !ROSEN_EQ(geoPtr->GetScaleY(), 1.f))) {
        // disable background antialias when surfacenode has scale animation
        RSPropertiesPainter::SetBgAntiAlias(false);
    }

    canvas_->MultiplyAlpha(property.GetAlpha());

    bool isSelfDrawingSurface = node.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE;
    // [planning] surfaceNode use frame instead
    // This is for SELF_DRAWING_NODE like RosenRenderTexture
    // BoundsRect of RosenRenderTexture is the size of video, not the size of the component.
    // The size of RosenRenderTexture is the paintRect (always be set to FrameRect) which is not passed to RenderNode
    // because RSSurfaceRenderNode is designed only affected by BoundsRect.
    // When RosenRenderTexture has child node, child node is layouted
    // according to paintRect of RosenRenderTexture, not the BoundsRect.
    // So when draw SELF_DRAWING_NODE, we should save canvas
    // to avoid child node being layout according to the BoundsRect of RosenRenderTexture.
    // Temporarily, we use parent of SELF_DRAWING_NODE which has the same paintRect with its child instead.
    // to draw child node of SELF_DRAWING_NODE
    if (isSelfDrawingSurface && !property.IsSpherizeValid()) {
        canvas_->Save();
    }

    canvas_->ConcatMatrix(geoPtr->GetMatrix());
    isNodeSingleFrameComposer_ = node.GetNodeIsSingleFrameComposer();
    bool isSubNodeOfSurfaceInProcess = isSubNodeOfSurfaceInProcess_;
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        isSubNodeOfSurfaceInProcess_ = true;
    }
    if (property.IsSpherizeValid()) {
        DrawSpherize(node);
    } else {
        if (isSelfDrawingSurface) {
            RSUniRenderUtil::FloorTransXYInCanvasMatrix(*canvas_);
        }
        if (isUIFirst_ && node.GetCacheType() == CacheType::ANIMATE_PROPERTY) {
            RSUniRenderUtil::HandleSubThreadNode(node, *canvas_);
            if (node.IsMainWindowType() || node.IsLeashWindow()) {
                isSubNodeOfSurfaceInProcess_ = isSubNodeOfSurfaceInProcess;
            }
            return;
        }
        node.ProcessRenderBeforeChildren(*canvas_);
        if (isUIFirst_ && RSUniRenderUtil::HandleSubThreadNode(node, *canvas_)) {
            node.ProcessRenderAfterChildren(*canvas_);
            if (node.IsMainWindowType() || node.IsLeashWindow()) {
                isSubNodeOfSurfaceInProcess_ = isSubNodeOfSurfaceInProcess;
            }
            return;
        }
        if (node.GetBuffer() != nullptr) {
            int rotation = RSUniRenderUtil::GetRotationFromMatrix(node.GetTotalMatrix());
            if (node.IsHardwareEnabledType()) {
                // since node has buffer, hwc disabledState could be reset by filter or surface cached
                bool backgroundTransparent =
                    static_cast<uint8_t>(node.GetRenderProperties().GetBackgroundColor().GetAlpha()) < UINT8_MAX;
                node.SetHardwareForcedDisabledState(
                    (node.IsHardwareForcedDisabledByFilter() || canvas_->GetAlpha() < 1.f ||
                    backgroundTransparent || IsRosenWebHardwareDisabled(node, rotation) ||
                    RSUniRenderUtil::GetRotationDegreeFromMatrix(node.GetTotalMatrix()) % ROTATION_90 != 0 ||
                    canvas_->GetBlendOffscreenLayerCnt() > 0) &&
                    (!node.IsHardwareEnabledTopSurface() || node.HasSubNodeShouldPaint()));
                node.SetHardwareDisabledByCache(isUpdateCachedSurface_);
                node.ResetHardwareForcedDisabledBySrcRect();
                RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: IsHardwareEnabledType:%d backgroundTransparent:%d "
                    "DisabledByFilter:%d alpha:%.2f RosenWebHardwareDisabled:%d rotation:%d "
                    "isUpdateCachedSurface_:%d IsHardwareComposerEnabled:%d node.IsHardwareForcedDisabled():%d",
                    node.IsHardwareEnabledType(), backgroundTransparent,
                    node.IsHardwareForcedDisabledByFilter(), canvas_->GetAlpha(),
                    IsRosenWebHardwareDisabled(node, rotation),
                    RSUniRenderUtil::GetRotationDegreeFromMatrix(node.GetTotalMatrix()), isUpdateCachedSurface_,
                    IsHardwareComposerEnabled(), node.IsHardwareForcedDisabled());
            }
            // if this window is in freeze state, disable hardware composer for its child surfaceView
            if (IsHardwareComposerEnabled() && node.IsHardwareEnabledType() && (!node.IsHardwareForcedDisabled() ||
                ForceHardwareComposer(node)) && UpdateSrcRectForHwcNode(node)) {
                if (!node.IsHardwareEnabledTopSurface()) {
                    canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
                }
                node.SetGlobalAlpha(canvas_->GetAlpha());
                RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode src:%{public}s, dst:%{public}s name:%{public}s"
                    " id:%{public}" PRIu64 "", node.GetSrcRect().ToString().c_str(),
                    node.GetDstRect().ToString().c_str(), node.GetName().c_str(), node.GetId());
            } else {
                node.SetGlobalAlpha(1.0f);
                auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false, threadIndex_);
                params.targetColorGamut = newColorSpace_;
#ifdef USE_VIDEO_PROCESSING_ENGINE
                auto screenManager = CreateOrGetScreenManager();
                std::shared_ptr<RSDisplayRenderNode> ancestor = nullptr;
                if (node.GetAncestorDisplayNode().lock() != nullptr) {
                    ancestor = node.GetAncestorDisplayNode().lock()->ReinterpretCastTo<RSDisplayRenderNode>();
                }
                if (ancestor != nullptr) {
                    params.screenBrightnessNits = screenManager->GetScreenBrightnessNits(ancestor->GetScreenId());
                }
#endif
                auto bgColor = property.GetBackgroundColor();
                if ((node.GetSelfDrawingNodeType() != SelfDrawingNodeType::VIDEO) &&
                    (bgColor != RgbPalette::Transparent())) {
                    auto bounds = RSPropertiesPainter::Rect2DrawingRect(property.GetBoundsRect());
                    Drawing::SaveLayerOps layerOps(&bounds, nullptr);
                    canvas_->SaveLayer(layerOps);
                    Drawing::Brush brush;
                    brush.SetColor(Drawing::Color(bgColor.AsArgbInt()));
                    canvas_->AttachBrush(brush);
                    canvas_->DrawRoundRect(RSPropertiesPainter::RRect2DrawingRRect(property.GetRRect()));
                    canvas_->DetachBrush();
                    renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
                    canvas_->Restore();
                } else {
                    renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
                }
            }
        }

        if (isSelfDrawingSurface) {
            canvas_->Restore();
        }

        if (node.IsAppWindow()) {
            if (!node.IsNotifyUIBufferAvailable() && IsFirstFrameReadyToDraw(node)) {
                node.NotifyUIBufferAvailable();
            }
            CheckAndSetNodeCacheType(node);
            DrawChildRenderNode(node);
        } else if (node.IsScbScreen() && !node.IsNotifyUIBufferAvailable() && IsFirstFrameReadyToDraw(node)) {
            node.NotifyUIBufferAvailable();
            ProcessChildren(node);
        } else {
            ProcessChildren(node);
        }
        node.ProcessRenderAfterChildren(*canvas_);
    }

    RSPropertiesPainter::SetBgAntiAlias(bgAntiAliasState);
    if (node.IsAppWindow()) {
        canvas_->SetVisibleRect(Drawing::Rect(0, 0, 0, 0));

        // count processed canvas node
        RS_TRACE_NAME_FMT("%s PureContainerNode/ProcessedNodes: %u/%u", node.GetName().c_str(),
            processedPureContainerNode_, processedCanvasNodeInCurrentSurface_);
        processedCanvasNodeInCurrentSurface_ = 0; // reset
        processedPureContainerNode_ = 0;
    }
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        isSubNodeOfSurfaceInProcess_ = isSubNodeOfSurfaceInProcess;
        // release full children list used by sub thread
    }
}

void RSUniRenderVisitor::ProcessProxyRenderNode(RSProxyRenderNode& node)
{
    if (RSSystemProperties::GetProxyNodeDebugEnabled() && node.contextClipRect_.has_value() &&
        node.target_.lock() != nullptr) {
        // draw transparent green rect to indicate clip area of proxy node
        Drawing::Brush brush;
        brush.SetARGB(0x80, 0, 0xFF, 0); // transparent green
        canvas_->AttachBrush(brush);
        canvas_->DrawRect(node.contextClipRect_.value());
        canvas_->DetachBrush();
    }
    ProcessChildren(node);
}

void RSUniRenderVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    RS_LOGD("RSUniRenderVisitor::ProcessRootRenderNode node: %{public}" PRIu64 ", child size:%{public}u", node.GetId(),
        node.GetChildrenCount());
    if (!node.ShouldPaint()) {
        RS_LOGD("RSUniRenderVisitor::ProcessRootRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessRootRenderNode, canvas is nullptr");
        return;
    }
    int saveCount = canvas_->Save();
    ProcessCanvasRenderNode(node);
    canvas_->RestoreToCount(saveCount);
}

bool RSUniRenderVisitor::GenerateNodeContentCache(RSRenderNode& node)
{
    // Node cannot have cache.
    uint32_t cacheRenderNodeMapCnt;
    if (node.GetDrawingCacheType() == RSDrawingCacheType::DISABLED_CACHE) {
        {
            std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
            cacheRenderNodeMapCnt = cacheRenderNodeMap.count(node.GetId());
        }
        if (cacheRenderNodeMapCnt > 0) {
            node.SetCacheType(CacheType::NONE);
            RSUniRenderUtil::ClearCacheSurface(node, threadIndex_);
            {
                std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
                cacheRenderNodeMap.erase(node.GetId());
            }
        }
        return false;
    }

    // The node goes down the tree to clear the cache.
    {
        std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
        cacheRenderNodeMapCnt = cacheRenderNodeMap.count(node.GetId());
    }
    if (node.GetCacheType() == CacheType::NONE && cacheRenderNodeMapCnt > 0) {
        {
            std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
            cacheRenderNodeMap.erase(node.GetId());
        }
    }
    return true;
}

void RSUniRenderVisitor::ClearRenderGroupCache()
{
    std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
    cacheRenderNodeMap.clear();
}

bool RSUniRenderVisitor::InitNodeCache(RSRenderNode& node)
{
    if (node.GetDrawingCacheType() == RSDrawingCacheType::FORCED_CACHE ||
        node.GetDrawingCacheType() == RSDrawingCacheType::TARGETED_CACHE) {
        uint32_t cacheRenderNodeMapCnt;
        {
            std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
            cacheRenderNodeMapCnt = cacheRenderNodeMap.count(node.GetId());
        }
        if (cacheRenderNodeMapCnt == 0 || (!node.IsStaticCached() && node.NeedInitCacheCompletedSurface())) {
            RenderParam val { node.shared_from_this(), canvas_->GetCanvasStatus() };
            curGroupedNodes_.push(val);
            node.SetCacheType(CacheType::CONTENT);
            RSUniRenderUtil::ClearCacheSurface(node, threadIndex_);
            if (UpdateCacheSurface(node)) {
                node.UpdateCompletedCacheSurface();
                ChangeCacheRenderNodeMap(node);
                cacheReuseTimes = 0;
                node.ResetDrawingCacheNeedUpdate();
            }
            curGroupedNodes_.pop();
            return true;
        }
    }
    return false;
}

void RSUniRenderVisitor::ChangeCacheRenderNodeMap(RSRenderNode& node, const uint32_t count)
{
    std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
    cacheRenderNodeMap[node.GetId()] = count;
}

void RSUniRenderVisitor::UpdateCacheRenderNodeMapWithBlur(RSRenderNode& node)
{
    curCacheFilterRects_.push(allCacheFilterRects_[node.GetId()]);
    auto canvasType = canvas_->GetCacheType();
    canvas_->SetCacheType(RSPaintFilterCanvas::CacheType::OFFSCREEN);
    UpdateCacheRenderNodeMap(node);
    canvas_->SetCacheType(canvasType);
    RS_TRACE_NAME_FMT("Draw cache with blur [%llu]", node.GetId());
    Drawing::AutoCanvasRestore arc(*canvas_, true);
    auto nodeType = node.GetCacheType();
    node.SetCacheType(CacheType::NONE);
    bool isOpDropped = isOpDropped_;
    isOpDropped_ = false;
    // Label the ProcessChildren in drawCacheWithBlur
    drawCacheWithBlur_ = true;
    DrawChildRenderNode(node);
    isOpDropped_ = isOpDropped;
    drawCacheWithBlur_ = false;
    node.SetCacheType(nodeType);
    curCacheFilterRects_.pop();
}

void RSUniRenderVisitor::UpdateCacheRenderNodeMap(RSRenderNode& node)
{
    if (InitNodeCache(node)) {
        RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderVisitor::UpdateCacheRenderNodeMap, generate the node cache for the first"
            "time, NodeId: %" PRIu64 " ", node.GetId());
        return;
    }
    uint32_t updateTimes = 0;
    if (node.GetDrawingCacheType() == RSDrawingCacheType::FORCED_CACHE) {
        // Regardless of the number of consecutive refreshes, the current cache is forced to be updated.
        if (node.GetDrawingCacheChanged()) {
            RenderParam val { node.shared_from_this(), canvas_->GetCanvasStatus() };
            curGroupedNodes_.push(val);
            {
                std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
                updateTimes = cacheRenderNodeMap[node.GetId()] + 1;
            }
            node.SetCacheType(CacheType::CONTENT);
            if (UpdateCacheSurface(node)) {
                node.UpdateCompletedCacheSurface();
                ChangeCacheRenderNodeMap(node, updateTimes);
                cacheReuseTimes = 0;
                node.ResetDrawingCacheNeedUpdate();
            }
            curGroupedNodes_.pop();
            return;
        }
    }
    if (node.GetDrawingCacheType() == RSDrawingCacheType::TARGETED_CACHE) {
        // If the number of consecutive refreshes exceeds CACHE_MAX_UPDATE_TIME times, the cache is cleaned,
        // otherwise the cache is updated.
        if (node.GetDrawingCacheChanged()) {
            {
                std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
                updateTimes = cacheRenderNodeMap[node.GetId()] + 1;
            }
            if (updateTimes >= CACHE_MAX_UPDATE_TIME) {
                node.SetCacheType(CacheType::NONE);
                node.MarkNodeGroup(RSRenderNode::GROUPED_BY_UI, false, false);
                node.MarkNodeGroup(RSRenderNode::GROUPED_BY_ANIM, false, false);
                RSUniRenderUtil::ClearCacheSurface(node, threadIndex_);
                cacheRenderNodeMap.erase(node.GetId());
                cacheReuseTimes = 0;
                return;
            }
            node.SetCacheType(CacheType::CONTENT);
            if (UpdateCacheSurface(node)) {
                node.UpdateCompletedCacheSurface();
                ChangeCacheRenderNodeMap(node, updateTimes);
                cacheReuseTimes = 0;
                node.ResetDrawingCacheNeedUpdate();
            }
            curGroupedNodes_.pop();
            return;
        }
    }
    // The cache is not refreshed continuously.
    ChangeCacheRenderNodeMap(node);
    cacheReuseTimes++;
    RS_OPTIONAL_TRACE_NAME("RSUniRenderVisitor::UpdateCacheRenderNodeMap ,NodeId: " + std::to_string(node.GetId()) +
        " ,CacheRenderNodeMapCnt: " + std::to_string(cacheReuseTimes));
}

void RSUniRenderVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
#ifdef DDGR_ENABLE_FEATURE_OP_INC_DFX
    RS_TRACE_NAME_FMT("process_canvas %.2f, %s", canvas_ ? canvas_->GetAlpha() : -1.0f,
        node.GetAutoCache()->GetNodeDebugInfo().c_str());
#endif
    processedCanvasNodeInCurrentSurface_++;
    if (!node.ShouldPaint() ||
        (canvas_ && canvas_->GetDeviceClipBounds().IsEmpty() && hardwareEnabledNodes_.empty())) {
#ifdef DDGR_ENABLE_FEATURE_OPINC
        if (isOpincDropNodeExt_) {
            return;
        }
#else
        return;
#endif
    }
    auto geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
    if (isSkipCanvasNodeOutOfScreen_ && !isSubNodeOfSurfaceInProcess_ && !node.HasSubSurface() &&
        geoPtr && IsOutOfScreenRegion(geoPtr->GetAbsRect()) && !isSubThread_) {
#ifdef DDGR_ENABLE_FEATURE_OPINC
        if (isOpincDropNodeExt_) {
            return;
        }
#else
        return;
#endif
    }
    node.MarkNodeSingleFrameComposer(isNodeSingleFrameComposer_);

#ifdef RS_ENABLE_STACK_CULLING
    if (RSSystemProperties::GetViewOcclusionCullingEnabled()) {
        if (node.isCoveredByOtherNode_ && isSubNodeOfSurfaceInProcess_) {
            RS_LOGD("%lu [%d %d %d] is coverd by other node SKIP, curSurface %lu %s. %s",
                node.GetId(), node.isFullSurfaceOpaquCanvasNode_, node.hasChildFullSurfaceOpaquCanvasNode_,
                node.isCoveredByOtherNode_, (curSurfaceNode_ ? curSurfaceNode_->GetId() : 0),
                (curSurfaceNode_ ? curSurfaceNode_->GetName() : "N/A"),
                (node.GetRenderProperties().GetBoundsGeometry()->GetAbsRect()).ToString().c_str());
            RS_OPTIONAL_TRACE_NAME_FMT("%lu [%d %d %d] is coverd by other node SKIP, curSurface %lu %s. %s",
                node.GetId(), node.isFullSurfaceOpaquCanvasNode_, node.hasChildFullSurfaceOpaquCanvasNode_,
                node.isCoveredByOtherNode_, (curSurfaceNode_ ? curSurfaceNode_->GetId() : 0),
                (curSurfaceNode_ ? curSurfaceNode_->GetName() : "N/A"),
                (node.GetRenderProperties().GetBoundsGeometry()->GetAbsRect()).ToString().c_str());

            node.isCoveredByOtherNode_ = false;
#ifdef DDGR_ENABLE_FEATURE_OPINC
            if (isOpincDropNodeExt_) {
                return;
            }
#else
            return;
#endif
        }
    }
#endif

    if ((isOpDropped_ && (curSurfaceNode_ != nullptr)) || isCanvasNodeSkipDfxEnabled_) {
        // If all the child nodes have drawing areas that do not exceed the current node, then current node
        // can be directly skipped if not intersect with any dirtyregion.
        // Otherwise, its childrenRect_ should be considered.
        RectI dirtyRect = node.HasChildrenOutOfRect() ?
            node.GetOldDirtyInSurface().JoinRect(node.GetChildrenRect()) : node.GetOldDirtyInSurface();
        if (isSubNodeOfSurfaceInProcess_ && !node.IsAncestorDirty() &&
            !curSurfaceNode_->SubNodeNeedDraw(dirtyRect, partialRenderType_) && !node.IsParentLeashWindow() &&
            !node.IsParentScbScreen()) {
            bool subSurfaceNeedDraw = false;
            if (isSubSurfaceEnabled_) {
                subSurfaceNeedDraw = node.SubSurfaceNodeNeedDraw(partialRenderType_);
            }
            if (!subSurfaceNeedDraw) {
                if (isCanvasNodeSkipDfxEnabled_) {
                    curSurfaceNode_->GetDirtyManager()->UpdateDirtyRegionInfoForDfx(
                        node.GetId(), node.GetType(), DirtyRegionType::CANVAS_NODE_SKIP_RECT, dirtyRect);
                } else {
#ifdef DDGR_ENABLE_FEATURE_OPINC
                    if (isOpincDropNodeExt_) {
                        return;
                    }
#else
                    return;
#endif
                }
            }
        }
    }
    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
#if defined(RS_ENABLE_DRIVEN_RENDER)
    // clip hole for driven render
    if (drivenInfo_ && !drivenInfo_->prepareInfo.hasInvalidScene &&
        drivenInfo_->currDrivenRenderMode != DrivenUniRenderMode::RENDER_WITH_NORMAL) {
        // skip render driven node sub tree
        if (RSDrivenRenderManager::GetInstance().ClipHoleForDrivenNode(*canvas_, node)) {
#ifdef DDGR_ENABLE_FEATURE_OPINC
            if (isOpincDropNodeExt_) {
                return;
            }
#else
            return;
#endif
        }
    }
#endif
#ifdef DDGR_ENABLE_FEATURE_OPINC
    bool isProcessAutoCache = autoCacheEnable_ && !isDiscardSurface_ && node->GetAutoCache()->IsOpincUnchangeNode();
    if (isProcessAutoCache) {
        if (node->GetAutoCache()->BeforeDrawCache(nodeCacheType_, autoCacheRenderNodeInfos_,
            autoCacheDrawingEnable_, isDiscardSurface_, canvas_, screenInfo_.width, screenInfo_.height)) {
            return;
        }
    }
#endif
    // in case preparation'update is skipped
    canvas_->Save();
    if (node.GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        RSUniRenderUtil::FloorTransXYInCanvasMatrix(*canvas_);
    }

    if (node.GetSharedTransitionParam()) {
        // draw self and children in sandbox which will not be affected by parent's transition
        const auto& sandboxMatrix = node.GetRenderProperties().GetSandBoxMatrix();
        if (sandboxMatrix) {
            canvas_->SetMatrix(*sandboxMatrix);
        }
    }
    if (node.GetRenderProperties().GetUseEffect() && RSSystemParameters::GetDrawingEffectRegionEnabledDfx()) {
        const auto& effectData = canvas_->GetEffectData();
        if ((effectData == nullptr || effectData->cachedImage_ == nullptr) ||
            !RSSystemProperties::GetEffectMergeEnabled()) {
            nodesUseEffectFallbackForDfx_.emplace_back(geoPtr->GetAbsRect());
        } else {
            nodesUseEffectForDfx_.emplace_back(geoPtr->GetAbsRect());
        }
    }
    const auto& property = node.GetRenderProperties();
    if (property.IsSpherizeValid()) {
        DrawSpherize(node);
#ifdef DDGR_ENABLE_FEATURE_OPINC
        if (isProcessAutoCache) {
            node->GetAutoCache()->AfterDrawCache(nodeCacheType_);
        }
#endif
        return;
    }
    if (auto drawingNode = node.ReinterpretCastTo<RSCanvasDrawingRenderNode>()) {
        std::lock_guard<std::mutex> lock(drawingMutex_);
        auto clearFunc = [id = threadIndex_](std::shared_ptr<Drawing::Surface> surface) {
            // The second param is null, 0 is an invalid value.
            RSUniRenderUtil::ClearNodeCacheSurface(std::move(surface), nullptr, id, 0);
        };
        auto threadId = RSSubThreadManager::Instance()->GetReThreadIndexMap()[threadIndex_];
        drawingNode->SetSurfaceClearFunc({ threadIndex_, clearFunc }, threadId);
    }
    CheckAndSetNodeCacheType(node);
#ifdef DDGR_ENABLE_FEATURE_OPINC
        if (isProcessAutoCache) {
            node->GetAutoCache()->OpincCalculateBefore(canvas_, isOpincDropNodeExt_);
        }
#endif
    DrawChildCanvasRenderNode(node);
#ifdef DDGR_ENABLE_FEATURE_OPINC
        if (isProcessAutoCache) {
            node->GetAutoCache()->OpincCalculateAfter(canvas_, isOpincDropNodeExt_);
        }
#endif
    notRunCheckAndSetNodeCacheType_ = false;
    canvas_->Restore();
#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (isProcessAutoCache) {
        node->GetAutoCache()->AfterDrawCache(canvas_, isDiscardSurface_, nodeCacheType_);
    }
#endif
}

void RSUniRenderVisitor::ProcessEffectRenderNode(RSEffectRenderNode& node)
{
    if (!node.ShouldPaint()) {
        RS_LOGD("RSUniRenderVisitor::ProcessEffectRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessEffectRenderNode, canvas is nullptr");
        return;
    }
    nodesUseEffectForDfx_.clear();
    nodesUseEffectFallbackForDfx_.clear();
    Drawing::AutoCanvasRestore acr(*canvas_.get(), true);
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessChildren(node);
    node.ProcessRenderAfterChildren(*canvas_);
    effectNodeMapForDfx_[node.GetId()].first = nodesUseEffectForDfx_;
    effectNodeMapForDfx_[node.GetId()].second = nodesUseEffectFallbackForDfx_;
}

void RSUniRenderVisitor::PrepareOffscreenRender(RSRenderNode& node)
{
    RS_TRACE_NAME("PrepareOffscreenRender");
#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (autoCacheEnable_) {
        node->GetAutoCache()->DisableAutoCacheWithForce(unchangeMark_);
    }
#endif
    // cleanup
    canvasBackup_ = nullptr;
    offscreenSurface_ = nullptr;
    // check offscreen size and hardware renderer
    int32_t offscreenWidth = node.GetRenderProperties().GetFrameWidth();
    int32_t offscreenHeight = node.GetRenderProperties().GetFrameHeight();
    if (offscreenWidth <= 0 || offscreenHeight <= 0) {
        RS_LOGD("RSUniRenderVisitor::PrepareOffscreenRender, offscreenWidth or offscreenHeight is invalid");
        return;
    }
    if (canvas_->GetSurface() == nullptr) {
        canvas_->ClipRect(Drawing::Rect(0, 0, offscreenWidth, offscreenHeight), Drawing::ClipOp::INTERSECT, false);
        RS_LOGD("RSUniRenderVisitor::PrepareOffscreenRender, current surface is nullptr (software renderer?)");
        return;
    }
    // create offscreen surface and canvas
    offscreenSurface_ = canvas_->GetSurface()->MakeSurface(offscreenWidth, offscreenHeight);
    if (offscreenSurface_ == nullptr) {
        RS_LOGD("RSUniRenderVisitor::PrepareOffscreenRender, offscreenSurface is nullptr");
        canvas_->ClipRect(Drawing::Rect(0, 0, offscreenWidth, offscreenHeight), Drawing::ClipOp::INTERSECT, false);
        return;
    }
    auto offscreenCanvas = std::make_shared<RSPaintFilterCanvas>(offscreenSurface_.get());

    // copy current canvas properties into offscreen canvas
    offscreenCanvas->CopyConfiguration(*canvas_);

    // backup current canvas and replace with offscreen canvas
    canvasBackup_ = std::exchange(canvas_, offscreenCanvas);
}

void RSUniRenderVisitor::FinishOffscreenRender(bool isMirror)
{
    if (canvasBackup_ == nullptr) {
        RS_LOGD("RSUniRenderVisitor::FinishOffscreenRender, canvasBackup_ is nullptr");
        return;
    }
    RS_TRACE_NAME("RSUniRenderVisitor::OffscreenRender finish");
    // flush offscreen canvas, maybe unnecessary
    if (!isMirror) {
        canvas_->Flush();
    }
    // draw offscreen surface to current canvas
    Drawing::Brush paint;
    paint.SetAntiAlias(true);
    canvasBackup_->AttachBrush(paint);
    Drawing::SamplingOptions sampling =
        Drawing::SamplingOptions(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NEAREST);
    canvasBackup_->DrawImage(*offscreenSurface_->GetImageSnapshot().get(), 0, 0, sampling);
    canvasBackup_->DetachBrush();
    // restore current canvas and cleanup
    offscreenSurface_ = nullptr;
    canvas_ = std::move(canvasBackup_);
}

void RSUniRenderVisitor::StartOverDraw()
{
    if (!RSOverdrawController::GetInstance().IsEnabled()) {
        return;
    }
    auto gpuContext = canvas_->GetGPUContext();
    if (gpuContext == nullptr) {
        RS_LOGE("RSUniRenderVisitor::StartOverDraw failed: need gpu canvas");
        return;
    }

    auto width = canvas_->GetWidth();
    auto height = canvas_->GetHeight();
    Drawing::ImageInfo info =
        Drawing::ImageInfo { width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    overdrawSurface_ = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, info);
    if (!overdrawSurface_) {
        RS_LOGE("RSUniRenderVisitor::StartOverDraw failed: surface is nullptr");
        return;
    }
    overdrawCanvas_ = std::make_shared<Drawing::OverDrawCanvas>(overdrawSurface_->GetCanvas());
    canvas_->AddCanvas(overdrawCanvas_.get());
}

void RSUniRenderVisitor::FinishOverDraw()
{
    if (!RSOverdrawController::GetInstance().IsEnabled()) {
        return;
    }
    if (!overdrawSurface_) {
        RS_LOGE("RSRenderThreadVisitor::FinishOverDraw overdrawSurface is nullptr");
        return;
    }
    auto image = overdrawSurface_->GetImageSnapshot();
    if (image == nullptr) {
        RS_LOGE("RSRenderThreadVisitor::FinishOverDraw image is nullptr");
        return;
    }
    Drawing::Brush brush;
    auto overdrawColors = RSOverdrawController::GetInstance().GetColorArray();
    auto colorFilter = Drawing::ColorFilter::CreateOverDrawColorFilter(overdrawColors.data());
    Drawing::Filter filter;
    filter.SetColorFilter(colorFilter);
    brush.SetFilter(filter);
    canvas_->AttachBrush(brush);
    canvas_->DrawImage(*image, 0, 0, Drawing::SamplingOptions());
    canvas_->DetachBrush();
    overdrawSurface_ = nullptr;
    overdrawCanvas_ = nullptr;
}

void RSUniRenderVisitor::ClosePartialRenderWhenAnimatingWindows(std::shared_ptr<RSDisplayRenderNode>& node)
{
    if (!doAnimate_) {
        return;
    }
    if (appWindowNum_ > PHONE_MAX_APP_WINDOW_NUM) {
        node->GetDirtyManager()->MergeSurfaceRect();
    } else {
        isPartialRenderEnabled_ = false;
        isOpDropped_ = false;
        RS_TRACE_NAME("ClosePartialRender 0 Window Animation");
    }
}

void RSUniRenderVisitor::SetUniRenderThreadParam(std::unique_ptr<RSRenderThreadParams>& renderThreadParams)
{
    if (!renderThreadParams) {
        RS_LOGE("RSUniRenderVisitor::SetUniRenderThreadParam renderThreadParams is nullptr");
        return;
    }
    renderThreadParams->isPartialRenderEnabled_ = isPartialRenderEnabled_;
    renderThreadParams->isRegionDebugEnabled_ = isRegionDebugEnabled_;
    renderThreadParams->isDirtyRegionDfxEnabled_ = isDirtyRegionDfxEnabled_;
    renderThreadParams->isDisplayDirtyDfxEnabled_ = isDisplayDirtyDfxEnabled_;
    renderThreadParams->isOpaqueRegionDfxEnabled_ = isOpaqueRegionDfxEnabled_;
    renderThreadParams->isVisibleRegionDfxEnabled_ = isVisibleRegionDfxEnabled_;
    renderThreadParams->isTargetDirtyRegionDfxEnabled_ = isTargetDirtyRegionDfxEnabled_;
    renderThreadParams->dirtyRegionDebugType_ = dirtyRegionDebugType_;
    renderThreadParams->isOpDropped_ = isOpDropped_;
    renderThreadParams->dfxTargetSurfaceNames_ = std::move(dfxTargetSurfaceNames_);
}

void RSUniRenderVisitor::SetHardwareEnabledNodes(
    const std::vector<std::shared_ptr<RSSurfaceRenderNode>>& hardwareEnabledNodes)
{
    hardwareEnabledNodes_ = hardwareEnabledNodes;
}

bool RSUniRenderVisitor::DoDirectComposition(std::shared_ptr<RSBaseRenderNode> rootNode)
{
    auto children = rootNode->GetChildren();
    if (!IsHardwareComposerEnabled() || children->empty()) {
        RS_LOGD("RSUniRenderVisitor::DoDirectComposition HardwareComposer disabled");
        return false;
    }
    RS_TRACE_NAME("DoDirectComposition");
    auto& child = children->front();
    if (child == nullptr || !child->IsInstanceOf<RSDisplayRenderNode>()) {
        RS_LOGE("RSUniRenderVisitor::DoDirectComposition child type not match");
        return false;
    }
    auto displayNode = child->ReinterpretCastTo<RSDisplayRenderNode>();
    if (!displayNode ||
        displayNode->GetCompositeType() != RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE) {
        RS_LOGE("RSUniRenderVisitor::DoDirectComposition displayNode state error");
        return false;
    }
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    screenInfo_ = screenManager->QueryScreenInfo(displayNode->GetScreenId());
    if (screenInfo_.state != ScreenState::HDI_OUTPUT_ENABLE) {
        RS_LOGE("RSUniRenderVisitor::DoDirectComposition: ScreenState error!");
        return false;
    }
    processor_ = RSProcessorFactory::CreateProcessor(displayNode->GetCompositeType());
    if (processor_ == nullptr || renderEngine_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::DoDirectComposition: RSProcessor or renderEngine is null!");
        return false;
    }

    if (!processor_->Init(*displayNode, displayNode->GetDisplayOffsetX(), displayNode->GetDisplayOffsetY(),
        INVALID_SCREEN_ID, renderEngine_)) {
        RS_LOGE("RSUniRenderVisitor::DoDirectComposition: processor init failed!");
        return false;
    }

    if (!RSMainThread::Instance()->WaitHardwareThreadTaskExcute()) {
        RS_LOGW("RSUniRenderVisitor::DoDirectComposition: hardwareThread task has too many to excute");
    }
    if (!RSMainThread::Instance()->CheckIsHardwareEnabledBufferUpdated()) {
        for (auto& surfaceNode: hardwareEnabledNodes_) {
            if (!surfaceNode->IsHardwareForcedDisabled()) {
                surfaceNode->MarkCurrentFrameHardwareEnabled();
            }
        }
        RS_TRACE_NAME("DoDirectComposition skip commit");
        return true;
    }
    processor_->ProcessDisplaySurface(*displayNode);
    for (auto& node: hardwareEnabledNodes_) {
        if (!node->IsHardwareForcedDisabled()) {
            processor_->ProcessSurface(*node);
        }
    }
    DoScreenRcdTask(processor_, rcdInfo_, screenInfo_);
    processor_->PostProcess();
    RS_LOGD("RSUniRenderVisitor::DoDirectComposition end");
    return true;
}

void RSUniRenderVisitor::DrawWatermarkIfNeed(RSDisplayRenderNode& node, bool isMirror)
{
    if (RSMainThread::Instance()->GetWatermarkFlag()) {
        auto screenManager = CreateOrGetScreenManager();
        auto mainScreenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
        auto mainWidth = static_cast<float>(mainScreenInfo.width);
        auto mainHeight = static_cast<float>(mainScreenInfo.height);
        if (RSSystemProperties::IsFoldScreenFlag() && node.GetScreenId() == 0 && isMirror) {
            std::swap(mainWidth, mainHeight);
        }
        std::shared_ptr<Drawing::Image> drImage = RSMainThread::Instance()->GetWatermarkImg();
        if (drImage == nullptr) {
            return;
        }
        Drawing::Brush rectPaint;
        canvas_->AttachBrush(rectPaint);
        auto srcRect = Drawing::Rect(0, 0, drImage->GetWidth(), drImage->GetHeight());
        auto dstRect = Drawing::Rect(0, 0, mainWidth, mainHeight);
        canvas_->DrawImageRect(*drImage, srcRect, dstRect, Drawing::SamplingOptions(),
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        canvas_->DetachBrush();
    }
}

void RSUniRenderVisitor::SetAppWindowNum(uint32_t num)
{
    appWindowNum_ = num;
}

#ifdef ENABLE_RECORDING_DCL
void RSUniRenderVisitor::tryCapture(float width, float height)
{
    if (!RSSystemProperties::GetRecordingEnabled()) {
        return;
    }
    recordingCanvas_ = std::make_unique<ExtendRecordingCanvas>(width, height);
    RS_TRACE_NAME("RSUniRender:Recording begin");
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    auto renderContext = RSMainThread::Instance()->GetRenderEngine()->GetRenderContext();
    recordingCanvas_->SetGrRecordingContext(renderContext->GetSharedDrGPUContext());
#endif
    canvas_->AddCanvas(recordingCanvas_.get());
    RSRecordingThread::Instance(renderEngine_->GetRenderContext().get()).CheckAndRecording();
}

void RSUniRenderVisitor::endCapture() const
{
    if (!RSRecordingThread::Instance(renderEngine_->GetRenderContext().get()).GetRecordingEnabled()) {
        return;
    }
    auto drawCmdList = recordingCanvas_->GetDrawCmdList();
    RS_TRACE_NAME("RSUniRender:RecordingToFile curFrameNum = " +
        std::to_string(RSRecordingThread::Instance(renderEngine_->GetRenderContext().get()).GetCurDumpFrame()));
    RSRecordingThread::Instance(renderEngine_->GetRenderContext().get()).RecordingToFile(drawCmdList);
}
#endif

void RSUniRenderVisitor::RotateMirrorCanvasIfNeed(RSDisplayRenderNode& node, bool canvasRotation)
{
    auto mirrorNode = node.GetMirrorSource().lock();
    if ((canvasRotation && (RSSystemProperties::IsFoldScreenFlag() && mirrorNode->GetScreenId() == 0)) ||
        (!canvasRotation && !(RSSystemProperties::IsFoldScreenFlag() && mirrorNode->GetScreenId() == 0))) {
        return;
    }
    auto screenManager = CreateOrGetScreenManager();
    auto mainScreenInfo = screenManager->QueryScreenInfo(mirrorNode->GetScreenId());
    auto mainWidth = static_cast<float>(mainScreenInfo.width);
    auto mainHeight = static_cast<float>(mainScreenInfo.height);
    auto rotation = mirrorNode->GetScreenRotation();
    if (RSSystemProperties::IsFoldScreenFlag() && mirrorNode->GetScreenId() == 0) {
        if (rotation == ScreenRotation::ROTATION_0 || rotation == ScreenRotation::ROTATION_180) {
            std::swap(mainWidth, mainHeight);
        }
        auto oriRotation = node.GetOriginScreenRotation();
        rotation = static_cast<ScreenRotation>((static_cast<int>(oriRotation) -
            static_cast<int>(rotation) + SCREEN_ROTATION_NUM) % SCREEN_ROTATION_NUM);
    }
    if (rotation != ScreenRotation::ROTATION_0) {
        if (rotation == ScreenRotation::ROTATION_90) {
            canvas_->Rotate(90, 0, 0);
            canvas_->Translate(0, -mainHeight);
        } else if (rotation == ScreenRotation::ROTATION_180) {
            canvas_->Rotate(180, mainWidth / 2, mainHeight / 2);
        } else if (rotation == ScreenRotation::ROTATION_270) {
            canvas_->Rotate(270, 0, 0);
            canvas_->Translate(-mainWidth, 0);
        }
    }
}

void RSUniRenderVisitor::ScaleMirrorIfNeed(RSDisplayRenderNode& node, bool canvasRotation)
{
    auto screenManager = CreateOrGetScreenManager();
    auto mirrorNode = node.GetMirrorSource().lock();
    auto mainScreenInfo = screenManager->QueryScreenInfo(mirrorNode->GetScreenId());
    auto mainWidth = static_cast<float>(mainScreenInfo.width);
    auto mainHeight = static_cast<float>(mainScreenInfo.height);
    auto mirrorWidth = node.GetRenderProperties().GetBoundsWidth();
    auto mirrorHeight = node.GetRenderProperties().GetBoundsHeight();
    auto scaleMode = screenManager->GetScaleMode(node.GetScreenId());
    if (canvasRotation) {
        if ((RSSystemProperties::IsFoldScreenFlag() && mirrorNode->GetScreenId() == 0) ||
            mirrorNode->GetScreenRotation() == ScreenRotation::ROTATION_90 ||
            mirrorNode->GetScreenRotation() == ScreenRotation::ROTATION_270) {
            std::swap(mainWidth, mainHeight);
        }
    } else {
        if ((RSSystemProperties::IsFoldScreenFlag() && mirrorNode->GetScreenId() == 0)) {
            auto oriRotation = node.GetOriginScreenRotation();
            auto curRotation = node.GetScreenRotation();
            auto rotation = static_cast<ScreenRotation>((static_cast<int>(oriRotation) -
                static_cast<int>(curRotation) + SCREEN_ROTATION_NUM) % SCREEN_ROTATION_NUM);
            if (rotation == ScreenRotation::ROTATION_0 ||
                rotation == ScreenRotation::ROTATION_180) {
                std::swap(mainWidth, mainHeight);
            }
        } else {
            if ((node.GetOriginScreenRotation() == ScreenRotation::ROTATION_90 ||
                node.GetOriginScreenRotation() == ScreenRotation::ROTATION_270)) {
                std::swap(mirrorWidth, mirrorHeight);
            }
        }
    }
    // If the width and height not match the main screen, calculate the dstRect.
    if (mainWidth != mirrorWidth || mainHeight != mirrorHeight) {
        canvas_->Clear(SK_ColorBLACK);
        auto processor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor_);
        if (scaleMode == ScreenScaleMode::FILL_MODE) {
            processor->Fill(*canvas_, mainWidth, mainHeight, mirrorWidth, mirrorHeight);
        } else if (scaleMode == ScreenScaleMode::UNISCALE_MODE) {
            processor->UniScale(*canvas_, mainWidth, mainHeight, mirrorWidth, mirrorHeight);
        }
    }
}

void RSUniRenderVisitor::SetHasSharedTransitionNode(RSSurfaceRenderNode& surfaceNode, bool hasSharedTransitionNode)
{
    // only allow change hasSharedTransitionNode in leash window's child
    if (surfaceNode.GetSurfaceNodeType() == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
        return;
    }
    surfaceNode.SetHasSharedTransitionNode(hasSharedTransitionNode);
    // sync the change to parent leash window
    auto leashNode =
        RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(surfaceNode.GetParent().lock());
    if (leashNode && leashNode->GetSurfaceNodeType() == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
        leashNode->SetHasSharedTransitionNode(hasSharedTransitionNode);
    }
}

void RSUniRenderVisitor::ProcessUnpairedSharedTransitionNode()
{
    static auto unpairNode = [](const std::weak_ptr<RSRenderNode>& wptr) {
        auto sptr = wptr.lock();
        if (sptr == nullptr) {
            return;
        }
        sptr->SetSharedTransitionParam(nullptr);
        // make sure parent regenerates ChildrenDrawable after unpairing
        if (auto parent = sptr->GetParent().lock()) {
            parent->ApplyModifiers();
        }
    };
    for (auto& [id, wptr] : SharedTransitionParam::unpairedShareTransitions_) {
        auto sharedTransitionParam = wptr.lock();
        if (!sharedTransitionParam) {
            continue;
        }
        ROSEN_LOGE("RSUniRenderVisitor::ProcessUnpairedSharedTransitionNode: breaking up %s",
            sharedTransitionParam->Dump().c_str());
        unpairNode(sharedTransitionParam->inNode_);
        unpairNode(sharedTransitionParam->outNode_);
    }
    SharedTransitionParam::unpairedShareTransitions_.clear();
}

NodeId RSUniRenderVisitor::FindInstanceChildOfDisplay(std::shared_ptr<RSRenderNode> node)
{
    if (node == nullptr || node->GetParent().lock() == nullptr) {
        return INVALID_NODEID;
    } else if (node->GetParent().lock()->GetType() == RSRenderNodeType::DISPLAY_NODE) {
        return node->GetId();
    } else {
        return FindInstanceChildOfDisplay(node->GetParent().lock());
    }
}

bool RSUniRenderVisitor::CheckIfNeedResetRotate()
{
    if (canvas_ == nullptr) {
        return true;
    }
    int angle = RSUniRenderUtil::GetRotationFromMatrix(canvas_->GetTotalMatrix());
    return angle != 0 && angle % ROTATION_90 == 0;
}

bool RSUniRenderVisitor::IsOutOfScreenRegion(RectI rect)
{
    if (!canvas_) {
        return false;
    }

    auto deviceClipBounds = canvas_->GetDeviceClipBounds();

    if (rect.GetRight() <= deviceClipBounds.GetLeft() ||
        rect.GetLeft() >= deviceClipBounds.GetRight() ||
        rect.GetBottom() <= deviceClipBounds.GetTop() ||
        rect.GetTop() >= deviceClipBounds.GetBottom()) {
        return true;
    }

    return false;
}
} // namespace Rosen
} // namespace OHOS
