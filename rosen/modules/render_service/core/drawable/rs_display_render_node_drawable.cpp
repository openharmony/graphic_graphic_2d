/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_display_render_node_drawable.h"

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
#ifdef RS_ENABLE_OVERLAY_DISPLAY
#include "feature/overlay_display/rs_overlay_display_manager.h"
#endif
#ifdef RS_ENABLE_TV_PQ_METADATA
#include "feature/tv_metadata/rs_tv_metadata_manager.h"
#endif
#include "hgm_core.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_display_render_params.h"
#include "params/rs_surface_render_params.h"
#include "feature/anco_manager/rs_anco_manager.h"
#include "feature/dirty/rs_uni_dirty_compute_util.h"
#include "feature/drm/rs_drm_util.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node_drawable.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "feature/round_corner_display/rs_message_bus.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/rs_display_render_node.h"
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
#include "property/rs_point_light_manager.h"
#include "render/rs_pixel_map_util.h"
#include "screen_manager/rs_screen_manager.h"
#include "static_factory.h"
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
namespace OHOS::Rosen::DrawableV2 {
namespace {
constexpr const char* CLEAR_GPU_CACHE = "ClearGpuCache";
constexpr const char* DEFAULT_CLEAR_GPU_CACHE = "DefaultClearGpuCache";
constexpr int32_t NO_SPECIAL_LAYER = 0;
constexpr int32_t HAS_SPECIAL_LAYER = 1;
constexpr int32_t CAPTURE_WINDOW = 2; // To be deleted after captureWindow being deleted
constexpr int64_t MAX_JITTER_NS = 2000000; // 2ms
constexpr const float HALF = 2.0f;

std::string RectVectorToString(std::vector<RectI>& regionRects)
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
}}  // namespace

// Rcd node will be handled by RS tree in OH 6.0 rcd refactoring, should remove this later
void DoScreenRcdTask(RSDisplayRenderParams& params, std::shared_ptr<RSProcessor> &processor)
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

RSDisplayRenderNodeDrawable::Registrar RSDisplayRenderNodeDrawable::instance_;
std::shared_ptr<Drawing::RuntimeEffect> RSDisplayRenderNodeDrawable::brightnessAdjustmentShaderEffect_ = nullptr;

RSDisplayRenderNodeDrawable::RSDisplayRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node)), surfaceHandler_(std::make_shared<RSSurfaceHandler>(nodeId_)),
      syncDirtyManager_(std::make_shared<RSDirtyRegionManager>(true))
{}

RSRenderNodeDrawable::Ptr RSDisplayRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSDisplayRenderNodeDrawable(std::move(node));
}

std::unique_ptr<RSRenderFrame> RSDisplayRenderNodeDrawable::RequestFrame(
    RSDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor)
{
    RS_TRACE_NAME("RSDisplayRenderNodeDrawable:RequestFrame");
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (UNLIKELY(!renderEngine)) {
        RS_LOGE("RSDisplayRenderNodeDrawable::RequestFrame RenderEngine is null!");
        return nullptr;
    }

    if (!processor->InitForRenderThread(*this, INVALID_SCREEN_ID, renderEngine)) {
        RS_LOGE("RSDisplayRenderNodeDrawable::RequestFrame processor InitForRenderThread failed!");
        return nullptr;
    }

    if (!IsSurfaceCreated()) {
        sptr<IBufferConsumerListener> listener = new RSUniRenderListener(surfaceHandler_);
        if (!CreateSurface(listener)) {
            RS_LOGE("RSDisplayRenderNodeDrawable::RequestFrame CreateSurface failed");
            return nullptr;
        }
    }

    auto rsSurface = GetRSSurface();
    if (!rsSurface) {
        RS_LOGE("RSDisplayRenderNodeDrawable::RequestFrame No RSSurface found");
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
        RS_LOGE("RSDisplayRenderNodeDrawable::RequestFrame renderEngine requestFrame is null");
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
        RS_TRACE_NAME("RSDisplayDrawable: clipPath");
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

bool RSDisplayRenderNodeDrawable::HardCursorCreateLayer(std::shared_ptr<RSProcessor> processor)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!uniParam) {
        RS_LOGE("RSDisplayRenderNodeDrawable::HardCursorCreateLayer uniParam is null");
        return false;
    }
    auto& hardCursorDrawables = uniParam->GetHardCursorDrawables();
    if (hardCursorDrawables.empty()) {
        return false;
    }
    auto iter = hardCursorDrawables.find(GetId());
    if (iter == hardCursorDrawables.end()) {
        return false;
    }
    auto& hardCursorDrawable = iter->second;
    if (!hardCursorDrawable) {
        return false;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(hardCursorDrawable->GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("RSDisplayRenderNodeDrawable::HardCursorCreateLayer surfaceParams is null");
        return false;
    }
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(hardCursorDrawable);
    if (surfaceDrawable && surfaceParams->GetHardCursorStatus()) {
        processor->CreateLayerForRenderThread(*surfaceDrawable);
        return true;
    }
    return false;
}

void RSDisplayRenderNodeDrawable::RenderOverDraw()
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
        RS_LOGE("RSDisplayRenderNodeDrawable::RenderOverDraw failed: need gpu canvas");
        return;
    }
    Drawing::ImageInfo info = Drawing::ImageInfo { curCanvas_->GetWidth(), curCanvas_->GetHeight(),
        Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    auto overdrawSurface = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, info);
    if (!overdrawSurface) {
        RS_LOGE("RSDisplayRenderNodeDrawable::RenderOverDraw failed: surface is nullptr");
        return;
    }
    auto overdrawCanvas = std::make_shared<Drawing::OverDrawCanvas>(overdrawSurface->GetCanvas());
    overdrawCanvas->SetGrContext(gpuContext);
    auto paintCanvas = std::make_shared<RSPaintFilterCanvas>(overdrawCanvas.get());
    // traverse all drawable to detect overdraw
    auto params = static_cast<RSDisplayRenderParams*>(renderParams_.get());
    if (!params->GetNeedOffscreen()) {
        paintCanvas->ConcatMatrix(params->GetMatrix());
    }
    RS_TRACE_NAME_FMT("RSDisplayRenderNodeDrawable::RenderOverDraw");
    RSRenderNodeDrawable::OnDraw(*paintCanvas);
    DrawWatermarkIfNeed(*params, *paintCanvas);
    // show overdraw result in main display
    auto image = overdrawSurface->GetImageSnapshot();
    if (image == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::RenderOverDraw image is nullptr");
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

bool RSDisplayRenderNodeDrawable::CheckDisplayNodeSkip(
    RSDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor)
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
        RS_LOGD("RSDisplayRenderNodeDrawable::CheckDisplayNodeSkip IsHDRStatusChanged");
        return false;
    }

    RS_LOGD("DisplayNode skip");
    RS_TRACE_NAME("DisplayNode skip");
    GpuDirtyRegionCollection::GetInstance().AddSkipProcessFramesNumberForDFX(RSBaseRenderUtil::GetLastSendingPid());
#ifdef OHOS_PLATFORM
    RSJankStatsRenderFrameHelper::GetInstance().SetSkipJankAnimatorFrame(true);
#endif
    auto hardCursorDrawable = RSPointerWindowManager::Instance().GetHardCursorDrawable(GetId());
    bool hasHardCursor = (hardCursorDrawable != nullptr);
    bool hardCursorNeedCommit = (hasHardCursor != hardCursorLastCommitSuccess_);
    auto forceCommitReason = uniParam->GetForceCommitReason();
    bool layersNeedCommit = IsForceCommit(forceCommitReason, params.GetNeedForceUpdateHwcNodes(), hasHardCursor);
    RS_TRACE_NAME_FMT("DisplayNode skip, forceCommitReason: %d, forceUpdateByHwcNodes %d, "
        "byHardCursor: %d", forceCommitReason, params.GetNeedForceUpdateHwcNodes(), hardCursorNeedCommit);
    if (!layersNeedCommit && !hardCursorNeedCommit) {
        RS_TRACE_NAME("DisplayNodeSkip skip commit");
        return true;
    }

    if (!processor->InitForRenderThread(*this, INVALID_SCREEN_ID, RSUniRenderThread::Instance().GetRenderEngine())) {
        RS_LOGE("RSDisplayRenderNodeDrawable::CheckDisplayNodeSkip processor init failed");
        return false;
    }
    hardCursorLastCommitSuccess_ = hasHardCursor;
    if (hardCursorDrawable != nullptr) {
        processor->CreateLayerForRenderThread(*hardCursorDrawable);
    }
    auto& hardwareDrawables =
        RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeDrawables();
    for (const auto& [displayNodeId, drawable] : hardwareDrawables) {
        if (UNLIKELY(!drawable || !drawable->GetRenderParams()) || displayNodeId != params.GetId()) {
            continue;
        }

        if (drawable->GetRenderParams()->GetHardwareEnabled()) {
            auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(drawable);
            auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable->GetRenderParams().get());
            if (surfaceParams && surfaceParams->GetHwcGlobalPositionEnabled()) {
                surfaceParams->SetOffsetX(offsetX_);
                surfaceParams->SetOffsetY(offsetY_);
                surfaceParams->SetRogWidthRatio(surfaceParams->IsDRMCrossNode() ?
                    params.GetScreenInfo().GetRogWidthRatio() : 1.0f);
            } else {
                surfaceParams->SetOffsetX(0);
                surfaceParams->SetOffsetY(0);
                surfaceParams->SetRogWidthRatio(1.0f);
            }
            processor->CreateLayerForRenderThread(*surfaceDrawable);
        }
    }
    if (!RSMainThread::Instance()->WaitHardwareThreadTaskExecute()) {
        RS_LOGW("RSDisplayRenderNodeDrawable::CheckDisplayNodeSkip: hardwareThread task has too many to Execute"
                " TaskNum:[%{public}d]", RSHardwareThread::Instance().GetunExecuteTaskNum());
        RSHardwareThread::Instance().DumpEventQueue();
    }
    processor->ProcessDisplaySurfaceForRenderThread(*this);

    // commit RCD layers
    DoScreenRcdTask(params, processor);
    processor->PostProcess();
    return true;
}

void RSDisplayRenderNodeDrawable::PostClearMemoryTask() const
{
    auto& unirenderThread = RSUniRenderThread::Instance();
    if (unirenderThread.IsDefaultClearMemroyFinished()) {
        unirenderThread.DefaultClearMemoryCache(); //default clean with no rendering in 5s
        unirenderThread.SetDefaultClearMemoryFinished(false);
    }
}

void RSDisplayRenderNodeDrawable::SetDisplayNodeSkipFlag(RSRenderThreadParams& uniParam, bool flag)
{
    isDisplayNodeSkipStatusChanged_ = (isDisplayNodeSkip_ != flag);
    isDisplayNodeSkip_ = flag;
    uniParam.SetForceMirrorScreenDirty(isDisplayNodeSkipStatusChanged_ && isDisplayNodeSkip_);
}

void RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(RSSurfaceRenderParams& surfaceParams, RectI screenRect)
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

void RSDisplayRenderNodeDrawable::CheckAndUpdateFilterCacheOcclusion(
    RSDisplayRenderParams& params, ScreenInfo& screenInfo)
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

void RSDisplayRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    Drawing::GPUResourceTag::SetCurrentNodeId(GetId());
    std::shared_ptr<Drawing::GPUContext> gpuContext = nullptr;
    auto realTid = gettid();
    if (realTid == RSUniRenderThread::Instance().GetTid()) {
        gpuContext = RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext()->GetSharedDrGPUContext();
    } else {
        gpuContext = RSSubThreadManager::Instance()->GetGrContextFromSubThread(realTid);
    }
    RSTagTracker tagTracker(gpuContext, RSTagTracker::TAGTYPE::TAG_UNTAGGED);
    SetDrawSkipType(DrawSkipType::NONE);
    // canvas will generate in every request frame
    (void)canvas;

    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (UNLIKELY(!renderParams_ || !uniParam)) {
        SetDrawSkipType(DrawSkipType::RENDER_PARAMS_OR_UNI_PARAMS_NULL);
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw renderParams/uniParam is null!");
        return;
    }
    auto params = static_cast<RSDisplayRenderParams*>(renderParams_.get());

    // [Attention] do not return before layer created set false, otherwise will result in buffer not released
    auto& hardwareDrawables = uniParam->GetHardwareEnabledTypeDrawables();
    for (const auto& [displayNodeId, drawable] : hardwareDrawables) {
        if (UNLIKELY(!drawable || !drawable->GetRenderParams()) ||
            displayNodeId != params->GetId()) {
            continue;
        }
        drawable->GetRenderParams()->SetLayerCreated(false);
    }
    auto& hardCursorDrawables = uniParam->GetHardCursorDrawables();
    if (!hardCursorDrawables.empty()) {
        auto iter = hardCursorDrawables.find(GetId());
        if (iter != hardCursorDrawables.end() && iter->second && iter->second->GetRenderParams()) {
            iter->second->GetRenderParams()->SetLayerCreated(false);
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
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw ScreenManager is nullptr");
        return;
    }

    if (RSSystemProperties::IsFoldScreenFlag() && screenManager->IsScreenSwitching()) {
        SetDrawSkipType(DrawSkipType::RENDER_SKIP_IF_SCREEN_SWITCHING);
        RS_LOGI("RSDisplayRenderNodeDrawable::OnDraw FoldScreenNodeSwitching is true, do not drawframe");
        RS_TRACE_NAME_FMT("RSDisplayRenderNodeDrawable FoldScreenNodeSwitching is true");
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
    offsetX_ = params->GetDisplayOffsetX();
    offsetY_ = params->GetDisplayOffsetY();
    curDisplayScreenId_ = paramScreenId;
    RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw curScreenId=[%{public}" PRIu64 "], "
        "offsetX=%{public}d, offsetY=%{public}d", paramScreenId, offsetX_, offsetY_);
    SetScreenRotationForPointLight(*params);

    auto syncDirtyManager = GetSyncDirtyManager();
    const RectI& dirtyRegion = syncDirtyManager->GetCurrentFrameDirtyRegion();
    const auto& activeSurfaceRect = syncDirtyManager->GetActiveSurfaceRect().IsEmpty() ?
        syncDirtyManager->GetSurfaceRect() : syncDirtyManager->GetActiveSurfaceRect();
    ScreenInfo curScreenInfo = screenManager->QueryScreenInfo(paramScreenId);
    uint32_t vsyncRefreshRate = RSMainThread::Instance()->GetVsyncRefreshRate();
    RS_TRACE_NAME_FMT("RSDisplayRenderNodeDrawable::OnDraw[%" PRIu64 "][%" PRIu64"] zoomed(%d), "
        "currentFrameDirty(%d, %d, %d, %d), screen(%d, %d), active(%d, %d, %d, %d), vsyncRefreshRate(%u)",
        paramScreenId, GetId(), params->GetZoomed(),
        dirtyRegion.left_, dirtyRegion.top_, dirtyRegion.width_, dirtyRegion.height_,
        curScreenInfo.width, curScreenInfo.height,
        activeSurfaceRect.left_, activeSurfaceRect.top_, activeSurfaceRect.width_, activeSurfaceRect.height_,
        vsyncRefreshRate);
    RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw node: %{public}" PRIu64 "", GetId());
    ScreenId activeScreenId = HgmCore::Instance().GetActiveScreenId();

    // when set expectedRefreshRate, the vsyncRefreshRate maybe change from 60 to 120
    // so that need change whether equal vsync period and whether use virtual dirty
    if (curScreenInfo.skipFrameStrategy == SKIP_FRAME_BY_REFRESH_RATE) {
        bool isEqualVsyncPeriod = (vsyncRefreshRate == curScreenInfo.expectedRefreshRate);
        if (isEqualVsyncPeriod != curScreenInfo.isEqualVsyncPeriod) {
            curScreenInfo.isEqualVsyncPeriod = isEqualVsyncPeriod;
            screenManager->SetEqualVsyncPeriod(paramScreenId, isEqualVsyncPeriod);
        }
    }
    screenManager->RemoveForceRefreshTask();
    if (SkipFrame(vsyncRefreshRate, curScreenInfo)) {
        SetDrawSkipType(DrawSkipType::SKIP_FRAME);
        RS_TRACE_NAME_FMT("SkipFrame, screenId:%lu, strategy:%d, interval:%u, refreshrate:%u", paramScreenId,
            curScreenInfo.skipFrameStrategy, curScreenInfo.skipFrameInterval, curScreenInfo.expectedRefreshRate);
        screenManager->PostForceRefreshTask();
        return;
    }
    if (!curScreenInfo.isEqualVsyncPeriod) {
        virtualDirtyRefresh_ = true;
    }

    auto screenInfo = params->GetScreenInfo();
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    if (!processor) {
        SetDrawSkipType(DrawSkipType::CREATE_PROCESSOR_FAIL);
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw RSProcessor is null!");
        return;
    }

    auto mirroredDrawable = params->GetMirrorSourceDrawable().lock();
    auto mirroredRenderParams = mirroredDrawable ?
        static_cast<RSDisplayRenderParams*>(mirroredDrawable->GetRenderParams().get()) : nullptr;
    if (mirroredRenderParams ||
        params->GetCompositeType() == RSDisplayRenderNode::CompositeType::UNI_RENDER_EXPAND_COMPOSITE) {
        if (!processor->InitForRenderThread(*this,
            mirroredRenderParams ? mirroredRenderParams->GetScreenId() : INVALID_SCREEN_ID,
            RSUniRenderThread::Instance().GetRenderEngine())) {
            SetDrawSkipType(DrawSkipType::RENDER_ENGINE_NULL);
            syncDirtyManager_->ResetDirtyAsSurfaceSize();
            syncDirtyManager_->UpdateDirty(false);
            RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw processor init failed!");
            return;
        }
        if (mirroredRenderParams) {
            offsetX_ = mirroredRenderParams->GetDisplayOffsetX();
            offsetY_ = mirroredRenderParams->GetDisplayOffsetY();
            enableVisibleRect_ = screenInfo.enableVisibleRect;
            if (enableVisibleRect_) {
                const auto& rect = screenManager->GetMirrorScreenVisibleRect(paramScreenId);
                RS_TRACE_NAME_FMT("RSDisplayRenderNodeDrawable::OnDraw VisibleRect[%d, %d, %d, %d] to [%d, %d, %d, %d]",
                    curVisibleRect_.GetLeft(), curVisibleRect_.GetTop(),
                    curVisibleRect_.GetWidth(), curVisibleRect_.GetHeight(), rect.x, rect.y, rect.w, rect.h);
                curVisibleRect_ = Drawing::RectI(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
                RSUniRenderThread::Instance().SetVisibleRect(curVisibleRect_);
                RSUniRenderThread::Instance().SetEnableVisiableRect(enableVisibleRect_);
            }
            currentBlackList_ = screenManager->GetVirtualScreenBlackList(paramScreenId);
            RSUniRenderThread::Instance().SetBlackList(currentBlackList_);
            if (params->GetCompositeType() == RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE) {
                SetDrawSkipType(DrawSkipType::WIRED_SCREEN_PROJECTION);
                WiredScreenProjection(*params, processor);
                lastVisibleRect_ = curVisibleRect_;
                lastBlackList_ = currentBlackList_;
                RSUniRenderThread::Instance().SetVisibleRect(Drawing::RectI());
                return;
            }
            currentTypeBlackList_ = screenManager->GetVirtualScreenTypeBlackList(paramScreenId);
            RSUniRenderThread::Instance().SetTypeBlackList(currentTypeBlackList_);
            RSUniRenderThread::Instance().SetWhiteList(screenInfo.whiteList);
            curSecExemption_ = params->GetSecurityExemption();
            uniParam->SetSecExemption(curSecExemption_);
            DrawMirrorScreen(*params, processor);
            lastBlackList_ = currentBlackList_;
            lastTypeBlackList_ = currentTypeBlackList_;
            lastSecExemption_ = curSecExemption_;
            lastVisibleRect_ = curVisibleRect_;
            RSUniRenderThread::Instance().SetVisibleRect(Drawing::RectI());
            RSUniRenderThread::Instance().SetEnableVisiableRect(false);
        } else {
            bool isOpDropped = uniParam->IsOpDropped();
            uniParam->SetOpDropped(false);
            auto expandProcessor = RSProcessor::ReinterpretCast<RSUniRenderVirtualProcessor>(processor);
            if (!expandProcessor) {
                SetDrawSkipType(DrawSkipType::EXPAND_PROCESSOR_NULL);
                RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw expandProcessor is null!");
                return;
            }
            RSDirtyRectsDfx rsDirtyRectsDfx(*this);
            std::vector<RectI> damageRegionRects;
            // disable expand screen dirty when isEqualVsyncPeriod is false, because the dirty history is incorrect
            if (uniParam->IsExpandScreenDirtyEnabled() && uniParam->IsVirtualDirtyEnabled() &&
                curScreenInfo.isEqualVsyncPeriod) {
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
            DrawExpandScreen(*params, *expandProcessor);
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
    RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw HDRDraw isHdrOn: %{public}d, BrightnessRatio: %{public}f",
        isHdrOn, hdrBrightnessRatio);

    // checkDisplayNodeSkip need to be judged at the end
    if (RSAncoManager::Instance()->GetAncoHebcStatus() == AncoHebcStatus::INITIAL &&
        uniParam->IsOpDropped() && CheckDisplayNodeSkip(*params, processor)) {
        SetDrawSkipType(DrawSkipType::DISPLAY_NODE_SKIP);
        RSMainThread::Instance()->SetFrameIsRender(false);
        SetDisplayNodeSkipFlag(*uniParam, true);
        return;
    }
    SetDisplayNodeSkipFlag(*uniParam, false);
    RSMainThread::Instance()->SetFrameIsRender(true);

    CheckAndUpdateFilterCacheOcclusion(*params, curScreenInfo);
    if (isHdrOn) {
        params->SetNewPixelFormat(GRAPHIC_PIXEL_FMT_RGBA_1010102);
    }
    RSUniRenderThread::Instance().WaitUntilDisplayNodeBufferReleased(*this);
    // displayNodeSp to get  rsSurface witch only used in renderThread
    auto renderFrame = RequestFrame(*params, processor);
    if (!renderFrame) {
        SetDrawSkipType(DrawSkipType::REQUEST_FRAME_FAIL);
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw failed to request frame");
        return;
    }

    UpdateSlrScale(screenInfo, params);
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
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw DrawingSurface is null");
        return;
    }

    curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
    if (!curCanvas_) {
        SetDrawSkipType(DrawSkipType::CANVAS_NULL);
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw failed to create canvas");
        return;
    }

    curCanvas_->SetTargetColorGamut(params->GetNewColorSpace());
    curCanvas_->SetScreenId(paramScreenId);
    curCanvas_->SetHdrOn(isHdrOn);
    curCanvas_->SetDisableFilterCache(params->GetZoomed());

#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (RSOpincDrawCache::IsAutoCacheEnable()) {
        RSOpincDrawCache::SetScreenRectInfo({0, 0, screenInfo.width, screenInfo.height});
    }
#endif
    UpdateSurfaceDrawRegion(curCanvas_, params);
    // canvas draw
    {
        {
            RSSkpCaptureDfx capture(curCanvas_);
            Drawing::AutoCanvasRestore acr(*curCanvas_, true);

            bool isOpDropped = uniParam->IsOpDropped();
            bool isScRGBEnable = EnablescRGBForP3AndUiFirst(params->GetNewColorSpace());
            bool needOffscreen = params->GetNeedOffscreen() || isHdrOn || isScRGBEnable || screenInfo.isSamplingOn;
            if (params->GetNeedOffscreen()) {
                uniParam->SetOpDropped(false);
            }

            if (uniParam->IsOpDropped()) {
                if (uniParam->IsDirtyAlignEnabled()) {
                    RS_TRACE_NAME_FMT("dirty align enabled and no clip operation");
                    curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
                } else {
                    uniParam->SetClipRegion(clipRegion);
                    ClipRegion(*curCanvas_, clipRegion);
                }
            } else if (params->GetNeedOffscreen()) {
                // draw black background in rotation for camera
                curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
            } else {
                curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
            }

            if (needOffscreen) {
                ScaleCanvasIfNeeded(screenInfo);
                auto rect = curCanvas_->GetDeviceClipBounds();
                PrepareOffscreenRender(*this, !screenInfo.isSamplingOn, !screenInfo.isSamplingOn);
                if (params->GetHDRPresent() || isScRGBEnable) {
                    curCanvas_->ClipRect(rect);
                }
            }

            if (!params->GetNeedOffscreen()) {
                curCanvas_->ConcatMatrix(params->GetMatrix());
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
            Drawing::Matrix invertMatrix;
            if (params->GetNeedOffscreen() && params->GetMatrix().Invert(invertMatrix)) {
                curCanvas_->ConcatMatrix(invertMatrix);
            }
            rsDirtyRectsDfx.OnDraw(*curCanvas_);
            curCanvas_->Restore();
            DrawCurtainScreen();
            bool displayP3Enable = (params->GetNewColorSpace() == GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
            if (screenInfo.isSamplingOn) {
                // In expand screen down-sampling situation, process watermark and color filter during offscreen render.
                DrawWatermarkIfNeed(*params, *curCanvas_);
                SwitchColorFilter(*curCanvas_, hdrBrightnessRatio, displayP3Enable);
            }
            if (needOffscreen && canvasBackup_) {
                Drawing::AutoCanvasRestore acr(*canvasBackup_, true);
                if (params->GetNeedOffscreen()) {
                    canvasBackup_->ConcatMatrix(params->GetMatrix());
                }
                ClearTransparentBeforeSaveLayer();
                FinishOffscreenRender(Drawing::SamplingOptions(Drawing::FilterMode::NEAREST,
                    Drawing::MipmapMode::NONE), screenInfo.isSamplingOn, hdrBrightnessRatio);
                uniParam->SetOpDropped(isOpDropped);
            }
            if (!screenInfo.isSamplingOn) {
                // In normal situation, process watermark and color filter after offscreen render.
                DrawWatermarkIfNeed(*params, *curCanvas_);
                SwitchColorFilter(*curCanvas_, hdrBrightnessRatio, displayP3Enable);
            }
#ifdef RS_ENABLE_OVERLAY_DISPLAY
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

        if (RotateOffScreenParam::GetRotateOffScreenDisplayNodeEnable() && !params->IsRotationChanged()) {
            offscreenSurface_ = nullptr;
        }

        specialLayerType_ = GetSpecialLayerType(*params);
        if (RSSystemProperties::GetDrawMirrorCacheImageEnabled() && uniParam->HasMirrorDisplay() &&
            curCanvas_->GetSurface() != nullptr && (specialLayerType_ != HAS_SPECIAL_LAYER ||
            RSMainThread::Instance()->HasWiredMirrorDisplay())) {
            cacheImgForCapture_ = curCanvas_->GetSurface()->GetImageSnapshot();
        } else {
            SetCacheImgForCapture(nullptr);
        }
    }
    RenderOverDraw();
    RSMainThread::Instance()->SetDirtyFlag(false);

    if (Drawing::PerformanceCaculate::GetDrawingFlushPrint()) {
        RS_LOGI("Drawing Performance Flush start %{public}lld", Drawing::PerformanceCaculate::GetUpTime(false));
    }
    RS_TRACE_BEGIN("RSDisplayRenderNodeDrawable Flush");
    Drawing::GPUResourceTag::SetCurrentNodeId(GetId());
    RsFrameReport::GetInstance().BeginFlush();

#ifdef RS_ENABLE_TV_PQ_METADATA
    // set tv meta data to buffer
    auto surface = renderFrame->GetSurface();
    RSTvMetadataManager::Instance().CopyTvMetadataToSurface(surface);
#endif
    renderFrame->Flush();
    RS_TRACE_END();
    if (Drawing::PerformanceCaculate::GetDrawingFlushPrint()) {
        RS_LOGI("Drawing Performance Flush end %{public}lld", Drawing::PerformanceCaculate::GetUpTime(false));
        Drawing::PerformanceCaculate::ResetCaculateTimeCount();
    }

    // process round corner display
    DoScreenRcdTask(*params, processor);

    if (!RSMainThread::Instance()->WaitHardwareThreadTaskExecute()) {
        RS_LOGW("RSDisplayRenderNodeDrawable::ondraw: hardwareThread task has too many to Execute"
                " TaskNum:[%{public}d]", RSHardwareThread::Instance().GetunExecuteTaskNum());
        RSHardwareThread::Instance().DumpEventQueue();
    }

    RS_TRACE_BEGIN("RSDisplayRenderNodeDrawable CommitLayer");
    for (const auto& [displayNodeId, drawable] : hardwareDrawables) {
        if (UNLIKELY(!drawable || !drawable->GetRenderParams())) {
            continue;
        }
        auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(drawable);
        if (!surfaceDrawable || displayNodeId != params->GetId()) {
            continue;
        }
        if (drawable->GetRenderParams()->GetHardwareEnabled()) {
            auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable->GetRenderParams().get());
            if (surfaceParams && surfaceParams->GetHwcGlobalPositionEnabled()) {
                surfaceParams->SetOffsetX(offsetX_);
                surfaceParams->SetOffsetY(offsetY_);
                surfaceParams->SetRogWidthRatio(surfaceParams->IsDRMCrossNode() ?
                    params->GetScreenInfo().GetRogWidthRatio() : 1.0f);
            } else {
                surfaceParams->SetOffsetX(0);
                surfaceParams->SetOffsetY(0);
                surfaceParams->SetRogWidthRatio(1.0f);
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
    processor->ProcessDisplaySurfaceForRenderThread(*this);
    processor->PostProcess();
    RS_TRACE_END();

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    if (!mirrorDrawable) {
        RSMagicPointerRenderManager::GetInstance().ProcessColorPicker(processor, curCanvas_->GetGPUContext());
        RSMagicPointerRenderManager::GetInstance().SetCacheImgForPointer(nullptr);
    }
#endif
}

void RSDisplayRenderNodeDrawable::ClearCanvasStencil(RSPaintFilterCanvas& canvas,
    RSDisplayRenderParams& params, RSRenderThreadParams& uniParam)
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

void RSDisplayRenderNodeDrawable::DrawMirrorScreen(
    RSDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor)
{
    RS_TRACE_FUNC();
    // uniParam/drawable/mirroredParams/renderParams_ not null in caller
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    auto mirroredDrawable =
        std::static_pointer_cast<RSDisplayRenderNodeDrawable>(params.GetMirrorSourceDrawable().lock());
    auto& mirroredParams = mirroredDrawable->GetRenderParams();

    specialLayerType_ = GetSpecialLayerType(static_cast<RSDisplayRenderParams&>(*mirroredParams),
        enableVisibleRect_ ? params.HasSecLayerInVisibleRect() : true);
    auto virtualProcesser = RSProcessor::ReinterpretCast<RSUniRenderVirtualProcessor>(processor);
    if (!virtualProcesser) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawMirrorScreen virtualProcesser is null");
        return;
    }

    auto hardwareDrawables = uniParam->GetHardwareEnabledTypeDrawables();
    //if specialLayer is visible and no CacheImg
    if ((mirroredParams->GetSecurityDisplay() != params.GetSecurityDisplay() &&
        specialLayerType_ == HAS_SPECIAL_LAYER) || !mirroredDrawable->GetCacheImgForCapture() ||
        params.GetVirtualScreenMuteStatus()) {
        MirrorRedrawDFX(true, params.GetScreenId());
        virtualProcesser->SetDrawVirtualMirrorCopy(false);
        DrawMirror(params, virtualProcesser, &RSDisplayRenderNodeDrawable::OnCapture, *uniParam);
    } else {
        MirrorRedrawDFX(false, params.GetScreenId());
        virtualProcesser->SetDrawVirtualMirrorCopy(true);
        DrawMirrorCopy(*mirroredDrawable, params, virtualProcesser, *uniParam);
    }
}

void RSDisplayRenderNodeDrawable::SetScreenRotationForPointLight(RSDisplayRenderParams &params)
{
    auto screenManager = CreateOrGetScreenManager();
    auto mirrorDrawable = params.GetMirrorSourceDrawable().lock();
    auto mirrorParams = mirrorDrawable ? mirrorDrawable->GetRenderParams().get() : nullptr;
    ScreenId screenId = params.GetScreenId();
    ScreenRotation screenRotation = params.GetScreenRotation();
    if (mirrorParams) {
        screenId = mirrorParams->GetScreenId();
        screenRotation = mirrorParams->GetScreenRotation();
    }
    auto screenCorrection = screenManager->GetScreenCorrection(screenId);
    screenRotation = static_cast<ScreenRotation>(
        (static_cast<int>(screenRotation) + SCREEN_ROTATION_NUM - static_cast<int>(screenCorrection)) %
        SCREEN_ROTATION_NUM);
    RSPointLightManager::Instance()->SetScreenRotation(screenRotation);
}

void RSDisplayRenderNodeDrawable::UpdateDisplayDirtyManager(int32_t bufferage, bool useAlignedDirtyRegion)
{
    // syncDirtyManager_ not null in caller
    syncDirtyManager_->SetBufferAge(bufferage);
    syncDirtyManager_->UpdateDirty(useAlignedDirtyRegion);
}

int32_t RSDisplayRenderNodeDrawable::GetSpecialLayerType(RSDisplayRenderParams& params, bool isSecLayerInVisibleRect)
{
    auto& uniRenderThread = RSUniRenderThread::Instance();
    const auto& specialLayerManager = params.GetSpecialLayerMgr();
    bool hasGeneralSpecialLayer = (specialLayerManager.Find(SpecialLayerType::HAS_SECURITY) &&
        isSecLayerInVisibleRect) || specialLayerManager.Find(SpecialLayerType::HAS_SKIP) ||
        specialLayerManager.Find(SpecialLayerType::HAS_PROTECTED) ||
        params.GetHDRPresent() || uniRenderThread.IsColorFilterModeOn();
    RS_LOGD("RSDisplayRenderNodeDrawable::SpecialLayer:%{public}" PRIu32 ", CurtainScreen:%{public}d, "
        "HDRPresent:%{public}d, ColorFilter:%{public}d", specialLayerManager.Get(),
        uniRenderThread.IsCurtainScreenOn(), params.GetHDRPresent(), uniRenderThread.IsColorFilterModeOn());
    if (RSUniRenderThread::GetCaptureParam().isSnapshot_) {
        hasGeneralSpecialLayer |= (specialLayerManager.Find(SpecialLayerType::HAS_SNAPSHOT_SKIP) ||
            uniRenderThread.IsCurtainScreenOn());
        return hasGeneralSpecialLayer ? HAS_SPECIAL_LAYER :
            (params.HasCaptureWindow() ? CAPTURE_WINDOW : NO_SPECIAL_LAYER);
    }
    if (hasGeneralSpecialLayer || !uniRenderThread.GetWhiteList().empty() || !currentBlackList_.empty() ||
        !currentTypeBlackList_.empty()) {
        return HAS_SPECIAL_LAYER;
    } else if (params.HasCaptureWindow()) {
        return CAPTURE_WINDOW;
    }
    return NO_SPECIAL_LAYER;
}

std::vector<RectI> RSDisplayRenderNodeDrawable::CalculateVirtualDirty(
    std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser, RSDisplayRenderParams& params,
    Drawing::Matrix canvasMatrix)
{
    // uniParam/drawable/mirroredParams not null in caller
    std::vector<RectI> mappedDamageRegionRects;
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    auto drawable = params.GetMirrorSourceDrawable().lock();
    auto mirroredDrawable = std::static_pointer_cast<RSDisplayRenderNodeDrawable>(drawable);
    auto mirrorParams = static_cast<RSDisplayRenderParams*>(mirroredDrawable->GetRenderParams().get());
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSDisplayRenderNodeDrawable::CalculateVirtualDirty ScreenManager is nullptr");
        virtualProcesser->SetRoiRegionToCodec(mappedDamageRegionRects);
        return mappedDamageRegionRects;
    }
    ScreenInfo curScreenInfo = screenManager->QueryScreenInfo(params.GetScreenId());
    if (!curScreenInfo.isEqualVsyncPeriod) {
        RS_LOGD("RSDisplayRenderNodeDrawable::CalculateVirtualDirty frame rate is irregular");
        virtualProcesser->SetRoiRegionToCodec(mappedDamageRegionRects);
        return mappedDamageRegionRects;
    }
    ScreenInfo mainScreenInfo = screenManager->QueryScreenInfo(mirrorParams->GetScreenId());
    int32_t bufferAge = virtualProcesser->GetBufferAge();
    std::vector<RectI> damageRegionRects = RSUniRenderUtil::MergeDirtyHistoryInVirtual(
        *mirroredDrawable, bufferAge, mainScreenInfo, false);
    std::shared_ptr<RSObjAbsGeometry> tmpGeo = std::make_shared<RSObjAbsGeometry>();
    for (auto& rect : damageRegionRects) {
        RectI mappedRect = tmpGeo->MapRect(rect.ConvertTo<float>(), canvasMatrix);
        mappedDamageRegionRects.emplace_back(mappedRect);
    }
    if (!(lastMatrix_ == canvasMatrix) || !(lastMirrorMatrix_ == mirrorParams->GetMatrix()) ||
        uniParam->GetForceMirrorScreenDirty() || lastBlackList_ != currentBlackList_ ||
        lastTypeBlackList_ != currentTypeBlackList_ || mirrorParams->IsSpecialLayerChanged() ||
        lastSecExemption_ != curSecExemption_ || virtualDirtyRefresh_ ||
        (enableVisibleRect_ && (lastVisibleRect_ != curVisibleRect_ || params.HasSecLayerInVisibleRectChanged()))) {
        GetSyncDirtyManager()->ResetDirtyAsSurfaceSize();
        virtualDirtyRefresh_ = false;
        lastMatrix_ = canvasMatrix;
        lastMirrorMatrix_ = mirrorParams->GetMatrix();
    }
    RectI hwcRect = mirroredDrawable->GetSyncDirtyManager()->GetHwcDirtyRegion();
    if (!hwcRect.IsEmpty()) {
        if (mainScreenInfo.isSamplingOn && mainScreenInfo.samplingScale > 0) {
            Drawing::Matrix scaleMatrix;
            scaleMatrix.SetScaleTranslate(mainScreenInfo.samplingScale, mainScreenInfo.samplingScale,
                mainScreenInfo.samplingTranslateX, mainScreenInfo.samplingTranslateY);
            hwcRect = RSObjAbsGeometry::MapRect(hwcRect.ConvertTo<float>(), scaleMatrix);
            const Vector4<int> expandSize{mainScreenInfo.samplingDistance, mainScreenInfo.samplingDistance,
                mainScreenInfo.samplingDistance, mainScreenInfo.samplingDistance};
            hwcRect = hwcRect.MakeOutset(expandSize);
        }
        RectI mappedHwcRect = tmpGeo->MapRect(hwcRect.ConvertTo<float>(), canvasMatrix);
        GetSyncDirtyManager()->MergeDirtyRect(mappedHwcRect);
    }
    UpdateDisplayDirtyManager(bufferAge, false);
    auto extraDirty = GetSyncDirtyManager()->GetDirtyRegion();
    if (!extraDirty.IsEmpty()) {
        mappedDamageRegionRects.emplace_back(extraDirty);
    }
    if (!uniParam->IsVirtualDirtyDfxEnabled()) {
        virtualProcesser->SetDirtyInfo(mappedDamageRegionRects);
        RS_TRACE_NAME_FMT("SetDamageRegion damageRegionrects num: %zu, info: %s",
            mappedDamageRegionRects.size(), RectVectorToString(mappedDamageRegionRects).c_str());
    }
    return mappedDamageRegionRects;
}

void RSDisplayRenderNodeDrawable::DrawMirror(RSDisplayRenderParams& params,
    std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser, DrawFuncPtr drawFunc, RSRenderThreadParams& uniParam)
{
    RS_TRACE_FUNC();
    // uniParam/drawable/mirroredParams not null in caller
    auto drawable = params.GetMirrorSourceDrawable().lock();
    auto mirroredDrawable = std::static_pointer_cast<RSDisplayRenderNodeDrawable>(drawable);
    auto& mirroredParams = mirroredDrawable->GetRenderParams();

    curCanvas_ = virtualProcesser->GetCanvas();
    if (curCanvas_ == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawMirror failed to get canvas.");
        return;
    }
    // for HDR
    curCanvas_->SetOnMultipleScreen(true);
    curCanvas_->SetDisableFilterCache(true);
    auto hasSecSurface = static_cast<RSDisplayRenderParams*>
        (mirroredParams.get())->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY);
    if ((((!enableVisibleRect_ && hasSecSurface) || (enableVisibleRect_ && params.HasSecLayerInVisibleRect())) &&
        !uniParam.GetSecExemption()) || params.GetVirtualScreenMuteStatus()) {
        std::vector<RectI> emptyRects = {};
        virtualProcesser->SetRoiRegionToCodec(emptyRects);
        auto screenManager = CreateOrGetScreenManager();
        if (screenManager->GetScreenSecurityMask(params.GetScreenId())) {
            SetSecurityMask(*virtualProcesser);
        } else {
            SetCanvasBlack(*virtualProcesser);
        }
        virtualDirtyRefresh_ = true;
        curCanvas_->RestoreToCount(0);
        return;
    }
    curCanvas_->Save();
    virtualProcesser->ScaleMirrorIfNeed(GetOriginScreenRotation(), *curCanvas_);
    auto mirroredScreenInfo = mirroredParams->GetScreenInfo();
    UpdateSlrScale(mirroredScreenInfo);
    ScaleCanvasIfNeeded(mirroredScreenInfo);

    RSDirtyRectsDfx rsDirtyRectsDfx(*this);
    if (uniParam.IsVirtualDirtyEnabled()) {
        Drawing::Matrix matrix = curCanvas_->GetTotalMatrix();
        std::vector<RectI> dirtyRects = CalculateVirtualDirty(virtualProcesser, params, matrix);
        rsDirtyRectsDfx.SetVirtualDirtyRects(dirtyRects, params.GetScreenInfo());
    } else {
        std::vector<RectI> emptyRects = {};
        virtualProcesser->SetRoiRegionToCodec(emptyRects);
    }
    // Clean up the content of the previous frame
    curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
    virtualProcesser->CanvasClipRegionForUniscaleMode(visibleClipRectMatrix_, mirroredScreenInfo);
    curCanvas_->ConcatMatrix(mirroredParams->GetMatrix());
    PrepareOffscreenRender(*mirroredDrawable, false, false);

    // set mirror screen capture param
    // Don't need to scale here since the canvas has been switched from mirror frame to offscreen
    // surface in PrepareOffscreenRender() above. The offscreen surface has the same size as
    // the main display that's why no need additional scale.
    RSUniRenderThread::SetCaptureParam(CaptureParam(false, false, true));
    RSUniRenderThread::GetCaptureParam().virtualScreenId_ = params.GetScreenId();
    RSRenderParams::SetParentSurfaceMatrix(curCanvas_->GetTotalMatrix());
    bool isOpDropped = uniParam.IsOpDropped();
    uniParam.SetOpDropped(false); // disable partial render
    (mirroredDrawable.get()->*drawFunc)(*curCanvas_);
    uniParam.SetOpDropped(isOpDropped);
    RSUniRenderThread::ResetCaptureParam();
    FinishOffscreenRender(Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NEAREST),
        mirroredScreenInfo.isSamplingOn);
    // Restore the initial state of the canvas to avoid state accumulation
    curCanvas_->RestoreToCount(0);
    rsDirtyRectsDfx.OnDrawVirtual(*curCanvas_);
    RSUniRenderThread::Instance().SetBlackList({});
    RSUniRenderThread::Instance().SetTypeBlackList({});
    RSUniRenderThread::Instance().SetWhiteList({});
    uniParam.SetSecExemption(false);
}

void RSDisplayRenderNodeDrawable::DrawMirrorCopy(
    RSDisplayRenderNodeDrawable& mirrorDrawable, RSDisplayRenderParams& params,
    std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser, RSRenderThreadParams& uniParam)
{
    RS_TRACE_FUNC();
    // mirroredParams not null in caller
    const auto& mirroredParams = mirrorDrawable.GetRenderParams();
    auto cacheImage = mirrorDrawable.GetCacheImgForCapture();
    bool isOpDropped = uniParam.IsOpDropped();
    uniParam.SetOpDropped(false);
    mirrorDrawable.SetOriginScreenRotation(GetOriginScreenRotation());
    virtualProcesser->CalculateTransform(mirrorDrawable);
    RSDirtyRectsDfx rsDirtyRectsDfx(*this);
    std::shared_ptr<RSSLRScaleFunction> slrManager = enableVisibleRect_ ? nullptr : virtualProcesser->GetSlrManager();
    if (!uniParam.IsVirtualDirtyEnabled() || (enableVisibleRect_ && curVisibleRect_.GetTop() > 0)) {
        std::vector<RectI> emptyRects = {};
        virtualProcesser->SetRoiRegionToCodec(emptyRects);
    } else {
        auto dirtyRects = CalculateVirtualDirty(
            virtualProcesser, params, slrManager ? slrManager->GetScaleMatrix() : virtualProcesser->GetCanvasMatrix());
        rsDirtyRectsDfx.SetVirtualDirtyRects(dirtyRects, params.GetScreenInfo());
    }
    curCanvas_ = virtualProcesser->GetCanvas();
    if (!curCanvas_) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawMirrorCopy failed to get canvas.");
        return;
    }
    // Clean up the content of the previous frame
    curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
    virtualProcesser->CanvasClipRegionForUniscaleMode();
    RSUniRenderThread::SetCaptureParam(CaptureParam(false, false, true));
    if (slrManager) {
        curCanvas_->Save();
        auto scaleNum = slrManager->GetScaleNum();
        curCanvas_->Scale(scaleNum, scaleNum);
        mirrorDrawable.DrawHardwareEnabledNodesMissedInCacheImage(*curCanvas_);
        curCanvas_->Restore();
    } else {
        mirrorDrawable.DrawHardwareEnabledNodesMissedInCacheImage(*curCanvas_);
    }
    if (cacheImage && RSSystemProperties::GetDrawMirrorCacheImageEnabled()) {
        RS_TRACE_NAME("DrawMirrorCopy with cacheImage");
        if (!enableVisibleRect_) {
            virtualProcesser->ProcessCacheImage(*cacheImage);
        } else {
            RSUniRenderUtil::ProcessCacheImageRect(*curCanvas_, *cacheImage, curVisibleRect_,
                Drawing::Rect(0, 0, curVisibleRect_.GetWidth(), curVisibleRect_.GetHeight()));
        }
    } else {
        RS_TRACE_NAME("DrawMirrorCopy with displaySurface");
        virtualProcesser->ProcessDisplaySurfaceForRenderThread(mirrorDrawable);
    }
    if (slrManager) {
        curCanvas_->Save();
        auto scaleNum = slrManager->GetScaleNum();
        curCanvas_->Scale(scaleNum, scaleNum);
        mirrorDrawable.DrawHardwareEnabledTopNodesMissedInCacheImage(*curCanvas_);
        curCanvas_->Restore();
    } else if (mirroredParams->GetScreenInfo().isSamplingOn) {
        curCanvas_->Save();
        auto scaleNum = mirroredParams->GetScreenInfo().samplingScale;
        curCanvas_->Scale(scaleNum, scaleNum);
        mirrorDrawable.DrawHardwareEnabledTopNodesMissedInCacheImage(*curCanvas_);
        curCanvas_->Restore();
    } else {
        mirrorDrawable.DrawHardwareEnabledTopNodesMissedInCacheImage(*curCanvas_);
    }
    RSUniRenderThread::ResetCaptureParam();
    uniParam.SetOpDropped(isOpDropped);
    // Restore the initial state of the canvas to avoid state accumulation
    curCanvas_->RestoreToCount(0);
    rsDirtyRectsDfx.OnDrawVirtual(*curCanvas_);
}

void RSDisplayRenderNodeDrawable::DrawExpandScreen(
    RSDisplayRenderParams& params, RSUniRenderVirtualProcessor& processor)
{
    RS_TRACE_FUNC();
    curCanvas_ = processor.GetCanvas();
    if (curCanvas_ == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawExpandScreen failed to get canvas.");
        return;
    }
    // Clean up the content of the previous frame
    curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
    RSRenderNodeDrawable::OnCapture(*curCanvas_);
    RSUniRenderThread::ResetCaptureParam();
    // for HDR
    curCanvas_->SetOnMultipleScreen(true);
    auto targetSurfaceRenderNodeDrawable =
        std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(params.GetTargetSurfaceRenderNodeDrawable().lock());
    if (targetSurfaceRenderNodeDrawable && curCanvas_->GetSurface()) {
        RS_TRACE_NAME("DrawExpandScreen cacheImgForMultiScreenView");
        cacheImgForMultiScreenView_ = curCanvas_->GetSurface()->GetImageSnapshot();
    } else {
        cacheImgForMultiScreenView_ = nullptr;
    }
    // Restore the initial state of the canvas to avoid state accumulation
    curCanvas_->RestoreToCount(0);
}

void RSDisplayRenderNodeDrawable::WiredScreenProjection(
    RSDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor)
{
    RS_TRACE_FUNC();
    RSUniRenderThread::Instance().WaitUntilDisplayNodeBufferReleased(*this);
    auto renderFrame = RequestFrame(params, processor);
    if (!renderFrame) {
        RS_LOGE("RSDisplayRenderNodeDrawable::WiredScreenProjection failed to request frame");
        return;
    }
    auto drSurface = renderFrame->GetFrame()->GetSurface();
    if (!drSurface) {
        RS_LOGE("RSDisplayRenderNodeDrawable::WiredScreenProjection DrawingSurface is null");
        return;
    }
    curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
    if (!curCanvas_) {
        RS_LOGE("RSDisplayRenderNodeDrawable::WiredScreenProjection failed to create canvas");
        return;
    }
    auto mirroredDrawable =
        std::static_pointer_cast<RSDisplayRenderNodeDrawable>(params.GetMirrorSourceDrawable().lock());
    if (!mirroredDrawable) {
        RS_LOGE("RSDisplayRenderNodeDrawable::WiredScreenProjection mirroredDrawable is null");
        return;
    }
    if (!mirroredDrawable->GetRenderParams()) {
        RS_LOGE("RSDisplayRenderNodeDrawable::WiredScreenProjection mirroredDrawable GetRenderParams is null");
        return;
    }
    auto& mirroredParams = static_cast<RSDisplayRenderParams&>(*mirroredDrawable->GetRenderParams());
    bool isProcessSecLayer = !MultiScreenParam::IsExternalScreenSecure() &&
        mirroredParams.GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY);
    auto isRedraw = RSSystemParameters::GetDebugMirrorOndrawEnabled() ||
        (RSSystemParameters::GetWiredScreenOndrawEnabled() && !enableVisibleRect_ &&
            (mirroredParams.GetHDRPresent() || !currentBlackList_.empty() || isProcessSecLayer ||
                mirroredParams.GetSpecialLayerMgr().Find(SpecialLayerType::HAS_PROTECTED)));
    if (isRedraw) {
        isMirrorSLRCopy_ = false;
    } else {
        auto cacheImage = mirroredDrawable->GetCacheImgForCapture();
        isMirrorSLRCopy_ = cacheImage && RSSystemProperties::GetDrawMirrorCacheImageEnabled() &&
                           !enableVisibleRect_ && RSSystemProperties::GetSLRScaleEnabled();
    }

    curCanvas_->Save();
    ScaleAndRotateMirrorForWiredScreen(*mirroredDrawable);
    Drawing::Matrix curCanvasMatrix = curCanvas_->GetTotalMatrix();
    RSDirtyRectsDfx rsDirtyRectsDfx(*mirroredDrawable);
    // HDR does not support wired screen
    if (isRedraw) {
        MirrorRedrawDFX(true, params.GetScreenId());
        DrawWiredMirrorOnDraw(*mirroredDrawable, params);
        RSUniRenderThread::Instance().SetBlackList({});
    } else {
        MirrorRedrawDFX(false, params.GetScreenId());
        std::vector<RectI> damageRegionRects = CalculateVirtualDirtyForWiredScreen(renderFrame, params,
            isMirrorSLRCopy_ ? scaleManager_->GetScaleMatrix() : curCanvas_->GetTotalMatrix());
        rsDirtyRectsDfx.SetVirtualDirtyRects(damageRegionRects, params.GetScreenInfo());
        DrawWiredMirrorCopy(*mirroredDrawable);
    }
    curCanvas_->Restore();
    rsDirtyRectsDfx.OnDrawVirtual(*curCanvas_);
    renderFrame->Flush();
    if (isRedraw && mirroredParams.GetSpecialLayerMgr().Find(SpecialLayerType::HAS_PROTECTED)) {
        RSDrmUtil::DRMCreateLayer(processor, curCanvasMatrix);
        params.SetGlobalZOrder(params.GetGlobalZOrder() + 1);
    }
    processor->ProcessDisplaySurfaceForRenderThread(*this);
    if (params.GetHardCursorStatus()) {
        HardCursorCreateLayer(processor);
    }
    processor->PostProcess();
}

