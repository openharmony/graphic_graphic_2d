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

#include "feature/capture/rs_ui_capture_solo_task_parallel.h"

#include <memory>
#include <parameters.h>
#include <sys/mman.h>

#include "draw/surface.h"
#include "draw/color.h"

#include "rs_trace.h"

#include "common/rs_background_thread.h"
#include "common/rs_obj_abs_geometry.h"
#include "drawable/rs_canvas_render_node_drawable.h"
#include "feature/capture/rs_surface_capture_task_parallel.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "feature_cfg/graphic_feature_param_manager.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_render_service_connection.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_surface.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_skia_filter.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/rs_screen_mode_info.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/native_buffer_utils.h"
#endif

namespace OHOS {
namespace Rosen {

std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> RSUiCaptureSoloTaskParallel::CaptureSoloNode(
    NodeId id, const RSSurfaceCaptureConfig& captureConfig)
{
    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> pixelMapIdPairVector;
    std::vector<NodeId> nodeIdVec;
    std::shared_ptr<RSBaseRenderNode> instanceRootNode =
        RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(id);
    if (!instanceRootNode) {
        RS_LOGE("RSUiCaptureSoloTaskParallel::CaptureSoloNode RootNodeId:[%{public}" PRIu64 "] is nullptr", id);
        return pixelMapIdPairVector;
    }
    instanceRootNode->CollectAllChildren(instanceRootNode, nodeIdVec);
    for (auto nodeId : nodeIdVec) {
        std::unique_ptr<Media::PixelMap> soloPixelMap = CaptureSoloNodePixelMap(nodeId, captureConfig);
        if (!soloPixelMap) {
            RS_LOGE("RSUiCaptureSoloTaskParallel::CaptureSoloNode nodeId:[%{public}" PRIu64 "], "
                "soloPixelMap is nullptr", nodeId);
        }
        std::shared_ptr<Media::PixelMap> soloPixelMapShared(std::move(soloPixelMap));
        pixelMapIdPairVector.push_back(std::make_pair(nodeId, soloPixelMapShared));
    }
    return pixelMapIdPairVector;
}

std::unique_ptr<Media::PixelMap> RSUiCaptureSoloTaskParallel::CaptureSoloNodePixelMap(NodeId id,
    const RSSurfaceCaptureConfig& captureConfig)
{
    RS_LOGI("RSUiCaptureSoloTaskParallel::CaptureSoloNodePixelMap nodeId:[%{public}" PRIu64 "]", id);
    std::shared_ptr<RSUiCaptureSoloTaskParallel> captureHandle =
        std::make_shared<RSUiCaptureSoloTaskParallel>(id, captureConfig);
    if (captureHandle == nullptr) {
        RS_LOGE("RSUiCaptureSoloTaskParallel::CaptureSoloNodePixelMap captureHandle is nullptr!");
        return nullptr;
    }
    if (!captureHandle->CreateResources()) {
        RS_LOGE("RSUiCaptureSoloTaskParallel::CaptureSoloNodePixelMap CreateResources failed");
        return nullptr;
    }
    std::function<void()> captureTask = [captureHandle]() -> void {
        RSSystemProperties::SetForceHpsBlurDisabled(true);
        if (!captureHandle->Run()) {
            RS_LOGE("RSUiCaptureSoloTaskParallel::CaptureSoloNodePixelMap Run failed");
            captureHandle->pixelMap_ = nullptr;
        }
        RSSystemProperties::SetForceHpsBlurDisabled(false);
    };
    RSUniRenderThread::Instance().PostSyncTask(captureTask);
    if (captureHandle->pixelMap_ == nullptr) {
        RS_LOGD("RSUiCaptureSoloTaskParallel::CaptureSoloNodePixelMap pixelMap_ is nullptr");
    }
    return std::move(captureHandle->pixelMap_);
}

bool RSUiCaptureSoloTaskParallel::CreateResources()
{
    if (ROSEN_EQ(captureConfig_.scaleX, 0.f) || ROSEN_EQ(captureConfig_.scaleY, 0.f) ||
        captureConfig_.scaleX < 0.f || captureConfig_.scaleY < 0.f) {
        RS_LOGE("RSUiCaptureSoloTaskParallel::CreateResources: SurfaceCapture scale is invalid.");
        return false;
    }
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(nodeId_);
    if (node == nullptr) {
        RS_LOGE("RSUiCaptureSoloTaskParallel::CreateResources: Invalid nodeId:[%{public}" PRIu64 "]",
            nodeId_);
        return false;
    }
    if (node->GetType() == RSRenderNodeType::UNKNOW) {
        RS_LOGE("RSUiCaptureSoloTaskParallel::CreateResources: nodeId:[%{public}" PRIu64 "]"
            " RSRenderNodeType is UNKNOW", nodeId_);
        return false;
    }
#ifdef RS_ENABLE_VK
    float nodeBoundsWidth = node->GetRenderProperties().GetBoundsWidth();
    float nodeBoundsHeight = node->GetRenderProperties().GetBoundsHeight();
    int32_t width = ceil(nodeBoundsWidth * captureConfig_.scaleX);
    int32_t height = ceil(nodeBoundsHeight * captureConfig_.scaleY);
    if (width > 0 && static_cast<int32_t>(OHOS::Rosen::NativeBufferUtils::VKIMAGE_LIMIT_SIZE) / width < height) {
        RS_LOGE("RSUiCaptureSoloTaskParallel::CreateResources: image is too large,"
            " width:%{public}d, height::%{public}d", width, height);
        return false;
    }
    RS_LOGD("RSUiCaptureSoloTaskParallel::CreateResources: Origin nodeBoundsWidth is [%{public}f,]"
        " Origin nodeBoundsHeight is [%{public}f]", nodeBoundsWidth, nodeBoundsHeight);
#endif
    if (auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>()) {
        nodeDrawable_ = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        pixelMap_ = CreatePixelMapByNode(surfaceNode);
    } else if (auto canvasNode = node->ReinterpretCastTo<RSCanvasRenderNode>()) {
        nodeDrawable_ = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));
        pixelMap_ = CreatePixelMapByNode(canvasNode);
    } else if (auto effectNode = node->ReinterpretCastTo<RSEffectRenderNode>()) {
        nodeDrawable_ = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(effectNode));
        pixelMap_ = CreatePixelMapByNode(effectNode);
    } else if (node->GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE ||
        node->GetType() == RSRenderNodeType::ROOT_NODE) {
        nodeDrawable_ = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
        pixelMap_ = CreatePixelMapByNode(node);
    } else {
        RS_LOGE("RSUiCaptureSoloTaskParallel::CreateResources: nodeId:[%{public}" PRIu64 "]"
            " RSRenderNodeType is invalid", nodeId_);
        return false;
    }
    if (pixelMap_ == nullptr) {
        RS_LOGE("RSUiCaptureSoloTaskParallel::CreateResources: pixelMap_ is nullptr");
        return false;
    }
    return true;
}

