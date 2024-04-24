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

#include "pipeline/rs_surface_capture_task.h"

#include <memory>
#include <sys/mman.h>

#include "draw/surface.h"
#include "draw/color.h"
#include "rs_trace.h"

#include "common/rs_background_thread.h"
#include "common/rs_obj_abs_geometry.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_divided_render_util.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_connection.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_uni_render_util.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_surface.h"
#include "render/rs_skia_filter.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/rs_screen_mode_info.h"

namespace OHOS {
namespace Rosen {
bool RSSurfaceCaptureTask::Run(sptr<RSISurfaceCaptureCallback> callback)
{
    if (ROSEN_EQ(scaleX_, 0.f) || ROSEN_EQ(scaleY_, 0.f) || scaleX_ < 0.f || scaleY_ < 0.f) {
        RS_LOGE("RSSurfaceCaptureTask::Run: SurfaceCapture scale is invalid.");
        RSMainThread::Instance()->NotifySurfaceCapProcFinish();
        return false;
    }
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(nodeId_);
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: node is nullptr");
        RSMainThread::Instance()->NotifySurfaceCapProcFinish();
        return false;
    }
    std::unique_ptr<Media::PixelMap> pixelmap;
    visitor_ = std::make_shared<RSSurfaceCaptureVisitor>(scaleX_, scaleY_, RSUniRenderJudgement::IsUniRender());
    if (auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>()) {
        pixelmap = CreatePixelMapBySurfaceNode(surfaceNode, visitor_->IsUniRender());
        visitor_->IsDisplayNode(false);
    } else if (auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>()) {
        visitor_->SetHasingSecurityOrSkipLayer(FindSecurityOrSkipLayer());
        pixelmap = CreatePixelMapByDisplayNode(displayNode, visitor_->IsUniRender(),
            visitor_->GetHasingSecurityOrSkipLayer());
        visitor_->IsDisplayNode(true);
    } else {
        RS_LOGE("RSSurfaceCaptureTask::Run: Invalid RSRenderNodeType!");
        RSMainThread::Instance()->NotifySurfaceCapProcFinish();
        return false;
    }
    if (pixelmap == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: pixelmap == nullptr!");
        RSMainThread::Instance()->NotifySurfaceCapProcFinish();
        return false;
    }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::GPUContext* grContext = nullptr;
    if (isProcOnBgThread_) {
#if defined(RS_ENABLE_UNI_RENDER)
        grContext = RSBackgroundThread::Instance().GetShareGPUContext().get();
#endif
    } else {
#if defined(NEW_RENDER_CONTEXT)
        auto drawingContext = RSMainThread::Instance()->GetRenderEngine()->GetDrawingContext();
        grContext = drawingContext != nullptr ? drawingContext->GetDrawingContext() : nullptr;
    }
#else
        auto renderContext = RSMainThread::Instance()->GetRenderEngine()->GetRenderContext();
        grContext = renderContext != nullptr ? renderContext->GetDrGPUContext() : nullptr;
    }
#endif
    RSTagTracker tagTracker(grContext, node->GetId(), RSTagTracker::TAGTYPE::TAG_CAPTURE);
#endif
    auto surface = CreateSurface(pixelmap);
    if (surface == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: surface is nullptr!");
        RSMainThread::Instance()->NotifySurfaceCapProcFinish();
        return false;
    }
    visitor_->SetSurface(surface.get());
    node->Process(visitor_);
    RSMainThread::Instance()->NotifySurfaceCapProcFinish();
#if (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE)
#ifdef RS_ENABLE_UNI_RENDER
    if (RSSystemProperties::GetSnapshotWithDMAEnabled() && !isProcOnBgThread_) {
        surface->FlushAndSubmit(true);
        Drawing::BackendTexture backendTexture = surface->GetBackendTexture();
        if (!backendTexture.IsValid()) {
            RS_LOGE("RSSurfaceCaptureTask: SkiaSurface bind Image failed: BackendTexture is invalid");
            return false;
        }
        auto wrapper = std::make_shared<std::tuple<std::unique_ptr<Media::PixelMap>>>();
        std::get<0>(*wrapper) = std::move(pixelmap);
        auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>();
        auto rotation = (displayNode != nullptr) ? displayNode->GetScreenRotation()
            : ScreenRotation::INVALID_SCREEN_ROTATION;
        bool ableRotation = ((displayNode != nullptr) && visitor_->IsUniRender());
        auto id = nodeId_;
        auto screenCorrection = ScreenCorrection(screenCorrection_)
        auto wrapperSf = std::make_shared<std::tuple<std::shared_ptr<Drawing::Surface>>>();
        std::get<0>(*wrapperSf) = std::move(surface);
        std::function<void()> copytask = [wrapper, callback, backendTexture, wrapperSf,
                                             ableRotation, rotation, id, screenCorrection]() -> void {
            RS_TRACE_NAME("copy and send capture");
            if (!backendTexture.IsValid()) {
                RS_LOGE("RSSurfaceCaptureTask: Surface bind Image failed: BackendTexture is invalid");
                callback->OnSurfaceCapture(id, nullptr);
                RSUniRenderUtil::ClearNodeCacheSurface(
                    std::move(std::get<0>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
                return;
            }
            auto pixelmap = std::move(std::get<0>(*wrapper));
            if (pixelmap == nullptr) {
                RS_LOGE("RSSurfaceCaptureTask: pixelmap == nullptr");
                callback->OnSurfaceCapture(id, nullptr);
                RSUniRenderUtil::ClearNodeCacheSurface(
                    std::move(std::get<0>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
                return;
            }

            Drawing::ImageInfo info = Drawing::ImageInfo{ pixelmap->GetWidth(), pixelmap->GetHeight(),
                Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
            std::shared_ptr<Drawing::Surface> surface;
            auto grContext = RSBackgroundThread::Instance().GetShareGPUContext().get();
            surface = Drawing::Surface::MakeRenderTarget(grContext, false, info);
            if (surface == nullptr) {
                RS_LOGE("RSSurfaceCaptureTask::Run MakeRenderTarget fail.");
                callback->OnSurfaceCapture(id, nullptr);
                RSUniRenderUtil::ClearNodeCacheSurface(
                    std::move(std::get<0>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
                return;
            }
            auto canvas = std::make_shared<RSPaintFilterCanvas>(surface.get());
            auto tmpImg = std::make_shared<Drawing::Image>();
            Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
            Drawing::BitmapFormat bitmapFormat =
                Drawing::BitmapFormat{ Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
            tmpImg->BuildFromTexture(*canvas->GetGPUContext(), backendTexture.GetTextureInfo(),
                origin, bitmapFormat, nullptr);
            canvas->DrawImage(*tmpImg, 0.f, 0.f, Drawing::SamplingOptions());
            surface->FlushAndSubmit(true);
            if (!CopyDataToPixelMap(tmpImg, pixelmap)) {
                RS_LOGE("RSSurfaceCaptureTask::Run CopyDataToPixelMap failed");
                callback->OnSurfaceCapture(id, nullptr);
                RSUniRenderUtil::ClearNodeCacheSurface(
                    std::move(std::get<0>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
                return;
            }
            if (ableRotation) {
                if (screenCorrection != 0) {
                    pixelmap->rotate(screenCorrection);
                    RS_LOGI("RSSurfaceCaptureTask::Run: ScreenCorrection: %{public}d", screenCorrection);
                }

                if (rotation == ScreenRotation::ROTATION_90) {
                    pixelmap->rotate(static_cast<int32_t>(90)); // 90 degrees
                } else if (rotation == ScreenRotation::ROTATION_180) {
                    pixelmap->rotate(static_cast<int32_t>(180)); // 180 degrees
                } else if (rotation == ScreenRotation::ROTATION_270) {
                    pixelmap->rotate(static_cast<int32_t>(270)); // 270 degrees
                }
                RS_LOGI("RSSurfaceCaptureTask::Run: PixelmapRotation: %{public}d", static_cast<int32_t>(rotation));
            }
            // To get dump image
            // execute "param set rosen.dumpsurfacetype.enabled 3 && setenforce 0"
            RSBaseRenderUtil::WritePixelMapToPng(*pixelmap);
            callback->OnSurfaceCapture(id, pixelmap.get());
            RSBackgroundThread::Instance().CleanGrResource();
            RSUniRenderUtil::ClearNodeCacheSurface(
                std::move(std::get<0>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
        };
        RSBackgroundThread::Instance().PostTask(copytask);
        return true;
    } else {
        std::shared_ptr<Drawing::Image> img(surface.get()->GetImageSnapshot());
        if (!img) {
            RS_LOGE("RSSurfaceCaptureTask::Run: img is nullptr");
            return false;
        }
        if (!CopyDataToPixelMap(img, pixelmap)) {
            RS_LOGE("RSSurfaceCaptureTask::Run: CopyDataToPixelMap failed");
            return false;
        }
    }
#else
    std::shared_ptr<Drawing::Image> img(surface.get()->GetImageSnapshot());
    if (!img) {
        RS_LOGE("RSSurfaceCaptureTask::Run: img is nullptr");
        return false;
    }
    if (!CopyDataToPixelMap(img, pixelmap)) {
            RS_LOGE("RSSurfaceCaptureTask::Run: CopyDataToPixelMap failed");
            return false;
    }
#endif // RS_ENABLE_UNI_RENDER
#endif
    if (auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>()) {
        if (visitor_->IsUniRender()) {
            if (screenCorrection_ != ScreenRotation::ROTATION_0) {
                int32_t angle = ScreenCorrection(screenCorrection_);
                pixelmap->rotate(angle);
                RS_LOGI("RSSurfaceCaptureTask::Run: ScreenCorrection: %{public}d", angle);
            }

            auto rotation = displayNode->GetScreenRotation();
            if (rotation == ScreenRotation::ROTATION_90) {
                pixelmap->rotate(static_cast<int32_t>(90)); // 90 degrees
            } else if (rotation == ScreenRotation::ROTATION_180) {
                pixelmap->rotate(static_cast<int32_t>(180)); // 180 degrees
            } else if (rotation == ScreenRotation::ROTATION_270) {
                pixelmap->rotate(static_cast<int32_t>(270)); // 270 degrees
            }
            RS_LOGI("RSSurfaceCaptureTask::Run: PixelmapRotation: %{public}d", static_cast<int32_t>(rotation));
        }
    }
    // To get dump image
    // execute "param set rosen.dumpsurfacetype.enabled 3 && setenforce 0"
    RSBaseRenderUtil::WritePixelMapToPng(*pixelmap);
    callback->OnSurfaceCapture(nodeId_, pixelmap.get());
    return true;
}

int32_t RSSurfaceCaptureTask::ScreenCorrection(ScreenRotation screenCorrection)
{
    if (screenCorrection == ScreenRotation::ROTATION_90) {
        return -90; // -90 degrees
    } else if (screenCorrection == ScreenRotation::ROTATION_180) {
        return -180; // -180 degrees
    } else if (screenCorrection == ScreenRotation::ROTATION_270) {
        return -270; // -270 degrees
    }
    return 0;
}

std::unique_ptr<Media::PixelMap> RSSurfaceCaptureTask::CreatePixelMapBySurfaceNode(
    std::shared_ptr<RSSurfaceRenderNode> node, bool isUniRender)
{
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreatePixelMapBySurfaceNode: node == nullptr");
        return nullptr;
    }
    if (!isUniRender && node->GetBuffer() == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreatePixelMapBySurfaceNode: node GetBuffer == nullptr");
        return nullptr;
    }
    const auto& property = node->GetRenderProperties();
    int pixmapWidth = property.GetBoundsWidth();
    int pixmapHeight = property.GetBoundsHeight();
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixmapWidth * scaleX_);
    opts.size.height = ceil(pixmapHeight * scaleY_);
    RS_LOGD("RSSurfaceCaptureTask::CreatePixelMapBySurfaceNode: NodeId:[%{public}" PRIu64 "],"
        " origin pixelmap width is [%{public}u], height is [%{public}u],"
        " created pixelmap width is [%{public}u], height is [%{public}u],"
        " the scale is scaleY:[%{public}f], scaleY:[%{public}f],"
        " isProcOnBgThread_[%{public}d]",
        node->GetId(), pixmapWidth, pixmapHeight, opts.size.width, opts.size.height,
        scaleX_, scaleY_, isProcOnBgThread_);
    return Media::PixelMap::Create(opts);
}

std::unique_ptr<Media::PixelMap> RSSurfaceCaptureTask::CreatePixelMapByDisplayNode(
    std::shared_ptr<RSDisplayRenderNode> node, bool isUniRender, bool hasSecurityOrSkipLayer)
{
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreatePixelMapByDisplayNode: node is nullptr");
        return nullptr;
    }
    uint64_t screenId = node->GetScreenId();
    RSScreenModeInfo screenModeInfo;
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSSurfaceCaptureTask::CreatePixelMapByDisplayNode: screenManager is nullptr!");
        return nullptr;
    }
    auto screenInfo = screenManager->QueryScreenInfo(screenId);
    screenCorrection_ = screenManager->GetScreenCorrection(screenId);
    uint32_t pixmapWidth = screenInfo.width;
    uint32_t pixmapHeight = screenInfo.height;
    if (!isUniRender) {
        auto rotation = node->GetRotation();
        if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
            std::swap(pixmapWidth, pixmapHeight);
        }
    }
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixmapWidth * scaleX_);
    opts.size.height = ceil(pixmapHeight * scaleY_);
    RS_LOGI("RSSurfaceCaptureTask::CreatePixelMapByDisplayNode: NodeId:[%{public}" PRIu64 "],"
        " origin pixelmap width is [%{public}u], height is [%{public}u],"
        " created pixelmap width is [%{public}u], height is [%{public}u],"
        " the scale is scaleY:[%{public}f], scaleY:[%{public}f],"
        " isProcOnBgThread_[%{public}d]",
        node->GetId(), pixmapWidth, pixmapHeight, opts.size.width, opts.size.height,
        scaleX_, scaleY_, isProcOnBgThread_);
    return Media::PixelMap::Create(opts);
}

bool CopyDataToPixelMap(std::shared_ptr<Drawing::Image> img, const std::unique_ptr<Media::PixelMap>& pixelmap)
{
    auto size = pixelmap->GetRowBytes() * pixelmap->GetHeight();
#ifdef ROSEN_OHOS
    int fd = AshmemCreate("RSSurfaceCapture Data", size);
    if (fd < 0) {
        RS_LOGE("RSSurfaceCaptureTask::CopyDataToPixelMap AshmemCreate fd < 0");
        return false;
    }
    int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
    if (result < 0) {
        RS_LOGE("RSSurfaceCaptureTask::CopyDataToPixelMap AshmemSetProt error");
        ::close(fd);
        return false;
    }
    void* ptr = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    auto data = static_cast<uint8_t*>(ptr);
    if (ptr == MAP_FAILED || ptr == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CopyDataToPixelMap data is nullptr");
        ::close(fd);
        return false;
    }

    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    Drawing::Bitmap bitmap;
    bitmap.Build(pixelmap->GetWidth(), pixelmap->GetHeight(), format);
    bitmap.SetPixels(data);
    if (!img->ReadPixels(bitmap, 0, 0)) {
        RS_LOGE("RSSurfaceCaptureTask::CopyDataToPixelMap readPixels failed");
        ::close(fd);
        return false;
    }
    void* fdPtr = new int32_t();
    *static_cast<int32_t*>(fdPtr) = fd;
    pixelmap->SetPixelsAddr(data, fdPtr, size, Media::AllocatorType::SHARE_MEM_ALLOC, nullptr);

#else
    auto data = (uint8_t *)malloc(size);
    if (data == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CopyDataToPixelMap data is nullptr");
        return false;
    }

    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    Drawing::Bitmap bitmap;
    bitmap.Build(pixelmap->GetWidth(), pixelmap->GetHeight(), format);
    bitmap.SetPixels(data);
    if (!img->ReadPixels(bitmap, 0, 0)) {
        RS_LOGE("RSSurfaceCaptureTask::CopyDataToPixelMap readPixels failed");
        free(data);
        data = nullptr;
        return false;
    }

    pixelmap->SetPixelsAddr(data, nullptr, size, Media::AllocatorType::HEAP_ALLOC, nullptr);
#endif
    return true;
}

std::shared_ptr<Drawing::Surface> RSSurfaceCaptureTask::CreateSurface(const std::unique_ptr<Media::PixelMap>& pixelmap)
{
    if (pixelmap == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: pixelmap == nullptr");
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: address == nullptr");
        return nullptr;
    }
    Drawing::ImageInfo info = Drawing::ImageInfo{pixelmap->GetWidth(), pixelmap->GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL};
    if (isProcOnBgThread_) {
#if defined(RS_ENABLE_UNI_RENDER)
        auto gpuContext = RSBackgroundThread::Instance().GetShareGPUContext().get();
        return Drawing::Surface::MakeRenderTarget(gpuContext, false, info);
#endif
    } else {
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
#if defined(NEW_RENDER_CONTEXT)
            auto drawingContext = RSMainThread::Instance()->GetRenderEngine()->GetDrawingContext();
            if (drawingContext == nullptr) {
                RS_LOGE("RSSurfaceCaptureTask::CreateSurface: renderContext is nullptr");
                return nullptr;
            }
            drawingContext->SetUpDrawingContext();
            return Drawing::Surface::MakeRenderTarget(drawingContext->GetDrawingContext, fasle, info);
#else
            auto renderContext = RSMainThread::Instance()->GetRenderEngine()->GetRenderContext();
            if (renderContext == nullptr) {
                RS_LOGE("RSSurfaceCaptureTask::CreateSurface: renderContext is nullptr");
                return nullptr;
            }
            renderContext->SetUpGpuContext(nullptr);
            return Drawing::Surface::MakeRenderTarget(renderContext->GetDrGPUContext(), false, info);
#endif
        }
#endif
#ifdef RS_ENABLE_VK
        if (RSSystemProperties::IsUseVulkan()) {
            return Drawing::Surface::MakeRenderTarget(
                RSMainThread::Instance()->GetRenderEngine()->GetSkContext().get(), false, info);
        }
#endif
    }
    return Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
}

bool RSSurfaceCaptureTask::FindSecurityOrSkipLayer()
{
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    bool hasSecurityOrSkipLayer = false;
    nodeMap.TraverseSurfaceNodes([this,
        &hasSecurityOrSkipLayer](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)
        mutable {
        if (surfaceNode == nullptr || !surfaceNode->IsOnTheTree()) {
            return;
        }
        if (surfaceNode->GetSecurityLayer() || surfaceNode->GetSkipLayer()) {
            hasSecurityOrSkipLayer = true;
            return;
        }
    });
    return hasSecurityOrSkipLayer;
}

RSSurfaceCaptureVisitor::RSSurfaceCaptureVisitor(float scaleX, float scaleY, bool isUniRender)
    : scaleX_(scaleX), scaleY_(scaleY), isUniRender_(isUniRender)
{
    renderEngine_ = RSMainThread::Instance()->GetRenderEngine();
    isUIFirst_ = RSMainThread::Instance()->IsUIFirstOn();
}

void RSSurfaceCaptureVisitor::SetSurface(Drawing::Surface* surface)
{
    if (surface == nullptr) {
        RS_LOGE("RSSurfaceCaptureVisitor::SetSurface: surface == nullptr");
        return;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(surface);
    canvas_->Scale(scaleX_, scaleY_);
    canvas_->SetDisableFilterCache(true);
}

void RSSurfaceCaptureVisitor::ProcessChildren(RSRenderNode &node)
{
    if (DrawBlurInCache(node)) {
        return;
    }
    for (auto& child : *node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
}

void RSSurfaceCaptureVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode &node)
{
    RS_TRACE_NAME("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode:" +
        std::to_string(node.GetId()));
    RS_LOGD("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode child size:[%{public}d] total",
        node.GetChildrenCount());

    // Mirror Display is unable to snapshot.
    if (node.IsMirrorDisplay()) {
        RS_LOGW("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode: \
            Mirror Display(id:[%{public}" PRIu64 "])", node.GetId());
        return;
    }

    if (canvas_ == nullptr) {
        RS_LOGE("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode: Canvas is null!");
        return;
    }

    if (IsUniRender()) {
        bool isCurtainScreenOn = RSMainThread::Instance()->IsCurtainScreenOn();
        if (hasSecurityOrSkipLayer_ || isCurtainScreenOn) {
            RS_LOGD("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode: \
                process RSDisplayRenderNode(id:[%{public}" PRIu64 "]) Not using UniRender buffer.", node.GetId());

            // Adding matrix affine transformation logic
            auto geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
            if (geoPtr != nullptr) {
                canvas_->ConcatMatrix(geoPtr->GetMatrix());
            }

            ProcessChildren(node);
            DrawWatermarkIfNeed(node);
        } else {
            if (node.GetBuffer() == nullptr) {
                RS_LOGE("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode: buffer is null!");
                return;
            }

            RS_LOGD("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode: \
                process RSDisplayRenderNode(id:[%{public}" PRIu64 "]) using UniRender buffer.", node.GetId());

            FindHardwareEnabledNodes();

            if (hardwareEnabledNodes_.size() != 0) {
                AdjustZOrderAndDrawSurfaceNode(hardwareEnabledNodes_);
            }

            auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false);

            // Screen capture considering color inversion
            ColorFilterMode colorFilterMode = renderEngine_->GetColorFilterMode();
            if (colorFilterMode >= ColorFilterMode::INVERT_COLOR_ENABLE_MODE &&
                colorFilterMode <= ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE) {
                RS_LOGD("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode: \
                    SetColorFilterModeToPaint mode:%{public}d.", static_cast<int32_t>(colorFilterMode));
                RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, params.paint);
            }

            // To get dump image
            // execute "param set rosen.dumpsurfacetype.enabled 4 && setenforce 0 && param set rosen.afbc.enabled 0"
            RSBaseRenderUtil::WriteSurfaceBufferToPng(params.buffer);
            renderEngine_->DrawDisplayNodeWithParams(*canvas_, node, params);

            if (hardwareEnabledTopNodes_.size() != 0) {
                AdjustZOrderAndDrawSurfaceNode(hardwareEnabledTopNodes_);
            }
        }
    } else {
        ProcessChildren(node);
    }
}

void RSSurfaceCaptureVisitor::ProcessEffectRenderNode(RSEffectRenderNode& node)
{
    if (!IsUniRender()) {
        return;
    }
    if (!node.ShouldPaint()) {
        RS_LOGD("RSSurfaceCaptureVisitor::ProcessEffectRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        RS_LOGE("RSSurfaceCaptureVisitor::ProcessEffectRenderNode, canvas is nullptr");
        return;
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessChildren(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

void RSSurfaceCaptureVisitor::FindHardwareEnabledNodes()
{
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes([this](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
        if (surfaceNode == nullptr || !surfaceNode->IsOnTheTree()) {
            return;
        }
        if (surfaceNode->IsLastFrameHardwareEnabled() && surfaceNode->GetBuffer() != nullptr) {
            // To get dump image
            // execute "param set rosen.dumpsurfacetype.enabled 4 && setenforce 0 && param set rosen.afbc.enabled 0"
            auto buffer = surfaceNode->GetBuffer();
            RSBaseRenderUtil::WriteSurfaceBufferToPng(buffer, surfaceNode->GetId());
            if (surfaceNode->IsHardwareEnabledTopSurface()) {
                // surfaceNode which should be drawn above displayNode like pointer window
                hardwareEnabledTopNodes_.emplace_back(surfaceNode);
            } else {
                // surfaceNode which should be drawn below displayNode
                hardwareEnabledNodes_.emplace_back(surfaceNode);
            }
        }
    });
}

void RSSurfaceCaptureVisitor::AdjustZOrderAndDrawSurfaceNode(std::vector<std::shared_ptr<RSSurfaceRenderNode>>& nodes)
{
    if (!RSSystemProperties::GetHardwareComposerEnabled()) {
        RS_LOGW("RSSurfaceCaptureVisitor::AdjustZOrderAndDrawSurfaceNode: \
            HardwareComposer is not enabled.");
        return;
    }

    // sort the surfaceNodes by ZOrder
    std::stable_sort(
        nodes.begin(), nodes.end(), [](const auto& first, const auto& second) -> bool {
            return first->GetGlobalZOrder() < second->GetGlobalZOrder();
        });

    // draw hardware-composition nodes
    for (auto& surfaceNode : nodes) {
        if (surfaceNode->IsLastFrameHardwareEnabled() && surfaceNode->GetBuffer() != nullptr) {
            Drawing::AutoCanvasRestore acr(*canvas_, true);
            CaptureSurfaceInDisplayWithUni(*surfaceNode);
        }
    }
}

void RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni(RSSurfaceRenderNode& node)
{
    const auto& property = node.GetRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());
    if (!geoPtr) {
        RS_LOGE("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni node:%{public}" PRIu64 ", get geoPtr failed",
            node.GetId());
        return;
    }
    if (canvas_ == nullptr) {
        RS_LOGE("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni: canvas_ is nullptr.");
        return;
    }

    bool isSelfDrawingSurface = node.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE;
    if (isSelfDrawingSurface) {
        canvas_->Save();
    }

    if (node.IsAppWindow() || node.IsLeashWindow()) {
        // When CaptureSingleSurfaceNodeWithUni, we should consider scale factor of canvas_ and
        // child nodes (self-drawing surfaceNode) of AppWindow should use relative coordinates
        // which is the node relative to the upper-left corner of the window.
        // So we have to get the invert matrix of AppWindow here and apply it to canvas_
        // when we calculate the position of self-drawing surfaceNode.
        captureMatrix_.Set(Drawing::Matrix::Index::SCALE_X, scaleX_);
        captureMatrix_.Set(Drawing::Matrix::Index::SCALE_Y, scaleY_);
        Drawing::Matrix invertMatrix;
        if (geoPtr->GetAbsMatrix().Invert(invertMatrix)) {
            captureMatrix_.PreConcat(invertMatrix);
        }
    } else if (!node.IsStartingWindow()) {
        canvas_->SetMatrix(captureMatrix_);
        canvas_->ConcatMatrix(geoPtr->GetAbsMatrix());
    }

    const RectF absBounds = {0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()};
    RRect absClipRRect = RRect(absBounds, property.GetCornerRadius());
    if (isSelfDrawingSurface) {
        RSPropertiesPainter::DrawShadow(property, *canvas_, &absClipRRect);
        RSPropertiesPainter::DrawOutline(property, *canvas_);
    }
    canvas_->Save();
    if (isSelfDrawingSurface && !property.GetCornerRadius().IsZero()) {
        canvas_->ClipRoundRect(
            RSPropertiesPainter::RRect2DrawingRRect(absClipRRect), Drawing::ClipOp::INTERSECT, true);
    } else {
        canvas_->ClipRect(Drawing::Rect(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()),
            Drawing::ClipOp::INTERSECT, true);
    }
    if (node.GetSecurityLayer() || node.GetSkipLayer()) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni: \
            process RSSurfaceRenderNode(id:[%{public}" PRIu64 "]) clear white since it is security layer.",
            node.GetId());
        canvas_->Clear(Drawing::Color::COLOR_WHITE);
        canvas_->Restore(); // restore clipRect
        canvas_->Restore(); // restore translate and concat
        return;
    }

    if (isSelfDrawingSurface) {
        RSPropertiesPainter::DrawBackground(property, *canvas_);
        RSPropertiesPainter::DrawMask(property, *canvas_);
        RSPropertiesPainter::DrawFilter(property, *canvas_, FilterType::BACKGROUND_FILTER);
    } else {
        auto backgroundColor = static_cast<Drawing::ColorQuad>(property.GetBackgroundColor().AsArgbInt());
        if (Drawing::Color::ColorQuadGetA(backgroundColor) != Drawing::Color::COLOR_TRANSPARENT) {
            canvas_->DrawColor(backgroundColor);
        }
    }
    canvas_->Restore();
    if (!node.IsAppWindow() && node.GetBuffer() != nullptr) {
        auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false);
        renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
    }
    if (isSelfDrawingSurface) {
        RSPropertiesPainter::DrawFilter(property, *canvas_, FilterType::FOREGROUND_FILTER);
    }

    if (isSelfDrawingSurface) {
        canvas_->Restore();
    }

    auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.GetFirstLevelNode());
    if (firstLevelNode && !firstLevelNode->GetHasSecurityLayer() && !firstLevelNode->GetHasSkipLayer() &&
        isUIFirst_ && RSUniRenderUtil::HandleCaptureNode(node, *canvas_)) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni: \
            process RSSurfaceRenderNode [%{public}s, %{public}" PRIu64 "] use cache texture.",
            node.GetName().c_str(), node.GetId());
        return;
    }

    ProcessChildren(node);
}

void RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithUni(RSSurfaceRenderNode& node)
{
    if (node.GetSecurityLayer() || node.GetSkipLayer()) {
        RS_LOGD("RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithUni: \
            process RSSurfaceRenderNode(id:[%{public}" PRIu64 "]) paused since it is security layer.",
            node.GetId());
        return;
    }

    const auto& property = node.GetRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());
    if (geoPtr) {
        canvas_->SetMatrix(geoPtr->GetAbsMatrix());
    }

