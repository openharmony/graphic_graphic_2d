/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "drawable/rs_screen_render_node_drawable.h"

#include <memory>
#include <parameters.h>
#include <string>

#include "graphic_feature_param_manager.h"
#include "rs_trace.h"
#include "system/rs_system_parameters.h"

#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "common/rs_singleton.h"
#include "common/rs_special_layer_manager.h"
#include "display_engine/rs_luminance_control.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/anco_manager/rs_anco_manager.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#ifdef RS_ENABLE_OVERLAY_DISPLAY
#include "feature/overlay_display/rs_overlay_display_manager.h"
#endif
#include "hgm_core.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_screen_render_params.h"
#include "params/rs_surface_render_params.h"
#include "feature/dirty/rs_uni_dirty_compute_util.h"
#include "feature/drm/rs_drm_util.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node_drawable.h"
#include "feature/round_corner_display/rs_message_bus.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_pointer_window_manager.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/main_thread/rs_uni_render_listener.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/render_thread/rs_uni_render_virtual_processor.h"
#include "pipeline/sk_resource_manager.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "pipeline/magic_pointer_render/rs_magic_pointer_render_manager.h"
#endif
#include "platform/common/rs_log.h"
#include "platform/ohos/rs_jank_stats_helper.h"
#include "render/rs_pixel_map_util.h"
#include "screen_manager/rs_screen_manager.h"
#include "rs_frame_report.h"
// dfx
#include "drawable/dfx/rs_dirty_rects_dfx.h"
#include "drawable/dfx/rs_skp_capture_dfx.h"
#include "platform/ohos/overdraw/rs_overdraw_controller.h"
#include "utils/performanceCaculate.h"
// cpu boost
#include "c/ffrt_cpu_boost.h"
// xml parser
#include "graphic_feature_param_manager.h"
// hpae offline
#include "feature/hwc/hpae_offline/rs_hpae_offline_processor.h"

namespace OHOS::Rosen::DrawableV2 {
namespace {
constexpr const char* CLEAR_GPU_CACHE = "ClearGpuCache";
constexpr const char* DEFAULT_CLEAR_GPU_CACHE = "DefaultClearGpuCache";
constexpr int32_t CAPTURE_WINDOW = 2; // To be deleted after captureWindow being deleted
constexpr int64_t MAX_JITTER_NS = 2000000; // 2ms

std::string RectVectorToString(const std::vector<RectI>& regionRects)
{
    std::string results = "";
    for (auto& rect : regionRects) {
        results += rect.ToString();
    }
    return results;
}

Drawing::Region GetFlippedRegion(const std::vector<RectI>& rects, ScreenInfo& screenInfo)
{
    Drawing::Region region;

    for (const auto& r : rects) {
        int32_t topAfterFilp = 0;
#ifdef RS_ENABLE_VK
        topAfterFilp = (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
                           RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR)
                           ? r.top_
                           : static_cast<int32_t>(screenInfo.GetRotatedHeight()) - r.GetBottom();
#else
        topAfterFilp = static_cast<int32_t>(screenInfo.GetRotatedHeight()) - r.GetBottom();
#endif
        Drawing::Region tmpRegion;
        tmpRegion.SetRect(Drawing::RectI(r.left_, topAfterFilp, r.left_ + r.width_, topAfterFilp + r.height_));
        RS_OPTIONAL_TRACE_NAME_FMT("GetFlippedRegion orig ltrb[%d %d %d %d] to fliped rect ltrb[%d %d %d %d]",
            r.left_, r.top_, r.left_ + r.width_, r.top_ + r.height_, r.left_, topAfterFilp, r.left_ + r.width_,
            topAfterFilp + r.height_);
        region.Op(tmpRegion, Drawing::RegionOp::UNION);
    }
    return region;
}
}

// Rcd node will be handled by RS tree in OH 6.0 rcd refactoring, should remove this later
void DoScreenRcdTask(RSScreenRenderParams& params, std::shared_ptr<RSProcessor> &processor)
{
    if (processor == nullptr) {
        RS_LOGD("DoScreenRcdTask has no processor");
        return;
    }
    const auto &screenInfo = params.GetScreenInfo();
    if (screenInfo.state != ScreenState::HDI_OUTPUT_ENABLE) {
        RS_LOGD("DoScreenRcdTask is not at HDI_OUPUT mode");
        return;
    }

    bool res = true;
    bool resourceChanged = false;
    auto drawables = params.GetRoundCornerDrawables();
    for (auto drawable : drawables) {
        auto rcdDrawable = std::static_pointer_cast<DrawableV2::RSRcdSurfaceRenderNodeDrawable>(drawable);
        if (rcdDrawable) {
            resourceChanged |= rcdDrawable->IsResourceChanged();
            res &= rcdDrawable->DoProcessRenderTask(processor);
        }
    }
    if (resourceChanged && res) {
        RSSingleton<RsMessageBus>::GetInstance().SendMsg<NodeId, bool>(
            TOPIC_RCD_DISPLAY_HWRESOURCE, params.GetId(), true);
    }
}

RSScreenRenderNodeDrawable::Registrar RSScreenRenderNodeDrawable::instance_;

RSScreenRenderNodeDrawable::RSScreenRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node)), surfaceHandler_(std::make_shared<RSSurfaceHandler>(nodeId_)),
      syncDirtyManager_(std::make_shared<RSDirtyRegionManager>(true))
{}

RSRenderNodeDrawable::Ptr RSScreenRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSScreenRenderNodeDrawable(std::move(node));
}

std::unique_ptr<RSRenderFrame> RSScreenRenderNodeDrawable::RequestFrame(
    RSScreenRenderParams& params, std::shared_ptr<RSProcessor> processor)
{
    RS_TRACE_NAME_FMT("RSScreenRenderNodeDrawable:RequestFrame with colorSpace: %d, pixelformat: %d",
        params.GetNewColorSpace(), params.GetNewPixelFormat());
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (UNLIKELY(!renderEngine)) {
        RS_LOGE("RSScreenRenderNodeDrawable::RequestFrame RenderEngine is null!");
        return nullptr;
    }

    if (!processor->InitForRenderThread(*this, renderEngine)) {
        RS_LOGE("RSScreenRenderNodeDrawable::RequestFrame processor init failed!");
        return nullptr;
    }

    if (!IsSurfaceCreated()) {
        sptr<IBufferConsumerListener> listener = new RSUniRenderListener(surfaceHandler_);
        if (!CreateSurface(listener)) {
            RS_LOGE("RSScreenRenderNodeDrawable::RequestFrame CreateSurface failed");
            return nullptr;
        }
    }

    auto rsSurface = GetRSSurface();
    if (!rsSurface) {
        RS_LOGE("RSScreenRenderNodeDrawable::RequestFrame No RSSurface found");
        return nullptr;
    }
    auto bufferConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(params.GetScreenInfo(), false,
        params.GetNewColorSpace(), params.GetNewPixelFormat());
    RS_LOGD("RequestFrame colorspace is %{public}d, pixelformat is %{public}d", params.GetNewColorSpace(),
        params.GetNewPixelFormat());

    bool isHebc = true;
    if (RSAncoManager::Instance()->GetAncoHebcStatus() == AncoHebcStatus::NOT_USE_HEBC) {
        isHebc = false;
        RS_LOGI("anco request frame not use hebc");
    }
    RSAncoManager::Instance()->SetAncoHebcStatus(AncoHebcStatus::INITIAL);

    auto renderFrame = renderEngine->RequestFrame(std::static_pointer_cast<RSSurfaceOhos>(rsSurface),
        bufferConfig, false, isHebc);
    if (!renderFrame) {
        RS_LOGE("RSScreenRenderNodeDrawable::RequestFrame renderEngine requestFrame is null");
        return nullptr;
    }

    return renderFrame;
}

