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

#include "pipeline/main_thread/rs_uni_render_visitor.h"
#include <memory>
#include "rs_trace.h"
#include "screen_manager/rs_screen_manager.h"

#ifdef RS_ENABLE_OLD_VK
#include <vulkan_window.h>
#endif

#include "common/rs_common_def.h"
#include "common/rs_common_hook.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "common/rs_singleton.h"
#include "common/rs_special_layer_manager.h"
#include "feature/opinc/rs_opinc_manager.h"
#include "feature/hdr/rs_hdr_util.h"
#include "feature/hwc/rs_uni_hwc_compute_util.h"
#include "feature/occlusion_culling/rs_occlusion_handler.h"
#include "feature/opinc/rs_opinc_cache.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "monitor/self_drawing_node_monitor.h"
#ifdef RS_ENABLE_OVERLAY_DISPLAY
#include "feature/overlay_display/rs_overlay_display_manager.h"
#endif
#include "display_engine/rs_luminance_control.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_display_render_params.h"
#include "params/rs_rcd_render_params.h"
#include "pipeline/render_thread/rs_base_render_util.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/render_thread/rs_uni_render_virtual_processor.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_pointer_window_manager.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/hwc/rs_uni_hwc_visitor.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties_painter.h"
#include "system/rs_system_parameters.h"
#include "hgm_core.h"
#include "metadata_helper.h"
#include <v1_0/buffer_handle_meta_key_type.h>
#include <v1_0/cm_color_space.h>

#include "feature_cfg/graphic_feature_param_manager.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "feature/round_corner_display/rs_message_bus.h"

#include "rs_profiler.h"
#ifdef RS_PROFILER_ENABLED
#include "rs_profiler_capture_recorder.h"
#endif

// blur predict
#include "rs_frame_blur_predict.h"

#undef LOG_TAG
#define LOG_TAG "RSUniRenderVisitor"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t VISIBLEAREARATIO_FORQOS = 3;
constexpr int MAX_ALPHA = 255;
constexpr int TRACE_LEVEL_THREE = 3;
constexpr float EPSILON_SCALE = 0.00001f;
static const std::string CAPTURE_WINDOW_NAME = "CapsuleWindow";
constexpr const char* RELIABLE_GESTURE_BACK_SURFACE_NAME = "SCBGestureBack";
constexpr int MIN_OVERLAP = 2;
constexpr uint64_t INPUT_HWC_LAYERS = 3;

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
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (const auto& v : val) {
        auto surfaceNode = nodeMap.GetRenderNode<RSSurfaceRenderNode>(v.first);
        auto name = surfaceNode ? surfaceNode->GetName() : "";
        ss << "[" << name << ", " << v.first << ", " << v.second << "], ";
    }
    return ss.str();
}
} // namespace

bool RSUniRenderVisitor::isLastFrameRotating_ = false;

RSUniRenderVisitor::RSUniRenderVisitor()
    : hwcVisitor_(std::make_unique<RSUniHwcVisitor>(*this)),
      curSurfaceDirtyManager_(std::make_shared<RSDirtyRegionManager>())
{
    PartialRenderOptionInit();
    auto mainThread = RSMainThread::Instance();
    renderEngine_ = mainThread->GetRenderEngine();
    hasMirrorDisplay_ = mainThread->HasMirrorDisplay();
    // when occlusion enabled is false, subTree do not skip, but not influence visible region
    isOcclusionEnabled_ = RSSystemProperties::GetOcclusionEnabled();
    isDrawingCacheEnabled_ = RSSystemParameters::GetDrawingCacheEnabled();
#ifdef DDGR_ENABLE_FEATURE_OPINC
    autoCacheEnable_ = RSOpincManager::Instance().GetOPIncSwitch();
#endif
    RSTagTracker::UpdateReleaseResourceEnabled(RSSystemProperties::GetReleaseResourceEnabled());
    isScreenRotationAnimating_ = RSSystemProperties::GetCacheEnabledForRotation();
    isFirstFrameAfterScreenRotation_ = !isScreenRotationAnimating_ && isLastFrameRotating_;
    isLastFrameRotating_ = isScreenRotationAnimating_;
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (renderEngine_ && renderEngine_->GetRenderContext()) {
        auto subThreadManager = RSSubThreadManager::Instance();
        subThreadManager->Start(renderEngine_->GetRenderContext().get());
    }
#endif
    isUIFirstDebugEnable_ = RSSystemProperties::GetUIFirstDebugEnabled();
    isCrossNodeOffscreenOn_ = RSSystemProperties::GetCrossNodeOffScreenStatus();
    isDumpRsTreeDetailEnabled_ = RSSystemProperties::GetDumpRsTreeDetailEnabled();
}

RSUniRenderVisitor::~RSUniRenderVisitor() {}

void RSUniRenderVisitor::PartialRenderOptionInit()
{
    isPartialRenderEnabled_ = DirtyRegionParam::IsDirtyRegionEnable();
    isAdvancedDirtyRegionEnabled_ = DirtyRegionParam::IsAdvancedDirtyRegionEnable();
    isDirtyAlignEnabled_ = DirtyRegionParam::IsTileBasedAlignEnable();

    if (RSSystemProperties::GetStencilPixelOcclusionCullingEnabled() == StencilPixelOcclusionCullingType::DEFAULT) {
        isStencilPixelOcclusionCullingEnabled_ =
            OcclusionCullingParam::IsStencilPixelOcclusionCullingEnable();
    } else {
        isStencilPixelOcclusionCullingEnabled_ =
            RSSystemProperties::GetStencilPixelOcclusionCullingEnabled() != StencilPixelOcclusionCullingType::DISABLED;
    }
    
    partialRenderType_ = RSSystemProperties::GetUniPartialRenderEnabled();
    isPartialRenderEnabled_ &= (partialRenderType_ > PartialRenderType::DISABLED);

    isCompleteRenderEnabled_ = (partialRenderType_ == PartialRenderType::SET_DAMAGE_BUT_COMPLETE_RENDER);
    dirtyRegionDebugType_ = RSSystemProperties::GetDirtyRegionDebugType();
    surfaceRegionDebugType_ = RSSystemProperties::GetSurfaceRegionDfxType();
    isTargetDirtyRegionDfxEnabled_ = RSSystemProperties::GetTargetDirtyRegionDfxEnabled(dfxTargetSurfaceNames_) &&
        (surfaceRegionDebugType_ == SurfaceRegionDebugType::DISABLED);
    isTargetUIFirstDfxEnabled_ = RSSystemProperties::GetTargetUIFirstDfxEnabled(dfxUIFirstSurfaceNames_);
    isRegionDebugEnabled_ = (dirtyRegionDebugType_ != DirtyRegionDebugType::DISABLED) ||
        (surfaceRegionDebugType_ != SurfaceRegionDebugType::DISABLED) || (dfxTargetSurfaceNames_.size() > 0);
    isVisibleRegionDfxEnabled_ = (surfaceRegionDebugType_ == SurfaceRegionDebugType::VISIBLE_REGION);
    isOpaqueRegionDfxEnabled_ = (surfaceRegionDebugType_ == SurfaceRegionDebugType::OPAQUE_REGION);
    isAllSurfaceVisibleDebugEnabled_ = RSSystemProperties::GetAllSurfaceVisibleDebugEnabled();
    isDirtyRegionDfxEnabled_ = !isTargetDirtyRegionDfxEnabled_ &&
        (dirtyRegionDebugType_ == DirtyRegionDebugType::EGL_DAMAGE);
    isDisplayDirtyDfxEnabled_ = !isTargetDirtyRegionDfxEnabled_ &&
        (dirtyRegionDebugType_ == DirtyRegionDebugType::DISPLAY_DIRTY);
    isOpDropped_ = isPartialRenderEnabled_ &&
        (partialRenderType_ != PartialRenderType::SET_DAMAGE) && !isRegionDebugEnabled_;
    isVirtualDirtyDfxEnabled_ = RSSystemProperties::GetVirtualDirtyDebugEnabled();
    isVirtualDirtyEnabled_ = RSSystemProperties::GetVirtualDirtyEnabled() &&
        (RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL) && !isRegionDebugEnabled_;
    isExpandScreenDirtyEnabled_ = RSSystemProperties::GetExpandScreenDirtyEnabled();
    advancedDirtyType_ = isAdvancedDirtyRegionEnabled_ ?
        RSSystemProperties::GetAdvancedDirtyRegionEnabled() : AdvancedDirtyRegionType::DISABLED;
    isDirtyAlignEnabled_ &= RSSystemProperties::GetDirtyAlignEnabled() != DirtyAlignType::DISABLED;
    if (isStencilPixelOcclusionCullingEnabled_) {
        // SPOC relies on dirty region alignment; when SPOC is enabled, dirty region alignment must also be enabled
        isDirtyAlignEnabled_ = true;
    }
}

void RSUniRenderVisitor::MergeRemovedChildDirtyRegion(RSRenderNode& node, bool needMap)
{
    if (!node.HasRemovedChild()) {
        return;
    }
    RectI dirtyRect = RSSystemProperties::GetOptimizeParentNodeRegionEnabled() ?
        node.GetRemovedChildrenRect() : node.GetChildrenRect();
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

void RSUniRenderVisitor::CheckColorSpace(RSSurfaceRenderNode& node)
{
    // currently, P3 is the only supported wide color gamut, this may be modified later.
    if (node.IsAppWindow() && node.GetColorSpace() != GRAPHIC_COLOR_GAMUT_SRGB) {
        if (!curDisplayNode_) {
            RS_LOGD("RSUniRenderVisitor::CheckColorSpace: curDisplayNode_ is nullptr");
            return;
        }
        curDisplayNode_->SetColorSpace(GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
        RS_LOGD("RSUniRenderVisitor::CheckColorSpace: node(%{public}s) set new colorgamut %{public}d",
            node.GetName().c_str(), static_cast<int>(GRAPHIC_COLOR_GAMUT_DISPLAY_P3));
    }
}

void RSUniRenderVisitor::CheckColorSpaceWithSelfDrawingNode(RSSurfaceRenderNode& node, GraphicColorGamut& newColorSpace)
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
    auto appSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.GetInstanceRootNode());
    if (RsCommonHook::Instance().GetIsCoveredSurfaceCloseP3() && appSurfaceNode
        && appSurfaceNode->IsMainWindowType() && appSurfaceNode->GetVisibleRegion().IsEmpty() && GetIsOpDropped()) {
        RS_LOGD("RSUniRenderVisitor::CheckColorSpaceWithSelfDrawingNode node(%{public}s) failed to set new color "
                "gamut %{public}d because the window is blocked", node.GetName().c_str(), newColorSpace);
        return;
    }

    if (node.GetColorSpace() != GRAPHIC_COLOR_GAMUT_SRGB) {
        newColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
        RS_LOGD("RSUniRenderVisitor::CheckColorSpaceWithSelfDrawingNode node(%{public}s) set new colorgamut %{public}d",
            node.GetName().c_str(), newColorSpace);
    }
}

void RSUniRenderVisitor::UpdateColorSpaceAfterHwcCalc(RSDisplayRenderNode& node)
{
    auto colorSpace = node.GetColorSpace();
    if (colorSpace == GRAPHIC_COLOR_GAMUT_DISPLAY_P3) {
        RS_LOGD("RSUniRenderVisitor::UpdateColorSpaceAfterHwcCalc: newColorSpace is already DISPLAY_P3.");
        return;
    }
    const auto& selfDrawingNodes = RSMainThread::Instance()->GetSelfDrawingNodes();
    for (const auto& selfDrawingNode : selfDrawingNodes) {
        if (!selfDrawingNode) {
            RS_LOGD("RSUniRenderVisitor::UpdateColorSpaceAfterHwcCalc selfDrawingNode is nullptr");
            continue;
        }
        auto ancestorNode = selfDrawingNode->GetAncestorDisplayNode().lock();
        if (!ancestorNode) {
            RS_LOGD("RSUniRenderVisitor::UpdateColorSpaceAfterHwcCalc ancestorNode is nullptr");
            continue;
        }
        auto ancestorDisplayNode = ancestorNode->ReinterpretCastTo<RSDisplayRenderNode>();
        if (ancestorDisplayNode != nullptr && node.GetId() == ancestorDisplayNode->GetId()) {
            CheckColorSpaceWithSelfDrawingNode(*selfDrawingNode, colorSpace);
        }
    }
    node.SetColorSpace(colorSpace);
}

bool IsScreenSupportedWideColorGamut(ScreenId id, const sptr<RSScreenManager>& screenManager)
{
    std::vector<ScreenColorGamut> supportedColorGamut;
    if (screenManager->GetScreenSupportedColorGamuts(id, supportedColorGamut) != SUCCESS) {
        return false;
    }
    for (auto item : supportedColorGamut) {
        if (item == ScreenColorGamut::COLOR_GAMUT_DCI_P3 || item == ScreenColorGamut::COLOR_GAMUT_DISPLAY_P3) {
            return true;
        }
    }
    return false;
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
        node.SetColorSpace(static_cast<GraphicColorGamut>(screenColorGamut));
        return;
    } else if (ColorGamutParam::IsWiredExtendScreenCloseP3() && !RSMainThread::Instance()->HasWiredMirrorDisplay() &&
        node.GetScreenId() != 0) {
        // Current PC external screen do not support P3.
        // The wired extended screen is fixed to sRGB color space.
        node.SetColorSpace(GRAPHIC_COLOR_GAMUT_SRGB);
        return;
    }

    if (RSMainThread::Instance()->HasWiredMirrorDisplay() && !MultiScreenParam::IsMirrorDisplayCloseP3()) {
        std::shared_ptr<RSDisplayRenderNode> mirrorNode = node.GetMirrorSource().lock();
        if (!mirrorNode) {
            return;
        }
        std::vector<ScreenColorGamut> mode;
        int32_t ret = screenManager->GetScreenSupportedColorGamuts(node.GetScreenId(), mode);
        if (ret != SUCCESS) {
            RS_LOGD("RSUniRenderVisitor::HandleColorGamuts GetScreenSupportedColorGamuts failed, errorCode=%{public}d",
                ret);
            return;
        };
        bool isSupportedDisplayP3 =
            std::any_of(mode.begin(), mode.end(), [](const auto& gamut) { return gamut == COLOR_GAMUT_DISPLAY_P3; });
        if (isSupportedDisplayP3) {
            // wired mirror and mirror support P3, mirror gamut = main gamut
            node.SetColorSpace(mirrorNode->GetColorSpace());
        }
    }
}

