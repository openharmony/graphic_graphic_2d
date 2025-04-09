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
#include "common/rs_common_hook.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "common/rs_singleton.h"
#include "luminance/rs_luminance_control.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_display_render_params.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_draw_cmd.h"
#include "pipeline/rs_effect_render_node.h"
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
#include "pipeline/sk_resource_manager.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "platform/ohos/rs_jank_stats.h"
#include "platform/ohos/rs_node_stats.h"
#include "property/rs_properties_painter.h"
#include "property/rs_point_light_manager.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_skia_filter.h"
#include "system/rs_system_parameters.h"
#include "scene_board_judgement.h"
#include "hgm_core.h"
#include "benchmarks/rs_recording_thread.h"
#include "scene_board_judgement.h"
#include "metadata_helper.h"
#include <v1_0/buffer_handle_meta_key_type.h>
#include <v1_0/cm_color_space.h>

#include "pipeline/round_corner_display/rs_round_corner_display_manager.h"
#include "pipeline/round_corner_display/rs_message_bus.h"

#include "rs_profiler.h"
#ifdef RS_PROFILER_ENABLED
#include "rs_profiler_capture_recorder.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t PHONE_MAX_APP_WINDOW_NUM = 1;
constexpr int32_t CACHE_MAX_UPDATE_TIME = 2;
constexpr int32_t VISIBLEAREARATIO_FORQOS = 3;
constexpr int MAX_ALPHA = 255;
constexpr int TRACE_LEVEL_THREE = 3;
constexpr float EPSILON_SCALE = 0.00001f;
constexpr float CACHE_FILL_ALPHA = 0.2f;
constexpr float CACHE_UPDATE_FILL_ALPHA = 0.8f;
static const std::string CAPTURE_WINDOW_NAME = "CapsuleWindow";
constexpr const char* CLEAR_GPU_CACHE = "ClearGpuCache";
constexpr const char* RELIABLE_GESTURE_BACK_SURFACE_NAME = "SCBGestureBack";
constexpr int MIN_OVERLAP = 2;
static std::map<NodeId, uint32_t> cacheRenderNodeMap = {};
static uint32_t cacheReuseTimes = 0;
static std::mutex cacheRenderNodeMapMutex;
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

bool IsFirstFrameReadyToDraw(const RSSurfaceRenderNode& node)
{
    bool result = false;
    auto sortedChildren = node.GetSortedChildren();
    if (node.IsScbScreen() || node.IsSCBNode()) {
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

std::string VisibleDataToString(const VisibleData& val)
{
    std::stringstream ss;
    ss << "VisibleData[name, nodeId, visibleLevel]:";
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (const auto& v : val) {
        auto surfaceNode = nodeMap.GetRenderNode<RSSurfaceRenderNode>(v.first);
        auto name = surfaceNode ? surfaceNode->GetName() : "";
        ss << "[" << name << ", " << v.first << ", " << v.second << "], ";
    }
    return ss.str();
}
} // namespace

void DoScreenRcdTask(NodeId id, std::shared_ptr<RSProcessor>& processor, std::unique_ptr<RcdInfo>& rcdInfo,
    ScreenInfo& screenInfo_)
{
    if (screenInfo_.state != ScreenState::HDI_OUTPUT_ENABLE) {
        RS_LOGD("DoScreenRcdTask is not at HDI_OUPUT mode");
        return;
    }
    if (RSSingleton<RoundCornerDisplayManager>::GetInstance().GetRcdEnable()) {
        RSSingleton<RoundCornerDisplayManager>::GetInstance().RunHardwareTask(id,
            [id, &processor, &rcdInfo]() {
                auto hardInfo = RSSingleton<RoundCornerDisplayManager>::GetInstance().GetHardwareInfo(id);
                rcdInfo->processInfo = {processor, hardInfo.topLayer, hardInfo.bottomLayer,
                    hardInfo.resourceChanged};
                RSRcdRenderManager::GetInstance().DoProcessRenderMainThreadTask(id, rcdInfo->processInfo);
            });
    }
}

RSUniRenderVisitor::RSUniRenderVisitor()
    : curSurfaceDirtyManager_(std::make_shared<RSDirtyRegionManager>())
{
    PartialRenderOptionInit();
    auto mainThread = RSMainThread::Instance();
    renderEngine_ = mainThread->GetRenderEngine();
    hasMirrorDisplay_ = mainThread->HasMirrorDisplay();
    quickSkipPrepareType_ = RSSystemParameters::GetQuickSkipPrepareType();
    // when occlusion enabled is false, subTree do not skip, but not influence visible region
    isOcclusionEnabled_ = RSSystemProperties::GetOcclusionEnabled();
    isQuickSkipPreparationEnabled_ = (quickSkipPrepareType_ != QuickSkipPrepareType::DISABLED);
    isDrawingCacheEnabled_ = RSSystemParameters::GetDrawingCacheEnabled();
#ifdef DDGR_ENABLE_FEATURE_OPINC
    autoCacheEnable_ = RSSystemProperties::IsDdgrOpincEnable();
#endif
    RSTagTracker::UpdateReleaseResourceEnabled(RSSystemProperties::GetReleaseResourceEnabled());
    isScreenRotationAnimating_ = RSSystemProperties::GetCacheEnabledForRotation();
    isSubSurfaceEnabled_ = RSSystemProperties::GetSubSurfaceEnabled() && RSSystemProperties::IsPhoneType();
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
    isUIFirstDebugEnable_ = RSSystemProperties::GetUIFirstDebugEnabled();
    isPhone_ = RSMainThread::Instance()->GetDeviceType() == DeviceType::PHONE;
    isPc_ = RSMainThread::Instance()->GetDeviceType() == DeviceType::PC;
    isCurtainScreenOn_ = RSMainThread::Instance()->IsCurtainScreenOn();
    isPrevalidateHwcNodeEnable_ = RSSystemParameters::GetPrevalidateHwcNodeEnabled() &&
        RSUniHwcPrevalidateUtil::GetInstance().IsLoadSuccess();
    isOverdrawDfxOn_ = RSOverdrawController::GetInstance().IsEnabled();
}

void RSUniRenderVisitor::PartialRenderOptionInit()
{
    partialRenderType_ = RSSystemProperties::GetUniPartialRenderEnabled();
    isPartialRenderEnabled_ = (partialRenderType_ != PartialRenderType::DISABLED);
    isCompleteRenderEnabled_ = (partialRenderType_ == PartialRenderType::SET_DAMAGE_BUT_COMPLETE_RENDER);
    dirtyRegionDebugType_ = RSSystemProperties::GetDirtyRegionDebugType();
    surfaceRegionDebugType_ = RSSystemProperties::GetSurfaceRegionDfxType();
    isTargetDirtyRegionDfxEnabled_ = RSSystemProperties::GetTargetDirtyRegionDfxEnabled(dfxTargetSurfaceNames_) &&
        (surfaceRegionDebugType_ == SurfaceRegionDebugType::DISABLED);
    isRegionDebugEnabled_ = (dirtyRegionDebugType_ != DirtyRegionDebugType::DISABLED) ||
        (surfaceRegionDebugType_ != SurfaceRegionDebugType::DISABLED) || (dfxTargetSurfaceNames_.size() > 0);
    isVisibleRegionDfxEnabled_ = (surfaceRegionDebugType_ == SurfaceRegionDebugType::VISIBLE_REGION);
    isOpaqueRegionDfxEnabled_ = (surfaceRegionDebugType_ == SurfaceRegionDebugType::OPAQUE_REGION);
    isAllSurfaceVisibleDebugEnabled_ = RSSystemProperties::GetAllSurfaceVisibleDebugEnabled();
    isDirtyRegionDfxEnabled_ = !isTargetDirtyRegionDfxEnabled_ &&
        (dirtyRegionDebugType_ == DirtyRegionDebugType::EGL_DAMAGE);
    isDisplayDirtyDfxEnabled_ = !isTargetDirtyRegionDfxEnabled_ &&
        (dirtyRegionDebugType_ == DirtyRegionDebugType::DISPLAY_DIRTY);
    isCanvasNodeSkipDfxEnabled_ = (dirtyRegionDebugType_ == DirtyRegionDebugType::CANVAS_NODE_SKIP_RECT);
    isOpDropped_ = isPartialRenderEnabled_ &&
        (partialRenderType_ != PartialRenderType::SET_DAMAGE) && !isRegionDebugEnabled_;
    isCacheBlurPartialRenderEnabled_ = RSSystemProperties::GetCachedBlurPartialRenderEnabled();
    isVirtualDirtyDfxEnabled_ = RSSystemProperties::GetVirtualDirtyDebugEnabled();
    isVirtualDirtyEnabled_ = RSSystemProperties::GetVirtualDirtyEnabled() &&
        (RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL);
    isExpandScreenDirtyEnabled_ = RSSystemProperties::GetExpandScreenDirtyEnabled();
}

RSUniRenderVisitor::RSUniRenderVisitor(const RSUniRenderVisitor& visitor) : RSUniRenderVisitor()
{
    currentVisitDisplay_ = visitor.currentVisitDisplay_;
    screenInfo_ = visitor.screenInfo_;
    displayHasSecSurface_ = visitor.displayHasSecSurface_;
    displayHasSkipSurface_ = visitor.displayHasSkipSurface_;
    displayHasProtectedSurface_ = visitor.displayHasProtectedSurface_;
    displaySpecailSurfaceChanged_ = visitor.displaySpecailSurfaceChanged_;
    hasCaptureWindow_ = visitor.hasCaptureWindow_;
    parentSurfaceNodeMatrix_ = visitor.parentSurfaceNodeMatrix_;
    curAlpha_ = visitor.curAlpha_;
    dirtyFlag_ = visitor.dirtyFlag_;
    curDisplayNode_ = visitor.curDisplayNode_;
    currentFocusedNodeId_ = visitor.currentFocusedNodeId_;
    prepareClipRect_ = visitor.prepareClipRect_;
    isOpDropped_ = visitor.isOpDropped_;
    isPartialRenderEnabled_ = visitor.isPartialRenderEnabled_;
    isAllSurfaceVisibleDebugEnabled_ = visitor.isAllSurfaceVisibleDebugEnabled_;
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
    displayHasProtectedSurface_ = visitor->displayHasProtectedSurface_;
    displaySpecailSurfaceChanged_ = visitor->displaySpecailSurfaceChanged_;
    hasCaptureWindow_ = visitor->hasCaptureWindow_;
    parentSurfaceNodeMatrix_ = visitor->parentSurfaceNodeMatrix_;
    curAlpha_ = visitor->curAlpha_;
    dirtyFlag_ = visitor->dirtyFlag_;
    curDisplayNode_ = visitor->curDisplayNode_;
    currentFocusedNodeId_ = visitor->currentFocusedNodeId_;
    prepareClipRect_ = visitor->prepareClipRect_;
    isOpDropped_ = visitor->isOpDropped_;
    isPartialRenderEnabled_ = visitor->isPartialRenderEnabled_;
    isAllSurfaceVisibleDebugEnabled_ = visitor->isAllSurfaceVisibleDebugEnabled_;
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
        auto isDirty = child->IsDirty();
        if (isDirty) {
            curDirty_ = isDirty;
            child->Prepare(shared_from_this());
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
                return;
            }
        } else if (auto effectNode = child->ReinterpretCastTo<RSEffectRenderNode>()) {
            // effectNode need to update effectRegion so effectNode and use-effect child should be updated
            PrepareEffectNodeIfCacheReuse(cacheRootNode, effectNode);
            return;
        }
        const auto& property =  child->GetRenderProperties();
        if (property.NeedFilter() || property.GetUseEffect()) {
            child->Update(*curSurfaceDirtyManager_, cacheRootNode, dirtyFlag_, prepareClipRect_);
        }
        if (property.NeedFilter()) {
            UpdateForegroundFilterCacheWithDirty(*child, *curSurfaceDirtyManager_);
            if (curSurfaceNode_ && curSurfaceNode_->GetId() == child->GetInstanceRootNodeId()) {
                curSurfaceNode_->UpdateChildrenFilterRects(child, child->GetOldDirtyInSurface(),
                    child->IsFilterCacheValid());
            }
        }
        UpdateSubTreeInCache(child, *child->GetSortedChildren());
    }
}

void RSUniRenderVisitor::PrepareEffectNodeIfCacheReuse(const std::shared_ptr<RSRenderNode>& cacheRootNode,
    std::shared_ptr<RSEffectRenderNode> effectNode)
{
    if (effectNode == nullptr || curSurfaceDirtyManager_ == nullptr) {
        return;
    }
    // set rotationChanged true when screen is rotating or folding/expanding screen.
    UpdateRotationStatusForEffectNode(*effectNode);
    effectNode->SetVisitedFilterCacheStatus(curSurfaceDirtyManager_->IsCacheableFilterRectEmpty());
    effectNode->Update(*curSurfaceDirtyManager_, cacheRootNode, dirtyFlag_, prepareClipRect_);
    UpdateSubTreeInCache(effectNode, *effectNode->GetSortedChildren());
    if (effectNode->GetRenderProperties().NeedFilter()) {
        UpdateForegroundFilterCacheWithDirty(*effectNode, *curSurfaceDirtyManager_);
        if (curSurfaceNode_ && curSurfaceNode_->GetId() == effectNode->GetInstanceRootNodeId()) {
            curSurfaceNode_->UpdateChildrenFilterRects(effectNode, effectNode->GetOldDirtyInSurface(),
                effectNode->IsFilterCacheValid());
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
    const auto& properties = node.GetRenderProperties();

    if (!isSubNodeOfSurfaceInPrepare_) {
        Vector4f::Max(properties.GetCornerRadius(), curCornerRadius_, curCornerRadius_);
    }
    if (node.GetType() == RSRenderNodeType::SURFACE_NODE) {
        node.SetGlobalCornerRadius(curCornerRadius_);
    }

    float alpha = curAlpha_;
    curAlpha_ *= properties.GetAlpha();
    node.SetGlobalAlpha(curAlpha_);
    const auto& children = node.GetSortedChildren();
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
            SaveCurSurface();
            curDirty_ = child->IsDirty();
            child->Prepare(shared_from_this());
            if (child->ChildHasVisibleEffect()) {
                node.SetChildHasVisibleEffect(true);
            }
            RestoreCurSurface();
        }
        // Reset delay flag
        node.ResetGeoUpdateDelay();
        SetNodeCacheChangeStatus(node);
    }

    node.SetSubTreeDirty(false);
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
    if (needMap) {
        if (auto& geoPtr = node.GetRenderProperties().GetBoundsGeometry()) {
            dirtyRect = geoPtr->MapAbsRect(dirtyRect.ConvertTo<float>());
        }
        if (!node.HasChildrenOutOfRect()) {
            dirtyRect = dirtyRect.IntersectRect(node.GetOldClipRect());
        }
    } else {
        dirtyRect = prepareClipRect_.IntersectRect(node.GetChildrenRect());
    }
    dirtyManager->MergeDirtyRect(dirtyRect);
    RS_OPTIONAL_TRACE_NAME_FMT("MergeRemovedChildDirtyRegion NodeId:%" PRIu64 ", dirty rect:%s",
        node.GetId(), dirtyRect.ToString().c_str());
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
    if (curContentDirty_ || node.GetDrawingCacheChanged() || !node.IsCacheCompletedSurfaceValid() ||
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
    const auto& properties = node.GetRenderProperties();
    if (!curCacheFilterRects_.empty() && !node.IsInstanceOf<RSEffectRenderNode>() &&
        (properties.GetBackgroundFilter() || properties.GetUseEffect() ||
        properties.GetShadowColorStrategy() != SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE)) {
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
        auto childrenOutOfRect = node.HasChildrenOutOfRect();
        auto firstVisitedCacheForced = IsFirstVisitedCacheForced();
        // if target cached is reused, keep enable -- prepareskip
        // disable cache if it has outOfParent -- cannot cache right
        // [planning] if there is dirty subcache, disable upper targetcache
        // disable targeted cache node when first visited node is forced cache to avoid problem in case with blur
        if (childrenOutOfRect || (firstVisitedCache_ != node.GetId() && firstVisitedCacheForced)) {
            node.SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
            allCacheFilterRects_[firstVisitedCache_].insert(allCacheFilterRects_[node.GetId()].begin(),
                allCacheFilterRects_[node.GetId()].end());
            allCacheFilterRects_.erase(node.GetId());
            RS_OPTIONAL_TRACE_NAME_FMT("Disable cache %llu: outofparent %d, firstVisitedCache_ %llu & cacheforce %d",
                node.GetId(), childrenOutOfRect, firstVisitedCache_, firstVisitedCacheForced);
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

void RSUniRenderVisitor::SaveCurSurface()
{
    if (!isSubSurfaceEnabled_) {
        return;
    }
    surfaceDirtyManager_.push(curSurfaceDirtyManager_);
    surfaceNode_.push(curSurfaceNode_);
}

void RSUniRenderVisitor::RestoreCurSurface()
{
    if (!isSubSurfaceEnabled_) {
        return;
    }
    curSurfaceDirtyManager_ = surfaceDirtyManager_.top();
    curSurfaceNode_ = surfaceNode_.top();
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
                SaveCurSurface();
                PrepareSurfaceRenderNode(*surfaceNode);
                RestoreCurSurface();
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
                SaveCurSurface();
                ProcessSurfaceRenderNode(*surfaceNode);
                RestoreCurSurface();
            }
        }
    }
}

void RSUniRenderVisitor::SetNodeCacheChangeStatus(RSRenderNode& node)
{
    if (node.ChildHasVisibleFilter()) {
        auto directParent = node.GetParent().lock();
        if (directParent != nullptr) {
            directParent->SetChildHasVisibleFilter(true);
        }
    }
    auto drawingCacheType = node.GetDrawingCacheType();
    if (!isDrawingCacheEnabled_ ||
        drawingCacheType == RSDrawingCacheType::DISABLED_CACHE) {
        return;
    }
    if (!curCacheFilterRects_.empty()) {
        allCacheFilterRects_[node.GetId()].insert(curCacheFilterRects_.top().begin(),
            curCacheFilterRects_.top().end());
        node.ResetFilterRectsInCache(allCacheFilterRects_[node.GetId()]);
        curCacheFilterRects_.pop();
    }
    DisableNodeCacheInSetting(node);
    if (drawingCacheType != RSDrawingCacheType::DISABLED_CACHE) {
        // update visited cache roots including itself
        visitedCacheNodeIds_.emplace(node.GetId());
        node.SetVisitedCacheRootIds(visitedCacheNodeIds_);
        if (curSurfaceNode_) {
            curSurfaceNode_->UpdateDrawingCacheNodes(node.ReinterpretCastTo<RSRenderNode>());
        }
    }
    bool isDrawingCacheChanged = isDrawingCacheChanged_.empty() || isDrawingCacheChanged_.top();
    RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderVisitor::SetNodeCacheChangeStatus: node %" PRIu64 " drawingtype %d, "
        "staticCache %d, cacheChange %d, childHasVisibleFilter|effect: %d|%d, outofparent: %d, "
        "visitedCacheNodeIds num: %lu",
        node.GetId(), static_cast<int>(node.GetDrawingCacheType()), node.IsStaticCached(),
        static_cast<int>(isDrawingCacheChanged), node.ChildHasVisibleFilter(), node.ChildHasVisibleEffect(),
        static_cast<int>(node.HasChildrenOutOfRect()), visitedCacheNodeIds_.size());
    node.SetDrawingCacheChanged(!node.IsStaticCached() && isDrawingCacheChanged);
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
    // currently, P3 is the only supported wide color gamut, this may be modified later.
    if (node.IsAppWindow() && node.GetColorSpace() != GRAPHIC_COLOR_GAMUT_SRGB) {
        newColorSpace_ = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
        RS_LOGD("RSUniRenderVisitor::CheckColorSpace: node(%{public}s) set new colorgamut %{public}d",
            node.GetName().c_str(), newColorSpace_);
    }
}

void RSUniRenderVisitor::CheckColorSpaceWithSelfDrawingNode(RSSurfaceRenderNode& node)
{
    if (!node.IsOnTheTree()) {
        RS_LOGD("RSUniRenderVisitor::CheckColorSpaceWithSelfDrawingNode node(%{public}s) is not on the tree",
            node.GetName().c_str());
        return;
    }
    if (!node.IsHardwareForcedDisabled()) {
        RS_LOGD("RSUniRenderVisitor::CheckColorSpaceWithSelfDrawingNode node(%{public}s) is hardware-enabled",
            node.GetName().c_str());
        return;
    }
    // currently, P3 is the only supported wide color gamut, this may be modified later.
    node.UpdateColorSpaceWithMetadata();
    if (node.GetColorSpace() != GRAPHIC_COLOR_GAMUT_SRGB) {
        newColorSpace_ = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
        RS_LOGD("RSUniRenderVisitor::CheckColorSpaceWithSelfDrawingNode node(%{public}s) set new colorgamut %{public}d",
            node.GetName().c_str(), newColorSpace_);
    }
}