void RSDisplayRenderNodeDrawable::DrawWiredMirrorCopy(RSDisplayRenderNodeDrawable& mirroredDrawable)
{
    RS_TRACE_FUNC();
    auto cacheImage = mirroredDrawable.GetCacheImgForCapture();
    if (cacheImage && RSSystemProperties::GetDrawMirrorCacheImageEnabled()) {
        RS_TRACE_NAME("DrawWiredMirrorCopy with cacheImage");
        if (isMirrorSLRCopy_) {
            RS_TRACE_NAME("DrawWiredMirrorCopy with SLRScale");
            scaleManager_->ProcessCacheImage(*curCanvas_, *cacheImage);
        } else if (!enableVisibleRect_) {
            RS_TRACE_NAME("DrawWiredMirrorCopy with SkiaScale");
            RSUniRenderUtil::ProcessCacheImage(*curCanvas_, *cacheImage);
        } else {
            RS_TRACE_NAME_FMT("DrawWiredMirrorCopy with VisibleRect[%d, %d, %d, %d]",
                curVisibleRect_.GetLeft(), curVisibleRect_.GetTop(),
                curVisibleRect_.GetWidth(), curVisibleRect_.GetHeight());
            RSUniRenderUtil::ProcessCacheImageRect(*curCanvas_, *cacheImage, curVisibleRect_,
                Drawing::Rect(0, 0, curVisibleRect_.GetWidth(), curVisibleRect_.GetHeight()));
        }
    } else {
        RS_TRACE_NAME("DrawWiredMirrorCopy with displaySurface");
        auto drawParams = RSUniRenderUtil::CreateBufferDrawParam(
            *mirroredDrawable.GetRSSurfaceHandlerOnDraw(), false); // false: draw with gpu
        auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
        if (enableVisibleRect_) {
            drawParams.srcRect = curVisibleRect_;
            drawParams.dstRect = Drawing::Rect(0, 0, curVisibleRect_.GetWidth(), curVisibleRect_.GetHeight());
        }
        drawParams.isMirror = true;
        renderEngine->DrawDisplayNodeWithParams(*curCanvas_,
            *mirroredDrawable.GetRSSurfaceHandlerOnDraw(), drawParams);
        RSMainThread::Instance()->RequestNextVSync();
    }
}

