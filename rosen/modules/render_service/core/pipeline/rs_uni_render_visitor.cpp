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

#ifdef RS_ENABLE_OLD_VK
#include <vulkan_window.h>
#endif

#include "draw/color.h"
#ifndef USE_ROSEN_DRAWING
#include "include/core/SkRegion.h"
#include "include/core/SkTextBlob.h"
#else
#include "recording/recording_canvas.h"
#include "skia_adapter/skia_canvas.h"
#endif

#include "src/core/SkCanvasPriv.h"

#include "common/rs_background_thread.h"
#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "common/rs_singleton.h"
#include "memory/rs_tag_tracker.h"
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
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "platform/ohos/rs_jank_stats.h"
#include "property/rs_properties_painter.h"
#include "property/rs_point_light_manager.h"
#include "render/rs_skia_filter.h"
#include "pipeline/parallel_render/rs_parallel_render_manager.h"
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
constexpr int ROTATION_90 = 90;
constexpr int ROTATION_270 = 270;
constexpr float EPSILON_SCALE = 0.00001f;
static const std::string CAPTURE_WINDOW_NAME = "CapsuleWindow";
constexpr const char* CLEAR_GPU_CACHE = "ClearGpuCache";
static std::map<NodeId, uint32_t> cacheRenderNodeMap = {};
static uint32_t cacheReuseTimes = 0;
static std::mutex cacheRenderNodeMapMutex;
static std::mutex groupedTransitionNodesMutex;
using groupedTransitionNodesType = std::unordered_map<NodeId, std::pair<RSUniRenderVisitor::RenderParam,
    std::unordered_map<NodeId, RSUniRenderVisitor::RenderParam>>>;
static std::unordered_map<NodeId, std::pair<RSUniRenderVisitor::RenderParam,
    std::unordered_map<NodeId, RSUniRenderVisitor::RenderParam>>> groupedTransitionNodes = {};
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

#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
constexpr uint32_t PARALLEL_RENDER_MINIMUM_RENDER_NODE_NUMBER = 50;
#endif

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
#if defined(RS_ENABLE_DRIVEN_RENDER)
    if (RSDrivenRenderManager::GetInstance().GetDrivenRenderEnabled()) {
        drivenInfo_ = std::make_unique<DrivenInfo>();
    }
#endif
    if (RSRcdRenderManager::GetInstance().GetRcdRenderEnabled()) {
        rcdInfo_ = std::make_unique<RcdInfo>();
    }
    surfaceNodePrepareMutex_ = std::make_shared<std::mutex>();
    parallelRenderType_ = ParallelRenderingType::DISABLE;
#if defined(RS_ENABLE_PARALLEL_RENDER)
    isCalcCostEnable_ = RSSystemParameters::GetCalcCostEnabled();
#endif
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
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    auto screenNum = screenManager->GetAllScreenIds().size();
    isPartialRenderEnabled_ = (screenNum <= 1) && (partialRenderType_ != PartialRenderType::DISABLED) &&
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

RSUniRenderVisitor::RSUniRenderVisitor(std::shared_ptr<RSPaintFilterCanvas> canvas, uint32_t surfaceIndex)
    : RSUniRenderVisitor()
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    parallelRenderVisitorIndex_ = surfaceIndex;
#if defined(RS_ENABLE_GL)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        canvas_ = canvas;
    }
#endif
#endif
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
    surfaceNodePrepareMutex_ = visitor.surfaceNodePrepareMutex_;
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
    surfaceNodePrepareMutex_ = visitor->surfaceNodePrepareMutex_;
    prepareClipRect_ = visitor->prepareClipRect_;
    isOpDropped_ = visitor->isOpDropped_;
    isPartialRenderEnabled_ = visitor->isPartialRenderEnabled_;
    isHardwareForcedDisabled_ = visitor->isHardwareForcedDisabled_;
    doAnimate_ = visitor->doAnimate_;
    isDirty_ = visitor->isDirty_;
}

void RSUniRenderVisitor::CopyPropertyForParallelVisitor(RSUniRenderVisitor *mainVisitor)
{
    if (!mainVisitor) {
        RS_LOGE("main thread visitor is nullptr");
        return;
    }
    doAnimate_ = mainVisitor->doAnimate_;
    isParallel_ = mainVisitor->isParallel_;
    isUpdateCachedSurface_ = mainVisitor->isUpdateCachedSurface_;
    isHardwareForcedDisabled_ = mainVisitor->isHardwareForcedDisabled_;
    isOpDropped_ = mainVisitor->isOpDropped_;
    isPartialRenderEnabled_ = mainVisitor->isPartialRenderEnabled_;
    isUIFirst_ = mainVisitor->isUIFirst_;
    isSubThread_ = true;
}