void RSUniRenderVisitor::UpdateColorSpaceAfterHwcCalc(RSDisplayRenderNode& node)
{
    const auto& selfDrawingNodes = RSMainThread::Instance()->GetSelfDrawingNodes();
    for (const auto& selfDrawingNode : selfDrawingNodes) {
        if (newColorSpace_ == GRAPHIC_COLOR_GAMUT_DISPLAY_P3) {
            RS_LOGD("RSUniRenderVisitor::UpdateColorSpaceAfterHwcCalc: newColorSpace is already DISPLAY_P3.");
            return;
        }
        if (!selfDrawingNode || !selfDrawingNode->GetAncestorDisplayNode().lock()) {
            RS_LOGD("RSUniRenderVisitor::UpdateColorSpaceAfterHwcCalc selfDrawingNode or ancestorNode is nullptr");
            continue;
        }
        auto ancestor = selfDrawingNode->GetAncestorDisplayNode().lock()->ReinterpretCastTo<RSDisplayRenderNode>();
        if (ancestor != nullptr && node.GetId() == ancestor->GetId()) {
            CheckColorSpaceWithSelfDrawingNode(*selfDrawingNode);
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

    node.SetColorSpace(newColorSpace_);
    newColorSpace_ = GRAPHIC_COLOR_GAMUT_SRGB;
}

void RSUniRenderVisitor::CheckPixelFormat(RSSurfaceRenderNode& node)
{
    if (node.GetHDRPresent()) {
        RS_LOGD("SetHDRPresent true, surfaceNode: %{public}" PRIu64 "", node.GetId());
        hasUniRenderHdrSurface_ = true;
    }
    if (hasFingerprint_) {
        RS_LOGD("RSUniRenderVisitor::CheckPixelFormat hasFingerprint is true.");
        return;
    }
    if (!node.GetRSSurfaceHandler() || !node.GetRSSurfaceHandler()->GetBuffer()) {
        RS_LOGD("RSUniRenderVisitor::CheckPixelFormat node(%{public}s) did not have buffer.", node.GetName().c_str());
        return;
    }

    const sptr<SurfaceBuffer>& buffer = node.GetRSSurfaceHandler()->GetBuffer();

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
    if (!RSSystemProperties::GetHDRImageEnable()) {
        hasUniRenderHdrSurface_ = false;
    }
    auto stagingDisplayParams = static_cast<RSDisplayRenderParams*>(node.GetStagingRenderParams().get());
    if (!stagingDisplayParams) {
        RS_LOGD("RSUniRenderVisitor::HandlePixelFormat get StagingRenderParams failed.");
        return;
    }
    ScreenId screenId = stagingDisplayParams->GetScreenId();
    RSLuminanceControl::Get().SetHdrStatus(screenId, hasUniRenderHdrSurface_);
    bool isHdrOn = RSLuminanceControl::Get().IsHdrOn(screenId);
    float brightnessRatio = RSLuminanceControl::Get().GetHdrBrightnessRatio(screenId, 0);
    RS_TRACE_NAME_FMT("HDR:%d, in Unirender:%d brightnessRatio:%f", isHdrOn, hasUniRenderHdrSurface_, brightnessRatio);
    RS_LOGD("RSUniRenderVisitor::HandlePixelFormat HDR isHdrOn:%{public}d hasUniRenderHdrSurface:%{public}d "
        "brightnessRatio:%{public}f", isHdrOn, hasUniRenderHdrSurface_, brightnessRatio);
    if (!hasUniRenderHdrSurface_) {
        isHdrOn = false;
    }
    node.SetHDRPresent(isHdrOn);
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
    stagingDisplayParams->SetNewPixelFormat(newPixelFormat_);
}

void RSUniRenderVisitor::PrepareDisplayRenderNode(RSDisplayRenderNode& node)
{
    currentVisitDisplay_ = node.GetScreenId();
    displayHasSecSurface_.emplace(currentVisitDisplay_, false);
    displayHasSkipSurface_.emplace(currentVisitDisplay_, false);
    displayHasProtectedSurface_.emplace(currentVisitDisplay_, false);
    displaySpecailSurfaceChanged_.emplace(currentVisitDisplay_, false);
    hasCaptureWindow_.emplace(currentVisitDisplay_, false);
    node.GetDirtySurfaceNodeMap().clear();

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
    SendRcdMessage(node);
    parentSurfaceNodeMatrix_ = Drawing::Matrix();
    auto& geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
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
    if (node.IsRotationChanged()) {
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

    node.GetCurAllSurfaces(false).clear();
    node.GetCurAllSurfaces(true).clear();
    node.CollectSurface(node.shared_from_this(), node.GetCurAllSurfaces(false), true, false);
    node.CollectSurface(node.shared_from_this(), node.GetCurAllSurfaces(true), true, true);

    HandleColorGamuts(node, screenManager);
    HandlePixelFormat(node, screenManager);
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
    node.UpdateSurfaceCacheContentStatic();
    RS_OPTIONAL_TRACE_NAME("Skip static surface " + node.GetName() + " nodeid - pid: " +
        std::to_string(node.GetId()) + " - " + std::to_string(ExtractPid(node.GetId())));
    // static node's dirty region is empty
    auto& dirtyManager = node.GetDirtyManager();
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
    curDisplayNode_->AddSurfaceNodePosByDescZOrder(
        node.GetId(), curDisplayNode_->GetLastFrameSurfacePos(node.GetId()));
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
    if (node.IsLeashOrMainWindow()) {
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
    if (node.IsLeashOrMainWindow() && curSurfaceDirtyManager_->IsCurrentFrameDirty()) {
        accumulatedDirtyRegions_.emplace_back(curSurfaceDirtyManager_->GetCurrentFrameDirtyRegion());
    }
}

void RSUniRenderVisitor::UpdateSecuritySkipAndProtectedLayersRecord(RSSurfaceRenderNode& node)
{
    if (node.GetHasSecurityLayer()) {
        displayHasSecSurface_[currentVisitDisplay_] = true;
        curDisplayNode_->AddSecurityLayer(node.IsLeashWindow() ? node.GetLeashPersistentId() : node.GetId());
    }
    if (node.GetHasSkipLayer() && node.GetName().find(CAPTURE_WINDOW_NAME) == std::string::npos) {
        displayHasSkipSurface_[currentVisitDisplay_] = true;
    }
    if (node.GetHasProtectedLayer()) {
        displayHasProtectedSurface_[currentVisitDisplay_] = true;
    }
    if (node.IsSpecialLayerChanged()) {
        displaySpecailSurfaceChanged_[currentVisitDisplay_] = true;
    }
}

void RSUniRenderVisitor::UpdateForegroundFilterCacheWithDirty(RSRenderNode& node,
    RSDirtyRegionManager& dirtyManager, bool isForeground)
{
    node.UpdateFilterCacheWithBelowDirty(dirtyManager, isForeground);
    node.UpdateFilterCacheWithSelfDirty();
}

void RSUniRenderVisitor::UpdateSurfaceRenderNodeRotate(RSSurfaceRenderNode& node)
{
    if (!node.IsMainWindowType()) {
        return;
    }
    auto& geoPtr = node.GetRenderProperties().GetBoundsGeometry();
    if (!geoPtr) {
        return;
    }
    if (RSUniRenderUtil::GetRotationDegreeFromMatrix(geoPtr->GetAbsMatrix()) % RS_ROTATION_90 != 0) {
        node.SetIsRotating(true);
    }
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
            RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderVisitor::IsSubTreeOccluded node[%s]"
                "name:[%s] visibleRegionIsEmpty[%d]",
                std::to_string(node.GetId()).c_str(), surfaceNode.GetName().c_str(),
                surfaceNode.GetVisibleRegion().IsEmpty());
            auto isOccluded = hasMirrorDisplay_ ?
                surfaceNode.GetVisibleRegionInVirtual().IsEmpty() : surfaceNode.GetVisibleRegion().IsEmpty();
            isOccluded = isOccluded && (!RSUifirstManager::Instance().IsSubTreeNeedPrepareForSnapshot(surfaceNode));
            if (isOccluded && curSurfaceDirtyManager_) {
                curSurfaceDirtyManager_->Clear();
            }
            surfaceNode.AccmulateDirtyInOcclusion(isOccluded);
            return isOccluded;
        }
    }
    // step2.1 For partial visible surface, intersected region->rects in surface
    // step2.2 check if clean subtree in occlusion rects
    return false;
}

void RSUniRenderVisitor::ResetDisplayDirtyRegion()
{
    if (!curDisplayDirtyManager_) {
        return;
    }
    if (curDisplayNode_ == nullptr) {
        return;
    }
    bool isNeedNotchUpdate = RSSingleton<RoundCornerDisplayManager>::GetInstance().IsNotchNeedUpdate(
        curDisplayNode_->GetId(), RSSystemParameters::GetHideNotchStatus());
    bool ret = CheckScreenPowerChange() ||
        CheckColorFilterChange() ||
        CheckCurtainScreenUsingStatusChange() ||
        IsFirstFrameOfPartialRender() ||
        IsWatermarkFlagChanged() ||
        zoomStateChange_ ||
        isCompleteRenderEnabled_ ||
        CheckLuminanceStatusChange() ||
        IsFirstFrameOfOverdrawSwitch() ||
        isNeedNotchUpdate;
    if (ret) {
        curDisplayDirtyManager_->ResetDirtyAsSurfaceSize();
        RS_LOGD("RSUniRenderVisitor::ResetDisplayDirtyRegion on");
    }
}

bool RSUniRenderVisitor::CheckScreenPowerChange() const
{
    if (!RSMainThread::Instance()->GetScreenPowerOnChanged()) {
        return false;
    }
    RS_LOGD("RSUniRenderVisitor::CheckScreenPowerChange changed");
    return true;
}

bool RSUniRenderVisitor::CheckCurtainScreenUsingStatusChange() const
{
    if (!RSMainThread::Instance()->IsCurtainScreenUsingStatusChanged()) {
        return false;
    }
    RS_LOGD("RSUniRenderVisitor::CheckCurtainScreenUsingStatusChange changed");
    return true;
}

bool RSUniRenderVisitor::CheckLuminanceStatusChange()
{
    if (!RSMainThread::Instance()->ExchangeLuminanceChangingStatus()) {
        return false;
    }
    RS_LOGD("RSUniRenderVisitor::CheckLuminanceStatusChange changed");
    return true;
}

bool RSUniRenderVisitor::IsFirstFrameOfPartialRender() const
{
    if (!RSMainThread::Instance()->IsFirstFrameOfPartialRender()) {
        return false;
    }
    RS_LOGD("FirstFrameOfPartialRender");
    return true;
}

bool RSUniRenderVisitor::IsFirstFrameOfOverdrawSwitch() const
{
    if (!RSMainThread::Instance()->IsFirstFrameOfOverdrawSwitch()) {
        return false;
    }
    RS_LOGD("IsFirstFrameOfOverdrawSwitch");
    return true;
}

bool RSUniRenderVisitor::IsWatermarkFlagChanged() const
{
    if (RSMainThread::Instance()->IsWatermarkFlagChanged()) {
        RS_LOGD("FirstOrLastFrameOfWatermark");
        return true;
    } else {
        return false;
    }
}

void RSUniRenderVisitor::UpdateDisplayZoomState()
{
    if (!curDisplayNode_) {
        return;
    }
    auto scale = curDisplayNode_->GetRenderProperties().GetScale();
    bool curZoomState = scale.x_ > 1.f || scale.y_ > 1.f;
    curDisplayNode_->UpdateZoomState(curZoomState);
    zoomStateChange_ = curZoomState || curDisplayNode_->IsZoomStateChange();
}

void RSUniRenderVisitor::UpdateVirtualScreenSecurityExemption(RSDisplayRenderNode& node)
{
    // only for virtual screen
    if (!(node.IsMirrorDisplay())) {
        return;
    }
    auto mirrorNode = node.GetMirrorSource().lock();
    if (mirrorNode == nullptr || screenManager_ == nullptr) {
        return;
    }
    auto securityExemptionList = screenManager_->GetVirtualScreenSecurityExemptionList(node.GetScreenId());
    if (securityExemptionList.size() == 0) {
        RS_LOGD("UpdateVirtualScreenSecurityExemption::node:%{public}" PRIu64 ", isSecurityExemption:false",
            node.GetId());
        node.SetSecurityExemption(false);
        mirrorNode->ClearSecurityLayerList();
        return;
    }
    auto securityLayerList = mirrorNode->GetSecurityLayerList();
    for (const auto& exemptionLayer : securityExemptionList) {
        RS_LOGD("UpdateVirtualScreenSecurityExemption::node:%{public}" PRIu64 ""
            "securityExemption nodeId %{public}" PRIu64 ".", node.GetId(), exemptionLayer);
    }
    for (const auto& secLayer : securityLayerList) {
        RS_LOGD("UpdateVirtualScreenSecurityExemption::node:%{public}" PRIu64 ""
            "securityLayer nodeId %{public}" PRIu64 ".", mirrorNode->GetId(), secLayer);
    }
    bool isSecurityExemption = false;
    if (securityExemptionList.size() >= securityLayerList.size()) {
        isSecurityExemption = true;
        for (const auto& secLayer : securityLayerList) {
            if (std::find(securityExemptionList.begin(), securityExemptionList.end(), secLayer) ==
                securityExemptionList.end()) {
                isSecurityExemption = false;
                break;
            }
        }
    }
    RS_LOGD("UpdateVirtualScreenSecurityExemption::node:%{public}" PRIu64 ", isSecurityExemption:%{public}d",
        node.GetId(), isSecurityExemption);
    node.SetSecurityExemption(isSecurityExemption);
    mirrorNode->ClearSecurityLayerList();
}

void RSUniRenderVisitor::QuickPrepareDisplayRenderNode(RSDisplayRenderNode& node)
{
    // 0. init display info
    RS_TRACE_NAME("RSUniRender:QuickPrepareDisplayRenderNode " + std::to_string(node.GetScreenId()));
    if (!InitDisplayInfo(node)) {
        return;
    }
    UpdateDisplayZoomState();
    SendRcdMessage(node);
    UpdateVirtualScreenSecurityExemption(node);
    ancestorNodeHasAnimation_ = false;
    displayNodeRotationChanged_ = node.IsRotationChanged();
    dirtyFlag_ = isDirty_ || displayNodeRotationChanged_ || zoomStateChange_ ||
        curDisplayDirtyManager_->IsActiveSurfaceRectChanged();
    prepareClipRect_ = screenRect_;
    hasAccumulatedClip_ = false;

    curAlpha_ = 1.0f;
    globalZOrder_ = 0.0f;
    hasSkipLayer_ = false;
    node.UpdateRotation();
    if (!(RSMainThread::Instance()->IsRequestedNextVSync() || RSMainThread::Instance()->GetNextDVsyncAnimateFlag())) {
        RS_OPTIONAL_TRACE_NAME_FMT("do not request next vsync");
        needRequestNextVsync_ = false;
    }
    RSUifirstManager::Instance().SetRotationChanged(displayNodeRotationChanged_ || isScreenRotationAnimating_);
    if (node.IsSubTreeDirty() || node.IsRotationChanged()) {
        QuickPrepareChildren(node);
        TryNotifyUIBufferAvailable();
    }
    PostPrepare(node);
    UpdateHwcNodeEnable();
    UpdateSurfaceDirtyAndGlobalDirty();
    UpdateSurfaceOcclusionInfo();
    if (needRecalculateOcclusion_) {
        // Callback for registered self drawing surfacenode
        RSMainThread::Instance()->SurfaceOcclusionCallback();
    }
    //UIFirst layer must be above displayNode, so use zorder + 1
    RSUifirstManager::Instance().UpdateUIFirstLayerInfo(screenInfo_, globalZOrder_ + 1);
    curDisplayNode_->UpdatePartialRenderParams();
    RSDisplayRenderNode::ScreenRenderParams screenRenderParams;
    screenRenderParams.screenInfo = std::move(screenInfo_);
    screenRenderParams.displayHasSecSurface = std::move(displayHasSecSurface_);
    screenRenderParams.displayHasSkipSurface = std::move(displayHasSkipSurface_);
    screenRenderParams.displayHasProtectedSurface = std::move(displayHasProtectedSurface_);
    screenRenderParams.displaySpecailSurfaceChanged = std::move(displaySpecailSurfaceChanged_);
    screenRenderParams.hasCaptureWindow = std::move(hasCaptureWindow_);
    curDisplayNode_->UpdateScreenRenderParams(screenRenderParams);
    curDisplayNode_->UpdateOffscreenRenderParams(curDisplayNode_->IsRotationChanged());
    UpdateColorSpaceAfterHwcCalc(node);
    HandleColorGamuts(node, screenManager_);
    HandlePixelFormat(node, screenManager_);
    if (UNLIKELY(!SharedTransitionParam::unpairedShareTransitions_.empty())) {
        ProcessUnpairedSharedTransitionNode();
    }
    node.RenderTraceDebug();
}

void RSUniRenderVisitor::CheckFilterCacheNeedForceClearOrSave(RSRenderNode& node)
{
    if (!node.HasBlurFilter()) {
        return;
    }
    bool rotationChanged = curDisplayNode_ ?
        curDisplayNode_->IsRotationChanged() || curDisplayNode_->IsLastRotationChanged() : false;
    bool rotationStatusChanged = curDisplayNode_ ?
        curDisplayNode_->GetPreRotationStatus() != curDisplayNode_->GetCurRotationStatus() : false;
    node.CheckBlurFilterCacheNeedForceClearOrSave(rotationChanged, rotationStatusChanged);
}

// private method, curDisplayNode_ or curSurfaceNode_ will not be nullptr
void RSUniRenderVisitor::CheckMergeFilterDirtyByIntersectWithDirty(OcclusionRectISet& filterSet, bool isGlobalDirty)
{
    // Recursively traverses until the globalDirty do not change
    auto dirtyManager = isGlobalDirty ? curDisplayNode_->GetDirtyManager() : curSurfaceNode_->GetDirtyManager();
    if (dirtyManager == nullptr) {
        return;
    }
    for (auto it = filterSet.begin(); it != filterSet.end();) {
        auto dirtyRect = dirtyManager->GetCurrentFrameDirtyRegion();
        auto filterRegion = Occlusion::Region{ Occlusion::Rect{ it->second } };
        auto dirtyRegion = Occlusion::Region{ Occlusion::Rect{ dirtyRect } };
        auto filterDirtyRegion = filterRegion.And(dirtyRegion);
        if (!filterDirtyRegion.IsEmpty()) {
            if (isGlobalDirty) {
                RS_LOGD("RSUniRenderVisitor::CheckMergeGlobalFilterForDisplay global merge, "
                    "global dirty %{public}s, add filterRegion %{public}s",
                    dirtyRect.ToString().c_str(), (it->second).ToString().c_str());
            }
            dirtyManager->MergeDirtyRect(it->second);
            it = filterSet.erase(it);
            if (dirtyRect != dirtyManager->GetCurrentFrameDirtyRegion()) {
                // When dirtyRegion is changed, collect dirty filter region from begin.
                // After all filter region is added, the cycle will definitely stop.
                it = filterSet.begin();
            }
        } else {
            ++it;
        }
    }
    filterSet.clear();
}

void RSUniRenderVisitor::QuickPrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSUniRender::QuickPrepare:[%s] nodeId[%" PRIu64 "]"
        "pid[%d] nodeType[%u] subTreeDirty[%d]", node.GetName().c_str(), node.GetId(), ExtractPid(node.GetId()),
        static_cast<uint>(node.GetSurfaceNodeType()), node.IsSubTreeDirty());
    RS_LOGD("RSUniRender::QuickPrepareSurfaceRenderNode:[%{public}s] nodeid:[%{public}" PRIu64 "]"
        "pid:[%{public}d] nodeType:[%{public}d] subTreeDirty[%{public}d]", node.GetName().c_str(), node.GetId(),
        ExtractPid(node.GetId()), static_cast<int>(node.GetSurfaceNodeType()), node.IsSubTreeDirty());

    // 0. init curSurface info and check node info
    auto curCornerRadius = curCornerRadius_;
    auto parentSurfaceNodeMatrix = parentSurfaceNodeMatrix_;
    if (!BeforeUpdateSurfaceDirtyCalc(node)) {
        RS_LOGE("RSUniRenderVisitor::QuickPrepareSurfaceRenderNode BeforeUpdateSurfaceDirtyCalc fail");
        RSUifirstManager::Instance().DisableUifirstNode(node);
        node.OpincSetInAppStateEnd(unchangeMarkInApp_);
        return;
    }

    if (curSurfaceDirtyManager_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::QuickPrepareSurfaceRenderNode %{public}s curSurfaceDirtyManager is nullptr",
            node.GetName().c_str());
        return;
    }

    // 1. Update matrix and collect dirty region
    auto dirtyFlag = dirtyFlag_;
    auto prepareClipRect = prepareClipRect_;
    bool hasAccumulatedClip = hasAccumulatedClip_;
    auto prevAlpha = curAlpha_;
    curAlpha_ *= std::clamp(node.GetRenderProperties().GetAlpha(), 0.f, 1.f);
    node.SetGlobalAlpha(curAlpha_);
    CheckFilterCacheNeedForceClearOrSave(node);
    node.CheckContainerDirtyStatusAndUpdateDirty(curContainerDirty_);
    dirtyFlag_ = node.UpdateDrawRectAndDirtyRegion(
        *curSurfaceDirtyManager_, dirtyFlag_, prepareClipRect_, parentSurfaceNodeMatrix);
    auto& geoPtr = node.GetRenderProperties().GetBoundsGeometry();
    if (!geoPtr) {
        return;
    }
    parentSurfaceNodeMatrix_ = geoPtr->GetAbsMatrix();
    if (!AfterUpdateSurfaceDirtyCalc(node)) {
        RS_LOGE("RSUniRenderVisitor::QuickPrepareSurfaceRenderNode AfterUpdateSurfaceDirtyCalc fail");
        RSUifirstManager::Instance().DisableUifirstNode(node);
        node.OpincSetInAppStateEnd(unchangeMarkInApp_);
        return;
    }
    hasAccumulatedClip_ = node.SetAccumulatedClipFlag(hasAccumulatedClip_);
    bool isSubTreeNeedPrepare = node.IsSubTreeNeedPrepare(filterInGlobal_, IsSubTreeOccluded(node)) ||
        ForcePrepareSubTree();
    isSubTreeNeedPrepare ? QuickPrepareChildren(node) :
        node.SubTreeSkipPrepare(*curSurfaceDirtyManager_, curDirty_, dirtyFlag_, prepareClipRect_);
    curSurfaceDirtyManager_->ClipDirtyRectWithinSurface();

    // Update whether leash window's visible region is empty after prepare children
    UpdateLeashWindowVisibleRegionEmpty(node);

    if (node.IsLeashWindow() && RSSystemProperties::GetGpuOverDrawBufferOptimizeEnabled()) {
        CheckIsGpuOverDrawBufferOptimizeNode(node);
    }
    PostPrepare(node, !isSubTreeNeedPrepare);
    CheckMergeFilterDirtyByIntersectWithDirty(curSurfaceNoBelowDirtyFilter_, false);
    curAlpha_ = prevAlpha;
    prepareClipRect_ = prepareClipRect;
    hasAccumulatedClip_ = hasAccumulatedClip;
    dirtyFlag_ = dirtyFlag;

    PrepareForUIFirstNode(node);
    node.OpincSetInAppStateEnd(unchangeMarkInApp_);
    ResetCurSurfaceInfoAsUpperSurfaceParent(node);
    curCornerRadius_ = curCornerRadius;
    parentSurfaceNodeMatrix_ = parentSurfaceNodeMatrix;
    node.RenderTraceDebug();
    node.SetNeedOffscreen(isScreenRotationAnimating_);
    if (node.IsUIBufferAvailable()) {
        uiBufferAvailableId_.emplace_back(node.GetId());
    }
}

void RSUniRenderVisitor::PrepareForUIFirstNode(RSSurfaceRenderNode& node)
{
    MultiThreadCacheType lastFlag = node.GetLastFrameUifirstFlag();
    RSUifirstManager::Instance().UpdateUifirstNodes(node, ancestorNodeHasAnimation_ || node.GetCurFrameHasAnimation());
    RSUifirstManager::Instance().UpdateUIFirstNodeUseDma(node, globalSurfaceBounds_);
    if (node.GetLastFrameUifirstFlag() == MultiThreadCacheType::LEASH_WINDOW &&
        !node.IsHardwareForcedDisabled()) {
        if (auto& geo = node.GetRenderProperties().GetBoundsGeometry()) {
            UpdateSrcRect(node, geo->GetAbsMatrix(), geo->GetAbsRect());
        }
        UpdateHwcNodeByTransform(node);
    }
    if (RSUifirstManager::Instance().GetUseDmaBuffer(node.GetName()) && curSurfaceDirtyManager_ &&
        (node.GetLastFrameUifirstFlag() != lastFlag ||
        !node.IsHardwareForcedDisabled() != node.GetIsLastFrameHwcEnabled())) {
        curSurfaceDirtyManager_->MergeDirtyRect(node.GetOldDirty());
    }
}

void RSUniRenderVisitor::UpdateNodeVisibleRegion(RSSurfaceRenderNode& node)
{
    Occlusion::Rect selfDrawRect = node.GetSurfaceOcclusionRect(true);
    Occlusion::Region selfDrawRegion { selfDrawRect };
    if (needRecalculateOcclusion_) {
        Occlusion::Region subResult = selfDrawRegion.Sub(accumulatedOcclusionRegion_);
        node.SetVisibleRegion(subResult);
        Occlusion::Region subResultWithoutSkipLayer = selfDrawRegion.Sub(occlusionRegionWithoutSkipLayer_);
        node.SetVisibleRegionInVirtual(subResultWithoutSkipLayer);
    }
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_THREE,
        "RSUniRenderVisitor::UpdateNodeVisibleRegion name[%s] visibleRegion[%s]",
        node.GetName().c_str(), node.GetVisibleRegion().GetRegionInfo().c_str());
}

