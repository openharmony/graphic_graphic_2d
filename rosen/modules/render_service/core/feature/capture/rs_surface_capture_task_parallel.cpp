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

#include "feature/capture/rs_surface_capture_task_parallel.h"

#include <memory>
#include <sys/mman.h>

#include "draw/surface.h"
#include "draw/color.h"
#include "rs_trace.h"

#include "common/rs_background_thread.h"
#include "common/rs_obj_abs_geometry.h"
#include "feature/hdr/rs_hdr_util.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "feature_cfg/graphic_feature_param_manager.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_pointer_window_manager.h"
#include "pipeline/main_thread/rs_render_service_connection.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/drawing/rs_surface.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_skia_filter.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/rs_screen_mode_info.h"
#include "pipeline/rs_logical_display_render_node.h"

namespace OHOS {
namespace Rosen {
namespace {
#ifdef RS_ENABLE_GPU
inline void DrawCapturedImg(Drawing::Image& image,
    Drawing::Surface& surface, const Drawing::BackendTexture& backendTexture,
    Drawing::TextureOrigin& textureOrigin, Drawing::BitmapFormat& bitmapFormat)
{
    RSPaintFilterCanvas canvas(&surface);
    auto gpuContext = canvas.GetGPUContext();
    if (gpuContext == nullptr) {
        RS_LOGE("DrawCapturedImg failed: gpuContext is nullptr");
        return;
    }
    auto colorSpace = surface.GetImageInfo().GetColorSpace();
    image.BuildFromTexture(*gpuContext, backendTexture.GetTextureInfo(),
        textureOrigin, bitmapFormat, colorSpace);
    canvas.DrawImage(image, 0.f, 0.f, Drawing::SamplingOptions());
    surface.FlushAndSubmit(true);
}
#endif

uint32_t PixelMapSamplingDump(std::unique_ptr<Media::PixelMap>& pixelmap, int32_t x, int32_t y)
{
    if (pixelmap == nullptr) {
        RS_LOGE("PixelMapSamplingDump fail, pixelmap is nullptr");
        return 0;
    }
    if (x < 0 || x >= pixelmap->GetWidth() || y < 0 || y >= pixelmap->GetHeight()) {
        RS_LOGE("PixelMapSamplingDump fail, x or y is invalid");
        return 0;
    }
    uint32_t pixel = 0;
    pixelmap->ReadPixel({x, y}, pixel);
    return pixel;
}
}

void RSSurfaceCaptureTaskParallel::CheckModifiers(NodeId id, bool useCurWindow)
{
    RS_TRACE_NAME("RSSurfaceCaptureTaskParallel::CheckModifiers");
    bool needSync = RSMainThread::Instance()->IsOcclusionNodesNeedSync(id, useCurWindow) ||
        RSPointerWindowManager::Instance().GetBoundHasUpdate() ||
        RSMainThread::Instance()->IsHardwareEnabledNodesNeedSync();
    if (!needSync) {
        return;
    }
    std::function<void()> syncTask = []() -> void {
        RS_TRACE_NAME("RSSurfaceCaptureTaskParallel::SyncModifiers");
        RS_LOGI("RSSurfaceCaptureTaskParallel::SyncModifiers");
        RSPointerWindowManager::Instance().UpdatePointerInfo();
        auto& pendingSyncNodes = RSMainThread::Instance()->GetContext().pendingSyncNodes_;
        int skipTimes = 0;
        for (auto& [id, weakPtr] : pendingSyncNodes) {
            auto node = weakPtr.lock();
            if (node == nullptr) {
                continue;
            }
            if (!RSUifirstManager::Instance().CollectSkipSyncNode(node)) {
                node->Sync();
            } else {
                node->SkipSync();
                skipTimes++;
            }
        }
        if (skipTimes != 0) {
            RS_LOGW("RSSurfaceCaptureTaskParallel::CheckModifiers SkipSync times: [%{public}d]", skipTimes);
        }
        pendingSyncNodes.clear();
        RSUifirstManager::Instance().UifirstCurStateClear();
    };
    RSUniRenderThread::Instance().PostSyncTask(syncTask);
}

void RSSurfaceCaptureTaskParallel::Capture(
    sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureParam& captureParam)
{
    if (callback == nullptr) {
        RS_LOGE(
            "RSSurfaceCaptureTaskParallel::Capture nodeId:[%{public}" PRIu64 "], callback is nullptr", captureParam.id);
        return;
    }

    std::shared_ptr<RSSurfaceCaptureTaskParallel> captureHandle =
        std::make_shared<RSSurfaceCaptureTaskParallel>(captureParam.id, captureParam.config);
    if (!captureHandle->CreateResources()) {
        callback->OnSurfaceCapture(captureParam.id, captureParam.config, nullptr);
        return;
    }

    // When the HDR PixelMap is created, it indicates that an HDR screenshot is required.
    if (captureHandle->UseScreenShotWithHDR()) {
        std::function<void()> captureTask = [captureHandle, callback, captureParam]() -> void {
            RS_TRACE_NAME("RSSurfaceCaptureTaskParallel::TakeSurfaceCaptureHDR");
            if (!captureHandle->RunHDR(callback, captureParam)) {
                callback->OnSurfaceCapture(captureParam.id, captureParam.config, nullptr, nullptr);
            }
        };
        RSUniRenderThread::Instance().PostSyncTask(captureTask);
        return;
    }

    std::function<void()> captureTask = [captureHandle, callback, captureParam]() -> void {
        RS_TRACE_NAME("RSSurfaceCaptureTaskParallel::TakeSurfaceCapture");
        if (!captureHandle->Run(callback, captureParam)) {
            callback->OnSurfaceCapture(captureParam.id, captureParam.config, nullptr);
        }
    };
    RSUniRenderThread::Instance().PostSyncTask(captureTask);
}

void RSSurfaceCaptureTaskParallel::ClearCacheImageByFreeze(NodeId id)
{
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(id);
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::CreateResources: Invalid nodeId:[%{public}" PRIu64 "]", id);
        return;
    }
    if (auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>()) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        std::function<void()> clearCacheTask = [id, surfaceNodeDrawable]() -> void {
            surfaceNodeDrawable->SetCacheImageByCapture(nullptr);
        };
        RSUniRenderThread::Instance().PostTask(clearCacheTask);
    }
}