void RSUniRenderVisitor::UpdateSubTreeInCache(const std::shared_ptr<RSRenderNode>& cacheRootNode,
    const std::vector<RSRenderNode::SharedPtr>& children)
{
    for (auto& child : children) {
        if (child == nullptr) {
            continue;
        }
#ifdef DDGR_ENABLE_FEATURE_OPINC
        if (autoCacheEnable_) {
            child->GetAutoCache()->OpincStartMarkStableStaticNode(unchangeMark_, autoCacheChildDisable_);
        }
#endif
        if (child->IsDirty()) {
            curDirty_ = child->IsDirty();
            child->Prepare(shared_from_this());
            continue;
        }
        // set flag for surface node whose children contain shared transition node
        if (child->GetSharedTransitionParam().has_value() && curSurfaceNode_) {
            SetHasSharedTransitionNode(*curSurfaceNode_, true);
        }
        // [planning] pay attention to outofparent case
        if (auto surfaceNode = child->ReinterpretCastTo<RSSurfaceRenderNode>()) {
            // fully prepare hwcLayer Layer and its subnodes
            if (surfaceNode->IsHardwareEnabledType()) {
                PrepareSurfaceRenderNode(*surfaceNode);
                return;
            }
            UpdateSecurityAndSkipLayerRecord(*surfaceNode);
        } else if (auto effectNode = child->ReinterpretCastTo<RSEffectRenderNode>()) {
            // effectNode need to update effectRegion so effectNode and use-effect child should be updated
            PrepareEffectNodeIfCacheReuse(cacheRootNode, effectNode);
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
        if (autoCacheEnable_) {
            child->GetAutoCache()->OpincEndMarkStableStaticNode(unchangeMark_, autoCacheChildDisable_);
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
    effectNode->SetRotationChanged(curDisplayNode_->IsRotationChanged());
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
    auto parentNode = std::move(logicParentNode_);
    logicParentNode_ = node.weak_from_this();
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
            isSurfaceDirtyNodeLimited_, node.HasUseEffectNodes());
        UpdateSubTreeInCache(node.ReinterpretCastTo<RSRenderNode>(), *children);
        node.UpdateEffectRegion(effectRegion_,
            (node.GetDrawingCacheType() != RSDrawingCacheType::DISABLED_CACHE && node.HasUseEffectNodes()));
    } else {
        // Get delay flag to restore geo changes
        if (node.GetCacheGeoPreparationDelay()) {
            dirtyFlag_ = true;
        }
        node.SetUseEffectNodes(false);
        for (auto& child : *children) {
            if (child && UNLIKELY(child->GetSharedTransitionParam().has_value())) {
                PrepareSharedTransitionNode(*child);
            }
            SaveCurSurface(curSurfaceDirtyManager_, curSurfaceNode_);
            curDirty_ = child->IsDirty();
            child->Prepare(shared_from_this());
            if (child->HasUseEffectNodes()) {
                node.SetUseEffectNodes(true);
            }
            RestoreCurSurface(curSurfaceDirtyManager_, curSurfaceNode_);
        }
        // Reset delay flag
        node.ResetCacheGeoPreparationDelay();
        SetNodeCacheChangeStatus(node);
    }

    curCornerRadius_ = std::move(tempCornerRadius);
    curAlpha_ = alpha;
    // restore environment variables
    logicParentNode_ = std::move(parentNode);
}

void RSUniRenderVisitor::MergeRemovedChildDirtyRegion(RSRenderNode& node)
{
    if (curSurfaceDirtyManager_ && curSurfaceNode_ && node.HasRemovedChild()) {
        RectI dirtyRect = prepareClipRect_.IntersectRect(node.GetChildrenRect());
        if (isSubNodeOfSurfaceInPrepare_) {
            curSurfaceDirtyManager_->MergeDirtyRect(dirtyRect);
            if (curSurfaceDirtyManager_->IsTargetForDfx()) {
                // since childRect includes multiple rects, defaultly marked as canvas_node
                curSurfaceDirtyManager_->UpdateDirtyRegionInfoForDfx(node.GetId(), RSRenderNodeType::CANVAS_NODE,
                    DirtyRegionType::REMOVE_CHILD_RECT, dirtyRect);
            }
        } else {
            curDisplayDirtyManager_->MergeDirtyRect(dirtyRect);
        }
        node.ResetHasRemovedChild();
    }
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
    node.SetCacheGeoPreparationDelay(dirtyFlag_);
    if (allCacheFilterRects_.count(node.GetId())) {
        node.SetChildHasFilter(true);
        if (const auto directParent = node.GetParent().lock()) {
            directParent->SetChildHasFilter(true);
        }
    }
    return true;
}

bool RSUniRenderVisitor::UpdateCacheChangeStatus(RSRenderNode& node)
{
    node.SetChildHasFilter(false);
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
            if (surfaceNode != nullptr && ProcessSharedTransitionNode(*surfaceNode)) {
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
    if (directParent != nullptr && node.ChildHasFilter()) {
        directParent->SetChildHasFilter(true);
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
        "staticCache %d, cacheChange %d, childHasFilter|effect: %d|%d, outofparent: %d, visitedCacheNodeIds num: %lu",
        node.GetId(), static_cast<int>(node.GetDrawingCacheType()), node.IsStaticCached(),
        static_cast<int>(isDrawingCacheChanged), node.ChildHasFilter(), node.HasUseEffectNodes(),
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

#ifndef USE_ROSEN_DRAWING
    parentSurfaceNodeMatrix_ = SkMatrix::I();
#else
    parentSurfaceNodeMatrix_ = Drawing::Matrix();
#endif
    auto geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr != nullptr) {
        geoPtr->UpdateByMatrixFromSelf();
        parentSurfaceNodeMatrix_ = geoPtr->GetAbsMatrix();
        if (geoPtr->IsNeedClientCompose()) {
            isHardwareForcedDisabled_ = true;
        }
    }
    dirtyFlag_ = dirtyFlag_ || node.IsRotationChanged();
#if defined(RS_ENABLE_DRIVEN_RENDER)
    if (drivenInfo_) {
        drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || dirtyFlag_;
    }
#endif
    // when display is in rotation state, occlusion relationship will be ruined,
    // hence partial-render quick-reject should be disabled.
    if(node.IsRotationChanged()) {
        isOpDropped_ = false;
        RS_TRACE_NAME("ClosePartialRender 1 RotationChanged");
    }
    node.UpdateRotation();
    curAlpha_ = node.GetRenderProperties().GetAlpha();
    isParallel_ = false;
    newColorSpace_ = GRAPHIC_COLOR_GAMUT_SRGB;
    hasFingerprint_ = false;
    newPixelFormat_ = GRAPHIC_PIXEL_FMT_RGBA_8888;
#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    ParallelPrepareDisplayRenderNodeChildrens(node);
#else
    PrepareChildren(node);
#endif
    auto mirrorNode = node.GetMirrorSource().lock();
    if (mirrorNode) {
        mirroredDisplays_.insert(mirrorNode->GetScreenId());
    }

    node.GetCurAllSurfaces().clear();
    node.CollectSurface(node.shared_from_this(), node.GetCurAllSurfaces(), true, false);

    HandleColorGamuts(node, screenManager);
    HandlePixelFormat(node, screenManager);

#if defined(RS_ENABLE_DRIVEN_RENDER)
    if (drivenInfo_) {
        RS_OPTIONAL_TRACE_NAME("RSUniRender:DrivenRenderPrepare");
        drivenInfo_->prepareInfo.hasInvalidScene = drivenInfo_->prepareInfo.hasInvalidScene ||
            node.GetChildrenCount() >= 8 || // default value, count > 8 means invalid scene
            isHardwareForcedDisabled_ || node.GetRotation() != ScreenRotation::ROTATION_0;
        drivenInfo_->prepareInfo.screenRect = RectI(0, 0, screenInfo_.width, screenInfo_.height),
        // prepare driven render tree
        RSDrivenRenderManager::GetInstance().DoPrepareRenderTask(drivenInfo_->prepareInfo);
        // merge dirty rect for driven render
        auto uniDrivenRenderMode = RSDrivenRenderManager::GetInstance().GetUniDrivenRenderMode();
        if (uniDrivenRenderMode == DrivenUniRenderMode::RENDER_WITH_CLIP_HOLE &&
            drivenInfo_->surfaceDirtyManager != nullptr) {
            auto drivenRenderDirtyRect = RSDrivenRenderManager::GetInstance().GetUniRenderSurfaceClipHoleRect();
            RS_OPTIONAL_TRACE_NAME("merge driven render dirty rect: " + drivenRenderDirtyRect.ToString());
            drivenInfo_->surfaceDirtyManager->MergeDirtyRect(drivenRenderDirtyRect);
        }
    }
#endif
    RSRcdRenderManager::GetInstance().DoPrepareRenderTask(rcdInfo_->prepareInfo);
}

void RSUniRenderVisitor::ParallelPrepareDisplayRenderNodeChildrens(RSDisplayRenderNode& node)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    auto parallelRenderManager = RSParallelRenderManager::Instance();
    doParallelRender_ = (node.GetChildrenCount() >= PARALLEL_RENDER_MINIMUM_RENDER_NODE_NUMBER);
    isParallel_ = AdaptiveSubRenderThreadMode(doParallelRender_) && parallelRenderManager->GetParallelMode();
    isDirtyRegionAlignedEnable_ = false;
    // we will open prepare parallel after check all properties.
    if (isParallel_ &&
        RSSystemProperties::GetPrepareParallelRenderingEnabled() != ParallelRenderingType::DISABLE) {
        parallelRenderManager->CopyPrepareVisitorAndPackTask(*this, node);
        parallelRenderManager->LoadBalanceAndNotify(TaskType::PREPARE_TASK);
        parallelRenderManager->WaitPrepareEnd(*this);
    } else {
        PrepareChildren(node);
    }
#endif
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
    auto directParent = node.GetParent().lock();
    if (directParent == nullptr) {
        return;
    }
    if (auto parentInstance = directParent->GetInstanceRootNode()) {
        // in case leashwindow is not directParent
        auto surfaceParent = parentInstance->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (surfaceParent && (surfaceParent->IsLeashWindow() || surfaceParent->IsMainWindowType())) {
            curSurfaceNode_ = surfaceParent;
            curSurfaceDirtyManager_ = surfaceParent->GetDirtyManager();
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
#ifndef USE_ROSEN_DRAWING
        canvas_->save();
        canvas_->clipRect({ static_cast<float>(dstRect.GetLeft()), static_cast<float>(dstRect.GetTop()),
                            static_cast<float>(dstRect.GetRight()), static_cast<float>(dstRect.GetBottom()) });
        canvas_->clear(SK_ColorTRANSPARENT);
        canvas_->restore();
#else
        canvas_->Save();
        canvas_->ClipRect({ static_cast<float>(dstRect.GetLeft()), static_cast<float>(dstRect.GetTop()),
                              static_cast<float>(dstRect.GetRight()), static_cast<float>(dstRect.GetBottom()) },
            Drawing::ClipOp::INTERSECT, false);
        canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
        canvas_->Restore();
#endif
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

    if (isParallel_ && !isUIFirst_) {
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
            RSParallelRenderManager::Instance()->AddAppWindowNode(parallelRenderVisitorIndex_, surfaceNode);
        }
#endif
    } else {
        if (surfaceNode->IsHardwareEnabledTopSurface()) {
            hardwareEnabledTopNodes_.emplace_back(surfaceNode);
        } else {
            appWindowNodesInZOrder_.emplace_back(surfaceNode);
        }
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

    // record whether child node is security or skip layer
    // it should be call after reset to false for each loop
    UpdateSecurityAndSkipLayerRecord(node);
}

void RSUniRenderVisitor::ClassifyUIFirstSurfaceDirtyStatus(RSSurfaceRenderNode& node)
{
    if (node.IsMainWindowType()) {
        isCachedSurfaceReuse_ = (quickSkipPrepareType_ >= QuickSkipPrepareType::STATIC_CACHE_SURFACE) &&
            (RSMainThread::Instance()->GetDeviceType() == DeviceType::PC) &&
            CheckIfUIFirstSurfaceContentReusable(curSurfaceNode_, isSurfaceDirtyNodeLimited_);
        // The condition of childHasFilter in QuerySubAssignable can not be used here
        // Because child node's filter is not collected yet, so disable prepare optimization when node is transparent
        // [planning]: detect the filter change before prepare, and use the last frame result
        isSurfaceDirtyNodeLimited_ = (quickSkipPrepareType_ == QuickSkipPrepareType::CONTENT_DIRTY_CACHE_SURFACE) &&
            !node.IsTransparent() && isSurfaceDirtyNodeLimited_ &&
            node.IsOnlyBasicGeoTransform() && node.IsContentDirtyNodeLimited();
        if (isCachedSurfaceReuse_) {
            node.SetCacheGeoPreparationDelay(dirtyFlag_);
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
            parentNode->SetChildHasFilter(true);
        }
        if (curSurfaceNode_) {
            curSurfaceNode_->UpdateFilterNodes(node.shared_from_this());
        }
    }
    if (node.IsLeashWindow()) {
        auto children = node.GetSortedChildren();
        for (auto& child : *children) {
            if (child->ChildHasFilter()) {
                node.SetChildHasFilter(true);
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
    if (curSurfaceNode_ == nullptr) {
        return;
    }
    if (node.GetSecurityLayer() && curSurfaceNode_->GetId() == node.GetInstanceRootNodeId()) {
        curSurfaceNode_->SetHasSecurityLayer(true);
        displayHasSecSurface_[currentVisitDisplay_] = true;
    }
    if (node.GetSkipLayer() && curSurfaceNode_->GetId() == node.GetInstanceRootNodeId() &&
        node.GetName().find(CAPTURE_WINDOW_NAME) == std::string::npos) {
        curSurfaceNode_->SetHasSkipLayer(true);
        displayHasSkipSurface_[currentVisitDisplay_] = true;
    }
}

void RSUniRenderVisitor::UpdateForegroundFilterCacheWithDirty(RSRenderNode& node,
    RSDirtyRegionManager& dirtyManager)
{
    node.UpdateFilterCacheWithDirty(dirtyManager, true);
    node.UpdateFilterCacheManagerWithCacheRegion(dirtyManager, prepareClipRect_);
}

void RSUniRenderVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    RS_OPTIONAL_TRACE_NAME("RSUniRender::Prepare:[" + node.GetName() + "] nodeid: " +
        std::to_string(node.GetId()) +  " pid: " + std::to_string(ExtractPid(node.GetId())) +
        ", nodeType " + std::to_string(static_cast<uint>(node.GetSurfaceNodeType())));
#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (autoCacheEnable_) {
        node->GetAutoCache()->DisableAutoCacheWithForce(unchangeMark_);
    }
#endif
    if (curDisplayNode_ == nullptr) {
        ROSEN_LOGE("RSUniRenderVisitor::PrepareSurfaceRenderNode, curDisplayNode_ is nullptr.");
        return;
    }
    // avoid EntryView upload texture while screen rotation
    if (node.GetName() == "EntryView") {
        node.SetStaticCached(curDisplayNode_->IsRotationChanged());
    }
    node.UpdatePositionZ();
#if defined(RS_ENABLE_DRIVEN_RENDER)
    if (drivenInfo_ && (node.GetName() == "imeWindow" || node.GetName() == "RecentView")) {
        drivenInfo_->prepareInfo.hasInvalidScene = true;
    }
#endif
    if (node.GetName().find(CAPTURE_WINDOW_NAME) != std::string::npos) {
        hasCaptureWindow_[currentVisitDisplay_] = true;
    }

    node.SetAncestorDisplayNode(curDisplayNode_);
    CheckColorSpace(node);
    CheckPixelFormat(node);

    // stop traversal if node keeps static
    if (isQuickSkipPreparationEnabled_ && CheckIfSurfaceRenderNodeStatic(node)) {
        // node type is mainwindow.
        if (node.GetHasSecurityLayer() && node.GetId() == node.GetInstanceRootNodeId()) {
            displayHasSecSurface_[currentVisitDisplay_] = true;
        }
        if (node.GetHasSkipLayer() && node.GetId() == node.GetInstanceRootNodeId() &&
            node.GetName().find(CAPTURE_WINDOW_NAME) == std::string::npos) {
            displayHasSkipSurface_[currentVisitDisplay_] = true;
        }
        PrepareSubSurfaceNodes(node);
        return;
    }
    // Attension: Updateinfo before info reset
    node.StoreMustRenewedInfo();
    SetHasSharedTransitionNode(node, false);
    // reset HasSecurityLayer
    node.SetHasSecurityLayer(false);
    node.SetHasSkipLayer(false);
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
        node->GetAutoCache()->OpincSetInAppStateStart(unchangeMark_, opNodeDepth_, opIncSubtreePropDirty_);
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
#ifndef USE_ROSEN_DRAWING
            boundsRect_ = SkRect::MakeWH(property.GetBoundsWidth(), property.GetBoundsHeight());
#else
            boundsRect_ = Drawing::Rect(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
#endif
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

#if defined(RS_ENABLE_DRIVEN_RENDER)
    bool isLeashWindowNode = false;
    if (drivenInfo_) {
        drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || dirtyFlag_;
        if (drivenInfo_->drivenUniTreePrepareMode == DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_BEFORE) {
            drivenInfo_->prepareInfo.dirtyInfo.backgroundDirty =
                drivenInfo_->prepareInfo.dirtyInfo.backgroundDirty || dirtyFlag_;
        } else if (drivenInfo_->drivenUniTreePrepareMode == DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_AFTER) {
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
                drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || node.GetRenderProperties().NeedFilter();
        }

        if (node.GetSurfaceNodeType() == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
            isLeashWindowNode = true;
            drivenInfo_->isPrepareLeashWinSubTree = true;
        }
        if (node.IsSelfDrawingType()) {
            drivenInfo_->prepareInfo.hasInvalidScene = true;
        }
    }
#endif

    bool isSubNodeOfSurfaceInPrepare = isSubNodeOfSurfaceInPrepare_;
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        isSubNodeOfSurfaceInPrepare_ = true;
    }
    node.UpdateChildrenOutOfRectFlag(false);
    // [planning] ShouldPrepareSubnodes would be applied again if condition constraint ensures
    PrepareChildren(node);
#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    rsParent = (logicParentNode_.lock());
    if (rsParent == curDisplayNode_) {
        std::unique_lock<std::mutex> lock(*surfaceNodePrepareMutex_);
        node.UpdateParentChildrenRect(logicParentNode_.lock());
    } else {
        node.UpdateParentChildrenRect(logicParentNode_.lock());
    }
#else
    node.UpdateParentChildrenRect(logicParentNode_.lock());
#endif
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
#if defined(RS_ENABLE_DRIVEN_RENDER)
    if (drivenInfo_ && isLeashWindowNode) {
        drivenInfo_->isPrepareLeashWinSubTree = false;
    }
#endif
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
        node->GetAutoCache()->OpincSetInAppStateEnd(unchangeMark_);
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
#ifndef USE_ROSEN_DRAWING
        isScale = (!ROSEN_EQ(absMatrix.getScaleX(), 1.f, EPSILON_SCALE) ||
            !ROSEN_EQ(absMatrix.getScaleY(), 1.f, EPSILON_SCALE));
#else
        isScale = (!ROSEN_EQ(absMatrix.Get(Drawing::Matrix::SCALE_X), 1.f, EPSILON_SCALE) ||
            !ROSEN_EQ(absMatrix.Get(Drawing::Matrix::SCALE_Y), 1.f, EPSILON_SCALE));
#endif
    } else {
        bool getMinMaxScales = false;
#ifndef USE_ROSEN_DRAWING
        // skScaleFactors[0]-minimum scaling factor, skScaleFactors[1]-maximum scaling factor
        SkScalar skScaleFactors[2];
        getMinMaxScales = absMatrix.getMinMaxScales(skScaleFactors);
        if (getMinMaxScales) {
            isScale = !ROSEN_EQ(skScaleFactors[0], 1.f) || !ROSEN_EQ(skScaleFactors[1], 1.f);
        }
#else
        // scaleFactors[0]-minimum scaling factor, scaleFactors[1]-maximum scaling factor
        Drawing::scalar scaleFactors[2];
        getMinMaxScales = absMatrix.GetMinMaxScales(scaleFactors);
        if (getMinMaxScales) {
            isScale = !ROSEN_EQ(scaleFactors[0], 1.f) || !ROSEN_EQ(scaleFactors[1], 1.f);
        }
#endif
        if (!getMinMaxScales) {
            RS_LOGD("getMinMaxScales fail, node:%{public}s %{public}" PRIu64 "", node.GetName().c_str(), node.GetId());
            auto dstRect = node.GetDstRect();
            float dstRectWidth = dstRect.GetWidth();
            float dstRectHeight = dstRect.GetHeight();
            float boundsWidth = property.GetBoundsWidth();
            float boundsHeight = property.GetBoundsHeight();
            isScale = !ROSEN_EQ(std::min(dstRectWidth, dstRectHeight), std::min(boundsWidth, boundsHeight))
                || !ROSEN_EQ(std::max(dstRectWidth, dstRectHeight), std::max(boundsWidth, boundsHeight));
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
    auto rsParent = (logicParentNode_.lock());
    if (rsParent == nullptr) {
        return;
    }
    auto& property = rsParent->GetMutableRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());

    // Context matrix should be relative to the parent surface node, so we need to revert the parentSurfaceNodeMatrix_.
#ifndef USE_ROSEN_DRAWING
    SkMatrix invertMatrix;
    auto contextMatrix = geoPtr->GetAbsMatrix();
    if (parentSurfaceNodeMatrix_.invert(&invertMatrix)) {
        contextMatrix.preConcat(invertMatrix);
    } else {
        ROSEN_LOGE("RSUniRenderVisitor::PrepareProxyRenderNode, invert parentSurfaceNodeMatrix_ failed");
    }
#else
    Drawing::Matrix invertMatrix;
    Drawing::Matrix contextMatrix = geoPtr->GetAbsMatrix();

    if (parentSurfaceNodeMatrix_.Invert(invertMatrix)) {
        contextMatrix.PreConcat(invertMatrix);
    } else {
        ROSEN_LOGE("RSUniRenderVisitor::PrepareProxyRenderNode, invert parentSurfaceNodeMatrix_ failed");
    }
#endif
    node.SetContextMatrix(contextMatrix);

    // For now, we only set the clipRegion if the parent node has ClipToBounds set to true.
    if (!property.GetClipToBounds()) {
        node.SetContextClipRegion(std::nullopt);
    } else {
        // Maybe we should use prepareClipRect_ and make the clipRegion in device coordinate, but it will be more
        // complex to calculate the intersect, and it will make app developers confused.
        auto rect = property.GetBoundsRect();
        // Context clip region is in the parent node coordinate, so we don't need to map it.
#ifndef USE_ROSEN_DRAWING
        node.SetContextClipRegion(SkRect::MakeXYWH(rect.left_, rect.top_, rect.width_, rect.height_));
#else
        node.SetContextClipRegion(Drawing::Rect(
            rect.GetLeft(), rect.GetTop(), rect.GetWidth() + rect.GetLeft(), rect.GetHeight() + rect.GetTop()));
#endif
    }

    // prepare children
    PrepareChildren(node);
}

void RSUniRenderVisitor::PrepareRootRenderNode(RSRootRenderNode& node)
{
    bool dirtyFlag = dirtyFlag_;
    auto parentSurfaceNodeMatrix = parentSurfaceNodeMatrix_;
    RectI prepareClipRect = prepareClipRect_;

    auto rsParent = (node.GetParent().lock());
    const auto& property = node.GetRenderProperties();
    bool geoDirty = property.IsGeoDirty();
    auto geoPtr = (property.GetBoundsGeometry());

    if (curSurfaceDirtyManager_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::PrepareRootRenderNode curSurfaceDirtyManager is nullptr");
        return;
    }
    dirtyFlag_ = node.Update(*curSurfaceDirtyManager_, rsParent, dirtyFlag_, prepareClipRect_);
#if defined(RS_ENABLE_DRIVEN_RENDER)
    if (drivenInfo_) {
        drivenInfo_->currentRootNode = node.shared_from_this();
        drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || dirtyFlag_;
        if (drivenInfo_->drivenUniTreePrepareMode == DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_BEFORE) {
            drivenInfo_->prepareInfo.dirtyInfo.backgroundDirty = dirtyFlag_;
        } else if (drivenInfo_->drivenUniTreePrepareMode == DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_AFTER) {
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
                drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || node.GetRenderProperties().NeedFilter();
        }
    }
#endif
    if (rsParent == curSurfaceNode_) {
        const float rootWidth = property.GetFrameWidth() * property.GetScaleX();
        const float rootHeight = property.GetFrameHeight() * property.GetScaleY();
#ifndef USE_ROSEN_DRAWING
        SkMatrix gravityMatrix;
        (void)RSPropertiesPainter::GetGravityMatrix(frameGravity_,
            RectF { 0.0f, 0.0f, boundsRect_.width(), boundsRect_.height() }, rootWidth, rootHeight, gravityMatrix);
#else
        Drawing::Matrix gravityMatrix;
        (void)RSPropertiesPainter::GetGravityMatrix(frameGravity_,
            RectF { 0.0f, 0.0f, boundsRect_.GetWidth(), boundsRect_.GetHeight() },
            rootWidth, rootHeight, gravityMatrix);
#endif
        // Only Apply gravityMatrix when rootNode is dirty
        if (geoPtr != nullptr && (dirtyFlag || geoDirty)) {
            geoPtr->ConcatMatrix(gravityMatrix);
        }
    }

    if (geoPtr != nullptr) {
        parentSurfaceNodeMatrix_ = geoPtr->GetAbsMatrix();
    }
    node.UpdateChildrenOutOfRectFlag(false);
    PrepareChildren(node);
    node.UpdateParentChildrenRect(logicParentNode_.lock());

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
    if (curSurfaceDirtyManager_ == nullptr || curDisplayDirtyManager_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::PrepareCanvasRenderNode curXDirtyManager is nullptr");
        return;
    }
    if (node.GetSharedTransitionParam().has_value()) {
        node.GetMutableRenderProperties().UpdateSandBoxMatrix(parentSurfaceNodeMatrix_);
    }
    if (isSubNodeOfSurfaceInPrepare_ && curSurfaceNode_ &&
        curSurfaceNode_->IsHardwareEnabledTopSurface() && node.ShouldPaint()) {
        curSurfaceNode_->SetSubNodeShouldPaint();
    }
#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (autoCacheEnable_) {
        node->GetAutoCache()->OpincStartMarkStableNode(unchangeMark_, autoCacheChildDisable_,
            opIncSubtreePropDirty_, opNodeDepth_);
    }
#endif
    // if canvasNode is not sub node of surfaceNode, merge the dirtyRegion to curDisplayDirtyManager_
    auto dirtyManager = isSubNodeOfSurfaceInPrepare_ ? curSurfaceDirtyManager_ : curDisplayDirtyManager_;
    dirtyFlag_ = node.Update(*dirtyManager, nodeParent, dirtyFlag_, prepareClipRect_);

#if defined(RS_ENABLE_DRIVEN_RENDER)
    // driven render
    bool isContentCanvasNode = false;
    bool isBeforeContentNodeDirty = false;
    if (drivenInfo_ && currentVisitDisplay_ == 0 && drivenInfo_->isPrepareLeashWinSubTree && node.IsMarkDriven()) {
        auto drivenCanvasNode = RSDrivenRenderManager::GetInstance().GetContentSurfaceNode()->GetDrivenCanvasNode();
        if (node.IsMarkDrivenRender() ||
            (!drivenInfo_->hasDrivenNodeMarkRender &&
            drivenCanvasNode != nullptr && node.GetId() == drivenCanvasNode->GetId())) {
            drivenInfo_->prepareInfo.backgroundNode = drivenInfo_->currentRootNode;
            drivenInfo_->prepareInfo.contentNode = node.shared_from_this();
            drivenInfo_->drivenUniTreePrepareMode = DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE;
            drivenInfo_->prepareInfo.dirtyInfo.contentDirty = false;
            drivenInfo_->surfaceDirtyManager = curSurfaceDirtyManager_;
            isContentCanvasNode = true;
            isBeforeContentNodeDirty = drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty;
            if (node.IsMarkDrivenRender()) {
                drivenInfo_->prepareInfo.dirtyInfo.type = DrivenDirtyType::MARK_DRIVEN_RENDER;
            } else {
                drivenInfo_->prepareInfo.dirtyInfo.type = DrivenDirtyType::MARK_DRIVEN;
            }
        }
    }
    if (drivenInfo_) {
        drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || dirtyFlag_;
        if (drivenInfo_->drivenUniTreePrepareMode == DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_BEFORE) {
            drivenInfo_->prepareInfo.dirtyInfo.backgroundDirty =
                drivenInfo_->prepareInfo.dirtyInfo.backgroundDirty || dirtyFlag_;
        } else if (drivenInfo_->drivenUniTreePrepareMode == DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_AFTER) {
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
                drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || node.GetRenderProperties().NeedFilter();
        } else {
            if (node.IsContentChanged()) {
                drivenInfo_->prepareInfo.dirtyInfo.contentDirty = true;
            }
        }
        if (node.IsContentChanged()) {
            node.SetIsContentChanged(false);
        }
    }
#endif
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
#ifndef USE_ROSEN_DRAWING
        RectF frameRect{
            property.GetFrameOffsetX() * geoPtr->GetAbsMatrix().getScaleX(),
            property.GetFrameOffsetY() * geoPtr->GetAbsMatrix().getScaleY(),
            property.GetFrameWidth(), property.GetFrameHeight()};
#else
        RectF frameRect{
            property.GetFrameOffsetX() * geoPtr->GetAbsMatrix().Get(Drawing::Matrix::SCALE_X),
            property.GetFrameOffsetY() * geoPtr->GetAbsMatrix().Get(Drawing::Matrix::SCALE_Y),
            property.GetFrameWidth(), property.GetFrameHeight()};
#endif
        prepareClipRect_ = prepareClipRect_.IntersectRect(geoPtr->MapAbsRect(frameRect));
    }

    node.UpdateChildrenOutOfRectFlag(false);

    if (isSkipCanvasNodeOutOfScreen_ && !isSubNodeOfSurfaceInPrepare_ && !node.HasSubSurface() &&
        IsOutOfScreenRegion(geoPtr->GetAbsRect())) {
        return;
    }

    PrepareChildren(node);
    // attention: accumulate direct parent's childrenRect
    node.UpdateParentChildrenRect(logicParentNode_.lock());
    if (property.GetUseEffect()) {
        if (auto directParent = node.GetParent().lock()) {
            directParent->SetUseEffectNodes(true);
        }
    }

    node.UpdateEffectRegion(effectRegion_);
    if (property.NeedFilter()) {
        // filterRects_ is used in RSUniRenderVisitor::CalcDirtyFilterRegion
        // When oldDirtyRect of node with filter has intersect with any surfaceNode or displayNode dirtyRegion,
        // the whole oldDirtyRect should be render in this vsync.
        // Partial rendering of node with filter would cause display problem.
        if (auto directParent = node.GetParent().lock()) {
            directParent->SetChildHasFilter(true);
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
    if (autoCacheEnable_) {
        node->GetAutoCache()->OpincEndMarkStableNode(unchangeMark_, autoCacheChildDisable_, opNodeDepth_);
    }
#endif
#if defined(RS_ENABLE_DRIVEN_RENDER)
    // skip content node and its children, calculate dirty contain background and foreground
    if (drivenInfo_ && isContentCanvasNode) {
        drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty = isBeforeContentNodeDirty;
        drivenInfo_->drivenUniTreePrepareMode = DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_AFTER;
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
    auto parentNode = node.GetParent().lock();
    node.SetRotationChanged(curDisplayNode_->IsRotationChanged());
    if (curDisplayNode_->IsRotationChanged()) {
        int invalidateTimes = 2; // node call invalidate cache 3 times in one frame.
        node.SetInvalidateTimesForRotation(invalidateTimes);
    }
    node.SetVisitedFilterCacheStatus(curSurfaceDirtyManager_->IsCacheableFilterRectEmpty());
    dirtyFlag_ = node.Update(*curSurfaceDirtyManager_, parentNode, dirtyFlag_, prepareClipRect_);

    node.UpdateChildrenOutOfRectFlag(false);
    PrepareChildren(node);
    node.UpdateParentChildrenRect(logicParentNode_.lock());
    node.SetEffectRegion(effectRegion_);
    
    if (node.GetRenderProperties().NeedFilter()) {
        // filterRects_ is used in RSUniRenderVisitor::CalcDirtyFilterRegion
        // When oldDirtyRect of node with filter has intersect with any surfaceNode or displayNode dirtyRegion,
        // the whole oldDirtyRect should be render in this vsync.
        // Partial rendering of node with filter would cause display problem.
        if (parentNode) {
            parentNode->SetChildHasFilter(true);
        }
        if (curSurfaceDirtyManager_->IsTargetForDfx()) {
            curSurfaceDirtyManager_->UpdateDirtyRegionInfoForDfx(node.GetId(), RSRenderNodeType::CANVAS_NODE,
                DirtyRegionType::FILTER_RECT, node.GetOldDirtyInSurface());
        }
#ifndef USE_ROSEN_DRAWING
        if (curSurfaceNode_ && effectRegion_.has_value() && !effectRegion_->isEmpty()) {
            RectI filterRect(effectRegion_->left(), effectRegion_->top(),
                effectRegion_->width(), effectRegion_->height());
#else
        if (curSurfaceNode_ && effectRegion_.has_value() && !effectRegion_->IsEmpty()) {
            RectI filterRect(effectRegion_->GetLeft(), effectRegion_->GetTop(),
                effectRegion_->GetWidth(), effectRegion_->GetHeight());
#endif
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

void RSUniRenderVisitor::CopyForParallelPrepare(std::shared_ptr<RSUniRenderVisitor> visitor)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    isPartialRenderEnabled_ = isPartialRenderEnabled_ && visitor->isPartialRenderEnabled_;
    isOpDropped_ = isOpDropped_ && visitor->isOpDropped_;
    needFilter_ = needFilter_ || visitor->needFilter_;
    for (const auto &u : visitor->displayHasSecSurface_) {
        displayHasSecSurface_[u.first] = displayHasSecSurface_[u.first] || u.second;
    }
    for (const auto &u : visitor->displayHasSkipSurface_) {
        displayHasSkipSurface_[u.first] = displayHasSkipSurface_[u.first] || u.second;
    }
    for (const auto &u : visitor->dirtySurfaceNodeMap_) {
        dirtySurfaceNodeMap_[u.first] = u.second;
    }
    for (const auto &u : visitor->hasCaptureWindow_) {
        hasCaptureWindow_[u.first] = u.second;
    }

#endif
}

#ifndef USE_ROSEN_DRAWING
void RSUniRenderVisitor::DrawDirtyRectForDFX(const RectI& dirtyRect, const SkColor color,
    const SkPaint::Style fillType, float alpha, int edgeWidth = 6)
{
    if (dirtyRect.width_ <= 0 || dirtyRect.height_ <= 0) {
        ROSEN_LOGD("DrawDirtyRectForDFX dirty rect is invalid.");
        return;
    }
    ROSEN_LOGD("DrawDirtyRectForDFX current dirtyRect = %{public}s", dirtyRect.ToString().c_str());
    auto skRect = SkRect::MakeXYWH(dirtyRect.left_, dirtyRect.top_, dirtyRect.width_, dirtyRect.height_);
    std::string position = std::to_string(dirtyRect.left_) + ',' + std::to_string(dirtyRect.top_) + ',' +
        std::to_string(dirtyRect.width_) + ',' + std::to_string(dirtyRect.height_);
    const int defaultTextOffsetX = edgeWidth;
    const int defaultTextOffsetY = 30; // text position has 30 pixelSize under the skRect
    SkPaint rectPaint;
    // font size: 24
    sk_sp<SkTypeface> typeface = SkTypeface::MakeFromName("HarmonyOS Sans SC", SkFontStyle::Normal());
    sk_sp<SkTextBlob> SkTextBlob = SkTextBlob::MakeFromString(position.c_str(), SkFont(typeface, 24.0f, 1.0f, 0.0f));
    rectPaint.setColor(color);
    rectPaint.setAntiAlias(true);
    rectPaint.setAlphaf(alpha);
    rectPaint.setStyle(fillType);
    rectPaint.setStrokeWidth(edgeWidth);
    if (fillType == SkPaint::kFill_Style) {
        rectPaint.setStrokeJoin(SkPaint::kRound_Join);
    }
    canvas_->drawRect(skRect, rectPaint);
    canvas_->drawTextBlob(SkTextBlob, dirtyRect.left_ + defaultTextOffsetX,
        dirtyRect.top_ + defaultTextOffsetY, SkPaint());
}
#else
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
        std::to_string(dirtyRect.width_) + ',' + std::to_string(dirtyRect.height_);
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
#endif

void RSUniRenderVisitor::DrawDirtyRegionForDFX(std::vector<RectI> dirtyRects)
{
    const float fillAlpha = 0.2;
    for (const auto& subRect : dirtyRects) {
#ifndef USE_ROSEN_DRAWING
        DrawDirtyRectForDFX(subRect, SK_ColorBLUE, SkPaint::kStroke_Style, fillAlpha);
#else
        DrawDirtyRectForDFX(subRect, Drawing::Color::COLOR_BLUE, RSPaintStyle::STROKE, fillAlpha);
#endif
    }
}

void RSUniRenderVisitor::DrawCacheRegionForDFX(std::vector<RectI> cacheRects)
{
    const float fillAlpha = 0.2;
    for (const auto& subRect : cacheRects) {
#ifndef USE_ROSEN_DRAWING
        DrawDirtyRectForDFX(subRect, SK_ColorBLUE, SkPaint::kFill_Style, fillAlpha);
#else
        DrawDirtyRectForDFX(subRect, Drawing::Color::COLOR_BLUE, RSPaintStyle::FILL, fillAlpha);
#endif
    }
}

#ifdef DDGR_ENABLE_FEATURE_OPINC
constexpr uint32_t DRAW_DFX_EDGE_WIDTH = 6;
void RSUniRenderVisitor::DrawAutoCacheRegionForDFX(std::vector<RectI, std::string> cacheRegionInfo)
{
    const float fillAlpha = 0.2;
    for (const auto& info : cacheRegionInfo) {
#ifndef USE_ROSEN_DRAWING
        DrawDirtyRectForDFX(info.first, SK_ColorBLUE, SkPaint::kFill_Style, fillAlpha,
            DRAW_DFX_EDGE_WIDTH, info.second);
#else
        DrawDirtyRectForDFX(info.first, Drawing::Color::COLOR_BLUE, RSPaintStyle::FILL, fillAlpha,
            DRAW_DFX_EDGE_WIDTH, info.second);
#endif
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
#ifndef USE_ROSEN_DRAWING
    DrawDirtyRectForDFX(dirtySurfaceRect, SK_ColorRED, SkPaint::kStroke_Style, fillAlpha);
#else
    DrawDirtyRectForDFX(dirtySurfaceRect, Drawing::Color::COLOR_RED, RSPaintStyle::STROKE, fillAlpha);
#endif
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
#ifndef USE_ROSEN_DRAWING
        DrawDirtyRectForDFX(absRect, SK_ColorBLUE, SkPaint::kStroke_Style, strokeAlpha, strokeWidth);
#else
        DrawDirtyRectForDFX(absRect, Drawing::Color::COLOR_BLUE, RSPaintStyle::STROKE, strokeAlpha, strokeWidth);
#endif
        // draw useEffect nodes
        const auto& useEffectRects = effectNode.second.first;
        for (const auto& rect : useEffectRects) {
#ifndef USE_ROSEN_DRAWING
            DrawDirtyRectForDFX(rect, SK_ColorGREEN, SkPaint::kStroke_Style, strokeAlpha, strokeWidth);
#else
            DrawDirtyRectForDFX(rect, Drawing::Color::COLOR_GREEN, RSPaintStyle::STROKE, strokeAlpha, strokeWidth);
#endif
        }

        // draw fallback nodes
        const auto fallbackRects = effectNode.second.second;
        for (const auto& rect : fallbackRects) {
#ifndef USE_ROSEN_DRAWING
            DrawDirtyRectForDFX(rect, SK_ColorRED, SkPaint::kStroke_Style, strokeAlpha, strokeWidth);
#else
            DrawDirtyRectForDFX(rect, Drawing::Color::COLOR_RED, RSPaintStyle::STROKE, strokeAlpha, strokeWidth);
#endif
        }
    }
}

void RSUniRenderVisitor::DrawCurrentRefreshRate(uint32_t currentRefreshRate, uint32_t realtimeRefreshRate)
{
    std::string info = std::to_string(currentRefreshRate) + " " + std::to_string(realtimeRefreshRate);
    auto color = currentRefreshRate <= 60 ? SK_ColorRED : SK_ColorGREEN;
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkTypeface> tf = SkTypeface::MakeFromName("HarmonyOS Sans SC", SkFontStyle::Normal());
    SkFont font;
    font.setSize(100);  // 100:Scalar of setting font size
    font.setTypeface(tf);
    sk_sp<SkTextBlob> textBlob = SkTextBlob::MakeFromString(info.c_str(), font);

    SkPaint paint;
    paint.setColor(color);
    paint.setAntiAlias(true);
    canvas_->drawTextBlob(
        textBlob, 100.f, 200.f, paint);  // 100.f:Scalar x of drawing TextBlob; 200.f:Scalar y of drawing TextBlob
#else
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
#endif
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
#ifndef USE_ROSEN_DRAWING
                DrawDirtyRectForDFX(rect, info.second, SkPaint::kStroke_Style, fillAlpha);
#else
                DrawDirtyRectForDFX(rect, info.second, RSPaintStyle::STROKE, fillAlpha);
#endif
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
#ifndef USE_ROSEN_DRAWING
        DrawDirtyRectForDFX(subRect.ToRectI(), SK_ColorGREEN, SkPaint::kFill_Style, 0.2f, 0);
#else
        DrawDirtyRectForDFX(subRect.ToRectI(), Drawing::Color::COLOR_GREEN,
            RSPaintStyle::FILL, 0.2f, 0);
#endif
    }
}

void RSUniRenderVisitor::ProcessShadowFirst(RSRenderNode& node, bool inSubThread)
{
    if (RSSystemProperties::GetUseShadowBatchingEnabled()
        && (node.GetRenderProperties().GetUseShadowBatching())) {
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
        node.ChildHasFilter() && updateCacheProcessCnt_ == 0) {
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
    if (child && ProcessSharedTransitionNode(*child)) {
        if (node.GetDrawingCacheRootId() != INVALID_NODEID) {
            child->SetDrawingCacheRootId(node.GetDrawingCacheRootId());
        }
        SaveCurSurface(curSurfaceDirtyManager_, curSurfaceNode_);
        UpdateVirtualScreenFilterAppRootId(child);
        child->Process(shared_from_this());
        UpdateVirtualScreenFilterAppRootId(child);
        RestoreCurSurface(curSurfaceDirtyManager_, curSurfaceNode_);
    }
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

void RSUniRenderVisitor::ProcessParallelDisplayRenderNode(RSDisplayRenderNode& node)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_VK)
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return;
    }
    RS_TRACE_NAME("ProcessParallelDisplayRenderNode[" + std::to_string(node.GetId()));
    RS_LOGD("RSUniRenderVisitor::ProcessParallelDisplayRenderNode node: %{public}" PRIu64 ", child size:%{public}u",
        node.GetId(), node.GetChildrenCount());
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode ScreenManager is nullptr");
        return;
    }
    screenInfo_ = screenManager->QueryScreenInfo(node.GetScreenId());
    isSecurityDisplay_ = node.GetSecurityDisplay();
    switch (screenInfo_.state) {
        case ScreenState::HDI_OUTPUT_ENABLE:
            node.SetCompositeType(node.IsForceSoftComposite() ?
                RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE :
                RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE);
            break;
        case ScreenState::PRODUCER_SURFACE_ENABLE:
        default:
            RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode ScreenState only support HDI_OUTPUT_ENABLE");
            return;
    }
    offsetX_ = node.GetDisplayOffsetX();
    offsetY_ = node.GetDisplayOffsetY();
    processor_ = RSProcessorFactory::CreateProcessor(node.GetCompositeType());
    if (processor_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode: RSProcessor is null!");
        return;
    }

    // ParallelDisplayRenderNode cannot have mirror source.
    if (!processor_->Init(node, node.GetDisplayOffsetX(), node.GetDisplayOffsetY(),
        INVALID_SCREEN_ID, renderEngine_)) {
        RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode: processor init failed!");
        return;
    }
    std::shared_ptr<RSBaseRenderNode> nodePtr = node.shared_from_this();
    auto displayNodePtr = nodePtr->ReinterpretCastTo<RSDisplayRenderNode>();
    if (!displayNodePtr) {
        RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode ReinterpretCastTo fail");
        return;
    }
    if (renderFrame_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode renderFrame_ nullptr");
        return;
    }
    std::shared_ptr<RSCanvasListener> overdrawListener = nullptr;
    AddOverDrawListener(renderFrame_, overdrawListener);

    if (canvas_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode: failed to create canvas");
        return;
    }
#ifndef USE_ROSEN_DRAWING
    canvas_->clear(SK_ColorTRANSPARENT);
#else
    canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
#endif
    RSPropertiesPainter::SetBgAntiAlias(true);
#ifndef USE_ROSEN_DRAWING
    int saveCount = canvas_->save();
#else
    int saveCount = canvas_->Save();
#endif
    canvas_->SetHighContrast(renderEngine_->IsHighContrastEnabled());
    auto geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr != nullptr) {
#ifndef USE_ROSEN_DRAWING
        canvas_->concat(geoPtr->GetMatrix());
#else
        canvas_->ConcatMatrix(geoPtr->GetMatrix());
#endif
    }
    for (auto& childNode : *node.GetChildren()) {
        RSParallelRenderManager::Instance()->StartTiming(parallelRenderVisitorIndex_);
        childNode->Process(shared_from_this());
        RSParallelRenderManager::Instance()->StopTimingAndSetRenderTaskCost(
            parallelRenderVisitorIndex_, childNode->GetId(), TaskType::PROCESS_TASK);
    }
#ifndef USE_ROSEN_DRAWING
    canvas_->restoreToCount(saveCount);
#else
    canvas_->RestoreToCount(saveCount);
#endif

    if (overdrawListener != nullptr) {
        overdrawListener->Draw();
    }
    DrawWatermarkIfNeed(node);
    RS_TRACE_BEGIN("ProcessParallelDisplayRenderNode:FlushFrame");
    renderFrame_->Flush();
    RS_TRACE_END();
    RS_LOGD("RSUniRenderVisitor::ProcessParallelDisplayRenderNode end");
#endif
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkImage> RSUniRenderVisitor::GetCacheImageFromMirrorNode(std::shared_ptr<RSDisplayRenderNode> mirrorNode)
{
    sk_sp<SkImage> image = nullptr;
    auto cacheImage = mirrorNode->GetCacheImgForCapture();

    bool parallelComposition = RSMainThread::Instance()->GetParallelCompositionEnabled();
    if (!parallelComposition) {
        return cacheImage;
    }

    if (cacheImage != nullptr) {
        auto renderContext = renderEngine_->GetRenderContext();
        if (renderContext != nullptr) {
            auto grContext = renderContext->GetGrContext();
            auto imageBackendTexure = cacheImage->getBackendTexture(false);
            if (grContext != nullptr && imageBackendTexure.isValid()) {
                image = SkImage::MakeFromTexture(grContext, imageBackendTexure,
                    kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr);
            }
        }
    }
    return image;
}
#else
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
#endif

void RSUniRenderVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode& node)
{
    if (mirroredDisplays_.size() == 0) {
        node.SetCacheImgForCapture(nullptr);
    }
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_VK)
    if ((RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) && (node.IsParallelDisplayNode())) {
        ProcessParallelDisplayRenderNode(node);
        return;
    }
#endif
    RS_TRACE_NAME("ProcessDisplayRenderNode[" + std::to_string(node.GetScreenId()) + "]" +
        node.GetDirtyManager()->GetDirtyRegion().ToString().c_str());
    RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode node: %{public}" PRIu64 ", child size:%{public}u",
        node.GetId(), node.GetChildrenCount());
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
    bool isNeedCalcCost = false;
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        isNeedCalcCost = node.GetSurfaceChangedRects().size() > 0;
    }
#endif
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

#if defined(RS_ENABLE_DRIVEN_RENDER)
    // [PLANNING]: processing of layers z-order to be implemented
    if (drivenInfo_ && !drivenInfo_->prepareInfo.hasInvalidScene) {
        drivenInfo_->currDrivenRenderMode = RSDrivenRenderManager::GetInstance().GetUniDrivenRenderMode();
        globalZOrder_ = RSDrivenRenderManager::GetInstance().GetUniRenderGlobalZOrder();
    }
#endif

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
#ifndef USE_ROSEN_DRAWING
        canvas_->save();
#else
        canvas_->Save();
#endif
        ScaleMirrorIfNeed(node);
        RotateMirrorCanvasIfNeed(node);
        bool forceCPU = false;
        auto params = RSUniRenderUtil::CreateBufferDrawParam(*mirrorNode, forceCPU);
        params.isMirror = true;
        renderEngine_->DrawDisplayNodeWithParams(*canvas_, *mirrorNode, params);
#ifndef USE_ROSEN_DRAWING
        canvas_->restore();
#else
        canvas_->Restore();
#endif
        mirrorAutoRotate_ = false;
        renderFrame_->Flush();
        processor_->ProcessDisplaySurface(node);
        processor_->PostProcess(&node);
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
#ifndef USE_ROSEN_DRAWING
                canvas_->clear(SK_ColorBLACK);
#else
                canvas_->Clear(Drawing::Color::COLOR_BLACK);
#endif
                processor_->PostProcess();
                RS_LOGI("RSUniRenderVisitor::ProcessDisplayRenderNode, set canvas to black because of security layer.");
                canvas_->SetDisableFilterCache(false);
                return;
            }
#ifndef USE_ROSEN_DRAWING
            sk_sp<SkImage> cacheImageProcessed = GetCacheImageFromMirrorNode(mirrorNode);
            bool canvasRotation = screenManager->GetCanvasRotation(node.GetScreenId());
            if (cacheImageProcessed && !displayHasSkipSurface_[mirrorNode->GetScreenId()] &&
                !displayHasSecSurface_[mirrorNode->GetScreenId()] && screenInfo_.filteredAppSet.empty()) {
                RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode screen recording optimization is enable");
                ScaleMirrorIfNeed(node, canvasRotation);
                if (canvasRotation && !(RSSystemProperties::IsFoldScreenFlag() && mirrorNode->GetScreenId() == 0)) {
                    RotateMirrorCanvasIfNeed(node);
                }
                canvas_->save();
                // If both canvas and skImage have rotated, we need to reset the canvas
                if (resetRotate_) {
                    SkMatrix invertMatrix;
                    if (processor->GetScreenTransformMatrix().invert(&invertMatrix)) {
                        canvas_->concat(invertMatrix);
                    }
                }
                SkPaint paint;
                paint.setAntiAlias(true);
                canvas_->drawImage(cacheImageProcessed, 0, 0,
                    SkSamplingOptions(SkCubicResampler { 1 / 3.0f, 1 / 3.0f }), &paint); // 1/3.0f is Mitchell sampling
                canvas_->restore();
                bool parallelComposition = RSMainThread::Instance()->GetParallelCompositionEnabled();
                if (!parallelComposition) {
                    int saveCount = canvas_->save();
                    ProcessChildrenForScreenRecordingOptimization(
                        *mirrorNode, mirrorNode->GetRootIdOfCaptureWindow());
                    canvas_->restoreToCount(saveCount);
                }
                DrawWatermarkIfNeed(*mirrorNode, true);
            } else {
                RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode screen recording optimization is disable");
                mirrorNode->SetCacheImgForCapture(nullptr);
                int saveCount = canvas_->save();
                ScaleMirrorIfNeed(node, canvasRotation);
                if (canvasRotation && !(RSSystemProperties::IsFoldScreenFlag() && mirrorNode->GetScreenId() == 0)) {
                    RotateMirrorCanvasIfNeed(node);
                }
                PrepareOffscreenRender(*mirrorNode);
                canvas_->SetDisableFilterCache(true);
                ProcessChildren(*mirrorNode);
                FinishOffscreenRender(true);
                DrawWatermarkIfNeed(*mirrorNode, true);
                canvas_->restoreToCount(saveCount);
            }
#else
            std::shared_ptr<Drawing::Image> cacheImageProcessed = GetCacheImageFromMirrorNode(mirrorNode);
            bool canvasRotation = screenManager->GetCanvasRotation(node.GetScreenId());
            if (cacheImageProcessed && !displayHasSkipSurface_[mirrorNode->GetScreenId()] &&
                !displayHasSecSurface_[mirrorNode->GetScreenId()] && screenInfo_.filteredAppSet.empty()) {
                RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode screen recording optimization is enable");
                ScaleMirrorIfNeed(node, canvasRotation);
                if (canvasRotation && !(RSSystemProperties::IsFoldScreenFlag() && mirrorNode->GetScreenId() == 0)) {
                    RotateMirrorCanvasIfNeed(node);
                }
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
                if (canvasRotation && !(RSSystemProperties::IsFoldScreenFlag() && mirrorNode->GetScreenId() == 0)) {
                    RotateMirrorCanvasIfNeed(node);
                }
                PrepareOffscreenRender(*mirrorNode);
                canvas_->SetDisableFilterCache(true);
                ProcessChildren(*mirrorNode);
                FinishOffscreenRender(true);
                DrawWatermarkIfNeed(*mirrorNode, true);
                canvas_->RestoreToCount(saveCount);
            }
#endif
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
#if defined(RS_ENABLE_DRIVEN_RENDER)
    } else if (drivenInfo_ && drivenInfo_->currDrivenRenderMode == DrivenUniRenderMode::REUSE_WITH_CLIP_HOLE) {
        RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode DrivenUniRenderMode is REUSE_WITH_CLIP_HOLE");
        node.SetGlobalZOrder(globalZOrder_);
        processor_->ProcessDisplaySurface(node);
#endif
    } else {
#ifdef RS_ENABLE_EGLQUERYSURFACE
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
            RSMainThread::Instance()->rsVSyncDistributor_->MarkRSNotRendering();
            GpuDirtyRegion::GetInstance().AddSkipProcessFramesNumberForXpower(node.GetScreenId());
#ifdef OHOS_PLATFORM
            RSJankStats::GetInstance().SetSkipDisplayNode();
#endif
            resetRotate_ = CheckIfNeedResetRotate();
            if (!IsHardwareComposerEnabled()) {
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
                return;
            }
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
                processor_->PostProcess(&node);
            }
            return;
        }