void RSUniRenderVisitor::CalculateOcclusion(RSSurfaceRenderNode& node)
{
    if (!curDisplayNode_) {
        RS_LOGE("RSUniRenderVisitor::CalculateOcclusion curDisplayNode is nullptr");
        return;
    }
    // CheckAndUpdateOpaqueRegion only in mainWindow
    auto parent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.GetParent().lock());
    auto isFocused = node.IsFocusedNode(currentFocusedNodeId_) ||
        (parent && parent->IsLeashWindow() && parent->IsFocusedNode(focusedLeashWindowId_));
    node.CheckAndUpdateOpaqueRegion(screenRect_, curDisplayNode_->GetRotation(), isFocused);
    if (!needRecalculateOcclusion_) {
        needRecalculateOcclusion_ = node.CheckIfOcclusionChanged();
    }
    // Update node visibleRegion
    hasSkipLayer_ = hasSkipLayer_ || node.GetSkipLayer();
    UpdateNodeVisibleRegion(node);
    auto mainThread = RSMainThread::Instance();
    node.SetOcclusionInSpecificScenes(mainThread->GetDeviceType() == DeviceType::PC &&
        mainThread->IsPCThreeFingerScenesListScene());
    // check current surface Participate In Occlusion
    if (node.CheckParticipateInOcclusion() && !isAllSurfaceVisibleDebugEnabled_) {
        accumulatedOcclusionRegion_.OrSelf(node.GetOpaqueRegion());
        std::unordered_set<NodeId> currentBlackList = GetCurrentBlackList();
        if (IsValidInVirtualScreen(node) && currentBlackList.find(node.GetId()) == currentBlackList.end()) {
            occlusionRegionWithoutSkipLayer_.OrSelf(node.GetOpaqueRegion());
        }
    }
    node.SetOcclusionInSpecificScenes(false);
    CollectOcclusionInfoForWMS(node);
}

void RSUniRenderVisitor::CollectOcclusionInfoForWMS(RSSurfaceRenderNode& node)
{
    if (!node.IsMainWindowType()) {
        return;
    }
    // collect mainWindow occlusion visibleLevel
    Occlusion::Region selfDrawRegion { node.GetSurfaceOcclusionRect(true) };
    auto visibleLevel = GetRegionVisibleLevel(node.GetVisibleRegion(), selfDrawRegion);
    // wms default all visible about sefdrawing node and AbilityComponent node
    auto instanceNode = node.GetInstanceRootNode() ?
        node.GetInstanceRootNode()->ReinterpretCastTo<RSSurfaceRenderNode>() : nullptr;
    if (instanceNode == nullptr) {
        return;
    }
    if ((node.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE &&
        !instanceNode->GetVisibleRegion().IsEmpty()) || node.IsAbilityComponent()) {
        dstCurVisVec_.emplace_back(std::make_pair(node.GetId(),
            WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE));
        return;
    }
    dstCurVisVec_.emplace_back(std::make_pair(node.GetId(),
        node.GetVisibleLevelForWMS(visibleLevel)));
}

void RSUniRenderVisitor::SurfaceOcclusionCallbackToWMS()
{
    if (RSSystemParameters::GetOcclusionCallBackToWMSDebugType()) {
        allDstCurVisVec_.clear();
        const auto& curAllSurfaces = curDisplayNode_->GetAllMainAndLeashSurfaces();
        for (const auto& surfacePtr : curAllSurfaces) {
            if (surfacePtr == nullptr) {
                continue;
            }
            const auto& surfaceNode = static_cast<RSSurfaceRenderNode&>(*surfacePtr);
            if (surfaceNode.IsMainWindowType()) {
                allDstCurVisVec_.emplace_back(std::make_pair(surfacePtr->GetId(),
                    WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE));
            }
        }
    }
    if (allDstCurVisVec_ != allLastVisVec_) {
        RSMainThread::Instance()->SurfaceOcclusionChangeCallback(allDstCurVisVec_);
        RS_LOGI("RSUniRenderVisitor::SurfaceOcclusionCallbackToWMS %{public}s",
            VisibleDataToString(allDstCurVisVec_).c_str());
        allLastVisVec_ = std::move(allDstCurVisVec_);
    }
}

std::unordered_set<NodeId> RSUniRenderVisitor::GetCurrentBlackList() const
{
    if (!screenManager_ || !curDisplayNode_) {
        return std::unordered_set<NodeId>();
    }

    std::unordered_set<NodeId> currentBlackList;
    if (screenManager_->GetCastScreenEnableSkipWindow(curDisplayNode_->GetScreenId())) {
        screenManager_->GetCastScreenBlackList(currentBlackList);
    } else {
        currentBlackList = screenManager_->GetVirtualScreenBlackList(curDisplayNode_->GetScreenId());
    }
    return currentBlackList;
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

void RSUniRenderVisitor::QuickPrepareEffectRenderNode(RSEffectRenderNode& node)
{
    // 0. check current node need to tranverse
    auto dirtyManager = curSurfaceNode_ ? curSurfaceDirtyManager_ : curDisplayDirtyManager_;
    if (!dirtyManager) {
        RS_LOGE("RSUniRenderVisitor::QuickPrepareEffectRenderNode dirtyManager is nullptr");
        return;
    }
    auto dirtyFlag = dirtyFlag_;
    auto prevAlpha = curAlpha_;
    auto curCornerRadius = curCornerRadius_;
    curAlpha_ *= std::clamp(node.GetRenderProperties().GetAlpha(), 0.f, 1.f);
    UpdateRotationStatusForEffectNode(node);
    CheckFilterCacheNeedForceClearOrSave(node);
    RectI prepareClipRect = prepareClipRect_;
    bool hasAccumulatedClip = hasAccumulatedClip_;
    dirtyFlag_ =
        node.UpdateDrawRectAndDirtyRegion(*dirtyManager, dirtyFlag_, prepareClipRect_, parentSurfaceNodeMatrix_);
    node.UpdateCurCornerRadius(curCornerRadius_);
    // 1. Recursively traverse child nodes
    hasAccumulatedClip_ = node.SetAccumulatedClipFlag(hasAccumulatedClip_);
    bool isSubTreeNeedPrepare = node.IsSubTreeNeedPrepare(filterInGlobal_) || ForcePrepareSubTree();
    isSubTreeNeedPrepare ? QuickPrepareChildren(node) :
        node.SubTreeSkipPrepare(*dirtyManager, curDirty_, dirtyFlag_, prepareClipRect_);

    PostPrepare(node, !isSubTreeNeedPrepare);
    prepareClipRect_ = prepareClipRect;
    hasAccumulatedClip_ = hasAccumulatedClip;
    dirtyFlag_ = dirtyFlag;
    curAlpha_ = prevAlpha;
    curCornerRadius_ = curCornerRadius;
    node.RenderTraceDebug();
}

void RSUniRenderVisitor::QuickPrepareCanvasRenderNode(RSCanvasRenderNode& node)
{
    // 0. check current node need to traverse
    auto dirtyManager = curSurfaceNode_ ? curSurfaceDirtyManager_ : curDisplayDirtyManager_;
    auto dirtyFlag = dirtyFlag_;
    auto prevAlpha = curAlpha_;
    auto curCornerRadius = curCornerRadius_;
    curAlpha_ *= std::clamp(node.GetRenderProperties().GetAlpha(), 0.f, 1.f);
    CheckFilterCacheNeedForceClearOrSave(node);
    if (isDrawingCacheEnabled_) {
        node.UpdateDrawingCacheInfoBeforeChildren(isScreenRotationAnimating_);
    }
    node.OpincQuickMarkStableNode(unchangeMarkInApp_, unchangeMarkEnable_,
        RSMainThread::Instance()->IsAccessibilityConfigChanged());

    RectI prepareClipRect = prepareClipRect_;
    bool hasAccumulatedClip = hasAccumulatedClip_;

    if (!dirtyManager) {
        return;
    }
    dirtyFlag_ =
        node.UpdateDrawRectAndDirtyRegion(*dirtyManager, dirtyFlag_, prepareClipRect_, parentSurfaceNodeMatrix_);
    // update prepare clip before children
    UpdatePrepareClip(node);
    node.UpdateCurCornerRadius(curCornerRadius_);

    // 1. Recursively traverse child nodes if above curSurfaceNode and subnode need draw
    hasAccumulatedClip_ = node.SetAccumulatedClipFlag(hasAccumulatedClip_);
    bool isSubTreeNeedPrepare = !curSurfaceNode_ || node.IsSubTreeNeedPrepare(filterInGlobal_) ||
        ForcePrepareSubTree() || node.OpincForcePrepareSubTree();
    isSubTreeNeedPrepare ? QuickPrepareChildren(node) :
        node.SubTreeSkipPrepare(*dirtyManager, curDirty_, dirtyFlag_, prepareClipRect_);

    PostPrepare(node, !isSubTreeNeedPrepare);
    prepareClipRect_ = prepareClipRect;
    hasAccumulatedClip_ = hasAccumulatedClip;
    dirtyFlag_ = dirtyFlag;
    curAlpha_ = prevAlpha;
    curCornerRadius_ = curCornerRadius;
    node.OpincUpdateRootFlag(unchangeMarkEnable_);
    node.RenderTraceDebug();
}

void RSUniRenderVisitor::UpdateRotationStatusForEffectNode(RSEffectRenderNode& node)
{
     // folding/expanding screen force invalidate cache.
    node.SetFoldStatusChanged(doAnimate_ &&
        curDisplayNode_->GetScreenId() != node.GetCurrentAttachedScreenId());
    node.SetCurrentAttachedScreenId(curDisplayNode_->GetScreenId());
    node.SetRotationChanged(curDisplayNode_->IsRotationChanged());
}

void RSUniRenderVisitor::UpdatePrepareClip(RSRenderNode& node)
{
    const auto& property = node.GetRenderProperties();
    auto& geoPtr = property.GetBoundsGeometry();
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
    if (property.GetClipToRRect()) {
        RectF rect = property.GetClipRRect().rect_;
        prepareClipRect_ = prepareClipRect_.IntersectRect(geoPtr->MapAbsRect(rect));
    }
}

bool RSUniRenderVisitor::IsLeashAndHasMainSubNode(RSRenderNode& node) const
{
    if (node.GetType() != RSRenderNodeType::SURFACE_NODE) {
        return false;
    }
    const auto& surfaceNode = static_cast<RSSurfaceRenderNode&>(node);
    if (!surfaceNode.IsLeashWindow()) {
        return false;
    }
    // check leashWindow surface has first level mainwindow node
    auto children = node.GetSortedChildren();
    auto iter = std::find_if((*children).begin(), (*children).end(),
        [](const std::shared_ptr<RSRenderNode>& node) {
        if (node && node->GetType() == RSRenderNodeType::SURFACE_NODE) {
            const auto& surfaceNode = static_cast<RSSurfaceRenderNode&>(*node);
            return surfaceNode.IsMainWindowType();
        }
        return false;
    });
    return iter != (*children).end();
}

bool RSUniRenderVisitor::NeedPrepareChindrenInReverseOrder(RSRenderNode& node) const
{
    if (!curSurfaceNode_ && node.GetType() != RSRenderNodeType::RS_NODE) {
        return true;
    }
    return IsLeashAndHasMainSubNode(node);
}

void RSUniRenderVisitor::QuickPrepareChildren(RSRenderNode& node)
{
    MergeRemovedChildDirtyRegion(node, true);
    if (node.LastFrameSubTreeSkipped() && curSurfaceDirtyManager_) {
        node.ForceMergeSubTreeDirtyRegion(*curSurfaceDirtyManager_, prepareClipRect_);
    }
    bool animationBackup = ancestorNodeHasAnimation_;
    ancestorNodeHasAnimation_ = ancestorNodeHasAnimation_ || node.GetCurFrameHasAnimation();
    node.ResetChildRelevantFlags();
    node.ResetChildUifirstSupportFlag();
    auto children = node.GetSortedChildren();
    if (NeedPrepareChindrenInReverseOrder(node)) {
        std::for_each((*children).rbegin(), (*children).rend(), [this](const std::shared_ptr<RSRenderNode>& node) {
            if (!node) {
                return;
            }
            auto containerDirty = curContainerDirty_;
            curDirty_ = node->IsDirty();
            curContainerDirty_ = curContainerDirty_ || node->IsDirty();
            node->QuickPrepare(shared_from_this());
            curContainerDirty_ = containerDirty;
        });
    } else {
        std::for_each((*children).begin(), (*children).end(), [this](const std::shared_ptr<RSRenderNode>& node) {
            if (!node) {
                return;
            }
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
    displayHasProtectedSurface_.emplace(currentVisitDisplay_, false);
    displaySpecailSurfaceChanged_.emplace(currentVisitDisplay_, false);
    hasCaptureWindow_.emplace(currentVisitDisplay_, false);
    curDisplayDirtyManager_ = node.GetDirtyManager();
    curDisplayNode_ = node.shared_from_this()->ReinterpretCastTo<RSDisplayRenderNode>();
    if (!curDisplayDirtyManager_ || !curDisplayNode_) {
        RS_LOGE("RSUniRenderVisitor::InitDisplayInfo dirtyMgr or node ptr is nullptr");
        return false;
    }
    curDisplayDirtyManager_->Clear();
    transparentCleanFilter_.clear();
    transparentDirtyFilter_.clear();
    transparentHwcCleanFilter_.clear();
    transparentHwcDirtyFilter_.clear();
    // 2 init screenManager info
    screenManager_ = CreateOrGetScreenManager();
    if (!screenManager_) {
        RS_LOGE("RSUniRenderVisitor::InitDisplayInfo screenManager_ is nullptr");
        return false;
    }
    screenInfo_ = screenManager_->QueryScreenInfo(node.GetScreenId());
    screenRect_ = screenInfo_.activeRect.IsEmpty() ? RectI{0, 0, screenInfo_.width, screenInfo_.height} :
        screenInfo_.activeRect;
    curDisplayDirtyManager_->SetSurfaceSize(screenInfo_.width, screenInfo_.height);
    curDisplayDirtyManager_->SetActiveSurfaceRect(screenInfo_.activeRect);

    // 3 init Occlusion info
    needRecalculateOcclusion_ = false;
    accumulatedOcclusionRegion_.Reset();
    occlusionRegionWithoutSkipLayer_.Reset();
    if (!curMainAndLeashWindowNodesIds_.empty()) {
        std::queue<NodeId>().swap(curMainAndLeashWindowNodesIds_);
    }

    // 4. check isHardwareForcedDisabled
    auto& geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        RS_LOGE("RSUniRenderVisitor::InitDisplayInfo geoPtr is nullptr");
        return false;
    }
    if (geoPtr->IsNeedClientCompose()) {
        isHardwareForcedDisabled_ = true;
    }

    // 5. check compositeType
    auto mirrorNode = node.GetMirrorSource().lock();
    node.SetIsMirrorScreen(mirrorNode != nullptr);
    switch (screenInfo_.state) {
        case ScreenState::SOFTWARE_OUTPUT_ENABLE:
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
    UpdateSecuritySkipAndProtectedLayersRecord(node);
    node.UpdateUIFirstFrameGravity();
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        // UpdateCurCornerRadius must process before curSurfaceNode_ update
        node.UpdateCurCornerRadius(curCornerRadius_);
        curSurfaceNode_ = node.ReinterpretCastTo<RSSurfaceRenderNode>();
        curSurfaceDirtyManager_ = node.GetDirtyManager();
        if (!curSurfaceDirtyManager_ || !curSurfaceNode_) {
            RS_LOGE("RSUniRenderVisitor::BeforeUpdateSurfaceDirtyCalc %{public}s has invalid"
                " SurfaceDirtyManager or node ptr", node.GetName().c_str());
            return false;
        }
        curSurfaceDirtyManager_->Clear();
        curSurfaceDirtyManager_->SetSurfaceSize(screenInfo_.width, screenInfo_.height);
        curSurfaceDirtyManager_->SetActiveSurfaceRect(screenInfo_.activeRect);
        filterInGlobal_ = curSurfaceNode_->IsTransparent();
        // update surfaceNode contentDirty and subTreeDirty flag for UIFirst purging policy
        RSMainThread::Instance()->CheckAndUpdateInstanceContentStaticStatus(curSurfaceNode_);
        curSurfaceNode_->UpdateSurfaceCacheContentStaticFlag();
        curSurfaceNode_->UpdateSurfaceSubTreeDirtyFlag();
        curSurfaceNode_->SetLeashWindowVisibleRegionEmpty(false);
    } else if (node.IsAbilityComponent()) {
        if (auto nodePtr = node.ReinterpretCastTo<RSSurfaceRenderNode>()) {
            RSMainThread::Instance()->CheckAndUpdateInstanceContentStaticStatus(nodePtr);
            nodePtr->UpdateSurfaceCacheContentStaticFlag();
        }
    }
    // 2. update surface info and CheckIfOcclusionReusable
    node.SetAncestorDisplayNode(curDisplayNode_); // set for boot animation
    node.UpdateAncestorDisplayNodeInRenderParams();
    node.CleanDstRectChanged();
    // [planning] check node isDirty can be optimized.
    needRecalculateOcclusion_ = needRecalculateOcclusion_ || node.IsDirty() ||
        node.CheckIfOcclusionReusable(preMainAndLeashWindowNodesIds_);
    if (autoCacheEnable_ && node.IsAppWindow()) {
        node.OpincSetInAppStateStart(unchangeMarkInApp_);
    }
    // 3. check color space pixelFormat and update RelMatrix
    CheckColorSpace(node);
    CheckPixelFormat(node);
    if (node.GetRSSurfaceHandler() && node.GetRSSurfaceHandler()->GetBuffer()) {
        node.SetBufferRelMatrix(RSUniRenderUtil::GetMatrixOfBufferToRelRect(node));
    }
    return true;
}

bool RSUniRenderVisitor::AfterUpdateSurfaceDirtyCalc(RSSurfaceRenderNode& node)
{
    // 1. Update surfaceNode info and AppWindow gravity
    const auto& property = node.GetRenderProperties();
    if (node.IsAppWindow()) {
        boundsRect_ = Drawing::Rect(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        frameGravity_ = property.GetFrameGravity();
    }
    auto& geoPtr = property.GetBoundsGeometry();
    if (geoPtr == nullptr) {
        return false;
    }
    UpdateDstRect(node, geoPtr->GetAbsRect(), prepareClipRect_);
    node.UpdatePositionZ();
    if (node.IsHardwareEnabledType() && node.GetZorderChanged() && curSurfaceNode_) {
        curSurfaceNode_->SetNeedCollectHwcNode(true);
    }
    UpdateSurfaceRenderNodeScale(node);
    UpdateSurfaceRenderNodeRotate(node);
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        curDisplayNode_->UpdateSurfaceNodePos(node.GetId(), node.GetOldDirtyInSurface());
        curDisplayNode_->AddSurfaceNodePosByDescZOrder(node.GetId(), node.GetOldDirtyInSurface());
    }
    // 2. Update Occlusion info before children preparation
    if (node.IsMainWindowType()) {
        CalculateOcclusion(node);
        if (node.GetFirstLevelNodeId() == node.GetId()) {
            globalSurfaceBounds_.emplace_back(node.GetAbsDrawRect());
        }
    }
    // 3. Update HwcNode Info for appNode
    UpdateHwcNodeInfoForAppNode(node);
    return true;
}

void RSUniRenderVisitor::UpdateLeashWindowVisibleRegionEmpty(RSSurfaceRenderNode& node)
{
    if (!node.IsLeashWindow()) {
        return;
    }
    bool isVisibleRegionEmpty = true;
    for (const auto& child : *node.GetSortedChildren()) {
        const auto childSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
        if (childSurfaceNode && childSurfaceNode->IsAppWindow()) {
            // leash window is visible when child app has visible region
            if (!childSurfaceNode->GetVisibleRegion().IsEmpty()) {
                isVisibleRegionEmpty = false;
            } else {
                RS_OPTIONAL_TRACE_NAME_FMT("%s's visible region is empty", childSurfaceNode->GetName().c_str());
            }
        } else {
            // leash window is visible when child is not app window node
            isVisibleRegionEmpty = false;
        }
        if (!isVisibleRegionEmpty) {
            break;
        }
    }
    node.SetLeashWindowVisibleRegionEmpty(isVisibleRegionEmpty);
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
        if (!node.GetHardWareDisabledByReverse()) {
            node.SetHardwareForcedDisabledState(node.GetIsHwcPendingDisabled());
            node.SetIsHwcPendingDisabled(false);
        }
        node.SetInFixedRotation(displayNodeRotationChanged_ || isScreenRotationAnimating_);
        if (!IsHardwareComposerEnabled() || !node.IsDynamicHardwareEnable() ||
            curSurfaceNode_->GetVisibleRegion().IsEmpty() ||
            !node.GetRSSurfaceHandler() || !node.GetRSSurfaceHandler()->GetBuffer()) {
            RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by param/invisible/no buffer",
                node.GetName().c_str(), node.GetId());
            node.SetHardwareForcedDisabledState(true);
            hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(node.GetId(),
                HwcDisabledReasons::DISABLED_BY_INVALID_PARAM, node.GetName());
            if (!node.GetFixRotationByUser()) {
                return;
            }
        }
        auto& geo = node.GetRenderProperties().GetBoundsGeometry();
        if (geo == nullptr) {
            return;
        }
        UpdateSrcRect(node, geo->GetAbsMatrix(), geo->GetAbsRect());
        UpdateHwcNodeByTransform(node);
        UpdateHwcNodeEnableByBackgroundAlpha(node);
        UpdateHwcNodeEnableByBufferSize(node);
        UpdateHwcNodeEnableBySrcRect(node);
    }
}

void RSUniRenderVisitor::UpdateSrcRect(RSSurfaceRenderNode& node,
    const Drawing::Matrix& absMatrix, const RectI& absRect)
{
    auto canvas = std::make_unique<Rosen::Drawing::Canvas>(screenInfo_.phyWidth, screenInfo_.phyHeight);
    canvas->ConcatMatrix(absMatrix);

    const auto& dstRect = node.GetDstRect();
    Drawing::RectI dst = { std::round(dstRect.GetLeft()), std::round(dstRect.GetTop()), std::round(dstRect.GetRight()),
                           std::round(dstRect.GetBottom()) };
    node.UpdateSrcRect(*canvas.get(), dst);
    if (node.GetRSSurfaceHandler() && node.GetRSSurfaceHandler()->GetBuffer()) {
        RSUniRenderUtil::UpdateRealSrcRect(node, absRect);
    }
}

void RSUniRenderVisitor::UpdateDstRect(RSSurfaceRenderNode& node, const RectI& absRect, const RectI& clipRect)
{
    auto dstRect = absRect;
    // If the screen is expanded, intersect the destination rectangle with the screen rectangle
    dstRect = dstRect.IntersectRect(RectI(curDisplayNode_->GetDisplayOffsetX(), curDisplayNode_->GetDisplayOffsetY(),
        screenInfo_.width, screenInfo_.height));
    // Remove the offset of the screen
    dstRect.left_ = dstRect.left_ - curDisplayNode_->GetDisplayOffsetX();
    dstRect.top_ = dstRect.top_ - curDisplayNode_->GetDisplayOffsetY();
    // If the node is a hardware-enabled type, intersect its destination rectangle with the prepare clip rectangle
    if (node.IsHardwareEnabledType()) {
        dstRect = dstRect.IntersectRect(clipRect);
        if (curSurfaceNode_ && (node.GetId() != curSurfaceNode_->GetId())) {
            dstRect = dstRect.IntersectRect(curSurfaceNode_->GetDstRect());
        }
    }
    dstRect.left_ = static_cast<int>(std::round(dstRect.left_ * screenInfo_.GetRogWidthRatio()));
    dstRect.top_ = static_cast<int>(std::round(dstRect.top_ * screenInfo_.GetRogHeightRatio()));
    dstRect.width_ = static_cast<int>(std::round(dstRect.width_ * screenInfo_.GetRogWidthRatio()));
    dstRect.height_ = static_cast<int>(std::round(dstRect.height_ * screenInfo_.GetRogHeightRatio()));
    // Set the destination rectangle of the node
    node.SetDstRect(dstRect);
}

void RSUniRenderVisitor::UpdateHwcNodeByTransform(RSSurfaceRenderNode& node)
{
    if (!node.GetRSSurfaceHandler() || !node.GetRSSurfaceHandler()->GetBuffer()) {
        return;
    }
    node.SetInFixedRotation(displayNodeRotationChanged_ || isScreenRotationAnimating_);
    RSUniRenderUtil::DealWithNodeGravity(node, screenInfo_);
    RSUniRenderUtil::LayerRotate(node, screenInfo_);
    RSUniRenderUtil::LayerCrop(node, screenInfo_);
    RSUniRenderUtil::DealWithScalingMode(node, screenInfo_);
    node.SetCalcRectInPrepare(true);
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByBackgroundAlpha(RSSurfaceRenderNode& node)
{
    if (node.GetAncoForceDoDirect()) {
        return;
    }
    bool bgTransport =
        static_cast<uint8_t>(node.GetRenderProperties().GetBackgroundColor().GetAlpha()) < UINT8_MAX;
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(node.GetStagingRenderParams().get());
    if (bgTransport) {
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by background color alpha < 1",
            node.GetName().c_str(), node.GetId());
        // use in skip updating hardware state for hwcnode with background alpha in specific situation
        if (!RsCommonHook::Instance().GetHardwareEnabledByBackgroundAlphaFlag()) {
            node.SetHardwareForcedDisabledState(true);
        }
        node.SetNodeHasBackgroundColorAlpha(true);
        hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(node.GetId(),
            HwcDisabledReasons::DISABLED_BY_BACKGROUND_ALPHA, node.GetName());
    } else if (RsCommonHook::Instance().GetIsWhiteListForSolidColorLayerFlag() &&
        stagingSurfaceParams->GetSelfDrawingNodeType() == SelfDrawingNodeType::XCOM &&
        node.GetRenderProperties().GetBackgroundColor() != RgbPalette::Black()) {
        stagingSurfaceParams->SetIsHwcEnabledBySolidLayer(true);
    } else if (!RsCommonHook::Instance().GetIsWhiteListForSolidColorLayerFlag() &&
        stagingSurfaceParams->GetSelfDrawingNodeType() == SelfDrawingNodeType::XCOM &&
        node.GetRenderProperties().GetBackgroundColor() != RgbPalette::Black()) {
        node.SetHardwareForcedDisabledState(true);
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by solid background color",
            node.GetName().c_str(), node.GetId());
    }
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByBufferSize(RSSurfaceRenderNode& node)
{
    if (!node.IsRosenWeb() || node.IsHardwareForcedDisabled()) {
        return;
    }
    if (!node.GetRSSurfaceHandler() || !node.GetRSSurfaceHandler()->GetBuffer()) {
        return;
    }
    const auto& property = node.GetRenderProperties();
    auto gravity = property.GetFrameGravity();
    if (gravity != Gravity::TOP_LEFT) {
        return;
    }
    auto surfaceHandler = node.GetRSSurfaceHandler();
    auto consumer = surfaceHandler->GetConsumer();
    if (consumer == nullptr) {
        return;
    }

    auto buffer = surfaceHandler->GetBuffer();
    const auto bufferWidth = buffer->GetSurfaceBufferWidth();
    const auto bufferHeight = buffer->GetSurfaceBufferHeight();
    auto boundsWidth = property.GetBoundsWidth();
    auto boundsHeight = property.GetBoundsHeight();

    auto transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    if (consumer->GetSurfaceBufferTransformType(buffer, &transformType) != GSERROR_OK) {
        RS_LOGE("RSUniRenderVisitor::UpdateHwcNodeEnableByBufferSize GetSurfaceBufferTransformType failed");
    }
    if (transformType == GraphicTransformType::GRAPHIC_ROTATE_270 ||
        transformType == GraphicTransformType::GRAPHIC_ROTATE_90) {
        std::swap(boundsWidth, boundsHeight);
    }
    if ((bufferWidth < boundsWidth) || (bufferHeight < boundsHeight)) {
        RS_OPTIONAL_TRACE_NAME_FMT(
            "hwc debug: name:%s id:%" PRIu64 " buffer:[%d, %d] bounds:[%f, %f] disabled by buffer nonmatching",
            node.GetName().c_str(), node.GetId(), bufferWidth, bufferHeight, boundsWidth, boundsHeight);
        node.SetHardwareForcedDisabledState(true);
        hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(
            node.GetId(), HwcDisabledReasons::DISABLED_BY_BUFFER_NONMATCH, node.GetName());
    }
}

void RSUniRenderVisitor::UpdateHwcNodeEnableBySrcRect(RSSurfaceRenderNode& node)
{
    if (node.IsHardwareForcedDisabled()) {
        return;
    }
    bool hasRotation = false;
    if (node.GetRSSurfaceHandler() && node.GetRSSurfaceHandler()->GetConsumer()) {
        const auto consumer = node.GetRSSurfaceHandler()->GetConsumer();
        auto rotation = RSBaseRenderUtil::GetRotateTransform(
            RSBaseRenderUtil::GetSurfaceBufferTransformType(consumer, node.GetRSSurfaceHandler()->GetBuffer()));
        hasRotation = rotation == GRAPHIC_ROTATE_90 || rotation == GRAPHIC_ROTATE_270;
    }
    node.UpdateHwcDisabledBySrcRect(hasRotation);
    if (node.IsHardwareDisabledBySrcRect()) {
        node.SetHardwareForcedDisabledState(true);
        hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(node.GetId(),
            HwcDisabledReasons::DISABLED_BY_SRC_PIXEL, node.GetName());
    }
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(std::vector<RectI>& hwcRects,
    std::shared_ptr<RSSurfaceRenderNode>& hwcNode)
{
    if (hwcNode->IsHardwareForcedDisabled()) {
        return;
    }
    auto dst = hwcNode->GetDstRect();
    if (hwcNode->GetAncoForceDoDirect()) {
        hwcRects.emplace_back(dst);
        return;
    }
    for (auto rect : hwcRects) {
        if (dst.Intersect(rect) && !RsCommonHook::Instance().GetHardwareEnabledByHwcnodeBelowSelfInAppFlag()) {
            if (RsCommonHook::Instance().GetVideoSurfaceFlag() &&
                ((dst.GetBottom() - rect.GetTop() <= MIN_OVERLAP && dst.GetBottom() - rect.GetTop() >= 0) ||
                (rect.GetBottom() - dst.GetTop() <= MIN_OVERLAP && rect.GetBottom() - dst.GetTop() >= 0))) {
                return;
            }
            RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by hwc node above",
                hwcNode->GetName().c_str(), hwcNode->GetId());
            hwcNode->SetHardwareForcedDisabledState(true);
            hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(hwcNode->GetId(),
                HwcDisabledReasons::DISABLED_BY_HWC_NODE_ABOVE, hwcNode->GetName());
            return;
        }
    }
    hwcRects.emplace_back(dst);
}

void RSUniRenderVisitor::UpdateHwcNodeProperty(std::shared_ptr<RSSurfaceRenderNode> hwcNode)
{
    auto hwcNodeGeo = hwcNode->GetRenderProperties().GetBoundsGeometry();
    if (!hwcNodeGeo) {
        RS_LOGE("hwcNode Geometry is not prepared.");
        return;
    }
    bool hasCornerRadius = !hwcNode->GetRenderProperties().GetCornerRadius().IsZero();
    std::vector<RectI> currIntersectedRoundCornerAABBs = {};
    float alpha = hwcNode->GetRenderProperties().GetAlpha();
    Drawing::Matrix totalMatrix = hwcNodeGeo->GetMatrix();
    auto hwcNodeRect = hwcNodeGeo->GetAbsRect();
    bool isNodeRenderByDrawingCache = false;
    RSUniRenderUtil::TraverseParentNodeAndReduce(hwcNode,
        [&isNodeRenderByDrawingCache](std::shared_ptr<RSRenderNode> parent) {
            if (isNodeRenderByDrawingCache) {
                return;
            }
            // if the parent node of hwcNode is marked freeze or nodegroup, RS closes hardware composer of hwcNode.
            isNodeRenderByDrawingCache = isNodeRenderByDrawingCache || parent->IsStaticCached() ||
                (parent->GetNodeGroupType() != RSRenderNode::NodeGroupType::NONE);
        },
        [&alpha](std::shared_ptr<RSRenderNode> parent) {
            auto& parentProperty = parent->GetRenderProperties();
            alpha *= parentProperty.GetAlpha();
        },
        [&totalMatrix](std::shared_ptr<RSRenderNode> parent) {
            if (auto opt = RSUniRenderUtil::GetMatrix(parent)) {
                totalMatrix.PostConcat(opt.value());
            } else {
                return;
            }
        },
        [&currIntersectedRoundCornerAABBs, hwcNodeRect](std::shared_ptr<RSRenderNode> parent) {
            auto& parentProperty = parent->GetRenderProperties();
            auto cornerRadius = parentProperty.GetCornerRadius();
            auto maxCornerRadius = *std::max_element(std::begin(cornerRadius.data_), std::end(cornerRadius.data_));
            auto parentGeo = parentProperty.GetBoundsGeometry();
            static const std::array offsetVecs {
                UIPoint { 0, 0 },
                UIPoint { 1, 0 },
                UIPoint { 0, 1 },
                UIPoint { 1, 1 }
            };
            
            // The logic here is to calculate whether the HWC Node affects
            // the round corner property of the parent node.
            // The method is calculating the rounded AABB of each HWC node
            // with respect to all parent nodes above it and storing the results.
            // When a HWC node is found below, the AABBs and the HWC node
            // are checked for intersection. If there is an intersection,
            // the node above it is disabled from taking the HWC pipeline.
            auto checkIntersectWithRoundCorner = [&currIntersectedRoundCornerAABBs, hwcNodeRect](
                const RectI& rect, float radiusX, float radiusY) {
                if (radiusX <= 0 || radiusY <= 0) {
                    return;
                }
                UIPoint offset { rect.GetWidth() - radiusX, rect.GetHeight() - radiusY };
                UIPoint anchorPoint { rect.GetLeft(), rect.GetTop() };
                std::for_each(std::begin(offsetVecs), std::end(offsetVecs),
                    [&currIntersectedRoundCornerAABBs, hwcNodeRect, offset,
                        radiusX, radiusY, anchorPoint](auto offsetVec) {
                        auto res = anchorPoint + offset * offsetVec;
                        auto roundCornerAABB = RectI(res.x_, res.y_, radiusX, radiusY);
                        if (!roundCornerAABB.IntersectRect(hwcNodeRect).IsEmpty()) {
                            currIntersectedRoundCornerAABBs.push_back(roundCornerAABB);
                        }
                    }
                );
            };
            if (parentGeo) {
                auto parentRect = parentGeo->GetAbsRect();
                checkIntersectWithRoundCorner(parentRect, maxCornerRadius, maxCornerRadius);

                if (parentProperty.GetClipToRRect()) {
                    RRect parentClipRRect = parentProperty.GetClipRRect();
                    RectI parentClipRect = parentGeo->MapAbsRect(parentClipRRect.rect_);
                    float maxClipRRectCornerRadiusX = 0, maxClipRRectCornerRadiusY = 0;
                    constexpr size_t radiusVecSize = 4;
                    for (size_t i = 0; i < radiusVecSize; ++i) {
                        maxClipRRectCornerRadiusX = std::max(maxClipRRectCornerRadiusX, parentClipRRect.radius_[i].x_);
                        maxClipRRectCornerRadiusY = std::max(maxClipRRectCornerRadiusY, parentClipRRect.radius_[i].y_);
                    }
                    checkIntersectWithRoundCorner(parentClipRect, maxClipRRectCornerRadiusX, maxClipRRectCornerRadiusY);
                }
            }
        }
    );
    if (isNodeRenderByDrawingCache) {
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by drawing cache",
            hwcNode->GetName().c_str(), hwcNode->GetId());
        hwcNode->SetHardwareForcedDisabledState(isNodeRenderByDrawingCache);
    }
    hwcNode->SetTotalMatrix(totalMatrix);
    hwcNode->SetGlobalAlpha(alpha);
    hwcNode->SetIntersectedRoundCornerAABBs(std::move(currIntersectedRoundCornerAABBs));
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByRotateAndAlpha(std::shared_ptr<RSSurfaceRenderNode>& hwcNode)
{
    auto alpha = hwcNode->GetGlobalAlpha();
    auto totalMatrix = hwcNode->GetTotalMatrix();
    if (alpha < 1.0f) {
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by accumulated alpha:%.2f",
            hwcNode->GetName().c_str(), hwcNode->GetId(), alpha);
        hwcNode->SetHardwareForcedDisabledState(true);
        hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(hwcNode->GetId(),
            HwcDisabledReasons::DISABLED_BY_ACCUMULATED_ALPHA, hwcNode->GetName());
        return;
    }
    // [planning] degree only multiples of 90 now
    float degree = RSUniRenderUtil::GetFloatRotationDegreeFromMatrix(totalMatrix);
    bool hasRotate = !ROSEN_EQ(std::remainder(degree, 90.f), 0.f, EPSILON);
    hasRotate = hasRotate || RSUniRenderUtil::HasNonZRotationTransform(totalMatrix);
    if (hasRotate) {
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by rotation:%d",
            hwcNode->GetName().c_str(), hwcNode->GetId(), degree);
        hwcNode->SetHardwareForcedDisabledState(true);
        hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(hwcNode->GetId(),
            HwcDisabledReasons::DISABLED_BY_ROTATION, hwcNode->GetName());
        return;
    }
}