static void ClipRegion(Drawing::Canvas& canvas, Drawing::Region& region, bool clear = true)
{
    if (region.IsEmpty()) {
        // [planning] Remove this after frame buffer can cancel
        canvas.ClipRect(Drawing::Rect());
    } else if (region.IsRect()) {
        canvas.ClipRegion(region);
    } else {
        RS_TRACE_NAME("RSScreenRenderNodeDrawable: clipPath");
#ifdef RS_ENABLE_VK
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
            canvas.ClipRegion(region);
        } else {
            Drawing::Path dirtyPath;
            region.GetBoundaryPath(&dirtyPath);
            canvas.ClipPath(dirtyPath, Drawing::ClipOp::INTERSECT, true);
        }
#else
        Drawing::Path dirtyPath;
        region.GetBoundaryPath(&dirtyPath);
        canvas.ClipPath(dirtyPath, Drawing::ClipOp::INTERSECT, true);
#endif
    }

    // clear canvas after clip region if need
    if (clear && !region.IsEmpty()) {
        canvas.Clear(Drawing::Color::COLOR_TRANSPARENT);
    }
}

bool RSScreenRenderNodeDrawable::HardCursorCreateLayer(std::shared_ptr<RSProcessor> processor)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!uniParam) {
        RS_LOGE("RSScreenRenderNodeDrawable::HardCursorCreateLayer uniParam is null");
        return false;
    }
    auto& hardCursorDrawables = uniParam->GetHardCursorDrawables();
    if (hardCursorDrawables.empty()) {
        return false;
    }
    auto iter = std::find_if(hardCursorDrawables.begin(), hardCursorDrawables.end(), [id = GetId()](const auto& node) {
        return std::get<0>(node) == id;
    });
    if (iter == hardCursorDrawables.end()) {
        return false;
    }
    auto& hardCursorDrawable = std::get<2>(*iter);
    if (!hardCursorDrawable) {
        return false;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(hardCursorDrawable->GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("RSScreenRenderNodeDrawable::HardCursorCreateLayer surfaceParams is null");
        return false;
    }
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(hardCursorDrawable);
    if (surfaceDrawable && surfaceParams->GetHardCursorStatus()) {
        processor->CreateLayerForRenderThread(*surfaceDrawable);
        return true;
    }
    return false;
}

void RSScreenRenderNodeDrawable::RenderOverDraw()
{
    bool isEnabled = false;
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (LIKELY(uniParam)) {
        isEnabled = uniParam->IsOverDrawEnabled();
    }
    bool enable = isEnabled && curCanvas_ != nullptr;
    if (!enable) {
        return;
    }
    auto gpuContext = curCanvas_->GetGPUContext();
    if (gpuContext == nullptr) {
        RS_LOGE("RSScreenRenderNodeDrawable::RenderOverDraw failed: need gpu canvas");
        return;
    }
    Drawing::ImageInfo info = Drawing::ImageInfo { curCanvas_->GetWidth(), curCanvas_->GetHeight(),
        Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    auto overdrawSurface = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, info);
    if (!overdrawSurface) {
        RS_LOGE("RSScreenRenderNodeDrawable::RenderOverDraw failed: surface is nullptr");
        return;
    }
    auto overdrawCanvas = std::make_shared<Drawing::OverDrawCanvas>(overdrawSurface->GetCanvas());
    overdrawCanvas->SetGrContext(gpuContext);
    auto paintCanvas = std::make_shared<RSPaintFilterCanvas>(overdrawCanvas.get());
    // traverse all drawable to detect overdraw
    auto params = static_cast<RSScreenRenderParams*>(renderParams_.get());
    if (!params->GetNeedOffscreen()) {
        paintCanvas->ConcatMatrix(params->GetMatrix());
    }
    RS_TRACE_NAME_FMT("RSScreenRenderNodeDrawable::RenderOverDraw");
    RSRenderNodeDrawable::OnDraw(*paintCanvas);
    // show overdraw result in main display
    auto image = overdrawSurface->GetImageSnapshot();
    if (image == nullptr) {
        RS_LOGE("RSScreenRenderNodeDrawable::RenderOverDraw image is nullptr");
        return;
    }
    Drawing::Brush brush;
    auto overdrawColors = RSOverdrawController::GetInstance().GetColorArray();
    auto colorFilter = Drawing::ColorFilter::CreateOverDrawColorFilter(overdrawColors.data());
    Drawing::Filter filter;
    filter.SetColorFilter(colorFilter);
    brush.SetFilter(filter);
    curCanvas_->AttachBrush(brush);
    curCanvas_->DrawImage(*image, 0, 0, Drawing::SamplingOptions());
    curCanvas_->DetachBrush();
}

static inline bool IsForceCommit(uint32_t forceCommitReason, bool needForceUpdateHwcNode,
    bool hasHardCursor)
{
    /* force commit in 3 situations:
     1.force commit reasons is not hwc nodes updated or cursor moved
     2.hwc nodes forced commit and hwc nodes is visible
     3.soft cursor's position changed */
    return (forceCommitReason & ~(ForceCommitReason::FORCED_BY_HWC_UPDATE |
                                  ForceCommitReason::FORCED_BY_POINTER_WINDOW)) ||
           ((forceCommitReason & ForceCommitReason::FORCED_BY_HWC_UPDATE) && needForceUpdateHwcNode) ||
           ((forceCommitReason & ForceCommitReason::FORCED_BY_POINTER_WINDOW) && !hasHardCursor);
}

bool RSScreenRenderNodeDrawable::CheckScreenNodeSkip(
    RSScreenRenderParams& params, std::shared_ptr<RSProcessor> processor)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (params.HasChildCrossNode() && !params.IsMirrorScreen() &&
        uniParam->GetCrossNodeOffScreenStatus() != CrossNodeOffScreenRenderDebugType::DISABLED) {
        return false;
    }
    if (GetSyncDirtyManager()->IsCurrentFrameDirty() ||
        (params.GetMainAndLeashSurfaceDirty() || RSUifirstManager::Instance().HasForceUpdateNode()) ||
        RSMainThread::Instance()->GetDirtyFlag()) {
        return false;
    }
    if (params.IsHDRStatusChanged()) {
        RS_LOGD("RSScreenRenderNodeDrawable::CheckScreenNodeSkip IsHDRStatusChanged");
        return false;
    }

    RS_LOGD("ScreenNode skip");
    RS_TRACE_NAME("ScreenNode skip");
    GpuDirtyRegionCollection::GetInstance().AddSkipProcessFramesNumberForDFX(RSBaseRenderUtil::GetLastSendingPid());
#ifdef OHOS_PLATFORM
    RSJankStatsRenderFrameHelper::GetInstance().SetSkipJankAnimatorFrame(true);
