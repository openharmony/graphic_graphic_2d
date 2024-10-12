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

#include "drawable/rs_surface_render_node_drawable.h"
#include "rs_draw_window_cache.h"
#include "rs_uni_render_thread.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

RSDrawWindowCache::~RSDrawWindowCache()
{
    ClearCache();
}

void RSDrawWindowCache::DrawAndCacheWindowContent(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable,
    RSPaintFilterCanvas& canvas, const Drawing::Rect& bounds)
{
    if (surfaceDrawable == nullptr) {
        RS_LOGE("RSDrawWindowCache::DrawAndCacheWindowContent drawable nullptr.");
        return;
    }

    // prepare offscreen canvas
    auto mainSurface = canvas.GetSurface();
    if (mainSurface == nullptr) {
        RS_LOGE("RSDrawWindowCache::DrawAndCacheWindowContent main surface nullptr.");
        return;
    }
    auto windowSurface = mainSurface->MakeSurface(bounds.GetWidth(), bounds.GetHeight());
    if (windowSurface == nullptr) {
        RS_LOGE("RSDrawWindowCache::DrawAndCacheWindowContent surface nullptr.");
        return;
    }
    auto windowCanvas = std::make_shared<RSPaintFilterCanvas>(windowSurface.get());
    if (windowCanvas == nullptr) {
        RS_LOGE("RSDrawWindowCache::DrawAndCacheWindowContent canvas nullptr.");
        windowSurface = nullptr;
        return;
    }
    RS_TRACE_NAME_FMT("DrawAndCacheWindow node[%lld] %s", surfaceDrawable->GetId(), surfaceDrawable->GetName().c_str());
    // copy HDR properties into offscreen canvas
    windowCanvas->CopyHDRConfiguration(canvas);
    // copy current canvas properties into offscreen canvas
    windowCanvas->CopyConfigurationToOffscreenCanvas(canvas);
    windowCanvas->SetDisableFilterCache(true);
    auto acr = std::make_unique<RSAutoCanvasRestore>(windowCanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    windowCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);

    // draw window content/children onto offscreen canvas
    auto& uniParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    bool isOpDropped = uniParams != nullptr ? uniParams->IsOpDropped() : true;
    if (uniParams) {
        uniParams->SetOpDropped(false); // temporarily close partial render
    }
    surfaceDrawable->DrawContent(*windowCanvas, bounds); // draw content
    surfaceDrawable->DrawChildren(*windowCanvas, bounds); // draw children
    if (uniParams) {
        uniParams->SetOpDropped(isOpDropped);
    }

    // cache and draw snapshot of offscreen canvas onto target canvas
    image_ = windowSurface->GetImageSnapshot();
    if (image_ == nullptr) {
        RS_LOGE("RSDrawWindowCache::DrawAndCacheWindowContent snapshot nullptr.");
        return;
    }
    Drawing::Brush paint;
    paint.SetAntiAlias(true);
    canvas.AttachBrush(paint);
    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
    canvas.DrawImage(*image_, 0, 0, samplingOptions);
    canvas.DetachBrush();
}

bool RSDrawWindowCache::DealWithCachedWindow(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable,
    RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams)
{
    if (surfaceDrawable == nullptr ||
        surfaceDrawable->HasCachedTexture() ||
        !HasCache() ||
        surfaceParams.GetUifirstNodeEnableParam() == MultiThreadCacheType::NONE) {
        ClearCache();
        return false;
    }
    if (ROSEN_EQ(image_->GetWidth(), 0) || ROSEN_EQ(image_->GetHeight(), 0)) {
        RS_LOGE("RSDrawWindowCache::DealWithCachedWindow buffer size is zero.");
        return false;
    }
    RS_TRACE_NAME_FMT("DealWithCachedWindow node[%lld] %s",
        surfaceDrawable->GetId(), surfaceDrawable->GetName().c_str());
    if (!RSUniRenderThread::GetCaptureParam().isSnapshot_) {
        canvas.MultiplyAlpha(surfaceParams.GetAlpha());
        canvas.ConcatMatrix(surfaceParams.GetMatrix());
    }
    auto boundSize = surfaceParams.GetFrameRect();
    // draw background
    surfaceDrawable->DrawBackground(canvas, boundSize);
    float scaleX = boundSize.GetWidth() / static_cast<float>(image_->GetWidth());
    float scaleY = boundSize.GetHeight() / static_cast<float>(image_->GetHeight());
    canvas.Save();
    canvas.Scale(scaleX, scaleY);
    if (RSSystemProperties::GetRecordingEnabled()) {
        if (image_->IsTextureBacked()) {
            RS_LOGI("RSDrawWindowCache::DealWithCachedWindow convert image from texture to raster image.");
            image_ = image_->MakeRasterImage();
        }
    }
    Drawing::Brush brush;
    canvas.AttachBrush(brush);
    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    auto gravityTranslate = surfaceDrawable->GetGravityTranslate(image_->GetWidth(), image_->GetHeight());
    // draw content/children
    canvas.DrawImage(*image_, gravityTranslate.x_, gravityTranslate.y_, samplingOptions);
    canvas.DetachBrush();
    canvas.Restore();
    // draw foreground
    surfaceDrawable->DrawForeground(canvas, boundSize);
    return true;
}

void RSDrawWindowCache::ClearCache()
{
    image_ = nullptr;
}

bool RSDrawWindowCache::HasCache() const
{
    return image_ != nullptr;
}

} // Rosen
} // OHOS