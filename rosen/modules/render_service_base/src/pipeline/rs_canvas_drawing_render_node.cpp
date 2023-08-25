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
#ifdef NEW_SKIA
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#endif

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
RSCanvasDrawingRenderNode::RSCanvasDrawingRenderNode(NodeId id, const std::weak_ptr<RSContext>& context)
    : RSCanvasRenderNode(id, context)
{}

RSCanvasDrawingRenderNode::~RSCanvasDrawingRenderNode()
{
#ifndef USE_ROSEN_DRAWING
    if (preThreadInfo_.second && skSurface_) {
        preThreadInfo_.second(std::move(skSurface_));
    }
#else
    if (preThreadInfo_.second && surface_) {
        preThreadInfo_.second(std::move(surface_));
    }
#endif
}

void RSCanvasDrawingRenderNode::OnTreeStateChanged()
{
    RSRenderNode::OnTreeStateChanged();
#if !defined(USE_ROSEN_DRAWING) && defined(RS_ENABLE_GL) && defined(NEW_SKIA)
    if (IsOnTheTree()) {
        return;
    }
    
    if (gpuResourceClearFun_ && canvas_) {
        gpuResourceClearFun_(canvas_.get(), curThreadInfo_.first);
    }
#endif
}

#if !defined(USE_ROSEN_DRAWING) && defined(RS_ENABLE_GL) && defined(NEW_SKIA)
void RSCanvasDrawingRenderNode::SetGpuResourceClearFunc(GpuResourceClearFun fun)
{
    gpuResourceClearFun_ = fun;
}
#endif

void RSCanvasDrawingRenderNode::ProcessRenderContents(RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    int width = 0;
    int height = 0;
    if (!GetSizeFromDrawCmdModifiers(width, height)) {
        return;
    }

    if (IsNeedResetSurface(width, height)) {
        if (preThreadInfo_.second && skSurface_) {
            preThreadInfo_.second(std::move(skSurface_));
        }
        if (!ResetSurface(width, height, canvas)) {
            return;
        }
        preThreadInfo_ = curThreadInfo_;
    } else if (preThreadInfo_.first != curThreadInfo_.first) {
        auto preMatrix = canvas_->getTotalMatrix();
        auto preSurface = skSurface_;
        if (!ResetSurface(width, height, canvas)) {
            return;
        }
#if (defined NEW_SKIA) && (defined RS_ENABLE_GL)
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
        canvas_->drawImage(sharedTexture, 0.f, 0.f);
#else
        if (auto image = preSurface->makeImageSnapshot()) {
            canvas_->drawImage(image, 0.f, 0.f);
        }
#endif
        if (preThreadInfo_.second && preSurface) {
            preThreadInfo_.second(std::move(preSurface));
        }
        preThreadInfo_ = curThreadInfo_;
        canvas_->setMatrix(preMatrix);
    }

    RSModifierContext context = { GetMutableRenderProperties(), canvas_.get() };
    ApplyDrawCmdModifier(context, RSModifierType::CONTENT_STYLE);
    ApplyDrawCmdModifier(context, RSModifierType::OVERLAY_STYLE);

    SkMatrix mat;
    if (RSPropertiesPainter::GetGravityMatrix(
        GetRenderProperties().GetFrameGravity(), GetRenderProperties().GetFrameRect(), width, height, mat)) {
        canvas.concat(mat);
    }
    auto image = skSurface_->makeImageSnapshot();
#ifdef NEW_SKIA
    auto samplingOptions = SkSamplingOptions(SkFilterMode::kLinear, SkMipmapMode::kLinear);
    canvas.drawImage(image, 0.f, 0.f, samplingOptions, nullptr);
#else
    canvas.drawImage(image, 0.f, 0.f, nullptr);
#endif
#endif
}