bool RSSurfaceCaptureTaskParallel::CreateResources()
{
    RS_LOGD("RSSurfaceCaptureTaskParallel capture nodeId:[%{public}" PRIu64 "] scaleX:%{public}f"
        " scaleY:%{public}f useCurWindow:%{public}d", nodeId_, captureConfig_.scaleX,
        captureConfig_.scaleY,  captureConfig_.useCurWindow);
    if (ROSEN_EQ(captureConfig_.scaleX, 0.f) || ROSEN_EQ(captureConfig_.scaleY, 0.f) ||
        captureConfig_.scaleX < 0.f || captureConfig_.scaleY < 0.f ||
        captureConfig_.scaleX > 1.f || captureConfig_.scaleY > 1.f) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::CreateResources: SurfaceCapture scale is invalid.");
        return false;
    }
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(nodeId_);
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::CreateResources: Invalid nodeId:[%{public}" PRIu64 "]",
            nodeId_);
        return false;
    }

    if (auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>()) {
        surfaceNode_ = surfaceNode;
        auto curNode = surfaceNode;
        if (!captureConfig_.useCurWindow) {
            auto parentNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(surfaceNode->GetParent().lock());
            if (parentNode && parentNode->IsLeashWindow() && parentNode->ShouldPaint()) {
                curNode = parentNode;
            }
        }
        if (!curNode->ShouldPaint()) {
            RS_LOGW("RSSurfaceCaptureTaskParallel::CreateResources: curNode should not paint!");
            return false;
        }
        if (curNode->GetSortedChildren()->size() == 0) {
            RS_LOGW("RSSurfaceCaptureTaskParallel::CreateResources: curNode has no childrenList!");
        }
        surfaceNodeDrawable_ = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(curNode));
        // When the window freeze capture invokes the F16 screenshot mode and window need use F16 capture,
        // or when the HDR mode is invoked and the window contains HDR resources, an F16 buffer needs to be allocated.
        bool isF16Capture = (captureConfig_.needF16WindowCaptureForScRGB && RSHdrUtil::NeedUseF16Capture(curNode)) ||
            (captureConfig_.isHdrCapture && surfaceNode->GetHDRPresent());
        pixelMap_ = CreatePixelMapBySurfaceNode(curNode, isF16Capture);
        screenId_ = surfaceNode->GetScreenId();
    } else if (auto displayNode = node->ReinterpretCastTo<RSLogicalDisplayRenderNode>()) {
        displayNodeDrawable_ = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(displayNode));
        pixelMap_ = CreatePixelMapByDisplayNode(displayNode);
        auto screenNode = std::static_pointer_cast<RSScreenRenderNode>(displayNode->GetParent().lock());
        // When the app calls HDR screenshot and the screen contains HDR content, two pixelmaps need to be captured.
        if (captureConfig_.isHdrCapture && screenNode && (screenNode->GetDisplayHdrStatus() != HdrStatus::NO_HDR)) {
            pixelMapHDR_ = CreatePixelMapByDisplayNode(displayNode, true);
            SetUseScreenShotWithHDR(true);
        }
    } else {
        RS_LOGE("RSSurfaceCaptureTaskParallel::CreateResources: Invalid RSRenderNodeType!");
        return false;
    }
    if (pixelMap_ == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::CreateResources: pixelMap_ is nullptr!");
        return false;
    }
    return true;
}

bool RSSurfaceCaptureTaskParallel::Run(
    sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureParam& captureParam)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    SetupGpuContext();
    std::string nodeName("RSSurfaceCaptureTaskParallel");
    RSTagTracker tagTracker(gpuContext_, nodeId_, RSTagTracker::TAGTYPE::TAG_CAPTURE, nodeName);