void RSUniRenderVisitor::CheckPixelFormat(RSSurfaceRenderNode& node)
{
    if (hasFingerprint_[currentVisitDisplay_]) {
        RS_LOGD("RSUniRenderVisitor::CheckPixelFormat hasFingerprint is true.");
        return;
    }
    if (!curDisplayNode_) {
        RS_LOGE("RSUniRenderVisitor::CheckPixelFormat curDisplayNode is null");
        return;
    }
    if (node.GetFingerprint()) {
        hasFingerprint_[currentVisitDisplay_] = true;
        curDisplayNode_->SetPixelFormat(GRAPHIC_PIXEL_FMT_RGBA_1010102);
        RS_LOGD("RSUniRenderVisitor::CheckPixelFormat pixelFormate is set 1010102 for fingerprint.");
        return;
    }
    if (!RSSystemProperties::GetHdrImageEnabled()) {
        RS_LOGD("RSUniRenderVisitor::CheckPixelFormat HdrImageEnabled false");
        return;
    }
    if (curDisplayNode_->GetForceCloseHdr()) {
        RS_LOGD("RSUniRenderVisitor::CheckPixelFormat curDisplayNode forceclosehdr.");
        return;
    }
    if (node.GetHDRPresent()) {
        RS_LOGD("RSUniRenderVisitor::CheckPixelFormat HDRService SetHDRPresent true, surfaceNode: %{public}" PRIu64 "",
            node.GetId());
        curDisplayNode_->SetHasUniRenderHdrSurface(true);
        curDisplayNode_->CollectHdrStatus(HdrStatus::HDR_PHOTO);
        RSHdrUtil::SetHDRParam(node, true);
        if (curDisplayNode_->GetIsLuminanceStatusChange()) {
            node.SetContentDirty();
        }
    }
}

void RSUniRenderVisitor::HandlePixelFormat(RSDisplayRenderNode& node, const sptr<RSScreenManager>& screenManager)
{
    if (!curDisplayNode_) {
        RS_LOGE("RSUniRenderVisitor::HandlePixelFormat curDisplayNode is null");
        return;
    }
    SetHdrWhenMultiDisplayChange();
    RS_LOGD("HDRSwitch isHdrImageEnabled: %{public}d, isHdrVideoEnabled: %{public}d",
        RSSystemProperties::GetHdrImageEnabled(), RSSystemProperties::GetHdrVideoEnabled());
    ScreenId screenId = node.GetScreenId();
    bool hasUniRenderHdrSurface = node.GetHasUniRenderHdrSurface();
    if ((RSLuminanceControl::Get().IsCloseHardwareHdr() && !drmNodes_.empty()) || node.GetForceCloseHdr()) {
        // Disable hdr when drm videos exist to avoid flicker
        RSLuminanceControl::Get().SetHdrStatus(screenId, HdrStatus::NO_HDR);
    } else {
        RSLuminanceControl::Get().SetHdrStatus(screenId, node.GetDisplayHdrStatus());
    }
    bool isHdrOn = RSLuminanceControl::Get().IsHdrOn(screenId);
    rsHdrCollection_->HandleHdrState(isHdrOn);
    float brightnessRatio = RSLuminanceControl::Get().GetHdrBrightnessRatio(screenId, 0);
    RS_TRACE_NAME_FMT("HDR:%d, in Unirender:%d brightnessRatio:%f", isHdrOn, hasUniRenderHdrSurface, brightnessRatio);
    RS_LOGD("RSUniRenderVisitor::HandlePixelFormat HDRService isHdrOn:%{public}d hasUniRenderHdrSurface:%{public}d "
        "brightnessRatio:%{public}f screenId: %{public}" PRIu64 " status:%{public}d", isHdrOn, hasUniRenderHdrSurface,
        brightnessRatio, screenId, node.GetDisplayHdrStatus());
    if ((!hasUniRenderHdrSurface && !RSLuminanceControl::Get().IsCloseHardwareHdr()) || node.GetForceCloseHdr()) {
        isHdrOn = false;
    }
    node.SetHDRPresent(isHdrOn);
    hasDisplayHdrOn_ |= isHdrOn;
    RSScreenType screenType = BUILT_IN_TYPE_SCREEN;
    if (screenManager->GetScreenType(node.GetScreenId(), screenType) != SUCCESS) {
        RS_LOGD("RSUniRenderVisitor::HandlePixelFormat get screen type failed.");
        return;
    }

    if (screenType == VIRTUAL_TYPE_SCREEN) {
        auto pixelFormat = node.GetPixelFormat();
        if (screenManager->GetPixelFormat(node.GetScreenId(), pixelFormat) != SUCCESS) {
            RS_LOGD("RSUniRenderVisitor::HandlePixelFormat get screen color gamut failed.");
        } else {
            node.SetPixelFormat(pixelFormat);
        }
    }
}

void RSUniRenderVisitor::ResetCurSurfaceInfoAsUpperSurfaceParent(RSSurfaceRenderNode& node)
{
    // record current frame mainwindow or leashwindow node
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        curMainAndLeashWindowNodesIds_.push(node.GetId());
        RSMainThread::Instance()->GetRSVsyncRateReduceManager().PushWindowNodeId(node.GetId());
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
    return !hwcVisitor_->isHardwareForcedDisabled_;
}

void RSUniRenderVisitor::MarkHardwareForcedDisabled()
{
    hwcVisitor_->isHardwareForcedDisabled_ = true;
}

void RSUniRenderVisitor::UpdateSpecialLayersRecord(RSSurfaceRenderNode& node)
{
    if (node.ShouldPaint() == false) {
        return;
    }
    auto specialLayerMgr = node.GetMultableSpecialLayerMgr();
    if (specialLayerMgr.Find(SpecialLayerType::HAS_SECURITY)) {
        curDisplayNode_->AddSecurityLayer(node.IsLeashWindow() ? node.GetLeashPersistentId() : node.GetId());
        curDisplayNode_->AddSecurityVisibleLayer(node.GetId());
    }
    if (specialLayerMgr.Find(SpecialLayerType::HAS_PROTECTED)) {
        screenManager_->SetScreenHasProtectedLayer(currentVisitDisplay_, true);
        RS_TRACE_NAME_FMT("SetScreenHasProtectedLayer: %d", currentVisitDisplay_);
    }
    auto specialLayerType = specialLayerMgr.Get();
    if (node.GetName().find(CAPTURE_WINDOW_NAME) != std::string::npos) {
        specialLayerType &= ~SpecialLayerType::HAS_SKIP;
    }
    curDisplayNode_->GetMultableSpecialLayerMgr().AddIds((specialLayerType >> SPECIAL_TYPE_NUM), node.GetId());
    if (node.IsSpecialLayerChanged()) {
        displaySpecailSurfaceChanged_[currentVisitDisplay_] = true;
    }
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
    bool ret = CheckScreenPowerChange() ||
        CheckCurtainScreenUsingStatusChange() ||
        IsFirstFrameOfPartialRender() ||
        IsWatermarkFlagChanged() ||
        zoomStateChange_ ||
        isCompleteRenderEnabled_ ||
        curDisplayNode_->GetIsLuminanceStatusChange() ||
        IsFirstFrameOfOverdrawSwitch() ||
        IsFirstFrameOfDrawingCacheDfxSwitch() ||
        IsAccessibilityConfigChanged() ||
        curDisplayNode_->HasMirroredDisplayChanged();

#ifdef RS_ENABLE_OVERLAY_DISPLAY
    ret = ret || RSOverlayDisplayManager::Instance().CheckStatusChanged();
#endif
    if (ret) {
        curDisplayDirtyManager_->ResetDirtyAsSurfaceSize();
        RS_LOGD("RSUniRenderVisitor::ResetDisplayDirtyRegion on");
    }
}

