/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_paint_filter_canvas.h"

#include "include/core/SkColorFilter.h"

namespace OHOS {
namespace Rosen {

RSPaintFilterCanvas::RSPaintFilterCanvas(SkCanvas* canvas, float alpha)
    : SkPaintFilterCanvas(canvas), alphaStack_({ std::clamp(alpha, 0.f, 1.f) }), // construct stack with given alpha
      // Temporary fix, this default color should be 0x000000FF, fix this after foreground color refactor
      envStack_({ Env({ Color(0xFF000000) }) }) // construct stack with default foreground color
{}

RSPaintFilterCanvas::RSPaintFilterCanvas(SkSurface* skSurface, float alpha)
    : SkPaintFilterCanvas(skSurface ? skSurface->getCanvas() : nullptr), skSurface_(skSurface),
      alphaStack_({ std::clamp(alpha, 0.f, 1.f) }), // construct stack with given alpha
      // Temporary fix, this default color should be 0x000000FF, fix this after foreground color refactor
      envStack_({ Env({ Color(0xFF000000) }) }) // construct stack with default foreground color
{}

SkSurface* RSPaintFilterCanvas::GetSurface() const
{
    return skSurface_;
}

bool RSPaintFilterCanvas::onFilter(SkPaint& paint) const
{
    if (paint.getColor() == 0x00000001) { // foreground color and foreground color strategy identification
        paint.setColor(envStack_.top().envForegroundColor.AsArgbInt());
    }

    if (alphaStack_.top() >= 1.f) {
        return true;
    } else if (alphaStack_.top() <= 0.f) {
        return false;
    }
    // use alphaStack_.top() to multiply alpha
    paint.setAlphaf(paint.getAlphaf() * alphaStack_.top());
    return true;
}

void RSPaintFilterCanvas::onDrawPicture(const SkPicture* picture, const SkMatrix* matrix, const SkPaint* paint)
{
    SkPaint filteredPaint(paint ? *paint : SkPaint());
    if (this->onFilter(filteredPaint)) {
        this->SkCanvas::onDrawPicture(picture, matrix, &filteredPaint);
    }
}

void RSPaintFilterCanvas::MultiplyAlpha(float alpha)
{
    // multiply alpha to top of stack
    alphaStack_.top() *= std::clamp(alpha, 0.f, 1.f);
}

int RSPaintFilterCanvas::SaveAlpha()
{
    // make a copy of top of stack
    alphaStack_.push(alphaStack_.top());
    // return prev stack height
    return alphaStack_.size() - 1;
}

float RSPaintFilterCanvas::GetAlpha() const
{
    // return top of stack
    return alphaStack_.top();
}

void RSPaintFilterCanvas::RestoreAlpha()
{
    // sanity check, stack should not be empty
    if (alphaStack_.size() <= 1u) {
        return;
    }
    alphaStack_.pop();
}

int RSPaintFilterCanvas::GetAlphaSaveCount() const
{
    return alphaStack_.size();
}

void RSPaintFilterCanvas::RestoreAlphaToCount(int count)
{
    // sanity check, stack should not be empty
    if (count < 1) {
        count = 1;
    }
    // poo stack until stack height equals count
    int n = static_cast<int>(alphaStack_.size()) - count;
    for (int i = 0; i < n; ++i) {
        alphaStack_.pop();
    }
}

int RSPaintFilterCanvas::SaveEnv()
{
    // make a copy of top of stack
    envStack_.push(envStack_.top());
    // return prev stack height
    return envStack_.size() - 1;
}

void RSPaintFilterCanvas::RestoreEnv()
{
    // sanity check, stack should not be empty
    if (envStack_.size() <= 1) {
        return;
    }
    envStack_.pop();
}

void RSPaintFilterCanvas::RestoreEnvToCount(int count)
{
    // sanity check, stack should not be empty
    if (count < 1) {
        count = 1;
    }
    // poo stack until stack height equals count
    int n = static_cast<int>(envStack_.size()) - count;
    for (int i = 0; i < n; ++i) {
        envStack_.pop();
    }
}

int RSPaintFilterCanvas::GetEnvSaveCount() const
{
    return envStack_.size();
}

void RSPaintFilterCanvas::SetEnvForegroundColor(Color color)
{
    // sanity check, stack should not be empty
    if (envStack_.empty()) {
        return;
    }
    envStack_.top().envForegroundColor = color;
}

Color RSPaintFilterCanvas::GetEnvForegroundColor() const
{
    // sanity check, stack should not be empty
    if (envStack_.empty()) {
        return Color { 0xFF000000 }; // 0xFF000000 is default value -- black
    }
    return envStack_.top().envForegroundColor;
}

RSPaintFilterCanvas::SaveStatus RSPaintFilterCanvas::Save()
{
    // simultaneously save canvas and alpha
    return { save(), SaveAlpha(), SaveEnv() };
}

RSPaintFilterCanvas::SaveStatus RSPaintFilterCanvas::GetSaveStatus() const
{
    return { getSaveCount(), GetAlphaSaveCount(), GetEnvSaveCount() };
}

void RSPaintFilterCanvas::RestoreStatus(const SaveStatus& status)
{
    // simultaneously restore canvas and alpha
    restoreToCount(status.canvasSaveCount);
    RestoreAlphaToCount(status.alphaSaveCount);
    RestoreEnvToCount(status.envSaveCount);
}

void RSPaintFilterCanvas::CopyConfiguration(const RSPaintFilterCanvas& other)
{
    // Note:
    // 1. we don't need to copy alpha status, alpha will be applied when drawing cache.
    // copy high contrast flag
    isHighContrastEnabled_.store(other.isHighContrastEnabled_.load());
    // copy env
    envStack_.top() = other.envStack_.top();
    // cache related
    if (other.isHighContrastEnabled()) {
        // explicit disable cache for high contrast mode
        SetCacheType(RSPaintFilterCanvas::CacheType::DISABLED);
    } else {
        // planning: maybe we should copy source cache status
        SetCacheType(RSPaintFilterCanvas::CacheType::UNDEFINED);
    }
}

RSColorFilterCanvas::RSColorFilterCanvas(RSPaintFilterCanvas* canvas)
    : RSPaintFilterCanvas(canvas)
{}

bool RSColorFilterCanvas::onFilter(SkPaint& paint) const
{
    // foreground color and foreground color strategy identification
    if (paint.getColor() == 0x00000001) {
        // creates a color filter that blends the foreground color with the destination color
        paint.setColorFilter(SkColorFilters::Blend(GetEnvForegroundColor().AsArgbInt(), SkBlendMode::kDstIn));
    }

    return RSPaintFilterCanvas::onFilter(paint);
}

RSAutoCanvasRestore::RSAutoCanvasRestore(RSPaintFilterCanvas* canvas, SaveType type) : canvas_(canvas)
{
    if (canvas_) {
        saveCount_ = canvas->GetSaveStatus();
        if (SaveType::kCanvas & type) {
            canvas->save();
        }
        if (SaveType::kAlpha & type) {
            canvas->SaveAlpha();
        }
        if (SaveType::kEnv & type) {
            canvas->SaveEnv();
        }
    }
}

void RSPaintFilterCanvas::SetHighContrast(bool enabled)
{
    isHighContrastEnabled_ = enabled;
}
bool RSPaintFilterCanvas::isHighContrastEnabled() const
{
    return isHighContrastEnabled_;
}

void RSPaintFilterCanvas::SetCacheType(CacheType type)
{
    cacheType_ = type;
}
RSPaintFilterCanvas::CacheType RSPaintFilterCanvas::GetCacheType() const
{
    return cacheType_;
}

void RSPaintFilterCanvas::SetVisibleRect(SkRect visibleRect)
{
    visibleRect_ = visibleRect;
}

SkRect RSPaintFilterCanvas::GetVisibleRect() const
{
    return visibleRect_;
}

std::optional<SkRect> RSPaintFilterCanvas::GetLocalClipBounds(const SkCanvas& canvas, const SkIRect* clipRect)
{
    // if clipRect is explicitly specified, use it as the device clip bounds
    SkRect bounds = SkRect::Make((clipRect != nullptr) ? *clipRect : canvas.getDeviceClipBounds());

    if (bounds.isEmpty()) {
        return std::nullopt;
    }

    SkMatrix inverse;
    // if we can't invert the CTM, we can't return local clip bounds
    if (!(canvas.getTotalMatrix().invert(&inverse))) {
        return std::nullopt;
    }
    // return the inverse of the CTM applied to the device clip bounds as local clip bounds
    return inverse.mapRect(bounds);
}
} // namespace Rosen
} // namespace OHOS
