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

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkColorFilter.h"
#endif

namespace OHOS {
namespace Rosen {

#ifdef USE_ROSEN_DRAWING
using namespace Drawing;

RSPaintFilterCanvasBase::RSPaintFilterCanvasBase(Drawing::Canvas* canvas) : canvas_(canvas) {}

void RSPaintFilterCanvasBase::DrawPoint(const Point& point)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawPoint(point);
    }
}

void RSPaintFilterCanvasBase::DrawLine(const Point& startPt, const Point& endPt)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawLine(startPt, endPt);
    }
}

void RSPaintFilterCanvasBase::DrawRect(const Rect& rect)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawRect(rect);
    }
}

void RSPaintFilterCanvasBase::DrawRoundRect(const RoundRect& roundRect)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawRoundRect(roundRect);
    }
}

void RSPaintFilterCanvasBase::DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawNestedRoundRect(outer, inner);
    }
}

void RSPaintFilterCanvasBase::DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawArc(oval, startAngle, sweepAngle);
    }
}

void RSPaintFilterCanvasBase::DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawPie(oval, startAngle, sweepAngle);
    }
}

void RSPaintFilterCanvasBase::DrawOval(const Rect& oval)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawOval(oval);
    }
}

void RSPaintFilterCanvasBase::DrawCircle(const Point& centerPt, scalar radius)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawCircle(centerPt, radius);
    }
}

void RSPaintFilterCanvasBase::DrawPath(const Path& path)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawPath(path);
    }
}

void RSPaintFilterCanvasBase::DrawBackground(const Brush& brush)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawBackground(brush);
    }
}

void RSPaintFilterCanvasBase::DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos,
    scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawShadow(path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
    }
}

void RSPaintFilterCanvasBase::DrawRegion(const Drawing::Region& region)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawRegion(region);
    }
}

void RSPaintFilterCanvasBase::DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawBitmap(bitmap, px, py);
    }
}

void RSPaintFilterCanvasBase::DrawBitmap(Media::PixelMap& pixelMap, const scalar px, const scalar py)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawBitmap(pixelMap, px, py);
    }
}

void RSPaintFilterCanvasBase::DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawImage(image, px, py, sampling);
    }
}

void RSPaintFilterCanvasBase::DrawImageRect(const Image& image, const Rect& src, const Rect& dst,
    const SamplingOptions& sampling, SrcRectConstraint constraint)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawImageRect(image, src, dst, sampling, constraint);
    }
}

void RSPaintFilterCanvasBase::DrawImageRect(const Image& image, const Rect& dst, const SamplingOptions& sampling)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawImageRect(image, dst, sampling);
    }
}

void RSPaintFilterCanvasBase::DrawPicture(const Picture& picture)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawPicture(picture);
    }
}

void RSPaintFilterCanvasBase::ClipRect(const Drawing::Rect& rect, Drawing::ClipOp op, bool doAntiAlias)
{
    if (canvas_ != nullptr) {
        canvas_->ClipRect(rect, op, doAntiAlias);
    }
}

void RSPaintFilterCanvasBase::ClipRoundRect(const RoundRect& roundRect, ClipOp op, bool doAntiAlias)
{
    if (canvas_ != nullptr) {
        canvas_->ClipRoundRect(roundRect, op, doAntiAlias);
    }
}

void RSPaintFilterCanvasBase::ClipPath(const Path& path, ClipOp op, bool doAntiAlias)
{
    if (canvas_ != nullptr) {
        canvas_->ClipPath(path, op, doAntiAlias);
    }
}

void RSPaintFilterCanvasBase::SetMatrix(const Matrix& matrix)
{
    if (canvas_ != nullptr) {
        canvas_->SetMatrix(matrix);
    }
}

void RSPaintFilterCanvasBase::ResetMatrix()
{
    if (canvas_ != nullptr) {
        canvas_->ResetMatrix();
    }
}

void RSPaintFilterCanvasBase::ConcatMatrix(const Matrix& matrix)
{
    if (canvas_ != nullptr) {
        canvas_->ConcatMatrix(matrix);
    }
}

void RSPaintFilterCanvasBase::Translate(scalar dx, scalar dy)
{
    if (canvas_ != nullptr) {
        canvas_->Translate(dx, dy);
    }
}

void RSPaintFilterCanvasBase::Scale(scalar sx, scalar sy)
{
    if (canvas_ != nullptr) {
        canvas_->Scale(sx, sy);
    }
}

void RSPaintFilterCanvasBase::Rotate(scalar deg, scalar sx, scalar sy)
{
    if (canvas_ != nullptr) {
        canvas_->Rotate(deg, sx, sy);
    }
}