bool RSUiCaptureSoloTaskParallel::Run()
{
    RS_TRACE_NAME_FMT("RSUiCaptureSoloTaskParallel::TakeSurfaceCaptureSolo. NodeId: [%" PRIu64 "], from pid: [%d]",
        nodeId_, ExtractPid(nodeId_));

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    auto renderContext = RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext();
    auto grContext = renderContext != nullptr ? renderContext->GetDrGPUContext() : nullptr;
    std::string nodeName("RSUiCaptureSoloTaskParallel");
    RSTagTracker tagTracker(renderContext != nullptr ? renderContext->GetSharedDrGPUContext() : nullptr,
        nodeId_, RSTagTracker::TAGTYPE::TAG_CAPTURE, nodeName);
#endif
    auto surface = CreateSurface(pixelMap_);
    if (surface == nullptr) {
        RS_LOGE("RSUiCaptureSoloTaskParallel::Run: surface is nullptr!");
        return false;
    }
    if (!nodeDrawable_) {
        RS_LOGE("RSUiCaptureSoloTaskParallel::Run: Invalid RSRenderNodeDrawable!");
        return false;
    }

    RSPaintFilterCanvas canvas(surface.get());
    canvas.Scale(captureConfig_.scaleX, captureConfig_.scaleY);
    canvas.SetDisableFilterCache(true);
    canvas.SetUICapture(true);
    const auto& nodeParams = nodeDrawable_->GetRenderParams();
    if (UNLIKELY(!nodeParams)) {
        RS_LOGE("RSUiCaptureSoloTaskParallel::Run: RenderParams is nullptr!");
        return false;
    }
    Drawing::Matrix relativeMatrix = Drawing::Matrix();
    relativeMatrix.Set(Drawing::Matrix::Index::SCALE_X, captureConfig_.scaleX);
    relativeMatrix.Set(Drawing::Matrix::Index::SCALE_Y, captureConfig_.scaleY);
    Drawing::Matrix invertMatrix;
    if (nodeParams->GetMatrix().Invert(invertMatrix)) {
        relativeMatrix.PreConcat(invertMatrix);
    }
    canvas.SetMatrix(relativeMatrix);

    RSUniRenderThread::SetCaptureParam(CaptureParam(true, true, false, false, false, false, false, true));
    nodeDrawable_->OnCapture(canvas);
    RSUniRenderThread::ResetCaptureParam();

#if (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE)
#ifdef RS_ENABLE_UNI_RENDER
    RSUniRenderUtil::OptimizedFlushAndSubmit(surface, grContext, GetFeatureParamValue("UICaptureConfig",
        &UICaptureParam::IsUseOptimizedFlushAndSubmitEnabled).value_or(false));
    bool snapshotDmaEnabled = system::GetBoolParameter("rosen.snapshotDma.enabled", true);
    bool isEnableFeature = GetFeatureParamValue("CaptureConfig",
        &CaptureBaseParam::IsSnapshotWithDMAEnabled).value_or(false);
    if (snapshotDmaEnabled && isEnableFeature) {
        auto copytask =
            RSUiCaptureSoloTaskParallel::CreateSurfaceSyncCopyTask(
                surface, pixelMap_, nodeId_, captureConfig_);
        if (!copytask) {
            RS_LOGE("RSUiCaptureSoloTaskParallel::Run: create capture task failed!");
            return false;
        }
        RSBackgroundThread::Instance().PostSyncTask(copytask);
        return true;
    } else {
        std::shared_ptr<Drawing::Image> img(surface.get()->GetImageSnapshot());
        if (!img) {
            RS_LOGE("RSUiCaptureSoloTaskParallel::Run: img is nullptr");
            return false;
        }
        if (!CopyDataToPixelMap(img, pixelMap_)) {
            RS_LOGE("RSUiCaptureSoloTaskParallel::Run: CopyDataToPixelMap failed");
            return false;
        }
    }
#endif
#endif
    // To get dump image
    // execute "param set rosen.dumpsurfacetype.enabled 3 && setenforce 0"
    RSBaseRenderUtil::WritePixelMapToPng(*pixelMap_);
    RS_LOGI("RSUiCaptureSoloTaskParallel::CaptureSoloNodePixelMap capture success nodeId:[%{public}" PRIu64
        "], pixelMap width: %{public}d, height: %{public}d",
        nodeId_, pixelMap_->GetWidth(), pixelMap_->GetHeight());
    return true;
}