void RSDisplayRenderNodeDrawable::DrawWiredMirrorOnDraw(
    RSDisplayRenderNodeDrawable& mirroredDrawable, RSDisplayRenderParams& params)
{
    RS_TRACE_FUNC();
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (uniParam == nullptr) {
        return;
    }
    const auto mirroredParams = static_cast<RSDisplayRenderParams*>(mirroredDrawable.GetRenderParams().get());
    if (mirroredParams == nullptr) {
        return;
    }
    RS_TRACE_NAME("DrawWiredMirror with Redraw");
    // for HDR
    curCanvas_->SetOnMultipleScreen(true);
    curCanvas_->SetDisableFilterCache(true);
    if (!MultiScreenParam::IsExternalScreenSecure()) {
        auto hasSecSurface = mirroredParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY);
        if (hasSecSurface) {
            curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
            virtualDirtyRefresh_ = true;
            RS_LOGI("RSDisplayRenderNodeDrawable::DrawWiredMirrorOnDraw, "
                "set canvas to black because of security layer.");
            return;
        }
    }
    curCanvas_->SetHighContrast(RSUniRenderThread::Instance().IsHighContrastTextModeOn());
    bool isOpDropped = uniParam->IsOpDropped();
    uniParam->SetOpDropped(false);

    auto screenInfo = mirroredParams->GetScreenInfo();
    uniParam->SetScreenInfo(screenInfo);
    Drawing::Rect rect(0, 0, screenInfo.width, screenInfo.height);
    RSUniRenderThread::SetCaptureParam(CaptureParam(false, false, true));
    curCanvas_->ClipRect(rect, Drawing::ClipOp::INTERSECT, false);
    curCanvas_->ConcatMatrix(mirroredParams->GetMatrix());
    RSRenderParams::SetParentSurfaceMatrix(curCanvas_->GetTotalMatrix());
    mirroredDrawable.RSRenderNodeDrawable::OnDraw(*curCanvas_);
    DrawCurtainScreen();
    DrawWatermarkIfNeed(*mirroredParams, *curCanvas_);
    bool displayP3Enable = (params.GetNewColorSpace() == GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    SwitchColorFilter(*curCanvas_, 1.f, displayP3Enable); // 1.f: wired screen not use hdr, use default value 1.f
    RSUniRenderThread::ResetCaptureParam();

    uniParam->SetOpDropped(isOpDropped);
}