void RSPaintFilterCanvasBase::Shear(scalar sx, scalar sy)
{
    if (canvas_ != nullptr) {
        canvas_->Shear(sx, sy);
    }
}

void RSPaintFilterCanvasBase::Flush()
{
    if (canvas_ != nullptr) {
        canvas_->Flush();
    }
}

void RSPaintFilterCanvasBase::Clear(ColorQuad color)
{
    if (canvas_ != nullptr) {
        canvas_->Clear(color);
    }
}

void RSPaintFilterCanvasBase::Save()
{
    if (canvas_ != nullptr) {
        canvas_->Save();
    }
}

void RSPaintFilterCanvasBase::SaveLayer(const SaveLayerOps& saveLayerRec)
{
    if (canvas_ != nullptr) {
        canvas_->SaveLayer(saveLayerRec);
    }
}

void RSPaintFilterCanvasBase::Restore()
{
    if (canvas_ != nullptr) {
        canvas_->Restore();
    }
}

CoreCanvas& RSPaintFilterCanvasBase::AttachPen(const Pen& pen)
{
    if (canvas_ != nullptr) {
        canvas_->AttachPen(pen);
    }
    return *this;
}

CoreCanvas& RSPaintFilterCanvasBase::AttachBrush(const Brush& brush)
{
    if (canvas_ != nullptr) {
        canvas_->AttachBrush(brush);
    }
    return *this;
}

CoreCanvas& RSPaintFilterCanvasBase::DetachPen()
{
    if (canvas_ != nullptr) {
        canvas_->DetachPen();
    }
    return *this;
}

CoreCanvas& RSPaintFilterCanvasBase::DetachBrush()
{
    if (canvas_ != nullptr) {
        canvas_->DetachBrush();
    }
    return *this;
}
#endif

#ifndef USE_ROSEN_DRAWING
RSPaintFilterCanvas::RSPaintFilterCanvas(SkCanvas* canvas, float alpha)
    : SkPaintFilterCanvas(canvas),
      alphaStack_({ std::clamp(alpha, 0.f, 1.f) }), // construct stack with given alpha
      // Temporary fix, this default color should be 0x000000FF, fix this after foreground color refactor
      envStack_({ Env({ Color(0xFF000000) }) }), // construct stack with default foreground color
      effectStack_({ CacheEffectData {} })
{}

RSPaintFilterCanvas::RSPaintFilterCanvas(SkSurface* skSurface, float alpha)
    : SkPaintFilterCanvas(skSurface ? skSurface->getCanvas() : nullptr), skSurface_(skSurface),
      alphaStack_({ std::clamp(alpha, 0.f, 1.f) }), // construct stack with given alpha
      // Temporary fix, this default color should be 0x000000FF, fix this after foreground color refactor
      envStack_({ Env({ Color(0xFF000000) }) }), // construct stack with default foreground color
      effectStack_({ CacheEffectData {} })
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
#else
RSPaintFilterCanvas::RSPaintFilterCanvas(Drawing::Canvas* canvas, float alpha)
    : RSPaintFilterCanvasBase(canvas), alphaStack_({ std::clamp(alpha, 0.f, 1.f) }), // construct stack with given alpha
      // Temporary fix, this default color should be 0x000000FF, fix this after foreground color refactor
      envStack_({ Env({ RSColor(0xFF000000) }) }) // construct stack with default foreground color
{}

RSPaintFilterCanvas::RSPaintFilterCanvas(Drawing::Surface* surface, float alpha)
    : RSPaintFilterCanvasBase(surface ? surface->GetCanvas().get() : nullptr), surface_(surface),
      alphaStack_({ std::clamp(alpha, 0.f, 1.f) }), // construct stack with given alpha
      // Temporary fix, this default color should be 0x000000FF, fix this after foreground color refactor
      envStack_({ Env({ RSColor(0xFF000000) }) }) // construct stack with default foreground color
{}

Drawing::Surface* RSPaintFilterCanvas::GetSurface() const
{
    return surface_;
}

CoreCanvas& RSPaintFilterCanvas::AttachPen(const Pen& pen)
{
    if (canvas_ == nullptr) {
        return *this;
    }

    Pen p(pen);
    if (p.GetColor() == 0x00000001) { // foreground color and foreground color strategy identification
        p.SetColor(envStack_.top().envForegroundColor.AsArgbInt());
    }

    // use alphaStack_.top() to multiply alpha
    if (alphaStack_.top() < 1 && alphaStack_.top() > 0) {
        p.SetAlpha(p.GetAlpha() * alphaStack_.top());
    }

    canvas_->AttachPen(p);
    return *this;
}

