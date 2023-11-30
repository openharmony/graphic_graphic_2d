/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_canvas_drawing_render_node.h"

#include "include/core/SkCanvas.h"
#include "src/image/SkImage_Base.h"
#ifdef NEW_SKIA
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#endif

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/sk_resource_manager.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "visitor/rs_node_visitor.h"
#include "rs_trace.h"
namespace OHOS {
namespace Rosen {
RSCanvasDrawingRenderNode::RSCanvasDrawingRenderNode(NodeId id, const std::weak_ptr<RSContext>& context)
    : RSCanvasRenderNode(id, context)
{}

RSCanvasDrawingRenderNode::~RSCanvasDrawingRenderNode()
{
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#ifndef USE_ROSEN_DRAWING
    if (preThreadInfo_.second && skSurface_) {
        preThreadInfo_.second(std::move(skSurface_));
    }
#else
    if (preThreadInfo_.second && surface_) {
        preThreadInfo_.second(std::move(surface_));
    }
#endif
#endif
}

#ifndef USE_ROSEN_DRAWING
void RSCanvasDrawingRenderNode::ProcessRenderContents(RSPaintFilterCanvas& canvas)
{
    int width = 0;
    int height = 0;
    if (!GetSizeFromDrawCmdModifiers(width, height)) {
        return;
    }
    if (IsNeedResetSurface(width, height)) {
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
        if (preThreadInfo_.second && skSurface_) {
            preThreadInfo_.second(std::move(skSurface_));
        }
        preThreadInfo_ = curThreadInfo_;
#endif
        if (!ResetSurface(width, height, canvas)) {
            return;
        }
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    } else if ((isGpuSurface_) && (preThreadInfo_.first != curThreadInfo_.first)) {
        auto preMatrix = canvas_->getTotalMatrix();
        auto preSurface = skSurface_;
        if (!ResetSurface(width, height, canvas)) {
            return;
        }
        auto image = preSurface->makeImageSnapshot();
        if (!image) {
            return;
        }
        GrSurfaceOrigin origin = kBottomLeft_GrSurfaceOrigin;
        auto sharedBackendTexture = image->getBackendTexture(false, &origin);
        if (!sharedBackendTexture.isValid()) {
            RS_LOGE("RSCanvasDrawingRenderNode::ProcessRenderContents sharedBackendTexture is nullptr");
            return;
        }
        auto sharedTexture = SkImage::MakeFromTexture(
            canvas.recordingContext(), sharedBackendTexture, origin, image->colorType(), image->alphaType(), nullptr);
        if (sharedTexture == nullptr) {
            RS_LOGE("RSCanvasDrawingRenderNode::ProcessRenderContents sharedTexture is nullptr");
            return;
        }
        if (RSSystemProperties::GetRecordingEnabled()) {
            if (sharedTexture->isTextureBacked()) {
                RS_LOGI("RSCanvasDrawingRenderNode::ProcessRenderContents sharedTexture from texture to raster image");
                sharedTexture = sharedTexture->makeRasterImage();
            }
        }
        canvas_->drawImage(sharedTexture, 0.f, 0.f);
        if (preThreadInfo_.second && preSurface) {
            preThreadInfo_.second(std::move(preSurface));
        }
        preThreadInfo_ = curThreadInfo_;
        canvas_->setMatrix(preMatrix);
    }
#else
    }
#endif

    RSModifierContext context = { GetMutableRenderProperties(), canvas_.get() };
    ApplyDrawCmdModifier(context, RSModifierType::CONTENT_STYLE);
    ApplyDrawCmdModifier(context, RSModifierType::OVERLAY_STYLE);