bool RSUniRenderVisitor::IsAccessibilityConfigChanged() const
{
    return RSMainThread::Instance()->IsAccessibilityConfigChanged();
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

bool RSUniRenderVisitor::CheckLuminanceStatusChange(ScreenId id)
{
    // ExchangeLuminanceChangingStatus will be reset false after called in one frame
    // Use isLuminanceStatusChange_ if need call this function multiple times in one frame
    if (!RSMainThread::Instance()->ExchangeLuminanceChangingStatus(id)) {
        return false;
    }
    curDisplayNode_->SetIsLuminanceStatusChange(true);
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

bool RSUniRenderVisitor::IsFirstFrameOfDrawingCacheDfxSwitch() const
{
    return RSMainThread::Instance()->IsFirstFrameOfDrawingCacheDFXSwitch();
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

void RSUniRenderVisitor::UpdateVirtualScreenInfo(RSDisplayRenderNode& node)
{
    // only for virtual screen
    if (!(node.IsMirrorDisplay())) {
        node.ClearSecurityLayerList();
        node.ClearSecurityVisibleLayerList();
        return;
    }
    auto mirrorNode = node.GetMirrorSource().lock();
    if (mirrorNode == nullptr || screenManager_ == nullptr) {
        return;
    }

    UpdateVirtualScreenSecurityExemption(node, *mirrorNode);
    UpdateVirtualScreenVisibleRectSecurity(node, *mirrorNode);
}

void RSUniRenderVisitor::UpdateVirtualScreenSecurityExemption(RSDisplayRenderNode& node,
    RSDisplayRenderNode& mirrorNode)
{
    auto securityExemptionList = screenManager_->GetVirtualScreenSecurityExemptionList(node.GetScreenId());
    if (securityExemptionList.size() == 0) {
        RS_LOGD("UpdateVirtualScreenSecurityExemption::node:%{public}" PRIu64 ", isSecurityExemption:false",
            node.GetId());
        node.SetSecurityExemption(false);
        return;
    }
    auto securityLayerList = mirrorNode.GetSecurityLayerList();
    for (const auto& exemptionLayer : securityExemptionList) {
        RS_LOGD("UpdateVirtualScreenSecurityExemption::node:%{public}" PRIu64 ""
            "securityExemption nodeId %{public}" PRIu64 ".", node.GetId(), exemptionLayer);
    }
    for (const auto& secLayer : securityLayerList) {
        RS_LOGD("UpdateVirtualScreenSecurityExemption::node:%{public}" PRIu64 ""
            "securityLayer nodeId %{public}" PRIu64 ".", mirrorNode.GetId(), secLayer);
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
}

void RSUniRenderVisitor::UpdateVirtualScreenVisibleRectSecurity(RSDisplayRenderNode& node,
    RSDisplayRenderNode& mirrorNode)
{
    if (!screenManager_->QueryScreenInfo(node.GetScreenId()).enableVisibleRect) {
        return;
    }
    auto rect = screenManager_->GetMirrorScreenVisibleRect(node.GetScreenId());
    RectI visibleRect(rect.x, rect.y, rect.w, rect.h);
    auto securityVisibleLayerList = mirrorNode.GetSecurityVisibleLayerList();
    bool hasSecLayerInVisibleRect = false;
    for (const auto& secLayerId : securityVisibleLayerList) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(
            RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(secLayerId));
        if (surfaceNode == nullptr) {
            continue;
        }
        const auto& dstRect = surfaceNode->GetDstRect();
        if (dstRect.Intersect(visibleRect)) {
            hasSecLayerInVisibleRect = true;
            break;
        }
    }
    RS_LOGD("%{public}s: hasSecLayerInVisibleRect:%{public}d.", __func__, hasSecLayerInVisibleRect);
    node.SetHasSecLayerInVisibleRect(hasSecLayerInVisibleRect);
}

void RSUniRenderVisitor::UpdateCurFrameInfoDetail(RSRenderNode& node, bool subTreeSkipped, bool isPostPrepare)
{
    if (GetDumpRsTreeDetailEnabled()) {
        auto& curFrameInfoDetail = node.GetCurFrameInfoDetail();
        curFrameInfoDetail.curFrameVsyncId = HgmCore::Instance().GetVsyncId();
        curFrameInfoDetail.curFrameSubTreeSkipped = subTreeSkipped;
        if (isPostPrepare) {
            curFrameInfoDetail.curFramePostPrepareSeqNum = IncreasePostPrepareSeq();
        } else {
            curFrameInfoDetail.curFramePrepareSeqNum = IncreasePrepareSeq();
        }
    }
}

void RSUniRenderVisitor::QuickPrepareDisplayRenderNode(RSDisplayRenderNode& node)
{
    UpdateCurFrameInfoDetail(node);
    // 0. init display info
    RS_TRACE_NAME("RSUniRender:QuickPrepareDisplayRenderNode " + std::to_string(node.GetScreenId()));
    if (!InitDisplayInfo(node)) {
        return;
    }
    UpdateDisplayZoomState();
    SendRcdMessage(node);
    UpdateVirtualScreenInfo(node);
    ancestorNodeHasAnimation_ = false;
    displayNodeRotationChanged_ = node.IsRotationChanged();
    dirtyFlag_ = isDirty_ || displayNodeRotationChanged_ || zoomStateChange_ ||
        curDisplayDirtyManager_->IsActiveSurfaceRectChanged() ||
        curDisplayDirtyManager_->IsSurfaceRectChanged() || node.IsDirty();
    prepareClipRect_ = screenRect_;
    hasAccumulatedClip_ = false;

    globalShouldPaint_ = true;
    curAlpha_ = 1.0f;
    globalZOrder_ = 0.0f;
    appWindowZOrder_ = 0;
    hasSkipLayer_ = false;
    hwcVisitor_->curZOrderForHwcEnableByFilter_ = 0;
    node.GetHwcRecorder().SetZOrderForHwcEnableByFilter(hwcVisitor_->curZOrderForHwcEnableByFilter_++);
    node.UpdateRotation();
    if (!(RSMainThread::Instance()->IsRequestedNextVSync() || RSMainThread::Instance()->GetNextDVsyncAnimateFlag())) {
        RS_OPTIONAL_TRACE_NAME_FMT("do not request next vsync");
        needRequestNextVsync_ = false;
    }
    RSUifirstManager::Instance().SetRotationChanged(displayNodeRotationChanged_ || isScreenRotationAnimating_);
    if (node.IsSubTreeDirty() || node.IsRotationChanged()) {
        rsDisplayNodeChildNum_ = 0;
        QuickPrepareChildren(node);
        RS_TRACE_NAME_FMT("RSUniRenderVisitor::QuickPrepareDisplayRenderNode childNumber: %d", rsDisplayNodeChildNum_);
        TryNotifyUIBufferAvailable();
    }
    PostPrepare(node);
    hwcVisitor_->UpdateHwcNodeEnable();
    UpdateDisplayRcdRenderNode();
    UpdateSurfaceDirtyAndGlobalDirty();
    UpdateSurfaceOcclusionInfo();
    if (needRecalculateOcclusion_) {
        // Callback for registered self drawing surfacenode
        RSMainThread::Instance()->SurfaceOcclusionCallback();
    }
    curDisplayNode_->UpdatePartialRenderParams();
    RSDisplayRenderNode::ScreenRenderParams screenRenderParams;
    screenRenderParams.screenInfo = std::move(screenInfo_);
    screenRenderParams.displaySpecailSurfaceChanged = std::move(displaySpecailSurfaceChanged_);
    screenRenderParams.hasCaptureWindow = std::move(hasCaptureWindow_);
    curDisplayNode_->SetFingerprint(hasFingerprint_[currentVisitDisplay_]);
    curDisplayNode_->UpdateScreenRenderParams(screenRenderParams);
    curDisplayNode_->UpdateOffscreenRenderParams(curDisplayNode_->IsRotationChanged());
    UpdateColorSpaceAfterHwcCalc(node);
    RSHdrUtil::UpdatePixelFormatAfterHwcCalc(node);
    HandleColorGamuts(node, screenManager_);
    HandlePixelFormat(node, screenManager_);
    if (UNLIKELY(!SharedTransitionParam::unpairedShareTransitions_.empty())) {
        ProcessUnpairedSharedTransitionNode();
    }
    node.HandleCurMainAndLeashSurfaceNodes();
    layerNum_ += node.GetSurfaceCountForMultiLayersPerf();
    node.RenderTraceDebug();
    containerFilter_.clear();
    globalFilter_.clear();
    curSurfaceNoBelowDirtyFilter_.clear();
    curDisplayNode_->ResetMirroredDisplayChangedFlag();
    PrepareForMultiScreenViewDisplayNode(node);
    node.ResetDirtyFlag();
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
    if (curDisplayNode_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::CheckMergeFilterDirtyByIntersectWithDirty: curDisplayNode_ is nullptr");
        return;
    }
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

void RSUniRenderVisitor::PrepareForSkippedCrossNode(RSSurfaceRenderNode& surfaceNode)
{
    // 1. Find the actual surface node of which the coordinates must be recalculated.
    auto sourceNode = surfaceNode.GetSourceCrossNode().lock();
    auto sourceSurface = sourceNode ? sourceNode->ReinterpretCastTo<RSSurfaceRenderNode>() : nullptr;
    RSSurfaceRenderNode& nodeRef = sourceSurface ? *sourceSurface : surfaceNode;
    auto geoPtr = nodeRef.GetRenderProperties().GetBoundsGeometry();
    if (!geoPtr) {
        return;
    }
    // 2. Get the AbsMatrix under current display node. Concatenate current parent matrix and nodeRef relative matrix.
    Drawing::Matrix nodeAbsMatrix = geoPtr->GetMatrix();
    if (nodeRef.GetGlobalPositionEnabled()) {
        nodeAbsMatrix.PostTranslate(-curDisplayNode_->GetDisplayOffsetX(), -curDisplayNode_->GetDisplayOffsetY());
    }
    if (auto parent = surfaceNode.GetParent().lock()) {
        auto parentGeoPtr = parent->GetRenderProperties().GetBoundsGeometry();
        nodeAbsMatrix.PostConcat(parentGeoPtr ? parentGeoPtr->GetAbsMatrix() : Drawing::Matrix());
    }
    // 3. Get the conversion matrix from first display coordinate system to current display coordinate system.
    // 3.1 Invert AbsMatrix under first display.
    // 3.2 Apply AbsMatrix under second display.
    Drawing::Matrix conversionMatrix;
    if (!geoPtr->GetAbsMatrix().Invert(conversionMatrix)) {
        curDisplayDirtyManager_->ResetDirtyAsSurfaceSize();
        RS_LOGW("Cross-Node matrix inverting for %{public}s failed, set full screen dirty.", nodeRef.GetName().c_str());
        return;
    }
    conversionMatrix.PostConcat(nodeAbsMatrix);

    // 4. record this surface node and its position on second display, for global dirty region conversion.
    curDisplayNode_->RecordMainAndLeashSurfaces(nodeRef.shared_from_this());
    nodeRef.UpdateCrossNodeSkipDisplayConversionMatrices(curDisplayNode_->GetId(), conversionMatrix);
    RectI surfaceRect =
        geoPtr->MapRect(nodeRef.GetOldDirty().ConvertTo<float>(), conversionMatrix).IntersectRect(screenRect_);
    curDisplayNode_->UpdateSurfaceNodePos(nodeRef.GetId(), surfaceRect);
    curDisplayNode_->AddSurfaceNodePosByDescZOrder(nodeRef.GetId(), surfaceRect);
    // 5. record all children surface nodes and their position on second display, for global dirty region conversion.
    std::vector<std::pair<NodeId, std::weak_ptr<RSSurfaceRenderNode>>> allSubSurfaceNodes;
    nodeRef.GetAllSubSurfaceNodes(allSubSurfaceNodes);
    for (auto& [_, subSurfaceNode] : allSubSurfaceNodes) {
        if (auto childPtr = subSurfaceNode.lock()) {
            curDisplayNode_->RecordMainAndLeashSurfaces(childPtr);
            childPtr->SetFirstLevelCrossNode(nodeRef.IsFirstLevelCrossNode());
            childPtr->UpdateCrossNodeSkipDisplayConversionMatrices(curDisplayNode_->GetId(), conversionMatrix);
            RectI childSurfaceRect = geoPtr->MapRect(
                childPtr->GetOldDirty().ConvertTo<float>(), conversionMatrix).IntersectRect(screenRect_);
            curDisplayNode_->UpdateSurfaceNodePos(childPtr->GetId(), childSurfaceRect);
            curDisplayNode_->AddSurfaceNodePosByDescZOrder(childPtr->GetId(), childSurfaceRect);
        }
    }
}

bool RSUniRenderVisitor::CheckSkipAndPrepareForCrossNode(RSSurfaceRenderNode& node)
{
    auto sourceRenderNode = node.GetSourceCrossNode().lock();
    RSSurfaceRenderNode::SharedPtr sourceNode = sourceRenderNode ?
        sourceRenderNode->ReinterpretCastTo<RSSurfaceRenderNode>() : nullptr;
    // CloneCrossNode must have a source node.
    if (node.IsCloneCrossNode() && sourceNode == nullptr) {
        RS_LOGE("RSUniRenderVisitor::QuickPrepareSurfaceRenderNode source node of clone node %{public}"
            PRIu64 " is null", node.GetId());
        return true;
    }

    // skip CrossNode not on first display
    if (CheckSkipCrossNode(node)) {
        // when skip cloneCrossNode prepare, we need switch to record sourceNode dirty info
        PrepareForSkippedCrossNode(node);
        return true;
    }

    // when prepare CloneCrossNode on first display, we need switch to prepare sourceNode.
    if (node.IsCloneCrossNode()) {
        isSwitchToSourceCrossNodePrepare_ = true;
        sourceNode->SetCurCloneNodeParent(node.GetParent().lock());
        sourceNode->SetFirstLevelCrossNode(true);
        sourceNode->QuickPrepare(shared_from_this());
        sourceNode->SetCurCloneNodeParent(nullptr);
        isSwitchToSourceCrossNodePrepare_ = false;
        return true;
    }
    return false;
}

void RSUniRenderVisitor::QuickPrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    UpdateCurFrameInfoDetail(node);
    RS_TRACE_NAME_FMT("RSUniRender::QuickPrepare:[%s] nodeId[%" PRIu64 "] pid[%d] nodeType[%u]"
        " subTreeDirty[%d], crossDisplay:[%d]", node.GetName().c_str(), node.GetId(), ExtractPid(node.GetId()),
        static_cast<uint>(node.GetSurfaceNodeType()), node.IsSubTreeDirty(), node.IsFirstLevelCrossNode());
    RS_LOGD("RSUniRender::QuickPrepareSurfaceRenderNode:[%{public}s] nodeid:[%{public}" PRIu64 "] pid:[%{public}d] "
        "nodeType:[%{public}d] subTreeDirty[%{public}d] crossDisplay[%{public}d]", node.GetName().c_str(), node.GetId(),
        ExtractPid(node.GetId()), static_cast<int>(node.GetSurfaceNodeType()), node.IsSubTreeDirty(),
        node.IsFirstLevelCrossNode());

    if (PrepareForCloneNode(node)) {
        return;
    }

    // AppWindow is traversed in reverse order
    // Prepare operation will be executed first when the node is at the top level
    // The value of appWindowZOrder_ decreases from 0 to negative
    if (node.IsAppWindow()) {
        node.SetAppWindowZOrder(appWindowZOrder_--);
    }

    // avoid cross node subtree visited twice or more
    UpdateSpecialLayersRecord(node);
    if (CheckSkipAndPrepareForCrossNode(node)) {
        return;
    }
    // 0. init curSurface info and check node info
    auto curCornerRadius = curCornerRadius_;
    auto parentSurfaceNodeMatrix = parentSurfaceNodeMatrix_;
    if (!BeforeUpdateSurfaceDirtyCalc(node)) {
        RS_LOGE("RSUniRenderVisitor::QuickPrepareSurfaceRenderNode BeforeUpdateSurfaceDirtyCalc fail");
        RSUifirstManager::Instance().DisableUifirstNode(node);
        node.GetOpincCache().OpincSetInAppStateEnd(unchangeMarkInApp_);
        return;
    }

    if (curSurfaceDirtyManager_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::QuickPrepareSurfaceRenderNode %{public}s curSurfaceDirtyManager is nullptr",
            node.GetName().c_str());
        node.GetOpincCache().OpincSetInAppStateEnd(unchangeMarkInApp_);
        return;
    }
    curSurfaceDirtyManager_->SetAdvancedDirtyRegionType(advancedDirtyType_);

    // 1. Update matrix and collect dirty region
    auto dirtyFlag = dirtyFlag_;
    auto prepareClipRect = prepareClipRect_;
    bool hasAccumulatedClip = hasAccumulatedClip_;
    auto prevAlpha = curAlpha_;
    curAlpha_ *= std::clamp(node.GetRenderProperties().GetAlpha(), 0.f, 1.f);
    node.SetGlobalAlpha(curAlpha_);
    auto preGlobalShouldPaint = globalShouldPaint_;
    globalShouldPaint_ &= node.ShouldPaint();
    node.SetSelfAndParentShouldPaint(globalShouldPaint_);
    CheckFilterCacheNeedForceClearOrSave(node);
    MarkFilterInForegroundFilterAndCheckNeedForceClearCache(node);
    node.CheckContainerDirtyStatusAndUpdateDirty(curContainerDirty_);
    node.ClearCrossNodeSkipDisplayConversionMatrices();
    node.SetPreparedDisplayOffsetX(curDisplayNode_->GetDisplayOffsetX());
    node.SetPreparedDisplayOffsetY(curDisplayNode_->GetDisplayOffsetY());
    if (node.GetGlobalPositionEnabled()) {
        parentSurfaceNodeMatrix_.Translate(
            -curDisplayNode_->GetDisplayOffsetX(), -curDisplayNode_->GetDisplayOffsetY());
    }

    dirtyFlag_ = node.UpdateDrawRectAndDirtyRegion(
        *curSurfaceDirtyManager_, dirtyFlag_, prepareClipRect_, parentSurfaceNodeMatrix);
    auto& geoPtr = node.GetRenderProperties().GetBoundsGeometry();
    if (!geoPtr) {
        node.GetOpincCache().OpincSetInAppStateEnd(unchangeMarkInApp_);
        globalShouldPaint_ = preGlobalShouldPaint;
        return;
    }
    parentSurfaceNodeMatrix_ = geoPtr->GetAbsMatrix();
    if (!AfterUpdateSurfaceDirtyCalc(node)) {
        RS_LOGE("RSUniRenderVisitor::QuickPrepareSurfaceRenderNode AfterUpdateSurfaceDirtyCalc fail");
        RSUifirstManager::Instance().DisableUifirstNode(node);
        node.GetOpincCache().OpincSetInAppStateEnd(unchangeMarkInApp_);
        globalShouldPaint_ = preGlobalShouldPaint;
        return;
    }
    bool isDimmingOn = RSLuminanceControl::Get().IsDimmingOn(curDisplayNode_->GetScreenId());
    if (isDimmingOn) {
        bool hasHdrPresent = node.GetHDRPresent();
        bool hasHdrVideo = node.GetVideoHdrStatus() != HdrStatus::NO_HDR;
        RS_LOGD("HDRDiming IsDimmingOn: %{public}d, GetHDRPresent: %{public}d, GetHdrVideo: %{public}d",
            isDimmingOn, hasHdrPresent, hasHdrVideo);
        if (hasHdrPresent || hasHdrVideo) {
            node.SetContentDirty(); // HDR content is dirty on Dimming status.
        }
    }
    CollectSelfDrawingNodeRectInfo(node);
    hasAccumulatedClip_ = node.SetAccumulatedClipFlag(hasAccumulatedClip_);
    bool isSubTreeNeedPrepare = node.IsSubTreeNeedPrepare(filterInGlobal_, IsSubTreeOccluded(node)) ||
        ForcePrepareSubTree();
    isSubTreeNeedPrepare ? QuickPrepareChildren(node) :
        node.SubTreeSkipPrepare(*curSurfaceDirtyManager_, curDirty_, dirtyFlag_, prepareClipRect_);
    if (curSurfaceDirtyManager_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::QuickPrepareSurfaceRenderNode %{public}s curSurfaceDirtyManager "
            "is set to nullptr by QuickPrepareChildren", node.GetName().c_str());
        node.GetOpincCache().OpincSetInAppStateEnd(unchangeMarkInApp_);
        globalShouldPaint_ = preGlobalShouldPaint;
        return;
    }
    if (!node.IsFirstLevelCrossNode()) {
        curSurfaceDirtyManager_->ClipDirtyRectWithinSurface();
    }
    auto dirtyRect = node.GetDirtyManager()->GetCurrentFrameDirtyRegion();
    RS_LOGD("QuickPrepare [%{public}s, %{public}" PRIu64 "] DirtyRect[%{public}d, %{public}d, %{public}d, %{public}d]",
        node.GetName().c_str(), node.GetId(), dirtyRect.GetLeft(), dirtyRect.GetTop(), dirtyRect.GetWidth(),
        dirtyRect.GetHeight());

    RSUifirstManager::Instance().RecordDirtyRegionMatrix(node, geoPtr->GetAbsMatrix());
    // Update whether leash window's visible region is empty after prepare children
    UpdateLeashWindowVisibleRegionEmpty(node);

    if (node.IsLeashWindow() && RSSystemProperties::GetGpuOverDrawBufferOptimizeEnabled()) {
        CheckIsGpuOverDrawBufferOptimizeNode(node);
    }
    PostPrepare(node, !isSubTreeNeedPrepare);
    if (node.IsHardwareEnabledTopSurface() && node.shared_from_this()) {
        RSUniHwcComputeUtil::UpdateHwcNodeProperty(node.shared_from_this()->ReinterpretCastTo<RSSurfaceRenderNode>());
    }
    CalculateOpaqueAndTransparentRegion(node);
    CheckMergeFilterDirtyByIntersectWithDirty(curSurfaceNoBelowDirtyFilter_, false);
    curAlpha_ = prevAlpha;
    globalShouldPaint_ = preGlobalShouldPaint;
    prepareClipRect_ = prepareClipRect;
    hasAccumulatedClip_ = hasAccumulatedClip;
    dirtyFlag_ = dirtyFlag;
    PrepareForUIFirstNode(node);
    PrepareForCrossNode(node);
    node.UpdateInfoForClonedNode(clonedSourceNodeId_);
    node.GetOpincCache().OpincSetInAppStateEnd(unchangeMarkInApp_);
    ResetCurSurfaceInfoAsUpperSurfaceParent(node);
    curCornerRadius_ = curCornerRadius;
    parentSurfaceNodeMatrix_ = parentSurfaceNodeMatrix;

    // [Attention] Only used in PC window resize scene now
    windowKeyFrameNodeInf_.PrepareRootNodeOffscreen(node);

    node.RenderTraceDebug();
    node.SetNeedOffscreen(isScreenRotationAnimating_);
    if (node.NeedUpdateDrawableBehindWindow()) {
        node.UpdateDrawableBehindWindow();
        node.SetOldNeedDrawBehindWindow(node.NeedDrawBehindWindow());
    }
    if (node.IsUIBufferAvailable()) {
        uiBufferAvailableId_.emplace_back(node.GetId());
    }
    PrepareForMultiScreenViewSurfaceNode(node);
}

void RSUniRenderVisitor::PrepareForMultiScreenViewSurfaceNode(RSSurfaceRenderNode& node)
{
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    auto sourceNode = nodeMap.GetRenderNode<RSDisplayRenderNode>(node.GetSourceDisplayRenderNodeId());
    if (!sourceNode) {
        return;
    }
    node.SetContentDirty();
    sourceNode->SetTargetSurfaceRenderNodeId(node.GetId());
    auto sourceNodeDrawable = sourceNode->GetRenderDrawable();
    if (!sourceNodeDrawable) {
        return;
    }
    RS_TRACE_NAME_FMT("PrepareForMultiScreenViewSurfaceNode surfaceNodeId: %llu sourceVirtualDisplayId: %llu",
        node.GetId(), node.GetSourceDisplayRenderNodeId());
    node.SetSourceDisplayRenderNodeDrawable(sourceNodeDrawable);
}

void RSUniRenderVisitor::PrepareForMultiScreenViewDisplayNode(RSDisplayRenderNode& node)
{
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    auto targetNode = nodeMap.GetRenderNode<RSSurfaceRenderNode>(node.GetTargetSurfaceRenderNodeId());
    if (!targetNode) {
        return;
    }
    auto targetRenderNodeDrawable = targetNode->GetRenderDrawable();
    if (!targetRenderNodeDrawable) {
        return;
    }
    RS_TRACE_NAME_FMT("PrepareForMultiScreenViewDisplayNode displayNodeId: %llu targetSurfaceRenderNodeId: %llu",
        node.GetId(), node.GetTargetSurfaceRenderNodeId());
    node.SetTargetSurfaceRenderNodeDrawable(targetRenderNodeDrawable);
}

bool RSUniRenderVisitor::PrepareForCloneNode(RSSurfaceRenderNode& node)
{
    if (!node.IsCloneNode() || !node.IsLeashOrMainWindow()) {
        return false;
    }
    RS_LOGD("RSUniRenderVisitor::PrepareForCloneNode %{public}s is cloneNode", node.GetName().c_str());
    node.SetIsClonedNodeOnTheTree(false);
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    auto clonedNode = nodeMap.GetRenderNode<RSSurfaceRenderNode>(node.GetClonedNodeId());
    if (clonedNode == nullptr) {
        RS_LOGE("RSUniRenderVisitor::PrepareForCloneNode clonedNode is nullptr");
        return false;
    }
    if (!clonedNode->IsMainWindowType()) {
        return false;
    }
    auto clonedNodeRenderDrawable = clonedNode->GetRenderDrawable();
    if (clonedNodeRenderDrawable == nullptr) {
        RS_LOGE("RSUniRenderVisitor::PrepareForCloneNode clonedNodeRenderDrawable is nullptr");
        return false;
    }
    node.SetIsClonedNodeOnTheTree(clonedNode->IsOnTheTree());
    clonedSourceNodeId_ = node.GetClonedNodeId();
    node.SetClonedNodeRenderDrawable(clonedNodeRenderDrawable);
    node.UpdateRenderParams();
    node.AddToPendingSyncList();
    return true;
}

void RSUniRenderVisitor::PrepareForCrossNode(RSSurfaceRenderNode& node)
{
    if (isCrossNodeOffscreenOn_ == CrossNodeOffScreenRenderDebugType::DISABLED) {
        return;
    }
    if (curDisplayNode_ == nullptr) {
        RS_LOGE("%{public}s curDisplayNode_ is nullptr", __func__);
        return;
    }
    if (curDisplayNode_->IsFirstVisitCrossNodeDisplay() && node.IsCrossNode()) {
        // check surface cache condition, not support ratation, transparent filter situation.
        bool needCacheSurface = node.QuerySubAssignable(curDisplayNode_->IsRotationChanged());
        if (needCacheSurface) {
            node.SetHwcChildrenDisabledState();
            RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " children disabled by crossNode",
                node.GetName().c_str(), node.GetId());
        }
        node.SetNeedCacheSurface(needCacheSurface);
    }
}