std::vector<RectI> RSDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen(
    std::unique_ptr<RSRenderFrame>& renderFrame, RSDisplayRenderParams& params, Drawing::Matrix canvasMatrix)
{
    std::vector<RectI> damageRegionRects;
    auto mirroredDrawable =
        std::static_pointer_cast<RSDisplayRenderNodeDrawable>(params.GetMirrorSourceDrawable().lock());
    if (!mirroredDrawable || !mirroredDrawable->GetRenderParams()) {
        RS_LOGE("RSDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen mirroredNode is null");
        return damageRegionRects;
    }
    const auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (uniParam == nullptr || !uniParam->IsVirtualDirtyEnabled() ||
        (enableVisibleRect_ && curVisibleRect_.GetTop() > 0)) {
        RS_LOGE("RSDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen invalid uniparam");
        return damageRegionRects;
    }
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen screenManager is null");
        return damageRegionRects;
    }
    auto curScreenInfo = params.GetScreenInfo();
    if (!curScreenInfo.isEqualVsyncPeriod) {
        RS_LOGD("RSDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen frame rate is irregular");
        return damageRegionRects;
    }
    int32_t bufferAge = renderFrame->GetBufferAge();
    const auto& mirroredParams = mirroredDrawable->GetRenderParams();
    ScreenInfo mainScreenInfo = screenManager->QueryScreenInfo(mirroredParams->GetScreenId());
    std::shared_ptr<RSObjAbsGeometry> tmpGeo = std::make_shared<RSObjAbsGeometry>();
    // merge history dirty and map to mirrored wired screen by matrix
    auto tempDamageRegionRects = RSUniRenderUtil::MergeDirtyHistoryInVirtual(
        *mirroredDrawable, bufferAge, mainScreenInfo, true);
    for (auto& rect : tempDamageRegionRects) {
        RectI mappedRect = tmpGeo->MapRect(rect.ConvertTo<float>(), canvasMatrix);
        damageRegionRects.emplace_back(mappedRect);
    }

    auto syncDirtyManager = GetSyncDirtyManager();
    // reset dirty rect as mirrored wired screen size when first time connection or matrix changed
    if (!(lastMatrix_ == canvasMatrix) || !(lastMirrorMatrix_ == mirroredParams->GetMatrix()) ||
        uniParam->GetForceMirrorScreenDirty() || (enableVisibleRect_ && lastVisibleRect_ != curVisibleRect_) ||
        lastBlackList_ != currentBlackList_ || virtualDirtyRefresh_) {
        GetSyncDirtyManager()->ResetDirtyAsSurfaceSize();
        virtualDirtyRefresh_ = false;
        lastMatrix_ = canvasMatrix;
        lastMirrorMatrix_ = mirroredParams->GetMatrix();
    }
    UpdateDisplayDirtyManager(bufferAge, false);
    auto extraDirty = syncDirtyManager->GetDirtyRegion();
    if (!extraDirty.IsEmpty()) {
        damageRegionRects.emplace_back(extraDirty);
    }
    if (!uniParam->IsVirtualDirtyDfxEnabled()) {
        renderFrame->SetDamageRegion(damageRegionRects);
        RS_TRACE_NAME_FMT("SetDamageRegion damageRegionrects num: %zu, info: %s",
            damageRegionRects.size(), RectVectorToString(damageRegionRects).c_str());
    }
    return damageRegionRects;
}

void RSDisplayRenderNodeDrawable::ScaleAndRotateMirrorForWiredScreen(RSDisplayRenderNodeDrawable& mirroredDrawable)
{
    const auto& mirroredParams = mirroredDrawable.GetRenderParams();
    if (!mirroredParams) {
        RS_LOGE("RSDisplayRenderNodeDrawable::ScaleAndRotateMirrorForWiredScreen mirroredParams is null");
        return;
    }
    auto& mirrorDisplayParams = static_cast<RSDisplayRenderParams&>(*mirroredParams);
    const auto& nodeParams = GetRenderParams();
    if (!nodeParams) {
        RS_LOGE("RSDisplayRenderNodeDrawable::ScaleAndRotateMirrorForWiredScreen nodeParams is null");
        return;
    }
    auto mainScreenInfo = mirroredParams->GetScreenInfo();
    auto mainWidth = enableVisibleRect_ ? curVisibleRect_.GetWidth() : static_cast<float>(mainScreenInfo.width);
    auto mainHeight = enableVisibleRect_ ? curVisibleRect_.GetHeight() : static_cast<float>(mainScreenInfo.height);
    auto mirrorScreenInfo = nodeParams->GetScreenInfo();
    auto mirrorWidth = static_cast<float>(mirrorScreenInfo.width);
    auto mirrorHeight = static_cast<float>(mirrorScreenInfo.height);

    auto rotation = mirroredParams->GetScreenRotation();
    auto screenManager = CreateOrGetScreenManager();
    RS_TRACE_NAME_FMT("ScaleAndRotateMirrorForWiredScreen[%" PRIu64 "](%f, %f), [%" PRIu64 "](%f, %f), rotation: %d",
        mirroredParams->GetScreenId(), mainWidth, mainHeight, nodeParams->GetScreenId(),
        mirrorWidth, mirrorHeight, rotation);
    if (screenManager) {
        auto screenCorrection = screenManager->GetScreenCorrection(mirroredParams->GetScreenId());
        if (screenCorrection != ScreenRotation::INVALID_SCREEN_ROTATION &&
            screenCorrection != ScreenRotation::ROTATION_0) {
            // Recaculate rotation if mirrored screen has additional rotation angle
            rotation = static_cast<ScreenRotation>((static_cast<int>(rotation) + SCREEN_ROTATION_NUM
                - static_cast<int>(screenCorrection)) % SCREEN_ROTATION_NUM);
        }
    }
    // Rotate
    RotateMirrorCanvas(rotation, mirrorWidth, mirrorHeight);
    // not support rotation for MirrorScreen enableVisibleRect
    rotation = enableVisibleRect_ ? ScreenRotation::ROTATION_0 : rotation;
    if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
        std::swap(mirrorWidth, mirrorHeight);
    }
    curCanvas_->Clear(SK_ColorBLACK);
    // must after rotate and swap width/height
    if (isMirrorSLRCopy_) {
        if (scaleManager_ == nullptr) {
            scaleManager_ = std::make_shared<RSSLRScaleFunction>(
                mirrorWidth, mirrorHeight, mainWidth, mainHeight);
        } else {
            scaleManager_->CheckOrRefreshScreen(mirrorWidth, mirrorHeight, mainWidth, mainHeight);
        }
        scaleManager_->CheckOrRefreshColorSpace(mirrorDisplayParams.GetNewColorSpace());
        isMirrorSLRCopy_ = scaleManager_->GetIsSLRCopy();
    }
    // Scale
    if (mainWidth > 0 && mainHeight > 0) {
        if (isMirrorSLRCopy_) {
            scaleManager_->CanvasScale(*curCanvas_);
        } else {
            auto scaleNum = std::min(mirrorWidth / mainWidth, mirrorHeight / mainHeight);
            int angle = RSUniRenderUtil::GetRotationFromMatrix(curCanvas_->GetTotalMatrix());
            if (RSSystemProperties::IsSuperFoldDisplay() && RSBaseRenderUtil::PortraitAngle(angle)) {
                scaleNum = mirrorHeight / mainWidth;
                std::swap(mirrorWidth, mirrorHeight);
            }
            curCanvas_->Translate((mirrorWidth - scaleNum * mainWidth) / HALF,
                (mirrorHeight - scaleNum * mainHeight) / HALF);
            curCanvas_->Scale(scaleNum, scaleNum);
            curCanvas_->ClipRect(Drawing::Rect(0, 0, mainWidth, mainHeight), Drawing::ClipOp::INTERSECT, false);
        }
    }
}