    SkMatrix mat;
    if (RSPropertiesPainter::GetGravityMatrix(
        GetRenderProperties().GetFrameGravity(), GetRenderProperties().GetFrameRect(), width, height, mat)) {
        canvas.concat(mat);
    }
    skImage_ = skSurface_->makeImageSnapshot();
    if (skImage_) {
        SKResourceManager::Instance().HoldResource(skImage_);
    }
#ifdef NEW_SKIA
    auto samplingOptions = SkSamplingOptions(SkFilterMode::kLinear, SkMipmapMode::kLinear);
    canvas.drawImage(skImage_, 0.f, 0.f, samplingOptions, nullptr);
#else
    canvas.drawImage(skImage_, 0.f, 0.f, nullptr);
#endif
}

#else // USE_ROSEN_DRAWING
void RSCanvasDrawingRenderNode::ProcessRenderContents(RSPaintFilterCanvas& canvas)
{
    int width = 0;
    int height = 0;
    RS_TRACE_NAME_FMT("RSCanvasDrawingRenderNode::ProcessRenderContents %llu", GetId());
    if (!GetSizeFromDrawCmdModifiers(width, height)) {
        return;
    }

    if (IsNeedResetSurface(width, height)) {
        if (preThreadInfo_.second && surface_) {
            preThreadInfo_.second(std::move(surface_));
        }
        if (!ResetSurface(width, height, canvas)) {
            return;
        }
        preThreadInfo_ = curThreadInfo_;
    } else if (preThreadInfo_.first != curThreadInfo_.first) {
        auto preMatrix = canvas_->GetTotalMatrix();
        auto preSurface = surface_;
        if (!ResetSurface(width, height, canvas)) {
            return;
        }
#if defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)
        auto image = preSurface->GetImageSnapshot();
        if (!image) {
            return;
        }
        Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
        auto sharedBackendTexture = image->GetBackendTexture(false, &origin);
        if (!sharedBackendTexture.IsValid()) {
            RS_LOGE("RSCanvasDrawingRenderNode::ProcessRenderContents sharedBackendTexture is nullptr");
            return;
        }
        auto newImage = std::make_shared<Drawing::Image>();
        Drawing::BitmapFormat info = Drawing::BitmapFormat { Drawing::COLORTYPE_RGBA_8888,
            Drawing::ALPHATYPE_PREMUL };
        bool ret = newImage->BuildFromTexture(*canvas.GetGPUContext(), sharedBackendTexture.GetTextureInfo(),
            origin, info, nullptr);
        if (!ret) {
            RS_LOGE("RSCanvasDrawingRenderNode::ProcessRenderContents BuildFromTexture failed");
            return;
        }
        canvas_->DrawImage(*newImage, 0.f, 0.f, Drawing::SamplingOptions());
#else
        if (auto image = preSurface->GetImageSnapshot()) {
            canvas_->DrawImage(*image, 0.f, 0.f, Drawing::SamplingOptions());
        }
#endif
        if (preThreadInfo_.second && preSurface) {
            preThreadInfo_.second(std::move(preSurface));
        }
        preThreadInfo_ = curThreadInfo_;
        canvas_->SetMatrix(preMatrix);
    }

    RSModifierContext context = { GetMutableRenderProperties(), canvas_.get() };
    ApplyDrawCmdModifier(context, RSModifierType::CONTENT_STYLE);
    ApplyDrawCmdModifier(context, RSModifierType::OVERLAY_STYLE);

    Rosen::Drawing::Matrix mat;
    if (RSPropertiesPainter::GetGravityMatrix(
        GetRenderProperties().GetFrameGravity(), GetRenderProperties().GetFrameRect(), width, height, mat)) {
        canvas.ConcatMatrix(mat);
    }
    auto image_ = surface_->GetImageSnapshot();

    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::LINEAR);
    canvas.DrawImage(*image_, 0.f, 0.f, samplingOptions);
}
#endif

#ifndef USE_ROSEN_DRAWING
bool RSCanvasDrawingRenderNode::ResetSurface(int width, int height, RSPaintFilterCanvas& canvas)
{
    SkImageInfo info = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType, kPremul_SkAlphaType);

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#ifdef NEW_SKIA
    auto grContext = canvas.recordingContext();
#else
    auto grContext = canvas.getGrContext();
#endif
    isGpuSurface_ = true;
    if (grContext == nullptr) {
        RS_LOGD("RSCanvasDrawingRenderNode::ResetSurface: GrContext is nullptr");
        isGpuSurface_ = false;
        skSurface_ = SkSurface::MakeRaster(info);
    } else {
        skSurface_ = SkSurface::MakeRenderTarget(grContext, SkBudgeted::kNo, info);
        if (!skSurface_) {
            isGpuSurface_ = false;
            skSurface_ = SkSurface::MakeRaster(info);
        }
    }