#endif
    auto hardCursorDrawable = RSPointerWindowManager::Instance().GetHardCursorDrawable(GetId());
    bool hasHardCursor = (hardCursorDrawable != nullptr);
    bool hardCursorNeedCommit = (hasHardCursor != hardCursorLastCommitSuccess_);
    auto forceCommitReason = uniParam->GetForceCommitReason();
    bool layersNeedCommit = IsForceCommit(forceCommitReason, params.GetNeedForceUpdateHwcNodes(), hasHardCursor);
    RS_TRACE_NAME_FMT("ScreenNode skip, forceCommitReason: %d, forceUpdateByHwcNodes %d, "
        "byHardCursor: %d", forceCommitReason, params.GetNeedForceUpdateHwcNodes(), hardCursorNeedCommit);
    if (!layersNeedCommit && !hardCursorNeedCommit) {
        RS_TRACE_NAME("ScreenNodeSkip skip commit");
        return true;
    }

    if (!processor->InitForRenderThread(*this, RSUniRenderThread::Instance().GetRenderEngine())) {
        RS_LOGE("RSScreenRenderNodeDrawable::CheckScreenNodeSkip processor init failed");
        return false;
    }
    hardCursorLastCommitSuccess_ = hasHardCursor;
    if (hardCursorDrawable != nullptr) {
        processor->CreateLayerForRenderThread(*hardCursorDrawable);
    }
    auto& hardwareDrawables =
        RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeDrawables();
    for (const auto& [screenNodeId, _, drawable] : hardwareDrawables) {
        if (UNLIKELY(!drawable || !drawable->GetRenderParams()) || screenNodeId != params.GetId()) {
            continue;
        }

        if (drawable->GetRenderParams()->GetHardwareEnabled()) {
            auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(drawable);
            auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable->GetRenderParams().get());
            if (surfaceParams && surfaceParams->GetHwcGlobalPositionEnabled()) {
                surfaceParams->SetOffsetX(offsetX_);
                surfaceParams->SetOffsetY(offsetY_);
                surfaceParams->SetRogWidthRatio(surfaceParams->IsHwcCrossNode() ?
                    params.GetScreenInfo().GetRogWidthRatio() : 1.0f);
            } else {
                surfaceParams->SetOffsetX(0);
                surfaceParams->SetOffsetY(0);
                surfaceParams->SetRogWidthRatio(1.0f);
            }
            // hpae offline
            if (surfaceParams->GetLayerInfo().useDeviceOffline &&
                ProcessOfflineSurfaceDrawable(processor, surfaceDrawable, false)) {
                continue;
            }
            processor->CreateLayerForRenderThread(*surfaceDrawable);
        }
    }
    if (!RSMainThread::Instance()->WaitHardwareThreadTaskExecute()) {
        RS_LOGW("RSScreenRenderNodeDrawable::CheckScreenNodeSkip: hardwareThread task has too many to Execute"
                " TaskNum:[%{public}d]", RSHardwareThread::Instance().GetunExecuteTaskNum());
        RSHardwareThread::Instance().DumpEventQueue();
    }
    processor->ProcessScreenSurfaceForRenderThread(*this);

    // commit RCD layers

    DoScreenRcdTask(params, processor);
    processor->PostProcess();
    return true;
}

void RSScreenRenderNodeDrawable::PostClearMemoryTask() const
{
    auto& unirenderThread = RSUniRenderThread::Instance();
    if (unirenderThread.IsDefaultClearMemroyFinished()) {
        unirenderThread.DefaultClearMemoryCache(); //default clean with no rendering in 5s
        unirenderThread.SetDefaultClearMemoryFinished(false);
    }
}

void RSScreenRenderNodeDrawable::SetScreenNodeSkipFlag(RSRenderThreadParams& uniParam, bool flag)
{
    isScreenNodeSkipStatusChanged_ = (isScreenNodeSkip_ != flag);
    isScreenNodeSkip_ = flag;
    uniParam.SetForceMirrorScreenDirty(isScreenNodeSkipStatusChanged_ && isScreenNodeSkip_);
}

void RSScreenRenderNodeDrawable::CheckFilterCacheFullyCovered(RSSurfaceRenderParams& surfaceParams, RectI screenRect)
{
    surfaceParams.SetFilterCacheFullyCovered(false);
    if (!surfaceParams.IsTransparent() || surfaceParams.GetIsRotating()) {
        RS_OPTIONAL_TRACE_NAME_FMT("CheckFilterCacheFullyCovered NodeId[%" PRIu64 "], isOpaque: %d, isRotating: %d",
            surfaceParams.GetId(), !surfaceParams.IsTransparent(), surfaceParams.GetIsRotating());
        return;
    }
    bool dirtyBelowContainsFilterNode = false;
    for (auto& filterNodeId : surfaceParams.GetVisibleFilterChild()) {
        auto drawableAdapter = DrawableV2::RSRenderNodeDrawableAdapter::GetDrawableById(filterNodeId);
        if (drawableAdapter == nullptr) {
            continue;
        }
        auto filterNodeDrawable = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(drawableAdapter);
        if (filterNodeDrawable == nullptr) {
            RS_LOGD("CheckFilterCacheFullyCovered filter node drawable is nullptr, Name[%{public}s],"
                "NodeId[%" PRIu64 "]", surfaceParams.GetName().c_str(), filterNodeId);
            continue;
        }
        const auto& filterParams = filterNodeDrawable->GetRenderParams();
        if (filterParams == nullptr || !filterParams->HasBlurFilter()) {
            RS_LOGD("CheckFilterCacheFullyCovered filter params is nullptr or has no blur, Name[%{public}s],"
                "NodeId[%" PRIu64 "]", surfaceParams.GetName().c_str(), filterNodeId);
            continue;
        }
        // Filter cache occlusion need satisfy:
        // 1.The filter node global alpha equals 1;
        // 2.There is no invalid filter cache node below, which should take snapshot;
        // 3.The filter node has no global corner;
        // 4.The node type is not EFFECT_NODE;
        if (ROSEN_EQ(filterParams->GetGlobalAlpha(), 1.f) && !dirtyBelowContainsFilterNode &&
            !filterParams->HasGlobalCorner() && filterParams->GetType() != RSRenderNodeType::EFFECT_NODE) {
            surfaceParams.CheckValidFilterCacheFullyCoverTarget(
                filterNodeDrawable->IsFilterCacheValidForOcclusion(),
                filterNodeDrawable->GetFilterCachedRegion(), screenRect);
        }
        RS_OPTIONAL_TRACE_NAME_FMT("CheckFilterCacheFullyCovered NodeId[%" PRIu64 "], globalAlpha: %f, "
            "hasInvalidFilterCacheBefore: %d, hasNoCorner: %d, isNodeTypeCorrect: %d, isCacheValid: %d, "
            "cacheRect: %s", filterNodeId, filterParams->GetGlobalAlpha(), !dirtyBelowContainsFilterNode,
            !filterParams->HasGlobalCorner(), filterParams->GetType() != RSRenderNodeType::EFFECT_NODE,
            filterNodeDrawable->IsFilterCacheValidForOcclusion(),
            filterNodeDrawable->GetFilterCachedRegion().ToString().c_str());
        if (filterParams->GetEffectNodeShouldPaint() && !filterNodeDrawable->IsFilterCacheValidForOcclusion()) {
            dirtyBelowContainsFilterNode = true;
        }
    }
}

