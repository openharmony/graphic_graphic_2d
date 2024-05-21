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

#include "pipeline/rs_surface_capture_task_parallel.h"

#include <memory>
#include <sys/mman.h>

#include "draw/surface.h"
#include "draw/color.h"
#include "rs_trace.h"

#include "common/rs_background_thread.h"
#include "common/rs_obj_abs_geometry.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_divided_render_util.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_service_connection.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_uni_render_util.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_surface.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_skia_filter.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/rs_screen_mode_info.h"

namespace OHOS {
namespace Rosen {
bool RSSurfaceCaptureTaskParallel::Run(sptr<RSISurfaceCaptureCallback> callback)
{
    if (ROSEN_EQ(scaleX_, 0.f) || ROSEN_EQ(scaleY_, 0.f) || scaleX_ < 0.f || scaleY_ < 0.f) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::Run: SurfaceCapture scale is invalid.");
        return false;
    }
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(nodeId_);
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::Run: node is nullptr");
        return false;
    }
    std::unique_ptr<Media::PixelMap> pixelmap;
    std::shared_ptr<DrawableV2::RSRenderNodeDrawable> surfaceNodeDrawable = nullptr;
    std::shared_ptr<DrawableV2::RSRenderNodeDrawable> displayNodeDrawable = nullptr;
    visitor_ = std::make_shared<RSSurfaceCaptureVisitor>(scaleX_, scaleY_, RSUniRenderJudgement::IsUniRender());
    if (auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>()) {
        // Determine whether cache can be used
        auto curNode = surfaceNode;
        auto parentNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(surfaceNode->GetParent().lock());
        if (parentNode && parentNode->IsLeashWindow()) {
            curNode = parentNode;
        }
        auto curNodeParams = static_cast<RSSurfaceRenderParams*>(curNode->GetRenderParams().get());
        if (curNodeParams && curNodeParams->GetUifirstNodeEnableParam() == MultiThreadCacheType::LEASH_WINDOW) {
            surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
                DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(curNode));
        } else {
            surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
                DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        }
        pixelmap = CreatePixelMapBySurfaceNode(curNode, visitor_->IsUniRender());
        visitor_->IsDisplayNode(false);
    } else if (auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>()) {
        visitor_->SetHasingSecurityOrSkipOrProtectedLayer(FindSecurityOrSkipOrProtectedLayer());
        pixelmap = CreatePixelMapByDisplayNode(displayNode, visitor_->IsUniRender(),
            visitor_->GetHasingSecurityOrSkipOrProtectedLayer());
        visitor_->IsDisplayNode(true);
        displayNodeDrawable = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(displayNode));
    } else {
        RS_LOGE("RSSurfaceCaptureTaskParallel::Run: Invalid RSRenderNodeType!");
        return false;
    }
    if (pixelmap == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::Run: pixelmap == nullptr!");
        return false;
    }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    auto renderContext = RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext();
    auto grContext = renderContext != nullptr ? renderContext->GetDrGPUContext() : nullptr;
    RSTagTracker tagTracker(grContext, node->GetId(), RSTagTracker::TAGTYPE::TAG_CAPTURE);
#endif
    auto surface = CreateSurface(pixelmap);
    if (surface == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::Run: surface is nullptr!");
        return false;
    }
    if (rsSurfaceCaptureType_ == RsSurfaceCaptureType::RS_SURFACE_CAPTURE_TYPE_RENDER_THREAD_VISITOR) {
        visitor_->SetSurface(surface.get());
        node->Process(visitor_);
    } else {
        RSPaintFilterCanvas canvas(surface.get());
        canvas.Scale(scaleX_, scaleY_);
        canvas.SetDisableFilterCache(true);
        if (surfaceNodeDrawable) {
            RSUniRenderThread::SetCaptureParam(CaptureParam(true, true, false, scaleX_, scaleY_, true));
            surfaceNodeDrawable->OnCapture(canvas);
        } else if (displayNodeDrawable) {
            RSUniRenderThread::SetCaptureParam(CaptureParam(true, false, false, scaleX_, scaleY_));
            displayNodeDrawable->OnCapture(canvas);
        } else {
            RS_LOGE("RSSurfaceCaptureTaskParallel::Run: Invalid RSRenderNodeDrawable!");
            return false;
        }
        RSUniRenderThread::ResetCaptureParam();
    }