#endif
    auto surface = CreateSurface(pixelMap_);
    if (surface == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::Run: surface is nullptr!");
        return false;
    }

    RSPaintFilterCanvas canvas(surface.get());
    canvas.Scale(captureConfig_.scaleX, captureConfig_.scaleY);
    const Drawing::Rect& rect = captureConfig_.mainScreenRect;
    if (rect.GetWidth() > 0 && rect.GetHeight() > 0) {
        canvas.ClipRect({0, 0, rect.GetWidth(), rect.GetHeight()});
        canvas.Translate(-rect.GetLeft(), -rect.GetTop());
    }
    canvas.Translate(-boundsX_, -boundsY_);
    canvas.SetDisableFilterCache(true);
    RSSurfaceRenderParams* curNodeParams = nullptr;
    if (surfaceNodeDrawable_) {
        curNodeParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable_->GetRenderParams().get());
        RSUiFirstProcessStateCheckerHelper stateCheckerHelper(
            curNodeParams->GetFirstLevelNodeId(), curNodeParams->GetUifirstRootNodeId());
        RSUniRenderThread::SetCaptureParam(CaptureParam(true, true, false, true, captureParam.isSystemCalling,
            captureParam.isSelfCapture, captureParam.blurParam.isNeedBlur));
        bool isHDRCapture = captureConfig_.isHdrCapture && curNodeParams->GetHDRPresent();
        RSPaintFilterCanvas::ScreenshotType type = isHDRCapture ? RSPaintFilterCanvas::ScreenshotType::HDR_WINDOWSHOT :
            RSPaintFilterCanvas::ScreenshotType::SDR_WINDOWSHOT;
        canvas.SetScreenshotType(type);
        canvas.SetIsWindowFreezeCapture(captureParam.isFreeze);
        canvas.Clear(captureParam.config.backGroundColor);
        surfaceNodeDrawable_->OnCapture(canvas);
        RS_LOGI("RSSurfaceCaptureTaskParallel::Run: the number of total processedNodes: %{public}d",
            DrawableV2::RSRenderNodeDrawable::GetSnapshotProcessedNodeCount());
        DrawableV2::RSRenderNodeDrawable::ClearSnapshotProcessedNodeCount();
        if (captureParam.isFreeze) {
            surfaceNodeDrawable_->SetCacheImageByCapture(surface->GetImageSnapshot());
        }
        if (captureParam.blurParam.isNeedBlur) {
            AddBlur(canvas, surface, captureParam.blurParam.blurRadius);
        }
    } else if (displayNodeDrawable_) {
        RSUniRenderThread::SetCaptureParam(CaptureParam(true, false, false));
        canvas.SetScreenshotType(RSPaintFilterCanvas::ScreenshotType::SDR_SCREENSHOT);
        // Screenshot blacklist, exclude surfaceNode in blacklist while capturing displaynode
        std::unordered_set<NodeId> blackList(captureConfig_.blackList.begin(), captureConfig_.blackList.end());
        RSUniRenderThread::Instance().SetBlackList(blackList);
        displayNodeDrawable_->OnCapture(canvas);
        RSUniRenderThread::Instance().SetBlackList({});
    } else {
        RS_LOGE("RSSurfaceCaptureTaskParallel::Run: Invalid RSRenderNodeDrawable!");
        return false;
    }
    RSUniRenderThread::ResetCaptureParam();

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)) && \
    (defined(RS_ENABLE_EGLIMAGE) && defined(RS_ENABLE_UNI_RENDER))
    RSUniRenderUtil::OptimizedFlushAndSubmit(surface, gpuContext_.get(), GetFeatureParamValue("CaptureConfig",
        &CaptureBaseParam::IsSnapshotWithDMAEnabled).value_or(false));
    if (curNodeParams && curNodeParams->IsNodeToBeCaptured()) {
        RSUifirstManager::Instance().AddCapturedNodes(curNodeParams->GetId());
    }
    bool snapshotDmaEnabled = system::GetBoolParameter("rosen.snapshotDma.enabled", true);
    bool isEnableFeature = GetFeatureParamValue("CaptureConfig",
        &CaptureBaseParam::IsSnapshotWithDMAEnabled).value_or(false);
    if (snapshotDmaEnabled && isEnableFeature) {
        auto copytask = CreateSurfaceSyncCopyTask(surface, std::move(pixelMap_),
            nodeId_, captureConfig_, callback, finalRotationAngle_);
        if (!copytask) {
            RS_LOGE("RSSurfaceCaptureTaskParallel::Run: create capture task failed!");
            return false;
        }
        RSBackgroundThread::Instance().PostTask(copytask);
        return true;
    } else {
        std::shared_ptr<Drawing::Image> img(surface.get()->GetImageSnapshot());
        if (!img) {
            RS_LOGE("RSSurfaceCaptureTaskParallel::Run: img is nullptr");
            return false;
        }
        if (!CopyDataToPixelMap(img, pixelMap_)) {
            RS_LOGE("RSSurfaceCaptureTaskParallel::Run: CopyDataToPixelMap failed");
            return false;
        }
    }
#endif
    if (finalRotationAngle_) {
        pixelMap_->rotate(finalRotationAngle_);
    }
    // To get dump image
    // execute "param set rosen.dumpsurfacetype.enabled 3 && setenforce 0"
    RSBaseRenderUtil::WritePixelMapToPng(*pixelMap_);
    RS_LOGD("RSSurfaceCaptureTaskParallel::Run CaptureTask make a pixleMap with colorSpaceName: %{public}d",
        pixelMap_->InnerGetGrColorSpace().GetColorSpaceName());
    callback->OnSurfaceCapture(nodeId_, captureConfig_, pixelMap_.get());
    return true;
}

bool RSSurfaceCaptureTaskParallel::DrawHDRSurfaceContent(std::shared_ptr<Drawing::Surface> surface, bool isOnHDR)
{
    if (surface == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::DrawHDRSurfaceContent surface is nullptr!");
        return false;
    }
    RSPaintFilterCanvas canvas(surface.get());
    canvas.Scale(captureConfig_.scaleX, captureConfig_.scaleY);
    const Drawing::Rect& rect = captureConfig_.mainScreenRect;
    if (rect.GetWidth() > 0 && rect.GetHeight() > 0) {
        canvas.ClipRect({0, 0, rect.GetWidth(), rect.GetHeight()});
        canvas.Translate(0 - rect.GetLeft(), 0 - rect.GetTop());
    }
    canvas.SetDisableFilterCache(true);
    RSSurfaceRenderParams* curNodeParams = nullptr;
    if (displayNodeDrawable_) {
        RSPaintFilterCanvas::ScreenshotType type = isOnHDR ? RSPaintFilterCanvas::ScreenshotType::HDR_SCREENSHOT :
            RSPaintFilterCanvas::ScreenshotType::SDR_SCREENSHOT;
        canvas.SetScreenshotType(type);
        RSUniRenderThread::SetCaptureParam(CaptureParam(true, false, false));
        // Screenshot blacklist, exclude surfaceNode in blacklist while capturing displaynode
        std::unordered_set<NodeId> blackList(captureConfig_.blackList.begin(), captureConfig_.blackList.end());
        RSUniRenderThread::Instance().SetBlackList(blackList);
        displayNodeDrawable_->OnCapture(canvas);
        RSUniRenderThread::Instance().SetBlackList({});
    } else {
        RS_LOGE("RSSurfaceCaptureTaskParallel::DrawHDRSurfaceContent: Invalid RSRenderNodeDrawable!");
        return false;
    }
    RSUniRenderThread::ResetCaptureParam();
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)) && \
    (defined(RS_ENABLE_EGLIMAGE) && defined(RS_ENABLE_UNI_RENDER))
    RSUniRenderUtil::OptimizedFlushAndSubmit(surface, gpuContext_.get(), GetFeatureParamValue("CaptureConfig",
        &CaptureBaseParam::IsSnapshotWithDMAEnabled).value_or(false));
#endif
    return true;
}

bool RSSurfaceCaptureTaskParallel::RunHDR(
    sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureParam& captureParam)
{
    if (!UseScreenShotWithHDR() || !callback) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::RunHDR not HDR screenshot or callback is nullptr.");
        return false;
    }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    SetupGpuContext();
    std::string nodeName("RSSurfaceCaptureTaskParallel");
    RSTagTracker tagTracker(gpuContext_, nodeId_, RSTagTracker::TAGTYPE::TAG_CAPTURE, nodeName);