#endif

#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_OLD_VK)
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
            if (isParallel_ &&!isPartialRenderEnabled_) {
                auto parallelRenderManager = RSParallelRenderManager::Instance();
                vulkan::VulkanWindow::InitializeVulkan(
                    parallelRenderManager->GetParallelThreadNumber());
                RS_OPTIONAL_TRACE_BEGIN("RSUniRender::VK::WaitFence");
                vulkan::VulkanWindow::WaitForSharedFence();
                vulkan::VulkanWindow::ResetSharedFence();
                RS_OPTIONAL_TRACE_END();
                parallelRenderManager->CopyVisitorAndPackTask(*this, node);
                parallelRenderManager->InitDisplayNodeAndRequestFrame(renderEngine_, screenInfo_);
                parallelRenderManager->LoadBalanceAndNotify(TaskType::PROCESS_TASK);
                parallelRenderManager->WaitProcessEnd();
                parallelRenderManager->CommitSurfaceNum(node.GetChildrenCount());
                vulkan::VulkanWindow::PresentAll();

                RS_OPTIONAL_TRACE_BEGIN("RSUniRender:WaitUtilUniRenderFinished");
                RSMainThread::Instance()->WaitUtilUniRenderFinished();
                RS_OPTIONAL_TRACE_END();

                parallelRenderManager->ProcessParallelDisplaySurface(*this);
                processor_->PostProcess();

                parallelRenderManager->ReleaseBuffer();

                isParallel_ = false;
                return;
            }
        }