#else
    skSurface_ = SkSurface::MakeRaster(info);
#endif
    if (!skSurface_) {
        RS_LOGE("RSCanvasDrawingRenderNode::ResetSurface SkSurface is nullptr");
        return false;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(skSurface_.get());
    return true;
}
#else
bool RSCanvasDrawingRenderNode::ResetSurface(int width, int height, RSPaintFilterCanvas& canvas)
{
    Drawing::ImageInfo info =
        Drawing::ImageInfo{ Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };

#if (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE)
    auto gpuContext = canvas.GetGPUContext();
    if (gpuContext == nullptr) {
        RS_LOGD("RSCanvasDrawingRenderNode::ResetSurface: gpuContext is nullptr");
        surface_ = Drawing::Surface::MakeRaster(info);
    } else {
        surface_ = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, info);
        if (!surface_) {
            surface_ = Drawing::Surface::MakeRaster(info);
        }
    }
#else
    surface_ = Drawing::Surface::MakeRaster(info);
#endif
    if (!surface_) {
        RS_LOGE("RSCanvasDrawingRenderNode::ResetSurface surface is nullptr");
        return false;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(surface_.get());
    return true;
}
#endif

void RSCanvasDrawingRenderNode::ApplyDrawCmdModifier(RSModifierContext& context, RSModifierType type)
{
    std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
    auto it = drawCmdLists_.find(type);
    if (it == drawCmdLists_.end() || it->second.empty()) {
        return;
    }
    for (const auto& drawCmdList : it->second) {
        drawCmdList->Playback(*context.canvas_);
        drawCmdList->ClearOp();
    }
    it->second.clear();
}

#ifndef USE_ROSEN_DRAWING
SkBitmap RSCanvasDrawingRenderNode::GetBitmap(const uint32_t tid)
{
    SkBitmap bitmap;
    std::lock_guard<std::mutex> lock(drawingMutex_);
    if (!skImage_) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: skImage_ is nullptr");
        return bitmap;
    }
    if (GetTid() != tid) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: skImage_ used by multi threads");
        return bitmap;
    }
    if (!skImage_->asLegacyBitmap(&bitmap)) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: asLegacyBitmap failed");
    }
    return bitmap;
}

bool RSCanvasDrawingRenderNode::GetPixelmap(
    const std::shared_ptr<Media::PixelMap> pixelmap, const SkRect* rect, const uint32_t tid)
{
    std::lock_guard<std::mutex> lock(drawingMutex_);
    if (!pixelmap || !rect) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: pixelmap or rect is nullptr");
        return false;
    }

    if (!skSurface_) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: SkSurface is nullptr");
        return false;
    }

    if (GetTid() != tid) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: SkSurface used by multi threads");
        return false;
    }

    sk_sp<SkImage> skImage = skSurface_->makeImageSnapshot();
    if (skImage == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: makeImageSnapshot failed");
        return false;
    }

    SkImageInfo info =
        SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(), kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    if (!skImage->readPixels(info, pixelmap->GetWritablePixels(), pixelmap->GetRowBytes(), rect->x(), rect->y())) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: readPixels failed");
        return false;
    }
    return true;
}

#else
Drawing::Bitmap RSCanvasDrawingRenderNode::GetBitmap()
{
    Drawing::Bitmap bitmap;
    std::lock_guard<std::mutex> lock(mutex_);
    if (!image_) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: image_ is nullptr");
        return bitmap;
    }
    if (!image_->AsLegacyBitmap(bitmap)) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: asLegacyBitmap failed");
    }
    return bitmap;
}