std::unique_ptr<Media::PixelMap> RSUiCaptureSoloTaskParallel::CreatePixelMapByNode(
    std::shared_ptr<RSRenderNode> node) const
{
    float pixmapWidth = node->GetRenderProperties().GetBoundsWidth();
    float pixmapHeight = node->GetRenderProperties().GetBoundsHeight();
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixmapWidth * captureConfig_.scaleX);
    opts.size.height = ceil(pixmapHeight * captureConfig_.scaleY);
    RS_LOGD("RSUiCaptureSoloTaskParallel::CreatePixelMapByNode: NodeId:[%{public}" PRIu64 "],"
        " origin pixelmap width is [%{public}f], height is [%{public}f],"
        " created pixelmap width is [%{public}u], height is [%{public}u],"
        " the scale is scaleX:[%{public}f], scaleY:[%{public}f]",
        node->GetId(), pixmapWidth, pixmapHeight, opts.size.width, opts.size.height,
        captureConfig_.scaleX, captureConfig_.scaleY);
    return Media::PixelMap::Create(opts);
}

std::shared_ptr<Drawing::Surface> RSUiCaptureSoloTaskParallel::CreateSurface(
    const std::unique_ptr<Media::PixelMap>& pixelmap) const
{
    if (pixelmap == nullptr) {
        RS_LOGE("RSUiCaptureSoloTaskParallel::CreateSurface: pixelmap == nullptr");
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        RS_LOGE("RSUiCaptureSoloTaskParallel::CreateSurface: address == nullptr");
        return nullptr;
    }
    Drawing::ImageInfo info = Drawing::ImageInfo{pixelmap->GetWidth(), pixelmap->GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL};

#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        auto renderContext = RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext();
        if (renderContext == nullptr) {
            RS_LOGE("RSUiCaptureSoloTaskParallel::CreateSurface: renderContext is nullptr");
            return nullptr;
        }
        renderContext->SetUpGpuContext(nullptr);
        return Drawing::Surface::MakeRenderTarget(renderContext->GetDrGPUContext(), false, info);
    }