#endif

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
#ifndef USE_ROSEN_DRAWING
        SkRegion region;
#else
        Drawing::Region region;
#endif
        Occlusion::Region dirtyRegionTest;
        std::vector<RectI> rects;
        bool clipPath = false;
#ifdef RS_ENABLE_VK
        int saveCountBeforeClip = 0;
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
#ifndef USE_ROSEN_DRAWING
            saveCountBeforeClip = canvas_->save();
#else
            saveCountBeforeClip = canvas_->Save();
#endif
        }
#endif
#ifdef RS_ENABLE_EGLQUERYSURFACE
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
                displayNodePtr, RSMainThread::Instance()->GetDrawStatusVec(), isDirtyRegionAlignedEnable_);
            dirtyRegionTest = dirtyRegion;
            if (isDirtyRegionAlignedEnable_) {
                SetSurfaceGlobalAlignedDirtyRegion(displayNodePtr, dirtyRegion);
            } else {
                SetSurfaceGlobalDirtyRegion(displayNodePtr);
            }
            rects = GetDirtyRects(dirtyRegion);
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
                    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
                        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
                        topAfterFlip = r.top_;
                    } else {
                        topAfterFlip = static_cast<int32_t>(screenInfo_.GetRotatedHeight()) - r.GetBottom();
                    }