#if (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE)
#ifdef RS_ENABLE_UNI_RENDER
    if (RSSystemProperties::GetSnapshotWithDMAEnabled() && !isProcOnBgThread_) {
        RSUniRenderUtil::OptimizedFlushAndSubmit(surface, grContext);
        Drawing::BackendTexture backendTexture = surface->GetBackendTexture();
        if (!backendTexture.IsValid()) {
            RS_LOGE("RSSurfaceCaptureTaskParallel: SkiaSurface bind Image failed: BackendTexture is invalid");
            return false;
        }
        auto wrapper = std::make_shared<std::tuple<std::unique_ptr<Media::PixelMap>>>();
        std::get<0>(*wrapper) = std::move(pixelmap);
        auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>();
        auto rotation = (displayNode != nullptr) ? displayNode->GetScreenRotation()
            : ScreenRotation::INVALID_SCREEN_ROTATION;
        bool ableRotation = ((displayNode != nullptr) && visitor_->IsUniRender());
        auto id = nodeId_;
        auto screenCorrection = ScreenCorrection(screenCorrection_);
        auto wrapperSf = std::make_shared<std::tuple<std::shared_ptr<Drawing::Surface>>>();
        std::get<0>(*wrapperSf) = std::move(surface);
        std::function<void()> copytask = [wrapper, callback, backendTexture, wrapperSf,
                                             ableRotation, rotation, id, screenCorrection]() -> void {
            RS_TRACE_NAME("copy and send capture");
            if (!backendTexture.IsValid()) {
                RS_LOGE("RSSurfaceCaptureTaskParallel: Surface bind Image failed: BackendTexture is invalid");
                callback->OnSurfaceCapture(id, nullptr);
                RSUniRenderUtil::ClearNodeCacheSurface(
                    std::move(std::get<0>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
                return;
            }
            auto pixelmap = std::move(std::get<0>(*wrapper));
            if (pixelmap == nullptr) {
                RS_LOGE("RSSurfaceCaptureTaskParallel: pixelmap == nullptr");
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
                RS_LOGE("RSSurfaceCaptureTaskParallel::Run MakeRenderTarget fail.");
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
                RS_LOGE("RSSurfaceCaptureTaskParallel::Run CopyDataToPixelMap failed");
                callback->OnSurfaceCapture(id, nullptr);
                RSUniRenderUtil::ClearNodeCacheSurface(
                    std::move(std::get<0>(*wrapperSf)), nullptr, UNI_MAIN_THREAD_INDEX, 0);
                return;
            }
            if (ableRotation) {
                if (screenCorrection != 0) {
                    pixelmap->rotate(screenCorrection);
                    RS_LOGI("RSSurfaceCaptureTaskParallel::Run: ScreenCorrection: %{public}d", screenCorrection);
                }

                if (rotation == ScreenRotation::ROTATION_90) {
                    pixelmap->rotate(static_cast<int32_t>(90)); // 90 degrees
                } else if (rotation == ScreenRotation::ROTATION_180) {
                    pixelmap->rotate(static_cast<int32_t>(180)); // 180 degrees
                } else if (rotation == ScreenRotation::ROTATION_270) {
                    pixelmap->rotate(static_cast<int32_t>(270)); // 270 degrees
                }
                RS_LOGI("RSSurfaceCaptureTaskParallel::Run: PixelmapRotation: %{public}d",
                    static_cast<int32_t>(rotation));
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
            RS_LOGE("RSSurfaceCaptureTaskParallel::Run: img is nullptr");
            return false;
        }
        if (!CopyDataToPixelMap(img, pixelmap)) {
            RS_LOGE("RSSurfaceCaptureTaskParallel::Run: CopyDataToPixelMap failed");
            return false;
        }
    }
#else
    std::shared_ptr<Drawing::Image> img(surface.get()->GetImageSnapshot());
    if (!img) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::Run: img is nullptr");
        return false;
    }
    if (!CopyDataToPixelMap(img, pixelmap)) {
            RS_LOGE("RSSurfaceCaptureTaskParallel::Run: CopyDataToPixelMap failed");
            return false;
    }
#endif // RS_ENABLE_UNI_RENDER
#endif
    if (auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>()) {
        if (visitor_->IsUniRender()) {
            if (screenCorrection_ != ScreenRotation::ROTATION_0) {
                int32_t angle = ScreenCorrection(screenCorrection_);
                pixelmap->rotate(angle);
                RS_LOGI("RSSurfaceCaptureTaskParallel::Run: ScreenCorrection: %{public}d", angle);
            }

            auto rotation = displayNode->GetScreenRotation();
            if (rotation == ScreenRotation::ROTATION_90) {
                pixelmap->rotate(static_cast<int32_t>(90)); // 90 degrees
            } else if (rotation == ScreenRotation::ROTATION_180) {
                pixelmap->rotate(static_cast<int32_t>(180)); // 180 degrees
            } else if (rotation == ScreenRotation::ROTATION_270) {
                pixelmap->rotate(static_cast<int32_t>(270)); // 270 degrees
            }
            RS_LOGI("RSSurfaceCaptureTaskParallel::Run: PixelmapRotation: %{public}d", static_cast<int32_t>(rotation));
        }
    }
    // To get dump image
    // execute "param set rosen.dumpsurfacetype.enabled 3 && setenforce 0"
    RSBaseRenderUtil::WritePixelMapToPng(*pixelmap);
    callback->OnSurfaceCapture(nodeId_, pixelmap.get());
    return true;
}

int32_t RSSurfaceCaptureTaskParallel::ScreenCorrection(ScreenRotation screenCorrection)
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

std::unique_ptr<Media::PixelMap> RSSurfaceCaptureTaskParallel::CreatePixelMapBySurfaceNode(
    std::shared_ptr<RSSurfaceRenderNode> node, bool isUniRender)
{
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::CreatePixelMapBySurfaceNode: node == nullptr");
        return nullptr;
    }
    if (!isUniRender && node->GetBuffer() == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::CreatePixelMapBySurfaceNode: node GetBuffer == nullptr");
        return nullptr;
    }
    int pixmapWidth = node->GetRenderProperties().GetBoundsWidth();
    int pixmapHeight = node->GetRenderProperties().GetBoundsHeight();
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixmapWidth * scaleX_);
    opts.size.height = ceil(pixmapHeight * scaleY_);
    RS_LOGD("RSSurfaceCaptureTaskParallel::CreatePixelMapBySurfaceNode: NodeId:[%{public}" PRIu64 "],"
        " origin pixelmap width is [%{public}u], height is [%{public}u],"
        " created pixelmap width is [%{public}u], height is [%{public}u],"
        " the scale is scaleY:[%{public}f], scaleY:[%{public}f],"
        " isProcOnBgThread_[%{public}d]",
        node->GetId(), pixmapWidth, pixmapHeight, opts.size.width, opts.size.height,
        scaleX_, scaleY_, isProcOnBgThread_);
    return Media::PixelMap::Create(opts);
}

