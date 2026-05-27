/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "feature/opinc/rs_layer_part_draw_cache.h"

#include "common/rs_optional_trace.h"
#include "params/rs_render_params.h"

namespace OHOS::Rosen::DrawableV2 {
namespace {
constexpr int32_t BORDER_WIDTH = 6;
constexpr int32_t MARGIN = 20;
constexpr float LAYER_RECT_PEN_ALPHA = 0.5f;
constexpr int32_t MIN_UNCHANGE_COUNT = 3;
constexpr float FONT_SIZE = 24.0f;
constexpr float FONT_SCALE = 0.6f;
constexpr float FONT_SKEW = 0.0f;
}

void RSLayerPartDrawCache::PushLayerPartRenderDirtyRegion(const RSRenderParams& params,
    RSPaintFilterCanvas& curCanvas, RSPaintFilterCanvas& cacheCanvas, int nodeCount)
{
    Drawing::RectF absDrawRect(params.GetAbsDrawRect().GetLeft(), params.GetAbsDrawRect().GetTop(),
        params.GetAbsDrawRect().GetRight(), params.GetAbsDrawRect().GetBottom());
    Drawing::Matrix inverseMatrix;
    if (!curCanvas.GetTotalMatrix().Invert(inverseMatrix)) {
        return;
    }
    Drawing::RectF absDrawRectTemp;
    std::swap(absDrawRectTemp, absDrawRect);
    if (!inverseMatrix.MapRect(absDrawRect, absDrawRectTemp) || absDrawRect.IsEmpty()) {
        return;
    }

    RectI currentFrameDirty;
    if (unchangeCount_ <= MIN_UNCHANGE_COUNT) {
        unchangeCount_++;
        currentFrameDirty = { absDrawRect.GetLeft(), absDrawRect.GetTop(),
            absDrawRect.GetRight(), absDrawRect.GetBottom() };
    } else {
        currentFrameDirty = params.GetLayerPartRenderCurrentFrameDirtyRegion();
    }

    float boundsWidth = params.GetCacheSize().x_;
    float boundsHeight = params.GetCacheSize().y_;
    float widthScale = boundsWidth / absDrawRect.GetWidth();
    float heightScale = boundsHeight / absDrawRect.GetHeight();
    float left = (currentFrameDirty.GetLeft() - absDrawRect.GetLeft()) * widthScale;
    float top = (currentFrameDirty.GetTop() - absDrawRect.GetTop()) * heightScale;
    float width = currentFrameDirty.GetWidth() * widthScale;
    float height = currentFrameDirty.GetHeight() * heightScale;
    Drawing::RectF tempRect(left, top, left + width, top + height);
    auto currentFrameDirtyResult = tempRect.RoundOut();

    dirtyRegion_.SetRect(currentFrameDirtyResult);
    auto bounds = dirtyRegion_.GetBounds();
    RS_OPTIONAL_TRACE_NAME_FMT("id:%" PRIu64 ", nodeCount:%d, layerPartRenderDirtyRegion:[%d %d %d %d],"
        "absDrawRect:[%.1f, %.1f, %.1f, %.1f], cacheSize:[%.1f, %.1f]",
        params.GetId(), nodeCount, bounds.GetLeft(), bounds.GetTop(), bounds.GetWidth(), bounds.GetHeight(),
        absDrawRect.GetLeft(), absDrawRect.GetTop(), absDrawRect.GetWidth(), absDrawRect.GetHeight(),
        boundsWidth, boundsHeight);
    cacheCanvas.PushLayerPartRenderDirtyRegion(dirtyRegion_);
}

void RSLayerPartDrawCache::LayerPartRenderClipDirtyRegion(RSPaintFilterCanvas& canvas) const
{
    if (dirtyRegion_.IsEmpty()) {
        return;
    }
    canvas.ClipRect(dirtyRegion_.GetBounds());
}

void RSLayerPartDrawCache::PopLayerPartRenderDirtyRegion(RSPaintFilterCanvas& canvas) const
{
    LayerDirtyRegionDfx(canvas, dirtyRegion_.GetBounds());
    canvas.PopLayerPartRenderDirtyRegion();
}

void RSLayerPartDrawCache::ResetUnchangeCount()
{
    unchangeCount_ = 0;
}

void RSLayerPartDrawCache::Clear()
{
    dirtyRegion_.SetEmpty();
    unchangeCount_ = 0;
}

void RSLayerPartDrawCache::LayerDirtyRegionDfx(RSPaintFilterCanvas& canvas, const Drawing::RectI& dirtyRect)
{
    if (!RSSystemProperties::GetLayerPartRenderDebugEnabled()) {
        return;
    }

    Drawing::Brush brush;
    brush.SetColor(Drawing::Color(0x8090EE90));
    brush.SetAntiAlias(true);
    brush.SetAlphaF(LAYER_RECT_PEN_ALPHA);
    std::shared_ptr<Drawing::Typeface> typeFace = nullptr;
    std::string position = "pos:[" + dirtyRect.ToString() + "]";
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromString(position.c_str(), Drawing::Font(typeFace, FONT_SIZE, FONT_SCALE, FONT_SKEW));
    canvas.AttachBrush(brush);
    canvas.DrawRect(dirtyRect);
    canvas.DetachBrush();
    canvas.AttachBrush(Drawing::Brush());
    canvas.DrawTextBlob(textBlob.get(), dirtyRect.GetLeft() + BORDER_WIDTH, dirtyRect.GetTop() + MARGIN);
    canvas.DetachBrush();
}
} // namespace OHOS::Rosen::DrawableV2