#else
                    topAfterFlip = static_cast<int32_t>(screenInfo_.GetRotatedHeight()) - r.GetBottom();
#endif
#ifndef USE_ROSEN_DRAWING
                    region.op(SkIRect::MakeXYWH(r.left_, topAfterFlip, r.width_, r.height_), SkRegion::kUnion_Op);
#else
                    Drawing::Region tmpRegion;
                    tmpRegion.SetRect(Drawing::RectI(r.left_, topAfterFlip,
                        r.left_ + r.width_, topAfterFlip + r.height_));
                    region.Op(tmpRegion, Drawing::RegionOp::UNION);
#endif
                }
            }
            // SetDamageRegion and opDrop will be disabled for dirty region DFX visualization
            if (!isRegionDebugEnabled_) {
                renderFrame_->SetDamageRegion(rects);
            }
        }
        if (isOpDropped_ && !isDirtyRegionAlignedEnable_) {
#ifndef USE_ROSEN_DRAWING
            if (region.isEmpty()) {
                // [planning] Remove this after frame buffer can cancel
                canvas_->clipRect(SkRect::MakeEmpty());
            } else if (region.isRect()) {
                canvas_->clipRegion(region);
                canvas_->clear(SK_ColorTRANSPARENT);
            } else {
                RS_TRACE_NAME("RSUniRenderVisitor: clipPath");
                clipPath = true;
#ifdef RS_ENABLE_VK
                if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
                    RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
                    canvas_->clipRegion(region);
                } else {
                    SkPath dirtyPath;
                    region.getBoundaryPath(&dirtyPath);
                    canvas_->clipPath(dirtyPath, true);
                }
#else
                SkPath dirtyPath;
                region.getBoundaryPath(&dirtyPath);
                canvas_->clipPath(dirtyPath, true);
#endif
            }
#else
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
                if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
                    RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
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
#endif
        } else {
#ifndef USE_ROSEN_DRAWING
            canvas_->clear(SK_ColorTRANSPARENT);
#else
            canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
#endif
        }
#endif

        RSPropertiesPainter::SetBgAntiAlias(true);
        if (!isParallel_ || isUIFirst_) {
#ifndef USE_ROSEN_DRAWING
            int saveCount = canvas_->save();
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
                canvas_->concat(geoPtr->GetMatrix());
            }
            if (needOffscreen) {
                // we are doing rotation animation, try offscreen render if capable
                displayNodeMatrix_ = canvas_->getTotalMatrix();
                PrepareOffscreenRender(node);
                ProcessChildren(node);
                FinishOffscreenRender();
            } else {
                // render directly
                ProcessChildren(node);
            }
            SwitchColorFilterDrawing(saveCount);
            canvas_->restoreToCount(saveCount);
#else
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
#endif
        }
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
            if ((isParallel_ && !isUIFirst_ && ((rects.size() > 0) || !isPartialRenderEnabled_)) && isCalcCostEnable_) {
                auto parallelRenderManager = RSParallelRenderManager::Instance();
                parallelRenderManager->CopyCalcCostVisitorAndPackTask(*this, node, isNeedCalcCost,
                    doAnimate_, isOpDropped_);
                if (parallelRenderManager->IsNeedCalcCost()) {
                    parallelRenderManager->LoadBalanceAndNotify(TaskType::CALC_COST_TASK);
                    parallelRenderManager->WaitCalcCostEnd();
                    parallelRenderManager->UpdateNodeCost(node);
                }
            }
            if (isParallel_ && !isUIFirst_ && ((rects.size() > 0) || !isPartialRenderEnabled_)) {
                ClearTransparentBeforeSaveLayer();
                auto parallelRenderManager = RSParallelRenderManager::Instance();
                parallelRenderManager->SetFrameSize(screenInfo_.width, screenInfo_.height);
                parallelRenderManager->CopyVisitorAndPackTask(*this, node);
                parallelRenderManager->LoadBalanceAndNotify(TaskType::PROCESS_TASK);
                parallelRenderManager->MergeRenderResult(*canvas_);
                parallelRenderManager->CommitSurfaceNum(node.GetChildrenCount());
                parallelRenderManager->ProcessFilterSurfaceRenderNode();
            }
        }
#endif
        if (saveLayerCnt > 0) {
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
#ifdef NEW_RENDER_CONTEXT
            RSTagTracker tagTracker(
                renderEngine_->GetDrawingContext()->GetDrawingContext(),
                RSTagTracker::TAGTYPE::TAG_RESTORELAYER_DRAW_NODE);
#else
#ifndef USE_ROSEN_DRAWING
            RSTagTracker tagTracker(
                renderEngine_->GetRenderContext()->GetGrContext(), RSTagTracker::TAGTYPE::TAG_RESTORELAYER_DRAW_NODE);
            RS_TRACE_NAME("RSUniRender:RestoreLayer");
            canvas_->restoreToCount(saveLayerCnt);
#else
            RSTagTracker tagTracker(renderEngine_->GetRenderContext()->GetDrGPUContext(),
                RSTagTracker::TAGTYPE::TAG_RESTORELAYER_DRAW_NODE);
            RS_TRACE_NAME("RSUniRender:RestoreLayer");
            canvas_->RestoreToCount(saveLayerCnt);
#endif
#endif
#endif
        }
        if (UNLIKELY(!unpairedTransitionNodes_.empty())) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode  unpairedTransitionNodes_ is not empty.");
            ProcessUnpairedSharedTransitionNode();
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

#ifdef DDGR_ENABLE_FEATURE_OPINC
        if (autoCacheDrawingEnable_) {
            DrawCacheRegionForDFX(autoCacheRenderNodeInfos_);
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
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
#ifndef USE_ROSEN_DRAWING
            canvas_->restoreToCount(saveCountBeforeClip);
#else
            canvas_->RestoreToCount(saveCountBeforeClip);
#endif
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
        UpdateHardwareEnabledInfoBeforeCreateLayer();
        AssignGlobalZOrderAndCreateLayer(appWindowNodesInZOrder_);
        node.SetGlobalZOrder(globalZOrder_++);
        processor_->ProcessDisplaySurface(node);
        auto& surfaceHandler = static_cast<RSSurfaceHandler&>(node);
        auto& fence = surfaceHandler.GetAcquireFence();
        auto subThreadManager = RSSubThreadManager::Instance();
        subThreadManager->SetFenceSubThread(fence);
        AssignGlobalZOrderAndCreateLayer(hardwareEnabledTopNodes_);
    }

#if defined(RS_ENABLE_DRIVEN_RENDER)
    if (drivenInfo_ && !drivenInfo_->prepareInfo.hasInvalidScene) {
        RS_TRACE_NAME("RSUniRender:DrivenRenderProcess");
        // process driven render tree
        drivenInfo_->processInfo = { processor_, newColorSpace_, node.GetGlobalZOrder() };
        RSDrivenRenderManager::GetInstance().DoProcessRenderTask(drivenInfo_->processInfo);
    }
#endif

    DoScreenRcdTask(processor_, rcdInfo_, screenInfo_);

    if (!RSMainThread::Instance()->WaitHardwareThreadTaskExcute()) {
        RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode: hardwareThread task has too many to excute");
    }
    processor_->PostProcess(&node);
    {
        std::lock_guard<std::mutex> lock(groupedTransitionNodesMutex);
        EraseIf(groupedTransitionNodes, [](auto& iter) -> bool {
            auto& [id, pair] = iter;
            if (pair.second.empty()) {
                return true;
            }
            const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
            auto node = nodeMap.GetRenderNode<RSRenderNode>(iter.first);
            return node ? (!node->IsOnTheTree()) : true;
        });
    }
    auto mainThread = RSMainThread::Instance();
    if (!mainThread->GetClearMemoryFinished()) {
        mainThread->ClearMemoryCache(mainThread->GetClearMoment(), mainThread->GetClearMemDeeply());
    }
    RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode end");