void RSDisplayRenderNodeDrawable::SetCanvasBlack(RSProcessor& processor)
{
    curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
    processor.PostProcess();
    RS_LOGI("RSDisplayRenderNodeDrawable::SetCanvasBlack, set canvas to black because of security layer/mute status.");
    curCanvas_->SetDisableFilterCache(false);
}

void RSDisplayRenderNodeDrawable::SetSecurityMask(RSProcessor& processor)
{
    RS_TRACE_FUNC();
    auto params = static_cast<RSDisplayRenderParams*>(GetRenderParams().get());
    if (auto screenManager = CreateOrGetScreenManager()) {
        auto imagePtr = screenManager->GetScreenSecurityMask(params->GetScreenId());
        auto image = RSPixelMapUtil::ExtractDrawingImage(imagePtr);
        if (!image || image->GetWidth() == 0 || image->GetHeight() == 0) {
            return;
        }

        auto watermark = RSUniRenderThread::Instance().GetWatermarkImg();
        auto screenInfo = screenManager->QueryScreenInfo(params->GetScreenId());
        float realImageWidth = static_cast<float>(image->GetWidth());
        float realImageHeight = static_cast<float>(image->GetHeight());

        curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
        auto srcRect = Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight());
        float screenWidth = static_cast<float>(screenInfo.width);
        float screenHeight = static_cast<float>(screenInfo.height);
        // Area to be drawn in the actual image
        auto dstRect = RSUniRenderUtil::GetImageRegions(screenWidth, screenHeight, realImageWidth, realImageHeight);
        // Make sure the canvas is oriented accurately.
        curCanvas_->ResetMatrix();

        Drawing::Brush brush;
        curCanvas_->AttachBrush(brush);
        curCanvas_->DrawImageRect(*image, srcRect, dstRect, Drawing::SamplingOptions(),
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        if (watermark) {
            curCanvas_->DrawImageRect(*watermark, srcRect, dstRect, Drawing::SamplingOptions(),
                Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        }
        curCanvas_->DetachBrush();

        processor.PostProcess();
        RS_LOGI("RSDisplayRenderNodeDrawable::SetSecurityMask, this interface is invoked"
            "when the security layer is used and mask resources are set.");
        curCanvas_->SetDisableFilterCache(false);
    }
}

void RSDisplayRenderNodeDrawable::RotateMirrorCanvas(ScreenRotation& rotation, float width, float height)
{
    switch (rotation) {
        case ScreenRotation::ROTATION_0:
            break;
        case ScreenRotation::ROTATION_90:
            curCanvas_->Translate(width / 2.0f, height / 2.0f);
            curCanvas_->Rotate(90, 0, 0); // 90 is the rotate angle
            curCanvas_->Translate(-(height / 2.0f), -(width / 2.0f));
            break;
        case ScreenRotation::ROTATION_180:
            // 180 is the rotate angle, calculate half width and half height requires divide by 2
            curCanvas_->Rotate(180, width / 2.0f, height / 2.0f);
            break;
        case ScreenRotation::ROTATION_270:
            curCanvas_->Translate(width / 2.0f, height / 2.0f);
            curCanvas_->Rotate(270, 0, 0); // 270 is the rotate angle
            curCanvas_->Translate(-(height / 2.0f), -(width / 2.0f));
            break;
        default:
            break;
    }
}

void RSDisplayRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas)
{
    auto params = static_cast<RSDisplayRenderParams*>(GetRenderParams().get());
    if (!params) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnCapture params is null!");
        return;
    }

    auto rscanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rscanvas) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnCapture, rscanvas us nullptr");
        return;
    }

    Drawing::AutoCanvasRestore acr(canvas, true);

    bool noBuffer = RSUniRenderThread::GetCaptureParam().isSnapshot_ &&
        GetRSSurfaceHandlerOnDraw()->GetBuffer() == nullptr;
    if (noBuffer) {
        RS_LOGW("RSDisplayRenderNodeDrawable::OnCapture: buffer is null!");
    }

    specialLayerType_ = GetSpecialLayerType(*params);
    // Screenshot blacklist, exclude surfacenode in blacklist while capturing displaynode
    auto currentBlackList = RSUniRenderThread::Instance().GetBlackList();
    if (specialLayerType_ != NO_SPECIAL_LAYER || UNLIKELY(noBuffer) || params->GetScreenInfo().isSamplingOn ||
        UNLIKELY(RSUniRenderThread::GetCaptureParam().isMirror_) || isRenderSkipIfScreenOff_ ||
        !currentBlackList.empty()) {
        RS_LOGD("RSDisplayRenderNodeDrawable::OnCapture: \
            process RSDisplayRenderNode(id:[%{public}" PRIu64 "]) Not using UniRender buffer.",
            params->GetId());
        RS_TRACE_NAME_FMT("RSDisplayRenderNodeDrawable::%s screenId: [%" PRIu64 "]"
            " Not using UniRender buffer. specialLayer: %d, noBuffer: %d, "
            "isSamplingOn: %d, isRenderSkipIfScreenOff: %d, blackList: %lu", __func__, params->GetScreenId(),
            specialLayerType_ != NO_SPECIAL_LAYER, noBuffer, params->GetScreenInfo().isSamplingOn,
            isRenderSkipIfScreenOff_, currentBlackList.size());

        // Adding matrix affine transformation logic
        if (!UNLIKELY(RSUniRenderThread::GetCaptureParam().isMirror_)) {
            rscanvas->ConcatMatrix(params->GetMatrix());
        }

        RSRenderNodeDrawable::OnCapture(canvas);
        DrawWatermarkIfNeed(*params, *rscanvas);
        Drawing::Matrix invertMatrix;
        if (params->GetMatrix().Invert(invertMatrix)) {
            rscanvas->ConcatMatrix(invertMatrix);
        }
        RSDirtyRectsDfx rsDirtyRectsDfx(*this);
        rsDirtyRectsDfx.OnDraw(*rscanvas);
    } else {
        DrawHardwareEnabledNodes(canvas, *params);
    }
}

void RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes(Drawing::Canvas& canvas, RSDisplayRenderParams& params)
{
    auto rscanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rscanvas) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes, rscanvas us nullptr");
        return;
    }

    FindHardwareEnabledNodes(params);

    uint32_t hwcNodesNum = static_cast<uint32_t>(params.GetHardwareEnabledDrawables().size());
    uint32_t hwcTopNodesNum = static_cast<uint32_t>(params.GetHardwareEnabledTopDrawables().size());

    RS_LOGI("RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes: \
        process RSDisplayRenderNode(id:[%{public}" PRIu64 "]) \
        using UniRender buffer with hwcNodes(%{public}u, %{public}u)",
        GetId(), hwcNodesNum, hwcTopNodesNum);
    RS_TRACE_NAME_FMT("Process RSDisplayRenderNodeDrawable[%" PRIu64 "] \
        using UniRender buffer with hwcNodes(%u, %u)",
        params.GetScreenId(), hwcNodesNum, hwcTopNodesNum);

    if (hwcNodesNum > 0) {
        AdjustZOrderAndDrawSurfaceNode(params.GetHardwareEnabledDrawables(), canvas, params);
    }

    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    auto drawParams = RSUniRenderUtil::CreateBufferDrawParam(*GetRSSurfaceHandlerOnDraw(), false);

    // To get dump image
    // execute "param set rosen.dumpsurfacetype.enabled 4 && setenforce 0 && param set rosen.afbc.enabled 0"
    RSBaseRenderUtil::WriteSurfaceBufferToPng(drawParams.buffer);
    renderEngine->DrawDisplayNodeWithParams(*rscanvas, *GetRSSurfaceHandlerOnDraw(), drawParams);

    if (hwcTopNodesNum > 0) {
        AdjustZOrderAndDrawSurfaceNode(params.GetHardwareEnabledTopDrawables(), canvas, params);
    }
}

void RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes(Drawing::Canvas& canvas)
{
    bool noBuffer = (GetRSSurfaceHandlerOnDraw()->GetBuffer() == nullptr);
    if (!renderParams_ || noBuffer) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes params or buffer is null!");
        return;
    }

    auto displayParams = static_cast<RSDisplayRenderParams*>(renderParams_.get());
    Drawing::AutoCanvasRestore acr(canvas, true);
    DrawHardwareEnabledNodes(canvas, *displayParams);
}

void RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodesMissedInCacheImage(Drawing::Canvas& canvas)
{
    auto params = static_cast<RSDisplayRenderParams*>(GetRenderParams().get());
    if (!params) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodesMissedInCacheImage params is null!");
        return;
    }

    Drawing::AutoCanvasRestore acr(canvas, true);
    FindHardwareEnabledNodes(*params);
    if (params->GetHardwareEnabledDrawables().size() != 0) {
        AdjustZOrderAndDrawSurfaceNode(params->GetHardwareEnabledDrawables(), canvas, *params);
    }
}

void RSDisplayRenderNodeDrawable::DrawHardwareEnabledTopNodesMissedInCacheImage(Drawing::Canvas& canvas)
{
    auto params = static_cast<RSDisplayRenderParams*>(GetRenderParams().get());
    if (!params) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodesMissedInCacheImage params is null!");
        return;
    }

    Drawing::AutoCanvasRestore acr(canvas, true);
    if (params->GetHardwareEnabledTopDrawables().size() != 0) {
        AdjustZOrderAndDrawSurfaceNode(params->GetHardwareEnabledTopDrawables(), canvas, *params);
    }
}

void RSDisplayRenderNodeDrawable::DrawHardCursorNodesMissedInCacheImage(Drawing::Canvas& canvas)
{
    auto params = static_cast<RSDisplayRenderParams*>(GetRenderParams().get());
    if (!params) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawHardCursorNodesMissedInCacheImage params is null!");
        return;
    }

    Drawing::AutoCanvasRestore acr(canvas, true);
    FindHardCursorNodes(*params);
    if (params->GetHardwareEnabledTopDrawables().size() != 0) {
        AdjustZOrderAndDrawSurfaceNode(params->GetHardwareEnabledTopDrawables(), canvas, *params);
    }
}

void RSDisplayRenderNodeDrawable::FindHardCursorNodes(RSDisplayRenderParams& params)
{
    params.GetHardwareEnabledTopDrawables().clear();
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!uniParam) {
        RS_LOGE("RSDisplayRenderNodeDrawable::FindHardCursorNodes uniParam is null");
        return;
    }
    auto& hardCursorDrawables = uniParam->GetHardCursorDrawables();
    if (hardCursorDrawables.empty()) {
        return;
    }
    auto iter = hardCursorDrawables.find(GetId());
    if (iter != hardCursorDrawables.end()) {
        auto& hardCursorDrawable = iter->second;
        if (!hardCursorDrawable) {
            return;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams *>(hardCursorDrawable->GetRenderParams().get());
        if (!surfaceParams) {
            RS_LOGE("RSDisplayRenderNodeDrawable::FindHardwareEnabledNodes surfaceParams is null");
            return;
        }
        auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(hardCursorDrawable);
        if (surfaceDrawable && surfaceParams->GetHardCursorStatus()) {
            params.GetHardwareEnabledTopDrawables().emplace_back(surfaceDrawable);
        }
    }
}