void RSScreenRenderNodeDrawable::CheckAndUpdateFilterCacheOcclusionFast()
{
    auto params = static_cast<RSScreenRenderParams*>(renderParams_.get());
    ScreenId paramScreenId = params->GetScreenId();
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        SetDrawSkipType(DrawSkipType::SCREEN_MANAGER_NULL);
        RS_LOGE("RSScreenRenderNodeDrawable::OnDraw ScreenManager is nullptr");
        return;
    }
    ScreenInfo curScreenInfo = screenManager->QueryScreenInfo(paramScreenId);
    CheckAndUpdateFilterCacheOcclusion(*params, curScreenInfo);
    filterCacheOcclusionUpdated_ = true;
}

void RSScreenRenderNodeDrawable::CheckAndUpdateFilterCacheOcclusion(
    RSScreenRenderParams& params, const ScreenInfo& screenInfo)
{
    if (!RSSystemParameters::GetFilterCacheOcculusionEnabled()) {
        return;
    }
    bool isScreenOccluded = false;
    RectI screenRect = {0, 0, screenInfo.width, screenInfo.height};
    // top-down traversal all mainsurface
    // if upper surface reuse filter cache which fully cover whole screen
    // mark lower layers for process skip
    auto& curAllSurfaceDrawables = params.GetAllMainAndLeashSurfaceDrawables();
    for (const auto& adapter : curAllSurfaceDrawables) {
        if (adapter == nullptr || adapter->GetNodeType() != RSRenderNodeType::SURFACE_NODE) {
            RS_LOGD("CheckAndUpdateFilterCacheOcclusion adapter is nullptr or error type");
            continue;
        }
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(adapter);
        if (surfaceNodeDrawable == nullptr) {
            RS_LOGD("CheckAndUpdateFilterCacheOcclusion surfaceNodeDrawable is nullptr");
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        if (surfaceParams == nullptr) {
            RS_LOGD("CheckAndUpdateFilterCacheOcclusion surface params is nullptr");
            continue;
        }

        CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

        if (surfaceParams->IsMainWindowType()) {
            // reset occluded status for all mainwindow
            surfaceParams->SetOccludedByFilterCache(isScreenOccluded);
        }
        isScreenOccluded = isScreenOccluded || surfaceParams->GetFilterCacheFullyCovered();
    }
}

int32_t RSScreenRenderNodeDrawable::GetBufferAge()
{
    return expandRenderFrame_ ? expandRenderFrame_->GetBufferAge() : 0;
}

void RSScreenRenderNodeDrawable::SetDamageRegion(const std::vector<RectI>& rects)
{
    if (expandRenderFrame_ == nullptr) {
        return;
    }
    expandRenderFrame_->SetDamageRegion(rects);
}

void RSScreenRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    Drawing::GPUResourceTag::SetCurrentNodeId(GetId());
    SetDrawSkipType(DrawSkipType::NONE);
    // canvas will generate in every request frame
    (void)canvas;

    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (UNLIKELY(!renderParams_ || !uniParam)) {
        SetDrawSkipType(DrawSkipType::RENDER_PARAMS_OR_UNI_PARAMS_NULL);
        RS_LOGE("RSScreenRenderNodeDrawable::OnDraw renderParams/uniParam is null!");
        return;
    }
    auto params = static_cast<RSScreenRenderParams*>(renderParams_.get());
    if (params->GetChildDisplayCount() == 0) {
        SetDrawSkipType(DrawSkipType::NO_DISPLAY_NODE);
        return;
    }
    auto screenInfo = params->GetScreenInfo();
    if (screenInfo.state == ScreenState::DISABLED) {
        SetDrawSkipType(DrawSkipType::SCREEN_STATE_INVALID);
        return;
    }
    // [Attention] do not return before layer created set false, otherwise will result in buffer not released
    auto& hardwareDrawables = uniParam->GetHardwareEnabledTypeDrawables();
    for (const auto& [screenNodeId, _, drawable] : hardwareDrawables) {
        if (UNLIKELY(!drawable || !drawable->GetRenderParams()) ||
            screenNodeId != params->GetId()) {
            continue;
        }
        drawable->GetRenderParams()->SetLayerCreated(false);
    }
    auto& hardCursorDrawables = uniParam->GetHardCursorDrawables();
    auto iter = std::find_if(hardCursorDrawables.begin(), hardCursorDrawables.end(), [id = GetId()](const auto& node) {
        return std::get<0>(node) == id;
    });
    if (iter != hardCursorDrawables.end()) {
        auto drawable = std::get<2>(*iter);
        if (drawable && drawable->GetRenderParams()) {
            drawable->GetRenderParams()->SetLayerCreated(false);
        }
    }

    // if screen power off, skip on draw, needs to draw one more frame.
    isRenderSkipIfScreenOff_ = RSUniRenderUtil::CheckRenderSkipIfScreenOff(true, params->GetScreenId());
    if (isRenderSkipIfScreenOff_) {
        SetDrawSkipType(DrawSkipType::RENDER_SKIP_IF_SCREEN_OFF);
        return;
    }

    PostClearMemoryTask();

    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        SetDrawSkipType(DrawSkipType::SCREEN_MANAGER_NULL);
        RS_LOGE("RSScreenRenderNodeDrawable::OnDraw ScreenManager is nullptr");
        return;
    }

    if (RSSystemProperties::IsFoldScreenFlag() && screenManager->IsScreenSwitching()) {
        SetDrawSkipType(DrawSkipType::RENDER_SKIP_IF_SCREEN_SWITCHING);
        RS_LOGI("RSScreenRenderNodeDrawable::OnDraw FoldScreenNodeSwitching is true, do not drawframe");
        RS_TRACE_NAME_FMT("RSScreenRenderNodeDrawable FoldScreenNodeSwitching is true");
        return;
    }

    // dfx
    RSRenderNodeDrawable::InitDfxForCacheInfo();
    // set for cache and draw cross node in extended screen model
    uniParam->SetIsMirrorScreen(params->IsMirrorScreen());
    uniParam->SetCurrentVisitDisplayDrawableId(GetId());
    uniParam->SetIsFirstVisitCrossNodeDisplay(params->IsFirstVisitCrossNodeDisplay());
    uniParam->SetCompositeType(params->GetCompositeType());
    params->SetDirtyAlignEnabled(uniParam->IsDirtyAlignEnabled());
    ScreenId paramScreenId = params->GetScreenId();
    offsetX_ = params->GetScreenOffsetX();
    offsetY_ = params->GetScreenOffsetY();
    curDisplayScreenId_ = paramScreenId;
    RS_LOGD("RSScreenRenderNodeDrawable::OnDraw curScreenId=[%{public}" PRIu64 "], "
        "offsetX=%{public}d, offsetY=%{public}d", paramScreenId, offsetX_, offsetY_);

    auto syncDirtyManager = GetSyncDirtyManager();
    const RectI& dirtyRegion = syncDirtyManager->GetCurrentFrameDirtyRegion();
    const auto& activeSurfaceRect = syncDirtyManager->GetActiveSurfaceRect().IsEmpty() ?
        syncDirtyManager->GetSurfaceRect() : syncDirtyManager->GetActiveSurfaceRect();
    uint32_t vsyncRefreshRate = RSMainThread::Instance()->GetVsyncRefreshRate();
    RS_TRACE_NAME_FMT("RSScreenRenderNodeDrawable::OnDraw[%" PRIu64 "][%" PRIu64"] zoomed(%d), "
        "currentFrameDirty(%d, %d, %d, %d), screen(%d, %d), active(%d, %d, %d, %d), vsyncRefreshRate(%u)",
        paramScreenId, GetId(), params->GetZoomed(),
        dirtyRegion.left_, dirtyRegion.top_, dirtyRegion.width_, dirtyRegion.height_,
        screenInfo.width, screenInfo.height,
        activeSurfaceRect.left_, activeSurfaceRect.top_, activeSurfaceRect.width_, activeSurfaceRect.height_,
        vsyncRefreshRate);
    RS_LOGD("RSScreenRenderNodeDrawable::OnDraw node: %{public}" PRIu64 "", GetId());
    ScreenId activeScreenId = HgmCore::Instance().GetActiveScreenId();

    // when set expectedRefreshRate, the vsyncRefreshRate maybe change from 60 to 120
    // so that need change whether equal vsync period and whether use virtual dirty
    if (screenInfo.skipFrameStrategy == SKIP_FRAME_BY_REFRESH_RATE) {
        bool isEqualVsyncPeriod = (vsyncRefreshRate == screenInfo.expectedRefreshRate);
        if (screenInfo.isEqualVsyncPeriod != isEqualVsyncPeriod) {
            screenInfo.isEqualVsyncPeriod = isEqualVsyncPeriod;
            screenManager->SetEqualVsyncPeriod(paramScreenId, isEqualVsyncPeriod);
        }
    }
    screenManager->RemoveForceRefreshTask();

    bool isVirtualExpandComposite =
        params->GetCompositeType() == CompositeType::UNI_RENDER_EXPAND_COMPOSITE;
    if (isVirtualExpandComposite) {
        RSUniDirtyComputeUtil::UpdateVirtualExpandScreenAccumulatedParams(*params, *this);
        if (RSUniDirtyComputeUtil::CheckVirtualExpandScreenSkip(*params, *this)) {
            RS_TRACE_NAME("VirtualExpandScreenNode skip");
            return;
        }
    }

    if (SkipFrame(vsyncRefreshRate, screenInfo)) {
        SetDrawSkipType(DrawSkipType::SKIP_FRAME);
        RS_TRACE_NAME_FMT("SkipFrame, screenId:%lu, strategy:%d, interval:%u, refreshrate:%u", paramScreenId,
            screenInfo.skipFrameStrategy, screenInfo.skipFrameInterval, screenInfo.expectedRefreshRate);
        screenManager->PostForceRefreshTask();
        return;
    }
    if (!screenInfo.isEqualVsyncPeriod) {
        uniParam->SetVirtualDirtyRefresh(true);
    }

    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    if (!processor) {
        SetDrawSkipType(DrawSkipType::CREATE_PROCESSOR_FAIL);
        RS_LOGE("RSScreenRenderNodeDrawable::OnDraw RSProcessor is null!");
        return;
    }

    uniParam->SetRSProcessor(processor);
    auto mirroredDrawable = params->GetMirrorSourceDrawable().lock();
    auto mirroredRenderParams = mirroredDrawable ?
        static_cast<RSScreenRenderParams*>(mirroredDrawable->GetRenderParams().get()) : nullptr;
    if (mirroredRenderParams ||
        params->GetCompositeType() == CompositeType::UNI_RENDER_EXPAND_COMPOSITE) {
        DirtyStatusAutoUpdate dirtyStatusAutoUpdate(*uniParam, params->GetChildDisplayCount() == 1);
        if (!processor->InitForRenderThread(*this, RSUniRenderThread::Instance().GetRenderEngine())) {
            SetDrawSkipType(DrawSkipType::RENDER_ENGINE_NULL);
            syncDirtyManager_->ResetDirtyAsSurfaceSize();
            syncDirtyManager_->UpdateDirty(false);
            RS_LOGE("RSScreenRenderNodeDrawable::OnDraw processor init failed!");
            return;
        }
        if (mirroredRenderParams) {
            offsetX_ = mirroredRenderParams->GetScreenOffsetX();
            offsetY_ = mirroredRenderParams->GetScreenOffsetY();
            RSUniRenderThread::Instance().SetEnableVisibleRect(screenInfo.enableVisibleRect);
            if (screenInfo.enableVisibleRect) {
                const auto& rect = screenManager->GetMirrorScreenVisibleRect(paramScreenId);
                auto curVisibleRect = Drawing::RectI(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
                RSUniRenderThread::Instance().SetVisibleRect(curVisibleRect);
            }
            currentBlackList_ = screenManager->GetVirtualScreenBlackList(paramScreenId);
            RSUniRenderThread::Instance().SetBlackList(currentBlackList_);
            if (params->GetCompositeType() == CompositeType::UNI_RENDER_COMPOSITE) {
                SetDrawSkipType(DrawSkipType::WIRED_SCREEN_PROJECTION);
                RSUniRenderThread::Instance().WaitUntilScreenNodeBufferReleased(*this);
                expandRenderFrame_ = RequestFrame(*params, processor);
                if (!expandRenderFrame_) {
                    SetDrawSkipType(DrawSkipType::REQUEST_FRAME_FAIL);
                    RS_LOGE("RSScreenRenderNodeDrawable::OnDraw failed to request frame");
                    return;
                }
                auto drSurface = expandRenderFrame_->GetFrame()->GetSurface();
                if (!drSurface) {
                    RS_LOGE("RSScreenRenderNodeDrawable::OnDraw DrawingSurface is null");
                    return;
                }
                curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
                RSRenderNodeDrawable::OnDraw(*curCanvas_);
                expandRenderFrame_->Flush();
                processor->ProcessScreenSurfaceForRenderThread(*this);
                HardCursorCreateLayer(processor);
                processor->PostProcess();
                lastVisibleRect_ = curVisibleRect_;
                RSUniRenderThread::Instance().SetVisibleRect(Drawing::RectI());
                expandRenderFrame_ = nullptr;
                return;
            }
            auto virtualProcessor = RSProcessor::ReinterpretCast<RSUniRenderVirtualProcessor>(processor);
            if (!virtualProcessor) {
                RS_LOGE("RSScreenRenderNodeDrawable::OnDraw virtualProcessor is null!");
                return;
            }
            curCanvas_ = virtualProcessor->GetCanvas();
            RSRenderNodeDrawable::OnDraw(*curCanvas_);
            params->ResetVirtualExpandAccumulatedParams();
        } else {
            RS_LOGD("RSScreenRenderNodeDrawable::OnDraw Expand screen.");
            bool isOpDropped = uniParam->IsOpDropped();
            uniParam->SetOpDropped(false);
            auto expandProcessor = RSProcessor::ReinterpretCast<RSUniRenderVirtualProcessor>(processor);
            if (!expandProcessor) {
                SetDrawSkipType(DrawSkipType::EXPAND_PROCESSOR_NULL);
                RS_LOGE("RSScreenRenderNodeDrawable::OnDraw expandProcessor is null!");
                return;
            }
            RSDirtyRectsDfx rsDirtyRectsDfx(*this);
            std::vector<RectI> damageRegionRects;
            // disable expand screen dirty when isEqualVsyncPeriod is false, because the dirty history is incorrect
            if (uniParam->IsExpandScreenDirtyEnabled() && uniParam->IsVirtualDirtyEnabled() &&
                screenInfo.isEqualVsyncPeriod) {
                int32_t bufferAge = expandProcessor->GetBufferAge();
                damageRegionRects = RSUniRenderUtil::MergeDirtyHistory(
                    *this, bufferAge, screenInfo, rsDirtyRectsDfx, *params);
                uniParam->Reset();
                if (!uniParam->IsVirtualDirtyDfxEnabled()) {
                    expandProcessor->SetDirtyInfo(damageRegionRects);
                }
            } else {
                std::vector<RectI> emptyRects = {};
                expandProcessor->SetRoiRegionToCodec(emptyRects);
            }
            rsDirtyRectsDfx.SetVirtualDirtyRects(damageRegionRects, screenInfo);
            curCanvas_ = expandProcessor->GetCanvas();
            curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
            RSRenderNodeDrawable::OnDraw(*curCanvas_);
            params->ResetVirtualExpandAccumulatedParams();
            auto targetSurfaceRenderNodeDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
                params->GetTargetSurfaceRenderNodeDrawable().lock());
            if ((targetSurfaceRenderNodeDrawable || params->HasMirrorScreen()) && curCanvas_->GetSurface()) {
                RS_TRACE_NAME("DrawExpandScreen cacheImgForMultiScreenView");
                cacheImgForMultiScreenView_ = curCanvas_->GetSurface()->GetImageSnapshot();
            } else {
                cacheImgForMultiScreenView_ = nullptr;
            }
            // Restore the initial state of the canvas to avoid state accumulation
            curCanvas_->RestoreToCount(0);
            if (curCanvas_) {
                rsDirtyRectsDfx.OnDrawVirtual(*curCanvas_);
            }
            uniParam->SetOpDropped(isOpDropped);
        }
        DrawCurtainScreen();
        processor->PostProcess();
        SetDrawSkipType(DrawSkipType::MIRROR_DRAWABLE_SKIP);
        return;
    }

    bool isHdrOn = params->GetHDRPresent();
    // 0 means defalut hdrBrightnessRatio
    float hdrBrightnessRatio = RSLuminanceControl::Get().GetHdrBrightnessRatio(paramScreenId, 0);