void RSUniRenderVisitor::ProcessAncoNode(std::shared_ptr<RSSurfaceRenderNode>& hwcNodePtr)
{
    if ((hwcNodePtr->GetAncoFlags() & static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE)) == 0) {
        return;
    }
    ancoNodes_.insert(hwcNodePtr);
    auto alpha = hwcNodePtr->GetGlobalAlpha();
    RS_LOGD("rs debug: name:%{public}s id:%{public}" PRIu64 "src %{public}s dst %{public}s "
        "alpha:%{public}.2f", hwcNodePtr->GetName().c_str(), hwcNodePtr->GetId(),
        hwcNodePtr->GetSrcRect().ToString().c_str(), hwcNodePtr->GetDstRect().ToString().c_str(), alpha);
    if (ROSEN_EQ(alpha, 0.0f)) {
        return;
    }

    if (!hwcNodePtr->GetRSSurfaceHandler() || !hwcNodePtr->GetRSSurfaceHandler()->GetBuffer()) {
        RS_LOGD("rs debug: name:%{public}s id:%{public}" PRIu64 " handler or buffer is null, skip",
            hwcNodePtr->GetName().c_str(), hwcNodePtr->GetId());
        return;
    }

    ancoHasGpu_ = (ancoHasGpu_ || hwcNodePtr->IsHardwareForcedDisabled());
}

void RSUniRenderVisitor::InitAncoStatus()
{
    ancoHasGpu_ = false;
    ancoNodes_.clear();
}

void RSUniRenderVisitor::UpdateHwcNodeEnable()
{
    InitAncoStatus();

    auto& curMainAndLeashSurfaces = curDisplayNode_->GetAllMainAndLeashSurfaces();
    std::for_each(curMainAndLeashSurfaces.rbegin(), curMainAndLeashSurfaces.rend(),
        [this](RSBaseRenderNode::SharedPtr& nodePtr) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodePtr);
        if (!surfaceNode) {
            return;
        }

        if (RSSystemProperties::GetHveFilterEnabled()) {
            const auto &preHwcNodes = surfaceNode->GetChildHardwareEnabledNodes();
            for (const auto& preHwcNode : preHwcNodes) {
                auto hwcNodePtr = preHwcNode.lock();
                if (!hwcNodePtr || !hwcNodePtr->IsOnTheTree()) {
                    continue;
                }
                hwcNodePtr->ResetMakeImageState();
            }
        }

        UpdateHwcNodeEnableByGlobalFilter(surfaceNode);
        surfaceNode->ResetNeedCollectHwcNode();
        const auto& hwcNodes = surfaceNode->GetChildHardwareEnabledNodes();
        if (hwcNodes.empty()) {
            return;
        }
        std::vector<RectI> hwcRects;
        for (const auto& hwcNode : hwcNodes) {
            auto hwcNodePtr = hwcNode.lock();
            if (!hwcNodePtr || !hwcNodePtr->IsOnTheTree()) {
                continue;
            }
            if (hwcNodePtr->GetProtectedLayer()) {
                drmNodes_.emplace_back(hwcNode);
            }
            UpdateHwcNodeProperty(hwcNodePtr);
            UpdateHwcNodeEnableByRotateAndAlpha(hwcNodePtr);
            UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(hwcRects, hwcNodePtr);

            ProcessAncoNode(hwcNodePtr);
        }
    });
    UpdateAncoNodeHWCDisabledState();
    PrevalidateHwcNode();
    UpdateHwcNodeEnableByNodeBelow();
}

void RSUniRenderVisitor::UpdateAncoNodeHWCDisabledState()
{
    if (ancoHasGpu_) {
        for (const auto& hwcNodePtr : ancoNodes_) {
            hwcNodePtr->SetHardwareForcedDisabledState(true);
        }
    }

    InitAncoStatus();
}

void RSUniRenderVisitor::PrevalidateHwcNode()
{
    if (!isPrevalidateHwcNodeEnable_) {
        RS_LOGD_IF(DEBUG_PREVALIDATE, "RSUniRenderVisitor::PrevalidateHwcNode prevalidate close");
        return;
    }
    auto& curMainAndLeashSurfaces = curDisplayNode_->GetAllMainAndLeashSurfaces();
    std::vector<RequestLayerInfo> prevalidLayers;
    uint32_t curFps = OHOS::Rosen::HgmCore::Instance().GetScreenCurrentRefreshRate(curDisplayNode_->GetScreenId());
    uint32_t zOrder = static_cast<uint32_t>(globalZOrder_);
    // add surfaceNode layer
    RSUniHwcPrevalidateUtil::GetInstance().CollectSurfaceNodeLayerInfo(
        prevalidLayers, curMainAndLeashSurfaces, curFps, zOrder, screenInfo_);
    std::vector<RequestLayerInfo> uiFirstLayers;
    // collect uifirst layer
    // zOrder + 1.f is displayNode, UIFirst layer must be above displayNode(zorder + 2.f)
    RSUniHwcPrevalidateUtil::GetInstance().CollectUIFirstLayerInfo(
        uiFirstLayers, curFps, static_cast<float>(zOrder) + 2.f, screenInfo_);
    RS_TRACE_NAME_FMT("PrevalidateHwcNode hwcLayer: %u, uifirstLayer: %u", prevalidLayers.size(), uiFirstLayers.size());
    if (prevalidLayers.size() == 0 && uiFirstLayers.size() == 0) {
        RS_LOGI_IF(DEBUG_PREVALIDATE, "RSUniRenderVisitor::PrevalidateHwcNode no hardware layer");
        return;
    }
    // add display layer
    RequestLayerInfo displayLayer;
    if (RSUniHwcPrevalidateUtil::GetInstance().CreateDisplayNodeLayerInfo(
        zOrder++, curDisplayNode_, screenInfo_, curFps, displayLayer)) {
        prevalidLayers.emplace_back(displayLayer);
    }
    // add uiFirst layer
    prevalidLayers.insert(prevalidLayers.end(), uiFirstLayers.begin(), uiFirstLayers.end());
    // add rcd layer
    RequestLayerInfo rcdLayer;
    if (RSSingleton<RoundCornerDisplayManager>::GetInstance().GetRcdEnable()) {
        auto rcdSurface = RSRcdRenderManager::GetInstance().GetBottomSurfaceNode(curDisplayNode_->GetId());
        if (RSUniHwcPrevalidateUtil::GetInstance().CreateRCDLayerInfo(rcdSurface, screenInfo_, curFps, rcdLayer)) {
            prevalidLayers.emplace_back(rcdLayer);
        }
        rcdSurface = RSRcdRenderManager::GetInstance().GetTopSurfaceNode(curDisplayNode_->GetId());
        if (RSUniHwcPrevalidateUtil::GetInstance().CreateRCDLayerInfo(rcdSurface, screenInfo_, curFps, rcdLayer)) {
            prevalidLayers.emplace_back(rcdLayer);
        }
    }
    std::map<uint64_t, RequestCompositionType> strategy;
    if (!RSUniHwcPrevalidateUtil::GetInstance().PreValidate(screenInfo_.id, prevalidLayers, strategy)) {
        RS_LOGI_IF(DEBUG_PREVALIDATE, "RSUniRenderVisitor::PrevalidateHwcNode prevalidate failed");
        return;
    }
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (auto it : strategy) {
        RS_LOGD_IF(DEBUG_PREVALIDATE, "RSUniRenderVisitor::PrevalidateHwcNode id: %{public}" PRIu64 ","
            " result: %{public}d", it.first, it.second);
        if (it.second == RequestCompositionType::DEVICE) {
            continue;
        }
        auto node = nodeMap.GetRenderNode<RSSurfaceRenderNode>(it.first);
        if (node == nullptr) {
            continue;
        }
        if (it.second == RequestCompositionType::DEVICE_VSCF) {
            node->SetArsrTag(false);
            continue;
        }
        if (node->IsInFixedRotation() || node->GetProtectedLayer()) {
            continue;
        }
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by prevalidate",
            node->GetName().c_str(), node->GetId());
        node->SetHardwareForcedDisabledState(true);
        if (node->GetRSSurfaceHandler()) {
            node->GetRSSurfaceHandler()->SetGlobalZOrder(-1.f);
        }
        hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(node->GetId(),
            HwcDisabledReasons::DISABLED_BY_PREVALIDATE, node->GetName());
    }
}

void RSUniRenderVisitor::UpdateHwcNodeDirtyRegionAndCreateLayer(std::shared_ptr<RSSurfaceRenderNode>& node)
{
    const auto& hwcNodes = node->GetChildHardwareEnabledNodes();
    if (hwcNodes.empty()) {
        return;
    }
    std::shared_ptr<RSSurfaceRenderNode> pointWindow;
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> topLayers;
    for (auto hwcNode : hwcNodes) {
        auto hwcNodePtr = hwcNode.lock();
        if (!hwcNodePtr || !hwcNodePtr->IsOnTheTree()) {
            continue;
        }
        auto surfaceHandler = hwcNodePtr->GetMutableRSSurfaceHandler();
        if (hwcNodePtr->IsLayerTop()) {
            topLayers.emplace_back(hwcNodePtr);
            continue;
        }
        if (node->IsHardwareEnabledTopSurface()) {
            pointWindow = hwcNodePtr;
            continue;
        }
        if ((hasUniRenderHdrSurface_ || !drmNodes_.empty()) && !hwcNodePtr->GetProtectedLayer()) {
            RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64
                " disabled by having UniRenderHdrSurface/DRM nodes",
                hwcNodePtr->GetName().c_str(), hwcNodePtr->GetId());
            hwcNodePtr->SetHardwareForcedDisabledState(true);
            // DRM will force HDR to use unirender
            hasUniRenderHdrSurface_ = hasUniRenderHdrSurface_ || RSMainThread::CheckIsHdrSurface(*hwcNodePtr);
        }
        UpdateHwcNodeDirtyRegionForApp(node, hwcNodePtr);
        hwcNodePtr->SetCalcRectInPrepare(false);
        hwcNodePtr->SetHardWareDisabledByReverse(false);
        surfaceHandler->SetGlobalZOrder(hwcNodePtr->IsHardwareForcedDisabled() && !hwcNodePtr->GetProtectedLayer()
            ? -1.f : globalZOrder_++);
        auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(hwcNodePtr->GetStagingRenderParams().get());
        if (stagingSurfaceParams->GetIsHwcEnabledBySolidLayer()) {
            surfaceHandler->SetGlobalZOrder(globalZOrder_++);
        }
        auto transform = RSUniRenderUtil::GetLayerTransform(*hwcNodePtr, screenInfo_);
        hwcNodePtr->UpdateHwcNodeLayerInfo(transform);
    }
    curDisplayNode_->SetDisplayGlobalZOrder(globalZOrder_);
    if (!topLayers.empty()) {
        UpdateTopLayersDirtyStatus(topLayers);
    }
    if (pointWindow) {
        UpdatePointWindowDirtyStatus(pointWindow);
    }
}