bool RSUniRenderVisitor::CheckSkipCrossNode(RSSurfaceRenderNode& node)
{
    if (!node.IsCrossNode() || isSwitchToSourceCrossNodePrepare_) {
        return false;
    }
    if (curDisplayNode_ == nullptr) {
        RS_LOGE("%{public}s curDisplayNode_ is nullptr", __func__);
        return false;
    }
    node.SetCrossNodeOffScreenStatus(isCrossNodeOffscreenOn_);
    curDisplayNode_->SetHasChildCrossNode(true);
    if (node.HasVisitedCrossNode()) {
        RS_OPTIONAL_TRACE_NAME_FMT("%s cross node[%s] skip", __func__, node.GetName().c_str());
        return true;
    }
    curDisplayNode_->SetIsFirstVisitCrossNodeDisplay(true);
    node.SetCrossNodeVisitedStatus(true);
    // If there are n source cross screen nodes, they will enter n times. size of hasVisitedCrossNodeIds_.() is n.
    hasVisitedCrossNodeIds_.push_back(node.GetId());
    return false;
}

void RSUniRenderVisitor::ResetCrossNodesVisitedStatus()
{
    if (hasVisitedCrossNodeIds_.empty()) {
        return;
    }
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (NodeId nodeId : hasVisitedCrossNodeIds_) {
        auto visitedNode = nodeMap.GetRenderNode<RSSurfaceRenderNode>(nodeId);
        if (!visitedNode) {
            RS_LOGE("%{public}s visitedNode is nullptr NodeId[%{public}" PRIu64 "]", __func__, nodeId);
            continue;
        }
        RS_LOGD("%{public}s NodeId[%{public}" PRIu64 "]", __func__, nodeId);
        visitedNode->SetCrossNodeVisitedStatus(false);
    }
    hasVisitedCrossNodeIds_.clear();
}

Occlusion::Region RSUniRenderVisitor::GetSurfaceTransparentFilterRegion(NodeId surfaceNodeId) const
{
    Occlusion::Region surfaceTransparentFilterRegion;
    auto transparentCleanFilterIt = transparentCleanFilter_.find(surfaceNodeId);
    if (transparentCleanFilterIt != transparentCleanFilter_.end()) {
        for (auto it = transparentCleanFilterIt->second.begin(); it != transparentCleanFilterIt->second.end(); ++it) {
            surfaceTransparentFilterRegion.OrSelf(Occlusion::Region(it->second));
        }
    }
    auto transparentDirtyFilterIt = transparentDirtyFilter_.find(surfaceNodeId);
    if (transparentDirtyFilterIt != transparentDirtyFilter_.end()) {
        for (auto it = transparentDirtyFilterIt->second.begin(); it != transparentDirtyFilterIt->second.end(); ++it) {
            surfaceTransparentFilterRegion.OrSelf(Occlusion::Region(it->second));
        }
    }
    return surfaceTransparentFilterRegion;
}

void RSUniRenderVisitor::CollectTopOcclusionSurfacesInfo(RSSurfaceRenderNode& node, bool isParticipateInOcclusion)
{
    if (!isStencilPixelOcclusionCullingEnabled_ || occlusionSurfaceOrder_ <= DEFAULT_OCCLUSION_SURFACE_ORDER) {
        return;
    }
    if (curDisplayNode_ == nullptr) {
        RS_LOGE("%s: curDisplayNode_ is nullptr", __func__);
        return;
    }
    auto parent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.GetParent().lock());
    if (!parent || !parent->IsLeashWindow()) {
        return;
    }
    auto stencilVal = occlusionSurfaceOrder_ * OCCLUSION_ENABLE_SCENE_NUM;
    auto opaqueRegionRects = node.GetOpaqueRegion().GetRegionRects();
    if (!isParticipateInOcclusion || opaqueRegionRects.empty()) {
        ++stencilVal;
        RS_OPTIONAL_TRACE_NAME_FMT("%s: %s set stencil[%d]", __func__,
            node.GetName().c_str(), static_cast<int>(stencilVal));
        parent->SetStencilVal(stencilVal);
        if (occlusionSurfaceOrder_ < TOP_OCCLUSION_SURFACES_NUM &&
            !GetSurfaceTransparentFilterRegion(node.GetId()).And(
                curDisplayNode_->GetTopSurfaceOpaqueRegion()).IsEmpty()) {
            RS_OPTIONAL_TRACE_NAME_FMT("%s: %s's transparent filter terminate spoc, current occlusion surface "
                "order: %" PRId16 "", __func__, node.GetName().c_str(), occlusionSurfaceOrder_);
            occlusionSurfaceOrder_ = DEFAULT_OCCLUSION_SURFACE_ORDER;
        }
        return;
    }
    parent->SetStencilVal(stencilVal);
    if (occlusionSurfaceOrder_ > 0) {
        auto maxOpaqueRect = std::max_element(opaqueRegionRects.begin(), opaqueRegionRects.end(),
            [](Occlusion::Rect a, Occlusion::Rect b) ->bool { return a.Area() < b.Area(); });
        RS_OPTIONAL_TRACE_NAME_FMT("%s: record name[%s] rect[%s] stencil[%d]", __func__,
            node.GetName().c_str(), maxOpaqueRect->GetRectInfo().c_str(), static_cast<int>(stencilVal));
        curDisplayNode_->RecordTopSurfaceOpaqueRects(*maxOpaqueRect);
        --occlusionSurfaceOrder_;
    }
}

CM_INLINE void RSUniRenderVisitor::PrepareForUIFirstNode(RSSurfaceRenderNode& node)
{
    RSUifirstManager::Instance().MarkSubHighPriorityType(node);
    auto isSurface = CheckIfSurfaceForUIFirstDFX(node.GetName());
    if (isTargetUIFirstDfxEnabled_) {
        auto isTargetUIFirstDfxSurface = CheckIfSurfaceForUIFirstDFX(node.GetName());
        if (!node.isTargetUIFirstDfxEnabled_ && isTargetUIFirstDfxSurface) {
            RS_LOGD("UIFirstDFX Name[%{public}s] ID[%{public}" PRIu64 "] OpenDebug",
                node.GetName().c_str(), node.GetId());
        }
        node.isTargetUIFirstDfxEnabled_ = isTargetUIFirstDfxSurface;
    }
    RSUifirstManager::Instance().UpdateUifirstNodes(node, ancestorNodeHasAnimation_ || node.GetCurFrameHasAnimation());
    RSUifirstManager::Instance().RecordScreenRect(node, screenRect_);
}