#ifdef RS_ENABLE_OVERLAY_DISPLAY // only for TV
    /*
     * Force hdrBrightnessRatio to be set to 1.0 when overlay_display display enabled.
     * Do not change pixel value; the pixel value is encoded when overlay display enabled.
     * If hdrBrightnessRatio is not equal 1.0, DSS will change the pixel value, so the
     * decoded pixels is wrong and display is abnormal.
     */
    if (RSOverlayDisplayManager::Instance().IsOverlayDisplayEnableForCurrentVsync()) {
        hdrBrightnessRatio = 1.0f;
    }
#endif
    if (!isHdrOn) {
        params->SetBrightnessRatio(hdrBrightnessRatio);
        hdrBrightnessRatio = 1.0f;
    }
    params->SetHdrBrightnessRatio(hdrBrightnessRatio);
    RS_LOGD("RSScreenRenderNodeDrawable::OnDraw HDRDraw isHdrOn: %{public}d, BrightnessRatio: %{public}f",
        isHdrOn, hdrBrightnessRatio);

    // checkScreenNodeSkip need to be judged at the end
    if (RSAncoManager::Instance()->GetAncoHebcStatus() == AncoHebcStatus::INITIAL &&
        uniParam->IsOpDropped() && CheckScreenNodeSkip(*params, processor)) {
        SetDrawSkipType(DrawSkipType::SCREEN_NODE_SKIP);
        RSMainThread::Instance()->SetFrameIsRender(false);
        SetScreenNodeSkipFlag(*uniParam, true);
        return;
    }
    SetScreenNodeSkipFlag(*uniParam, false);
    RSMainThread::Instance()->SetFrameIsRender(true);

    if (filterCacheOcclusionUpdated_) {
        filterCacheOcclusionUpdated_ = false;
    } else {
        CheckAndUpdateFilterCacheOcclusion(*params, screenInfo);
    }
    if (isHdrOn) {
        params->SetNewPixelFormat(RSHdrUtil::GetRGBA1010108Enabled() ?
            GRAPHIC_PIXEL_FMT_RGBA_1010108 : GRAPHIC_PIXEL_FMT_RGBA_1010102);
    }
    // hpae offline: post offline task
    CheckAndPostAsyncProcessOfflineTask();
    RSUniRenderThread::Instance().WaitUntilScreenNodeBufferReleased(*this);
    // displayNodeSp to get  rsSurface witch only used in renderThread
    auto renderFrame = RequestFrame(*params, processor);
    if (!renderFrame) {
        SetDrawSkipType(DrawSkipType::REQUEST_FRAME_FAIL);
        RS_LOGE("RSScreenRenderNodeDrawable::OnDraw failed to request frame");
        return;
    }

    uniParam->SetSLRScaleManager(nullptr);
    if (screenInfo.isSamplingOn) {
        auto scaleManager = std::make_shared<RSSLRScaleFunction>(
            screenInfo.phyWidth, screenInfo.phyHeight, screenInfo.width, screenInfo.height);
        screenInfo.samplingDistance = scaleManager->GetKernelSize();
        uniParam->SetSLRScaleManager(scaleManager);
    }
    RSDirtyRectsDfx rsDirtyRectsDfx(*this);
    std::vector<RectI> damageRegionrects;
    std::vector<RectI> curFrameVisibleRegionRects;
    Drawing::Region clipRegion;
    if (uniParam->IsPartialRenderEnabled()) {
        damageRegionrects = RSUniRenderUtil::MergeDirtyHistory(
            *this, renderFrame->GetBufferAge(), screenInfo, rsDirtyRectsDfx, *params);
        curFrameVisibleRegionRects = RSUniDirtyComputeUtil::GetCurrentFrameVisibleDirty(*this, screenInfo, *params);
        uniParam->Reset();
        clipRegion = GetFlippedRegion(damageRegionrects, screenInfo);
        RS_TRACE_NAME_FMT("SetDamageRegion damageRegionrects num: %zu, info: %s",
            damageRegionrects.size(), RectVectorToString(damageRegionrects).c_str());
        if (!uniParam->IsRegionDebugEnabled()) {
            renderFrame->SetDamageRegion(damageRegionrects);
        }
    }

    auto drSurface = renderFrame->GetFrame()->GetSurface();
    if (!drSurface) {
        SetDrawSkipType(DrawSkipType::SURFACE_NULL);
        RS_LOGE("RSScreenRenderNodeDrawable::OnDraw DrawingSurface is null");
        return;
    }

    curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
    if (!curCanvas_) {
        SetDrawSkipType(DrawSkipType::CANVAS_NULL);
        RS_LOGE("RSScreenRenderNodeDrawable::OnDraw failed to create canvas");
        return;
    }
    curCanvas_->SetDrawnRegion(params->GetDrawnRegion());
    curCanvas_->SetTargetColorGamut(params->GetNewColorSpace());
    curCanvas_->SetScreenId(paramScreenId);
    curCanvas_->SetHdrOn(isHdrOn);
    curCanvas_->SetDisableFilterCache(params->GetZoomed());