void RSUniRenderVisitor::UpdatePointWindowDirtyStatus(std::shared_ptr<RSSurfaceRenderNode>& pointWindow)
{
    std::shared_ptr<RSSurfaceHandler> pointSurfaceHandler = pointWindow->GetMutableRSSurfaceHandler();
    if (pointSurfaceHandler) {
        // globalZOrder_ + 2 is displayNode layer, point window must be at the top.
        pointSurfaceHandler->SetGlobalZOrder(globalZOrder_ + 2);
        pointWindow->SetHardwareForcedDisabledState(!IsHardwareComposerEnabled() || !pointWindow->ShouldPaint() ||
            hasUniRenderHdrSurface_);
        auto transform = RSUniRenderUtil::GetLayerTransform(*pointWindow, screenInfo_);
        pointWindow->UpdateHwcNodeLayerInfo(transform);
    }
}

void RSUniRenderVisitor::UpdateTopLayersDirtyStatus(const std::vector<std::shared_ptr<RSSurfaceRenderNode>>& topLayers)
{
    for (const auto& topLayer : topLayers) {
        std::shared_ptr<RSSurfaceHandler> topLayerSurfaceHandler = topLayer->GetMutableRSSurfaceHandler();
        if (topLayerSurfaceHandler) {
            topLayerSurfaceHandler->SetGlobalZOrder(globalZOrder_ + 1);
            topLayer->SetCalcRectInPrepare(false);
            topLayer->SetHardwareForcedDisabledState(!IsHardwareComposerEnabled() || !topLayer->ShouldPaint() ||
                hasUniRenderHdrSurface_ || !drmNodes_.empty());
            auto transform = RSUniRenderUtil::GetLayerTransform(*topLayer, screenInfo_);
            topLayer->UpdateHwcNodeLayerInfo(transform);
        }
    }
}

void RSUniRenderVisitor::UpdateHwcNodeDirtyRegionForApp(std::shared_ptr<RSSurfaceRenderNode>& appNode,
    std::shared_ptr<RSSurfaceRenderNode>& hwcNode)
{
    // if current frame hwc enable status not equal with last frame
    // or current frame do gpu composition and has buffer consumed,
    // we need merge hwc node dst rect to dirty region.
    if (!hwcNode->IsHardwareForcedDisabled() != hwcNode->GetIsLastFrameHwcEnabled()) {
        appNode->GetDirtyManager()->MergeDirtyRect(hwcNode->GetDstRect());
        return;
    }
    if (!hwcNode->GetRSSurfaceHandler()) {
        return;
    }
    if (hwcNode->IsHardwareForcedDisabled() && hwcNode->GetRSSurfaceHandler()->IsCurrentFrameBufferConsumed()) {
        appNode->GetDirtyManager()->MergeDirtyRect(hwcNode->GetOldDirtyInSurface());
    }
    if (hasMirrorDisplay_ && hwcNode->GetRSSurfaceHandler()->IsCurrentFrameBufferConsumed() &&
        !appNode->GetVisibleRegion().IsEmpty()) {
        // merge hwc node dst rect for virtual screen dirty, in case the main display node skip
        curDisplayDirtyManager_->MergeHwcDirtyRect(hwcNode->GetDstRect());
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
        if (!surfaceNode) {
            RS_LOGE("RSUniRenderVisitor::UpdateSurfaceDirtyAndGlobalDirty surfaceNode is nullptr");
            return;
        }
        auto dirtyManager = surfaceNode->GetDirtyManager();
        RSMainThread::Instance()->GetContext().AddPendingSyncNode(nodePtr);
        // 0. update hwc node dirty region and create layer
        UpdateHwcNodeDirtyRegionAndCreateLayer(surfaceNode);
        // 1. calculate abs dirtyrect and update partialRenderParams
        // currently only sync visible region info
        surfaceNode->UpdatePartialRenderParams();
        if (dirtyManager && dirtyManager->IsCurrentFrameDirty()) {
            hasMainAndLeashSurfaceDirty = true;
        }
        // 2. check surface node dirtyrect need merge into displayDirtyManager
        CheckMergeSurfaceDirtysForDisplay(surfaceNode);
        // 3. check merge transparent filter when it intersects with pre-dirty
        CheckMergeDisplayDirtyByTransparentFilter(surfaceNode, accumulatedDirtyRegion);
    });
    curDisplayNode_->SetMainAndLeashSurfaceDirty(hasMainAndLeashSurfaceDirty);
    CheckMergeDebugRectforRefreshRate(curMainAndLeashSurfaces);
    CheckMergeGlobalFilterForDisplay(accumulatedDirtyRegion);
    ResetDisplayDirtyRegion();
    if (curDisplayDirtyManager_) {
        curDisplayDirtyManager_->ClipDirtyRectWithinSurface();
        if (curDisplayDirtyManager_->IsActiveSurfaceRectChanged()) {
            RS_TRACE_NAME_FMT("ActiveSurfaceRectChanged, form %s to %s",
                curDisplayDirtyManager_->GetLastActiveSurfaceRect().ToString().c_str(),
                curDisplayDirtyManager_->GetActiveSurfaceRect().ToString().c_str());
            curDisplayDirtyManager_->MergeDirtyRect(curDisplayDirtyManager_->GetSurfaceRect());
        }
    }
    curDisplayNode_->ClearCurrentSurfacePos();
    std::swap(preMainAndLeashWindowNodesIds_, curMainAndLeashWindowNodesIds_);

#ifdef RS_PROFILER_ENABLED
    RS_PROFILER_SET_DIRTY_REGION(accumulatedDirtyRegion);
#endif
}
void RSUniRenderVisitor::UpdateHwcNodeEnableByNodeBelow()
{
    auto& curMainAndLeashSurfaces = curDisplayNode_->GetAllMainAndLeashSurfaces();
    // this is used to record mainAndLeash surface accumulatedDirtyRegion by Pre-order travelsal
    std::vector<RectI> hwcRects;
    std::for_each(curMainAndLeashSurfaces.rbegin(), curMainAndLeashSurfaces.rend(),
        [this, &hwcRects](RSBaseRenderNode::SharedPtr& nodePtr) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodePtr);
        if (!surfaceNode) {
            RS_LOGE("RSUniRenderVisitor::UpdateHwcNodeEnableByNodeBelow surfaceNode is nullptr");
            return;
        }
        // use in temporary scheme to realize DSS
        auto& hwcNodes = surfaceNode->GetChildHardwareEnabledNodes();
        if (!hwcNodes.empty() && RsCommonHook::Instance().GetHardwareEnabledByBackgroundAlphaFlag() &&
            RsCommonHook::Instance().GetHardwareEnabledByHwcnodeBelowSelfInAppFlag()) {
            UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes);
        }
        // use end
        // disable hwc node with corner radius if intersects with hwc node below
        UpdateChildHwcNodeEnableByHwcNodeBelow(hwcRects, surfaceNode);
    });
}

void RSUniRenderVisitor::UpdateChildHwcNodeEnableByHwcNodeBelow(std::vector<RectI>& hwcRects,
    std::shared_ptr<RSSurfaceRenderNode>& appNode)
{
    const auto& hwcNodes = appNode->GetChildHardwareEnabledNodes();
    for (auto hwcNode : hwcNodes) {
        auto hwcNodePtr = hwcNode.lock();
        if (!hwcNodePtr || !hwcNodePtr->IsOnTheTree()) {
            continue;
        }
        UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, hwcNodePtr,
            hwcNodePtr->GetIntersectedRoundCornerAABBsSize() != 0);
    }
}

void RSUniRenderVisitor::UpdateCornerRadiusInfoForDRM(std::shared_ptr<RSSurfaceRenderNode> hwcNode,
    std::vector<RectI>& hwcRects)
{
    if (!hwcNode || !hwcNode->GetProtectedLayer()) {
        return;
    }
    auto instanceNode = hwcNode->GetInstanceRootNode() ?
        hwcNode->GetInstanceRootNode()->ReinterpretCastTo<RSSurfaceRenderNode>() : nullptr;
    if (!instanceNode) {
        hwcNode->SetCornerRadiusInfoForDRM({});
        return;
    }
    auto instanceAbsRect = instanceNode->GetAbsDrawRect();
    auto instanceCornerRadius = instanceNode->GetGlobalCornerRadius();
    if (instanceAbsRect.IsEmpty() || instanceCornerRadius.IsZero() ||
        ROSEN_EQ(instanceNode->GetRenderProperties().GetBoundsWidth(), 0.0f)) {
        hwcNode->SetCornerRadiusInfoForDRM({});
        return;
    }
    auto hwcGeo = hwcNode->GetRenderProperties().GetBoundsGeometry();
    if (!hwcGeo) {
        hwcNode->SetCornerRadiusInfoForDRM({});
        return;
    }
    auto hwcAbsRect = hwcGeo->MapRect(hwcNode->GetSelfDrawRect(), hwcNode->GetTotalMatrix());
    hwcAbsRect = hwcAbsRect.IntersectRect(instanceAbsRect);
    if (hwcAbsRect.IsEmpty()) {
        hwcNode->SetCornerRadiusInfoForDRM({});
        return;
    }
    auto ratio = static_cast<float>(instanceAbsRect.GetWidth()) /
        instanceNode->GetRenderProperties().GetBoundsWidth();
    std::vector<float> ratioVector = { 0.0f, 0.0f, 0.0f, 0.0f };
    bool isIntersectWithRoundCorner =
        CheckIfRoundCornerIntersectDRM(ratio, ratioVector, instanceCornerRadius, instanceAbsRect, hwcAbsRect);
    // store radius information when drm overlaps with other hwc nodes
    if (isIntersectWithRoundCorner) {
        for (const auto& rect : hwcRects) {
            if (hwcAbsRect.Intersect(rect)) {
                std::vector<float> drmCornerRadiusInfo = {
                    static_cast<float>(hwcAbsRect.GetLeft()), static_cast<float>(hwcAbsRect.GetTop()),
                    static_cast<float>(hwcAbsRect.GetWidth()), static_cast<float>(hwcAbsRect.GetHeight()),
                    // get corner radius num by index 0, 1, 2, 3
                    instanceCornerRadius[0] * ratioVector[0], instanceCornerRadius[1] * ratioVector[1],
                    instanceCornerRadius[2] * ratioVector[2], instanceCornerRadius[3] * ratioVector[3]};
                hwcNode->SetCornerRadiusInfoForDRM(drmCornerRadiusInfo);
                return;
            }
        }
    }
    hwcNode->SetCornerRadiusInfoForDRM({});
}

bool RSUniRenderVisitor::CheckIfRoundCornerIntersectDRM(const float& ratio, std::vector<float>& ratioVector,
    const Vector4f& instanceCornerRadius, const RectI& instanceAbsRect, const RectI& hwcAbsRect)
{
    auto maxRadius = *std::max_element(std::begin(instanceCornerRadius.data_),
        std::end(instanceCornerRadius.data_)) * ratio;
    bool isIntersectWithRoundCorner = false;
    static const std::vector<UIPoint> offsetVecs = { UIPoint { 0, 0 }, UIPoint { 1, 0 },
        UIPoint { 0, 1 }, UIPoint { 1, 1 } };
    UIPoint offset { instanceAbsRect.GetWidth() - maxRadius, instanceAbsRect.GetHeight() - maxRadius };
    UIPoint anchorPoint { instanceAbsRect.GetLeft(), instanceAbsRect.GetTop() };
    // if round corners intersect drm, update ratioVectors
    for (size_t i = 0; i < offsetVecs.size(); i++) {
        auto res = anchorPoint + offset * offsetVecs[i];
        if (RectI(res.x_, res.y_, maxRadius, maxRadius).Intersect(hwcAbsRect)) {
            isIntersectWithRoundCorner = true;
            ratioVector[i] = ratio;
        }
    }
    return isIntersectWithRoundCorner;
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByHwcNodeBelowSelf(std::vector<RectI>& hwcRects,
    std::shared_ptr<RSSurfaceRenderNode>& hwcNode, bool isIntersectWithRoundCorner)
{
    if (hwcNode->IsHardwareForcedDisabled()) {
        if (RSMainThread::CheckIsHdrSurface(*hwcNode)) {
            hasUniRenderHdrSurface_ = true;
        }
        return;
    }
    auto absBound = RectI();
    if (auto geo = hwcNode->GetRenderProperties().GetBoundsGeometry()) {
        absBound = geo->GetAbsRect();
    } else {
        return;
    }
    if (hwcNode->GetAncoForceDoDirect() || !isIntersectWithRoundCorner) {
        hwcRects.emplace_back(absBound);
        return;
    }
    for (const auto& rect : hwcRects) {
        for (auto& roundCornerAABB : hwcNode->GetIntersectedRoundCornerAABBs()) {
            if (!roundCornerAABB.IntersectRect(rect).IsEmpty()) {
                RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64
                    " disabled by corner radius + hwc node below",
                    hwcNode->GetName().c_str(), hwcNode->GetId());
                if (hwcNode->GetProtectedLayer()) {
                    continue;
                }
                hwcNode->SetHardwareForcedDisabledState(true);
                if (RSMainThread::CheckIsHdrSurface(*hwcNode)) {
                    hasUniRenderHdrSurface_ = true;
                }
                hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(hwcNode->GetId(),
                    HwcDisabledReasons::DISABLED_BY_HWC_NODE_ABOVE, hwcNode->GetName());
                return;
            }
        }
    }
    hwcRects.emplace_back(absBound);
}

void RSUniRenderVisitor::UpdateSurfaceOcclusionInfo()
{
    allDstCurVisVec_.insert(allDstCurVisVec_.end(), dstCurVisVec_.begin(), dstCurVisVec_.end());
    dstCurVisVec_.clear();
}

void RSUniRenderVisitor::CheckMergeDisplayDirtyByTransparent(RSSurfaceRenderNode& surfaceNode) const
{
    // surfaceNode is transparent
    const auto& dirtyRect = surfaceNode.GetDirtyManager()->GetCurrentFrameDirtyRegion();
    auto oldDirtyInSurface = surfaceNode.GetOldDirtyInSurface();
    Occlusion::Region visibleRegion = hasMirrorDisplay_ ?
        surfaceNode.GetVisibleRegionInVirtual() : surfaceNode.GetVisibleRegion();
    if (surfaceNode.IsMainWindowType() && !visibleRegion.IsIntersectWith(dirtyRect)) {
        return;
    }
    if (surfaceNode.IsTransparent()) {
        RectI transparentDirtyRect = oldDirtyInSurface.IntersectRect(dirtyRect);
        if (!transparentDirtyRect.IsEmpty()) {
            RS_LOGD("CheckMergeDisplayDirtyByTransparent global merge transparent dirty "
                "%{public}s: global dirty %{public}s, add rect %{public}s", surfaceNode.GetName().c_str(),
                curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
                transparentDirtyRect.ToString().c_str());
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(transparentDirtyRect);
        }
    }
    // surfaceNode has transparent regions
    CheckMergeDisplayDirtyByTransparentRegions(surfaceNode);
}

void RSUniRenderVisitor::CheckMergeDisplayDirtyByZorderChanged(RSSurfaceRenderNode& surfaceNode) const
{
    auto oldDirtyInSurface = surfaceNode.GetOldDirtyInSurface();
    if (surfaceNode.GetZorderChanged()) {
        RS_LOGD("CheckMergeDisplayDirtyByZorderChanged global merge GetZorderChanged "
            "%{public}s: global dirty %{public}s, add rect %{public}s", surfaceNode.GetName().c_str(),
            curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
            oldDirtyInSurface.ToString().c_str());
        curDisplayNode_->GetDirtyManager()->MergeDirtyRect(oldDirtyInSurface);
    }
}

void RSUniRenderVisitor::CheckMergeDisplayDirtyByPosChanged(RSSurfaceRenderNode& surfaceNode) const
{
    RectI lastFrameSurfacePos = curDisplayNode_->GetLastFrameSurfacePos(surfaceNode.GetId());
    RectI currentFrameSurfacePos = curDisplayNode_->GetCurrentFrameSurfacePos(surfaceNode.GetId());
    if (surfaceNode.GetAnimateState() || lastFrameSurfacePos != currentFrameSurfacePos) {
        RS_LOGD("CheckMergeDisplayDirtyByPosChanged global merge surface pos changed "
            "%{public}s: global dirty %{public}s, lastFrameRect %{public}s currentFrameRect %{public}s",
            surfaceNode.GetName().c_str(),
            curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
            lastFrameSurfacePos.ToString().c_str(), currentFrameSurfacePos.ToString().c_str());
        if (!lastFrameSurfacePos.IsEmpty()) {
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(lastFrameSurfacePos);
        }
        if (!currentFrameSurfacePos.IsEmpty()) {
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(currentFrameSurfacePos);
        }
    }
}

void RSUniRenderVisitor::CheckMergeDisplayDirtyByShadowChanged(RSSurfaceRenderNode& surfaceNode) const
{
    const auto& dirtyRect = surfaceNode.GetDirtyManager()->GetCurrentFrameDirtyRegion();
    auto oldDirtyInSurface = surfaceNode.GetOldDirtyInSurface();
    bool isShadowDisappear = !surfaceNode.GetRenderProperties().IsShadowValid() &&
        surfaceNode.IsShadowValidLastFrame();
    if (surfaceNode.GetRenderProperties().IsShadowValid() || isShadowDisappear) {
        RectI shadowDirtyRect = oldDirtyInSurface.IntersectRect(dirtyRect);
        // There are two situation here:
        // 1. SurfaceNode first has shadow or shadow radius is larger than the last frame,
        // dirtyRect == surfaceNode.GetOldDirtyInSurface()
        // 2. SurfaceNode remove shadow or shadow radius is smaller than the last frame,
        // dirtyRect > surfaceNode.GetOldDirtyInSurface()
        // So we should always merge dirtyRect here.
        if (!shadowDirtyRect.IsEmpty()) {
            RS_LOGD("CheckMergeDisplayDirtyByShadowChanged global merge ShadowValid %{public}s: "
                "global dirty %{public}s, add rect %{public}s", surfaceNode.GetName().c_str(),
                curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
                dirtyRect.ToString().c_str());
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(dirtyRect);
        }
        if (isShadowDisappear) {
            surfaceNode.SetShadowValidLastFrame(false);
        }
    }
}

void RSUniRenderVisitor::CheckMergeDisplayDirtyBySurfaceChanged() const
{
    std::vector<RectI> surfaceChangedRects = curDisplayNode_->GetSurfaceChangedRects();
    for (auto& surfaceChangedRect : surfaceChangedRects) {
        if (!surfaceChangedRect.IsEmpty()) {
            RS_LOGD("CheckMergeDisplayDirtyBySurfaceChanged global merge Surface closed, global dirty %{public}s,"
                "add rect %{public}s",
                curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
                surfaceChangedRect.ToString().c_str());
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(surfaceChangedRect);
        }
    }
}

void RSUniRenderVisitor::CheckMergeSurfaceDirtysForDisplay(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const
{
    if (surfaceNode->GetDirtyManager() == nullptr || curDisplayNode_->GetDirtyManager() == nullptr) {
        return;
    }
    // 1 Handles the case of transparent surface, merge transparent dirty rect
    CheckMergeDisplayDirtyByTransparent(*surfaceNode);
    // 2 Zorder changed case, merge surface dest Rect
    CheckMergeDisplayDirtyByZorderChanged(*surfaceNode);
    // 3 surfacePos chanded case, merge surface lastframe pos or curframe pos
    CheckMergeDisplayDirtyByPosChanged(*surfaceNode);
    // 4 shadow disappear and appear case.
    CheckMergeDisplayDirtyByShadowChanged(*surfaceNode);
    // 5 handle last and curframe surfaces which appear or disappear case
    CheckMergeDisplayDirtyBySurfaceChanged();
    // More: any other display dirty caused by surfaceNode should be added here like CheckMergeDisplayDirtByXXX
}

void RSUniRenderVisitor::CheckMergeDisplayDirtyByTransparentRegions(RSSurfaceRenderNode& surfaceNode) const
{
    const auto& dirtyRect = surfaceNode.GetDirtyManager()->GetCurrentFrameDirtyRegion();
    if (surfaceNode.HasContainerWindow()) {
        // If a surface's dirty is intersect with container region (which can be considered transparent)
        // should be added to display dirty region.
        // Note: we use containerRegion rather transparentRegion to bypass inner corner dirty problem.
        auto containerRegion = surfaceNode.GetContainerRegion();
        auto surfaceDirtyRegion = Occlusion::Region{ Occlusion::Rect{ dirtyRect } };
        auto containerDirtyRegion = containerRegion.And(surfaceDirtyRegion);
        if (!containerDirtyRegion.IsEmpty()) {
            RS_LOGD("CheckMergeDisplayDirtyByContainer global merge containerDirtyRegion "
                "%{public}s: global dirty %{public}s, add region %{public}s", surfaceNode.GetName().c_str(),
                curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
                containerDirtyRegion.GetRegionInfo().c_str());
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
        if (!surfaceNode.IsNodeDirty()) {
            return;
        }
        auto transparentRegion = surfaceNode.GetTransparentRegion();
        auto surfaceDirtyRegion = Occlusion::Region{ Occlusion::Rect{ dirtyRect } };
        Occlusion::Region transparentDirtyRegion = transparentRegion.And(surfaceDirtyRegion);
        if (!transparentDirtyRegion.IsEmpty()) {
            RS_LOGD("CheckMergeDisplayDirtyByTransparentRegions global merge TransparentDirtyRegion "
                "%{public}s: global dirty %{public}s, add region %{public}s", surfaceNode.GetName().c_str(),
                curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
                transparentDirtyRegion.GetRegionInfo().c_str());
            const std::vector<Occlusion::Rect>& rects = transparentDirtyRegion.GetRegionRects();
            for (const auto& rect : rects) {
                curDisplayNode_->GetDirtyManager()->MergeDirtyRect(RectI{ rect.left_, rect.top_,
                    rect.right_ - rect.left_, rect.bottom_ - rect.top_ });
            }
        }
    }
}

void RSUniRenderVisitor::CheckMergeDisplayDirtyByTransparentFilter(
    std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
    Occlusion::Region& accumulatedDirtyRegion)
{
    auto disappearedSurfaceRegionBelowCurrent =
        curDisplayNode_->GetDisappearedSurfaceRegionBelowCurrent(surfaceNode->GetId());
    accumulatedDirtyRegion.OrSelf(disappearedSurfaceRegionBelowCurrent);
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    auto filterVecIter = transparentCleanFilter_.find(surfaceNode->GetId());
    if (filterVecIter != transparentCleanFilter_.end()) {
        RS_LOGD("RSUniRenderVisitor::CheckMergeDisplayDirtyByTransparentFilter surface:%{public}s "
            "has transparentCleanFilter", surfaceNode->GetName().c_str());
        // check accumulatedDirtyRegion influence filter nodes which in the current surface
        for (auto it = filterVecIter->second.begin(); it != filterVecIter->second.end(); ++it) {
            auto filterNode = nodeMap.GetRenderNode(it->first);
            if (filterNode == nullptr) {
                continue;
            }
            auto filterRegion = Occlusion::Region{ Occlusion::Rect{ it->second } };
            auto filterDirtyRegion = filterRegion.And(accumulatedDirtyRegion);
            if (!filterDirtyRegion.IsEmpty()) {
                if (filterNode->GetRenderProperties().GetBackgroundFilter()) {
                    // backgroundfilter affected by below dirty
                    filterNode->MarkFilterStatusChanged(false, false);
                }
                RS_LOGD("RSUniRenderVisitor::CheckMergeDisplayDirtyByTransparentFilter global merge filterRegion "
                    "%{public}s: global dirty %{public}s, add rect %{public}s", surfaceNode->GetName().c_str(),
                    curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
                    it->second.ToString().c_str());
                curDisplayNode_->GetDirtyManager()->MergeDirtyRect(it->second);
                if (filterNode->GetRenderProperties().GetFilter()) {
                    // foregroundfilter affected by below dirty
                    filterNode->MarkFilterStatusChanged(true, false);
                }
            } else {
                globalFilter_.insert(*it);
            }
            filterNode->PostPrepareForBlurFilterNode(*(curDisplayNode_->GetDirtyManager()), needRequestNextVsync_);
        }
    }
    auto surfaceDirtyRegion = Occlusion::Region{
        Occlusion::Rect{ surfaceNode->GetDirtyManager()->GetCurrentFrameDirtyRegion() } };
    accumulatedDirtyRegion.OrSelf(surfaceDirtyRegion);
}

void RSUniRenderVisitor::UpdateDisplayDirtyAndExtendVisibleRegion()
{
    if (curDisplayNode_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::UpdateDisplayDirtyAndExtendVisibleRegion curDisplayNode_ is nullptr");
        return;
    }
    auto& curMainAndLeashSurfaces = curDisplayNode_->GetAllMainAndLeashSurfaces();
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    std::for_each(curMainAndLeashSurfaces.rbegin(), curMainAndLeashSurfaces.rend(),
        [this, &nodeMap](RSBaseRenderNode::SharedPtr& nodePtr) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodePtr);
        if (surfaceNode == nullptr) {
            RS_LOGE("RSUniRenderVisitor::UpdateDisplayDirtyAndExtendVisibleRegion surfaceNode is nullptr");
            return;
        }
        if (!surfaceNode->IsMainWindowType()) {
            return;
        }
        Occlusion::Region extendRegion;
        if (!surfaceNode->GetVisibleRegion().IsEmpty()) {
            ProcessFilterNodeObscured(surfaceNode, extendRegion, nodeMap);
        }
        surfaceNode->UpdateExtendVisibleRegion(extendRegion);
    });
}