#endif
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return Drawing::Surface::MakeRenderTarget(
            RSUniRenderThread::Instance().GetRenderEngine()->GetSkContext().get(), false, info);
    }
#endif

    return Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
}

#ifdef RS_ENABLE_UNI_RENDER
std::function<void()> RSUiCaptureSoloTaskParallel::CreateSurfaceSyncCopyTask(
    std::shared_ptr<Drawing::Surface> surface, std::unique_ptr<Media::PixelMap>& pixelMap,
    NodeId id, const RSSurfaceCaptureConfig& captureConfig, int32_t rotation)
{
    Drawing::BackendTexture backendTexture = surface->GetBackendTexture();
    if (!backendTexture.IsValid()) {
        RS_LOGE("RSUiCaptureSoloTaskParallel: SkiaSurface bind Image failed: BackendTexture is invalid");
        return {};
    }
    auto wrapper = std::make_shared<std::tuple<std::unique_ptr<Media::PixelMap>>>();
    std::get<0>(*wrapper) = std::move(pixelMap);
    auto wrapperSf = std::make_shared<std::tuple<std::shared_ptr<Drawing::Surface>>>();
    std::get<0>(*wrapperSf) = std::move(surface);
    std::function<void()> copytask = [
        &pixelMap, wrapper, captureConfig, backendTexture, wrapperSf, id, rotation]() -> void {
        RS_TRACE_NAME_FMT("TakeSurfaceCaptureSolo: copy and send capture");
        if (!backendTexture.IsValid()) {
            RS_LOGE("RSUiCaptureSoloTaskParallel: Surface bind Image failed: BackendTexture is invalid");
            std::get<0>(*wrapper) = nullptr;
            RSUniRenderUtil::ClearNodeCacheSurface(
                std::move(std::get<0>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
            return;
        }
        auto pixelmap = std::move(std::get<0>(*wrapper));
        if (pixelmap == nullptr) {
            RS_LOGE("RSUiCaptureSoloTaskParallel: pixelmap == nullptr");
            std::get<0>(*wrapper) = nullptr;
            RSUniRenderUtil::ClearNodeCacheSurface(
                std::move(std::get<0>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
            return;
        }

        Drawing::ImageInfo info = Drawing::ImageInfo{ pixelmap->GetWidth(), pixelmap->GetHeight(),
            Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
        Drawing::TextureOrigin textureOrigin = Drawing::TextureOrigin::BOTTOM_LEFT;
        Drawing::BitmapFormat bitmapFormat =
            Drawing::BitmapFormat{ Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
        std::shared_ptr<Drawing::Surface> surface;
        auto grContext = RSBackgroundThread::Instance().GetShareGPUContext();
        if (!grContext) {
            RS_LOGE("RSUiCaptureSoloTaskParallel: SharedGPUContext get failed");
            std::get<0>(*wrapper) = nullptr;
            return;
        }

        auto tmpImg = std::make_shared<Drawing::Image>();
        tmpImg->BuildFromTexture(*grContext, backendTexture.GetTextureInfo(),
            textureOrigin, bitmapFormat, nullptr);
        if (!CopyDataToPixelMap(tmpImg, pixelmap)) {
            RS_LOGE("RSUiCaptureSoloTaskParallel: CopyDataToPixelMap failed");
            std::get<0>(*wrapper) = nullptr;
            RSUniRenderUtil::ClearNodeCacheSurface(
                std::move(std::get<0>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
            return;
        }

        if (rotation) {
            pixelmap->rotate(rotation);
        }
        // To get dump image
        // execute "param set rosen.dumpsurfacetype.enabled 3 && setenforce 0"
        RSBaseRenderUtil::WritePixelMapToPng(*pixelmap);
        RS_LOGI("RSUiCaptureSoloTaskParallel::CaptureSoloNodePixelMap CaptureSolo success nodeId:[%{public}" PRIu64
            "], pixelMap width: %{public}d, height: %{public}d", id, pixelmap->GetWidth(), pixelmap->GetHeight());
        pixelMap = std::move(pixelmap);
        RSBackgroundThread::Instance().CleanGrResource();
        RSUniRenderUtil::ClearNodeCacheSurface(
            std::move(std::get<0>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
    };
    return copytask;
}
#endif
} // namespace Rosen
} // namespace OHOS