void RSDisplayRenderNodeDrawable::SwitchColorFilter(RSPaintFilterCanvas& canvas, float hdrBrightnessRatio,
    bool displayP3Enable) const
{
    const auto& renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (!renderEngine) {
        RS_LOGE("RSDisplayRenderNodeDrawable::SwitchColorFilter renderEngine is null");
        return;
    }
    ColorFilterMode colorFilterMode = renderEngine->GetColorFilterMode();
    if (colorFilterMode == ColorFilterMode::INVERT_COLOR_DISABLE_MODE ||
        colorFilterMode >= ColorFilterMode::DALTONIZATION_NORMAL_MODE) {
        return;
    }

    if (displayP3Enable) {
        SwitchColorFilterWithP3(canvas, colorFilterMode, hdrBrightnessRatio);
        return;
    }

    Drawing::AutoCanvasRestore acr(*curCanvas_, true);
    RS_TRACE_NAME_FMT("RSDisplayRenderNodeDrawable::SetColorFilterModeToPaint mode:%d",
        static_cast<int32_t>(colorFilterMode));
    Drawing::Brush brush;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, brush, hdrBrightnessRatio);
#if defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)
    RSTagTracker tagTracker(
        renderEngine->GetRenderContext()->GetSharedDrGPUContext(),
        RSTagTracker::TAG_SAVELAYER_COLOR_FILTER);
#endif
    Drawing::SaveLayerOps slr(nullptr, &brush, Drawing::SaveLayerOps::INIT_WITH_PREVIOUS);
    canvas.SaveLayer(slr);
}

void RSDisplayRenderNodeDrawable::SwitchColorFilterWithP3(RSPaintFilterCanvas& canvas,
    ColorFilterMode colorFilterMode, float hdrBrightnessRatio) const
{
    RS_TRACE_NAME_FMT("RSDisplayRenderNodeDrawable::SwitchColorFilterWithP3 mode:%d",
        static_cast<int32_t>(colorFilterMode));

    int32_t offscreenWidth = canvas.GetWidth();
    int32_t offscreenHeight = canvas.GetHeight();

    Drawing::ImageInfo info = Drawing::ImageInfo { offscreenWidth, offscreenHeight,
        Drawing::COLORTYPE_RGBA_F16, Drawing::ALPHATYPE_PREMUL, Drawing::ColorSpace::CreateSRGB()};
    auto offscreenSurface = canvas.GetSurface()->MakeSurface(info);
    auto offscreenCanvas = std::make_shared<RSPaintFilterCanvas>(offscreenSurface.get());

    Drawing::Brush brush;
    auto originSurfaceImage = canvas.GetSurface()->GetImageSnapshot();
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, brush, hdrBrightnessRatio);
    offscreenCanvas->AttachBrush(brush);
    offscreenCanvas->DrawImage(*originSurfaceImage, 0.f, 0.f, Drawing::SamplingOptions());
    offscreenCanvas->DetachBrush();

    auto offscreenImage = offscreenCanvas->GetSurface()->GetImageSnapshot();
    Drawing::AutoCanvasRestore acr(canvas, true);
    canvas.ResetMatrix();
    canvas.DrawImage(*offscreenImage, 0.f, 0.f, Drawing::SamplingOptions());
}

void RSDisplayRenderNodeDrawable::FindHardwareEnabledNodes(RSDisplayRenderParams& params)
{
    params.GetHardwareEnabledTopDrawables().clear();
    params.GetHardwareEnabledDrawables().clear();
    auto& hardwareDrawables =
        RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeDrawables();
    for (const auto& [displayNodeId, drawable] : hardwareDrawables) {
        auto surfaceNodeDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(drawable);
        if (!surfaceNodeDrawable || !surfaceNodeDrawable->ShouldPaint() ||
            displayNodeId != params.GetId()) {
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        if (surfaceParams == nullptr || !surfaceParams->GetHardwareEnabled()) {
            continue;
        }
        // To get dump image
        // execute "param set rosen.dumpsurfacetype.enabled 4 && setenforce 0 && param set rosen.afbc.enabled 0"
        auto buffer = surfaceParams->GetBuffer();
        RSBaseRenderUtil::WriteSurfaceBufferToPng(buffer, surfaceParams->GetId());
        if (surfaceNodeDrawable->IsHardwareEnabledTopSurface() || surfaceParams->IsLayerTop()) {
            // surfaceNode which should be drawn above displayNode like pointer window
            params.GetHardwareEnabledTopDrawables().emplace_back(drawable);
        } else {
            // surfaceNode which should be drawn below displayNode
            params.GetHardwareEnabledDrawables().emplace_back(drawable);
        }
    }
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!uniParam) {
        RS_LOGE("RSDisplayRenderNodeDrawable::FindHardCursorNodes uniParam is null");
        return;
    }
    auto& hardCursorDrawables = uniParam->GetHardCursorDrawables();
    if (hardCursorDrawables.empty()) {
        return;
    }
    auto iter = hardCursorDrawables.find(GetId());
    if (iter != hardCursorDrawables.end()) {
        auto& hardCursorDrawable = iter->second;
        if (!hardCursorDrawable) {
            return;
        }
        auto surfaceNodeDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(hardCursorDrawable);
        if (!surfaceNodeDrawable) {
            return;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        if (surfaceParams == nullptr) {
            return;
        }
        if (!RSUniRenderThread::GetCaptureParam().isSnapshot_) {
            auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(hardCursorDrawable);
            if (surfaceDrawable && surfaceParams->GetHardCursorStatus()) {
                params.GetHardwareEnabledTopDrawables().emplace_back(surfaceDrawable);
            }
        }
    }
}

void RSDisplayRenderNodeDrawable::AdjustZOrderAndDrawSurfaceNode(
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& drawables,
    Drawing::Canvas& canvas, RSDisplayRenderParams& params) const
{
    if (!RSSystemProperties::GetHardwareComposerEnabled()) {
        RS_LOGW("RSDisplayRenderNodeDrawable::AdjustZOrderAndDrawSurfaceNode: \
            HardwareComposer is not enabled.");
        return;
    }

    // sort the surfaceNodes by ZOrder
    std::stable_sort(drawables.begin(), drawables.end(), [](const auto& first, const auto& second) -> bool {
        auto firstDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(first);
        auto secondDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(second);
        if (!firstDrawable || !firstDrawable->GetRenderParams() ||
            !secondDrawable || !secondDrawable->GetRenderParams()) {
            return false;
        }
        return firstDrawable->GetRenderParams()->GetLayerInfo().zOrder <
               secondDrawable->GetRenderParams()->GetLayerInfo().zOrder;
    });

    Drawing::AutoCanvasRestore acr(canvas, true);
    auto rscanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rscanvas) {
        RS_LOGE("RSDisplayRenderNodeDrawable::AdjustZOrderAndDrawSurfaceNode, rscanvas is nullptr");
        return;
    }
    // draw hardware-composition nodes
    for (auto& drawable : drawables) {
        Drawing::AutoCanvasRestore acr(canvas, true);
        if (!drawable || !drawable->GetRenderParams()) {
            RS_LOGE("RSDisplayRenderNodeDrawable::AdjustZOrderAndDrawSurfaceNode surfaceParams is nullptr");
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable->GetRenderParams().get());
        // SelfDrawingNodes need to use LayerMatrix(totalMatrix) when doing capturing
        auto matrix = surfaceParams->GetLayerInfo().matrix;
        // Use for mirror screen visible rect projection
        auto rect = surfaceParams->GetLayerInfo().dstRect;
        auto dstRect = Drawing::RectI(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
        const auto &visibleRect = RSUniRenderThread::Instance().GetVisibleRect();
        if (dstRect.Intersect(visibleRect)) {
            matrix.PostTranslate(-visibleRect.GetLeft(), -visibleRect.GetTop());
        }
        canvas.ConcatMatrix(matrix);
        auto surfaceNodeDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(drawable);
        surfaceNodeDrawable->DealWithSelfDrawingNodeBuffer(*rscanvas, *surfaceParams);
    }
}

void RSDisplayRenderNodeDrawable::DrawWatermarkIfNeed(RSDisplayRenderParams& params, RSPaintFilterCanvas& canvas) const
{
    if (!RSUniRenderThread::Instance().GetWatermarkFlag()) {
        return;
    }
    auto image = RSUniRenderThread::Instance().GetWatermarkImg();
    if (image == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawWatermarkIfNeed image is null");
        return;
    }
    if (auto screenManager = CreateOrGetScreenManager()) {
        RS_TRACE_FUNC();
        auto screenInfo = screenManager->QueryScreenInfo(params.GetScreenId());
        auto mainWidth = static_cast<float>(screenInfo.width);
        auto mainHeight = static_cast<float>(screenInfo.height);

        // in certain cases (such as fold screen), the width and height must be swapped to fix the screen rotation.
        int angle = RSUniRenderUtil::GetRotationFromMatrix(canvas.GetTotalMatrix());
        if (angle == RS_ROTATION_90 || angle == RS_ROTATION_270) {
            std::swap(mainWidth, mainHeight);
        }
        auto srcRect = Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight());
        auto dstRect = Drawing::Rect(0, 0, mainWidth, mainHeight);
        Drawing::Brush rectBrush;
        canvas.AttachBrush(rectBrush);
        canvas.DrawImageRect(*image, srcRect, dstRect, Drawing::SamplingOptions(),
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        canvas.DetachBrush();
    }
}

void RSDisplayRenderNodeDrawable::DrawCurtainScreen() const
{
    if (!RSUniRenderThread::Instance().IsCurtainScreenOn() || !curCanvas_) {
        return;
    }
    RS_TRACE_FUNC();
    curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
}

void RSDisplayRenderNodeDrawable::UpdateSlrScale(ScreenInfo& screenInfo, RSDisplayRenderParams* params)
{
    if (screenInfo.isSamplingOn && RSSystemProperties::GetSLRScaleEnabled()) {
        if (slrScale_ == nullptr) {
            slrScale_ = std::make_unique<RSSLRScaleFunction>(
                screenInfo.phyWidth, screenInfo.phyHeight, screenInfo.width, screenInfo.height);
        } else {
            slrScale_->CheckOrRefreshScreen(
                screenInfo.phyWidth, screenInfo.phyHeight, screenInfo.width, screenInfo.height);
        }
        if (params != nullptr) {
            slrScale_->CheckOrRefreshColorSpace(params->GetNewColorSpace());
        }
        screenInfo.samplingDistance = slrScale_->GetKernelSize();
    } else {
        slrScale_ = nullptr;
    }
}

void RSDisplayRenderNodeDrawable::ScaleCanvasIfNeeded(const ScreenInfo& screenInfo)
{
    if (!screenInfo.isSamplingOn) {
        slrScale_ = nullptr;
        return;
    }
    if (RSSystemProperties::GetSLRScaleEnabled() && slrScale_ != nullptr) {
        slrScale_->CanvasScale(*curCanvas_);
        return;
    }
    slrScale_ = nullptr;
    if (enableVisibleRect_) {
        // save canvas matrix to calculate visible clip rect
        visibleClipRectMatrix_ = curCanvas_->GetTotalMatrix();
    }
    curCanvas_->Translate(screenInfo.samplingTranslateX, screenInfo.samplingTranslateY);
    curCanvas_->Scale(screenInfo.samplingScale, screenInfo.samplingScale);
}

void RSDisplayRenderNodeDrawable::ClearTransparentBeforeSaveLayer()
{
    if (!canvasBackup_) {
        return;
    }
    RS_TRACE_NAME("ClearTransparentBeforeSaveLayer");
    auto& hardwareDrawables =
        RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeDrawables();
    if (UNLIKELY(!renderParams_)) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw renderParams is null!");
        return;
    }
    auto params = static_cast<RSDisplayRenderParams*>(renderParams_.get());
    for (const auto& [displayNodeId, drawable] : hardwareDrawables) {
        auto surfaceDrawable = static_cast<RSSurfaceRenderNodeDrawable*>(drawable.get());
        if (!surfaceDrawable || displayNodeId != params->GetId()) {
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable->GetRenderParams().get());
        if (!surfaceParams || !surfaceParams->GetHardwareEnabled()) {
            continue;
        }
        RSAutoCanvasRestore arc(canvasBackup_.get());
        canvasBackup_->SetMatrix(surfaceParams->GetLayerInfo().matrix);
        canvasBackup_->ClipRect(surfaceParams->GetBounds());
        canvasBackup_->Clear(Drawing::Color::COLOR_TRANSPARENT);
    }
}

void RSDisplayRenderNodeDrawable::PrepareHdrDraw(int32_t offscreenWidth, int32_t offscreenHeight)
{
    offscreenSurface_ = curCanvas_->GetSurface()->MakeSurface(offscreenWidth, offscreenHeight);
}

void RSDisplayRenderNodeDrawable::FinishHdrDraw(Drawing::Brush& paint, float hdrBrightnessRatio)
{
    return;
}

bool RSDisplayRenderNodeDrawable::EnablescRGBForP3AndUiFirst(const GraphicColorGamut& currentGamut)
{
    return RSSystemParameters::IsNeedScRGBForP3(currentGamut) && RSUifirstManager::Instance().GetUiFirstSwitch();
}