void RSUniRenderVisitor::ProcessFilterNodeObscured(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
    Occlusion::Region& extendRegion, const RSRenderNodeMap& nodeMap)
{
    const auto& visibleFilterChild = surfaceNode->GetVisibleFilterChild();
    auto visibleRegion = surfaceNode->GetVisibleRegion();
    auto currentFrameDirtyRegion = surfaceNode->GetDirtyManager()->GetCurrentFrameDirtyRegion();
    auto isTransparent = surfaceNode->IsTransparent();
    for (const auto& child : visibleFilterChild) {
        auto& filterNode = nodeMap.GetRenderNode<RSRenderNode>(child);
        if (filterNode == nullptr || !filterNode->HasBlurFilter()) {
            continue;
        }
        MarkBlurIntersectWithDRM(filterNode);
        auto filterRect = filterNode->GetOldDirtyInSurface();
        auto visibleRects = visibleRegion.GetRegionRectIs();
        auto iter = std::find_if(visibleRects.begin(), visibleRects.end(), [&filterRect](const auto& rect) {
            return filterRect.IsInsideOf(rect);
        });
        if (iter != visibleRects.end()) {
            continue;
        }
        if (!visibleRegion.IsIntersectWith(filterRect)) {
            continue;
        }
        auto filterRegion = Occlusion::Region{ Occlusion::Rect{ filterRect } };
        extendRegion = extendRegion.Or(filterRegion);
        if (!isTransparent && filterRect.Intersect(currentFrameDirtyRegion)) {
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(filterRect);
        }
    }
}

void RSUniRenderVisitor::UpdateOccludedStatusWithFilterNode(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const
{
    if (surfaceNode == nullptr) {
        return;
    }
    if (surfaceNode->HasBlurFilter()) {
        surfaceNode->SetOccludedStatus(surfaceNode->IsOccludedByFilterCache());
    }
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (auto& child : surfaceNode->GetVisibleFilterChild()) {
        auto& filterNode = nodeMap.GetRenderNode<RSRenderNode>(child);
        if (filterNode == nullptr || !filterNode->HasBlurFilter()) {
            continue;
        }
        RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderVisitor::UpdateOccludedStatusWithFilterNode "
            "surfaceNode: %s, filterNode:[%lld], IsOccludedByFilterCache:%d", surfaceNode->GetName().c_str(),
            filterNode->GetId(), surfaceNode->IsOccludedByFilterCache());
        filterNode->SetOccludedStatus(surfaceNode->IsOccludedByFilterCache());
    }
}

void RSUniRenderVisitor::CheckMergeGlobalFilterForDisplay(Occlusion::Region& accumulatedDirtyRegion)
{
    // [planning] if not allowed containerNode filter, The following processing logic can be removed
    // Recursively traverses container nodes need filter
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (auto it = containerFilter_.begin(); it != containerFilter_.end(); ++it) {
        auto& filterNode = nodeMap.GetRenderNode<RSRenderNode>(it->first);
        if (filterNode == nullptr) {
            continue;
        }
        auto filterRegion = Occlusion::Region{ Occlusion::Rect{ it->second } };
        auto filterDirtyRegion = filterRegion.And(accumulatedDirtyRegion);
        RS_OPTIONAL_TRACE_NAME_FMT("CheckMergeGlobalFilterForDisplay::filternode:%" PRIu64
                                   ", filterRect:%s, dirtyRegion:%s",
            filterNode->GetId(), it->second.ToString().c_str(), accumulatedDirtyRegion.GetRegionInfo().c_str());
        if (!filterDirtyRegion.IsEmpty()) {
            RS_LOGD("RSUniRenderVisitor::CheckMergeGlobalFilterForDisplay global merge, "
                "global dirty %{public}s, add container filterRegion %{public}s",
                curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
                (it->second).ToString().c_str());
            if (filterNode->GetRenderProperties().GetBackgroundFilter()) {
                filterNode->MarkFilterStatusChanged(false, false); // background filter affected by below dirty
            }
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(it->second);
            if (filterNode->GetRenderProperties().GetFilter()) {
                filterNode->MarkFilterStatusChanged(true, false); // foreground filter affected by below dirty
            }
        } else {
            globalFilter_.insert(*it);
        }
        filterNode->UpdateFilterCacheWithSelfDirty();
        filterNode->PostPrepareForBlurFilterNode(*(curDisplayNode_->GetDirtyManager()), needRequestNextVsync_);
    }
    UpdateDisplayDirtyAndExtendVisibleRegion();
    CheckMergeFilterDirtyByIntersectWithDirty(globalFilter_, true);
}

void RSUniRenderVisitor::CollectEffectInfo(RSRenderNode& node)
{
    auto nodeParent = node.GetParent().lock();
    if (nodeParent == nullptr) {
        return;
    }
    if (node.GetRenderProperties().NeedFilter() || node.ChildHasVisibleFilter()) {
        nodeParent->SetChildHasVisibleFilter(true);
        nodeParent->UpdateVisibleFilterChild(node);
    }
    if ((node.GetRenderProperties().GetUseEffect() && node.ShouldPaint()) || node.ChildHasVisibleEffect()) {
        nodeParent->SetChildHasVisibleEffect(true);
        nodeParent->UpdateVisibleEffectChild(node);
    }
    if (node.GetSharedTransitionParam() || node.ChildHasSharedTransition()) {
        nodeParent->SetChildHasSharedTransition(true);
    }
}

void RSUniRenderVisitor::PostPrepare(RSRenderNode& node, bool subTreeSkipped)
{
    auto curDirtyManager = curSurfaceNode_ ? curSurfaceDirtyManager_ : curDisplayDirtyManager_;
    if (!curDirtyManager) {
        return;
    }
    auto isOccluded = curSurfaceNode_ ?
        curSurfaceNode_->IsMainWindowType() && curSurfaceNode_->GetVisibleRegion().IsEmpty() : false;
    if (subTreeSkipped && !isOccluded) {
        UpdateHwcNodeRectInSkippedSubTree(node);
        CheckFilterNodeInSkippedSubTreeNeedClearCache(node, *curDirtyManager);
        UpdateSubSurfaceNodeRectInSkippedSubTree(node);
    }
    if (node.GetRenderProperties().NeedFilter()) {
        UpdateHwcNodeEnableByFilterRect(
            curSurfaceNode_, node.GetOldDirtyInSurface(), NeedPrepareChindrenInReverseOrder(node));
        auto globalFilterRect = (node.IsInstanceOf<RSEffectRenderNode>() && !node.FirstFrameHasEffectChildren()) ?
            GetVisibleEffectDirty(node) : node.GetOldDirtyInSurface();
        auto globalHwcFilterRect = (node.IsInstanceOf<RSEffectRenderNode>() && !node.FirstFrameHasEffectChildren()) ?
            GetHwcVisibleEffectDirty(node, globalFilterRect) : node.GetOldDirtyInSurface();
        node.CalVisibleFilterRect(prepareClipRect_);
        node.MarkClearFilterCacheIfEffectChildrenChanged();
        CollectFilterInfoAndUpdateDirty(node, *curDirtyManager, globalFilterRect, globalHwcFilterRect);
        node.SetGlobalAlpha(curAlpha_);
    }
    CollectEffectInfo(node);
    node.MapAndUpdateChildrenRect();
    node.UpdateSubTreeInfo(prepareClipRect_);
    node.UpdateLocalDrawRect();
    node.UpdateAbsDrawRect();
    node.ResetChangeState();
    if (isDrawingCacheEnabled_) {
        node.UpdateDrawingCacheInfoAfterChildren();
    }
    if (auto nodeParent = node.GetParent().lock()) {
        nodeParent->UpdateChildUifirstSupportFlag(node.GetUifirstSupportFlag());
        nodeParent->OpincUpdateNodeSupportFlag(node.OpincGetNodeSupportFlag());
    }
    auto& stagingRenderParams = node.GetStagingRenderParams();
    if (stagingRenderParams != nullptr) {
        if (node.GetSharedTransitionParam() && node.GetRenderProperties().GetSandBox()) {
            stagingRenderParams->SetAlpha(curAlpha_);
        } else {
            stagingRenderParams->SetAlpha(node.GetRenderProperties().GetAlpha());
        }
    }

    // planning: only do this if node is dirty
    node.UpdateRenderParams();

    // add if node is dirty
    node.AddToPendingSyncList();
}

void RSUniRenderVisitor::MarkBlurIntersectWithDRM(std::shared_ptr<RSRenderNode> node) const
{
    if (!RSSystemProperties::GetDrmMarkedFilterEnabled()) {
        return;
    }
    static std::vector<std::string> drmKeyWins = { "SCBVolumePanel", "SCBBannerNotification" };
    auto appWindowNodeId = node->GetInstanceRootNodeId();
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    auto appWindowNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodeMap.GetRenderNode(appWindowNodeId));
    if (appWindowNode == nullptr) {
        return;
    }
    for (const auto& win : drmKeyWins) {
        if (appWindowNode->GetName().find(win) == std::string::npos) {
            continue;
        }
        for (auto& drmNode : drmNodes_) {
            auto drmNodePtr = drmNode.lock();
            if (drmNodePtr == nullptr) {
                continue;
            }
            bool isIntersect =
                drmNodePtr->GetRenderProperties().GetBoundsGeometry()->GetAbsRect().Intersect(node->GetFilterRegion());
            if (isIntersect) {
                node->MarkBlurIntersectWithDRM(true, RSMainThread::Instance()->GetGlobalDarkColorMode());
            }
        }
    }
}

void RSUniRenderVisitor::CheckFilterNodeInSkippedSubTreeNeedClearCache(
    const RSRenderNode& rootNode, RSDirtyRegionManager& dirtyManager)
{
    bool rotationChanged = curDisplayNode_ ?
        curDisplayNode_->IsRotationChanged() || curDisplayNode_->IsLastRotationChanged() : false;
    bool rotationStatusChanged = curDisplayNode_ ?
        curDisplayNode_->GetPreRotationStatus() != curDisplayNode_->GetCurRotationStatus() : false;
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (auto& child : rootNode.GetVisibleFilterChild()) {
        auto& filterNode = nodeMap.GetRenderNode<RSRenderNode>(child);
        if (filterNode == nullptr) {
            continue;
        }
        RS_OPTIONAL_TRACE_NAME_FMT("CheckFilterNodeInSkippedSubTreeNeedClearCache node[%lld]", filterNode->GetId());
        if (auto effectNode = RSRenderNode::ReinterpretCast<RSEffectRenderNode>(filterNode)) {
            UpdateRotationStatusForEffectNode(*effectNode);
        }
        filterNode->CheckBlurFilterCacheNeedForceClearOrSave(rotationChanged, rotationStatusChanged);
        filterNode->MarkClearFilterCacheIfEffectChildrenChanged();
        if (filterNode->GetRenderProperties().GetBackgroundFilter()) {
            filterNode->UpdateFilterCacheWithBelowDirty(dirtyManager, false);
        }
        RectI filterRect;
        filterNode->UpdateFilterRegionInSkippedSubTree(dirtyManager, rootNode, filterRect, prepareClipRect_);
        UpdateHwcNodeEnableByFilterRect(curSurfaceNode_, filterNode->GetOldDirtyInSurface());
        CollectFilterInfoAndUpdateDirty(*filterNode, dirtyManager, filterRect, filterRect);
    }
}

void RSUniRenderVisitor::UpdateHwcNodeRectInSkippedSubTree(const RSRenderNode& rootNode)
{
    if (RS_PROFILER_SHOULD_BLOCK_HWCNODE()) {
        return;
    }
    const auto& hwcNodes = RSMainThread::Instance()->GetSelfDrawingNodes();
    if (hwcNodes.empty()) {
        return;
    }
    for (auto hwcNodePtr : hwcNodes) {
        if (!hwcNodePtr || !hwcNodePtr->IsOnTheTree() || hwcNodePtr->GetCalcRectInPrepare()) {
            continue;
        }
        const auto& property = hwcNodePtr->GetRenderProperties();
        auto geoPtr = property.GetBoundsGeometry();
        if (geoPtr == nullptr) {
            return;
        }
        auto originalMatrix = geoPtr->GetMatrix();
        auto matrix = Drawing::Matrix();
        auto parent = hwcNodePtr->GetParent().lock();
        bool findInRoot = parent ? parent->GetId() == rootNode.GetId() : false;
        while (parent && parent->GetType() != RSRenderNodeType::DISPLAY_NODE) {
            if (auto opt = RSUniRenderUtil::GetMatrix(parent)) {
                matrix.PostConcat(opt.value());
            } else {
                break;
            }
            parent = parent->GetParent().lock();
            if (!parent) {
                break;
            }
            findInRoot = parent->GetId() == rootNode.GetId() ? true : findInRoot;
        }
        if (!findInRoot) {
            continue;
        }
        if (parent) {
            const auto& parentGeoPtr = parent->GetRenderProperties().GetBoundsGeometry();
            if (parentGeoPtr) {
                matrix.PostConcat(parentGeoPtr->GetMatrix());
            }
        }
        auto surfaceHandler = hwcNodePtr->GetMutableRSSurfaceHandler();
        auto& properties = hwcNodePtr->GetMutableRenderProperties();
        auto offset = std::nullopt;
        properties.UpdateGeometryByParent(&matrix, offset);
        matrix.PreConcat(originalMatrix);
        Drawing::Rect bounds = Drawing::Rect(0, 0, properties.GetBoundsWidth(), properties.GetBoundsHeight());
        Drawing::Rect absRect;
        matrix.MapRect(absRect, bounds);
        RectI rect = {std::round(absRect.left_), std::round(absRect.top_),
            std::round(absRect.GetWidth()), std::round(absRect.GetHeight())};
        UpdateDstRect(*hwcNodePtr, rect, prepareClipRect_);
        UpdateSrcRect(*hwcNodePtr, matrix, rect);
        UpdateHwcNodeByTransform(*hwcNodePtr);
        UpdateHwcNodeEnableBySrcRect(*hwcNodePtr);
        UpdateHwcNodeEnableByBufferSize(*hwcNodePtr);
        hwcNodePtr->SetTotalMatrix(matrix);
        hwcNodePtr->SetOldDirtyInSurface(geoPtr->MapRect(hwcNodePtr->GetSelfDrawRect(), matrix));
    }
}

void RSUniRenderVisitor::UpdateHardwareStateByHwcNodeBackgroundAlpha(
    const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& hwcNodes)
{
    std::list<RectI> hwcRects;
    for (size_t i = 0; i < hwcNodes.size(); i++) {
        auto hwcNodePtr = hwcNodes[i].lock();
        if (!hwcNodePtr) {
            continue;
        }
        if (!hwcNodePtr->IsNodeHasBackgroundColorAlpha() && !hwcNodePtr->IsHardwareForcedDisabled()) {
            hwcRects.push_back(hwcNodePtr->GetDstRect());
        } else if (hwcNodePtr->IsNodeHasBackgroundColorAlpha() && !hwcNodePtr->IsHardwareForcedDisabled() &&
            hwcRects.size() != 0 && IsNodeAboveInsideOfNodeBelow(hwcNodePtr->GetDstRect(), hwcRects)) {
            continue;
        } else {
            hwcNodePtr->SetHardwareForcedDisabledState(true);
        }
    }
}

bool RSUniRenderVisitor::IsNodeAboveInsideOfNodeBelow(const RectI& rectAbove, std::list<RectI>& hwcNodeRectList)
{
    for (auto rectBelow: hwcNodeRectList) {
        if (rectAbove.IsInsideOf(rectBelow)) {
            return true;
        }
    }
    return false;
}

void RSUniRenderVisitor::CalcHwcNodeEnableByFilterRect(
    std::shared_ptr<RSSurfaceRenderNode>& node, const RectI& filterRect, bool isReverseOrder)
{
    if (!node) {
        return;
    }
    auto& geoPtr = node->GetRenderProperties().GetBoundsGeometry();
    if (geoPtr == nullptr) {
        return;
    }
    auto bound = geoPtr->GetAbsRect();
    bool isIntersect = !bound.IntersectRect(filterRect).IsEmpty();
    if (isIntersect) {
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by filter rect",
            node->GetName().c_str(), node->GetId());
        node->SetIsHwcPendingDisabled(true);
        node->SetHardwareForcedDisabledState(true);
        node->SetHardWareDisabledByReverse(isReverseOrder);
        hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(node->GetId(),
            HwcDisabledReasons::DISABLED_BY_FLITER_RECT, node->GetName());
    }
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByFilterRect(
    std::shared_ptr<RSSurfaceRenderNode>& node, const RectI& filterRect, bool isReverseOrder)
{
    if (filterRect.IsEmpty()) {
        return;
    }
    if (!node) {
        const auto& selfDrawingNodes = RSMainThread::Instance()->GetSelfDrawingNodes();
        if (selfDrawingNodes.empty()) {
            return;
        }
        for (auto hwcNode : selfDrawingNodes) {
            CalcHwcNodeEnableByFilterRect(hwcNode, filterRect, isReverseOrder);
        }
    } else {
        const auto& hwcNodes = node->GetChildHardwareEnabledNodes();
        if (hwcNodes.empty()) {
            return;
        }
        for (auto hwcNode : hwcNodes) {
            auto hwcNodePtr = hwcNode.lock();
            CalcHwcNodeEnableByFilterRect(hwcNodePtr, filterRect, isReverseOrder);
        }
    }
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByGlobalFilter(std::shared_ptr<RSSurfaceRenderNode>& node)
{
    auto cleanFilter = transparentHwcCleanFilter_.find(node->GetId());
    bool cleanFilterFound = (cleanFilter != transparentHwcCleanFilter_.end());
    auto dirtyFilter = transparentHwcDirtyFilter_.find(node->GetId());
    bool dirtyFilterFound = (dirtyFilter != transparentHwcDirtyFilter_.end());
    auto& curMainAndLeashSurfaces = curDisplayNode_->GetAllMainAndLeashSurfaces();
    for (auto it = curMainAndLeashSurfaces.rbegin(); it != curMainAndLeashSurfaces.rend(); ++it) {
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
        for (auto hwcNode : hwcNodes) {
            auto hwcNodePtr = hwcNode.lock();
            if (!hwcNodePtr || hwcNodePtr->IsHardwareForcedDisabled() ||
                !hwcNodePtr->GetRenderProperties().GetBoundsGeometry()) {
                continue;
            }
            if (cleanFilterFound) {
                UpdateHwcNodeEnableByGlobalCleanFilter(cleanFilter->second, *hwcNodePtr);
                if (hwcNodePtr->IsHardwareForcedDisabled()) {
                    ProcessAncoNode(hwcNodePtr);
                    continue;
                }
            }
            if (!dirtyFilterFound) {
                continue;
            }
            for (auto filter = dirtyFilter->second.begin(); filter != dirtyFilter->second.end(); ++filter) {
                if (hwcNodePtr->GetRenderProperties().GetBoundsGeometry()->GetAbsRect().Intersect(filter->second)) {
                    RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by transparentDirtyFilter",
                        hwcNodePtr->GetName().c_str(), hwcNodePtr->GetId());
                    hwcNodePtr->SetHardwareForcedDisabledState(true);
                    ProcessAncoNode(hwcNodePtr);
                    hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(hwcNodePtr->GetId(),
                        HwcDisabledReasons::DISABLED_BY_TRANSPARENT_DIRTY_FLITER, hwcNodePtr->GetName());
                    break;
                }
            }
        }
    }
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByGlobalCleanFilter(
    const std::vector<std::pair<NodeId, RectI>>& cleanFilter, RSSurfaceRenderNode& hwcNodePtr)
{
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    bool intersectedWithAIBar = false;
    bool checkDrawAIBar = false;
    for (auto filter = cleanFilter.begin(); filter != cleanFilter.end(); ++filter) {
        auto geo = hwcNodePtr.GetRenderProperties().GetBoundsGeometry();
        if (!geo) {
            return;
        }
        if (!geo->GetAbsRect().IntersectRect(filter->second).IsEmpty()) {
            auto& rendernode = nodeMap.GetRenderNode<RSRenderNode>(filter->first);
            if (rendernode == nullptr) {
                ROSEN_LOGD("RSUniRenderVisitor::UpdateHwcNodeByFilter: rendernode is null");
                continue;
            }

            if (rendernode->IsAIBarFilter()) {
                intersectedWithAIBar = true;
                if (rendernode->IsAIBarFilterCacheValid()) {
                    ROSEN_LOGD("RSUniRenderVisitor::UpdateHwcNodeByFilter: skip intersection for using cache");
                    continue;
                } else if (RSSystemProperties::GetHveFilterEnabled()) {
                    checkDrawAIBar = true;
                    continue;
                }
            }

            RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by transparentCleanFilter",
                hwcNodePtr.GetName().c_str(), hwcNodePtr.GetId());
            hwcNodePtr.SetHardwareForcedDisabledState(true);
            hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(hwcNodePtr.GetId(),
                HwcDisabledReasons::DISABLED_BY_TRANSPARENT_CLEAN_FLITER, hwcNodePtr.GetName());
            break;
        }
    }
    RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64", checkDrawAIBar:%d, intersectedWithAIBar:%d",
        hwcNodePtr.GetName().c_str(), hwcNodePtr.GetId(), checkDrawAIBar, intersectedWithAIBar);
    if (checkDrawAIBar) {
        hwcNodePtr.SetHardwareNeedMakeImage(checkDrawAIBar);
    }
    if (intersectedWithAIBar) {
        hwcNodePtr.SetIntersectWithAIBar(intersectedWithAIBar);
    }
}

inline static void ResetSubSurfaceNodesCalState(
    std::vector<std::pair<NodeId, std::weak_ptr<RSSurfaceRenderNode>>>& subSurfaceNodes)
{
    for (auto& [id, node] : subSurfaceNodes) {
        auto subSurfaceNodePtr = node.lock();
        if (!subSurfaceNodePtr) {
            continue;
        }
        subSurfaceNodePtr->SetCalcRectInPrepare(false);
    }
}

