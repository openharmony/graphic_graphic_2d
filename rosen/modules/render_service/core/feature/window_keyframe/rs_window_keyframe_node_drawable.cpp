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

#include "rs_window_keyframe_node_drawable.h"

#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS::Rosen::DrawableV2 {
RSWindowKeyFrameNodeDrawable::Registrar RSWindowKeyFrameNodeDrawable::instance_;

RSWindowKeyFrameNodeDrawable::RSWindowKeyFrameNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node))
{
}

RSRenderNodeDrawable::Ptr RSWindowKeyFrameNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new (std::nothrow) RSWindowKeyFrameNodeDrawable(std::move(node));
}

bool RSWindowKeyFrameNodeDrawable::CheckAndDrawToOffscreen(Drawing::Canvas& canvas,
    RSCanvasRenderNodeDrawable& linkedNodeDrawable)
{
    const auto& linkedNodeParams = linkedNodeDrawable.GetRenderParams();
    if (UNLIKELY(linkedNodeParams == nullptr)) {
        return false;
    }

    auto drawable = linkedNodeParams->GetWindowKeyFrameNodeDrawable().lock();
    if (LIKELY(drawable == nullptr)) {
        return false;
    }

    if (UNLIKELY(drawable->GetDrawableType() != RSRenderNodeDrawableType::WINDOW_KEYFRAME_NODE_DRAWABLE)) {
        RS_LOGE("CheckAndDrawToOffscreen: Drawable in RenderParams is not WindowKeyFrameNode type?");
        return false;
    }

    auto keyframeDrawable = std::static_pointer_cast<RSWindowKeyFrameNodeDrawable>(drawable);
    return keyframeDrawable->OnLinkedNodeDraw(canvas, linkedNodeDrawable, *linkedNodeParams);
}

void RSWindowKeyFrameNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    SetDrawSkipType(DrawSkipType::NONE);
    if (!ShouldPaint()) {
        SetDrawSkipType(DrawSkipType::SHOULD_NOT_PAINT);
        return;
    }

    const auto& params = GetRenderParams();
    Drawing::Rect bounds = params->GetBounds();
    RS_TRACE_NAME_FMT("WindowKeyFrameDraw LinkedNode:(%.0fx%.0f), CurNode:(%.0fx%.0f) alpha:%.2f, freeze:%d",
        linkedNodeBounds_.GetWidth(), linkedNodeBounds_.GetHeight(),
        bounds.GetWidth(), bounds.GetHeight(), params->GetAlpha(), params->GetRSFreezeFlag());

    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    RSAutoCanvasRestore acr(paintFilterCanvas);
    params->ApplyAlphaAndMatrixToCanvas(*paintFilterCanvas);
    DrawLinkedNodeOffscreenBuffer(*paintFilterCanvas, bounds);
}