#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (RSOpincDrawCache::IsAutoCacheEnable()) {
        RSOpincDrawCache::SetScreenRectInfo({0, 0, screenInfo.width, screenInfo.height});
    }
#endif
    // canvas draw
    {
        {
            RSSkpCaptureDfx capture(curCanvas_);
            Drawing::AutoCanvasRestore acr(*curCanvas_, true);

            if (uniParam->IsOpDropped()) {
                if (uniParam->IsDirtyAlignEnabled()) {
                    RS_TRACE_NAME_FMT("dirty align enabled and no clip operation");
                    curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
                } else {
                    uniParam->SetClipRegion(clipRegion);
                    ClipRegion(*curCanvas_, clipRegion);
                }
            } else {
                curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
            }

            curCanvas_->SetHighContrast(RSUniRenderThread::Instance().IsHighContrastTextModeOn());
            ClearCanvasStencil(*curCanvas_, *params, *uniParam);
            // cpu boost feature start
            ffrt_cpu_boost_start(CPUBOOST_START_POINT + 1);
            RSRenderNodeDrawable::OnDraw(*curCanvas_);
            // cpu boost feature end
            ffrt_cpu_boost_end(CPUBOOST_START_POINT + 1);
            curCanvas_->Save();
            curCanvas_->ResetMatrix();
            if (screenInfo.isSamplingOn) {
                curCanvas_->ConcatMatrix(params->GetSlrMatrix());
            }
            rsDirtyRectsDfx.OnDraw(*curCanvas_);
            curCanvas_->Restore();
            DrawCurtainScreen();
            bool displayP3Enable = (params->GetNewColorSpace() == GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
            RSUniRenderUtil::SwitchColorFilter(*curCanvas_, hdrBrightnessRatio, displayP3Enable);
#ifdef RS_ENABLE_OVERLAY_DISPLAY
            // add post filter for TV overlay display conversion
            RSOverlayDisplayManager::Instance().PostProcFilter(*curCanvas_);
#endif
            auto dirtyManager = GetSyncDirtyManager();
            const auto& activeRect = dirtyManager->GetActiveSurfaceRect();
            if (!activeRect.IsEmpty() && (!dirtyManager->GetDirtyRegion().IsInsideOf(activeRect) ||
                !uniParam->IsPartialRenderEnabled() || uniParam->IsRegionDebugEnabled())) {
                RS_TRACE_NAME_FMT("global dirty region:[%s] is not inside of active surface rect:[%s], "
                    "clear extra area to black", dirtyManager->GetDirtyRegion().ToString().c_str(),
                    activeRect.ToString().c_str());
                curCanvas_->Save();
                auto activeRegion = RSUniDirtyComputeUtil::ScreenIntersectDirtyRects(
                    Occlusion::Region(activeRect), screenInfo);
                curCanvas_->ClipRegion(GetFlippedRegion(activeRegion, screenInfo), Drawing::ClipOp::DIFFERENCE);
                curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
                curCanvas_->Restore();
            }
        }

        if (RSSystemProperties::GetDrawMirrorCacheImageEnabled() && uniParam->HasMirrorDisplay() &&
            curCanvas_->GetSurface() != nullptr) {
            cacheImgForMultiScreenView_ = curCanvas_->GetSurface()->GetImageSnapshot();
        } else {
            cacheImgForMultiScreenView_ = nullptr;
        }
    }
    RenderOverDraw();
    RSMainThread::Instance()->SetDirtyFlag(false);

    if (Drawing::PerformanceCaculate::GetDrawingFlushPrint()) {
        RS_LOGI("Drawing Performance Flush start %{public}lld", Drawing::PerformanceCaculate::GetUpTime(false));
    }
    RS_TRACE_BEGIN("RSScreenRenderNodeDrawable Flush");
    RsFrameReport::GetInstance().CheckBeginFlushPoint();
    Drawing::GPUResourceTag::SetCurrentNodeId(GetId());

    renderFrame->Flush();
    RS_TRACE_END();
    if (Drawing::PerformanceCaculate::GetDrawingFlushPrint()) {
        RS_LOGI("Drawing Performance Flush end %{public}lld", Drawing::PerformanceCaculate::GetUpTime(false));
        Drawing::PerformanceCaculate::ResetCaculateTimeCount();
    }

    // process round corner display
    DoScreenRcdTask(*params, processor);

    if (!RSMainThread::Instance()->WaitHardwareThreadTaskExecute()) {
        RS_LOGW("RSScreenRenderNodeDrawable::ondraw: hardwareThread task has too many to Execute"
                " TaskNum:[%{public}d]", RSHardwareThread::Instance().GetunExecuteTaskNum());
        RSHardwareThread::Instance().DumpEventQueue();
    }

    RS_TRACE_BEGIN("RSScreenRenderNodeDrawable CommitLayer");
    for (const auto& [screenNodeId, _, drawable] : hardwareDrawables) {
        if (UNLIKELY(!drawable || !drawable->GetRenderParams())) {
            continue;
        }
        auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(drawable);
        if (!surfaceDrawable || screenNodeId != params->GetId()) {
            continue;
        }
        if (drawable->GetRenderParams()->GetHardwareEnabled()) {
            auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable->GetRenderParams().get());
            if (surfaceParams && surfaceParams->GetHwcGlobalPositionEnabled()) {
                surfaceParams->SetOffsetX(offsetX_);
                surfaceParams->SetOffsetY(offsetY_);
                surfaceParams->SetRogWidthRatio(surfaceParams->IsHwcCrossNode() ?
                    params->GetScreenInfo().GetRogWidthRatio() : 1.0f);
            } else {
                surfaceParams->SetOffsetX(0);
                surfaceParams->SetOffsetY(0);
                surfaceParams->SetRogWidthRatio(1.0f);
            }
            // hpae offline: wait task and create layer
            if (surfaceParams->GetLayerInfo().useDeviceOffline &&
                ProcessOfflineSurfaceDrawable(processor, surfaceDrawable, true)) {
                continue;
            }
            processor->CreateLayerForRenderThread(*surfaceDrawable);
        }
    }
    HardCursorCreateLayer(processor);
    if (screenInfo.activeRect.IsEmpty() ||
        screenInfo.activeRect == RectI(0, 0, screenInfo.width, screenInfo.height) ||
        DirtyRegionParam::IsComposeDirtyRegionEnableInPartialDisplay()) {
        if (uniParam->IsRegionDebugEnabled()) {
            std::vector<RectI> emptyRegionRects = {};
            SetDirtyRects(emptyRegionRects);
        } else {
            SetDirtyRects(RSSystemProperties::GetOptimizeHwcComposeAreaEnabled() ?
                curFrameVisibleRegionRects : damageRegionrects);
        }
    } else {
        SetDirtyRects({GetSyncDirtyManager()->GetRectFlipWithinSurface(screenInfo.activeRect)});
    }
    processor->ProcessScreenSurfaceForRenderThread(*this);
    processor->PostProcess();
    RS_TRACE_END();

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    if (RSMagicPointerRenderManager::GetInstance().GetPointerColorInversionEnabled() && !mirroredDrawable) {
        RSMagicPointerRenderManager::GetInstance().ProcessColorPicker(processor, curCanvas_->GetGPUContext());
        RSMagicPointerRenderManager::GetInstance().SetCacheImgForPointer(nullptr);
    }