#ifndef USE_ROSEN_DRAWING
bool RSCanvasDrawingRenderNode::ResetSurface(int width, int height, RSPaintFilterCanvas& canvas)
{
    SkImageInfo info = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType, kPremul_SkAlphaType);

#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
#ifdef NEW_SKIA
    auto grContext = canvas.recordingContext();
#else
    auto grContext = canvas.getGrContext();
#endif
    if (grContext == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNode::ProcessRenderContents: GrContext is nullptr");
        return false;
    }
    skSurface_ = SkSurface::MakeRenderTarget(grContext, SkBudgeted::kNo, info);
    if (!skSurface_ && width > 0 && height > 0) {
        skSurface_ = SkSurface::MakeRaster(info);
    }
#else
    skSurface_ = SkSurface::MakeRaster(info);
#endif
    if (!skSurface_) {
        RS_LOGE("RSCanvasDrawingRenderNode::ResetSurface SkSurface is nullptr");
        return false;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(skSurface_.get());
    return skSurface_ != nullptr;
}
#else
bool RSCanvasDrawingRenderNode::ResetSurface(int width, int height, RSPaintFilterCanvas& canvas)
{
    Drawing::BitmapFormat info = Drawing::BitmapFormat{ Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    auto bitmap = std::make_shared<Drawing::Bitmap>();
    bitmap->Build(width, height, info);

#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    auto gpuContext = canvas.GetGPUContext();
    if (gpuContext == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNode::ProcessRenderContents: GpuContext is nullptr");
        return false;
    }
    auto image = std::make_shared<Drawing::Image>();
    if (!image->BuildFromBitmap(*gpuContext, *bitmap)) {
        RS_LOGE("RSCanvasDrawingRenderNode::ResetSurface Drawing::Image is nullptr");
        return false;
    }
    auto surface = std::make_shared<Drawing::Surface>();
    if (!surface->Bind(*image)) {
        if (!surface->Bind(*bitmap)) {
            RS_LOGE("RSCanvasDrawingRenderNode::ResetSurface Drawing::Surface is nullptr");
            return false;
        }
        bitmap_ = bitmap;
    }
#else
    auto surface = std::make_shared<Drawing::Surface>();
    if (!surface->Bind(*bitmap)) {
        RS_LOGE("RSCanvasDrawingRenderNode::ResetSurface Drawing::Surface is nullptr");
        return false;
    }
    bitmap_ = bitmap;
#endif
    surface_ = surface;
    canvas_ = std::make_unique<RSPaintFilterCanvas>(surface_.get());
    return true;
}
#endif

void RSCanvasDrawingRenderNode::ApplyDrawCmdModifier(RSModifierContext& context, RSModifierType type)
{
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
SkBitmap RSCanvasDrawingRenderNode::GetBitmap()
{
    SkBitmap bitmap;
    if (skSurface_ == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: SkSurface is nullptr");
        return bitmap;
    }
    sk_sp<SkImage> image = skSurface_->makeImageSnapshot();
    if (image == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: SkImage is nullptr");
        return bitmap;
    }
    if (!image->asLegacyBitmap(&bitmap)) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: asLegacyBitmap failed");
    }
    return bitmap;
}
#else
Drawing::Bitmap RSCanvasDrawingRenderNode::GetBitmap()
{
    Drawing::Bitmap bitmap;
    if (surface_ == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: Drawing::Surface is nullptr");
        return bitmap;
    }
    std::shared_ptr<Drawing::Image> image = surface_->GetImageSnapshot();
    if (image == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: Drawing::Image is nullptr");
        return bitmap;
    }
    RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: Drawing asLegacyBitmap failed");
    return bitmap;
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
    dirtyTypes_.emplace(type);
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
            if (auto cmd = std::static_pointer_cast<RSRenderProperty<DrawCmdListPtr>>(prop)->Get()) {
                drawCmdLists_[drawCmdModifier.first].emplace_back(cmd);
            }
        }
    }
}
} // namespace Rosen
} // namespace OHOS