    if (!node.GetHasSecurityLayer() && !node.GetHasSkipLayer() &&
        isUIFirst_ && RSUniRenderUtil::HandleSubThreadNode(node, *canvas_)) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithUni: \
            process RSSurfaceRenderNode [%{public}s, %{public}" PRIu64 "] use cache texture.",
            node.GetName().c_str(), node.GetId());
        return;
    }

    bool isSelfDrawingSurface = node.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE;
    if (isSelfDrawingSurface) {
        canvas_->Save();
    }

    const RectF absBounds = {0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()};
    RRect absClipRRect = RRect(absBounds, property.GetCornerRadius());
    RSPropertiesPainter::DrawShadow(property, *canvas_, &absClipRRect);
    RSPropertiesPainter::DrawOutline(property, *canvas_);
    if (!property.GetCornerRadius().IsZero()) {
        canvas_->ClipRoundRect(
            RSPropertiesPainter::RRect2DrawingRRect(absClipRRect), Drawing::ClipOp::INTERSECT, true);
    } else {
        canvas_->ClipRect(Drawing::Rect(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()),
            Drawing::ClipOp::INTERSECT, false);
    }
    auto backgroundColor = static_cast<Drawing::ColorQuad>(property.GetBackgroundColor().AsArgbInt());
    if (Drawing::Color::ColorQuadGetA(backgroundColor) != Drawing::Color::COLOR_TRANSPARENT) {
        canvas_->DrawColor(backgroundColor);
    }
    RSPropertiesPainter::DrawMask(property, *canvas_);

    RSPropertiesPainter::DrawFilter(property, *canvas_, FilterType::BACKGROUND_FILTER);
    if (isSelfDrawingSurface) {
        canvas_->Restore();
    }

    if (!node.IsAppWindow() && node.GetBuffer() != nullptr) {
        auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false);
        renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
    }

    ProcessChildren(node);
    RSPropertiesPainter::DrawFilter(property, *canvas_, FilterType::FOREGROUND_FILTER);
}