void RSUniRenderVisitor::UpdateNodeVisibleRegion(RSSurfaceRenderNode& node)
{
    if (!curDisplayNode_) {
        RS_LOGE("RSUniRenderVisitor::UpdateNodeVisibleRegion curDisplayNode is nullptr");
        return;
    }
    // occlusion - 0. Calculate node visible region considering accumulated opaque region of upper surface nodes.
    if (!curDisplayNode_->IsFirstVisitCrossNodeDisplay() && node.IsFirstLevelCrossNode()) {
        RS_LOGD("RSUniRenderVisitor::UpdateNodeVisibleRegion NodeName: %{public}s, NodeId: %{public}" PRIu64 ""
            "not paticipate in occlusion when cross node in expand screen", node.GetName().c_str(), node.GetId());
        return;
    }
    Occlusion::Rect selfDrawRect = node.GetSurfaceOcclusionRect(true);
    Occlusion::Region selfDrawRegion { selfDrawRect };
    needRecalculateOcclusion_ = needRecalculateOcclusion_ || node.CheckIfOcclusionChanged();
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

CM_INLINE void RSUniRenderVisitor::CalculateOpaqueAndTransparentRegion(RSSurfaceRenderNode& node)
{
    if (!curDisplayNode_) {
        RS_LOGE("RSUniRenderVisitor::CalculateOpaqueAndTransparentRegion curDisplayNode is nullptr");
        return;
    }
    // occlusion - 1. Only non-cross-screen main window surface node participate in occlusion.
    if ((!curDisplayNode_->IsFirstVisitCrossNodeDisplay() && node.IsFirstLevelCrossNode()) ||
        !node.IsMainWindowType()) {
        RS_LOGD("RSUniRenderVisitor::CalculateOpaqueAndTransparentRegion Node: %{public}s, NodeId: %{public}" PRIu64 ""
            "not paticipate in occlusion", node.GetName().c_str(), node.GetId());
        return;
    }

    // occlusion - 2. Calculate opaque/transparent region based on round corner, container window, etc.
    auto parent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.GetParent().lock());
    auto isFocused = node.IsFocusedNode(currentFocusedNodeId_) ||
        (parent && parent->IsLeashWindow() && parent->IsFocusedNode(focusedLeashWindowId_));
    node.CheckAndUpdateOpaqueRegion(screenRect_, curDisplayNode_->GetRotation(), isFocused);
    // occlusion - 3. Accumulate opaque region to occlude lower surface nodes (with/without special layer).
    hasSkipLayer_ = hasSkipLayer_ || node.GetSpecialLayerMgr().Find(SpecialLayerType::SKIP);
    auto mainThread = RSMainThread::Instance();
    node.SetOcclusionInSpecificScenes(mainThread->GetIsRegularAnimation());
    bool occlusionInAnimation = node.GetOcclusionInSpecificScenes() || !ancestorNodeHasAnimation_;
    bool isParticipateInOcclusion = node.CheckParticipateInOcclusion() &&
        occlusionInAnimation && !isAllSurfaceVisibleDebugEnabled_;
    CollectTopOcclusionSurfacesInfo(node, isParticipateInOcclusion);
    if (isParticipateInOcclusion) {
        RS_OPTIONAL_TRACE_NAME_FMT("Occlusion: surface node[%s] participate in occlusion with opaque region: [%s]",
            node.GetName().c_str(), node.GetOpaqueRegion().GetRegionInfo().c_str());
        accumulatedOcclusionRegion_.OrSelf(node.GetOpaqueRegion());
        if (IsValidInVirtualScreen(node)) {
            occlusionRegionWithoutSkipLayer_.OrSelf(node.GetOpaqueRegion());
        }
    }
    needRecalculateOcclusion_ = needRecalculateOcclusion_ || node.CheckIfOcclusionChanged();
    node.SetOcclusionInSpecificScenes(false);
    CollectOcclusionInfoForWMS(node);
    RSMainThread::Instance()->GetRSVsyncRateReduceManager().CollectSurfaceVsyncInfo(screenInfo_, node);
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
        RS_LOGI("OcclusionInfo %{public}s", VisibleDataToString(allDstCurVisVec_).c_str());
        allLastVisVec_ = std::move(allDstCurVisVec_);
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

void RSUniRenderVisitor::QuickPrepareEffectRenderNode(RSEffectRenderNode& node)
{
    UpdateCurFrameInfoDetail(node);
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
    MarkFilterInForegroundFilterAndCheckNeedForceClearCache(node);
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

void RSUniRenderVisitor::MarkFilterInForegroundFilterAndCheckNeedForceClearCache(RSRenderNode& node)
{
    node.MarkFilterInForegroundFilterAndCheckNeedForceClearCache(inForegroundFilter_);
}

void RSUniRenderVisitor::UpdateDrawingCacheInfoBeforeChildren(RSCanvasRenderNode& node)
{
    if (!isDrawingCacheEnabled_) {
        return;
    }
    node.UpdateDrawingCacheInfoBeforeChildren(isScreenRotationAnimating_);
    if (node.GetDrawingCacheType() == RSDrawingCacheType::FOREGROUND_FILTER_CACHE) {
        inForegroundFilter_ = true;
    }
}

void RSUniRenderVisitor::QuickPrepareCanvasRenderNode(RSCanvasRenderNode& node)
{
    UpdateCurFrameInfoDetail(node);
    // 0. check current node need to traverse
    auto dirtyManager = curSurfaceNode_ ? curSurfaceDirtyManager_ : curDisplayDirtyManager_;
    auto dirtyFlag = dirtyFlag_;
    auto prevAlpha = curAlpha_;
    auto curCornerRadius = curCornerRadius_;
    curAlpha_ *= std::clamp(node.GetRenderProperties().GetAlpha(), 0.f, 1.f);
    auto preGlobalShouldPaint = globalShouldPaint_;
    auto preInForegroundFilter = inForegroundFilter_;
    globalShouldPaint_ &= node.ShouldPaint();
    CheckFilterCacheNeedForceClearOrSave(node);
    if (IsAccessibilityConfigChanged()) {
        node.SetIsAccessibilityConfigChanged(true);
    }
    UpdateDrawingCacheInfoBeforeChildren(node);
    MarkFilterInForegroundFilterAndCheckNeedForceClearCache(node);
    node.SetIsAccessibilityConfigChanged(false);
    // The opinc state needs to be reset in the following cases:
    // 1. subtree is dirty
    // 2. content is dirty
    // 3. the node is marked as a node group
    // 4. the node is marked as a root node of opinc and the high-contrast state change
    auto isSelfDirty = node.IsSubTreeDirty() || node.IsContentDirty() ||
        node.GetNodeGroupType() > RSRenderNode::NodeGroupType::NONE ||
        (node.GetOpincCache().OpincGetRootFlag() && IsAccessibilityConfigChanged());
    node.GetOpincCache().OpincQuickMarkStableNode(unchangeMarkInApp_, unchangeMarkEnable_, isSelfDirty);
    node.UpdateOpincParam();
    RectI prepareClipRect = prepareClipRect_;
    bool hasAccumulatedClip = hasAccumulatedClip_;

    if (!dirtyManager) {
        return;
    }
    dirtyFlag_ =
        node.UpdateDrawRectAndDirtyRegion(*dirtyManager, dirtyFlag_, prepareClipRect_, parentSurfaceNodeMatrix_);

    // update prepare clip before children
    hwcVisitor_->UpdatePrepareClip(node);
    node.UpdateCurCornerRadius(curCornerRadius_);

    bool isCurrOffscreen = hwcVisitor_->UpdateIsOffscreen(node);

    // 1. Recursively traverse child nodes if above curSurfaceNode and subnode need draw
    hasAccumulatedClip_ = node.SetAccumulatedClipFlag(hasAccumulatedClip_);
    auto subTreeDirty = node.GetOpincCache().OpincForcePrepareSubTree(autoCacheEnable_,
        node.IsSubTreeDirty() || node.IsContentDirty(), node.OpincGetNodeSupportFlag());
    if (subTreeDirty) {
        node.SetParentSubTreeDirty();
    }
    bool isSubTreeNeedPrepare = !curSurfaceNode_ || node.IsSubTreeNeedPrepare(filterInGlobal_) ||
        ForcePrepareSubTree() || subTreeDirty;
    isSubTreeNeedPrepare ? QuickPrepareChildren(node) :
        node.SubTreeSkipPrepare(*dirtyManager, curDirty_, dirtyFlag_, prepareClipRect_);

    PostPrepare(node, !isSubTreeNeedPrepare);
    prepareClipRect_ = prepareClipRect;
    hasAccumulatedClip_ = hasAccumulatedClip;
    dirtyFlag_ = dirtyFlag;
    curAlpha_ = prevAlpha;
    curCornerRadius_ = curCornerRadius;
    node.GetOpincCache().OpincUpdateRootFlag(unchangeMarkEnable_, node.OpincGetNodeSupportFlag());
    node.UpdateOpincParam();
    globalShouldPaint_ = preGlobalShouldPaint;
    inForegroundFilter_ = preInForegroundFilter;

    hwcVisitor_->RestoreIsOffscreen(isCurrOffscreen);

    // [Attention] Only used in PC window resize scene now
    NodeId linedRootNodeId = node.GetLinkedRootNodeId();
    if (UNLIKELY(linedRootNodeId != INVALID_NODEID)) {
        windowKeyFrameNodeInf_.UpdateLinkedNodeId(node.GetId(), linedRootNodeId);
    }

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
    // Collect prepared node into the control-level occlusion culling handler.
    CollectNodeForOcclusion(node);
    bool animationBackup = ancestorNodeHasAnimation_;
    ancestorNodeHasAnimation_ = ancestorNodeHasAnimation_ || node.GetCurFrameHasAnimation();
    node.ResetChildRelevantFlags();
    node.ResetChildUifirstSupportFlag();
    auto children = node.GetSortedChildren();
    if (NeedPrepareChindrenInReverseOrder(node)) {
        auto& curFrameInfoDetail = node.GetCurFrameInfoDetail();
        curFrameInfoDetail.curFrameReverseChildren = true;
        RS_LOGD("RSUniRenderVisitor::%{public}s NeedPrepareChindrenInReverseOrder nodeId: %{public}" PRIu64,
            __func__, node.GetId());
        std::for_each(
            (*children).rbegin(), (*children).rend(), [this, &node](const std::shared_ptr<RSRenderNode>& child) {
            if (!child) {
                return;
            }
            rsDisplayNodeChildNum_++;
            auto containerDirty = curContainerDirty_;
            curDirty_ = child->IsDirty();
            curContainerDirty_ = curContainerDirty_ || child->IsDirty();
            child->SetFirstLevelCrossNode(node.IsFirstLevelCrossNode() || child->IsCrossNode());
            child->GetHwcRecorder().SetZOrderForHwcEnableByFilter(hwcVisitor_->curZOrderForHwcEnableByFilter_++);
            child->QuickPrepare(shared_from_this());
            curContainerDirty_ = containerDirty;
        });
    } else {
        auto& curFrameInfoDetail = node.GetCurFrameInfoDetail();
        curFrameInfoDetail.curFrameReverseChildren = false;
        std::for_each(
            (*children).begin(), (*children).end(), [this, &node](const std::shared_ptr<RSRenderNode>& child) {
            if (!child) {
                return;
            }
            rsDisplayNodeChildNum_++;
            curDirty_ = child->IsDirty();
            child->SetFirstLevelCrossNode(node.IsFirstLevelCrossNode() || child->IsCrossNode());
            child->GetHwcRecorder().SetZOrderForHwcEnableByFilter(hwcVisitor_->curZOrderForHwcEnableByFilter_++);
            child->QuickPrepare(shared_from_this());
        });
    }
    ancestorNodeHasAnimation_ = animationBackup;
    node.ResetGeoUpdateDelay();
}

// Used to collect prepared node into the control-level occlusion culling handler.
inline void RSUniRenderVisitor::CollectNodeForOcclusion(RSRenderNode& node)
{
    if (curOcclusionHandler_) {
        curOcclusionHandler_->CollectNode(node);
    }
}

bool RSUniRenderVisitor::InitDisplayInfo(RSDisplayRenderNode& node)
{
    // 1 init curDisplay and curDisplayDirtyManager
    currentVisitDisplay_ = node.GetScreenId();
    displaySpecailSurfaceChanged_.emplace(currentVisitDisplay_, false);
    hasCaptureWindow_.emplace(currentVisitDisplay_, false);
    hasFingerprint_.emplace(currentVisitDisplay_, false);
    curDisplayDirtyManager_ = node.GetDirtyManager();
    curDisplayNode_ = node.shared_from_this()->ReinterpretCastTo<RSDisplayRenderNode>();
    if (!curDisplayDirtyManager_ || !curDisplayNode_) {
        RS_LOGE("RSUniRenderVisitor::InitDisplayInfo dirtyMgr or node ptr is nullptr");
        return false;
    }
    curDisplayDirtyManager_->SetAdvancedDirtyRegionType(advancedDirtyType_);
    curDisplayNode_->GetMultableSpecialLayerMgr().Set(HAS_GENERAL_SPECIAL, false);
    curDisplayDirtyManager_->Clear();
    transparentCleanFilter_.clear();
    transparentDirtyFilter_.clear();
    hwcVisitor_->transparentHwcCleanFilter_.clear();
    hwcVisitor_->transparentHwcDirtyFilter_.clear();
    occlusionSurfaceOrder_ = TOP_OCCLUSION_SURFACES_NUM;
    node.SetHasChildCrossNode(false);
    node.SetIsFirstVisitCrossNodeDisplay(false);
    node.SetHasUniRenderHdrSurface(false);
    node.SetIsLuminanceStatusChange(false);
    CheckLuminanceStatusChange(curDisplayNode_->GetScreenId());
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
    screenManager_->SetScreenHasProtectedLayer(currentVisitDisplay_, false);
    allBlackList_ = screenManager_->GetAllBlackList();
    allWhiteList_ = screenManager_->GetAllWhiteList();

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
        hwcVisitor_->isHardwareForcedDisabled_ = true;
        RS_OPTIONAL_TRACE_FMT("hwc debug: id:%" PRIu64 " disabled by displayNode rotation", node.GetId());
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

    // init hdr and color gamut info
    node.ResetDisplayHdrStatus();
    node.SetPixelFormat(GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888);
    node.SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);

    // [Attention] Only used in PC window resize scene now
    windowKeyFrameNodeInf_.ClearLinkedNodeInfo();

    return true;
}

CM_INLINE bool RSUniRenderVisitor::BeforeUpdateSurfaceDirtyCalc(RSSurfaceRenderNode& node)
{
    // 1. init and record surface info
    if (node.GetName().find(CAPTURE_WINDOW_NAME) != std::string::npos) {
        hasCaptureWindow_[currentVisitDisplay_] = true;
    }
    node.UpdateUIFirstFrameGravity();
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        node.SetStencilVal(Drawing::Canvas::INVALID_STENCIL_VAL);
        // UpdateCurCornerRadius must process before curSurfaceNode_ update
        node.UpdateCurCornerRadius(curCornerRadius_);
        curSurfaceNode_ = node.ReinterpretCastTo<RSSurfaceRenderNode>();
        // dirty manager should not be overrode by cross node in expand screen
        curSurfaceDirtyManager_ = (!curDisplayNode_->IsFirstVisitCrossNodeDisplay() && node.IsFirstLevelCrossNode()) ?
            std::make_shared<RSDirtyRegionManager>() : node.GetDirtyManager();
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
        curSurfaceNode_->UpdateSurfaceCacheContentStaticFlag(IsAccessibilityConfigChanged());
        curSurfaceNode_->UpdateSurfaceSubTreeDirtyFlag();
        curSurfaceNode_->SetLeashWindowVisibleRegionEmpty(false);
    } else if (node.IsAbilityComponent()) {
        if (auto nodePtr = node.ReinterpretCastTo<RSSurfaceRenderNode>()) {
            RSMainThread::Instance()->CheckAndUpdateInstanceContentStaticStatus(nodePtr);
            nodePtr->UpdateSurfaceCacheContentStaticFlag(IsAccessibilityConfigChanged());
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
        node.GetOpincCache().OpincSetInAppStateStart(unchangeMarkInApp_);
    }
    // 3. check pixelFormat and update RelMatrix
    CheckPixelFormat(node);
    if (node.GetRSSurfaceHandler() && node.GetRSSurfaceHandler()->GetBuffer()) {
        node.SetBufferRelMatrix(RSUniRenderUtil::GetMatrixOfBufferToRelRect(node));
    }
#ifdef RS_ENABLE_GPU
    // 4. collect cursors and check for null
    RSPointerWindowManager::Instance().CollectAllHardCursor(node, curDisplayNode_);
#endif
    node.setQosCal((RSMainThread::Instance()->GetDeviceType() == DeviceType::PC) &&
        RSSystemParameters::GetVSyncControlEnabled());
    return true;
}

CM_INLINE bool RSUniRenderVisitor::AfterUpdateSurfaceDirtyCalc(RSSurfaceRenderNode& node)
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
    if (node.GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED)) {
        auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.GetFirstLevelNode());
        if (firstLevelNode) {
            node.SetHwcGlobalPositionEnabled(firstLevelNode->GetGlobalPositionEnabled());
            node.SetHwcCrossNode(firstLevelNode->IsFirstLevelCrossNode());
        }
    }
    UpdateAncoPrepareClip(node);
    hwcVisitor_->UpdateDstRect(node, geoPtr->GetAbsRect(), prepareClipRect_);
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
        UpdateNodeVisibleRegion(node);
    }
    // 3. Update HwcNode Info for appNode
    UpdateHwcNodeInfoForAppNode(node);
    if (node.IsHardwareEnabledTopSurface()) {
        hwcVisitor_->UpdateTopSurfaceSrcRect(node, geoPtr->GetAbsMatrix(), geoPtr->GetAbsRect());
        RSPointerWindowManager::CheckHardCursorValid(node);
    }
    // 4. Update color gamut for appNode
    if (!RsCommonHook::Instance().GetIsCoveredSurfaceCloseP3() ||
        !node.GetVisibleRegion().IsEmpty() || !GetIsOpDropped()) {
        CheckColorSpace(node);
    }
    return true;
}

void RSUniRenderVisitor::UpdateLeashWindowVisibleRegionEmpty(RSSurfaceRenderNode& node)
{
    if (!node.IsLeashWindow() || !RSSystemParameters::GetUIFirstOcclusionEnabled()) {
        return;
    }

    auto dirtyManager = node.GetDirtyManager();
    if (dirtyManager && dirtyManager->IsCurrentFrameDirty()) {
        RS_LOGD("RSUniRenderVisitor::UpdateLeashWindowVisibleRegionEmpty[false] : %{public}s is current frame dirty.",
            node.GetName().c_str());
        node.SetLeashWindowVisibleRegionEmpty(false);
        return;
    }

    auto sortedChildren = node.GetSortedChildren();
    if (sortedChildren == nullptr || sortedChildren->empty()) {
        RS_TRACE_NAME_FMT("%s don't have children", node.GetName().c_str());
        node.SetLeashWindowVisibleRegionEmpty(true);
        return;
    }

    // leash window's visible region is empty when all child are app windows with empty visible region
    for (const auto& child : *sortedChildren) {
        const auto childSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
        if (!childSurfaceNode || !childSurfaceNode->IsAppWindow() ||
            !childSurfaceNode->GetVisibleRegion().IsEmpty()) {
            node.SetLeashWindowVisibleRegionEmpty(false);
            return;
        }
    }
    RS_TRACE_NAME_FMT("visible region of %s's children is empty", node.GetName().c_str());
    node.SetLeashWindowVisibleRegionEmpty(true);
}