#endif
    auto surface = CreateSurface(pixelMap_);
    auto surfaceHDR = CreateSurface(pixelMapHDR_);
    if (!DrawHDRSurfaceContent(surface, false) || !DrawHDRSurfaceContent(surfaceHDR, true)) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::RunHDR: DrawHDRSurfaceContent failed!");
        return false;
    }
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)) && \
    (defined(RS_ENABLE_EGLIMAGE) && defined(RS_ENABLE_UNI_RENDER))
    bool snapshotDmaEnabled = system::GetBoolParameter("rosen.snapshotDma.enabled", true);
    bool isEnableFeature = GetFeatureParamValue("CaptureConfig",
        &CaptureBaseParam::IsSnapshotWithDMAEnabled).value_or(false);
    if (snapshotDmaEnabled && isEnableFeature) {
        auto copytask = CreateSurfaceSyncCopyTaskWithDoublePixelMap(surface, std::move(pixelMap_),
            surfaceHDR, std::move(pixelMapHDR_), nodeId_, captureConfig_, callback, finalRotationAngle_);
        if (!copytask) {
            RS_LOGE("RSSurfaceCaptureTaskParallel::RunHDR: create capture task failed!");
            return false;
        }
        RSBackgroundThread::Instance().PostTask(copytask);
        return true;
    } else {
        std::shared_ptr<Drawing::Image> img(surface.get()->GetImageSnapshot());
        std::shared_ptr<Drawing::Image> imgHDR(surfaceHDR.get()->GetImageSnapshot());
        if (!img || !imgHDR) {
            RS_LOGE("RSSurfaceCaptureTaskParallel::RunHDR: img or imgHDR is nullptr");
            return false;
        }
        if (!CopyDataToPixelMap(img, pixelMap_) || !CopyDataToPixelMap(imgHDR, pixelMapHDR_)) {
            RS_LOGE("RSSurfaceCaptureTaskParallel::RunHDR: CopyDataToPixelMap failed");
            return false;
        }
    }
#endif
    if (finalRotationAngle_) {
        pixelMap_->rotate(finalRotationAngle_);
        pixelMapHDR_->rotate(finalRotationAngle_);
    }
    // To get dump image
    // execute "param set rosen.dumpsurfacetype.enabled 3 && setenforce 0"
    RSBaseRenderUtil::WritePixelMapToPng(*pixelMap_);
    RSBaseRenderUtil::WritePixelMapToPng(*pixelMapHDR_);
    RS_LOGD("RSSurfaceCaptureTaskParallel::Run CaptureTask make pixleMaps with colorSpaceName:"
        " %{public}d and %{public}d", pixelMap_->InnerGetGrColorSpace().GetColorSpaceName(),
        pixelMapHDR_->InnerGetGrColorSpace().GetColorSpaceName());
    callback->OnSurfaceCapture(nodeId_, captureConfig_, pixelMap_.get(), pixelMapHDR_.get());
    return true;
}

std::unique_ptr<Media::PixelMap> RSSurfaceCaptureTaskParallel::CreatePixelMapBySurfaceNode(
    std::shared_ptr<RSSurfaceRenderNode> node, bool isF16Capture)
{
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::CreatePixelMapBySurfaceNode: node == nullptr");
        return nullptr;
    }
    int pixmapWidth = node->GetRenderProperties().GetBoundsWidth();
    int pixmapHeight = node->GetRenderProperties().GetBoundsHeight();

    Media::InitializationOptions opts;
    if (isF16Capture) {
        opts.pixelFormat = Media::PixelFormat::RGBA_F16;
    }
    opts.size.width = ceil(pixmapWidth * captureConfig_.scaleX);
    opts.size.height = ceil(pixmapHeight * captureConfig_.scaleY);
    // Surface Node currently does not support regional screenshot
    captureConfig_.mainScreenRect = {};
    RS_LOGI("RSSurfaceCaptureTaskParallel::CreatePixelMapBySurfaceNode: NodeId:[%{public}" PRIu64 "],"
        " origin pixelmap size: [%{public}u, %{public}u],"
        " scale: [%{public}f, %{public}f],"
        " useDma: [%{public}d], useCurWindow: [%{public}d],"
        " isOnTheTree: [%{public}d], isVisible: [%{public}d], isF16Capture: [%{public}d]",
        node->GetId(), pixmapWidth, pixmapHeight, captureConfig_.scaleX, captureConfig_.scaleY,
        captureConfig_.useDma, captureConfig_.useCurWindow, node->IsOnTheTree(),
        !surfaceNode_->GetVisibleRegion().IsEmpty(), isF16Capture);
    std::unique_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opts);
    if (pixelMap) {
        GraphicColorGamut windowColorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
        if (!isF16Capture) {
            windowColorGamut = node->IsLeashWindow() ? node->GetFirstLevelNodeColorGamut() : node->GetColorSpace();
        }
        pixelMap->InnerSetColorSpace(windowColorGamut == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB ?
            OHOS::ColorManager::ColorSpace(OHOS::ColorManager::ColorSpaceName::SRGB) :
            OHOS::ColorManager::ColorSpace(OHOS::ColorManager::ColorSpaceName::DISPLAY_P3));
    }
    return pixelMap;
}