bool RSCanvasDrawingRenderNode::GetPixelmap(const std::shared_ptr<Media::PixelMap> pixelmap, const Drawing::Rect* rect)
{
    if (!pixelmap) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: pixelmap is nullptr");
        return false;
    }

    if (!surface_) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: surface is nullptr");
        return false;
    }

    auto image = surface_->GetImageSnapshot();
    if (image == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: GetImageSnapshot failed");
        return false;
    }

    Drawing::BitmapFormat info =
        Drawing::BitmapFormat{ Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    auto bitmap = std::make_shared<Drawing::Bitmap>();
    bitmap->Build(pixelmap->GetWidth(), pixelmap->GetHeight(), info);
    if (!image->ReadPixels(*bitmap.get(), rect->GetLeft(), rect->GetTop())) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: readPixels failed");
        return false;
    }
    return true;
}
#endif

bool RSCanvasDrawingRenderNode::GetSizeFromDrawCmdModifiers(int& width, int& height)
{
    auto it = drawCmdModifiers_.find(RSModifierType::CONTENT_STYLE);
    if (it == drawCmdModifiers_.end() || it->second.empty()) {
        return false;
    }
    for (const auto& modifier : it->second) {
        auto prop = modifier->GetProperty();
#ifndef USE_ROSEN_DRAWING
        if (auto cmd = std::static_pointer_cast<RSRenderProperty<DrawCmdListPtr>>(prop)->Get()) {
#else
        if (auto cmd = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(prop)->Get()) {
#endif
            width = std::max(width, cmd->GetWidth());
            height = std::max(height, cmd->GetHeight());
        }
    }
    if (width <= 0 || height <= 0) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetSizeFromDrawCmdModifiers: The width or height of the canvas is less "
                "than or equal to 0");
        return false;
    }
    return true;
}

bool RSCanvasDrawingRenderNode::IsNeedResetSurface(const int& width, const int& height) const
{
#ifndef USE_ROSEN_DRAWING
    if (!skSurface_) {
#else
    if (!surface_ || !surface_->GetCanvas()) {
#endif
        return true;
    } else {
        // There is no need to reapply the buffer during the animation, only if the size of the DrawCmdList set by ArkUI
        // changes. When the component sets the margin and padding properties, ArkUI does not set the DrawCmdList size,
        // in which case the size of the SkSurface should be the same as the size of Render Properties Bounds. In other
        // cases, ArkUI sets the DrawCmdList to the same size as the Render Properties Bounds.
#ifndef USE_ROSEN_DRAWING
        return (skSurface_->width() != width || skSurface_->height() != height) &&
               (static_cast<int>(GetRenderProperties().GetBoundsWidth()) != skSurface_->width() ||
                   static_cast<int>(GetRenderProperties().GetBoundsHeight()) != skSurface_->height());
#else
        auto canvas = surface_->GetCanvas();
        return (canvas->GetWidth() != width || canvas->GetHeight() != height) &&
            (static_cast<int>(GetRenderProperties().GetBoundsWidth()) != canvas->GetWidth() ||
            static_cast<int>(GetRenderProperties().GetBoundsHeight()) != canvas->GetHeight());
#endif
    }
}

void RSCanvasDrawingRenderNode::AddDirtyType(RSModifierType type)
{
#ifndef USE_ROSEN_DRAWING
    dirtyTypes_.emplace(type);
#else
    dirtyTypes_.set(static_cast<int>(type), true);
#endif
    for (auto drawCmdModifier : drawCmdModifiers_) {
        if (drawCmdModifier.second.empty()) {
            continue;
        }
        for (const auto& modifier : drawCmdModifier.second) {
            if (modifier == nullptr) {
                continue;
            }
            auto prop = modifier->GetProperty();
            if (prop == nullptr) {
                continue;
            }
#ifndef USE_ROSEN_DRAWING
            if (auto cmd = std::static_pointer_cast<RSRenderProperty<DrawCmdListPtr>>(prop)->Get()) {
#else
            if (auto cmd = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(prop)->Get()) {
#endif
                std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
                drawCmdLists_[drawCmdModifier.first].emplace_back(cmd);
            }
        }
    }
    if (!IsOnTheTree()) {
        ClearOp();
    }
}

void RSCanvasDrawingRenderNode::ClearOp()
{
    std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
    drawCmdLists_.clear();
}
} // namespace Rosen
} // namespace OHOS