void RSUniRenderVisitor::UpdateHwcNodeInfoForAppNode(RSSurfaceRenderNode& node)
{
    // app node
    if (node.GetNeedCollectHwcNode()) {
        node.ResetChildHardwareEnabledNodes();
        node.SetExistTransparentHardwareEnabledNode(false);
    }
    // hwc node
    if (node.IsHardwareEnabledType() && curSurfaceNode_) {
        if (curSurfaceNode_->GetNeedCollectHwcNode()) {
            curSurfaceNode_->AddChildHardwareEnabledNode(node.ReinterpretCastTo<RSSurfaceRenderNode>());
        }
        auto& geo = node.GetRenderProperties().GetBoundsGeometry();
        if (geo == nullptr) {
            return;
        }
        hwcVisitor_->UpdateHwcNodeInfo(node, geo->GetAbsMatrix());
    }
}

void RSUniRenderVisitor::ProcessAncoNode(std::shared_ptr<RSSurfaceRenderNode>& hwcNodePtr, bool& ancoHasGpu)
{
    if (hwcNodePtr == nullptr) {
        return;
    }
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
    if (hwcNodePtr->IsHardwareForcedDisabled()) {
        RS_OPTIONAL_TRACE_NAME_FMT("ProcessAncoNode: name:%s id:%" PRIu64 " hardware force disabled",
            hwcNodePtr->GetName().c_str(), hwcNodePtr->GetId());
    }
    ancoHasGpu = (ancoHasGpu || hwcNodePtr->IsHardwareForcedDisabled());
}

void RSUniRenderVisitor::UpdateAncoNodeHWCDisabledState(
    std::unordered_set<std::shared_ptr<RSSurfaceRenderNode>>& ancoNodes)
{
    bool ancoHasGpu = false;
    for (auto hwcNodePtr : ancoNodes) {
        ProcessAncoNode(hwcNodePtr, ancoHasGpu);
    }
    if (ancoHasGpu) {
        for (const auto& hwcNodePtr : ancoNodes) {
            RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by anco has gpu",
                hwcNodePtr->GetName().c_str(), hwcNodePtr->GetId());
            hwcNodePtr->SetHardwareForcedDisabledState(true);
        }
    }
}

void RSUniRenderVisitor::PrevalidateHwcNode()
{
    if (!RSUniHwcPrevalidateUtil::GetInstance().IsPrevalidateEnable()) {
        RS_LOGD_IF(DEBUG_PREVALIDATE, "RSUniRenderVisitor::PrevalidateHwcNode prevalidate close");
        hwcVisitor_->PrintHiperfCounterLog("counter2", static_cast<uint64_t>(0));
        return;
    }
    auto& curMainAndLeashSurfaces = curDisplayNode_->GetAllMainAndLeashSurfaces();
    std::vector<RequestLayerInfo> prevalidLayers;
    uint32_t curFps = OHOS::Rosen::HgmCore::Instance().GetScreenCurrentRefreshRate(curDisplayNode_->GetScreenId());
    uint32_t zOrder = static_cast<uint32_t>(globalZOrder_);
    // add surfaceNode layer
    RSUniHwcPrevalidateUtil::GetInstance().CollectSurfaceNodeLayerInfo(
        prevalidLayers, curMainAndLeashSurfaces, curFps, zOrder, screenInfo_);
    RS_TRACE_NAME_FMT("PrevalidateHwcNode hwcLayer: %u", prevalidLayers.size());
    if (prevalidLayers.size() == 0) {
        RS_LOGI_IF(DEBUG_PREVALIDATE, "RSUniRenderVisitor::PrevalidateHwcNode no hardware layer");
        hwcVisitor_->PrintHiperfCounterLog("counter2", INPUT_HWC_LAYERS);
        return;
    }
    // add display layer
    RequestLayerInfo displayLayer;
    if (RSUniHwcPrevalidateUtil::GetInstance().CreateDisplayNodeLayerInfo(
        zOrder++, curDisplayNode_, screenInfo_, curFps, displayLayer)) {
        prevalidLayers.emplace_back(displayLayer);
    }
    // add rcd layer
    RequestLayerInfo rcdLayer;
    if (RSSingleton<RoundCornerDisplayManager>::GetInstance().GetRcdEnable()) {
        auto topNode = std::static_pointer_cast<RSRcdSurfaceRenderNode>(curDisplayNode_->GetRcdSurfaceNodeTop());
        if (topNode &&
            RSUniHwcPrevalidateUtil::GetInstance().CreateRCDLayerInfo(topNode, screenInfo_, curFps, rcdLayer)) {
            prevalidLayers.emplace_back(rcdLayer);
        }
        auto bottomNode = std::static_pointer_cast<RSRcdSurfaceRenderNode>(curDisplayNode_->GetRcdSurfaceNodeBottom());
        if (bottomNode &&
            RSUniHwcPrevalidateUtil::GetInstance().CreateRCDLayerInfo(bottomNode, screenInfo_, curFps, rcdLayer)) {
            prevalidLayers.emplace_back(rcdLayer);
        }
    }
    hwcVisitor_->PrintHiperfCounterLog("counter2", static_cast<uint64_t>(prevalidLayers.size()));
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
        if (node->IsInFixedRotation() || node->GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED)) {
            continue;
        }
        RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by preValidate",
            node->GetName().c_str(), node->GetId());
        hwcVisitor_->PrintHiperfLog(node, "preValidate");
        node->SetHardwareForcedDisabledState(true);
        if (node->GetRSSurfaceHandler()) {
            node->GetRSSurfaceHandler()->SetGlobalZOrder(-1.f);
        }
        hwcVisitor_->Statistics().UpdateHwcDisabledReasonForDFX(node->GetId(),
            HwcDisabledReasons::DISABLED_BY_PREVALIDATE, node->GetName());
    }
}

void RSUniRenderVisitor::UpdateHwcNodeDirtyRegionAndCreateLayer(std::shared_ptr<RSSurfaceRenderNode>& node)
{
    const auto& hwcNodes = node->GetChildHardwareEnabledNodes();
    if (hwcNodes.empty() || !curDisplayNode_) {
        return;
    }
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
        if (IsHdrUseUnirender(curDisplayNode_->GetHasUniRenderHdrSurface(), hwcNodePtr)) {
            hwcNodePtr->SetHardwareForcedDisabledState(true);
            if (!curDisplayNode_->GetForceCloseHdr()) {
                // DRM will force HDR to use unirender
                curDisplayNode_->SetHasUniRenderHdrSurface(curDisplayNode_->GetHasUniRenderHdrSurface() ||
                RSHdrUtil::CheckIsHdrSurface(*hwcNodePtr) != HdrStatus::NO_HDR);
            }
            hwcVisitor_->Statistics().UpdateHwcDisabledReasonForDFX(hwcNodePtr->GetId(),
                HwcDisabledReasons::DISABLED_BY_RENDER_HDR_SURFACE, hwcNodePtr->GetName());
        }
        UpdateHwcNodeDirtyRegionForApp(node, hwcNodePtr);
        hwcNodePtr->SetCalcRectInPrepare(false);
        hwcNodePtr->SetHardWareDisabledByReverse(false);
        surfaceHandler->SetGlobalZOrder(hwcNodePtr->IsHardwareForcedDisabled() &&
            !hwcNodePtr->GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED) ? -1.f : globalZOrder_++);
        auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(hwcNodePtr->GetStagingRenderParams().get());
        if (stagingSurfaceParams->GetIsHwcEnabledBySolidLayer()) {
            surfaceHandler->SetGlobalZOrder(globalZOrder_++);
        }
        auto transform = RSUniHwcComputeUtil::GetLayerTransform(*hwcNodePtr, screenInfo_);
        hwcNodePtr->UpdateHwcNodeLayerInfo(transform);
    }
    curDisplayNode_->SetDisplayGlobalZOrder(globalZOrder_);
    if (!topLayers.empty()) {
        UpdateTopLayersDirtyStatus(topLayers);
    }
}

bool RSUniRenderVisitor::IsHdrUseUnirender(bool hasUniRenderHdrSurface,
    std::shared_ptr<RSSurfaceRenderNode>& hwcNodePtr)
{
    if (hwcNodePtr->GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED)) {
        return false;
    }
    bool isHdrHardwareDisabled = RSLuminanceControl::Get().IsCloseHardwareHdr() &&
        (RSHdrUtil::CheckIsHdrSurface(*hwcNodePtr) != HdrStatus::NO_HDR ||
        RSLuminanceControl::Get().IsHdrOn(curDisplayNode_->GetScreenId()));
    if (hasUniRenderHdrSurface || !drmNodes_.empty() || hasFingerprint_[currentVisitDisplay_] ||
        isHdrHardwareDisabled) {
        RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64
            " disabled by having UniRenderHdrSurface/DRM nodes, isHdrHardwareDisabled:%d",
            hwcNodePtr->GetName().c_str(), hwcNodePtr->GetId(), isHdrHardwareDisabled);
        hwcVisitor_->PrintHiperfLog(hwcNodePtr, "uniRender HDR");
        return true;
    }
    return false;
}

void RSUniRenderVisitor::UpdatePointWindowDirtyStatus(std::shared_ptr<RSSurfaceRenderNode>& pointWindow)
{
    if (!pointWindow->IsHardwareEnabledTopSurface() || !pointWindow->ShouldPaint()) {
        pointWindow->SetHardCursorStatus(false);
        RSPointerWindowManager::Instance().SetIsPointerEnableHwc(false);
        return;
    }
    std::shared_ptr<RSSurfaceHandler> pointSurfaceHandler = pointWindow->GetMutableRSSurfaceHandler();
    if (pointSurfaceHandler) {
        // globalZOrder_ + 2 is displayNode layer, point window must be at the top.
        pointSurfaceHandler->SetGlobalZOrder(globalZOrder_ + 2);
        if (!curDisplayNode_) {
            return;
        }
        bool isHardCursor = RSPointerWindowManager::Instance().CheckHardCursorSupport(curDisplayNode_->GetScreenId());
        pointWindow->SetHardwareForcedDisabledState(true);
        RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by pointWindow and pointSurfaceHandler",
            pointWindow->GetName().c_str(), pointWindow->GetId());
        bool isMirrorMode = RSPointerWindowManager::Instance().HasMirrorDisplay();
        RSPointerWindowManager::Instance().SetIsPointerEnableHwc(isHardCursor && !isMirrorMode);
        auto transform = RSUniHwcComputeUtil::GetLayerTransform(*pointWindow, screenInfo_);
        pointWindow->SetHardCursorStatus(isHardCursor);
        pointWindow->UpdateHwcNodeLayerInfo(transform, isHardCursor);
        if (isHardCursor) {
            RSPointerWindowManager::Instance().UpdatePointerDirtyToGlobalDirty(pointWindow, curDisplayNode_);
        }
    }
}

void RSUniRenderVisitor::UpdateTopLayersDirtyStatus(const std::vector<std::shared_ptr<RSSurfaceRenderNode>>& topLayers)
{
    for (const auto& topLayer : topLayers) {
        std::shared_ptr<RSSurfaceHandler> topLayerSurfaceHandler = topLayer->GetMutableRSSurfaceHandler();
        if (topLayerSurfaceHandler) {
            topLayerSurfaceHandler->SetGlobalZOrder(globalZOrder_ + 1);
            topLayer->SetCalcRectInPrepare(false);
            bool hwcDisabled = !IsHardwareComposerEnabled() || !topLayer->ShouldPaint() ||
                curDisplayNode_->GetHasUniRenderHdrSurface() || !drmNodes_.empty();
            topLayer->SetHardwareForcedDisabledState(hwcDisabled);
            if (hwcDisabled) {
                RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by toplayers dirty status, "
                    "IsHardwareComposerEnabled[%d],TopLayerShouldPaint[%d],HasUniRenderHdrSurface[%d],DrmNodeEmpty[%d]",
                    topLayer->GetName().c_str(), topLayer->GetId(), IsHardwareComposerEnabled(),
                    topLayer->ShouldPaint(), curDisplayNode_->GetHasUniRenderHdrSurface(), drmNodes_.empty());
            }
            auto transform = RSUniHwcComputeUtil::GetLayerTransform(*topLayer, screenInfo_);
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
        // cal done node dirtyRegion for uifirst
        RSUifirstManager::Instance().MergeOldDirtyToDirtyManager(surfaceNode);
        UpdatePointWindowDirtyStatus(surfaceNode);
        // 1. calculate abs dirtyrect and update partialRenderParams
        // currently only sync visible region info
        surfaceNode->UpdatePartialRenderParams();
        if (dirtyManager && dirtyManager->IsCurrentFrameDirty() &&
            surfaceNode->GetVisibleRegion().IsIntersectWith(dirtyManager->GetCurrentFrameDirtyRegion())) {
            hasMainAndLeashSurfaceDirty = true;
        }
        // 2. check surface node dirtyrect need merge into displayDirtyManager
        CheckMergeSurfaceDirtysForDisplay(surfaceNode);
        // 3. check merge transparent filter when it intersects with pre-dirty.
        CheckMergeDisplayDirtyByTransparentFilter(surfaceNode, accumulatedDirtyRegion);
        // 4. for cross-display node, process its filter (which is not collected during prepare).
        CollectFilterInCrossDisplayWindow(surfaceNode, accumulatedDirtyRegion);
        // 5. accumulate dirty region of this surface.
        AccumulateSurfaceDirtyRegion(surfaceNode, accumulatedDirtyRegion);
    });
    curDisplayNode_->SetMainAndLeashSurfaceDirty(hasMainAndLeashSurfaceDirty);
    CheckMergeDebugRectforRefreshRate(curMainAndLeashSurfaces);
    CheckMergeDisplayDirtyByRoundCornerDisplay();
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
    UpdateIfHwcNodesHaveVisibleRegion(curMainAndLeashSurfaces);
    curDisplayNode_->ClearCurrentSurfacePos();
    std::swap(preMainAndLeashWindowNodesIds_, curMainAndLeashWindowNodesIds_);

#ifdef RS_PROFILER_ENABLED
    RS_PROFILER_SET_DIRTY_REGION(accumulatedDirtyRegion);
#endif
}

void RSUniRenderVisitor::UpdateIfHwcNodesHaveVisibleRegion(
    std::vector<RSBaseRenderNode::SharedPtr>& curMainAndLeashSurfaces)
{
    bool needForceUpdateHwcNodes = false;
    bool hasVisibleHwcNodes = false;
    for (auto& nodePtr : curMainAndLeashSurfaces) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodePtr);
        if (!surfaceNode) {
            RS_LOGD("[%{public}s]: surfaceNode is nullptr", __func__);
            continue;
        }
        if (surfaceNode->GetVisibleRegion().IsEmpty()) {
            continue;
        }
        const auto& hwcNodes = surfaceNode->GetChildHardwareEnabledNodes();
        if (hwcNodes.empty()) {
            continue;
        }
        UpdateHwcNodesIfVisibleForApp(surfaceNode, hwcNodes, hasVisibleHwcNodes, needForceUpdateHwcNodes);
        if (needForceUpdateHwcNodes) {
            break;
        }
    }
    curDisplayNode_->SetNeedForceUpdateHwcNodes(needForceUpdateHwcNodes, hasVisibleHwcNodes);
}