#ifdef RS_ENABLE_VK
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN) {
#ifdef USE_ROSEN_DRAWING
            renderEngine_->GetRenderContext()->GetDrGPUContext()->StoreVkPipelineCacheData();
#endif
        }
#endif
}

void RSUniRenderVisitor::DrawSurfaceLayer(const std::shared_ptr<RSDisplayRenderNode>& displayNode,
    const std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes) const
{
    auto subThreadManager = RSSubThreadManager::Instance();
    if (RSSingleton<RoundCornerDisplay>::GetInstance().GetRcdEnable()) {
        subThreadManager->StartRCDThread(renderEngine_->GetRenderContext().get());
    }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    subThreadManager->StartFilterThread(renderEngine_->GetRenderContext().get());
    subThreadManager->StartColorPickerThread(renderEngine_->GetRenderContext().get());
    subThreadManager->SubmitSubThreadTask(displayNode, subThreadNodes);
#endif
}

#ifndef USE_ROSEN_DRAWING
void RSUniRenderVisitor::SwitchColorFilterDrawing(int currentSaveCount)
{
    ColorFilterMode colorFilterMode = renderEngine_->GetColorFilterMode();
    if (colorFilterMode >= ColorFilterMode::INVERT_COLOR_ENABLE_MODE &&
        colorFilterMode <= ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE) {
        RS_LOGD("RsDebug RSBaseRenderEngine::SetColorFilterModeToPaint mode:%{public}d",
            static_cast<int32_t>(colorFilterMode));
        SkPaint paint;
        RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
#if defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)
#ifdef NEW_RENDER_CONTEXT
        RSTagTracker tagTracker(
            renderEngine_->GetDrawingContext()->GetDrawingContext(),
            RSTagTracker::TAG_SAVELAYER_COLOR_FILTER);
#else
        RSTagTracker tagTracker(
            renderEngine_->GetRenderContext()->GetGrContext(),
            RSTagTracker::TAG_SAVELAYER_COLOR_FILTER);
#endif
#endif
        SkCanvas::SaveLayerRec rec;
        rec.fSaveLayerFlags = SkCanvas::kInitWithPrevious_SaveLayerFlag;
        rec.fPaint = &paint;
        canvas_->saveLayer(rec);
        canvas_->restoreToCount(currentSaveCount);
    }
}
#else
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
#endif

void RSUniRenderVisitor::UpdateHardwareEnabledInfoBeforeCreateLayer()
{
    if (!IsHardwareComposerEnabled()) {
        return;
    }
    if (hardwareEnabledNodes_.empty()) {
        return;
    }
    if (isParallel_ && !isUIFirst_) {
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
            std::vector<std::shared_ptr<RSSurfaceRenderNode>>().swap(appWindowNodesInZOrder_);
            auto subThreadNum = RSParallelRenderManager::Instance()->GetParallelThreadNumber();
            auto appWindowNodesMap = RSParallelRenderManager::Instance()->GetAppWindowNodes();
            std::vector<std::shared_ptr<RSSurfaceRenderNode>> appWindowNodes;
            for (uint32_t i = 0; i < subThreadNum; i++) {
                appWindowNodes = appWindowNodesMap[i];
                appWindowNodesInZOrder_.insert(appWindowNodesInZOrder_.end(),
                    appWindowNodes.begin(), appWindowNodes.end());
            }
        }
#endif
    }
    globalZOrder_ = 0.0f;
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
#if !defined(USE_ROSEN_DRAWING)
    RS_OPTIONAL_TRACE_BEGIN("RSUniRender::GetSurface");
    auto skSurface = renderSurface->GetSurface();
    RS_OPTIONAL_TRACE_END();
    if (skSurface == nullptr) {
        RS_LOGE("RSUniRenderVisitor::AddOverDrawListener: skSurface is null");
        return;
    }
    if (skSurface->getCanvas() == nullptr) {
        ROSEN_LOGE("skSurface.getCanvas is null.");
        return;
    }
#else
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
#endif
#else
    if (renderFrame->GetFrame() == nullptr) {
        RS_LOGE("RSUniRenderVisitor::AddOverDrawListener: RSSurfaceFrame is nullptr");
        return;
    }
#if !defined(USE_ROSEN_DRAWING)
    RS_TRACE_BEGIN("RSUniRender::GetSurface");
    auto skSurface = renderFrame->GetFrame()->GetSurface();
    RS_TRACE_END();
    if (skSurface == nullptr) {
        RS_LOGE("RSUniRenderVisitor::AddOverDrawListener: skSurface is null");
        return;
    }
    if (skSurface->getCanvas() == nullptr) {
        ROSEN_LOGE("skSurface.getCanvas is null.");
        return;
    }
#else
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
#endif
#ifndef USE_ROSEN_DRAWING
    canvas_ = std::make_shared<RSPaintFilterCanvas>(skSurface.get());
#else
    canvas_ = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
#endif
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
        auto surfaceDirtyManager = surfaceNode->GetDirtyManager();
        if (surfaceDirtyManager == nullptr) {
            continue;
        }
        RectI surfaceDirtyRect = surfaceDirtyManager->GetCurrentFrameDirtyRegion();

        if (surfaceNode->HasContainerWindow()) {
            // If a surface's dirty is intersect with container region (which can be considered transparent)
            // should be added to display dirty region.
            // Note: we use containerRegion rather transparentRegion to bypass inner corner dirty problem.
            auto containerRegion = surfaceNode->GetContainerRegion();
            auto surfaceDirtyRegion = Occlusion::Region{Occlusion::Rect{surfaceDirtyRect}};
            auto containerDirtyRegion = containerRegion.And(surfaceDirtyRegion);
            if (!containerDirtyRegion.IsEmpty()) {
                RS_LOGD("CalcDirtyDisplayRegion merge containerDirtyRegion %{public}s region %{public}s",
                    surfaceNode->GetName().c_str(), containerDirtyRegion.GetRegionInfo().c_str());
                // plan: we can use surfacenode's absrect as containerRegion's bound
                const auto& rect = containerRegion.GetBoundRef();
                displayDirtyManager->MergeDirtyRect(
                    RectI{ rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_ });
            }
        } else {
            // warning: if a surfacenode has transparent region and opaque region, and its dirty pattern appears in
            // transparent region and opaque region in adjacent frame, may cause displaydirty region incomplete after
            // merge history (as surfacenode's dirty region merging opaque region will enlarge surface dirty region
            // which include transparent region but not counted in display dirtyregion)
            if (!surfaceNode->IsNodeDirty()) {
                continue;
            }
            auto transparentRegion = surfaceNode->GetTransparentRegion();
            Occlusion::Rect tmpRect = Occlusion::Rect { surfaceDirtyRect.left_, surfaceDirtyRect.top_,
                surfaceDirtyRect.GetRight(), surfaceDirtyRect.GetBottom() };
            Occlusion::Region surfaceDirtyRegion { tmpRect };
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

#ifdef RS_ENABLE_EGLQUERYSURFACE
std::vector<RectI> RSUniRenderVisitor::GetDirtyRects(const Occlusion::Region &region)
{
    const std::vector<Occlusion::Rect>& rects = region.GetRegionRects();
    std::vector<RectI> retRects;
    for (const Occlusion::Rect& rect : rects) {
        // origin transformation
#ifdef RS_ENABLE_VK
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
            retRects.emplace_back(RectI(rect.left_, rect.top_,
                rect.right_ - rect.left_, rect.bottom_ - rect.top_));
        } else {
            retRects.emplace_back(RectI(rect.left_, screenInfo_.GetRotatedHeight() - rect.bottom_,
                rect.right_ - rect.left_, rect.bottom_ - rect.top_));
        }
#else
        retRects.emplace_back(RectI(rect.left_, screenInfo_.GetRotatedHeight() - rect.bottom_,
            rect.right_ - rect.left_, rect.bottom_ - rect.top_));
#endif
    }
    RS_LOGD("GetDirtyRects size %{public}d %{public}s", region.GetSize(), region.GetRegionInfo().c_str());
    return retRects;
}
#endif

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
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
        node.InitCacheSurface(canvas_ ? canvas_->recordingContext() : nullptr, func, threadIndex_);
#else
        node.InitCacheSurface(canvas_ ? canvas_->getGrContext() : nullptr, func, threadIndex_);
#endif
#else
        node.InitCacheSurface(canvas_ ? canvas_->GetGPUContext().get() : nullptr, func, threadIndex_);
#endif
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

#ifndef USE_ROSEN_DRAWING
    cacheCanvas->clear(SK_ColorTRANSPARENT);
#else
    cacheCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
#endif

    swap(cacheCanvas, canvas_);
    // When cacheType == CacheType::ANIMATE_PROPERTY,
    // we should draw AnimateProperty on cacheCanvas
    if (cacheType == CacheType::ANIMATE_PROPERTY) {
        if (node.GetRenderProperties().IsShadowValid()
            && !node.GetRenderProperties().IsSpherizeValid()) {
#ifndef USE_ROSEN_DRAWING
            canvas_->save();
            canvas_->translate(node.GetShadowRectOffsetX(), node.GetShadowRectOffsetY());
#else
            canvas_->Save();
            canvas_->Translate(node.GetShadowRectOffsetX(), node.GetShadowRectOffsetY());
#endif
        }
        node.ProcessAnimatePropertyBeforeChildren(*canvas_);
    }
    if (node.IsNodeGroupIncludeProperty()) {
        node.ProcessAnimatePropertyBeforeChildren(*canvas_);
    }
    if (node.GetDrawingCacheType() != RSDrawingCacheType::DISABLED_CACHE) {
        node.SetDrawingCacheRootId(node.GetId());
    }
    node.ProcessRenderContents(*canvas_);
    // Set a count to label the ProcessChildren in updateCacheProcess
    updateCacheProcessCnt_++;
    ProcessChildren(node);
    updateCacheProcessCnt_--;

    if (cacheType == CacheType::ANIMATE_PROPERTY) {
        if (node.GetRenderProperties().IsShadowValid()
            && !node.GetRenderProperties().IsSpherizeValid()) {
#ifndef USE_ROSEN_DRAWING
            canvas_->restore();
#else
            canvas_->Restore();
#endif
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
            if (curCacheFilterRects_.empty() || !node.ChildHasFilter()) {
                // no filter to draw, return
                return true;
            }
        } else if (node.GetRenderProperties().GetShadowColorStrategy() !=
            SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE) {
            // clear hole while generating cache surface
#ifndef USE_ROSEN_DRAWING
            SkAutoCanvasRestore arc(canvas_.get(), true);
            RectI shadowRect;
            auto rrect = node.GetRenderProperties().GetRRect();
            RSPropertiesPainter::GetShadowDirtyRect(shadowRect, node.GetRenderProperties(), &rrect, false, false);
            SkCanvasPriv::ResetClip(canvas_.get());
            canvas_->clipRect(SkRect::MakeXYWH(shadowRect.left_, shadowRect.top_,
                shadowRect.width_, shadowRect.height_));
            canvas_->clear(SK_ColorTRANSPARENT);
#else
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
#endif
        } else if (node.GetRenderProperties().GetBackgroundFilter() || node.GetRenderProperties().GetUseEffect()) {
            // clear hole while generating cache surface
#ifndef USE_ROSEN_DRAWING
            SkAutoCanvasRestore arc(canvas_.get(), true);
            if (node.GetRenderProperties().GetClipBounds() != nullptr) {
                canvas_->clipRect(RSPropertiesPainter::Rect2SkRect(node.GetRenderProperties().GetBoundsRect()));
            } else {
                canvas_->clipRRect(RSPropertiesPainter::RRect2SkRRect(node.GetRenderProperties().GetRRect()));
            }
            canvas_->clear(SK_ColorTRANSPARENT);
#else
            Drawing::AutoCanvasRestore arc(*canvas_, true);
            if (node.GetRenderProperties().GetClipBounds() != nullptr) {
                canvas_->ClipRect(RSPropertiesPainter::Rect2DrawingRect(node.GetRenderProperties().GetBoundsRect()),
                    Drawing::ClipOp::INTERSECT, false);
            } else {
                canvas_->ClipRoundRect(RSPropertiesPainter::RRect2DrawingRRect(node.GetRenderProperties().GetRRect()),
                    Drawing::ClipOp::INTERSECT, false);
            }
            canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
#endif
        }
    } else if (curGroupedNodes_.empty() && !node.ChildHasFilter()) {
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
    if (node.LeashWindowRelatedAppWindowOccluded(appNode)) {
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
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(canvas_.get(), true);

    if (displayNodeMatrix_.has_value()) {
        auto& displayNodeMatrix = displayNodeMatrix_.value();
        canvas_->concat(displayNodeMatrix);
    }
    node.SetTotalMatrix(canvas_->getTotalMatrix());

    auto dstRect = node.GetDstRect();
    SkIRect dst = { dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(), dstRect.GetBottom() };
#else
    Drawing::AutoCanvasRestore acr(*canvas_.get(), true);

    if (displayNodeMatrix_.has_value()) {
        auto& displayNodeMatrix = displayNodeMatrix_.value();
        canvas_->ConcatMatrix(displayNodeMatrix);
    }
    node.SetTotalMatrix(canvas_->GetTotalMatrix());

    auto dstRect = node.GetDstRect();
    Drawing::RectI dst = { dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(),
                           dstRect.GetBottom() };
#endif
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
#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (autoCacheEnable_) {
        node.GetAutoCache->OpIncStateInitWithSurface(nodeCacheType_, opincRootRect_, isDiscardSurface_, isSubThread_);
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
#ifndef USE_ROSEN_DRAWING
            canvas_->concat(node.contextMatrix_.value_or(SkMatrix::I()));
            SkPaint paint;
            paint.setARGB(0x80, 0xFF, 0, 0); // transparent red
            canvas_->drawRect(node.contextClipRect_.value(), paint);
#else
            canvas_->ConcatMatrix(node.contextMatrix_.value_or(Drawing::Matrix()));
            Drawing::Brush brush;
            brush.SetARGB(0x80, 0xFF, 0, 0); // transparent red
            canvas_->AttachBrush(brush);
            canvas_->DrawRect(node.contextClipRect_.value());
            canvas_->DetachBrush();
#endif
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
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_RENDER_CONTEXT
    auto grContext = renderEngine_->GetDrawingContext()->GetDrawingContext();
#else
    auto grContext = canvas_ != nullptr ? static_cast<GrDirectContext*>(canvas_->recordingContext()) : nullptr;
#endif
    RSTagTracker tagTracker(grContext, node.GetId(), RSTagTracker::TAGTYPE::TAG_DRAW_SURFACENODE);
    node.SetGrContext(grContext);
#else
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
#endif
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
#ifndef USE_ROSEN_DRAWING
        node.SetGrContext(renderEngine_->GetSkContext().get());
#else
        node.SetDrawingGPUContext(renderEngine_->GetSkContext().get());
#endif
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
#ifdef RS_ENABLE_EGLQUERYSURFACE
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
#endif
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

#ifdef RS_ENABLE_EGLQUERYSURFACE
    // when display is in rotation state, occlusion relationship will be ruined,
    // hence visibleRegions cannot be used.
    if (isOpDropped_ && node.IsAppWindow()) {
        const auto& visibleRegions = node.GetVisibleRegion().GetRegionRects();
        if (visibleRegions.size() == 1) {
#ifndef USE_ROSEN_DRAWING
            canvas_->SetVisibleRect(SkRect::MakeLTRB(
                visibleRegions[0].left_, visibleRegions[0].top_, visibleRegions[0].right_, visibleRegions[0].bottom_));
#else
            canvas_->SetVisibleRect(Drawing::Rect(
                visibleRegions[0].left_, visibleRegions[0].top_, visibleRegions[0].right_, visibleRegions[0].bottom_));
#endif
        }
    }
#endif

    // when surfacenode named "CapsuleWindow", cache the current canvas as SkImage for screen recording
    if (!isSecurityDisplay_ && canvas_->GetSurface() != nullptr &&
        node.GetName().find(CAPTURE_WINDOW_NAME) != std::string::npos) {
#ifndef USE_ROSEN_DRAWING
        resetRotate_ = CheckIfNeedResetRotate();
        cacheImgForCapture_ = canvas_->GetSurface()->makeImageSnapshot();
#else
        resetRotate_ = CheckIfNeedResetRotate();
        cacheImgForCapture_ = canvas_->GetSurface()->GetImageSnapshot();
#endif
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
#ifndef USE_ROSEN_DRAWING
    if (isSelfDrawingSurface && !property.IsSpherizeValid()) {
        canvas_->save();
    }

    canvas_->concat(geoPtr->GetMatrix());
#else
    if (isSelfDrawingSurface && !property.IsSpherizeValid()) {
        canvas_->Save();
    }

    canvas_->ConcatMatrix(geoPtr->GetMatrix());
#endif
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
                    RSUniRenderUtil::GetRotationDegreeFromMatrix(node.GetTotalMatrix()) % ROTATION_90 != 0) &&
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
#ifndef USE_ROSEN_DRAWING
                if (!node.IsHardwareEnabledTopSurface()) {
                    canvas_->clear(SK_ColorTRANSPARENT);
                }
#else
                if (!node.IsHardwareEnabledTopSurface()) {
                    canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
                }
#endif
                node.SetGlobalAlpha(canvas_->GetAlpha());
                ParallelRenderEnableHardwareComposer(node);
                RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode src:%{public}s, dst:%{public}s name:%{public}s"
                    " id:%{public}" PRIu64 "", node.GetSrcRect().ToString().c_str(),
                    node.GetDstRect().ToString().c_str(), node.GetName().c_str(), node.GetId());
            } else {
                node.SetGlobalAlpha(1.0f);
                auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false, threadIndex_);
                params.targetColorGamut = newColorSpace_;