void RSUniRenderVisitor::UpdateSubSurfaceNodeRectInSkippedSubTree(const RSRenderNode& rootNode)
{
    if (!curSurfaceNode_ || !curSurfaceDirtyManager_) {
        return;
    }
    auto rootGeo = rootNode.GetRenderProperties().GetBoundsGeometry();
    if (!rootGeo) {
        return;
    }

    std::vector<std::pair<NodeId, std::weak_ptr<RSSurfaceRenderNode>>> allSubSurfaceNodes;
    curSurfaceNode_->GetAllSubSurfaceNodes(allSubSurfaceNodes);
    for (auto& [_, subSurfaceNode] : allSubSurfaceNodes) {
        auto subSurfaceNodePtr = subSurfaceNode.lock();
        Drawing::Matrix absMatrix;
        if (!subSurfaceNodePtr || subSurfaceNodePtr->GetCalcRectInPrepare() ||
            !subSurfaceNodePtr->GetAbsMatrixReverse(rootNode, absMatrix)) {
            continue;
        }

        Drawing::RectF absDrawRect;
        absMatrix.MapRect(absDrawRect, RSPropertiesPainter::Rect2DrawingRect(subSurfaceNodePtr->GetSelfDrawRect()));
        RectI subSurfaceRect = RectI(absDrawRect.GetLeft(), absDrawRect.GetTop(),
            absDrawRect.GetWidth(), absDrawRect.GetHeight());

        subSurfaceNodePtr->SetOldDirtyInSurface(subSurfaceRect.IntersectRect(prepareClipRect_));
        UpdateNodeVisibleRegion(*subSurfaceNodePtr);
        UpdateDstRect(*subSurfaceNodePtr, subSurfaceRect, prepareClipRect_);
        subSurfaceNodePtr->SetCalcRectInPrepare(true);
        if (subSurfaceNodePtr->IsLeashOrMainWindow()) {
            curMainAndLeashWindowNodesIds_.push(subSurfaceNodePtr->GetId());
            curDisplayNode_->RecordMainAndLeashSurfaces(subSurfaceNodePtr);
            curDisplayNode_->UpdateSurfaceNodePos(
                subSurfaceNodePtr->GetId(), subSurfaceNodePtr->GetOldDirtyInSurface());
            if (auto subSurfaceDirtyManager = subSurfaceNodePtr->GetDirtyManager()) {
                subSurfaceDirtyManager->MergeDirtyRect(subSurfaceNodePtr->GetOldDirtyInSurface().IntersectRect(
                    curSurfaceDirtyManager_->GetCurrentFrameDirtyRegion()));
            }
            CollectOcclusionInfoForWMS(*subSurfaceNodePtr);
            subSurfaceNodePtr->UpdateRenderParams();
        }
    }
    ResetSubSurfaceNodesCalState(allSubSurfaceNodes);
}

RectI RSUniRenderVisitor::GetVisibleEffectDirty(RSRenderNode& node) const
{
    RectI childEffectRect;
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (auto& nodeId : node.GetVisibleEffectChild()) {
        if (auto& subnode = nodeMap.GetRenderNode<RSRenderNode>(nodeId)) {
            childEffectRect = childEffectRect.JoinRect(subnode->GetOldDirtyInSurface());
        }
    }
    return childEffectRect;
}

RectI RSUniRenderVisitor::GetHwcVisibleEffectDirty(RSRenderNode& node, const RectI globalFilterRect) const
{
    RectI childEffectRect;
    if (!globalFilterRect.IsEmpty()) {
        childEffectRect = globalFilterRect.JoinRect(node.GetFilterRect());
    }
    return childEffectRect;
}

void RSUniRenderVisitor::CollectFilterInfoAndUpdateDirty(RSRenderNode& node,
    RSDirtyRegionManager& dirtyManager, const RectI& globalFilterRect, const RectI& globalHwcFilterRect)
{
    bool isNodeAddedToTransparentCleanFilters = false;
    if (curSurfaceNode_) {
        bool isIntersect = dirtyManager.GetCurrentFrameDirtyRegion().Intersect(globalFilterRect);
        if (isIntersect) {
            dirtyManager.MergeDirtyRect(globalFilterRect);
        } else {
            curSurfaceNoBelowDirtyFilter_.insert({node.GetId(), globalFilterRect});
        }
        if (node.GetRenderProperties().GetFilter()) {
            node.UpdateFilterCacheWithBelowDirty(dirtyManager, true);
        }
        node.UpdateFilterCacheWithSelfDirty();
        if (curSurfaceNode_->IsTransparent()) {
            globalFilterRects_.emplace_back(globalFilterRect);
            if (!isIntersect || (isIntersect && node.GetRenderProperties().GetBackgroundFilter() &&
                !node.IsBackgroundInAppOrNodeSelfDirty())) {
                // record nodes which has transparent clean filter
                RS_OPTIONAL_TRACE_NAME_FMT("CollectFilterInfoAndUpdateDirty::surfaceNode:%s, add node[%lld] to "
                    "transparentCleanFilter", curSurfaceNode_->GetName().c_str(), node.GetId());
                transparentCleanFilter_[curSurfaceNode_->GetId()].push_back({node.GetId(), globalFilterRect});
                transparentHwcCleanFilter_[curSurfaceNode_->GetId()].push_back({node.GetId(), globalHwcFilterRect});
                isNodeAddedToTransparentCleanFilters = true;
            }
            if (isIntersect) {
                transparentDirtyFilter_[curSurfaceNode_->GetId()].push_back({node.GetId(), globalFilterRect});
                transparentHwcDirtyFilter_[curSurfaceNode_->GetId()].push_back({node.GetId(), globalHwcFilterRect});
                RS_LOGD("RSUniRenderVisitor::CollectFilterInfoAndUpdateDirty global merge transparentDirtyFilter "
                    "%{public}s, global dirty %{public}s, add rect %{public}s", curSurfaceNode_->GetName().c_str(),
                    curDisplayDirtyManager_->GetCurrentFrameDirtyRegion().ToString().c_str(),
                    globalFilterRect.ToString().c_str());
                curDisplayDirtyManager_->MergeDirtyRect(globalFilterRect);
            }
        } else {
            // record surface nodes and nodes in surface which has clean filter
            globalFilter_.insert({node.GetId(), globalFilterRect});
        }
    } else {
        globalFilterRects_.emplace_back(globalFilterRect);
        // record container nodes which need filter
        containerFilter_.insert({node.GetId(), globalFilterRect});
    }
    if (curSurfaceNode_ && !isNodeAddedToTransparentCleanFilters) {
        node.PostPrepareForBlurFilterNode(dirtyManager, needRequestNextVsync_);
    }
}

void RSUniRenderVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    auto nodeId = node.GetId();
    RS_OPTIONAL_TRACE_NAME("RSUniRender::Prepare:[" + node.GetName() + "] nodeid: " +
        std::to_string(nodeId) +  " pid: " + std::to_string(ExtractPid(nodeId)) +
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
        node.SetContentDirty(); // screen recording capsule force mark dirty
    }

    node.SetAncestorDisplayNode(curDisplayNode_);
    node.UpdateAncestorDisplayNodeInRenderParams();
    CheckColorSpace(node);
    CheckPixelFormat(node);
    // only need collect first level node's security & skip layer info
    if (nodeId == node.GetFirstLevelNodeId()) {
        UpdateSecuritySkipAndProtectedLayersRecord(node);
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
    auto& geoPtr = (property.GetBoundsGeometry());
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

    if (node.GetRSSurfaceHandler()->GetBuffer() != nullptr) {
        node.SetBufferRelMatrix(RSUniRenderUtil::GetMatrixOfBufferToRelRect(node));
    }

    auto skipNodeMap = RSMainThread::Instance()->GetCacheCmdSkippedNodes();
    // Update node properties, including position (dstrect), OldDirty()
    auto parentNode = node.GetParent().lock();
    auto rsParent = (parentNode);
    if (skipNodeMap.count(nodeId) != 0) {
        dirtyFlag_ = node.Update(*curSurfaceDirtyManager_, rsParent, dirtyFlag_, prepareClipRect_);
        dirtyFlag_ = dirtyFlag;
        RS_TRACE_NAME(node.GetName() + " PreparedNodes cacheCmdSkiped");
        return;
    }

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

    if (node.IsLeashOrMainWindow()) {
        // record visible node position for display render node dirtyManager
        if (node.ShouldPaint()) {
            curDisplayNode_->UpdateSurfaceNodePos(nodeId, node.GetOldDirty());
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
    if (RSUniRenderUtil::GetRotationDegreeFromMatrix(parentSurfaceNodeMatrix_) % RS_ROTATION_90 != 0) {
        isSurfaceRotationChanged_ = true;
        doAnimate_ = true;
        node.SetAnimateState();
    }

    bool isSubNodeOfSurfaceInPrepare = isSubNodeOfSurfaceInPrepare_;
    if (node.IsLeashOrMainWindow()) {
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
    if (node.IsLeashOrMainWindow()) {
        isSubNodeOfSurfaceInPrepare_ = isSubNodeOfSurfaceInPrepare;
    }

    PrepareTypesOfSurfaceRenderNodeAfterUpdate(node);
    if (node.GetDstRectChanged() || (node.GetDirtyManager() && node.GetDirtyManager()->IsCurrentFrameDirty())) {
        curDisplayNode_->GetDirtySurfaceNodeMap().emplace(nodeId, node.ReinterpretCastTo<RSSurfaceRenderNode>());
    }
    UpdateSurfaceRenderNodeScale(node);
    // Due to the alpha is updated in PrepareChildren, so PrepareChildren
    // needs to be done before CheckOpaqueRegionBaseInfo
    auto screenRotation = curDisplayNode_->GetRotation();
    auto screenRect = RectI(0, 0, screenInfo_.width, screenInfo_.height);
    Vector4f cornerRadius;
    Vector4f::Max(node.GetWindowCornerRadius(), node.GetGlobalCornerRadius(), cornerRadius);
    Vector4<int> dstCornerRadius(static_cast<int>(std::round(cornerRadius.x_)),
                                 static_cast<int>(std::round(cornerRadius.y_)),
                                 static_cast<int>(std::round(cornerRadius.z_)),
                                 static_cast<int>(std::round(cornerRadius.w_)));
    auto parent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.GetParent().lock());
    auto isFocused = node.IsFocusedNode(currentFocusedNodeId_) ||
        (parent && parent->IsLeashWindow() && parent->IsFocusedNode(focusedLeashWindowId_));
    auto absRect = node.GetDstRect().IntersectRect(node.GetOldDirtyInSurface());
    if (!node.CheckOpaqueRegionBaseInfo(
        screenRect, absRect, screenRotation, isFocused, dstCornerRadius)
        && node.GetSurfaceNodeType() != RSSurfaceNodeType::SELF_DRAWING_NODE) {
        node.ResetSurfaceOpaqueRegion(screenRect, absRect, screenRotation, isFocused, dstCornerRadius);
    }
    node.SetOpaqueRegionBaseInfo(screenRect, absRect, screenRotation, isFocused, dstCornerRadius);
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
    auto& geoPtr = (property.GetBoundsGeometry());
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
            const auto& dstRect = node.GetDstRect();
            float dstRectWidth = dstRect.GetWidth();
            float dstRectHeight = dstRect.GetHeight();
            float boundsWidth = property.GetBoundsWidth();
            float boundsHeight = property.GetBoundsHeight();
            isScale =
                !ROSEN_EQ(std::min(dstRectWidth, dstRectHeight), std::min(boundsWidth, boundsHeight), EPSILON_SCALE) ||
                !ROSEN_EQ(std::max(dstRectWidth, dstRectHeight), std::max(boundsWidth, boundsHeight), EPSILON_SCALE);
        }
    }
    node.SetIsScaleInPreFrame(node.IsScale());
    node.SetIsScale(isScale);
}

void RSUniRenderVisitor::PrepareProxyRenderNode(RSProxyRenderNode& node)
{
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
    auto& geoPtr = (property.GetBoundsGeometry());

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
    RS_TRACE_NAME_FMT("RSUniRender::PrepareRootRenderNode:node[%" PRIu64 "] pid[%d] subTreeDirty[%d]",
        node.GetId(), ExtractPid(node.GetId()), node.IsSubTreeDirty());
    bool dirtyFlag = dirtyFlag_;
    auto parentSurfaceNodeMatrix = parentSurfaceNodeMatrix_;
    auto prepareClipRect = prepareClipRect_;

    auto nodeParent = (node.GetParent().lock());
    const auto& property = node.GetRenderProperties();
    bool geoDirty = property.IsGeoDirty();
    auto& geoPtr = (property.GetBoundsGeometry());
    auto prevAlpha = curAlpha_;
    curAlpha_ *= std::clamp(node.GetRenderProperties().GetAlpha(), 0.f, 1.f);

    if (curSurfaceDirtyManager_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::PrepareRootRenderNode curSurfaceDirtyManager is nullptr");
        return;
    }

    if (RSSystemProperties::GetQuickPrepareEnabled()) {
        dirtyFlag_ = node.UpdateDrawRectAndDirtyRegion(
            *curSurfaceDirtyManager_, dirtyFlag_, prepareClipRect_, parentSurfaceNodeMatrix_);
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
        bool isSubTreeNeedPrepare = node.IsSubTreeNeedPrepare(filterInGlobal_) || ForcePrepareSubTree();
        isSubTreeNeedPrepare ? QuickPrepareChildren(node) :
            node.SubTreeSkipPrepare(*curSurfaceDirtyManager_, curDirty_, dirtyFlag_, prepareClipRect_);
        PostPrepare(node, !isSubTreeNeedPrepare);
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
    if (LIKELY(nodeParent)) {
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
    // if canvasNode is not sub node of surfaceNode, merge the dirtyRegion to curDisplayDirtyManager_
    auto& dirtyManager = isSubNodeOfSurfaceInPrepare_ ? curSurfaceDirtyManager_ : curDisplayDirtyManager_;
    dirtyFlag_ = node.Update(*dirtyManager, nodeParent, dirtyFlag_, prepareClipRect_);

    const auto& property = node.GetRenderProperties();
    auto& geoPtr = (property.GetBoundsGeometry());
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
        if (curSurfaceDirtyManager_ && curSurfaceDirtyManager_->IsTargetForDfx()) {
            curSurfaceDirtyManager_->UpdateDirtyRegionInfoForDfx(node.GetId(), RSRenderNodeType::CANVAS_NODE,
                DirtyRegionType::FILTER_RECT, node.GetOldDirtyInSurface());
        }
        if (curSurfaceNode_) {
            curSurfaceNode_->UpdateChildrenFilterRects(node.shared_from_this(), node.GetOldDirtyInSurface(),
                node.IsFilterCacheValid());
            curSurfaceNode_->UpdateFilterNodes(node.shared_from_this());
        }
        UpdateForegroundFilterCacheWithDirty(node, *dirtyManager);
    }
    dirtyFlag_ = dirtyFlag;
    prepareClipRect_ = prepareClipRect;
#ifdef RS_ENABLE_STACK_CULLING
    if (RSSystemProperties::GetViewOcclusionCullingEnabled()) {
        node.SetFullSurfaceOpaqueMarks(curSurfaceNode_);
    }
#endif
}

void RSUniRenderVisitor::PrepareEffectRenderNode(RSEffectRenderNode& node)
{
    bool dirtyFlag = dirtyFlag_;
    RectI prepareClipRect = prepareClipRect_;
    auto effectRegion = effectRegion_;
    effectRegion_ = node.InitializeEffectRegion();

    UpdateRotationStatusForEffectNode(node);
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
                node.IsFilterCacheValid());
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
    std::string position = std::to_string(dirtyRect.left_) + ',' + std::to_string(dirtyRect.top_) + ',' +
        std::to_string(dirtyRect.width_) + ',' + std::to_string(dirtyRect.height_) + extra;
    const int defaultTextOffsetX = edgeWidth;
    const int defaultTextOffsetY = 30; // text position has 30 pixelSize under the Rect
    Drawing::Pen rectPen;
    Drawing::Brush rectBrush;
    std::shared_ptr<Drawing::Typeface> typeFace = nullptr;
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
        DrawDirtyRectForDFX(subRect, Drawing::Color::COLOR_BLUE, RSPaintStyle::FILL, fillAlpha);
    }
}

