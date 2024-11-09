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

#include "luminance/rs_luminance_control.h"
#include "rs_trace.h"
#include "system/rs_system_parameters.h"

#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "common/rs_singleton.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_display_render_params.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/round_corner_display/rs_rcd_render_manager.h"
#include "pipeline/round_corner_display/rs_round_corner_display_manager.h"
#include "pipeline/rs_anco_manager.h"
#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_uifirst_manager.h"
#include "pipeline/rs_uni_render_listener.h"
#include "pipeline/rs_uni_render_thread.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_uni_render_virtual_processor.h"
#include "pipeline/sk_resource_manager.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "pipeline/magic_pointer_render/rs_magic_pointer_render_manager.h"
#endif
#include "platform/common/rs_log.h"
#include "platform/ohos/rs_jank_stats.h"
#include "property/rs_point_light_manager.h"
#include "screen_manager/rs_screen_manager.h"
#include "static_factory.h"
// dfx
#include "drawable/dfx/rs_dirty_rects_dfx.h"
#include "drawable/dfx/rs_skp_capture_dfx.h"
#include "platform/ohos/overdraw/rs_overdraw_controller.h"
#include "utils/performanceCaculate.h"
namespace OHOS::Rosen::DrawableV2 {
namespace {
constexpr const char* CLEAR_GPU_CACHE = "ClearGpuCache";
constexpr const char* DEFAULT_CLEAR_GPU_CACHE = "DefaultClearGpuCache";
constexpr int32_t NO_SPECIAL_LAYER = 0;
constexpr int32_t HAS_SPECIAL_LAYER = 1;
constexpr int32_t CAPTURE_WINDOW = 2; // To be deleted after captureWindow being deleted
constexpr int64_t MAX_JITTER_NS = 2000000; // 2ms

std::string RectVectorToString(std::vector<RectI>& regionRects)
{
    std::string results = "";
    for (auto& rect : regionRects) {
        results += rect.ToString();
    }
    return results;
}

Drawing::Region GetFlippedRegion(std::vector<RectI>& rects, ScreenInfo& screenInfo)
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
void DoScreenRcdTask(NodeId id, std::shared_ptr<RSProcessor>& processor, std::unique_ptr<RcdInfo>& rcdInfo,
    const ScreenInfo& screenInfo)
{
    if (screenInfo.state != ScreenState::HDI_OUTPUT_ENABLE) {
        RS_LOGD("DoScreenRcdTask is not at HDI_OUPUT mode");
        return;
    }
    if (RSSingleton<RoundCornerDisplayManager>::GetInstance().GetRcdEnable()) {
        RSSingleton<RoundCornerDisplayManager>::GetInstance().RunHardwareTask(id,
            [id, &processor, &rcdInfo](void) {
                auto hardInfo = RSSingleton<RoundCornerDisplayManager>::GetInstance().GetHardwareInfo(id, true);
                rcdInfo->processInfo = {processor, hardInfo.topLayer, hardInfo.bottomLayer,
                    hardInfo.resourceChanged};
                RSRcdRenderManager::GetInstance().DoProcessRenderTask(id, rcdInfo->processInfo);
            });
    }
}

RSDisplayRenderNodeDrawable::Registrar RSDisplayRenderNodeDrawable::instance_;

RSDisplayRenderNodeDrawable::RSDisplayRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node)), surfaceHandler_(std::make_shared<RSSurfaceHandler>(nodeId_)),
      syncDirtyManager_(std::make_shared<RSDirtyRegionManager>(true))
{}

RSRenderNodeDrawable::Ptr RSDisplayRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSDisplayRenderNodeDrawable(std::move(node));
}

static std::vector<RectI> MergeDirtyHistory(RSDisplayRenderNodeDrawable& displayDrawable,
    int32_t bufferAge, ScreenInfo& screenInfo, RSDirtyRectsDfx& rsDirtyRectsDfx, RSDisplayRenderParams& params)
{
    // renderThreadParams/dirtyManager not null in caller
    auto dirtyManager = displayDrawable.GetSyncDirtyManager();
    auto& curAllSurfaceDrawables = params.GetAllMainAndLeashSurfaceDrawables();
    RSUniRenderUtil::MergeDirtyHistoryForDrawable(displayDrawable, bufferAge, params, false);
    Occlusion::Region dirtyRegion = RSUniRenderUtil::MergeVisibleDirtyRegion(
        curAllSurfaceDrawables, RSUniRenderThread::Instance().GetDrawStatusVec(), false);
    const auto clipRectThreshold = RSSystemProperties::GetClipRectThreshold();
    if (clipRectThreshold < 1.f) {
        Occlusion::Region allDirtyRegion{ Occlusion::Rect{ dirtyManager->GetDirtyRegion() } };
        allDirtyRegion.OrSelf(dirtyRegion);
        auto bound = allDirtyRegion.GetBound();
        if (allDirtyRegion.GetSize() > 1 && !bound.IsEmpty() &&
            allDirtyRegion.Area() > bound.Area() * clipRectThreshold) {
            dirtyManager->MergeDirtyRectAfterMergeHistory(bound.ToRectI());
            RS_OPTIONAL_TRACE_NAME_FMT("dirty expand: %s to %s",
                allDirtyRegion.GetRegionInfo().c_str(), bound.GetRectInfo().c_str());
        }
    }
    RSUniRenderUtil::SetAllSurfaceDrawableGlobalDityRegion(curAllSurfaceDrawables, dirtyManager->GetDirtyRegion());

    // DFX START
    rsDirtyRectsDfx.SetDirtyRegion(dirtyRegion);
    // DFX END

    RectI rect = dirtyManager->GetDirtyRegionFlipWithinSurface();
    auto rects = RSUniRenderUtil::ScreenIntersectDirtyRects(dirtyRegion, screenInfo);
    if (!rect.IsEmpty()) {
        rects.emplace_back(rect);
        RectI screenRectI(0, 0, static_cast<int32_t>(screenInfo.phyWidth), static_cast<int32_t>(screenInfo.phyHeight));
        GpuDirtyRegionCollection::GetInstance().UpdateGlobalDirtyInfoForDFX(rect.IntersectRect(screenRectI));
    }

    return rects;
}