#endif
}

void RSScreenRenderNodeDrawable::ClearCanvasStencil(RSPaintFilterCanvas& canvas,
    RSScreenRenderParams& params, RSRenderThreadParams& uniParam)
{
    if (!uniParam.IsStencilPixelOcclusionCullingEnabled()) {
        return;
    }
    auto topSurfaceOpaqueRects = params.GetTopSurfaceOpaqueRects();
    if (topSurfaceOpaqueRects.empty()) {
        return;
    }
    auto screenInfo = params.GetScreenInfo();
    RS_OPTIONAL_TRACE_NAME_FMT("ClearStencil, rect(0, 0, %d, %d), stencilVal: 0",
        screenInfo.width, screenInfo.height);
    canvas.ClearStencil({0, 0, screenInfo.width, screenInfo.height}, 0);
    std::reverse(topSurfaceOpaqueRects.begin(), topSurfaceOpaqueRects.end());
    auto maxStencilVal = TOP_OCCLUSION_SURFACES_NUM * OCCLUSION_ENABLE_SCENE_NUM;
    canvas.SetMaxStencilVal(maxStencilVal);
    for (size_t i = 0; i < topSurfaceOpaqueRects.size(); i++) {
        Drawing::RectI rect {topSurfaceOpaqueRects[i].left_,
            topSurfaceOpaqueRects[i].top_,
            topSurfaceOpaqueRects[i].right_,
            topSurfaceOpaqueRects[i].bottom_};
        auto stencilVal = OCCLUSION_ENABLE_SCENE_NUM *
            (TOP_OCCLUSION_SURFACES_NUM - topSurfaceOpaqueRects.size() + i + 1);
        RS_OPTIONAL_TRACE_NAME_FMT("ClearStencil, rect(%" PRId32 ", %" PRId32 ", %" PRId32 ", %" PRId32 "), "
            "stencilVal: %zu", rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight(), stencilVal);
        canvas.ClearStencil(rect, static_cast<uint32_t>(stencilVal));
    }
}