void RSSurfaceCaptureVisitor::ProcessSurfaceRenderNodeWithUni(RSSurfaceRenderNode &node)
{
    auto geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        RS_LOGW("RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode node:%{public}" PRIu64 ", get geoPtr failed",
            node.GetId());
        return;
    }

    Drawing::AutoCanvasRestore acr(*canvas_, true);
    canvas_->MultiplyAlpha(node.GetRenderProperties().GetAlpha());
    if (isDisplayNode_) {
        CaptureSurfaceInDisplayWithUni(node);
    } else {
        CaptureSingleSurfaceNodeWithUni(node);
    }
}

void RSSurfaceCaptureVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    if (!RSUniRenderJudgement::IsUniRender()) {
        return;
    }
    if (!node.ShouldPaint()) {
        RS_LOGD("RSSurfaceCaptureVisitor::ProcessRootRenderNode, no need process");
        return;
    }

    if (!canvas_) {
        RS_LOGE("RSSurfaceCaptureVisitor::ProcessRootRenderNode, canvas is nullptr");
        return;
    }

    Drawing::AutoCanvasRestore acr(*canvas_.get(), true);
    ProcessCanvasRenderNode(node);
}

void RSSurfaceCaptureVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (!IsUniRender()) {
        return;
    }
    if (!node.ShouldPaint()) {
        RS_LOGD("RSSurfaceCaptureVisitor::ProcessCanvasRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        RS_LOGE("RSSurfaceCaptureVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }

    const auto& property = node.GetRenderProperties();
    if (property.IsSpherizeValid()) {
        DrawSpherize(node);
        return;
    }

    if (node.GetCacheType() != CacheType::NONE) {
        ProcessCacheFilterRects(node);
    }

    DrawChildRenderNode(node);
}

void RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithoutUni(RSSurfaceRenderNode& node)
{
    Drawing::Matrix translateMatrix;
    auto parentPtr = node.GetParent().lock();
    if (parentPtr != nullptr && parentPtr->IsInstanceOf<RSSurfaceRenderNode>()) {
        // calculate the offset from this node's parent, and perform translate.
        auto parentNode = std::static_pointer_cast<RSSurfaceRenderNode>(parentPtr);
        const float parentNodeTranslateX = parentNode->GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_X);
        const float parentNodeTranslateY = parentNode->GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_Y);
        const float thisNodetranslateX = node.GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_X);
        const float thisNodetranslateY = node.GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_Y);
        translateMatrix.PreTranslate(
            thisNodetranslateX - parentNodeTranslateX, thisNodetranslateY - parentNodeTranslateY);
    }
    if (node.GetSecurityLayer() || node.GetSkipLayer()) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithoutUni: \
            process RSSurfaceRenderNode(id:[%{public}" PRIu64 "]) clear white since it is security layer.",
            node.GetId());
        Drawing::AutoCanvasRestore acr(*canvas_.get(), true);
        canvas_->ConcatMatrix(translateMatrix);
        canvas_->Clear(Drawing::Color::COLOR_WHITE);
        return;
    }

    if (node.GetChildrenCount() > 0) {
        canvas_->ConcatMatrix(translateMatrix);
        const auto saveCnt = canvas_->Save();
        ProcessChildren(node);
        canvas_->RestoreToCount(saveCnt);
        if (node.GetBuffer() != nullptr) {
            // in node's local coordinate.
            auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, true, false, false, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
    } else {
        Drawing::AutoCanvasRestore acr(*canvas_.get(), true);
        canvas_->ConcatMatrix(translateMatrix);
        if (node.GetBuffer() != nullptr) {
            // in node's local coordinate.
            auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, true, false, false, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
    }
}

void RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithoutUni(RSSurfaceRenderNode& node)
{
    if (node.GetSecurityLayer() || node.GetSkipLayer()) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithoutUni: \
            process RSSurfaceRenderNode(id:[%{public}" PRIu64 "]) paused since it is security layer.",
            node.GetId());
        return;
    }
    ProcessChildren(node);
    if (node.GetBuffer() != nullptr) {
        // in display's coordinate.
        auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, false, false, false, false);
        renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
    }
}