void RSDisplayRenderNodeDrawable::PrepareOffscreenRender(const RSDisplayRenderNodeDrawable& displayDrawable,
    bool useFixedSize, bool useCanvasSize)
{
    RS_TRACE_NAME_FMT("%s: useFixedSize:%d, useCanvasSize:%d", __func__, useFixedSize, useCanvasSize);
    // params not null in caller
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable.GetRenderParams().get());
    // cleanup
    canvasBackup_ = nullptr;
    // check offscreen size and hardware renderer
    useFixedOffscreenSurfaceSize_ = false;
    const ScreenInfo& screenInfo = params->GetScreenInfo();
    int32_t offscreenWidth = static_cast<int32_t>(screenInfo.width);
    int32_t offscreenHeight = static_cast<int32_t>(screenInfo.height);
    // use fixed surface size in order to reduce create texture
    if (useFixedSize && RotateOffScreenParam::GetRotateOffScreenDisplayNodeEnable()
        && params->IsRotationChanged()) {
        useFixedOffscreenSurfaceSize_ = true;
        int32_t maxRenderSize;
        if (RSSystemProperties::GetCacheOptimizeRotateEnable()) {
            int32_t screenWidth = params->GetScreenInfo().width;
            int32_t screenHeight = params->GetScreenInfo().height;
            maxRenderSize = std::ceil(std::sqrt(screenWidth * screenWidth + screenHeight * screenHeight));
            offscreenTranslateX_ = std::round((maxRenderSize - offscreenWidth) / HALF);
            offscreenTranslateY_ = std::round((maxRenderSize - offscreenHeight) / HALF);
        } else {
            maxRenderSize =
                static_cast<int32_t>(std::max(params->GetScreenInfo().width, params->GetScreenInfo().height));
            if (offscreenSurface_ != nullptr
                && maxRenderSize != std::max(offscreenSurface_->Width(), offscreenSurface_->Height())) {
                RS_TRACE_NAME("offscreen surface's max size has changed");
                offscreenSurface_ = nullptr;
            }
        }
        offscreenWidth = maxRenderSize;
        offscreenHeight = maxRenderSize;
    } else {
        offscreenTranslateX_ = 0;
        offscreenTranslateY_ = 0;
    }
    if (params->IsRotationChanged()) {
        if (RSUniRenderThread::Instance().GetVmaOptimizeFlag()) {
            Drawing::StaticFactory::SetVmaCacheStatus(true); // render this frame with vma cache on
        }
    }

    if (offscreenWidth <= 0 || offscreenHeight <= 0) {
        RS_LOGE("RSDisplayRenderNodeDrawable::PrepareOffscreenRender, offscreenWidth or offscreenHeight is invalid");
        return;
    }
    if (curCanvas_->GetSurface() == nullptr) {
        curCanvas_->ClipRect(Drawing::Rect(0, 0, offscreenWidth, offscreenHeight), Drawing::ClipOp::INTERSECT, false);
        RS_LOGE("RSDisplayRenderNodeDrawable::PrepareOffscreenRender, current surface is nullptr");
        return;
    }
    if (!params->GetNeedOffscreen() || !useFixedOffscreenSurfaceSize_ || offscreenSurface_ == nullptr ||
        (params->GetHDRPresent() &&
        offscreenSurface_->GetImageInfo().GetColorType() != Drawing::ColorType::COLORTYPE_RGBA_F16)) {
        RS_TRACE_NAME_FMT("make offscreen surface with fixed size: [%d, %d]", offscreenWidth, offscreenHeight);
        bool isScRGBEnable = EnablescRGBForP3AndUiFirst(params->GetNewColorSpace());
        if (!params->GetNeedOffscreen() && (params->GetHDRPresent() || isScRGBEnable) && useCanvasSize) {
            offscreenWidth = curCanvas_->GetWidth();
            offscreenHeight = curCanvas_->GetHeight();
        }
        if (params->GetHDRPresent() || isScRGBEnable) {
            PrepareHdrDraw(offscreenWidth, offscreenHeight);
        } else {
            offscreenSurface_ = curCanvas_->GetSurface()->MakeSurface(offscreenWidth, offscreenHeight);
        }
    }
    // create offscreen surface and canvas
    if (useFixedOffscreenSurfaceSize_) {
        if (offscreenSurface_ == nullptr) {
            RS_TRACE_NAME_FMT("make offscreen surface with fixed size: [%d, %d]", offscreenWidth, offscreenHeight);
            offscreenSurface_ = curCanvas_->GetSurface()->MakeSurface(offscreenWidth, offscreenHeight);
        }
    } else {
        offscreenSurface_ = curCanvas_->GetSurface()->MakeSurface(offscreenWidth, offscreenHeight);
    }

    if (offscreenSurface_ == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::PrepareOffscreenRender, offscreenSurface is nullptr");
        curCanvas_->ClipRect(Drawing::Rect(0, 0, offscreenWidth, offscreenHeight), Drawing::ClipOp::INTERSECT, false);
        return;
    }
    auto offscreenCanvas = std::make_shared<RSPaintFilterCanvas>(offscreenSurface_.get());

    if (RSSystemProperties::GetCacheOptimizeRotateEnable()) {
        offscreenCanvas->ResetMatrix();
        offscreenCanvas->Translate(offscreenTranslateX_, offscreenTranslateY_);
    }

    // copy HDR properties into offscreen canvas
    offscreenCanvas->CopyHDRConfiguration(*curCanvas_);
    // copy current canvas properties into offscreen canvas
    offscreenCanvas->CopyConfigurationToOffscreenCanvas(*curCanvas_);

    // backup current canvas and replace with offscreen canvas
    canvasBackup_ = std::exchange(curCanvas_, offscreenCanvas);
}

std::shared_ptr<Drawing::ShaderEffect> RSDisplayRenderNodeDrawable::MakeBrightnessAdjustmentShader(
    const std::shared_ptr<Drawing::Image>& image, const Drawing::SamplingOptions& sampling, float hdrBrightnessRatio)
{
    static const std::string shaderString(R"(
        uniform shader imageInput;
        uniform float ratio;
        half4 main(float2 xy) {
            half4 c = imageInput.eval(xy);
            return half4(c.rgb * ratio, c.a);
        }
    )");
    if (brightnessAdjustmentShaderEffect_ == nullptr) {
        brightnessAdjustmentShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(shaderString);
        if (brightnessAdjustmentShaderEffect_ == nullptr) {
            ROSEN_LOGE("RSDisplayRenderNodeDrawable::MakeBrightnessAdjustmentShaderBuilder effect is null");
            return nullptr;
        }
    }

    auto builder = std::make_shared<Drawing::RuntimeShaderBuilder>(brightnessAdjustmentShaderEffect_);
    if (!builder) {
        ROSEN_LOGE("RSDisplayRenderNodeDrawable::MakeBrightnessAdjustmentShaderBuilder builder is null");
        return nullptr;
    }
    builder->SetChild("imageInput", Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, sampling, Drawing::Matrix()));
    builder->SetUniform("ratio", hdrBrightnessRatio);
    return builder->MakeShader(nullptr, false);
}

void RSDisplayRenderNodeDrawable::FinishOffscreenRender(
    const Drawing::SamplingOptions& sampling, bool isSamplingOn, float hdrBrightnessRatio)
{
    RS_TRACE_NAME_FMT("%s: isSamplingOn:%d, hdrBrightnessRatio:%f", __func__, isSamplingOn, hdrBrightnessRatio);
    if (canvasBackup_ == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::FinishOffscreenRender, canvasBackup_ is nullptr");
        return;
    }
    if (offscreenSurface_ == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::FinishOffscreenRender, offscreenSurface_ is nullptr");
        return;
    }
    auto image = offscreenSurface_->GetImageSnapshot();
    if (image == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::FinishOffscreenRender, Surface::GetImageSnapshot is nullptr");
        return;
    }
    // draw offscreen surface to current canvas
    Drawing::Brush paint;
    bool isUseCustomShader = false;
    if (ROSEN_LNE(hdrBrightnessRatio, 1.0f)) {
        auto shader = MakeBrightnessAdjustmentShader(image, sampling, hdrBrightnessRatio);
        if (shader) {
            paint.SetShaderEffect(shader);
            isUseCustomShader = true;
        } else {
            FinishHdrDraw(paint, hdrBrightnessRatio);
        }
    }
    paint.SetAntiAlias(true);
    canvasBackup_->AttachBrush(paint);
    if (isSamplingOn) {
        if (RSSystemProperties::GetSLRScaleEnabled() && slrScale_) {
            slrScale_->ProcessOffscreenImage(*canvasBackup_, *image);
        } else {
            canvasBackup_->DrawImage(*image, 0, 0, sampling);
        }
    } else if (RSSystemProperties::GetCacheOptimizeRotateEnable()) {
        if (isUseCustomShader) {
            Drawing::Rect imageRect { 0., 0., image->GetImageInfo().GetWidth(), image->GetImageInfo().GetHeight() };
            canvasBackup_->Translate(-offscreenTranslateX_, -offscreenTranslateY_);
            canvasBackup_->DrawRect(imageRect);
        } else {
            canvasBackup_->DrawImage(*image, -offscreenTranslateX_, -offscreenTranslateY_, sampling);
        }
        canvasBackup_->Translate(offscreenTranslateX_, offscreenTranslateY_);
    } else {
        if (isUseCustomShader) {
            canvasBackup_->DrawRect({ 0., 0., image->GetImageInfo().GetWidth(), image->GetImageInfo().GetHeight() });
        } else {
            canvasBackup_->DrawImage(*image, 0, 0, sampling);
        }
    }
    canvasBackup_->DetachBrush();
    // restore current canvas and cleanup
    if (!useFixedOffscreenSurfaceSize_) {
        offscreenSurface_ = nullptr;
    }
    curCanvas_ = std::move(canvasBackup_);
}

void RSDisplayRenderNodeDrawable::UpdateSurfaceDrawRegion(std::shared_ptr<RSPaintFilterCanvas>& mainCanvas,
    RSDisplayRenderParams* params)
{
    auto& curAllSurfaces = params->GetAllMainAndLeashSurfaceDrawables();

    for (auto& renderNodeDrawable : curAllSurfaces) {
        if (renderNodeDrawable != nullptr &&
            renderNodeDrawable->GetNodeType() == RSRenderNodeType::SURFACE_NODE) {
            auto* surfaceNodeDrawable = static_cast<DrawableV2::RSSurfaceRenderNodeDrawable*>(renderNodeDrawable.get());
            surfaceNodeDrawable->UpdateSurfaceDirtyRegion(mainCanvas);
        }
    }
}

#ifndef ROSEN_CROSS_PLATFORM
bool RSDisplayRenderNodeDrawable::CreateSurface(sptr<IBufferConsumerListener> listener)
{
    auto consumer = surfaceHandler_->GetConsumer();
    if (consumer != nullptr && surface_ != nullptr) {
        RS_LOGI("RSDisplayRenderNode::CreateSurface already created, return");
        return true;
    }
    consumer = IConsumerSurface::Create("DisplayNode");
    if (consumer == nullptr) {
        RS_LOGE("RSDisplayRenderNode::CreateSurface get consumer surface fail");
        return false;
    }
    SurfaceError ret = consumer->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSDisplayRenderNode::CreateSurface RegisterConsumerListener fail");
        return false;
    }
    consumerListener_ = listener;
    auto producer = consumer->GetProducer();
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);
    if (!surface) {
        RS_LOGE("RSDisplayRenderNode::CreateSurface CreateSurfaceAsProducer fail");
        return false;
    }
    surface->SetQueueSize(BUFFER_SIZE);
    auto client = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    surface_ = client->CreateRSSurface(surface);
    RS_LOGI("RSDisplayRenderNode::CreateSurface end");
    surfaceCreated_ = true;
    surfaceHandler_->SetConsumer(consumer);
    return true;
}
#endif

bool RSDisplayRenderNodeDrawable::SkipFrameByInterval(uint32_t refreshRate, uint32_t skipFrameInterval)
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

bool RSDisplayRenderNodeDrawable::SkipFrameByRefreshRate(uint32_t refreshRate, uint32_t expectedRefreshRate)
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

bool RSDisplayRenderNodeDrawable::SkipFrame(uint32_t refreshRate, ScreenInfo screenInfo)
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

void RSDisplayRenderNodeDrawable::MirrorRedrawDFX(bool mirrorRedraw, ScreenId screenId)
{
    if (mirrorRedraw_ != mirrorRedraw) {
        mirrorRedraw_ = mirrorRedraw;
        RS_LOGI("RSDisplayRenderNodeDrawable::%{public}s mirror screenId: %{public}" PRIu64
            " drawing path changed, mirrorRedraw_: %{public}d", __func__, screenId, mirrorRedraw_);
    }
}
} // namespace OHOS::Rosen::DrawableV2