std::unique_ptr<Media::PixelMap> RSSurfaceCaptureTaskParallel::CreatePixelMapByDisplayNode(
    std::shared_ptr<RSDisplayRenderNode> node, bool isUniRender, bool hasSecurityOrSkipLayer)
{
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::CreatePixelMapByDisplayNode: node is nullptr");
        return nullptr;
    }
    uint64_t screenId = node->GetScreenId();
    RSScreenModeInfo screenModeInfo;
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::CreatePixelMapByDisplayNode: screenManager is nullptr!");
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
    RS_LOGI("RSSurfaceCaptureTaskParallel::CreatePixelMapByDisplayNode: NodeId:[%{public}" PRIu64 "],"
        " origin pixelmap width is [%{public}u], height is [%{public}u],"
        " created pixelmap width is [%{public}u], height is [%{public}u],"
        " the scale is scaleY:[%{public}f], scaleY:[%{public}f],"
        " isProcOnBgThread_[%{public}d]",
        node->GetId(), pixmapWidth, pixmapHeight, opts.size.width, opts.size.height,
        scaleX_, scaleY_, isProcOnBgThread_);
    return Media::PixelMap::Create(opts);
}

std::shared_ptr<Drawing::Surface> RSSurfaceCaptureTaskParallel::CreateSurface(
    const std::unique_ptr<Media::PixelMap>& pixelmap)
{
    if (pixelmap == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::CreateSurface: pixelmap == nullptr");
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        RS_LOGE("RSSurfaceCaptureTaskParallel::CreateSurface: address == nullptr");
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
            auto renderContext = RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext();
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
            return Drawing::Surface::MakeRenderTarget(
                RSUniRenderThread::Instance().GetRenderEngine()->GetSkContext().get(), false, info);
        }
#endif
    }
    return Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
}

bool RSSurfaceCaptureTaskParallel::FindSecurityOrSkipOrProtectedLayer()
{
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    bool hasSecurityOrSkipOrProtectedLayer = false;
    nodeMap.TraverseSurfaceNodes([this,
        &hasSecurityOrSkipOrProtectedLayer](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)
        mutable {
        if (surfaceNode == nullptr || !surfaceNode->IsOnTheTree()) {
            return;
        }
        if (surfaceNode->GetSecurityLayer() || surfaceNode->GetSkipLayer() || surfaceNode->GetProtectedLayer()) {
            hasSecurityOrSkipOrProtectedLayer = true;
            return;
        }
    });
    return hasSecurityOrSkipOrProtectedLayer;
}

} // namespace Rosen
} // namespace OHOS