void RSUniRenderVisitor::DrawCacheRegionForDFX(std::map<NodeId, RectI>& cacheRects)
{
    for (const auto& [nodeId, subRect] : cacheRects) {
        auto iter = cacheRenderNodeIsUpdateMap_.find(nodeId);
        if ((iter != cacheRenderNodeIsUpdateMap_.end()) && (iter->second)) {
            DrawDirtyRectForDFX(subRect, Drawing::Color::COLOR_RED, RSPaintStyle::FILL, CACHE_UPDATE_FILL_ALPHA);
        } else {
            DrawDirtyRectForDFX(subRect, Drawing::Color::COLOR_BLUE, RSPaintStyle::FILL, CACHE_FILL_ALPHA);
        }
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
        if (surfaceNode && surfaceNode->IsMainWindowType()) {
            DrawSurfaceOpaqueRegionForDFX(*surfaceNode);
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
        auto& geoPtr = node->GetRenderProperties().GetBoundsGeometry();
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

void RSUniRenderVisitor::DrawCurrentRefreshRate(
    uint32_t currentRefreshRate, uint32_t realtimeRefreshRate, RSDisplayRenderNode& node)
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
    auto rotation = node.GetScreenRotation();
    if (RSSystemProperties::IsFoldScreenFlag() && node.GetScreenId() == 0) {
        if (rotation == ScreenRotation::ROTATION_270) {
            rotation = ScreenRotation::ROTATION_0;
        } else {
            rotation = static_cast<ScreenRotation>(static_cast<int>(rotation) + 1);
        }
    }
    if (rotation != ScreenRotation::ROTATION_0) {
        auto screenManager = CreateOrGetScreenManager();
        auto mainScreenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
        if (rotation == ScreenRotation::ROTATION_90) {
            canvas_->Rotate(-RS_ROTATION_90, 0, 0);
            canvas_->Translate(-(static_cast<float>(mainScreenInfo.height)), 0);
        } else if (rotation == ScreenRotation::ROTATION_180) {
            canvas_->Rotate(-RS_ROTATION_180, static_cast<float>(mainScreenInfo.width) / 2,  // half of screen width
                static_cast<float>(mainScreenInfo.height) / 2);                           // half of screen height
        } else if (rotation == ScreenRotation::ROTATION_270) {
            canvas_->Rotate(-RS_ROTATION_270, 0, 0);
            canvas_->Translate(0, -(static_cast<float>(mainScreenInfo.width)));
        } else {
            return;
        }
    }
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
    RS_LOGE("It is update to DrawableV2 to process node now!!");
}

void RSUniRenderVisitor::ProcessChildrenForScreenRecordingOptimization(
    RSDisplayRenderNode& node, NodeId rootIdOfCaptureWindow)
{
    RS_LOGE("It is update to DrawableV2 to process node now!!");
}

void RSUniRenderVisitor::ProcessChildInner(RSRenderNode& node, const RSRenderNode::SharedPtr child)
{
    RS_LOGE("It is update to DrawableV2 to process node now!!");
}

void RSUniRenderVisitor::UpdateVirtualScreenWhiteListRootId(const RSRenderNode::SharedPtr& node)
{
    if (node->GetType() == RSRenderNodeType::SURFACE_NODE && virtualScreenWhiteListRootId_ == INVALID_NODEID &&
        screenInfo_.whiteList.find(node->GetId()) != screenInfo_.whiteList.end()) {
        // limit surface node is to reduce whiteList set times
        // don't update if node's parent is in whiteList
        virtualScreenWhiteListRootId_ = node->GetId();
    } else if (virtualScreenWhiteListRootId_ == node->GetId()) {
        // restore virtualScreenWhiteListRootId_ only by itself
        virtualScreenWhiteListRootId_ = INVALID_NODEID;
    }
}

void RSUniRenderVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode& node)
{
    RS_LOGE("RSUniRenderVisitor::%{public}s is upgraded to DrawableV2", __func__);
}

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
        auto& childHardwareEnabledNodes =
            const_cast<std::vector<std::weak_ptr<RSSurfaceRenderNode>>&>(appWindowNode->GetChildHardwareEnabledNodes());
        for (auto iter = childHardwareEnabledNodes.begin(); iter != childHardwareEnabledNodes.end();) {
            auto childNode = iter->lock();
            if (!childNode || !childNode->IsOnTheTree()) {
                iter = childHardwareEnabledNodes.erase(iter);
                continue;
            }
            auto surfaceHandler = childNode->GetMutableRSSurfaceHandler();
            if (surfaceHandler->GetBuffer() != nullptr &&
                (!childNode->IsHardwareForcedDisabled() || childNode->GetProtectedLayer())) {
                // SetGlobalZOrder here to ensure zOrder committed to composer is continuous
                surfaceHandler->SetGlobalZOrder(globalZOrder_++);
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
    if (renderFrame == nullptr) {
        RS_LOGE("RSUniRenderVisitor::AddOverDrawListener: renderFrame is nullptr");
        return;
    }
#if defined(NEW_RENDER_CONTEXT)
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

void RSUniRenderVisitor::ClipRegion(std::shared_ptr<Drawing::Canvas> canvas, const Drawing::Region& region) const
{
    if (region.IsEmpty()) {
        // [planning] Remove this after frame buffer can cancel
        canvas->ClipRect(Drawing::Rect());
    } else if (region.IsRect()) {
        canvas->ClipRegion(region);
    } else {
        RS_TRACE_NAME("RSUniRenderVisitor: clipPath");
#ifdef RS_ENABLE_VK
        if (RSSystemProperties::IsUseVulkan()) {
            canvas->ClipRegion(region);
        } else {
            Drawing::Path dirtyPath;
            region.GetBoundaryPath(&dirtyPath);
            canvas->ClipPath(dirtyPath, Drawing::ClipOp::INTERSECT, true);
        }
#else
        Drawing::Path dirtyPath;
        region.GetBoundaryPath(&dirtyPath);
        canvas->ClipPath(dirtyPath, Drawing::ClipOp::INTERSECT, true);
#endif
    }
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
            curDisplayNode_->GetDirtySurfaceNodeMap().emplace(
                surfaceNode->GetId(), surfaceNode->ReinterpretCastTo<RSSurfaceRenderNode>());
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

        bool isShadowDisappear =
            !surfaceNode->GetRenderProperties().IsShadowValid() && surfaceNode->IsShadowValidLastFrame();
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
        displayDirtyManager->MergeDirtyRect(tempRect, true);  // true：debugRect for dislplayNode skip
    }

#ifdef RS_PROFILER_ENABLED
    RS_LOGD("CalcDirtyRegion RSSystemProperties::GetFullDirtyScreenEnabled()");
    auto resolution = RSCaptureRecorder::GetInstance().GetDirtyRect(screenInfo_.width, screenInfo_.height);
    displayDirtyManager->MergeDirtyRect(RectI { 0, 0, resolution.first, resolution.second });
#endif
}

void RSUniRenderVisitor::MergeDirtyRectIfNeed(std::shared_ptr<RSSurfaceRenderNode> appNode,
    std::shared_ptr<RSSurfaceRenderNode> hwcNode)
{
    if ((hwcNode->IsLastFrameHardwareEnabled() || hwcNode->GetRSSurfaceHandler()->IsCurrentFrameBufferConsumed()) &&
        appNode && appNode->GetDirtyManager()) {
        appNode->GetDirtyManager()->MergeDirtyRect(hwcNode->GetDstRect());
        curDisplayNode_->GetDirtySurfaceNodeMap().emplace(appNode->GetId(), appNode);
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

void RSUniRenderVisitor::UpdateHardwareChildNodeStatus(std::shared_ptr<RSSurfaceRenderNode>& node,
    std::vector<SurfaceDirtyMgrPair>& curHwcEnabledNodes)
{
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
        if (!isPhone_ || childNode->GetAncoForceDoDirect()) {
            curHwcEnabledNodes.emplace_back(std::make_pair(subNode, node));
            continue;
        }
        for (auto &hwcNode: curHwcEnabledNodes) {
            if (childNode->GetDstRect().Intersect(hwcNode.first->GetDstRect())) {
                childNode->SetHardwareForcedDisabledStateByFilter(true);
                isIntersected = true;
                break;
            }
        }
        if (!isIntersected) {
            curHwcEnabledNodes.emplace_back(std::make_pair(subNode, node));
        }
    }
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
    auto filterRects = node->GetChildrenNeedFilterRectsWithoutCacheValid();
    // collect valid hwc surface which is not intersected with filterRects
    std::vector<SurfaceDirtyMgrPair> curHwcEnabledNodes;
    UpdateHardwareChildNodeStatus(node, curHwcEnabledNodes);

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
            auto filterRectsCacheValidNow = filterNodes[i]->IsFilterCacheValid();
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
        RSRenderNode::ClearCacheSurfaceFunc func = &RSUniRenderUtil::ClearNodeCacheSurface;
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
        cacheCanvas->CopyConfigurationToOffscreenCanvas(*canvas_);
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
    const auto& property = node.GetRenderProperties();
    if (cacheType == CacheType::ANIMATE_PROPERTY) {
        if (property.IsShadowValid()
            && !property.IsSpherizeValid()) {
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

    node.ProcessRenderContents(*canvas_);
    // Set a count to label the ProcessChildren in updateCacheProcess
    updateCacheProcessCnt_++;
    ProcessChildren(node);
    updateCacheProcessCnt_--;

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
    const auto& property = node.GetRenderProperties();
    if (curCacheFilterRects_.top().count(node.GetId())) {
        if (curGroupedNodes_.empty()) {
            // draw filter before drawing cached surface
            curCacheFilterRects_.top().erase(node.GetId());
            if (curCacheFilterRects_.empty() || !node.ChildHasVisibleFilter()) {
                // no filter to draw, return
                return true;
            }
        } else if (property.GetShadowColorStrategy() !=
            SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE) {
            // clear hole while generating cache surface
            Drawing::AutoCanvasRestore arc(*canvas_.get(), true);
            RectI shadowRect;
            auto rrect = property.GetRRect();
            RSPropertiesPainter::GetShadowDirtyRect(shadowRect, property, &rrect, false, false);
            std::shared_ptr<Drawing::CoreCanvasImpl> coreCanvas = canvas_->GetCanvasData();
            auto skiaCanvas = static_cast<Drawing::SkiaCanvas *>(coreCanvas.get());
            SkCanvasPriv::ResetClip(skiaCanvas->ExportSkCanvas());
            canvas_->ClipRect(Drawing::Rect(shadowRect.left_, shadowRect.top_,
                shadowRect.width_ + shadowRect.left_, shadowRect.height_ + shadowRect.top_));
            canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
        } else if (property.GetBackgroundFilter() || property.GetUseEffect()) {
            // clear hole while generating cache surface
            Drawing::AutoCanvasRestore arc(*canvas_, true);
            if (property.GetClipBounds() != nullptr) {
                canvas_->ClipRect(RSPropertiesPainter::Rect2DrawingRect(property.GetBoundsRect()),
                    Drawing::ClipOp::INTERSECT, false);
            } else {
                canvas_->ClipRoundRect(RSPropertiesPainter::RRect2DrawingRRect(property.GetRRect()),
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
            goto process;
        } else if (node.IsContentNode()) {
            node.ApplyBoundsGeometry(*canvas_);
            node.ApplyAlpha(*canvas_);
            node.ProcessRenderContents(*canvas_);
            process:
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
            cacheRenderNodeMapRects_[node.GetId()] = node.GetOldDirtyInSurface();
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
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> appNodes;
    if (node.LeashWindowRelatedAppWindowOccluded(appNodes) && !isSecurityDisplay_) {
        for (const auto& appNode : appNodes) {
            if (appNode) {
                MarkSubHardwareEnableNodeState(*appNode);
            }
        }
        RS_PROCESS_TRACE(isPhone_, node.GetName() + " App Occluded Leashwindow Skip");
        return false;
    }
    if (!screenInfo_.whiteList.empty() && virtualScreenWhiteListRootId_ == INVALID_NODEID) {
        RS_PROCESS_TRACE(isPhone_, node.GetName() + " skip because it isn't filtered App");
        RS_LOGD("RSUniRenderVisitor::CheckIfSurfaceRenderNodeNeedProcess:\
            %{public}s skip because it isn't filtered App", node.GetName().c_str());
        return false;
    }
    return true;
}

bool RSUniRenderVisitor::ForceHardwareComposer(RSSurfaceRenderNode& node) const
{
    auto bufferPixelFormat = node.GetRSSurfaceHandler()->GetBuffer()->GetFormat();
    return (bufferPixelFormat == GRAPHIC_PIXEL_FMT_RGBA_1010102 ||
         bufferPixelFormat == GRAPHIC_PIXEL_FMT_YCBCR_P010 ||
         bufferPixelFormat == GRAPHIC_PIXEL_FMT_YCRCB_P010) && !node.IsHardwareForcedDisabledByFilter() &&
         !isUpdateCachedSurface_;
}

bool RSUniRenderVisitor::UpdateSrcRectForHwcNode(RSSurfaceRenderNode& node, bool isProtected)
{
    Drawing::AutoCanvasRestore acr(*canvas_.get(), true);

    if (displayNodeMatrix_.has_value()) {
        auto& displayNodeMatrix = displayNodeMatrix_.value();
        auto matrix = canvas_->GetTotalMatrix();
        matrix.PostConcat(displayNodeMatrix);
        canvas_->SetMatrix(matrix);
    }
    node.SetTotalMatrix(canvas_->GetTotalMatrix());

    auto dstRect = node.GetDstRect();
    Drawing::RectI dst = { dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(),
                           dstRect.GetBottom() };
    bool hasRotation = false;
    if (node.GetRSSurfaceHandler()->GetConsumer() != nullptr) {
        auto rotation = RSBaseRenderUtil::GetRotateTransform(RSBaseRenderUtil::GetSurfaceBufferTransformType(
            node.GetRSSurfaceHandler()->GetConsumer(), node.GetRSSurfaceHandler()->GetBuffer()));
        hasRotation = rotation == GRAPHIC_ROTATE_90 || rotation == GRAPHIC_ROTATE_270;
    }
    node.UpdateSrcRect(*canvas_, dst, hasRotation);
    return isProtected ? true : !node.IsHardwareDisabledBySrcRect();
}

void RSUniRenderVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    RS_LOGE("RSUniRenderVisitor::%{public}s is upgraded to DrawableV2", __func__);
}

void RSUniRenderVisitor::ProcessProxyRenderNode(RSProxyRenderNode& node)
{
    RS_LOGE("RSUniRenderVisitor::%{public}s is upgraded to DrawableV2", __func__);
}

void RSUniRenderVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    RS_LOGE("RSUniRenderVisitor::%{public}s is upgraded to DrawableV2", __func__);
}

bool RSUniRenderVisitor::GenerateNodeContentCache(RSRenderNode& node)
{
    // Node cannot have cache.
    if (node.GetDrawingCacheType() == RSDrawingCacheType::DISABLED_CACHE) {
        if (node.cacheCnt_ > 0) {
            node.SetCacheType(CacheType::NONE);
            RSUniRenderUtil::ClearCacheSurface(node, threadIndex_);
            node.cacheCnt_ = -1;
        }
        return false;
    }

    // The node goes down the tree to clear the cache.
    if (node.GetCacheType() == CacheType::NONE && node.cacheCnt_ > 0) {
        node.cacheCnt_ = -1;
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
        if (node.cacheCnt_ == -1 || (!node.IsStaticCached() && node.NeedInitCacheCompletedSurface())) {
            RenderParam val { node.shared_from_this(), canvas_->GetCanvasStatus() };
            curGroupedNodes_.push(val);
            node.SetCacheType(CacheType::CONTENT);
            RSUniRenderUtil::ClearCacheSurface(node, threadIndex_);
            if (UpdateCacheSurface(node)) {
                node.UpdateCompletedCacheSurface();
                node.cacheCnt_++;
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
    cacheRenderNodeIsUpdateMap_[node.GetId()] = node.GetDrawingCacheChanged();
    if (node.GetDrawingCacheType() == RSDrawingCacheType::FORCED_CACHE) {
        // Regardless of the number of consecutive refreshes, the current cache is forced to be updated.
        if (node.GetDrawingCacheChanged()) {
            RenderParam val { node.shared_from_this(), canvas_->GetCanvasStatus() };
            curGroupedNodes_.push(val);
            node.SetCacheType(CacheType::CONTENT);
            if (UpdateCacheSurface(node)) {
                node.UpdateCompletedCacheSurface();
                node.cacheCnt_++;
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
            node.cacheCnt_++;
            if (node.cacheCnt_ >= CACHE_MAX_UPDATE_TIME) {
                node.SetCacheType(CacheType::NONE);
                node.MarkNodeGroup(RSRenderNode::GROUPED_BY_UI, false, false);
                node.MarkNodeGroup(RSRenderNode::GROUPED_BY_ANIM, false, false);
                RSUniRenderUtil::ClearCacheSurface(node, threadIndex_);
                node.cacheCnt_ = -1;
                cacheReuseTimes = 0;
                return;
            }
            node.SetCacheType(CacheType::CONTENT);
            if (UpdateCacheSurface(node)) {
                node.UpdateCompletedCacheSurface();
                cacheReuseTimes = 0;
                node.ResetDrawingCacheNeedUpdate();
            }
            curGroupedNodes_.pop();
            return;
        }
    }
    // The cache is not refreshed continuously.
    node.cacheCnt_ = 0;
    cacheReuseTimes++;
    RS_OPTIONAL_TRACE_NAME("RSUniRenderVisitor::UpdateCacheRenderNodeMap ,NodeId: " + std::to_string(node.GetId()) +
        " ,CacheRenderNodeMapCnt: " + std::to_string(cacheReuseTimes));
}

void RSUniRenderVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    RS_LOGE("RSUniRenderVisitor::%{public}s is upgraded to DrawableV2", __func__);
}

void RSUniRenderVisitor::ProcessEffectRenderNode(RSEffectRenderNode& node)
{
    RS_LOGE("RSUniRenderVisitor::%{public}s is upgraded to DrawableV2", __func__);
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
    renderThreadParams->isAllSurfaceVisibleDebugEnabled_ = isAllSurfaceVisibleDebugEnabled_;
    renderThreadParams->isTargetDirtyRegionDfxEnabled_ = isTargetDirtyRegionDfxEnabled_;
    renderThreadParams->dirtyRegionDebugType_ = dirtyRegionDebugType_;
    renderThreadParams->isOpDropped_ = isOpDropped_;
    renderThreadParams->isUIFirstDebugEnable_ = isUIFirstDebugEnable_;
    renderThreadParams->dfxTargetSurfaceNames_ = std::move(dfxTargetSurfaceNames_);
    renderThreadParams->isVirtualDirtyEnabled_ = isVirtualDirtyEnabled_;
    renderThreadParams->isVirtualDirtyDfxEnabled_ = isVirtualDirtyDfxEnabled_;
    renderThreadParams->isExpandScreenDirtyEnabled_ = isExpandScreenDirtyEnabled_;
    renderThreadParams->hasMirrorDisplay_ = hasMirrorDisplay_;
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

    if (!RSMainThread::Instance()->WaitHardwareThreadTaskExecute()) {
        RS_LOGW("RSUniRenderVisitor::DoDirectComposition: hardwareThread task has too many to Execute");
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
    DoScreenRcdTask(displayNode->GetId(), processor_, rcdInfo_, screenInfo_);
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

        Drawing::SaveLayerOps slr(nullptr, nullptr, Drawing::SaveLayerOps::INIT_WITH_PREVIOUS);
        canvas_->SaveLayer(slr); // avoid abnormal dsicard
        Drawing::Brush rectPaint;
        canvas_->AttachBrush(rectPaint);
        auto srcRect = Drawing::Rect(0, 0, drImage->GetWidth(), drImage->GetHeight());
        auto dstRect = Drawing::Rect(0, 0, mainWidth, mainHeight);
        canvas_->DrawImageRect(*drImage, srcRect, dstRect, Drawing::SamplingOptions(),
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        canvas_->DetachBrush();
        canvas_->Restore();
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
#ifdef RS_PROFILER_ENABLED
        if (auto canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(width, height)) {
            canvas_->AddCanvas(canvas);
        }
#endif
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
#ifdef RS_PROFILER_ENABLED
        RSCaptureRecorder::GetInstance().EndInstantCapture();
#endif
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
    if (!screenManager || !mirrorNode) {
        RS_LOGE("RSUniRenderVisitor::ScaleMirrorIfNeed screenManager or mirrorNode is nullptr");
        return;
    }
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
        if (!processor) {
            RS_LOGE("RSUniRenderVisitor::ScaleMirrorIfNeed processor is nullptr!");
            return;
        }
        if (scaleMode == ScreenScaleMode::FILL_MODE) {
            processor->Fill(*canvas_, mainWidth, mainHeight, mirrorWidth, mirrorHeight);
        } else if (scaleMode == ScreenScaleMode::UNISCALE_MODE) {
            processor->UniScale(*canvas_, mainWidth, mainHeight, mirrorWidth, mirrorHeight);
        }
    }
}

void RSUniRenderVisitor::SendRcdMessage(RSDisplayRenderNode& node)
{
    if ((screenInfo_.state == ScreenState::HDI_OUTPUT_ENABLE) &&
        RSSingleton<RoundCornerDisplayManager>::GetInstance().GetRcdEnable()) {
        RSRcdRenderManager::GetInstance().CheckRenderTargetNode(RSMainThread::Instance()->GetContext());
        RSSingleton<RoundCornerDisplayManager>::GetInstance().AddRoundCornerDisplay(node.GetId());
        using rcd_msg = RSSingleton<RsMessageBus>;
        rcd_msg::GetInstance().SendMsg<NodeId, uint32_t, uint32_t>(TOPIC_RCD_DISPLAY_SIZE,
            node.GetId(), screenInfo_.width, screenInfo_.height);
        rcd_msg::GetInstance().SendMsg<NodeId, ScreenRotation>(TOPIC_RCD_DISPLAY_ROTATION,
            node.GetId(), node.GetScreenRotation());
        rcd_msg::GetInstance().SendMsg<NodeId, int>(TOPIC_RCD_DISPLAY_NOTCH,
            node.GetId(), RSSystemParameters::GetHideNotchStatus());
    }
}

void RSUniRenderVisitor::RotateMirrorCanvasIfNeedForWiredScreen(RSDisplayRenderNode& node)
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
    if (rotation != ScreenRotation::ROTATION_0) {
        auto screenManager = CreateOrGetScreenManager();
        auto mainScreenInfo = screenManager->QueryScreenInfo(mirrorNode->GetScreenId());
        if (rotation == ScreenRotation::ROTATION_90) {
            canvas_->Rotate(RS_ROTATION_90, 0, 0);
            canvas_->Translate(0, -(static_cast<float>(mainScreenInfo.height)));
        } else if (rotation == ScreenRotation::ROTATION_180) {
            float ratio = 0.5f;
            canvas_->Rotate(RS_ROTATION_180, static_cast<float>(mainScreenInfo.width) * ratio,
                static_cast<float>(mainScreenInfo.height) * ratio);
        } else if (rotation == ScreenRotation::ROTATION_270) {
            canvas_->Rotate(RS_ROTATION_270, 0, 0);
            canvas_->Translate(-(static_cast<float>(mainScreenInfo.width)), 0);
        }
    }
}

void RSUniRenderVisitor::ScaleMirrorIfNeedForWiredScreen(RSDisplayRenderNode& node, bool canvasRotation)
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
    if ((mainWidth != boundsWidth || mainHeight != boundsHeight) &&
        (mainWidth > 0 && mainHeight > 0)) {
        canvas_->Clear(SK_ColorBLACK);
        float mirrorScale = 1.0f; // 1 for init scale
        float startX = 0.0f;
        float startY = 0.0f;
        float mirrorScaleX = boundsWidth / mainWidth;
        float mirrorScaleY = boundsHeight / mainHeight;
        float ratio = 0.5f;
        if (mirrorScaleY < mirrorScaleX) {
            mirrorScale = mirrorScaleY;
            startX = (boundsWidth - (mirrorScale * mainWidth)) * ratio;
        } else {
            mirrorScale = mirrorScaleX;
            startY = (boundsHeight - (mirrorScale * mainHeight)) * ratio;
        }
        canvas_->Translate(startX, startY);
        canvas_->Scale(mirrorScale, mirrorScale);
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
        // make sure parent regenerates ChildrenDrawable
        auto parent = sptr->GetParent().lock();
        if (parent == nullptr) {
            return;
        }
        parent->AddDirtyType(RSModifierType::CHILDREN);
        parent->ApplyModifiers();
        // avoid changing the paired status or unpairedShareTransitions_
        auto param = sptr->GetSharedTransitionParam();
        if (param == nullptr) {
            ROSEN_LOGE("RSUniRenderVisitor::ProcessUnpairedSharedTransitionNode: param is null");
            return;
        }
        param->paired_ = false;
        SharedTransitionParam::unpairedShareTransitions_.clear();
    };
    auto unpairedShareTransitions = std::move(SharedTransitionParam::unpairedShareTransitions_);
    for (auto& [id, wptr] : unpairedShareTransitions) {
        auto sharedTransitionParam = wptr.lock();
        // If the unpaired share transition is already deal with, do nothing
        if (!sharedTransitionParam) {
            continue;
        }
        if (!sharedTransitionParam->paired_) {
            sharedTransitionParam->ResetRelation();
            continue;
        }
        ROSEN_LOGD("RSUniRenderVisitor::ProcessUnpairedSharedTransitionNode: mark %s as unpaired",
            sharedTransitionParam->Dump().c_str());
        sharedTransitionParam->paired_ = false;
        unpairNode(sharedTransitionParam->inNode_);
        unpairNode(sharedTransitionParam->outNode_);
        sharedTransitionParam->ResetRelation();
    }
}

NodeId RSUniRenderVisitor::FindInstanceChildOfDisplay(std::shared_ptr<RSRenderNode> node)
{
    if (node == nullptr) {
        return INVALID_NODEID;
    }
    auto nodeParent = node->GetParent().lock();
    if (nodeParent == nullptr) {
        return INVALID_NODEID;
    }
    if (nodeParent->GetType() == RSRenderNodeType::DISPLAY_NODE) {
        return node->GetId();
    } else {
        return FindInstanceChildOfDisplay(nodeParent);
    }
}

bool RSUniRenderVisitor::CheckIfNeedResetRotate()
{
    if (canvas_ == nullptr) {
        return true;
    }
    auto matrix = canvas_->GetTotalMatrix();
    if (displayNodeMatrix_.has_value()) {
        matrix.PreConcat(displayNodeMatrix_.value());
    }
    int angle = RSUniRenderUtil::GetRotationFromMatrix(matrix);
    return angle != 0 && angle % RS_ROTATION_90 == 0;
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

void RSUniRenderVisitor::DrawCurtainScreen()
{
    if (!isCurtainScreenOn_ || !canvas_) {
        return;
    }
    float screenWidth = static_cast<float>(screenInfo_.width);
    float screenHeight = static_cast<float>(screenInfo_.height);
    Drawing::Brush brush;
    brush.SetARGB(MAX_ALPHA, 0, 0, 0); // not transparent black
    canvas_->AttachBrush(brush);
    canvas_->DrawRect(Drawing::Rect(0, 0, screenWidth, screenHeight));
    canvas_->DetachBrush();
}

bool RSUniRenderVisitor::CheckColorFilterChange() const
{
    if (!RSMainThread::Instance()->IsAccessibilityConfigChanged()) {
        return false;
    }
    RS_LOGD("RSUniRenderVisitor::CheckColorFilterChange changed");
    return true;
}

void RSUniRenderVisitor::CheckMergeDebugRectforRefreshRate(std::vector<RSBaseRenderNode::SharedPtr>& surfaces)
{
    // Debug dirtyregion of show current refreshRation
    if (RSRealtimeRefreshRateManager::Instance().GetShowRefreshRateEnabled()) {
        RectI tempRect = {100, 100, 500, 200};   // setDirtyRegion for RealtimeRefreshRate
        bool surfaceNodeSet = false;
        for (auto surface : surfaces) {
            auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(surface);
            if (surfaceNode == nullptr) {
                RS_LOGE("RSUniRenderVisitor::CheckMergeDebugRectforRefreshRate surfaceNode is nullptr");
                continue;
            }
            if (surfaceNode->GetName().find(RELIABLE_GESTURE_BACK_SURFACE_NAME) != std::string::npos) {
                // refresh rate rect for mainwindow
                auto& geoPtr = surfaceNode->GetRenderProperties().GetBoundsGeometry();
                if (!geoPtr) {
                    break;
                }
                tempRect = geoPtr->MapAbsRect(tempRect.ConvertTo<float>());
                curDisplayNode_->GetDirtyManager()->MergeDirtyRect(tempRect, true);
                surfaceNodeSet = true;
                break;
            }
        }
        if (!surfaceNodeSet) {
            auto &geoPtr = curDisplayNode_->GetRenderProperties().GetBoundsGeometry();
            if (!geoPtr) {
                return;
            }
            tempRect = geoPtr->MapAbsRect(tempRect.ConvertTo<float>());
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(tempRect, true);
        }
    }
}

void RSUniRenderVisitor::CheckIsGpuOverDrawBufferOptimizeNode(RSSurfaceRenderNode& node)
{
    bool hasAnim = ancestorNodeHasAnimation_ || node.GetCurFrameHasAnimation();
    if (!node.IsScale() || hasAnim || curCornerRadius_.IsZero() || curAlpha_ < 1) {
        node.SetGpuOverDrawBufferOptimizeNode(false);
        return;
    }

    for (auto& child : *(node.GetChildren())) {
        if (!child || !(child->IsInstanceOf<RSSurfaceRenderNode>() &&
            RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child)->IsLeashOrMainWindow())) {
            continue;
        }
        auto rootNode = child->GetFirstChild();
        if (!rootNode) {
            break;
        }
        auto canvasNode = rootNode->GetFirstChild();
        if (!canvasNode) {
            break;
        }
        const auto& surfaceProperties = node.GetRenderProperties();
        const auto& canvasProperties = canvasNode->GetRenderProperties();
        if (canvasProperties.GetAlpha() >= 1
            && canvasProperties.GetBackgroundColor().GetAlpha() >= MAX_ALPHA
            && canvasProperties.GetFrameWidth() == surfaceProperties.GetFrameWidth()
            && canvasProperties.GetFrameHeight() == surfaceProperties.GetFrameHeight()) {
            node.SetGpuOverDrawBufferOptimizeNode(true);
            node.SetOverDrawBufferNodeCornerRadius(curCornerRadius_);
            return;
        }
    }

    node.SetGpuOverDrawBufferOptimizeNode(false);
}

void RSUniRenderVisitor::TryNotifyUIBufferAvailable()
{
    for (auto& id : uiBufferAvailableId_) {
        const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
        auto surfaceNode = nodeMap.GetRenderNode<RSSurfaceRenderNode>(id);
        if (surfaceNode) {
            surfaceNode->NotifyUIBufferAvailable();
        }
    }
    uiBufferAvailableId_.clear();
}
} // namespace Rosen
} // namespace OHOS