void RSSurfaceCaptureVisitor::ProcessSurfaceRenderNodeWithoutUni(RSSurfaceRenderNode& node)
{
    if (isDisplayNode_) {
        CaptureSurfaceInDisplayWithoutUni(node);
    } else {
        CaptureSingleSurfaceNodeWithoutUni(node);
    }
}

void RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode &node)
{
    RS_TRACE_NAME("RSSurfaceCaptureVisitor::Process:" + node.GetName());

    if (canvas_ == nullptr) {
        RS_LOGE("RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }

    if (!node.ShouldPaint()) {
        RS_LOGD("RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode node: %{public}" PRIu64 " invisible", node.GetId());
        return;
    }

    // execute security layer in each case, ignore display snapshot and set it white for surface snapshot
    if (IsUniRender()) {
        ProcessSurfaceRenderNodeWithUni(node);
    } else {
        ProcessSurfaceRenderNodeWithoutUni(node);
    }
}

void RSSurfaceCaptureVisitor::DrawWatermarkIfNeed(RSDisplayRenderNode& node)
{
    if (RSMainThread::Instance()->GetWatermarkFlag()) {
        sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
        auto screenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
        auto image = RSMainThread::Instance()->GetWatermarkImg();
        if (image == nullptr) {
            return;
        }

        auto srcRect = Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight());
        auto dstRect = Drawing::Rect(0, 0, screenInfo.width, screenInfo.height);
        Drawing::Brush rectBrush;
        canvas_->AttachBrush(rectBrush);
        canvas_->DrawImageRect(*image, srcRect, dstRect, Drawing::SamplingOptions(),
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        canvas_->DetachBrush();
    }
}

void RSSurfaceCaptureVisitor::DrawSpherize(RSRenderNode& node)
{
    if (!canvas_) {
        RS_LOGE("RSSurfaceCaptureVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
    node.ProcessTransitionBeforeChildren(*canvas_);
    RSPropertiesPainter::DrawSpherize(
        node.GetRenderProperties(), *canvas_, node.GetCompletedCacheSurface(UNI_MAIN_THREAD_INDEX, true));
    node.ProcessTransitionAfterChildren(*canvas_);
}

void RSSurfaceCaptureVisitor::DrawChildRenderNode(RSRenderNode& node)
{
    if (!canvas_) {
        RS_LOGE("RSSurfaceCaptureVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
    CacheType cacheType = node.GetCacheType();
    switch (cacheType) {
        case CacheType::NONE: {
            node.ProcessRenderBeforeChildren(*canvas_);
            if (node.GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
                auto canvasDrawingNode = node.ReinterpretCastTo<RSCanvasDrawingRenderNode>();
                Drawing::Bitmap bitmap = canvasDrawingNode->GetBitmap();
                canvas_->DrawBitmap(bitmap, 0, 0);
            } else {
                node.ProcessRenderContents(*canvas_);
            }
            ProcessChildren(node);
            node.ProcessRenderAfterChildren(*canvas_);
            break;
        }
        case CacheType::CONTENT: {
            node.ProcessRenderBeforeChildren(*canvas_);
            node.DrawCacheSurface(*canvas_, UNI_MAIN_THREAD_INDEX, false);
            node.ProcessRenderAfterChildren(*canvas_);
            break;
        }
        case CacheType::ANIMATE_PROPERTY: {
            node.ProcessTransitionBeforeChildren(*canvas_);
            node.DrawCacheSurface(*canvas_, UNI_MAIN_THREAD_INDEX, false);
            node.ProcessTransitionAfterChildren(*canvas_);
            break;
        }
        default:
            break;
    }
}

void RSSurfaceCaptureVisitor::ProcessCacheFilterRects(RSRenderNode& node)
{
    std::unordered_map<NodeId, std::unordered_set<NodeId>> cacheFilterRectsMap = {};
    node.GetFilterRectsInCache(cacheFilterRectsMap);
    if (cacheFilterRectsMap[node.GetId()].empty()) {
        return;
    }
    curCacheFilterRects_ = cacheFilterRectsMap[node.GetId()];
    RS_TRACE_NAME_FMT("Draw cache with blur [%llu]", node.GetId());
    Drawing::AutoCanvasRestore arc(*canvas_, true);
    auto nodeType = node.GetCacheType();
    node.SetCacheType(CacheType::NONE);
    DrawChildRenderNode(node);
    node.SetCacheType(nodeType);
    curCacheFilterRects_ = {};
}

bool RSSurfaceCaptureVisitor::DrawBlurInCache(RSRenderNode& node)
{
    if (LIKELY(curCacheFilterRects_.empty())) {
        return false;
    }
    if (curCacheFilterRects_.count(node.GetId())) {
        // draw filter before drawing cachedSurface
        curCacheFilterRects_.erase(node.GetId());
        if (curCacheFilterRects_.empty() || !node.ChildHasVisibleFilter()) {
            // no filter to draw, return
            return true;
        }
    } else if (!node.ChildHasVisibleFilter()) {
        // no filter to draw, return
        return true;
    }
    return false;
}
} // namespace Rosen
} // namespace OHOS