void RSScreenRenderNodeDrawable::DrawCurtainScreen() const
{
    if (!RSUniRenderThread::Instance().IsCurtainScreenOn() || !curCanvas_) {
        return;
    }
    RS_TRACE_FUNC();
    curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
}

#ifndef ROSEN_CROSS_PLATFORM
bool RSScreenRenderNodeDrawable::CreateSurface(sptr<IBufferConsumerListener> listener)
{
    auto consumer = surfaceHandler_->GetConsumer();
    if (consumer != nullptr && surface_ != nullptr) {
        RS_LOGI("RSScreenRenderNodeDrawable::CreateSurface already created, return");
        return true;
    }
    consumer = IConsumerSurface::Create("ScreenNode");
    if (consumer == nullptr) {
        RS_LOGE("RSScreenRenderNodeDrawable::CreateSurface get consumer surface fail");
        return false;
    }
    SurfaceError ret = consumer->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSScreenRenderNodeDrawable::CreateSurface RegisterConsumerListener fail");
        return false;
    }
    consumerListener_ = listener;
    auto producer = consumer->GetProducer();
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);
    if (!surface) {
        RS_LOGE("RSScreenRenderNodeDrawable::CreateSurface CreateSurfaceAsProducer fail");
        return false;
    }
    surface->SetQueueSize(BUFFER_SIZE);
    auto client = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    surface_ = client->CreateRSSurface(surface);
    RS_LOGI("RSScreenRenderNodeDrawable::CreateSurface end");
    surfaceCreated_ = true;
    surfaceHandler_->SetConsumer(consumer);
    return true;
}
#endif

bool RSScreenRenderNodeDrawable::SkipFrameByInterval(uint32_t refreshRate, uint32_t skipFrameInterval)
{
    if (refreshRate == 0 || skipFrameInterval <= 1) {
        return false;
    }
    int64_t currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    // the skipFrameInterval is equal to 60 divide the virtual screen refresh rate
    int64_t refreshInterval = currentTime - lastRefreshTime_;
    // 1000000000ns == 1s, 110/100 allows 10% over.
    bool needSkip = refreshInterval < (1000000000LL / refreshRate) * (skipFrameInterval - 1) * 110 / 100;
    if (!needSkip) {
        lastRefreshTime_ = currentTime;
    }
    return needSkip;
}

bool RSScreenRenderNodeDrawable::SkipFrameByRefreshRate(uint32_t refreshRate, uint32_t expectedRefreshRate)
{
    if (refreshRate == 0 || expectedRefreshRate == 0 || refreshRate == expectedRefreshRate) {
        return false;
    }
    int64_t currentTime = RSMainThread::Instance()->GetCurrentVsyncTime();
    int64_t minFrameInterval = 1000000000LL / expectedRefreshRate;
    if (minFrameInterval == 0) {
        return false;
    }
    // lastRefreshTime_ is next frame expected refresh time for virtual display
    if (lastRefreshTime_ <= 0) {
        lastRefreshTime_ = currentTime + minFrameInterval;
        return false;
    }
    if (currentTime < (lastRefreshTime_ - MAX_JITTER_NS)) {
        return true;
    }
    int64_t intervalNums = (currentTime - lastRefreshTime_ + MAX_JITTER_NS) / minFrameInterval;
    lastRefreshTime_ += (intervalNums + 1) * minFrameInterval;
    return false;
}

bool RSScreenRenderNodeDrawable::SkipFrame(uint32_t refreshRate, ScreenInfo screenInfo)
{
    bool needSkip = false;
    switch (screenInfo.skipFrameStrategy) {
        case SKIP_FRAME_BY_INTERVAL:
            needSkip = SkipFrameByInterval(refreshRate, screenInfo.skipFrameInterval);
            break;
        case SKIP_FRAME_BY_REFRESH_RATE:
            needSkip = SkipFrameByRefreshRate(refreshRate, screenInfo.expectedRefreshRate);
            break;
        default:
            break;
    }
    return needSkip;
}

bool RSScreenRenderNodeDrawable::ProcessOfflineSurfaceDrawable(
    const std::shared_ptr<RSProcessor>& processor,
    std::shared_ptr<RSSurfaceRenderNodeDrawable>& surfaceDrawable, bool async)
{
    if (processor->ProcessOfflineLayer(surfaceDrawable, async)) {
        // use offline layer to display
        return true;
    }
    // reset offline tag, use original layer to display
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());
    surfaceParams->SetUseDeviceOffline(false);
    return false;
}

void RSScreenRenderNodeDrawable::CheckAndPostAsyncProcessOfflineTask()
{
    auto& hardwareDrawables =
        RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeDrawables();
    for (const auto& [screenNodeId, _, drawable] : hardwareDrawables) {
        if (UNLIKELY(!drawable || !drawable->GetRenderParams())) {
            continue;
        }
        auto params = static_cast<RSScreenRenderParams*>(renderParams_.get());
        if (screenNodeId != params->GetId()) {
            continue;
        }
        auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(drawable);
        if (surfaceDrawable->GetRenderParams()->GetHardwareEnabled() &&
            surfaceDrawable->GetRenderParams()->GetLayerInfo().useDeviceOffline) {
            if (!RSHpaeOfflineProcessor::GetOfflineProcessor().PostProcessOfflineTask(
                surfaceDrawable, RSUniRenderThread::Instance().GetVsyncId())) {
                RS_LOGW("RSUniRenderProcessor::ProcessSurface: post offline task failed, go redraw");
            }
        }
    }
}

} // namespace OHOS::Rosen::DrawableV2