CoreCanvas& RSPaintFilterCanvas::AttachBrush(const Brush& brush)
{
    if (canvas_ == nullptr) {
        return *this;
    }

    Brush b(brush);
    if (b.GetColor() == 0x00000001) { // foreground color and foreground color strategy identification
        b.SetColor(envStack_.top().envForegroundColor.AsArgbInt());
    }

    // use alphaStack_.top() to multiply alpha
    if (alphaStack_.top() < 1 && alphaStack_.top() > 0) {
        b.SetAlpha(b.GetAlpha() * alphaStack_.top());
    }

    canvas_->AttachBrush(b);
    return *this;
}

bool RSPaintFilterCanvas::OnFilter() const
{
    return alphaStack_.top() > 0.f;
}
#endif // USE_ROSEN_DRAWING

void RSPaintFilterCanvas::MultiplyAlpha(float alpha)
{
    // multiply alpha to top of stack
    alphaStack_.top() *= std::clamp(alpha, 0.f, 1.f);
}

void RSPaintFilterCanvas::SetAlpha(float alpha)
{
    alphaStack_.top() = std::clamp(alpha, 0.f, 1.f);
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

#ifndef USE_ROSEN_DRAWING
void RSPaintFilterCanvas::SetEnvForegroundColor(Color color)
#else
void RSPaintFilterCanvas::SetEnvForegroundColor(Rosen::RSColor color)
#endif
{
    // sanity check, stack should not be empty
    if (envStack_.empty()) {
        return;
    }
    envStack_.top().envForegroundColor = color;
}

#ifndef USE_ROSEN_DRAWING
Color RSPaintFilterCanvas::GetEnvForegroundColor() const
{
    // sanity check, stack should not be empty
    if (envStack_.empty()) {
        return Color { 0xFF000000 }; // 0xFF000000 is default value -- black
    }
    return envStack_.top().envForegroundColor;
}
#else
RSColor RSPaintFilterCanvas::GetEnvForegroundColor() const
{
    // sanity check, stack should not be empty
    if (envStack_.empty()) {
        return RSColor { 0xFF000000 }; // 0xFF000000 is default value -- black
    }
    return envStack_.top().envForegroundColor;
}
#endif

#ifndef USE_ROSEN_DRAWING
RSPaintFilterCanvas::SaveStatus RSPaintFilterCanvas::Save()
{
    // simultaneously save canvas and alpha
    return { save(), SaveAlpha(), SaveEnv() };
}
#else
RSPaintFilterCanvas::SaveStatus RSPaintFilterCanvas::SaveAllStatus()
{
    // simultaneously save canvas and alpha
    int canvasSaveCount = GetSaveCount();
    Save();
    return { canvasSaveCount, SaveAlpha(), SaveEnv() };
}
#endif

RSPaintFilterCanvas::SaveStatus RSPaintFilterCanvas::GetSaveStatus() const
{
#ifndef USE_ROSEN_DRAWING
    return { getSaveCount(), GetAlphaSaveCount(), GetEnvSaveCount() };
#else
    return { GetSaveCount(), GetAlphaSaveCount(), GetEnvSaveCount() };
#endif
}

void RSPaintFilterCanvas::RestoreStatus(const SaveStatus& status)
{
    // simultaneously restore canvas and alpha
#ifndef USE_ROSEN_DRAWING
    restoreToCount(status.canvasSaveCount);
#else
    RestoreToCount(status.canvasSaveCount);
#endif
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

#ifndef USE_ROSEN_DRAWING
bool RSColorFilterCanvas::onFilter(SkPaint& paint) const
{
    // foreground color and foreground color strategy identification
    if (paint.getColor() == 0x00000001) {
        // creates a color filter that blends the foreground color with the destination color
        paint.setColorFilter(SkColorFilters::Blend(GetEnvForegroundColor().AsArgbInt(), SkBlendMode::kDstIn));
    }

    return RSPaintFilterCanvas::onFilter(paint);
}
#else
CoreCanvas& RSColorFilterCanvas::AttachPen(const Pen& pen)
{
    if (canvas_ == nullptr) {
        return *this;
    }

    Pen p(pen);
    if (p.GetColor() == 0x00000001) { // foreground color and foreground color strategy identification
        p.SetColor(GetEnvStack().top().envForegroundColor.AsArgbInt());
        // creates a color filter that blends the foreground color with the destination color
        Drawing::Filter filter;
        filter.SetColorFilter(Drawing::ColorFilter::CreateBlendModeColorFilter(
            GetEnvForegroundColor().AsArgbInt(), Drawing::BlendMode::DST_IN));
        p.SetFilter(filter);
    }

    // use alphaStack_.top() to multiply alpha
    if (GetAlphaStack().top() < 1 && GetAlphaStack().top() > 0) {
        p.SetAlpha(p.GetAlpha() * GetAlphaStack().top());
    }

    canvas_->AttachPen(p);
    return *this;
}

CoreCanvas& RSColorFilterCanvas::AttachBrush(const Brush& brush)
{
    if (canvas_ == nullptr) {
        return *this;
    }

    Brush b(brush);
    if (b.GetColor() == 0x00000001) { // foreground color and foreground color strategy identification
        b.SetColor(GetEnvStack().top().envForegroundColor.AsArgbInt());
        // creates a color filter that blends the foreground color with the destination color
        Drawing::Filter filter;
        filter.SetColorFilter(Drawing::ColorFilter::CreateBlendModeColorFilter(
            GetEnvForegroundColor().AsArgbInt(), Drawing::BlendMode::DST_IN));
        b.SetFilter(filter);
    }

    // use alphaStack_.top() to multiply alpha
    if (GetAlphaStack().top() < 1 && GetAlphaStack().top() > 0) {
        b.SetAlpha(b.GetAlpha() * GetAlphaStack().top());
    }

    canvas_->AttachBrush(b);
    return *this;
}
#endif

RSAutoCanvasRestore::RSAutoCanvasRestore(RSPaintFilterCanvas* canvas, SaveType type) : canvas_(canvas)
{
    if (canvas_) {
        saveCount_ = canvas->GetSaveStatus();
        if (SaveType::kCanvas & type) {
#ifndef USE_ROSEN_DRAWING
            canvas->save();
#else
            canvas->Save();
#endif
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

#ifndef USE_ROSEN_DRAWING
void RSPaintFilterCanvas::SetVisibleRect(SkRect visibleRect)
{
    visibleRect_ = visibleRect;
}

SkRect RSPaintFilterCanvas::GetVisibleRect() const
{
    return visibleRect_;
}
#else
void RSPaintFilterCanvas::SetVisibleRect(Drawing::Rect visibleRect)
{
    visibleRect_ = visibleRect;
}

Drawing::Rect RSPaintFilterCanvas::GetVisibleRect() const
{
    return visibleRect_;
}
#endif

#ifndef USE_ROSEN_DRAWING
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
#else
std::optional<Drawing::Rect> RSPaintFilterCanvas::GetLocalClipBounds(const Drawing::Canvas& canvas,
    const Drawing::RectI* clipRect)
{
    // if clipRect is explicitly specified, use it as the device clip bounds
    auto tmpRect = (clipRect != nullptr) ? *clipRect : canvas.GetDeviceClipBounds();
    Drawing::Rect bounds(static_cast<scalar>(tmpRect.GetLeft()), static_cast<scalar>(tmpRect.GetTop()),
        static_cast<scalar>(tmpRect.GetRight()), static_cast<scalar>(tmpRect.GetBottom()));

    if (!bounds.IsValid()) {
        return std::nullopt;
    }

    Drawing::Matrix inverse;
    // if we can't invert the CTM, we can't return local clip bounds
    if (!(canvas.GetTotalMatrix().Invert(inverse))) {
        return std::nullopt;
    }
    // return the inverse of the CTM applied to the device clip bounds as local clip bounds
    Drawing::Rect dst;
    inverse.MapRect(dst, bounds);
    return dst;
}

std::stack<float> RSPaintFilterCanvas::GetAlphaStack()
{
    return alphaStack_;
}

std::stack<RSPaintFilterCanvas::Env> RSPaintFilterCanvas::GetEnvStack()
{
    return envStack_;
}
#endif

#ifndef USE_ROSEN_DRAWING
SkCanvas::SaveLayerStrategy RSPaintFilterCanvas::getSaveLayerStrategy(const SaveLayerRec& rec)
{
    SkPaint p = rec.fPaint ? *rec.fPaint : SkPaint();
    SaveLayerRec tmpRec = rec;
    if (onFilter(p)) {
        tmpRec.fPaint = &p;
    }
    return SkPaintFilterCanvas::getSaveLayerStrategy(tmpRec);
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSPaintFilterCanvas::SetEffectData(const CacheEffectData& effectData)
{
    effectStack_.top() = effectData;
}

void RSPaintFilterCanvas::SetChildrenPath(const SkPath& childrenPath)
{
    effectStack_.top().childrenPath_ = childrenPath;
}

const CacheEffectData& RSPaintFilterCanvas::GetEffectData() const
{
    return effectStack_.top();
}

void RSPaintFilterCanvas::SaveEffectData()
{
    effectStack_.push({});
}

void RSPaintFilterCanvas::RestoreEffectData()
{
    if (effectStack_.size() < 1u) {
        return;
    }
    effectStack_.pop();
}
#endif
} // namespace Rosen
} // namespace OHOS