#ifdef USE_VIDEO_PROCESSING_ENGINE
                auto screenManager = CreateOrGetScreenManager();
                auto ancestor = node.GetAncestorDisplayNode().lock()->ReinterpretCastTo<RSDisplayRenderNode>();
                params.screenBrightnessNits = screenManager->GetScreenBrightnessNits(ancestor->GetScreenId());
#endif
                auto bgColor = property.GetBackgroundColor();
                if ((node.GetSelfDrawingNodeType() != SelfDrawingNodeType::VIDEO) &&
                    (bgColor != RgbPalette::Transparent())) {
                    auto bounds = RSPropertiesPainter::Rect2DrawingRect(property.GetBoundsRect());
                    Drawing::SaveLayerOps layerOps(&bounds, nullptr);
                    canvas_->SaveLayer(layerOps);
                    canvas_->SaveAlpha();
                    canvas_->SetAlpha(1.0f);
                    Drawing::Brush brush;
                    brush.SetColor(Drawing::Color(bgColor.AsArgbInt()));
                    canvas_->AttachBrush(brush);
                    canvas_->DrawRoundRect(RSPropertiesPainter::RRect2DrawingRRect(property.GetRRect()));
                    canvas_->DetachBrush();
                    renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
                    canvas_->RestoreAlpha();
                    canvas_->Restore();
                } else {
                    renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
                }
            }
        }

        if (isSelfDrawingSurface) {
#ifndef USE_ROSEN_DRAWING
            canvas_->restore();
#else
            canvas_->Restore();
#endif
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
#ifndef USE_ROSEN_DRAWING
        canvas_->SetVisibleRect(SkRect::MakeLTRB(0, 0, 0, 0));
#else
        canvas_->SetVisibleRect(Drawing::Rect(0, 0, 0, 0));
#endif

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
#ifndef USE_ROSEN_DRAWING
        SkPaint paint;
        paint.setARGB(0x80, 0, 0xFF, 0); // transparent green
        canvas_->drawRect(node.contextClipRect_.value(), paint);
#else
        Drawing::Brush brush;
        brush.SetARGB(0x80, 0, 0xFF, 0); // transparent green
        canvas_->AttachBrush(brush);
        canvas_->DrawRect(node.contextClipRect_.value());
        canvas_->DetachBrush();
#endif
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
#ifndef USE_ROSEN_DRAWING
    int saveCount = canvas_->save();
    ProcessCanvasRenderNode(node);
    canvas_->restoreToCount(saveCount);
#else
    int saveCount = canvas_->Save();
    ProcessCanvasRenderNode(node);
    canvas_->RestoreToCount(saveCount);
#endif
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
            {
                std::lock_guard<std::mutex> lock(groupedTransitionNodesMutex);
                groupedTransitionNodes.erase(node.GetId());
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
        {
            std::lock_guard<std::mutex> lock(groupedTransitionNodesMutex);
            groupedTransitionNodes.erase(node.GetId());
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
            {
                std::lock_guard<std::mutex> lock(groupedTransitionNodesMutex);
                groupedTransitionNodes[node.GetId()] = { val, {} };
            }
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
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore arc(canvas_.get(), true);
#else
    Drawing::AutoCanvasRestore arc(*canvas_, true);
#endif
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
                std::lock_guard<std::mutex> lock(groupedTransitionNodesMutex);
                groupedTransitionNodes[node.GetId()] = { val, {} };
            }
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
                groupedTransitionNodes.erase(node.GetId());
                cacheReuseTimes = 0;
                return;
            }
            RenderParam val { node.shared_from_this(), canvas_->GetCanvasStatus() };
            curGroupedNodes_.push(val);
            {
                std::lock_guard<std::mutex> lock(groupedTransitionNodesMutex);
                groupedTransitionNodes[node.GetId()] = { val, {} };
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
    processedCanvasNodeInCurrentSurface_++;
    if (!node.ShouldPaint() ||
#ifndef USE_ROSEN_DRAWING
        (canvas_ && canvas_->getDeviceClipBounds().isEmpty() && hardwareEnabledNodes_.empty())) {
#else
        (canvas_ && canvas_->GetDeviceClipBounds().IsEmpty() && hardwareEnabledNodes_.empty())) {
#endif
        return;
    }
    auto geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
    if (isSkipCanvasNodeOutOfScreen_ && !isSubNodeOfSurfaceInProcess_ && !node.HasSubSurface() &&
        geoPtr && IsOutOfScreenRegion(geoPtr->GetAbsRect()) && !isSubThread_) {
        return;
    }
    node.MarkNodeSingleFrameComposer(isNodeSingleFrameComposer_);
#ifdef RS_ENABLE_EGLQUERYSURFACE
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
#endif
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
            return;
        }
    }
#endif
#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (autoCacheEnable_) {
        if (node->GetAutoCache()->BeforeDrawCache(nodeCacheType_, autoCacheRenderNodeInfos_,
            autoCacheDrawingEnable_, isDiscardSurface_, canvas_, screenInfo_.width, screenInfo_.height)) {
            return;
        }
    }
#endif
    // in case preparation'update is skipped
#ifndef USE_ROSEN_DRAWING
    canvas_->save();
#else
    canvas_->Save();
#endif
    if (node.GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        RSUniRenderUtil::FloorTransXYInCanvasMatrix(*canvas_);
    }

    if (node.GetSharedTransitionParam().has_value()) {
        // draw self and children in sandbox which will not be affected by parent's transition
        const auto& sandboxMatrix = node.GetRenderProperties().GetSandBoxMatrix();
        if (sandboxMatrix) {
#ifndef USE_ROSEN_DRAWING
            canvas_->setMatrix(*sandboxMatrix);
#else
            canvas_->SetMatrix(*sandboxMatrix);
#endif
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
        if (autoCacheEnable_) {
            node->GetAutoCache()->AfterDrawCache(nodeCacheType_);
        }
#endif
        return;
    }
    if (auto drawingNode = node.ReinterpretCastTo<RSCanvasDrawingRenderNode>()) {
        std::lock_guard<std::mutex> lock(drawingMutex_);
#ifndef USE_ROSEN_DRAWING
        auto clearFunc = [id = threadIndex_](sk_sp<SkSurface> surface) {
            // The second param is null, 0 is an invalid value.
            RSUniRenderUtil::ClearNodeCacheSurface(std::move(surface), nullptr, id, 0);
        };
#else
        auto clearFunc = [id = threadIndex_](std::shared_ptr<Drawing::Surface> surface) {
            // The second param is null, 0 is an invalid value.
            RSUniRenderUtil::ClearNodeCacheSurface(std::move(surface), nullptr, id, 0);
        };
#endif
        drawingNode->SetSurfaceClearFunc({ threadIndex_, clearFunc });
    }
    CheckAndSetNodeCacheType(node);
#ifdef DDGR_ENABLE_FEATURE_OPINC
        if (autoCacheEnable_) {
            node->GetAutoCache()->BeforeDrawCache(canvas_, opincRootRect_, isOpincDropNodeExt_);
        }
#endif
    DrawChildCanvasRenderNode(node);
#ifdef DDGR_ENABLE_FEATURE_OPINC
        if (autoCacheEnable_) {
            node->GetAutoCache()->AfterDrawCache(canvas_, isOpincDropNodeExt_);
        }
#endif
    notRunCheckAndSetNodeCacheType_ = false;
#ifndef USE_ROSEN_DRAWING
    canvas_->restore();
#else
    canvas_->Restore();
#endif
#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (autoCacheEnable_) {
        node->GetAutoCache()->AfterDrawCache(canvas_, isDiscardSurface_, nodeCacheType_, true);
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
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(canvas_.get(), true);
#else
    Drawing::AutoCanvasRestore acr(*canvas_.get(), true);
#endif
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
#ifndef USE_ROSEN_DRAWING
    if (canvas_->GetSurface() == nullptr) {
        canvas_->clipRect(SkRect::MakeWH(offscreenWidth, offscreenHeight));
        RS_LOGD("RSUniRenderVisitor::PrepareOffscreenRender, current surface is nullptr (software renderer?)");
        return;
    }
    // create offscreen surface and canvas
    offscreenSurface_ = canvas_->GetSurface()->makeSurface(offscreenWidth, offscreenHeight);
    if (offscreenSurface_ == nullptr) {
        RS_LOGD("RSUniRenderVisitor::PrepareOffscreenRender, offscreenSurface is nullptr");
        canvas_->clipRect(SkRect::MakeWH(offscreenWidth, offscreenHeight));
        return;
    }
    auto offscreenCanvas = std::make_shared<RSPaintFilterCanvas>(offscreenSurface_.get());

    // copy current canvas properties into offscreen canvas
    offscreenCanvas->CopyConfiguration(*canvas_);

    // backup current canvas and replace with offscreen canvas
    canvasBackup_ = std::exchange(canvas_, offscreenCanvas);
#else
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
#endif
}

void RSUniRenderVisitor::FinishOffscreenRender(bool isMirror)
{
    if (canvasBackup_ == nullptr) {
        RS_LOGD("RSUniRenderVisitor::FinishOffscreenRender, canvasBackup_ is nullptr");
        return;
    }
    RS_TRACE_NAME("RSUniRenderVisitor::OffscreenRender finish");
    // flush offscreen canvas, maybe unnecessary
#ifndef USE_ROSEN_DRAWING
    if (!isMirror) {
        canvas_->flush();
    }
    // draw offscreen surface to current canvas
    SkPaint paint;
    paint.setAntiAlias(true);
#ifdef NEW_SKIA
    canvasBackup_->drawImage(offscreenSurface_->makeImageSnapshot(), 0, 0, SkSamplingOptions(), &paint);
#else
    canvasBackup_->drawImage(offscreenSurface_->makeImageSnapshot(), 0, 0, &paint);
#endif
#else
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
#endif
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

bool RSUniRenderVisitor::AdaptiveSubRenderThreadMode(bool doParallel)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    doParallel = (doParallel && (parallelRenderType_ != ParallelRenderingType::DISABLE));
    if (!doParallel) {
        return doParallel;
    }
    auto parallelRenderManager = RSParallelRenderManager::Instance();
    switch (parallelRenderType_) {
        case ParallelRenderingType::AUTO:
            parallelRenderManager->SetParallelMode(doParallel);
            break;
        case ParallelRenderingType::DISABLE:
            parallelRenderManager->SetParallelMode(false);
            break;
        case ParallelRenderingType::ENABLE:
            parallelRenderManager->SetParallelMode(true);
            break;
    }
    return doParallel;
#else
    return false;
#endif
}
void RSUniRenderVisitor::ParallelRenderEnableHardwareComposer(RSSurfaceRenderNode& node)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined (RS_ENABLE_GL)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        if (isParallel_ && !isUIFirst_) {
            const auto& property = node.GetRenderProperties();
            auto dstRect = node.GetDstRect();
            RectF clipRect = {dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetWidth(), dstRect.GetHeight()};
            RSParallelRenderManager::Instance()->AddSelfDrawingSurface(parallelRenderVisitorIndex_,
                property.GetCornerRadius().IsZero(), clipRect, property.GetCornerRadius());
        }
    }