std::unique_ptr<Media::PixelMap> RSSurfaceCaptureTaskParallel::CreatePixelMapByDisplayNode(
    std::shared_ptr<RSLogicalDisplayRenderNode> node, bool isHDRCapture)
{
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::CreatePixelMapByDisplayNode: node is nullptr");
        return nullptr;
    }
    uint64_t screenId = node->GetScreenId();
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::CreatePixelMapByDisplayNode: screenManager is nullptr!");
        return nullptr;
    }

    screenCorrection_ = screenManager->GetScreenCorrection(screenId);
    screenRotation_ = node->GetScreenRotation();
    finalRotationAngle_ = CalPixelMapRotation();
    auto bounds = node->GetRenderProperties().GetBoundsGeometry();
    uint32_t pixmapWidth = static_cast<uint32_t>(bounds->GetWidth());
    uint32_t pixmapHeight = static_cast<uint32_t>(bounds->GetHeight());
    boundsX_ = bounds->GetX();
    boundsY_ = bounds->GetY();
    auto rotation = node->GetRotation();
    if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
        std::swap(pixmapWidth, pixmapHeight);
    }

    const Drawing::Rect& rect = captureConfig_.mainScreenRect;
    float rectWidth = rect.GetWidth();
    float rectHeight = rect.GetHeight();
    if (rectWidth > 0 && rectHeight > 0 && rectWidth <= pixmapWidth && rectHeight <= pixmapHeight) {
        pixmapWidth = floor(rectWidth);
        pixmapHeight = floor(rectHeight);
    }

    Media::InitializationOptions opts;
    if (isHDRCapture) {
        opts.pixelFormat = Media::PixelFormat::RGBA_F16;
    }
    opts.size.width = ceil(pixmapWidth * captureConfig_.scaleX);
    opts.size.height = ceil(pixmapHeight * captureConfig_.scaleY);
    RS_LOGI("RSSurfaceCaptureTaskParallel::%{public}s NodeId[%{public}" PRIu64 "],pixelmap[%{public}u, %{public}u],"
        " scale[%{public}f, %{public}f], rect[%{public}f, %{public}f, %{public}f, %{public}f], dma[%{public}d],"
        " rotation[%{public}d], correction[%{public}d], blackList[%{public}zu], isHDRCapture[%{public}d]", __func__,
        node->GetId(), pixmapWidth, pixmapHeight, captureConfig_.scaleX, captureConfig_.scaleY,
        rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight(), captureConfig_.useDma, screenRotation_,
        screenCorrection_, captureConfig_.blackList.size(), isHDRCapture);
    std::unique_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opts);
    auto screenNode = std::static_pointer_cast<RSScreenRenderNode>(node->GetParent().lock());
    if (pixelMap && screenNode) {
        GraphicColorGamut windowColorGamut = isHDRCapture ?
            GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB : screenNode->GetColorSpace();
        pixelMap->InnerSetColorSpace(windowColorGamut == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB ?
            OHOS::ColorManager::ColorSpace(OHOS::ColorManager::ColorSpaceName::SRGB) :
            OHOS::ColorManager::ColorSpace(OHOS::ColorManager::ColorSpaceName::DISPLAY_P3));
    }
    return pixelMap;
}

std::shared_ptr<Drawing::Surface> RSSurfaceCaptureTaskParallel::CreateSurface(
    const std::unique_ptr<Media::PixelMap>& pixelmap)
{
    if (pixelmap == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::CreateSurface: pixelmap == nullptr");
        return nullptr;
    }
    auto address = const_cast<uint8_t*>(pixelmap->GetPixels());
    if (address == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::CreateSurface: address == nullptr");
        return nullptr;
    }
    OHOS::ColorManager::ColorSpaceName colorSpaceName = pixelmap->InnerGetGrColorSpace().GetColorSpaceName();
    auto colorSpace = RSBaseRenderEngine::ConvertColorSpaceNameToDrawingColorSpace(colorSpaceName);
    auto colorType = pixelmap->GetPixelFormat() == Media::PixelFormat::RGBA_F16 ?
        Drawing::ColorType::COLORTYPE_RGBA_F16 :
        Drawing::ColorType::COLORTYPE_RGBA_8888;
    Drawing::ImageInfo info = Drawing::ImageInfo{pixelmap->GetWidth(), pixelmap->GetHeight(),
        colorType, Drawing::AlphaType::ALPHATYPE_PREMUL, colorSpace};

#if (defined(RS_ENABLE_GL) && defined(RS_ENABLE_EGLIMAGE))
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
        if (renderEngine == nullptr) {
            RS_LOGE("RSSurfaceCaptureTaskParallel::CreateSurface: renderEngine is nullptr");
            return nullptr;
        }
#if (defined(RS_ENABLE_GPU) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)))
        auto renderContext = renderEngine->GetRenderContext();
#else
        auto renderContext = nullptr;
#endif
        if (renderContext == nullptr) {
            RS_LOGE("RSSurfaceCaptureTaskParallel::CreateSurface: renderContext is nullptr");
            return nullptr;
        }
        renderContext->SetUpGpuContext(nullptr);
        return Drawing::Surface::MakeRenderTarget(renderContext->GetDrGPUContext(), false, info);
    }
#endif
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return Drawing::Surface::MakeRenderTarget(gpuContext_.get(), false, info);
    }
#endif

    return Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
}

#ifdef RS_ENABLE_GPU
void RSSurfaceCaptureTaskParallel::SetupGpuContext()
{
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (renderEngine == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::SetupGpuContext: renderEngine is nullptr");
        return;
    }
    auto renderContext = renderEngine->GetRenderContext();
    gpuContext_ = renderContext != nullptr ? renderContext->GetSharedDrGPUContext() : nullptr;
    if (gpuContext_ == nullptr) {
        RS_LOGW("RSSurfaceCaptureTaskParallel::SetupGpuContext gpuContext_ is nullptr");
    }
}
#endif

int32_t RSSurfaceCaptureTaskParallel::CalPixelMapRotation()
{
    auto screenRotation = ScreenRotationMapping(screenRotation_);
    auto screenCorrection = ScreenRotationMapping(screenCorrection_);
    int32_t rotation = screenRotation - screenCorrection;
    return rotation;
}

void RSSurfaceCaptureTaskParallel::AddBlur(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<Drawing::Surface>& surface, float blurRadius)
{
    Drawing::AutoCanvasRestore autoRestore(canvas, true);
    canvas.ResetMatrix();
    auto image = surface->GetImageSnapshot();
    if (!image) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::AddBlur image is null");
        return;
    }
    auto hpsParam = Drawing::HpsBlurParameter(Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight()),
        Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight()), blurRadius, 1.f, 1.f);
    auto filter = HpsBlurFilter::GetHpsBlurFilter();
    filter.ApplyHpsBlur(canvas, image, hpsParam, 1.f);
}