static std::vector<RectI> MergeDirtyHistoryInVirtual(RSDisplayRenderNodeDrawable& displayDrawable,
    int32_t bufferAge, ScreenInfo& screenInfo)
{
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable.GetRenderParams().get());
    auto& renderThreadParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!renderThreadParams || !params) {
        return {};
    }
    auto& curAllSurfaceDrawables = params->GetAllMainAndLeashSurfaceDrawables();
    auto dirtyManager = displayDrawable.GetSyncDirtyManager();
    RSUniRenderUtil::MergeDirtyHistoryInVirtual(displayDrawable, bufferAge);
    Occlusion::Region dirtyRegion = RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(curAllSurfaceDrawables);

    RectI rect = dirtyManager->GetRectFlipWithinSurface(dirtyManager->GetDirtyRegionInVirtual());
    auto rects = RSUniRenderUtil::ScreenIntersectDirtyRects(dirtyRegion, screenInfo);
    if (!rect.IsEmpty()) {
        rects.emplace_back(rect);
    }

    return rects;
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
    auto bufferConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(params.GetScreenInfo(), true, false,
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
    auto& hardCursorDrawable = uniParam->GetHardCursorDrawables();
    if (hardCursorDrawable.id != GetId() || !hardCursorDrawable.drawablePtr) {
        return false;
    }
    auto surfaceParams =
        static_cast<RSSurfaceRenderParams*>(hardCursorDrawable.drawablePtr->GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("RSDisplayRenderNodeDrawable::HardCursorCreateLayer surfaceParams is null");
        return false;
    }
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(hardCursorDrawable.drawablePtr);
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

bool RSDisplayRenderNodeDrawable::CheckDisplayNodeSkip(
    RSDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor)
{
    if (GetSyncDirtyManager()->IsCurrentFrameDirty() ||
        (params.GetMainAndLeashSurfaceDirty() || RSUifirstManager::Instance().HasDoneNode()) ||
        RSMainThread::Instance()->GetDirtyFlag()) {
        return false;
    }

    RS_LOGD("DisplayNode skip");
    RS_TRACE_NAME("DisplayNode skip");
    GpuDirtyRegionCollection::GetInstance().AddSkipProcessFramesNumberForDFX(RSBaseRenderUtil::GetLastSendingPid());
#ifdef OHOS_PLATFORM
    RSUniRenderThread::Instance().SetSkipJankAnimatorFrame(true);
#endif
    auto pendingDrawables = RSUifirstManager::Instance().GetPendingPostDrawables();
    auto isHardCursor = HardCursorCreateLayer(processor);
    if (!RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetForceCommitLayer() &&
        pendingDrawables.size() == 0 && !isHardCursor) {
        RS_TRACE_NAME("DisplayNodeSkip skip commit");
        return true;
    }

    if (!processor->InitForRenderThread(*this, INVALID_SCREEN_ID, RSUniRenderThread::Instance().GetRenderEngine())) {
        RS_LOGE("RSDisplayRenderNodeDrawable::CheckDisplayNodeSkip processor init failed");
        return false;
    }

    auto& hardwareDrawables =
        RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeDrawables();
    for (const auto& drawable : hardwareDrawables) {
        if (UNLIKELY(!drawable || !drawable->GetRenderParams())) {
            continue;
        }
        if (drawable->GetRenderParams()->GetHardwareEnabled()) {
            auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(drawable);
            processor->CreateLayerForRenderThread(*surfaceDrawable);
        }
    }
    if (!RSMainThread::Instance()->WaitHardwareThreadTaskExecute()) {
        RS_LOGW("RSDisplayRenderNodeDrawable::CheckDisplayNodeSkip: hardwareThread task has too many to Execute");
    }
    processor->ProcessDisplaySurfaceForRenderThread(*this);

    RSUifirstManager::Instance().CreateUIFirstLayer(processor);

    // commit RCD layers
    auto rcdInfo = std::make_unique<RcdInfo>();
    const auto& screenInfo = params.GetScreenInfo();
    DoScreenRcdTask(params.GetId(), processor, rcdInfo, screenInfo);
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
    for (const auto& drawable : hardwareDrawables) {
        if (UNLIKELY(!drawable || !drawable->GetRenderParams())) {
            continue;
        }
        drawable->GetRenderParams()->SetLayerCreated(false);
    }
    auto& hardCursorDrawable = uniParam->GetHardCursorDrawables();
    if (hardCursorDrawable.id == GetId() && hardCursorDrawable.drawablePtr &&
        hardCursorDrawable.drawablePtr->GetRenderParams()) {
        hardCursorDrawable.drawablePtr->GetRenderParams()->SetLayerCreated(false);
    }
    // if screen power off, skip on draw, needs to draw one more frame.
    if (params && RSUniRenderUtil::CheckRenderSkipIfScreenOff(true, params->GetScreenId())) {
        SetDrawSkipType(DrawSkipType::RENDER_SKIP_IF_SCREEN_OFF);
        return;
    }

    PostClearMemoryTask();

    // dfx
    RSRenderNodeDrawable::InitDfxForCacheInfo();

    // check rotation for point light
    constexpr int ROTATION_NUM = 4;
    auto screenRotation = GetRenderParams()->GetScreenRotation();
    ScreenId paramScreenId = params->GetScreenId();
    offsetX_ = params->GetDisplayOffsetX();
    offsetY_ = params->GetDisplayOffsetY();
    curDisplayScreenId_ = paramScreenId;
    RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw curScreenId=[%{public}" PRIu64 "], "
        "offsetX=%{public}d, offsetY=%{public}d", paramScreenId, offsetX_, offsetY_);

    if (RSSystemProperties::IsFoldScreenFlag() && paramScreenId == 0) {
        screenRotation = static_cast<ScreenRotation>((static_cast<int>(screenRotation) + 1) % ROTATION_NUM);
    }
    RSPointLightManager::Instance()->SetScreenRotation(screenRotation);
    const RectI& dirtyRegion = GetSyncDirtyManager()->GetCurrentFrameDirtyRegion();
    RS_TRACE_NAME_FMT("RSDisplayRenderNodeDrawable[%" PRIu64 "](%d, %d, %d, %d), zoomed(%d)", paramScreenId,
        dirtyRegion.left_, dirtyRegion.top_, dirtyRegion.width_, dirtyRegion.height_, params->GetZoomed());
    RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw node: %{public}" PRIu64 "", GetId());
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        SetDrawSkipType(DrawSkipType::SCREEN_MANAGER_NULL);
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw ScreenManager is nullptr");
        return;
    }
    ScreenInfo curScreenInfo = screenManager->QueryScreenInfo(paramScreenId);
    RSScreenModeInfo modeInfo = {};
    screenManager->GetDefaultScreenActiveMode(modeInfo);
    uint32_t refreshRate = modeInfo.GetScreenRefreshRate();
    // skip frame according to skipFrameInterval value of SetScreenSkipFrameInterval interface
    if (SkipFrame(refreshRate, curScreenInfo.skipFrameInterval)) {
        SetDrawSkipType(DrawSkipType::SKIP_FRAME);
        RS_TRACE_NAME("SkipFrame, screenId:" + std::to_string(paramScreenId));
        screenManager->ForceRefreshOneFrameIfNoRNV();
        return;
    }
    if (curScreenInfo.skipFrameInterval > 1) {
        virtualDirtyRefresh_ = true;
    }

    auto screenInfo = params->GetScreenInfo();
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    if (!processor) {
        SetDrawSkipType(DrawSkipType::CREATE_PROCESSOR_FAIL);
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw RSProcessor is null!");
        return;
    }

    auto mirrorDrawable = params->GetMirrorSourceDrawable().lock();
    auto mirrorParams = mirrorDrawable ? mirrorDrawable->GetRenderParams().get() : nullptr;
    if (mirrorParams || params->GetCompositeType() == RSDisplayRenderNode::CompositeType::UNI_RENDER_EXPAND_COMPOSITE) {
        if (!processor->InitForRenderThread(*this,
            mirrorParams ? mirrorParams->GetScreenId() : INVALID_SCREEN_ID,
            RSUniRenderThread::Instance().GetRenderEngine())) {
            SetDrawSkipType(DrawSkipType::RENDER_ENGINE_NULL);
            syncDirtyManager_->ResetDirtyAsSurfaceSize();
            syncDirtyManager_->UpdateDirty(false);
            RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw processor init failed!");
            return;
        }
        if (mirrorParams) {
            enableVisibleRect_ = screenManager->QueryScreenInfo(paramScreenId).enableVisibleRect;
            if (enableVisibleRect_) {
                const auto& rect = screenManager->GetMirrorScreenVisibleRect(paramScreenId);
                curVisibleRect_ = Drawing::RectI(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
            }
            if (params->GetCompositeType() == RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE) {
                SetDrawSkipType(DrawSkipType::WIRED_SCREEN_PROJECTION);
                RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw wired screen projection.");
                WiredScreenProjection(*params, processor);
                lastVisibleRect_ = curVisibleRect_;
                return;
            }
            currentBlackList_ = screenManager->GetVirtualScreenBlackList(paramScreenId);
            RSUniRenderThread::Instance().SetBlackList(currentBlackList_);
            RSUniRenderThread::Instance().SetWhiteList(screenInfo.whiteList);
            curSecExemption_ = params->GetSecurityExemption();
            uniParam->SetSecExemption(curSecExemption_);
            RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw Mirror screen.");
            DrawMirrorScreen(*params, processor);
            lastBlackList_ = currentBlackList_;
            lastSecExemption_ = curSecExemption_;
            lastVisibleRect_ = curVisibleRect_;
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
            // disable expand screen dirty when skipFrameInterval > 1, because the dirty history is incorrect
            if (uniParam->IsExpandScreenDirtyEnabled() && uniParam->IsVirtualDirtyEnabled() &&
                curScreenInfo.skipFrameInterval <= 1) {
                int32_t bufferAge = expandProcessor->GetBufferAge();
                damageRegionRects = MergeDirtyHistory(*this, bufferAge, screenInfo, rsDirtyRectsDfx, *params);
                uniParam->Reset();
                if (!uniParam->IsVirtualDirtyDfxEnabled()) {
                    expandProcessor->SetDirtyInfo(damageRegionRects);
                }
            } else {
                std::vector<RectI> emptyRects = {};
                expandProcessor->SetRoiRegionToCodec(emptyRects);
            }
            rsDirtyRectsDfx.SetVirtualDirtyRects(damageRegionRects, screenInfo);
            DrawExpandScreen(*expandProcessor);
            if (curCanvas_) {
                rsDirtyRectsDfx.OnDrawVirtual(*curCanvas_);
            }
            uniParam->SetOpDropped(isOpDropped);
        }
        processor->PostProcess();
        SetDrawSkipType(DrawSkipType::MIRROR_DRAWABLE_SKIP);
        return;
    }

    bool isHdrOn = params->GetHDRPresent();
    ScreenId screenId = curScreenInfo.id;
    // 0 means defalut hdrBrightnessRatio
    float hdrBrightnessRatio = RSLuminanceControl::Get().GetHdrBrightnessRatio(screenId, 0);
    if (!isHdrOn) {
        params->SetBrightnessRatio(hdrBrightnessRatio);
        hdrBrightnessRatio = 1.0f;
    }
    RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw HDR content in UniRender:%{public}d, BrightnessRatio:%{public}f",
        isHdrOn, hdrBrightnessRatio);

    if (uniParam->IsOpDropped() && CheckDisplayNodeSkip(*params, processor) &&
        RSAncoManager::Instance()->GetAncoHebcStatus() == AncoHebcStatus::INITIAL) {
        SetDrawSkipType(DrawSkipType::DISPLAY_NODE_SKIP);
        RSMainThread::Instance()->SetFrameIsRender(false);
        SetDisplayNodeSkipFlag(*uniParam, true);
        return;
    }
    SetDisplayNodeSkipFlag(*uniParam, false);
    RSMainThread::Instance()->SetFrameIsRender(true);

    CheckAndUpdateFilterCacheOcclusion(*params, curScreenInfo);
    RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw HDR isHdrOn: %{public}d", isHdrOn);
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

    RSDirtyRectsDfx rsDirtyRectsDfx(*this);
    std::vector<RectI> damageRegionrects;
    Drawing::Region clipRegion;
    if (uniParam->IsPartialRenderEnabled() &&
        (RSMainThread::Instance()->GetDeviceType() != DeviceType::PC || paramScreenId == 0)) {
        damageRegionrects = MergeDirtyHistory(*this, renderFrame->GetBufferAge(), screenInfo, rsDirtyRectsDfx, *params);
        uniParam->Reset();
        clipRegion = GetFlippedRegion(damageRegionrects, screenInfo);
        RS_OPTIONAL_TRACE_NAME_FMT("SetDamageRegion damageRegionrects num: %lu, info: %s",
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
    curCanvas_->SetScreenId(screenId);
    curCanvas_->SetHdrOn(isHdrOn);
    curCanvas_->SetDisableFilterCache(params->GetZoomed());

#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (autoCacheEnable_) {
        screenRectInfo_ = {0, 0, screenInfo.width, screenInfo.height};
    }
#endif

    // canvas draw
    {
        {
            RSSkpCaptureDfx capture(curCanvas_);
            Drawing::AutoCanvasRestore acr(*curCanvas_, true);

            bool isOpDropped = uniParam->IsOpDropped();
            if (RSMainThread::Instance()->GetDeviceType() == DeviceType::PC && paramScreenId != 0) {
                uniParam->SetOpDropped(false);
            }
            bool isScRGBEnable = EnablescRGBForP3AndUiFirst(params->GetNewColorSpace());
            bool needOffscreen = params->GetNeedOffscreen() || isHdrOn || isScRGBEnable;
            if (params->GetNeedOffscreen()) {
                uniParam->SetOpDropped(false);
            }

            if (uniParam->IsOpDropped()) {
                uniParam->SetClipRegion(clipRegion);
                ClipRegion(*curCanvas_, clipRegion);
            } else if (params->GetNeedOffscreen()) {
                // draw black background in rotation for camera
                curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
            } else {
                curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
            }

            if (needOffscreen) {
                PrepareOffscreenRender(*this, true);
                curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
            }

            if (!params->GetNeedOffscreen()) {
                curCanvas_->ConcatMatrix(params->GetMatrix());
            }

            curCanvas_->SetHighContrast(RSUniRenderThread::Instance().IsHighContrastTextModeOn());
            RSRenderNodeDrawable::OnDraw(*curCanvas_);
            DrawCurtainScreen();
            if (needOffscreen) {
                if (canvasBackup_ != nullptr) {
                    Drawing::AutoCanvasRestore acr(*canvasBackup_, true);
                    if (params->GetNeedOffscreen()) {
                        canvasBackup_->ConcatMatrix(params->GetMatrix());
                    }
                    ClearTransparentBeforeSaveLayer();
                    FinishOffscreenRender(Drawing::SamplingOptions(Drawing::FilterMode::NEAREST,
                        Drawing::MipmapMode::NONE), hdrBrightnessRatio);
                    uniParam->SetOpDropped(isOpDropped);
                } else {
                    RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw canvasBackup_ is nullptr");
                }
            }
            if (RSMainThread::Instance()->GetDeviceType() == DeviceType::PC && paramScreenId != 0) {
                uniParam->SetOpDropped(isOpDropped);
            }
            // watermark and color filter should be applied after offscreen render.
            DrawWatermarkIfNeed(*params, *curCanvas_);
            SwitchColorFilter(*curCanvas_, hdrBrightnessRatio);
        }
        rsDirtyRectsDfx.OnDraw(*curCanvas_);
        if ((RSSystemProperties::IsFoldScreenFlag() || RSSystemProperties::IsTabletType())
            && !params->IsRotationChanged()) {
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
    renderFrame->Flush();
    RS_TRACE_END();
    if (Drawing::PerformanceCaculate::GetDrawingFlushPrint()) {
        RS_LOGI("Drawing Performance Flush end %{public}lld", Drawing::PerformanceCaculate::GetUpTime(false));
        Drawing::PerformanceCaculate::ResetCaculateTimeCount();
    }

    // process round corner display
    auto rcdInfo = std::make_unique<RcdInfo>();
    DoScreenRcdTask(params->GetId(), processor, rcdInfo, screenInfo);

    if (!RSMainThread::Instance()->WaitHardwareThreadTaskExecute()) {
        RS_LOGW("RSDisplayRenderNodeDrawable::ondraw: hardwareThread task has too many to Execute");
    }

    RS_TRACE_BEGIN("RSDisplayRenderNodeDrawable CommitLayer");
    for (const auto& drawable : hardwareDrawables) {
        if (UNLIKELY(!drawable || !drawable->GetRenderParams())) {
            continue;
        }
        auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(drawable);
        if (drawable->GetRenderParams()->GetHardwareEnabled()) {
            processor->CreateLayerForRenderThread(*surfaceDrawable);
        }
    }
    HardCursorCreateLayer(processor);
    SetDirtyRects(damageRegionrects);
    processor->ProcessDisplaySurfaceForRenderThread(*this);
    RSUifirstManager::Instance().CreateUIFirstLayer(processor);
    processor->PostProcess();
    RS_TRACE_END();

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    if (!mirrorDrawable) {
        RSMagicPointerRenderManager::GetInstance().ProcessColorPicker(processor, curCanvas_->GetGPUContext());
        RSMagicPointerRenderManager::GetInstance().SetCacheImgForPointer(nullptr);
    }
#endif
}

void RSDisplayRenderNodeDrawable::DrawMirrorScreen(
    RSDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor)
{
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
    if (mirroredParams->GetSecurityDisplay() != params.GetSecurityDisplay() &&
        specialLayerType_ == HAS_SPECIAL_LAYER) {
        DrawMirror(params, virtualProcesser,
            &RSDisplayRenderNodeDrawable::OnCapture, *uniParam);
    } else {
        DrawMirrorCopy(*mirroredDrawable, params, virtualProcesser, *uniParam);
    }
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
    auto hasGeneralSpecialLayer = (params.HasSecurityLayer() && isSecLayerInVisibleRect) || params.HasSkipLayer() ||
        params.HasProtectedLayer() || uniRenderThread.IsCurtainScreenOn() || params.GetHDRPresent() ||
        uniRenderThread.IsColorFilterModeOn();
    RS_LOGD("RSDisplayRenderNodeDrawable::GetSpecialLayerType, SecurityLayer:%{public}d, SkipLayer:%{public}d,"
        "ProtectedLayer:%{public}d, CurtainScreen:%{public}d, HDRPresent:%{public}d, ColorFilter:%{public}d",
        params.HasSecurityLayer(), params.HasSkipLayer(), params.HasProtectedLayer(),
        uniRenderThread.IsCurtainScreenOn(), params.GetHDRPresent(), uniRenderThread.IsColorFilterModeOn());
    if (RSUniRenderThread::GetCaptureParam().isSnapshot_) {
        hasGeneralSpecialLayer |= params.HasSnapshotSkipLayer();
        return hasGeneralSpecialLayer ? HAS_SPECIAL_LAYER :
            (params.HasCaptureWindow() ? CAPTURE_WINDOW : NO_SPECIAL_LAYER);
    }
    if (hasGeneralSpecialLayer || !uniRenderThread.GetWhiteList().empty() || !currentBlackList_.empty()) {
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
        return mappedDamageRegionRects;
    }
    ScreenInfo mainScreenInfo = screenManager->QueryScreenInfo(mirrorParams->GetScreenId());
    ScreenInfo curScreenInfo = screenManager->QueryScreenInfo(params.GetScreenId());
    if (curScreenInfo.skipFrameInterval > 1) {
        return mappedDamageRegionRects;
    }

    int32_t bufferAge = virtualProcesser->GetBufferAge();
    int32_t actualAge = curScreenInfo.skipFrameInterval ?
        static_cast<int32_t>(curScreenInfo.skipFrameInterval) * bufferAge : bufferAge;
    std::vector<RectI> damageRegionRects = MergeDirtyHistoryInVirtual(*mirroredDrawable, actualAge, mainScreenInfo);
    std::shared_ptr<RSObjAbsGeometry> tmpGeo = std::make_shared<RSObjAbsGeometry>();
    for (auto& rect : damageRegionRects) {
        RectI mappedRect = tmpGeo->MapRect(rect.ConvertTo<float>(), canvasMatrix);
        mappedDamageRegionRects.emplace_back(mappedRect);
    }
    if (!(lastMatrix_ == canvasMatrix) || !(lastMirrorMatrix_ == mirrorParams->GetMatrix()) ||
        uniParam->GetForceMirrorScreenDirty() || lastBlackList_ != currentBlackList_ ||
        mirrorParams->IsSpecialLayerChanged() || lastSecExemption_ != curSecExemption_ || virtualDirtyRefresh_ ||
        (enableVisibleRect_ && (lastVisibleRect_ != curVisibleRect_ || params.HasSecLayerInVisibleRectChanged()))) {
        GetSyncDirtyManager()->ResetDirtyAsSurfaceSize();
        virtualDirtyRefresh_ = false;
        lastMatrix_ = canvasMatrix;
        lastMirrorMatrix_ = mirrorParams->GetMatrix();
    }
    RectI hwcRect = mirroredDrawable->GetSyncDirtyManager()->GetHwcDirtyRegion();
    if (!hwcRect.IsEmpty()) {
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
    }
    return mappedDamageRegionRects;
}

void RSDisplayRenderNodeDrawable::DrawMirror(RSDisplayRenderParams& params,
    std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser, DrawFuncPtr drawFunc, RSRenderThreadParams& uniParam)
{
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
    curCanvas_->SetCapture(true);
    curCanvas_->SetDisableFilterCache(true);
    auto mirroedDisplayParams = static_cast<RSDisplayRenderParams*>(mirroredParams.get());
    auto hasSecSurface = mirroedDisplayParams->GetDisplayHasSecSurface();
    if (hasSecSurface[mirroredParams->GetScreenId()] && !uniParam.GetSecExemption()) {
        std::vector<RectI> emptyRects = {};
        virtualProcesser->SetRoiRegionToCodec(emptyRects);
        SetCanvasBlack(*virtualProcesser);
        virtualDirtyRefresh_ = true;
        curCanvas_->RestoreToCount(0);
        return;
    }
    curCanvas_->Save();
    virtualProcesser->ScaleMirrorIfNeed(GetOriginScreenRotation(), *curCanvas_);

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
    virtualProcesser->CanvasClipRegionForUniscaleMode();
    curCanvas_->ConcatMatrix(mirroredParams->GetMatrix());

    // set mirror screen capture param
    // Don't need to scale here since the canvas has been switched from mirror frame to offscreen
    // surface in PrepareOffscreenRender() above. The offscreen surface has the same size as
    // the main display that's why no need additional scale.
    RSUniRenderThread::SetCaptureParam(CaptureParam(false, false, true, 1.0f, 1.0f));
    RSRenderParams::SetParentSurfaceMatrix(curCanvas_->GetTotalMatrix());
    bool isOpDropped = uniParam.IsOpDropped();
    uniParam.SetOpDropped(false); // disable partial render
    (mirroredDrawable.get()->*drawFunc)(*curCanvas_);
    uniParam.SetOpDropped(isOpDropped);
    RSUniRenderThread::ResetCaptureParam();
    // Restore the initial state of the canvas to avoid state accumulation
    curCanvas_->RestoreToCount(0);
    rsDirtyRectsDfx.OnDrawVirtual(*curCanvas_);
    RSUniRenderThread::Instance().SetBlackList({});
    RSUniRenderThread::Instance().SetWhiteList({});
    uniParam.SetSecExemption(false);
}

void RSDisplayRenderNodeDrawable::DrawMirrorCopy(
    RSDisplayRenderNodeDrawable& mirrorDrawable, RSDisplayRenderParams& params,
    std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser, RSRenderThreadParams& uniParam)
{
    auto cacheImage = mirrorDrawable.GetCacheImgForCapture();
    bool isOpDropped = uniParam.IsOpDropped();
    uniParam.SetOpDropped(false);
    mirrorDrawable.SetOriginScreenRotation(GetOriginScreenRotation());
    virtualProcesser->CalculateTransform(mirrorDrawable);
    RSDirtyRectsDfx rsDirtyRectsDfx(*this);
    if (!uniParam.IsVirtualDirtyEnabled() || (enableVisibleRect_ && curVisibleRect_.GetTop() > 0)) {
        std::vector<RectI> emptyRects = {};
        virtualProcesser->SetRoiRegionToCodec(emptyRects);
    } else {
        auto dirtyRects = CalculateVirtualDirty(
            virtualProcesser, params, virtualProcesser->GetCanvasMatrix());
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
    RSUniRenderThread::SetCaptureParam(CaptureParam(false, false, true, 1.0f, 1.0f));
    mirrorDrawable.DrawHardwareEnabledNodesMissedInCacheImage(*curCanvas_);
    if (cacheImage && RSSystemProperties::GetDrawMirrorCacheImageEnabled()) {
        RS_TRACE_NAME("DrawMirrorCopy with cacheImage");
        if (!enableVisibleRect_) {
            RSUniRenderUtil::ProcessCacheImage(*curCanvas_, *cacheImage);
        } else {
            RSUniRenderUtil::ProcessCacheImageRect(*curCanvas_, *cacheImage, curVisibleRect_,
                Drawing::Rect(0, 0, curVisibleRect_.GetWidth(), curVisibleRect_.GetHeight()));
        }
    } else {
        RS_TRACE_NAME("DrawMirrorCopy with displaySurface");
        virtualProcesser->ProcessDisplaySurfaceForRenderThread(mirrorDrawable);
    }
    mirrorDrawable.DrawHardwareEnabledTopNodesMissedInCacheImage(*curCanvas_);
    RSUniRenderThread::ResetCaptureParam();
    uniParam.SetOpDropped(isOpDropped);
    // Restore the initial state of the canvas to avoid state accumulation
    curCanvas_->RestoreToCount(0);
    rsDirtyRectsDfx.OnDrawVirtual(*curCanvas_);
}

void RSDisplayRenderNodeDrawable::DrawExpandScreen(RSUniRenderVirtualProcessor& processor)
{
    curCanvas_ = processor.GetCanvas();
    if (curCanvas_ == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawExpandScreen failed to get canvas.");
        return;
    }
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    // set expand screen capture param(isSnapshot, isSingleSurface, isMirror)
    RSUniRenderThread::SetCaptureParam(CaptureParam(false, false, false, scaleX, scaleY));
    RSRenderNodeDrawable::OnCapture(*curCanvas_);
    RSUniRenderThread::ResetCaptureParam();
    // Restore the initial state of the canvas to avoid state accumulation
    curCanvas_->RestoreToCount(0);
}

void RSDisplayRenderNodeDrawable::WiredScreenProjection(
    RSDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor)
{
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
    curCanvas_->Save();
    ScaleAndRotateMirrorForWiredScreen(*mirroredDrawable);
    RSDirtyRectsDfx rsDirtyRectsDfx(*mirroredDrawable);
    std::vector<RectI> damageRegionRects =
        CalculateVirtualDirtyForWiredScreen(renderFrame, params, curCanvas_->GetTotalMatrix());
    rsDirtyRectsDfx.SetVirtualDirtyRects(damageRegionRects, params.GetScreenInfo());
    DrawWiredMirrorCopy(*mirroredDrawable);
    RSUniRenderThread::SetCaptureParam(CaptureParam(false, false, true, 1.0f, 1.0f));
    mirroredDrawable->DrawHardCursorNodesMissedInCacheImage(*curCanvas_);
    RSUniRenderThread::ResetCaptureParam();
    curCanvas_->Restore();
    rsDirtyRectsDfx.OnDrawVirtual(*curCanvas_);
    renderFrame->Flush();
    processor->ProcessDisplaySurfaceForRenderThread(*this);
    processor->PostProcess();
}

void RSDisplayRenderNodeDrawable::DrawWiredMirrorCopy(RSDisplayRenderNodeDrawable& mirroredDrawable)
{
    auto cacheImage = mirroredDrawable.GetCacheImgForCapture();
    if (cacheImage && RSSystemProperties::GetDrawMirrorCacheImageEnabled()) {
        RS_TRACE_NAME("DrawWiredMirrorCopy with cacheImage");
        if (!enableVisibleRect_) {
            RSUniRenderUtil::ProcessCacheImage(*curCanvas_, *cacheImage);
        } else {
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
    }
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
    const auto& mirroredParams = mirroredDrawable->GetRenderParams();
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
    int32_t bufferAge = renderFrame->GetBufferAge();
    auto curScreenInfo = params.GetScreenInfo();
    int32_t actualAge = curScreenInfo.skipFrameInterval ?
        static_cast<int32_t>(curScreenInfo.skipFrameInterval) * bufferAge : bufferAge;
    ScreenInfo mainScreenInfo = screenManager->QueryScreenInfo(mirroredParams->GetScreenId());
    std::shared_ptr<RSObjAbsGeometry> tmpGeo = std::make_shared<RSObjAbsGeometry>();
    // merge history dirty and map to mirrored wired screen by matrix
    auto tempDamageRegionRects = MergeDirtyHistoryInVirtual(*mirroredDrawable, actualAge, mainScreenInfo);
    for (auto& rect : tempDamageRegionRects) {
        RectI mappedRect = tmpGeo->MapRect(rect.ConvertTo<float>(), canvasMatrix);
        damageRegionRects.emplace_back(mappedRect);
    }

    auto syncDirtyManager = GetSyncDirtyManager();
    // reset dirty rect as mirrored wired screen size when first time connection or matrix changed
    if (!(lastMatrix_ == canvasMatrix) || !(lastMirrorMatrix_ == mirroredParams->GetMatrix()) ||
        uniParam->GetForceMirrorScreenDirty() || (enableVisibleRect_ && lastVisibleRect_ != curVisibleRect_)) {
        GetSyncDirtyManager()->ResetDirtyAsSurfaceSize();
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
    if (screenManager) {
        auto screenCorrection = screenManager->GetScreenCorrection(mirroredParams->GetScreenId());
        if (screenCorrection != ScreenRotation::INVALID_SCREEN_ROTATION &&
            screenCorrection != ScreenRotation::ROTATION_0) {
            // Recaculate rotation if mirrored screen has additional rotation angle
            rotation = static_cast<ScreenRotation>((static_cast<int>(rotation) + SCREEN_ROTATION_NUM
                - static_cast<int>(screenCorrection)) % SCREEN_ROTATION_NUM);
        }
    }
    // not support rotation for MirrorScreen enableVisibleRect
    rotation = enableVisibleRect_ ? ScreenRotation::ROTATION_0 : rotation;
    if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
        std::swap(mainWidth, mainHeight);
    }
    curCanvas_->Clear(SK_ColorBLACK);
    // Scale
    if (mainWidth > 0 && mainHeight > 0) {
        auto scaleNum = std::min(mirrorWidth / mainWidth, mirrorHeight / mainHeight);
        // 2 for calc X and Y
        curCanvas_->Translate((mirrorWidth - (scaleNum * mainWidth)) / 2, (mirrorHeight - (scaleNum * mainHeight)) / 2);
        curCanvas_->Scale(scaleNum, scaleNum);
    }
    // Rotate
    RotateMirrorCanvas(rotation, static_cast<float>(mainScreenInfo.width), static_cast<float>(mainScreenInfo.height));
}

void RSDisplayRenderNodeDrawable::SetCanvasBlack(RSProcessor& processor)
{
    curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
    processor.PostProcess();
    RS_LOGI("RSDisplayRenderNodeDrawable::SetCanvasBlack, set canvas to black because of security layer.");
    curCanvas_->SetDisableFilterCache(false);
}

void RSDisplayRenderNodeDrawable::RotateMirrorCanvas(ScreenRotation& rotation, float mainWidth, float mainHeight)
{
    switch (rotation) {
        case ScreenRotation::ROTATION_0:
            break;
        case ScreenRotation::ROTATION_90:
            curCanvas_->Rotate(90, 0, 0); // 90 is the rotate angle
            curCanvas_->Translate(0, -mainHeight);
            break;
        case ScreenRotation::ROTATION_180:
            // 180 is the rotate angle, calculate half width and half height requires divide by 2
            curCanvas_->Rotate(180, mainWidth / 2, mainHeight / 2);
            break;
        case ScreenRotation::ROTATION_270:
            curCanvas_->Rotate(270, 0, 0); // 270 is the rotate angle
            curCanvas_->Translate(-mainWidth, 0);
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

    specialLayerType_ = GetSpecialLayerType(*params);
    if (specialLayerType_ != NO_SPECIAL_LAYER || UNLIKELY(RSUniRenderThread::GetCaptureParam().isMirror_)) {
        RS_LOGD("RSDisplayRenderNodeDrawable::OnCapture: \
            process RSDisplayRenderNode(id:[%{public}" PRIu64 "]) Not using UniRender buffer.",
            params->GetId());
        RS_TRACE_NAME("Process RSDisplayRenderNodeDrawable[" +
            std::to_string(params->GetScreenId()) + "] Not using UniRender buffer.");

        // Adding matrix affine transformation logic
        if (!UNLIKELY(RSUniRenderThread::GetCaptureParam().isMirror_)) {
            rscanvas->ConcatMatrix(params->GetMatrix());
        }

        RSRenderNodeDrawable::OnCapture(canvas);
        DrawWatermarkIfNeed(*params, *rscanvas);
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

    if (GetRSSurfaceHandlerOnDraw()->GetBuffer() == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes: buffer is null!");
        return;
    }

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
    if (!renderParams_) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes params is null!");
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
    auto& hardCursorDrawable = RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardCursorDrawables();
    if (!hardCursorDrawable.drawablePtr) {
        return;
    }
    auto surfaceParams =
        static_cast<RSSurfaceRenderParams*>(hardCursorDrawable.drawablePtr->GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("RSDisplayRenderNodeDrawable::FindHardCursorNodes surfaceParams is null");
        return;
    }
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(hardCursorDrawable.drawablePtr);
    if (surfaceDrawable && surfaceParams->GetHardCursorStatus()) {
        params.GetHardwareEnabledTopDrawables().emplace_back(surfaceDrawable);
    }
}

void RSDisplayRenderNodeDrawable::SwitchColorFilter(RSPaintFilterCanvas& canvas, float hdrBrightnessRatio) const
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

    Drawing::AutoCanvasRestore acr(*curCanvas_, true);
    RS_TRACE_NAME_FMT("RSDisplayRenderNodeDrawable::SetColorFilterModeToPaint mode:%d",
        static_cast<int32_t>(colorFilterMode));
    Drawing::Brush brush;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, brush, hdrBrightnessRatio);
#if defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)
    RSTagTracker tagTracker(
        renderEngine->GetRenderContext()->GetDrGPUContext(),
        RSTagTracker::TAG_SAVELAYER_COLOR_FILTER);
#endif
    Drawing::SaveLayerOps slr(nullptr, &brush, Drawing::SaveLayerOps::INIT_WITH_PREVIOUS);
    canvas.SaveLayer(slr);
}

void RSDisplayRenderNodeDrawable::FindHardwareEnabledNodes(RSDisplayRenderParams& params)
{
    params.GetHardwareEnabledTopDrawables().clear();
    params.GetHardwareEnabledDrawables().clear();
    auto& hardwareDrawables =
        RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeDrawables();
    for (const auto& drawable : hardwareDrawables) {
        auto surfaceNodeDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(drawable);
        if (!surfaceNodeDrawable || !surfaceNodeDrawable->ShouldPaint()) {
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
    auto& hardCursorDrawable = RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardCursorDrawables();
    if (hardCursorDrawable.id != GetId() || !hardCursorDrawable.drawablePtr) {
        return;
    }
    auto surfaceParams =
        static_cast<RSSurfaceRenderParams*>(hardCursorDrawable.drawablePtr->GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("RSDisplayRenderNodeDrawable::FindHardwareEnabledNodes surfaceParams is null");
        return;
    }
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(hardCursorDrawable.drawablePtr);
    if (surfaceDrawable && surfaceParams->GetHardCursorStatus()) {
        params.GetHardwareEnabledTopDrawables().emplace_back(surfaceDrawable);
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

void RSDisplayRenderNodeDrawable::ClearTransparentBeforeSaveLayer()
{
    if (!canvasBackup_) {
        return;
    }
    RS_TRACE_NAME("ClearTransparentBeforeSaveLayer");
    auto& hardwareDrawables =
        RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeDrawables();
    for (const auto& drawable : hardwareDrawables) {
        auto surfaceDrawable = static_cast<RSSurfaceRenderNodeDrawable*>(drawable.get());
        if (!surfaceDrawable) {
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
    return RSSystemParameters::IsNeedScRGBForP3(currentGamut) && RSMainThread::Instance()->IsUIFirstOn();
}

void RSDisplayRenderNodeDrawable::PrepareOffscreenRender(const RSDisplayRenderNodeDrawable& displayDrawable,
    bool useFixedSize)
{
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
    if (useFixedSize && (RSSystemProperties::IsFoldScreenFlag() || RSSystemProperties::IsTabletType())
        && params->IsRotationChanged()) {
        useFixedOffscreenSurfaceSize_ = true;
        int32_t maxRenderSize =
            static_cast<int32_t>(std::max(params->GetScreenInfo().width, params->GetScreenInfo().height));
        offscreenWidth = maxRenderSize;
        offscreenHeight = maxRenderSize;
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
    if (!params->GetNeedOffscreen() || !useFixedOffscreenSurfaceSize_ || offscreenSurface_ == nullptr) {
        RS_TRACE_NAME_FMT("make offscreen surface with fixed size: [%d, %d]", offscreenWidth, offscreenHeight);
        bool isScRGBEnable = EnablescRGBForP3AndUiFirst(params->GetNewColorSpace());
        if (!params->GetNeedOffscreen() && (params->GetHDRPresent() || isScRGBEnable)) {
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

    // copy HDR properties into offscreen canvas
    offscreenCanvas->CopyHDRConfiguration(*curCanvas_);
    // copy current canvas properties into offscreen canvas
    offscreenCanvas->CopyConfigurationToOffscreenCanvas(*curCanvas_);

    // backup current canvas and replace with offscreen canvas
    canvasBackup_ = std::exchange(curCanvas_, offscreenCanvas);
}

void RSDisplayRenderNodeDrawable::FinishOffscreenRender(const Drawing::SamplingOptions& sampling,
    float hdrBrightnessRatio)
{
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
    if (ROSEN_LNE(hdrBrightnessRatio, 1.0f)) {
        FinishHdrDraw(paint, hdrBrightnessRatio);
    }
    paint.SetAntiAlias(true);
    canvasBackup_->AttachBrush(paint);
    canvasBackup_->DrawImage(*image, 0, 0, sampling);
    canvasBackup_->DetachBrush();
    // restore current canvas and cleanup
    if (!useFixedOffscreenSurfaceSize_) {
        offscreenSurface_ = nullptr;
    }
    curCanvas_ = std::move(canvasBackup_);
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

bool RSDisplayRenderNodeDrawable::SkipFrame(uint32_t refreshRate, uint32_t skipFrameInterval)
{
    if (refreshRate == 0 || skipFrameInterval <= 1) {
        return false;
    }
    int64_t currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    // when refreshRate % skipFrameInterval != 0 means the skipFrameInterval is the virtual screen refresh rate
    if (refreshRate % skipFrameInterval != 0) {
        int64_t minFrameInterval = 1000000000LL / skipFrameInterval;
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
    // the skipFrameInterval is equal to 60 divide the virtual screen refresh rate
    int64_t refreshInterval = currentTime - lastRefreshTime_;
    // 1000000000ns == 1s, 110/100 allows 10% over.
    bool needSkip = refreshInterval < (1000000000LL / refreshRate) * (skipFrameInterval - 1) * 110 / 100;
    if (!needSkip) {
        lastRefreshTime_ = currentTime;
    }
    return needSkip;
}

} // namespace OHOS::Rosen::DrawableV2