void RSUniRenderVisitor::UpdateHwcNodesIfVisibleForApp(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
    const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& hwcNodes,
    bool& hasVisibleHwcNodes, bool& needForceUpdateHwcNodes)
{
    for (auto& hwcNode : hwcNodes) {
        auto hwcNodePtr = hwcNode.lock();
        if (!hwcNodePtr || !hwcNodePtr->IsOnTheTree()) {
            continue;
        }
        if (hwcNodePtr->IsHardwareForcedDisabled()) {
            continue;
        }
        Occlusion::Rect dstRect(hwcNodePtr->GetDstRect());
        if (surfaceNode->GetVisibleRegion().IsIntersectWith(dstRect)) {
            hwcNodePtr->SetLastFrameHasVisibleRegion(true); // visible Region
            hasVisibleHwcNodes = true;
            if (hwcNodePtr->GetRSSurfaceHandler() == nullptr) {
                continue;
            }
            if (hwcNodePtr->GetRSSurfaceHandler()->IsCurrentFrameBufferConsumed()) {
                needForceUpdateHwcNodes = true;
            }
        } else {
            hwcNodePtr->SetLastFrameHasVisibleRegion(false); // invisible Region
        }
    }
}

void RSUniRenderVisitor::UpdateDisplayRcdRenderNode()
{
    RSSingleton<RoundCornerDisplayManager>::GetInstance().RefreshFlagAndUpdateResource(curDisplayNode_->GetId());
    bool isRcdEnabled = RSSingleton<RoundCornerDisplayManager>::GetInstance().GetRcdEnable();
    if (isRcdEnabled) {
        if (!curDisplayNode_->GetRcdSurfaceNodeTop()) {
            auto node = RSRcdSurfaceRenderNode::Create(
                curDisplayNode_->GetId(), RCDSurfaceType::TOP, curDisplayNode_->GetContext());
            curDisplayNode_->SetRcdSurfaceNodeTop(node);
        }
        if (!curDisplayNode_->GetRcdSurfaceNodeBottom()) {
            auto node = RSRcdSurfaceRenderNode::Create(
                curDisplayNode_->GetId(), RCDSurfaceType::BOTTOM, curDisplayNode_->GetContext());
            curDisplayNode_->SetRcdSurfaceNodeBottom(node);
        }
        auto hardInfo =
            RSSingleton<RoundCornerDisplayManager>::GetInstance().PrepareHardwareInfo(curDisplayNode_->GetId());
        if (hardInfo.topLayer && hardInfo.bottomLayer) {
            auto topNode = std::static_pointer_cast<RSRcdSurfaceRenderNode>(curDisplayNode_->GetRcdSurfaceNodeTop());
            auto bottomNode =
                std::static_pointer_cast<RSRcdSurfaceRenderNode>(curDisplayNode_->GetRcdSurfaceNodeBottom());
            if (hardInfo.resourceChanged) {
                topNode->SetRenderDisplayRect(hardInfo.displayRect);
                topNode->PrepareHardwareResource(hardInfo.topLayer);
                bottomNode->SetRenderDisplayRect(hardInfo.displayRect);
                bottomNode->PrepareHardwareResource(hardInfo.bottomLayer);
            }
            topNode->UpdateRcdRenderParams(hardInfo.resourceChanged, hardInfo.topLayer->curBitmap);
            bottomNode->UpdateRcdRenderParams(hardInfo.resourceChanged, hardInfo.bottomLayer->curBitmap);
        }
    }
}

