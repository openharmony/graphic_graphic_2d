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

#include "pipeline/rs_canvas_drawing_render_node_content.h"

#include <memory>

#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {

RSCanvasDrawingRenderNodeContent::~RSCanvasDrawingRenderNodeContent()
{
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (preThreadInfo_.second && surface_) {
        preThreadInfo_.second(std::move(surface_));
    }
#endif
}

bool RSCanvasDrawingRenderNodeContent::InitSurface(int width, int height, RSPaintFilterCanvas& canvas)
{
    if (IsNeedResetSurface()) {
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
        if (preThreadInfo_.second && surface_) {
            preThreadInfo_.second(std::move(surface_));
        }
        preThreadInfo_ = curThreadInfo_;
#endif
        if (!ResetSurface(width, height, canvas)) {
            return false;
        }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    } else if ((isGpuSurface_) && (preThreadInfo_.first != curThreadInfo_.first)) {
        // if (!ResetSurfaceWithTexture(width, height, canvas)) {
        //     return false;
        // }
    }
#else
    }
#endif
    if (!surface_) {
        return false;
    }

    return true;
}

std::shared_ptr<RSPaintFilterCanvas> RSCanvasDrawingRenderNodeContent::GetCanvas()
{
    return canvas_;
}

std::shared_ptr<Drawing::Image> RSCanvasDrawingRenderNodeContent::Flush()
{
    image_ = surface_->GetImageSnapshot();
    return image_;
}

Drawing::Bitmap RSCanvasDrawingRenderNodeContent::GetBitmap(const uint64_t tid)
{
    Drawing::Bitmap bitmap;
    // std::lock_guard<std::mutex> lock(drawingMutex_);
    if (!image_) {
        RS_LOGE("Failed to get bitmap, image is null!");
        return bitmap;
    }
    if (GetTid() != tid) {
        RS_LOGE("Failed to get bitmap: image is used by multi threads");
        return bitmap;
    }
    if (!image_->AsLegacyBitmap(bitmap)) {
        RS_LOGE("Failed to get bitmap, asLegacyBitmap failed");
    }
    return bitmap;
}

bool RSCanvasDrawingRenderNodeContent::GetPixelmap(const std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, const uint64_t tid, std::shared_ptr<Drawing::DrawCmdList> drawCmdList)
{
    return false;
}

bool RSCanvasDrawingRenderNodeContent::IsNeedResetSurface() const
{
    if (!surface_ || !surface_->GetCanvas()) {
        return true;
    }
    return false;
}

bool RSCanvasDrawingRenderNodeContent::ResetSurface(int width, int height, RSPaintFilterCanvas& canvas)
{
    Drawing::ImageInfo info =
        Drawing::ImageInfo { width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    auto gpuContext = canvas.GetGPUContext();
    isGpuSurface_ = true;
    if (gpuContext == nullptr) {
        RS_LOGD("RSCanvasDrawingRenderNodeContent::ResetSurface: gpuContext is nullptr");
        isGpuSurface_ = false;
        surface_ = Drawing::Surface::MakeRaster(info);
    } else {
        surface_ = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, info);
        // if (!surface_) {
        //     isGpuSurface_ = false;
        //     surface_ = Drawing::Surface::MakeRaster(info);
        //     if (!surface_) {
        //         RS_LOGE("RSCanvasDrawingRenderNode::ResetSurface surface is nullptr");
        //         return false;
        //     }
        //     recordingCanvas_ = std::make_shared<ExtendRecordingCanvas>(width, height, false);
        //     canvas_ = std::shared_ptr<RSPaintFilterCanvas>(recordingCanvas_.get());
        //     return true;
        // }
    }
#else
    surface_ = Drawing::Surface::MakeRaster(info);
#endif
    if (!surface_) {
        RS_LOGE("RSCanvasDrawingRenderNodeContent::ResetSurface surface is nullptr");
        return false;
    }
    canvas_ = std::make_shared<RSPaintFilterCanvas>(surface_.get());
    return true;
}

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
bool RSCanvasDrawingRenderNodeContent::ResetSurfaceWithTexture(int width, int height, RSPaintFilterCanvas& canvas)
{
    auto preMatrix = canvas_->GetTotalMatrix();
    auto preSurface = surface_;
    if (!ResetSurface(width, height, canvas)) {
        return false;
    }
    auto image = preSurface->GetImageSnapshot();
    if (!image) {
        return false;
    }
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
    auto sharedBackendTexture = image->GetBackendTexture(false, &origin);
    if (!sharedBackendTexture.IsValid()) {
        RS_LOGE("RSCanvasDrawingRenderNode::ResetSurfaceWithTexture sharedBackendTexture is nullptr");
        return false;
    }

    Drawing::BitmapFormat bitmapFormat = { image->GetColorType(), image->GetAlphaType() };
    auto sharedTexture = std::make_shared<Drawing::Image>();
    if (!sharedTexture->BuildFromTexture(
            *canvas.GetGPUContext(), sharedBackendTexture.GetTextureInfo(), origin, bitmapFormat, nullptr)) {
        RS_LOGE("RSCanvasDrawingRenderNode::ResetSurfaceWithTexture sharedTexture is nullptr");
        return false;
    }
    if (RSSystemProperties::GetRecordingEnabled()) {
        if (sharedTexture->IsTextureBacked()) {
            RS_LOGI("RSCanvasDrawingRenderNode::ResetSurfaceWithTexture sharedTexture from texture to raster image");
            sharedTexture = sharedTexture->MakeRasterImage();
        }
    }
    canvas_->DrawImage(*sharedTexture, 0.f, 0.f, Drawing::SamplingOptions());
    if (preThreadInfo_.second && preSurface) {
        preThreadInfo_.second(std::move(preSurface));
    }
    preThreadInfo_ = curThreadInfo_;
    canvas_->SetMatrix(preMatrix);
    canvas_->Flush();
    return true;
}
#endif

} // namespace Rosen
} // namespace OHOS