#ifdef RS_ENABLE_UNI_RENDER
    std::function<void()> RSSurfaceCaptureTaskParallel::CreateSurfaceSyncCopyTask(
        std::shared_ptr<Drawing::Surface> surface, std::unique_ptr<Media::PixelMap> pixelMap, NodeId id,
        const RSSurfaceCaptureConfig& captureConfig, sptr<RSISurfaceCaptureCallback> callback,
        int32_t rotation)
{
    if (surface == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel: nodeId:[%{public}" PRIu64 "], surface is nullptr", id);
        return {};
    }
    Drawing::BackendTexture backendTexture = surface->GetBackendTexture();
    if (!backendTexture.IsValid()) {
        RS_LOGE("RSSurfaceCaptureTaskParallel: SkiaSurface bind Image failed: BackendTexture is invalid");
        return {};
    }
    auto wrapper = std::make_shared<std::tuple<std::unique_ptr<Media::PixelMap>>>();
    std::get<0>(*wrapper) = std::move(pixelMap);
    auto wrapperSf = std::make_shared<std::tuple<std::shared_ptr<Drawing::Surface>>>();
    std::get<0>(*wrapperSf) = std::move(surface);
    std::function<void()> copytask = [wrapper, captureConfig, callback, backendTexture, wrapperSf, id,
        rotation]() -> void {
        RS_TRACE_NAME_FMT("copy and send capture useDma:%d", captureConfig.useDma);
        if (callback == nullptr) {
            RS_LOGE("RSSurfaceCaptureTaskParallel: nodeId:[%{public}" PRIu64 "], callback is nullptr", id);
            return;
        }
        auto surfaceInfo = std::get<0>(*wrapperSf)->GetImageInfo();
        if (!backendTexture.IsValid()) {
            RS_LOGE("RSSurfaceCaptureTaskParallel: Surface bind Image failed: BackendTexture is invalid");
            callback->OnSurfaceCapture(id, captureConfig, nullptr);
            RSUniRenderUtil::ClearNodeCacheSurface(
                std::move(std::get<0>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
            return;
        }
        auto pixelmap = std::move(std::get<0>(*wrapper));
        if (pixelmap == nullptr) {
            RS_LOGE("RSSurfaceCaptureTaskParallel: pixelmap == nullptr");
            callback->OnSurfaceCapture(id, captureConfig, nullptr);
            RSUniRenderUtil::ClearNodeCacheSurface(
                std::move(std::get<0>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
            return;
        }

        if (!PixelMapCopy(pixelmap, surfaceInfo.GetColorSpace(), backendTexture, surfaceInfo.GetColorType(),
            captureConfig.useDma, rotation)) {
            callback->OnSurfaceCapture(id, captureConfig, nullptr);
            RSUniRenderUtil::ClearNodeCacheSurface(
                std::move(std::get<0>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
            return;
        }
        callback->OnSurfaceCapture(id, captureConfig, pixelmap.get());
        RS_LOGI("RSSurfaceCaptureTaskParallel::Capture capture success nodeId:[%{public}]" PRIu64
            "], pixelMap width: %{public}d, height: %{public}d",
            id, pixelmap->GetWidth(), pixelmap->GetHeight());
        RSBackgroundThread::Instance().CleanGrResource();
        RSUniRenderUtil::ClearNodeCacheSurface(
            std::move(std::get<0>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
    };
    return copytask;
}


std::function<void()> RSSurfaceCaptureTaskParallel::CreateSurfaceSyncCopyTaskWithDoublePixelMap(
    std::shared_ptr<Drawing::Surface> surface, std::unique_ptr<Media::PixelMap> pixelMap,
    std::shared_ptr<Drawing::Surface> surfaceHDR, std::unique_ptr<Media::PixelMap> pixelMapHDR, NodeId id,
    const RSSurfaceCaptureConfig& captureConfig, sptr<RSISurfaceCaptureCallback> callback, int32_t rotation)
{
    if (surface == nullptr || surfaceHDR == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel: nodeId:[%{public}"PRIu64"], surface is nullptr", id);
        return {};
    }
    Drawing::BackendTexture backendTexture = surface->GetBackendTexture();
    Drawing::BackendTexture backendTextureHDR = surfaceHDR->GetBackendTexture();
    if (!backendTexture.IsValid() || !backendTextureHDR.IsValid()) {
        RS_LOGE("RSSurfaceCaptureTaskParallel: SkiaSurface bind Image failed: BackendTexture is invalid");
        return {};
    }
    auto wrapper = std::make_shared<std::tuple<std::unique_ptr<Media::PixelMap>, std::unique_ptr<Media::PixelMap>>>();
    std::get<0>(*wrapper) = std::move(pixelMap);
    std::get<1>(*wrapper) = std::move(pixelMapHDR);
    auto wrapperSf =
        std::make_shared<std::tuple<std::shared_ptr<Drawing::Surface>, std::shared_ptr<Drawing::Surface>>>();
    std::get<0>(*wrapperSf) = std::move(surface);
    std::get<1>(*wrapperSf) = std::move(surfaceHDR);
    std::function<void()> copytask = [wrapper, captureConfig, callback, backendTexture, backendTextureHDR, wrapperSf,
        id, rotation]() -> void {
        RS_TRACE_NAME_FMT("copy and send capture useDma:%d", captureConfig.useDma);
        if (callback == nullptr) {
            RS_LOGE("RSSurfaceCaptureTaskParallel: nodeId:[%{public}" PRIu64 "], callback is nullptr", id);
            return;
        }
        auto surfaceInfo = std::get<0>(*wrapperSf)->GetImageInfo();
        auto surfaceInfoHDR = std::get<1>(*wrapperSf)->GetImageInfo();
        auto pixelmap = std::move(std::get<0>(*wrapper));
        auto pixelmapHDR = std::move(std::get<1>(*wrapper));
        if (pixelmap == nullptr || pixelmapHDR == nullptr) {
            RS_LOGE("RSSurfaceCaptureTaskParallel: pixelmap or pixelmapHDR is nullptr");
            callback->OnSurfaceCapture(id, captureConfig, nullptr, nullptr);
            RSUniRenderUtil::ClearNodeCacheSurface(
                std::move(std::get<0>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
            RSUniRenderUtil::ClearNodeCacheSurface(
                std::move(std::get<1>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
                return;
        }
        if (!PixelMapCopy(pixelmap, surfaceInfo.GetColorSpace(), backendTexture, surfaceInfo.GetColorType(),
            captureConfig.useDma, rotation) ||
            !PixelMapCopy(pixelmapHDR, surfaceInfoHDR.GetColorSpace(), backendTextureHDR, surfaceInfoHDR.GetColorType(),
            captureConfig.useDma, rotation)) {
            callback->OnSurfaceCapture(id, captureConfig, nullptr, nullptr);
            RSUniRenderUtil::ClearNodeCacheSurface(
                std::move(std::get<0>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
            RSUniRenderUtil::ClearNodeCacheSurface(
                std::move(std::get<1>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
            return;
        }
#ifdef USE_VIDEO_PROCESSING_ENGINE
        GSError ret = RSHdrUtil::SetMetadata(reinterpret_cast<SurfaceBuffer*>(pixelmap->GetFd()),
            RSHDRUtilConst::HDR_CAPTURE_SDR_COLORSPACE, static_cast<uint32_t>(
            HDI::Display::Graphic::Common::V1_0::CM_HDR_Metadata_Type::CM_IMAGE_HDR_VIVID_DUAL));
        if (ret != GSERROR_OK) {
            RS_LOGE("RSSurfaceCaptureTaskParallel: Set SDR metadata error with: %{public}d", ret);
            return;
        }
        ret = RSHdrUtil::SetMetadata(reinterpret_cast<SurfaceBuffer*>(pixelmapHDR->GetFd()),
            RSHDRUtilConst::HDR_CAPTURE_HDR_COLORSPACE, static_cast<uint32_t>(
            HDI::Display::Graphic::Common::V1_0::CM_HDR_Metadata_Type::CM_IMAGE_HDR_VIVID_SINGLE));
        if (ret != GSERROR_OK) {
            RS_LOGE("RSSurfaceCaptureTaskParallel: Set HDR metadata error with: %{public}d", ret);
            return;
        }
#endif
        callback->OnSurfaceCapture(id, captureConfig, pixelmap.get(), pixelmapHDR.get());
        RSBackgroundThread::Instance().CleanGrResource();
        RSUniRenderUtil::ClearNodeCacheSurface(
            std::move(std::get<0>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
        RSUniRenderUtil::ClearNodeCacheSurface(
            std::move(std::get<1>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
    };
    return copytask;
}

bool RSSurfaceCaptureTaskParallel::PixelMapCopy(std::unique_ptr<Media::PixelMap>& pixelmap,
    std::shared_ptr<Drawing::ColorSpace> colorSpace, const Drawing::BackendTexture& backendTexture,
    Drawing::ColorType colorType, bool useDma, int32_t rotation)
{
    if (!pixelmap) {
        return false;
    }
    Drawing::ImageInfo info = Drawing::ImageInfo{ pixelmap->GetWidth(), pixelmap->GetHeight(),
        colorType, Drawing::ALPHATYPE_PREMUL, colorSpace};
    Drawing::TextureOrigin textureOrigin = Drawing::TextureOrigin::BOTTOM_LEFT;
    Drawing::BitmapFormat bitmapFormat =
        Drawing::BitmapFormat{ colorType, Drawing::ALPHATYPE_PREMUL };
    std::shared_ptr<Drawing::Surface> surface;
    auto grContext = RSBackgroundThread::Instance().GetShareGPUContext();
    if (!grContext) {
        RS_LOGE("RSSurfaceCaptureTaskParallel: GetShareGPUContext fail.");
        return false;
    }
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    DmaMem dmaMem;
    if (useDma &&
        (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR)) {
        sptr<SurfaceBuffer> surfaceBuffer = dmaMem.DmaMemAlloc(info, pixelmap);
        if (surfaceBuffer != nullptr && colorSpace != nullptr && !colorSpace->IsSRGB()) {
            surfaceBuffer->SetSurfaceBufferColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
        }
        surface = dmaMem.GetSurfaceFromSurfaceBuffer(surfaceBuffer, grContext);
        if (surface == nullptr) {
            RS_LOGE("RSSurfaceCaptureTaskParallel: GetSurfaceFromSurfaceBuffer fail.");
            return false;
        }
        auto tmpImg = std::make_shared<Drawing::Image>();
        DrawCapturedImg(*tmpImg, *surface, backendTexture, textureOrigin, bitmapFormat);
    } else {
#else
    {
#endif
        auto tmpImg = std::make_shared<Drawing::Image>();
        tmpImg->BuildFromTexture(*grContext, backendTexture.GetTextureInfo(),
            textureOrigin, bitmapFormat, colorSpace);
        if (!CopyDataToPixelMap(tmpImg, pixelmap)) {
            RS_LOGE("RSSurfaceCaptureTaskParallel: CopyDataToPixelMap failed");
            return false;
        }
    }
    if (rotation) {
        pixelmap->ratate(rotation);
    }

    RSUniRenderUtil::FlushDmaSurfaceBuffer(pixelmap.get());
    //To get dump image
    // execute "param set rosen.dumpsurfacetype.enabled 3 && setenforce 0"
    RSBaseRenderUtil::WritePixelMapToPng(*pixelmap);
    auto pixelDump = PixelMapSamplingDump(pixelmap, pixelmap->GetWidth() / 2, 0) |
                    PixelMapSamplingDump(pixelmap, 0, pixelmap->GetHeight() / 2) |
                    PixelMapSamplingDump(pixelmap, pixelmap->GetWidth() / 2, pixelmap->GetHeight() / 2) |
                    PixelMapSamplingDump(pixelmap, pixelmap->GetWidth() - 1, pixelmap->GetHeight() / 2) |
                    PixelMapSamplingDump(pixelmap, pixelmap->GetWidth() / 2, pixelmap->GetHeight() - 1);
    if ((pixelDump & ALPHA_MASK) != 0) {
        RS_LOGI("RSSurfaceCaptureTaskParallel::CreateSurfaceSyncCopyTask pixelmap is Non-transparent");
    } else {
        RS_LOGW("RSSurfaceCaptureTaskParallel::CreateSurfaceSyncCopyTask pixelmap is transparent");
    }
    pixelmap->SetMemoryName("RSSurfaceCaptureForClient");
    return true;
}
#endif // RS_ENABLE_UNI_RENDER

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
DmaMem::~DmaMem()
{
    ReleaseDmaMemory();
}

void DmaMem::ReleaseDmaMemory()
{
    if (nativeWindowBuffer_ != nullptr) {
        DestroyNativeWindowBuffer(nativeWindowBuffer_);
        nativeWindowBuffer_ = nullptr;
    }
}

sptr<SurfaceBuffer> DmaMem::DmaMemAlloc(Drawing::ImageInfo &dstInfo, const std::unique_ptr<Media::PixelMap>& pixelmap)
{
#if defined(_WIN32) || defined(_APPLE) || defined(A_PLATFORM) || defined(IOS_PLATFORM)
    RS_LOGE("Unsupport dma mem alloc");
    return nullptr;
#else
    if (pixelmap == nullptr) {
        RS_LOGE("DmaMem::DmaMemAlloc: pixelmap is nullptr");
        return nullptr;
    }
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    if (!surfaceBuffer) {
        RS_LOGE("DmaMem::DmaMemAlloc: surfaceBuffer create failed");
        return nullptr;
    }
    auto colorGamut = pixelmap->InnerGetGrColorSpace().GetColorSpaceName() == OHOS::ColorManager::ColorSpaceName::SRGB ?
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB : GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    BufferRequestConfig requestConfig = {
        .width = dstInfo.GetWidth(),
        .height = dstInfo.GetHeight(),
        .strideAlignment = 0x8, // set 0x8 as default value to alloc SurfaceBufferImpl
        .format = dstInfo.GetColorType() == Drawing::ColorType::COLORTYPE_RGBA_F16 ?
            GRAPHIC_PIXEL_FMT_RGBA16_FLOAT : GRAPHIC_PIXEL_FMT_RGBA_8888, // PixelFormat
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE |
            BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_MMZ_CACHE,
        .timeout = 0,
        .colorGamut = colorGamut,
        .transform = GraphicTransformType::GRAPHIC_ROTATE_NONE,
    };
    GSError ret = surfaceBuffer->Alloc(requestConfig);
    if (ret != GSERROR_OK) {
        RS_LOGE("DmaMem::DmaMemAlloc: surfaceBuffer alloc failed, %{public}s", GSErrorStr(ret).c_str());
        return nullptr;
    }
    void* nativeBuffer = surfaceBuffer.GetRefPtr();
    OHOS::RefBase *ref = reinterpret_cast<OHOS::RefBase *>(nativeBuffer);
    ref->IncStrongRef(ref);
    int32_t bufferSize = pixelmap->GetByteCount();
    pixelmap->SetPixelsAddr(surfaceBuffer->GetVirAddr(), nativeBuffer, bufferSize,
        Media::AllocatorType::DMA_ALLOC, nullptr);
    return surfaceBuffer;
#endif
}

static inline void DeleteVkImage(void *context)
{
    NativeBufferUtils::VulkanCleanupHelper *cleanupHelper =
        static_cast<NativeBufferUtils::VulkanCleanupHelper *> (context);
    if (cleanupHelper != nullptr) {
        cleanupHelper->UnRef();
    }
}

std::shared_ptr<Drawing::Surface> DmaMem::GetSurfaceFromSurfaceBuffer(
    sptr<SurfaceBuffer> surfaceBuffer, std::shared_ptr<Drawing::GPUContext> gpuContext)
{
    if (surfaceBuffer == nullptr || gpuContext == nullptr) {
        RS_LOGE("GetSurfaceFromSurfaceBuffer surfaceBuffer or gpuContext is nullptr");
        return nullptr;
    }
    if (nativeWindowBuffer_ == nullptr) {
        nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&surfaceBuffer);
        if (!nativeWindowBuffer_) {
            RS_LOGE("DmaMem::GetSurfaceFromSurfaceBuffer: nativeWindowBuffer_ is nullptr");
            return nullptr;
        }
    }

    Drawing::BackendTexture backendTextureTmp =
        NativeBufferUtils::MakeBackendTextureFromNativeBuffer(nativeWindowBuffer_,
            surfaceBuffer->GetWidth(), surfaceBuffer->GetHeight());
    if (!backendTextureTmp.IsValid()) {
        return nullptr;
    }

    auto vkTextureInfo = backendTextureTmp.GetTextureInfo().GetVKTextureInfo();
    if (vkTextureInfo == nullptr) {
        return nullptr;
    }
    vkTextureInfo->imageUsageFlags = vkTextureInfo->imageUsageFlags | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    auto cleanUpHelper = new NativeBufferUtils::VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
        vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
    if (cleanUpHelper == nullptr) {
        return nullptr;
    }
    // attention: cleanUpHelper will be delete by NativeBufferUtils::DeleteVkImage, don't delete again
    auto colorSpace =
        RSBaseRenderEngine::ConvertColorGamutToDrawingColorSpace(surfaceBuffer->GetSurfaceBufferColorGamut());
    auto colorType = surfaceBuffer->GetBufferRequestConfig().format == GRAPHIC_PIXEL_FMT_RGBA16_FLOAT ?
        Drawing::ColorType::COLORTYPE_RGBA_F16 : Drawing::ColorType::COLORTYPE_RGBA_8888;
    auto drawingSurface = Drawing::Surface::MakeFromBackendTexture(
        gpuContext.get(),
        backendTextureTmp.GetTextureInfo(),
        Drawing::TextureOrigin::TOP_LEFT,
        1, colorType, colorSpace,
        NativeBufferUtils::DeleteVkImage, cleanUpHelper);
    return drawingSurface;
}
#endif
} // namespace Rosen
} // namespace OHOS