void RSUniRenderVisitor::UpdateCornerRadiusInfoForDRM(std::shared_ptr<RSSurfaceRenderNode> hwcNode,
    std::vector<RectI>& hwcRects)
{
    if (!hwcNode || !hwcNode->GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED)) {
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
    bool isHardCursor = RSPointerWindowManager::Instance().CheckHardCursorSupport(curDisplayNode_->GetScreenId());
    if (isHardCursor && surfaceNode.IsHardwareEnabledTopSurface() && surfaceNode.GetHardCursorStatus()) {
        return;
    }
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

void RSUniRenderVisitor::CheckMergeDisplayDirtyByAttraction(RSSurfaceRenderNode& surfaceNode) const
{
    if (surfaceNode.GetRenderProperties().IsAttractionValid()) {
        auto attractionDirtyRect = surfaceNode.GetRenderProperties().GetAttractionEffectCurrentDirtyRegion();
        RS_LOGD("CheckMergeDisplayDirtyByAttraction global merge attraction %{public}s: global dirty %{public}s,"
            "add rect %{public}s", surfaceNode.GetName().c_str(),
            curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
            attractionDirtyRect.ToString().c_str());
        auto boundsGeometry = curDisplayNode_->GetRenderProperties().GetBoundsGeometry();
        if (boundsGeometry) {
            Drawing::Rect rect(attractionDirtyRect.GetLeft(), attractionDirtyRect.GetTop(),
                attractionDirtyRect.GetRight(), attractionDirtyRect.GetBottom());
            Drawing::Rect tempRect;
            boundsGeometry->GetMatrix().MapRect(tempRect, rect);
            attractionDirtyRect =
                RectI(tempRect.GetLeft(), tempRect.GetTop(), tempRect.GetWidth(), tempRect.GetHeight());
        }
        curDisplayNode_->GetDirtyManager()->MergeDirtyRect(attractionDirtyRect);
    }
}

void RSUniRenderVisitor::CheckMergeSurfaceDirtysForDisplay(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const
{
    if (surfaceNode->GetDirtyManager() == nullptr || curDisplayNode_->GetDirtyManager() == nullptr) {
        return;
    }
    // 1 handle last and curframe surfaces which appear or disappear case
    CheckMergeDisplayDirtyBySurfaceChanged();
    // 2 if the surface node is cross-display and prepared again, convert its dirty region into global.
    if (surfaceNode->IsFirstLevelCrossNode() && !curDisplayNode_->IsFirstVisitCrossNodeDisplay()) {
        CheckMergeDisplayDirtyByPosChanged(*surfaceNode);
        CheckMergeDisplayDirtyByCrossDisplayWindow(*surfaceNode);
        return;
    }
    // 3 Handles the case of transparent surface, merge transparent dirty rect
    CheckMergeDisplayDirtyByTransparent(*surfaceNode);
    // 4 Zorder changed case, merge surface dest Rect
    CheckMergeDisplayDirtyByZorderChanged(*surfaceNode);
    // 5 surfacePos chanded case, merge surface lastframe pos or curframe pos
    CheckMergeDisplayDirtyByPosChanged(*surfaceNode);
    // 6 shadow disappear and appear case.
    CheckMergeDisplayDirtyByShadowChanged(*surfaceNode);
    // 7 handle surface has attraction effect
    CheckMergeDisplayDirtyByAttraction(*surfaceNode);
    // More: any other display dirty caused by surfaceNode should be added here like CheckMergeDisplayDirtByXXX
}

void RSUniRenderVisitor::CheckMergeDisplayDirtyByCrossDisplayWindow(RSSurfaceRenderNode& surfaceNode) const
{
    auto geoPtr = surfaceNode.GetRenderProperties().GetBoundsGeometry();
    if (!geoPtr) {
        return;
    }
    // transfer from the display coordinate system during quickprepare into current display coordinate system.
    auto dirtyRect = geoPtr->MapRect(surfaceNode.GetDirtyManager()->GetCurrentFrameDirtyRegion().ConvertTo<float>(),
        surfaceNode.GetCrossNodeSkipDisplayConversionMatrix(curDisplayNode_->GetId()));
    RS_OPTIONAL_TRACE_NAME_FMT("CheckMergeDisplayDirtyByCrossDisplayWindow %s, global dirty %s, add rect %s",
        surfaceNode.GetName().c_str(), curDisplayDirtyManager_->GetCurrentFrameDirtyRegion().ToString().c_str(),
        dirtyRect.ToString().c_str());
    curDisplayDirtyManager_->MergeDirtyRect(dirtyRect);
}

void RSUniRenderVisitor::CollectFilterInCrossDisplayWindow(
    std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, Occlusion::Region& accumulatedDirtyRegion)
{
    auto geoPtr = surfaceNode->GetRenderProperties().GetBoundsGeometry();
    if (!surfaceNode->IsFirstLevelCrossNode() || curDisplayNode_->IsFirstVisitCrossNodeDisplay() || !geoPtr) {
        return;
    }
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (auto& child : surfaceNode->GetVisibleFilterChild()) {
        auto& filterNode = nodeMap.GetRenderNode<RSRenderNode>(child);
        if (!filterNode) {
            continue;
        }
        auto filterRect = geoPtr->MapRect(filterNode->GetAbsDrawRect().ConvertTo<float>(),
            surfaceNode->GetCrossNodeSkipDisplayConversionMatrix(curDisplayNode_->GetId())).IntersectRect(screenRect_);
        if (surfaceNode->IsTransparent() && accumulatedDirtyRegion.IsIntersectWith(Occlusion::Rect(filterRect))) {
            RS_OPTIONAL_TRACE_NAME_FMT("CollectFilterInCrossDisplayWindow [%s] has filter, add [%s] to global dirty",
                surfaceNode->GetName().c_str(), filterRect.ToString().c_str());
            curDisplayDirtyManager_->MergeDirtyRect(filterRect);
        } else {
            RS_OPTIONAL_TRACE_NAME_FMT("CollectFilterInCrossDisplayWindow [%s] has filter, add [%s] to global filter",
                surfaceNode->GetName().c_str(), filterRect.ToString().c_str());
            globalFilter_.insert({ filterNode->GetId(), filterRect });
        }
    }
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
                if (filterNode->GetRenderProperties().GetBackgroundFilter() ||
                    filterNode->GetRenderProperties().GetNeedDrawBehindWindow()) {
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
            RsFrameBlurPredict::GetInstance().PredictDrawLargeAreaBlur(*filterNode);
        }
    }
}

void RSUniRenderVisitor::AccumulateSurfaceDirtyRegion(
    std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, Occlusion::Region& accumulatedDirtyRegion) const
{
    auto geoPtr = surfaceNode->GetRenderProperties().GetBoundsGeometry();
    if (!surfaceNode->GetDirtyManager() || !geoPtr) {
        return;
    }
    auto surfaceDirtyRect = surfaceNode->GetDirtyManager()->GetCurrentFrameDirtyRegion();
    if (surfaceNode->IsFirstLevelCrossNode() && !curDisplayNode_->IsFirstVisitCrossNodeDisplay()) {
        surfaceDirtyRect = geoPtr->MapRect(surfaceDirtyRect.ConvertTo<float>(),
            surfaceNode->GetCrossNodeSkipDisplayConversionMatrix(curDisplayNode_->GetId()));
    }
    auto surfaceDirtyRegion = Occlusion::Region{ Occlusion::Rect{ surfaceDirtyRect } };
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
    if (node.GetRenderProperties().NeedFilter() || node.ChildHasVisibleFilter() ||
        node.GetHwcRecorder().IsBlendWithBackground()) {
        nodeParent->SetChildHasVisibleFilter(true);
        nodeParent->UpdateVisibleFilterChild(node);
    }
    if ((node.GetRenderProperties().GetUseEffect() && node.ShouldPaint())|| node.ChildHasVisibleEffect()) {
        nodeParent->SetChildHasVisibleEffect(true);
        nodeParent->UpdateVisibleEffectChild(node);
    }
    if (node.GetSharedTransitionParam() || node.ChildHasSharedTransition()) {
        nodeParent->SetChildHasSharedTransition(true);
    }
}

CM_INLINE void RSUniRenderVisitor::PostPrepare(RSRenderNode& node, bool subTreeSkipped)
{
    // Collect prepared subtree into the control-level occlusion culling handler.
    // For the root node, trigger occlusion detection.
    CollectSubTreeAndProcessOcclusion(node, subTreeSkipped);
    UpdateCurFrameInfoDetail(node, subTreeSkipped, true);
    if (const auto& sharedTransitionParam = node.GetSharedTransitionParam()) {
        sharedTransitionParam->GenerateDrawable(node);
    }
    auto curDirtyManager = curSurfaceNode_ ? curSurfaceDirtyManager_ : curDisplayDirtyManager_;
    if (!curDirtyManager) {
        return;
    }

    if (subTreeSkipped) {
        auto isOccluded = curSurfaceNode_ ?
            curSurfaceNode_->IsMainWindowType() && curSurfaceNode_->GetVisibleRegion().IsEmpty() : false;
        if (!isOccluded || node.IsFirstLevelCrossNode()) {
            hwcVisitor_->UpdateHwcNodeRectInSkippedSubTree(node);
            CheckFilterNodeInSkippedSubTreeNeedClearCache(node, *curDirtyManager);
            UpdateSubSurfaceNodeRectInSkippedSubTree(node);
        }
    }
    if (node.NeedUpdateDrawableBehindWindow()) {
        node.GetMutableRenderProperties().SetNeedDrawBehindWindow(node.NeedDrawBehindWindow());
    }
    auto globalFilterRect = node.IsInstanceOf<RSEffectRenderNode>() && !node.FirstFrameHasEffectChildren() ?
        GetVisibleEffectDirty(node) : node.GetOldDirtyInSurface();
    auto globalHwcFilterRect = node.IsInstanceOf<RSEffectRenderNode>() && !node.FirstFrameHasEffectChildren() ?
        hwcVisitor_->GetHwcVisibleEffectDirty(node, globalFilterRect) : globalFilterRect;
    if (node.NeedDrawBehindWindow()) {
        node.CalDrawBehindWindowRegion();
        globalFilterRect = node.GetFilterRect();
    }
    if (node.GetRenderProperties().NeedFilter() || node.GetHwcRecorder().IsBlendWithBackground()) {
        hwcVisitor_->UpdateHwcNodeEnableByFilterRect(
            curSurfaceNode_, node, NeedPrepareChindrenInReverseOrder(node),
            node.GetHwcRecorder().GetZOrderForHwcEnableByFilter());
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
    node.SetHasUnobscuredUEC();
    if (isDrawingCacheEnabled_) {
        bool isInBlackList = false;
        if (node.GetType() == RSRenderNodeType::SURFACE_NODE) {
            auto& surfaceNode = static_cast<RSSurfaceRenderNode&>(node);
            if (surfaceNode.IsLeashWindow() &&
                allBlackList_.find(surfaceNode.GetLeashPersistentId()) != allBlackList_.end()) {
                isInBlackList = true;
            }
        }
        node.UpdateDrawingCacheInfoAfterChildren(isInBlackList);
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

void RSUniRenderVisitor::CollectSubTreeAndProcessOcclusion(OHOS::Rosen::RSRenderNode& node, bool subTreeSkipped)
{
    if (!curOcclusionHandler_) {
        return;
    }

    curOcclusionHandler_->CollectSubTree(node, !subTreeSkipped);
    if (node.GetId() != curOcclusionHandler_->GetRootNodeId()) {
        return;
    }

    curOcclusionHandler_->UpdateSkippedSubTreeProp();
    curOcclusionHandler_->CalculateFrameOcclusion();
    auto rootNode = RSBaseRenderNode::ReinterpretCast<RSRootRenderNode>(node.shared_from_this());
    if (!rootNode) {
        return;
    }

    auto occlusionParams = rootNode->GetOcclusionParams();
    if (!occlusionParams) {
        return;
    }

    occlusionParams->SetCulledNodes(curOcclusionHandler_->AcquireCulledNodes());
    if (!curOcclusionHandler_->IsOcclusionActive()) {
        occlusionParams->UpdateOcclusionCullingStatus(false, INVALID_NODEID);
    }
    curOcclusionHandler_ = nullptr;
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
        hwcVisitor_->UpdateHwcNodeEnableByFilterRect(curSurfaceNode_, *filterNode);
        CollectFilterInfoAndUpdateDirty(*filterNode, dirtyManager, filterRect, filterRect);
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
        hwcVisitor_->UpdateDstRect(*subSurfaceNodePtr, subSurfaceRect, prepareClipRect_);
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
            auto& rateReduceManager = RSMainThread::Instance()->GetRSVsyncRateReduceManager();
            rateReduceManager.PushWindowNodeId(subSurfaceNodePtr->GetId());
            rateReduceManager.CollectSurfaceVsyncInfo(screenInfo_, *subSurfaceNodePtr);
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

void RSUniRenderVisitor::CollectFilterInfoAndUpdateDirty(RSRenderNode& node,
    RSDirtyRegionManager& dirtyManager, const RectI& globalFilterRect, const RectI& globalHwcFilterRect)
{
    bool isNodeAddedToTransparentCleanFilters = false;
    if (curSurfaceNode_) {
        bool isIntersect = dirtyManager.GetCurrentFrameDirtyRegion().Intersect(globalFilterRect);
        if (isIntersect) {
            dirtyManager.MergeDirtyRect(globalHwcFilterRect);
        } else {
            curSurfaceNoBelowDirtyFilter_.insert({node.GetId(), globalHwcFilterRect});
        }
        if (node.GetRenderProperties().GetFilter()) {
            node.UpdateFilterCacheWithBelowDirty(dirtyManager, true);
        }
        node.UpdateFilterCacheWithSelfDirty();
        if (curSurfaceNode_->IsTransparent()) {
            if (!isIntersect || (isIntersect && (node.GetRenderProperties().GetBackgroundFilter() ||
                node.GetRenderProperties().GetNeedDrawBehindWindow()) && !node.IsBackgroundInAppOrNodeSelfDirty())) {
                // record nodes which has transparent clean filter
                RS_OPTIONAL_TRACE_NAME_FMT("CollectFilterInfoAndUpdateDirty::surfaceNode:%s, add node[%lld] to "
                    "transparentCleanFilter", curSurfaceNode_->GetName().c_str(), node.GetId());
                transparentCleanFilter_[curSurfaceNode_->GetId()].push_back({node.GetId(), globalHwcFilterRect});
                hwcVisitor_->transparentHwcCleanFilter_[curSurfaceNode_->GetId()].
                    push_back({node.GetId(), globalHwcFilterRect});
                isNodeAddedToTransparentCleanFilters = true;
            }
            if (isIntersect) {
                transparentDirtyFilter_[curSurfaceNode_->GetId()].push_back({node.GetId(), globalHwcFilterRect});
                hwcVisitor_->transparentHwcDirtyFilter_[curSurfaceNode_->GetId()].
                    push_back({node.GetId(), globalHwcFilterRect});
                RS_LOGD("RSUniRenderVisitor::CollectFilterInfoAndUpdateDirty global merge transparentDirtyFilter "
                    "%{public}s, global dirty %{public}s, add rect %{public}s", curSurfaceNode_->GetName().c_str(),
                    curDisplayDirtyManager_->GetCurrentFrameDirtyRegion().ToString().c_str(),
                    globalHwcFilterRect.ToString().c_str());
                curDisplayDirtyManager_->MergeDirtyRect(globalHwcFilterRect);
            }
        } else {
            // record surface nodes and nodes in surface which has clean filter
            globalFilter_.insert({node.GetId(), globalHwcFilterRect});
        }
    } else {
        // record container nodes which need filter
        containerFilter_.insert({node.GetId(), globalHwcFilterRect});
    }
    if (curSurfaceNode_ && !isNodeAddedToTransparentCleanFilters) {
        node.PostPrepareForBlurFilterNode(dirtyManager, needRequestNextVsync_);
        RsFrameBlurPredict::GetInstance().PredictDrawLargeAreaBlur(node);
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
    if (RSUifirstManager::Instance().GetUiFirstType() == UiFirstCcmType::MULTI) {
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
    dirtyFlag_ = node.UpdateDrawRectAndDirtyRegion(
        *curSurfaceDirtyManager_, dirtyFlag_, prepareClipRect_, parentSurfaceNodeMatrix_);

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

    node.EnableWindowKeyFrame(false);

    // Initialize the handler of control-level occlusion culling.
    InitializeOcclusionHandler(node);

    bool isSubTreeNeedPrepare = node.IsSubTreeNeedPrepare(filterInGlobal_) || ForcePrepareSubTree();
    isSubTreeNeedPrepare ? QuickPrepareChildren(node) :
        node.SubTreeSkipPrepare(*curSurfaceDirtyManager_, curDirty_, dirtyFlag_, prepareClipRect_);
    PostPrepare(node, !isSubTreeNeedPrepare);

    curAlpha_ = prevAlpha;
    parentSurfaceNodeMatrix_ = parentSurfaceNodeMatrix;
    dirtyFlag_ = dirtyFlag;
    prepareClipRect_ = prepareClipRect;
}

void RSUniRenderVisitor::InitializeOcclusionHandler(RSRootRenderNode& node)
{
    if (curOcclusionHandler_) {
        return;
    }

    auto occlusionParams = node.GetOcclusionParams();
    if (!occlusionParams || !occlusionParams->IsOcclusionCullingOn()) {
        return;
    }

    curOcclusionHandler_ = occlusionParams->GetOcclusionHandler();
    if (!curOcclusionHandler_) {
        curOcclusionHandler_ =
            std::make_shared<RSOcclusionHandler>(node.GetId(), occlusionParams->GetKeyOcclusionNodeId());
        occlusionParams->SetOcclusionHandler(curOcclusionHandler_);
    }
    curOcclusionHandler_->UpdateKeyOcclusionNodeId(occlusionParams->GetKeyOcclusionNodeId());
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
    renderThreadParams->isDirtyAlignEnabled_ = isDirtyAlignEnabled_;
    renderThreadParams->isStencilPixelOcclusionCullingEnabled_ = isStencilPixelOcclusionCullingEnabled_;
    renderThreadParams->isCrossNodeOffscreenOn_ = isCrossNodeOffscreenOn_;
    renderThreadParams->isUIFirstDebugEnable_ = isUIFirstDebugEnable_;
    renderThreadParams->dfxTargetSurfaceNames_ = std::move(dfxTargetSurfaceNames_);
    renderThreadParams->isVirtualDirtyEnabled_ = isVirtualDirtyEnabled_;
    renderThreadParams->isVirtualDirtyDfxEnabled_ = isVirtualDirtyDfxEnabled_;
    renderThreadParams->isExpandScreenDirtyEnabled_ = isExpandScreenDirtyEnabled_;
    renderThreadParams->advancedDirtyType_ = advancedDirtyType_;
    renderThreadParams->hasDisplayHdrOn_ = hasDisplayHdrOn_;
    renderThreadParams->hasMirrorDisplay_ = hasMirrorDisplay_;
    if (RSPointerWindowManager::Instance().IsNeedForceCommitByPointer()) {
        renderThreadParams->forceCommitReason_ |= ForceCommitReason::FORCED_BY_POINTER_WINDOW;
    }
}

void RSUniRenderVisitor::SendRcdMessage(RSDisplayRenderNode& node)
{
    if (RoundCornerDisplayManager::CheckRcdRenderEnable(screenInfo_) &&
        RSSingleton<RoundCornerDisplayManager>::GetInstance().GetRcdEnable()) {
        using rcd_msg = RSSingleton<RsMessageBus>;
        uint32_t left = 0; // render region
        uint32_t top = 0;
        uint32_t width = screenInfo_.width;
        uint32_t height = screenInfo_.height;
        if (!screenInfo_.activeRect.IsEmpty()) {
            left = static_cast<uint32_t>(std::max(0, screenInfo_.activeRect.GetLeft()));
            top = static_cast<uint32_t>(std::max(0, screenInfo_.activeRect.GetTop()));
            width = static_cast<uint32_t>(std::max(0, screenInfo_.activeRect.GetWidth()));
            height = static_cast<uint32_t>(std::max(0, screenInfo_.activeRect.GetHeight()));
        }
        rcd_msg::GetInstance().SendMsg<NodeId, uint32_t, uint32_t, uint32_t, uint32_t>(TOPIC_RCD_DISPLAY_SIZE,
            node.GetId(), left, top, width, height);
        rcd_msg::GetInstance().SendMsg<NodeId, ScreenRotation>(TOPIC_RCD_DISPLAY_ROTATION,
            node.GetId(), node.GetScreenRotation());
        rcd_msg::GetInstance().SendMsg<NodeId, int>(TOPIC_RCD_DISPLAY_NOTCH,
            node.GetId(), RSSystemParameters::GetHideNotchStatus());
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
        sharedTransitionParam->ResetRelation();
        if (!sharedTransitionParam->paired_) {
            continue;
        }
        ROSEN_LOGD("RSUniRenderVisitor::ProcessUnpairedSharedTransitionNode: mark %s as unpaired",
            sharedTransitionParam->Dump().c_str());
        sharedTransitionParam->paired_ = false;
        unpairNode(sharedTransitionParam->inNode_);
        unpairNode(sharedTransitionParam->outNode_);
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

void RSUniRenderVisitor::CheckMergeDebugRectforRefreshRate(std::vector<RSBaseRenderNode::SharedPtr>& surfaces)
{
    // Debug dirtyregion of show current refreshRation
    if (RSRealtimeRefreshRateManager::Instance().GetShowRefreshRateEnabled()) {
        if (curDisplayNode_ == nullptr) {
            RS_LOGE("RSUniRenderVisitor::CheckMergeDebugRectforRefreshRate  curDisplayNode is nullptr");
            return;
        }
        RectI tempRect = {100, 100, 500, 200};   // setDirtyRegion for RealtimeRefreshRate
        bool surfaceNodeSet = false;
        bool needMapAbsRect = true;
        auto windowContainer = curDisplayNode_->GetWindowContainer();
        if (windowContainer) {
            if (!ROSEN_EQ(windowContainer->GetRenderProperties().GetScaleX(), 1.0f, EPSILON_SCALE) ||
                !ROSEN_EQ(windowContainer->GetRenderProperties().GetScaleY(), 1.0f, EPSILON_SCALE)) {
                needMapAbsRect = false;
            }
        }
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
                if (needMapAbsRect) {
                    tempRect = geoPtr->MapAbsRect(tempRect.ConvertTo<float>());
                }
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
            if (needMapAbsRect) {
                tempRect = geoPtr->MapAbsRect(tempRect.ConvertTo<float>());
            }
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(tempRect, true);
        }
    }
}

void RSUniRenderVisitor::CheckMergeDisplayDirtyByRoundCornerDisplay() const
{
    if (!screenManager_ || !curDisplayNode_) {
        RS_LOGE(
            "RSUniRenderVisitor::CheckMergeDisplayDirtyByRoundCornerDisplay screenmanager or displaynode is nullptr");
        return;
    }

    RSScreenType screenType = BUILT_IN_TYPE_SCREEN;
    if (screenManager_->GetScreenType(curDisplayNode_->GetScreenId(), screenType) != SUCCESS) {
        RS_LOGD("RSUniRenderVisitor::CheckMergeDisplayDirtyByRoundCornerDisplay get screen type failed.");
        return;
    }

    if (screenType != EXTERNAL_TYPE_SCREEN) {
        return;
    }

    RectI dirtyRectTop, dirtyRectBottom;
    if (RSSingleton<RoundCornerDisplayManager>::GetInstance().HandleRoundCornerDirtyRect(
            curDisplayNode_->GetId(), dirtyRectTop, RoundCornerDisplayManager::RCDLayerType::TOP)) {
        RS_LOGD("RSUniRenderVisitor::CheckMergeDisplayDirtyByRoundCornerDisplay global merge topRcdNode dirty "
                "%{public}s, global dirty %{public}s, add rect %{public}s",
            std::to_string(curDisplayNode_->GetScreenId()).c_str(),
            curDisplayDirtyManager_->GetCurrentFrameDirtyRegion().ToString().c_str(),
            dirtyRectTop.ToString().c_str());
        curDisplayNode_->GetDirtyManager()->MergeDirtyRect(dirtyRectTop);
    }
    if (RSSingleton<RoundCornerDisplayManager>::GetInstance().HandleRoundCornerDirtyRect(
            curDisplayNode_->GetId(), dirtyRectBottom, RoundCornerDisplayManager::RCDLayerType::BOTTOM)) {
        RS_LOGD("RSUniRenderVisitor::CheckMergeDisplayDirtyByRoundCornerDisplay global merge bottomRcdNode dirty "
                "%{public}s, global dirty %{public}s, add rect %{public}s",
            std::to_string(curDisplayNode_->GetScreenId()).c_str(),
            curDisplayDirtyManager_->GetCurrentFrameDirtyRegion().ToString().c_str(),
            dirtyRectBottom.ToString().c_str());
        curDisplayNode_->GetDirtyManager()->MergeDirtyRect(dirtyRectBottom);
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

void RSUniRenderVisitor::SetHdrWhenMultiDisplayChange()
{
    auto mainThread = RSMainThread::Instance();
    if (!mainThread->GetMultiDisplayChange()) {
        return;
    }
    auto isMultiDisplay = mainThread->GetMultiDisplayStatus();
    RS_LOGI("RSUniRenderVisitor::SetHdrWhenMultiDisplayChange closeHdrStatus: %{public}d.", isMultiDisplay);
    RS_TRACE_NAME_FMT("RSUniRenderVisitor::SetHdrWhenMultiDisplayChange closeHdrStatus: %d", isMultiDisplay);
    RSLuminanceControl::Get().ForceCloseHdr(CLOSEHDR_SCENEID::MULTI_DISPLAY, isMultiDisplay);
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

void RSUniRenderVisitor::CollectSelfDrawingNodeRectInfo(RSSurfaceRenderNode& node)
{
    auto& monitor = SelfDrawingNodeMonitor::GetInstance();
    if (!monitor.IsListeningEnabled()) {
        return;
    }

    if (!node.IsSelfDrawingType()) {
        return;
    }
    auto rect = node.GetRenderProperties().GetBoundsGeometry()->GetAbsRect();
    std::string nodeName = node.GetName();
    monitor.InsertCurRectMap(node.GetId(), nodeName, rect);
}

void RSUniRenderVisitor::UpdateAncoPrepareClip(RSSurfaceRenderNode& node)
{
    const auto& property = node.GetRenderProperties();
    auto& geoPtr = property.GetBoundsGeometry();
    if (geoPtr == nullptr) {
        return;
    }
    if (node.GetAncoFlags() == static_cast<uint32_t>(AncoFlags::ANCO_SFV_NODE)) {
        // Dirty Region use abstract coordinate, property of node use relative coordinate
        // BoundsRect(if exists) is mapped to absRect_ of RSObjAbsGeometry
        if (property.GetClipToBounds()) {
            prepareClipRect_ = prepareClipRect_.IntersectRect(geoPtr->GetAbsRect());
        }
        if (property.GetClipToRRect()) {
            RectF rect = property.GetClipRRect().rect_;
            prepareClipRect_ = prepareClipRect_.IntersectRect(geoPtr->MapAbsRect(rect));
        }
    }
}
} // namespace Rosen
} // namespace OHOS