void RSWindowKeyFrameNodeDrawable::DrawLinkedNodeOffscreenBuffer(
    RSPaintFilterCanvas& canvas, const Drawing::Rect& bounds)
{
    Drawing::Brush paint;
    paint.SetAntiAlias(true);
    canvas.AttachBrush(paint);
    if (UNLIKELY(bounds.IsEmpty() || preCachedOffscreenImg_ == nullptr)) {
        RS_LOGE("RSWindowKeyFrameNodeDrawable::DrawLinkedNodeOffscreenBuffer preCachedOffscreenImg size is invalid");
        SetDrawSkipType(DrawSkipType::SHOULD_NOT_PAINT);
        return;
    }

    // keep aspect ratio for scale
    Drawing::Rect srcRect;
    srcRect.left_ = 0;
    srcRect.top_ = 0;
    int imgW = preCachedOffscreenImg_->GetWidth();
    int imgH = preCachedOffscreenImg_->GetHeight();
    if (imgW * bounds.GetHeight() >= bounds.GetWidth() * imgH) {
        srcRect.right_ = imgH * bounds.GetWidth() / bounds.GetHeight();
        srcRect.bottom_ = imgH;
    } else {
        srcRect.right_ = imgW;
        srcRect.bottom_ = imgW * bounds.GetHeight() / bounds.GetWidth();
    }

    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
    canvas.DrawImageRect(*preCachedOffscreenImg_, srcRect, bounds, samplingOptions,
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
    canvas.DetachBrush();
}

bool RSWindowKeyFrameNodeDrawable::OnLinkedNodeDraw(Drawing::Canvas& canvas,
    RSCanvasRenderNodeDrawable& linkedNodeDrawable, const RSRenderParams& linkedNodeParams)
{
    const auto& params = GetRenderParams();
    const auto& uniParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (UNLIKELY(params == nullptr || uniParams == nullptr)) {
        RS_LOGE("RSWindowKeyFrameNodeDrawable::OnLinkedRootNodeDraw render params is nullptr");
        return false;
    }

    auto curCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    auto curSurface = curCanvas->GetSurface();
    if (UNLIKELY(curSurface == nullptr)) {
        RS_LOGE("RSWindowKeyFrameNodeDrawable::OnLinkedRootNodeDraw surface is nullptr");
        return false;
    }

    float scale = 1.0f;
    std::shared_ptr<Drawing::Surface> cacheSurface;
    const auto& bounds = params->GetBounds();
    const auto& linkedNodebounds = linkedNodeParams.GetBounds();
    if (!linkedNodebounds.IsEmpty() && !bounds.IsEmpty()) {
        scale = std::max(bounds.GetWidth() / linkedNodebounds.GetWidth(),
            bounds.GetHeight() / linkedNodebounds.GetHeight());
        cacheSurface = curSurface->MakeSurface(static_cast<int>(std::round(linkedNodebounds.GetWidth() * scale)),
            static_cast<int>(std::round(linkedNodebounds.GetHeight() * scale)));
    } else {
        cacheSurface = curSurface->MakeSurface(linkedNodebounds.GetWidth(), linkedNodebounds.GetHeight());
    }

    if (UNLIKELY(cacheSurface == nullptr)) {
        RS_LOGE("RSWindowKeyFrameNodeDrawable::OnLinkedRootNodeDraw make surface failed");
        return false;
    }

    auto cacheCanvas = std::make_shared<RSPaintFilterCanvas>(cacheSurface.get());
    if (UNLIKELY(cacheCanvas == nullptr)) {
        RS_LOGE("RSWindowKeyFrameNodeDrawable::OnLinkedRootNodeDraw make canvas failed");
        return false;
    }

    // Copy HDR properties into offscreen canvas
    cacheCanvas->CopyHDRConfiguration(*curCanvas);

    // Copy current canvas properties into offscreen canvas
    cacheCanvas->CopyConfigurationToOffscreenCanvas(*curCanvas);
    cacheCanvas->SetDisableFilterCache(true);
    cacheCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
    bool isOpDropped = uniParams->IsOpDropped();
    uniParams->SetOpDropped(false); // temporarily close partial render
    cacheCanvas->Scale(scale, scale);
    linkedNodeDrawable.RSCanvasRenderNodeDrawable::OnDraw(*cacheCanvas);
    uniParams->SetOpDropped(isOpDropped);
    linkedNodeBounds_ = linkedNodebounds;

    RSAutoCanvasRestore acr(curCanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    linkedNodeParams.ApplyAlphaAndMatrixToCanvas(*curCanvas, true);
    return DrawOffscreenBuffer(canvas, *cacheSurface, *params);
}

bool RSWindowKeyFrameNodeDrawable::DrawOffscreenBuffer(Drawing::Canvas& canvas,
    Drawing::Surface& surface, const RSRenderParams& params)
{
    // cache and draw snapshot of offscreen canvas onto target canvas
    bool isFreezed = params.GetRSFreezeFlag();
    if (!isFreezed) {
        std::swap(cachedOffscreenImg_, preCachedOffscreenImg_);
    }

    cachedOffscreenImg_ = surface.GetImageSnapshot();
    if (UNLIKELY(cachedOffscreenImg_ == nullptr)) {
        RS_LOGE("RSWindowKeyFrameNodeDrawable::DrawOffscreenToBuffer get snapshot nullptr");
        return false;
    }

    if (preCachedOffscreenImg_ == nullptr) {
        preCachedOffscreenImg_ = cachedOffscreenImg_;
    }

    if (!isFreezed && LIKELY(!params.GetBounds().IsEmpty())) {
        return true;
    }

    Drawing::Brush paint;
    paint.SetAntiAlias(true);
    canvas.AttachBrush(paint);
    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
    canvas.DrawImage(*cachedOffscreenImg_, 0, 0, samplingOptions);
    canvas.DetachBrush();

    return true;
}

} // namespace OHOS::Rosen::DrawableV2