#endif
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
    processor_->PostProcess(displayNode.get());
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
#ifndef USE_ROSEN_DRAWING
        sk_sp<SkImage> skImage = RSMainThread::Instance()->GetWatermarkImg();
        if (!skImage) {
            RS_LOGE("RSUniRenderVisitor::DrawWatermarkIfNeed: no image!");
            return;
        }
        SkPaint rectPaint;
        auto skSrcRect = SkRect::MakeWH(skImage->width(), skImage->height());
        auto skDstRect = SkRect::MakeWH(mainWidth, mainHeight);
#ifdef NEW_SKIA
        canvas_->drawImageRect(
            skImage, skSrcRect, skDstRect, SkSamplingOptions(), &rectPaint, SkCanvas::kStrict_SrcRectConstraint);
#else
        canvas_->drawImageRect(skImage, skSrcRect, skDstRect, &rectPaint);
#endif
#else
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
#endif
    }
}

void RSUniRenderVisitor::SetAppWindowNum(uint32_t num)
{
    appWindowNum_ = num;
}

bool RSUniRenderVisitor::ParallelComposition(const std::shared_ptr<RSBaseRenderNode> rootNode)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined (RS_ENABLE_GL)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return false;
    }
    auto parallelRenderManager = RSParallelRenderManager::Instance();
    parallelRenderManager->SetParallelMode(true);
    if (parallelRenderManager->GetParallelMode()) {
        parallelRenderManager->PackParallelCompositionTask(shared_from_this(), rootNode);
        parallelRenderManager->LoadBalanceAndNotify(TaskType::COMPOSITION_TASK);
        parallelRenderManager->WaitCompositionEnd();
    } else {
        return false;
    }
    return true;
#else
    return false;
#endif
}

void RSUniRenderVisitor::PrepareSharedTransitionNode(RSBaseRenderNode& node)
{
    // set flag for surface node whose children contain shared transition node
    if (curSurfaceNode_) {
        SetHasSharedTransitionNode(*curSurfaceNode_, true);
    }

    // Sanity check done by caller, transitionParam should always has value.
    auto& transitionParam = node.GetSharedTransitionParam();

    auto pairedNode = transitionParam->second.lock();
    if (pairedNode == nullptr) {
        // paired node is already destroyed, clear transition param and prepare directly
        node.SetSharedTransitionParam(std::nullopt);
        return;
    }

    auto& pairedParam = pairedNode->GetSharedTransitionParam();
    if (!pairedParam.has_value() || pairedParam->first != transitionParam->first) {
        // if 1. paired node is not a transition node or 2. paired node is not paired with this node, then clear
        // transition param and prepare directly
        node.SetSharedTransitionParam(std::nullopt);
        return;
    }

    // hack to ensure that dirty region will include the whole shared-transition nodes, and won't be clipped by parent
    // clip rect.
    prepareClipRect_.SetAll(0, 0, INT_MAX, INT_MAX);
}

bool RSUniRenderVisitor::ProcessSharedTransitionNode(RSBaseRenderNode& node)
{
    // Sanity check done by caller, transitionParam should always has value.
    auto& transitionParam = node.GetSharedTransitionParam();
    if (LIKELY(!transitionParam.has_value())) {
        // non-transition node, prepare directly
        return true;
    }

    // Note: Sanity checks for shared transition nodes are already done in prepare phase, no need to do it again.
    // use transition key (in node id) as map index.
    auto key = transitionParam->first;
    // paired node is already visited, process both nodes in order.
    if (auto existingNodeIter = unpairedTransitionNodes_.find(key);
        existingNodeIter != unpairedTransitionNodes_.end()) {
        RSAutoCanvasRestore acr(canvas_);
        // restore render context and process the paired node.
        auto& [node, canvasStatus] = existingNodeIter->second;
        canvas_->SetCanvasStatus(canvasStatus);
        node->Process(shared_from_this());
        unpairedTransitionNodes_.erase(existingNodeIter);
        return true;
    }
    groupedTransitionNodesType nodes;
    {
        std::lock_guard<std::mutex> lock(groupedTransitionNodesMutex);
        nodes = groupedTransitionNodes;
    }
    for (auto& [unused, pair] : nodes) {
        if (auto existingNodeIter = pair.second.find(key); existingNodeIter != pair.second.end()) {
            RSAutoCanvasRestore acr(canvas_);
            // restore render context and process the paired node.
            auto& [unused2, PreCanvasStatus] = pair.first;
            auto& [child, canvasStatus] = existingNodeIter->second;
            canvas_->SetCanvasStatus(canvasStatus);
            canvas_->MultiplyAlpha(PreCanvasStatus.alpha_);
#ifndef USE_ROSEN_DRAWING
            canvas_->concat(PreCanvasStatus.matrix_);
#else
            canvas_->ConcatMatrix(PreCanvasStatus.matrix_);
#endif
            child->Process(shared_from_this());
            return true;
        }
    }

    auto pairedNode = transitionParam->second.lock();
    if (pairedNode == nullptr || pairedNode->GetGlobalAlpha() <= 0.0f) {
        // visitor may never visit the paired node, ignore the transition logic and process directly.
        return true;
    }

    if (!curGroupedNodes_.empty()) {
        // if in node group cache, add this node and render params (alpha and matrix) into groupedTransitionNodes.
        auto& [child, currentStatus] = curGroupedNodes_.top();
        auto canvasStatus = canvas_->GetCanvasStatus();
        if (!ROSEN_EQ(currentStatus.alpha_, 0.f)) {
            canvasStatus.alpha_ /= currentStatus.alpha_;
        } else {
            RS_LOGE("RSUniRenderVisitor::ProcessSharedTransitionNode: alpha_ is zero");
        }
#ifndef USE_ROSEN_DRAWING
        if (!currentStatus.matrix_.invert(&canvasStatus.matrix_)) {
            RS_LOGE("RSUniRenderVisitor::ProcessSharedTransitionNode invert failed");
        }
#else
        if (!currentStatus.matrix_.Invert(canvasStatus.matrix_)) {
            RS_LOGE("RSUniRenderVisitor::ProcessSharedTransitionNode invert failed");
        }
#endif
        RenderParam value { node.shared_from_this(), canvasStatus };
        {
            std::lock_guard<std::mutex> lock(groupedTransitionNodesMutex);
            groupedTransitionNodes[child->GetId()].second.emplace(key, std::move(value));
        }
        return false;
    }

    // all sanity checks passed, add this node and render params (alpha and matrix) into unpairedTransitionNodes_.
    RenderParam value { node.shared_from_this(), canvas_->GetCanvasStatus() };
    unpairedTransitionNodes_.emplace(key, std::move(value));

    // skip processing the current node and all its children.
    return false;
}

void RSUniRenderVisitor::ProcessUnpairedSharedTransitionNode()
{
    // Do cleanup for unpaired transition nodes.
    for (auto& [key, params] : unpairedTransitionNodes_) {
        RSAutoCanvasRestore acr(canvas_);
        // restore render context and process the unpaired node.
        auto& [node, canvasStatus] = params;
        canvas_->SetCanvasStatus(canvasStatus);
        node->Process(shared_from_this());
        // clear transition param
        node->SetSharedTransitionParam(std::nullopt);
    }
    unpairedTransitionNodes_.clear();
}
#ifdef ENABLE_RECORDING_DCL
#ifndef USE_ROSEN_DRAWING
void RSUniRenderVisitor::tryCapture(float width, float height)
{
    if (!RSSystemProperties::GetRecordingEnabled()) {
        return;
    }
    recordingCanvas_ = std::make_unique<RSRecordingCanvas>(width, height);
    RS_TRACE_NAME("RSUniRender:Recording begin");
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
#ifdef NEW_SKIA
    recordingCanvas_->SetGrRecordingContext(canvas_->recordingContext());
#else
    recordingCanvas_->SetGrContext(canvas_->getGrContext()); // SkImage::MakeFromCompressed need GrContext
#endif
#endif
    canvas_->addCanvas(recordingCanvas_.get());
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
#else
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
#endif

void RSUniRenderVisitor::RotateMirrorCanvasIfNeed(RSDisplayRenderNode& node)
{
    auto mirrorNode = node.GetMirrorSource().lock();
    auto rotation = mirrorNode->GetScreenRotation();
    if (RSSystemProperties::IsFoldScreenFlag() && mirrorNode->GetScreenId() == 0) {
        if (rotation == ScreenRotation::ROTATION_270) {
            rotation = ScreenRotation::ROTATION_0;
        } else {
            rotation = static_cast<ScreenRotation>(static_cast<int>(rotation) + 1);
        }
    }
#ifndef USE_ROSEN_DRAWING
    if (rotation != ScreenRotation::ROTATION_0) {
        auto screenManager = CreateOrGetScreenManager();
        auto mainScreenInfo = screenManager->QueryScreenInfo(mirrorNode->GetScreenId());
        if (rotation == ScreenRotation::ROTATION_90) {
            canvas_->rotate(90);
            canvas_->translate(0, -(static_cast<float>(mainScreenInfo.height)));
        } else if (rotation == ScreenRotation::ROTATION_180) {
            canvas_->rotate(180, static_cast<float>(mainScreenInfo.width) / 2,
                static_cast<float>(mainScreenInfo.height) / 2);
        } else if (rotation == ScreenRotation::ROTATION_270) {
            canvas_->rotate(270);
            canvas_->translate(-(static_cast<float>(mainScreenInfo.width)), 0);
        }
    }
#else
    if (rotation != ScreenRotation::ROTATION_0) {
        auto screenManager = CreateOrGetScreenManager();
        auto mainScreenInfo = screenManager->QueryScreenInfo(mirrorNode->GetScreenId());
        if (rotation == ScreenRotation::ROTATION_90) {
            canvas_->Rotate(90, 0, 0);
            canvas_->Translate(0, -(static_cast<float>(mainScreenInfo.height)));
        } else if (rotation == ScreenRotation::ROTATION_180) {
            canvas_->Rotate(180, static_cast<float>(mainScreenInfo.width) / 2,
                static_cast<float>(mainScreenInfo.height) / 2);
        } else if (rotation == ScreenRotation::ROTATION_270) {
            canvas_->Rotate(270, 0, 0);
            canvas_->Translate(-(static_cast<float>(mainScreenInfo.width)), 0);
        }
    }
#endif
}

void RSUniRenderVisitor::ScaleMirrorIfNeed(RSDisplayRenderNode& node, bool canvasRotation)
{
    auto screenManager = CreateOrGetScreenManager();
    auto mirrorNode = node.GetMirrorSource().lock();
    auto mainScreenInfo = screenManager->QueryScreenInfo(mirrorNode->GetScreenId());
    float mainWidth = static_cast<float>(mainScreenInfo.width);
    float mainHeight = static_cast<float>(mainScreenInfo.height);
    if (canvasRotation) {
        if ((RSSystemProperties::IsFoldScreenFlag() && mirrorNode->GetScreenId() == 0) ||
            mirrorNode->GetScreenRotation() == ScreenRotation::ROTATION_90 ||
            mirrorNode->GetScreenRotation() == ScreenRotation::ROTATION_270) {
            std::swap(mainWidth, mainHeight);
        }
    } else {
        if ((RSSystemProperties::IsFoldScreenFlag() && mirrorNode->GetScreenId() == 0) ||
            node.GetOriginScreenRotation() == ScreenRotation::ROTATION_90 ||
            node.GetOriginScreenRotation() == ScreenRotation::ROTATION_270) {
            std::swap(mainWidth, mainHeight);
        }
        if ((RSSystemProperties::IsFoldScreenFlag() && mirrorNode->GetScreenId() == 0 &&
            (mirrorNode->GetScreenRotation() == ScreenRotation::ROTATION_90 ||
            mirrorNode->GetScreenRotation() == ScreenRotation::ROTATION_270)) || mirrorAutoRotate_) {
            std::swap(mainWidth, mainHeight);
        }
    }
    float boundsWidth = node.GetRenderProperties().GetBoundsWidth();
    float boundsHeight = node.GetRenderProperties().GetBoundsHeight();
    // If the width and height not match the main screen, calculate the dstRect.
    if (mainWidth != boundsWidth || mainHeight != boundsHeight) {
#ifndef USE_ROSEN_DRAWING
        canvas_->clear(SK_ColorBLACK);
#else
        canvas_->Clear(SK_ColorBLACK);
#endif
        float mirrorScale = 1.0f; // 1 for init scale
        float startX = 0.0f;
        float startY = 0.0f;
        if ((boundsHeight / boundsWidth) < (mainHeight / mainWidth)) {
            mirrorScale = boundsHeight / mainHeight;
            startX = (boundsWidth - (mirrorScale * mainWidth)) / 2; // 2 for calc X
        } else if ((boundsHeight / boundsWidth) > (mainHeight / mainWidth)) {
            mirrorScale = boundsWidth / mainWidth;
            startY = (boundsHeight - (mirrorScale * mainHeight)) / 2; // 2 for calc Y
        }
#ifndef USE_ROSEN_DRAWING
        canvas_->translate(startX, startY);
        canvas_->scale(mirrorScale, mirrorScale);
#else
        canvas_->Translate(startX, startY);
        canvas_->Scale(mirrorScale, mirrorScale);
#endif
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
#ifndef USE_ROSEN_DRAWING
    int angle = RSUniRenderUtil::GetRotationFromMatrix(canvas_->getTotalMatrix());
#else
    int angle = RSUniRenderUtil::GetRotationFromMatrix(canvas_->GetTotalMatrix());
#endif
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
