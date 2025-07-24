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

#include <algorithm>

#include "common/rs_occlusion_region.h"
#include "common/rs_rect.h"
#include "draw/canvas.h"

#include "platform/common/rs_log.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {

using namespace Drawing;

RSPaintFilterCanvasBase::RSPaintFilterCanvasBase(Drawing::Canvas* canvas)
    : Canvas(canvas ? canvas->GetWidth() : 0, canvas ? canvas->GetHeight() : 0), canvas_(canvas)
{
}

Drawing::Matrix RSPaintFilterCanvasBase::GetTotalMatrix() const
{
    return canvas_->GetTotalMatrix();
}

Drawing::Rect RSPaintFilterCanvasBase::GetLocalClipBounds() const
{
    return canvas_->GetLocalClipBounds();
}

Drawing::RectI RSPaintFilterCanvasBase::GetDeviceClipBounds() const
{
    return canvas_->GetDeviceClipBounds();
}

Drawing::RectI RSPaintFilterCanvasBase::GetRoundInDeviceClipBounds() const
{
    return canvas_->GetRoundInDeviceClipBounds();
}

uint32_t RSPaintFilterCanvasBase::GetSaveCount() const
{
    return canvas_->GetSaveCount();
}

#ifdef RS_ENABLE_GPU
std::shared_ptr<Drawing::GPUContext> RSPaintFilterCanvasBase::GetGPUContext()
{
    return canvas_ != nullptr ? canvas_->GetGPUContext() : nullptr;
}
#endif

void RSPaintFilterCanvasBase::DrawPoint(const Point& point)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawPoint(point);
    }
}

void RSPaintFilterCanvasBase::DrawSdf(const SDFShapeBase& shape)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawSdf(shape);
    }
}

void RSPaintFilterCanvasBase::DrawPoints(PointMode mode, size_t count, const Point pts[])
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawPoints(mode, count, pts);
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
    Brush b(brush);
    if (canvas_ != nullptr && OnFilterWithBrush(b)) {
        canvas_->DrawBackground(b);
    }
}

void RSPaintFilterCanvasBase::DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos,
    scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawShadow(path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
    }
}

void RSPaintFilterCanvasBase::DrawShadowStyle(const Path& path, const Point3& planeParams, const Point3& devLightPos,
    scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag, bool isLimitElevation)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawShadowStyle(
            path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag, isLimitElevation);
    }
}

void RSPaintFilterCanvasBase::DrawColor(Drawing::ColorQuad color, Drawing::BlendMode mode)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawColor(color, mode);
    }
}

void RSPaintFilterCanvasBase::DrawRegion(const Drawing::Region& region)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawRegion(region);
    }
}

void RSPaintFilterCanvasBase::DrawPatch(const Drawing::Point cubics[12], const Drawing::ColorQuad colors[4],
    const Drawing::Point texCoords[4], Drawing::BlendMode mode)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawPatch(cubics, colors, texCoords, mode);
    }
}

void RSPaintFilterCanvasBase::DrawVertices(const Drawing::Vertices& vertices, Drawing::BlendMode mode)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawVertices(vertices, mode);
    }
}

bool RSPaintFilterCanvasBase::OpCalculateBefore(const Matrix& matrix)
{
    if (canvas_ != nullptr && OnFilter()) {
        return canvas_->OpCalculateBefore(matrix);
    }
    return false;
}

std::shared_ptr<Drawing::OpListHandle> RSPaintFilterCanvasBase::OpCalculateAfter(const Drawing::Rect& bound)
{
    if (canvas_ != nullptr && OnFilter()) {
        return canvas_->OpCalculateAfter(bound);
    }
    return nullptr;
}

void RSPaintFilterCanvasBase::DrawAtlas(const Drawing::Image* atlas, const Drawing::RSXform xform[],
    const Drawing::Rect tex[], const Drawing::ColorQuad colors[], int count, Drawing::BlendMode mode,
    const Drawing::SamplingOptions& sampling, const Drawing::Rect* cullRect)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawAtlas(atlas, xform, tex, colors, count, mode, sampling, cullRect);
    }
}

void RSPaintFilterCanvasBase::DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawBitmap(bitmap, px, py);
    }
}

void RSPaintFilterCanvasBase::DrawImageNine(const Drawing::Image* image, const Drawing::RectI& center,
    const Drawing::Rect& dst, Drawing::FilterMode filter, const Drawing::Brush* brush)
{
    if (canvas_ != nullptr && OnFilter()) {
        if (brush) {
            Drawing::Brush b(*brush);
            OnFilterWithBrush(b);
            canvas_->DrawImageNine(image, center, dst, filter, &b);
        } else {
            canvas_->DrawImageNine(image, center, dst, filter, GetFilteredBrush());
        }
    }
}

void RSPaintFilterCanvasBase::DrawImageLattice(const Drawing::Image* image, const Drawing::Lattice& lattice,
    const Drawing::Rect& dst, Drawing::FilterMode filter)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawImageLattice(image, lattice, dst, filter);
    }
}

void RSPaintFilterCanvasBase::DrawImage(
    const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling)
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

void RSPaintFilterCanvasBase::DrawTextBlob(
    const Drawing::TextBlob* blob, const Drawing::scalar x, const Drawing::scalar y)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawTextBlob(blob, x, y);
    }
}

void RSPaintFilterCanvasBase::ClipRect(const Drawing::Rect& rect, Drawing::ClipOp op, bool doAntiAlias)
{
    if (canvas_ != nullptr) {
        canvas_->ClipRect(rect, op, doAntiAlias);
    }
}

void RSPaintFilterCanvasBase::ClipIRect(const Drawing::RectI& rect, Drawing::ClipOp op)
{
    if (canvas_ != nullptr) {
        canvas_->ClipIRect(rect, op);
    }
}

void RSPaintFilterCanvasBase::ClipRoundRect(const RoundRect& roundRect, ClipOp op, bool doAntiAlias)
{
    if (canvas_ != nullptr) {
        canvas_->ClipRoundRect(roundRect, op, doAntiAlias);
    }
}

void RSPaintFilterCanvasBase::ClipRoundRect(const Drawing::Rect& rect,
    std::vector<Drawing::Point>& pts, bool doAntiAlias)
{
    if (canvas_ != nullptr) {
        canvas_->ClipRoundRect(rect, pts, doAntiAlias);
    }
}

void RSPaintFilterCanvasBase::ClipPath(const Path& path, ClipOp op, bool doAntiAlias)
{
    if (canvas_ != nullptr) {
        canvas_->ClipPath(path, op, doAntiAlias);
    }
}

void RSPaintFilterCanvasBase::ClipRegion(const Region& region, ClipOp op)
{
    if (canvas_ != nullptr) {
        canvas_->ClipRegion(region, op);
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

uint32_t RSPaintFilterCanvasBase::Save()
{
    if (canvas_ != nullptr) {
        return canvas_->Save();
    }
    return 0;
}

void RSPaintFilterCanvasBase::SaveLayer(const SaveLayerOps& saveLayerRec)
{
    if (canvas_ == nullptr) {
        return;
    }
    Brush brush;
    if (saveLayerRec.GetBrush() != nullptr) {
        brush = *saveLayerRec.GetBrush();
        OnFilterWithBrush(brush);
    }
    SaveLayerOps slo(saveLayerRec.GetBounds(), &brush, saveLayerRec.GetSaveLayerFlags());
    canvas_->SaveLayer(slo);
}

void RSPaintFilterCanvasBase::Restore()
{
    if (canvas_ != nullptr) {
        canvas_->Restore();
    }
}

void RSPaintFilterCanvasBase::Discard()
{
    if (canvas_ != nullptr) {
        canvas_->Discard();
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

CoreCanvas& RSPaintFilterCanvasBase::AttachPaint(const Drawing::Paint& paint)
{
    if (canvas_ != nullptr) {
        canvas_->AttachPaint(paint);
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

CoreCanvas& RSPaintFilterCanvasBase::DetachPaint()
{
    if (canvas_ != nullptr) {
        canvas_->DetachPaint();
    }
    return *this;
}

bool RSPaintFilterCanvasBase::DrawBlurImage(const Drawing::Image& image, const Drawing::HpsBlurParameter& blurParams)
{
    bool result = false;
    if (canvas_ != nullptr) {
        result = canvas_->DrawBlurImage(image, blurParams);
    }
    return result;
}

std::array<int, 2> RSPaintFilterCanvasBase::CalcHpsBluredImageDimension(const Drawing::HpsBlurParameter& blurParams)
{
    std::array<int, 2> result = {0, 0}; // There are 2 variables
    if (canvas_ != nullptr) {
        result = canvas_->CalcHpsBluredImageDimension(blurParams);
    }
    return result;
}

RSPaintFilterCanvas::RSPaintFilterCanvas(Drawing::Canvas* canvas, float alpha)
    : RSPaintFilterCanvasBase(canvas), alphaStack_({ 1.0f }),
      envStack_({ Env { .envForegroundColor_ = RSColor(0xFF000000), .hasOffscreenLayer_ = false } })
{
    (void)alpha; // alpha is no longer used, but we keep it for backward compatibility
}

RSPaintFilterCanvas::RSPaintFilterCanvas(Drawing::Surface* surface, float alpha)
    : RSPaintFilterCanvasBase(surface ? surface->GetCanvas().get() : nullptr), surface_(surface), alphaStack_({ 1.0f }),
      envStack_({ Env { .envForegroundColor_ = RSColor(0xFF000000), .hasOffscreenLayer_ = false } })
{
    (void)alpha; // alpha is no longer used, but we keep it for backward compatibility
}

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
        p.SetColor(envStack_.top().envForegroundColor_.AsArgbInt());
    }

    // use alphaStack_.top() to multiply alpha
    if (alphaStack_.top() < 1 && alphaStack_.top() > 0) {
        p.SetAlpha(p.GetAlpha() * alphaStack_.top());
    }

    // use envStack_.top().blender_ to set blender
    if (auto& blender = envStack_.top().blender_) {
        if (p.GetBlenderEnabled()) {
            p.SetBlender(blender);
        }
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
        b.SetColor(envStack_.top().envForegroundColor_.AsArgbInt());
    }

    // use alphaStack_.top() to multiply alpha
    if (alphaStack_.top() < 1 && alphaStack_.top() > 0) {
        b.SetAlpha(b.GetAlpha() * alphaStack_.top());
    }

    // use envStack_.top().blender_ to set blender
    if (auto& blender = envStack_.top().blender_) {
        if (b.GetBlenderEnabled()) {
            b.SetBlender(blender);
        }
    }

    canvas_->AttachBrush(b);
    return *this;
}

CoreCanvas& RSPaintFilterCanvas::AttachPaint(const Drawing::Paint& paint)
{
    if (canvas_ == nullptr) {
        return *this;
    }

    Paint p(paint);
    if (p.GetColor() == 0x00000001) { // foreground color and foreground color strategy identification
        p.SetColor(envStack_.top().envForegroundColor_.AsArgbInt());
    }

    // use alphaStack_.top() to multiply alpha
    if (alphaStack_.top() < 1 && alphaStack_.top() > 0) {
        p.SetAlpha(p.GetAlpha() * alphaStack_.top());
    }

    // use envStack_.top().blender_ to set blender
    if (auto& blender = envStack_.top().blender_) {
        if (p.GetBlenderEnabled()) {
            p.SetBlender(blender);
        }
    }

    canvas_->AttachPaint(p);
    return *this;
}

bool RSPaintFilterCanvas::OnFilter() const
{
    return alphaStack_.top() > 0.f;
}

Drawing::Canvas* RSPaintFilterCanvas::GetRecordingCanvas() const
{
    return recordingState_ ? canvas_ : nullptr;
}

bool RSPaintFilterCanvas::GetRecordingState() const
{
    return recordingState_;
}

void RSPaintFilterCanvas::SetRecordingState(bool flag)
{
    recordingState_ = flag;
}

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

void RSPaintFilterCanvas::SetBlendMode(std::optional<int> blendMode)
{
    std::shared_ptr<Drawing::Blender> blender = nullptr;
    if (blendMode) {
        // map blendMode to Drawing::BlendMode and convert to Blender
        blender = Drawing::Blender::CreateWithBlendMode(static_cast<Drawing::BlendMode>(*blendMode));
    }
    RSPaintFilterCanvas::SetBlender(blender);
}

void RSPaintFilterCanvas::SetBlender(std::shared_ptr<Drawing::Blender> blender)
{
    envStack_.top().blender_ = blender;
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

void RSPaintFilterCanvas::SetEnvForegroundColor(Rosen::RSColor color)
{
    // sanity check, stack should not be empty
    if (envStack_.empty()) {
        return;
    }
    envStack_.top().envForegroundColor_ = color;
}

Drawing::ColorQuad RSPaintFilterCanvas::GetEnvForegroundColor() const
{
    // sanity check, stack should not be empty
    if (envStack_.empty()) {
        return Drawing::Color::COLOR_BLACK; // 0xFF000000 is default value -- black
    }
    return envStack_.top().envForegroundColor_.AsArgbInt();
}

RSPaintFilterCanvas::SaveStatus RSPaintFilterCanvas::SaveAllStatus(SaveType type)
{
    // save and return status on demand
    return { (RSPaintFilterCanvas::kCanvas & type) ? Save() : GetSaveCount(),
        (RSPaintFilterCanvas::kAlpha & type) ? SaveAlpha() : GetAlphaSaveCount(),
        (RSPaintFilterCanvas::kEnv & type) ? SaveEnv() : GetEnvSaveCount() };
}

RSPaintFilterCanvas::SaveStatus RSPaintFilterCanvas::GetSaveStatus() const
{
    return { GetSaveCount(), GetAlphaSaveCount(), GetEnvSaveCount() };
}

void RSPaintFilterCanvas::RestoreStatus(const SaveStatus& status)
{
    // simultaneously restore canvas and alpha
    RestoreToCount(status.canvasSaveCount);
    RestoreAlphaToCount(status.alphaSaveCount);
    RestoreEnvToCount(status.envSaveCount);
}

void RSPaintFilterCanvas::PushDirtyRegion(Drawing::Region& resultRegion)
{
    dirtyRegionStack_.push(std::move(resultRegion));
}

void RSPaintFilterCanvas::PopDirtyRegion()
{
    if (dirtyRegionStack_.empty()) {
        RS_LOGW("PopDirtyRegion dirtyRegionStack_ is empty");
        return;
    }
    dirtyRegionStack_.pop();
}

Drawing::Region& RSPaintFilterCanvas::GetCurDirtyRegion()
{
    return dirtyRegionStack_.top();
}

bool RSPaintFilterCanvas::IsDirtyRegionStackEmpty()
{
    return dirtyRegionStack_.empty();
}

void RSPaintFilterCanvas::CopyHDRConfiguration(const RSPaintFilterCanvas& other)
{
    brightnessRatio_ = other.brightnessRatio_;
    screenId_ = other.screenId_;
    targetColorGamut_ = other.targetColorGamut_;
}

void RSPaintFilterCanvas::CopyConfigurationToOffscreenCanvas(const RSPaintFilterCanvas& other)
{
    // Note:
    // 1. we don't need to copy alpha status, alpha will be applied when drawing cache.
    // 2. This function should only be called when creating offscreen canvas.
    // copy high contrast flag
    isHighContrastEnabled_.store(other.isHighContrastEnabled_.load());
    // copy env
    envStack_.top() = other.envStack_.top();
    // update effect matrix
    auto effectData = other.envStack_.top().effectData_;
    if (effectData != nullptr) {
        // make a deep copy of effect data, and calculate the mapping matrix from
        // local coordinate system to global coordinate system.
        auto copiedEffectData = std::make_shared<CachedEffectData>(*effectData);
        if (copiedEffectData == nullptr) {
            ROSEN_LOGE("RSPaintFilterCanvas::CopyConfigurationToOffscreenCanvas fail to create effectData");
            return;
        }
        Drawing::Matrix inverse;
        if (other.GetTotalMatrix().Invert(inverse)) {
            copiedEffectData->cachedMatrix_.PostConcat(inverse);
        } else {
            ROSEN_LOGE("RSPaintFilterCanvas::CopyConfigurationToOffscreenCanvas get invert matrix failed!");
        }
        envStack_.top().effectData_ = copiedEffectData;
    }
    // cache related
    if (other.isHighContrastEnabled()) {
        // explicit disable cache for high contrast mode
        SetCacheType(RSPaintFilterCanvas::CacheType::DISABLED);
    } else {
        // planning: maybe we should copy source cache status
        SetCacheType(other.GetCacheType());
    }
    isParallelCanvas_ = other.isParallelCanvas_;
    disableFilterCache_ = other.disableFilterCache_;
    threadIndex_ = other.threadIndex_;
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
Drawing::CacheType RSPaintFilterCanvas::GetCacheType() const
{
    return cacheType_;
}

void RSPaintFilterCanvas::SetVisibleRect(Drawing::Rect visibleRect)
{
    visibleRect_ = visibleRect;
}

Drawing::Rect RSPaintFilterCanvas::GetVisibleRect() const
{
    return visibleRect_;
}

std::optional<Drawing::Rect> RSPaintFilterCanvas::GetLocalClipBounds(const Drawing::Canvas& canvas,
    const Drawing::RectI* clipRect)
{
    // if clipRect is explicitly specified, use it as the device clip bounds
    Drawing::Rect bounds = Rect((clipRect != nullptr) ? *clipRect : canvas.GetDeviceClipBounds());

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


void RSPaintFilterCanvas::SetEffectData(const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData>& effectData)
{
    envStack_.top().effectData_ = effectData;
}

void RSPaintFilterCanvas::SetDrawnRegion(const Occlusion::Region& region)
{
    drawnRegion_ = region;
}

const Occlusion::Region& RSPaintFilterCanvas::GetDrawnRegion() const
{
    return drawnRegion_;
}

const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData>& RSPaintFilterCanvas::GetEffectData() const
{
    return envStack_.top().effectData_;
}

void RSPaintFilterCanvas::SetBehindWindowData(
    const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData>& behindWindowData)
{
    envStack_.top().behindWindowData_ = behindWindowData;
}

const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData>& RSPaintFilterCanvas::GetBehindWindowData() const
{
    return envStack_.top().behindWindowData_;
}

void RSPaintFilterCanvas::ReplaceMainScreenData(std::shared_ptr<Drawing::Surface>& offscreenSurface,
    std::shared_ptr<RSPaintFilterCanvas>& offscreenCanvas)
{
    if (offscreenSurface != nullptr && offscreenCanvas != nullptr) {
        storeMainScreenSurface_.push(surface_);
        storeMainScreenCanvas_.push(canvas_);
        surface_ = offscreenSurface.get();
        canvas_ = offscreenCanvas.get();
        OffscreenData offscreenData = {offscreenSurface, offscreenCanvas};
        offscreenDataList_.push(offscreenData);
    }
}

void RSPaintFilterCanvas::SwapBackMainScreenData()
{
    if (!storeMainScreenSurface_.empty() && !storeMainScreenCanvas_.empty() && !offscreenDataList_.empty()) {
        surface_ = storeMainScreenSurface_.top();
        canvas_ = storeMainScreenCanvas_.top();
        storeMainScreenSurface_.pop();
        storeMainScreenCanvas_.pop();
        offscreenDataList_.pop();
    }
}

void RSPaintFilterCanvas::SavePCanvasList()
{
    storedPCanvasList_.push_back(pCanvasList_);
}

void RSPaintFilterCanvas::RestorePCanvasList()
{
    if (!storedPCanvasList_.empty()) {
        auto item = storedPCanvasList_.back();
        pCanvasList_.swap(item);
        storedPCanvasList_.pop_back();
    }
}

void RSPaintFilterCanvas::SetCanvasStatus(const CanvasStatus& status)
{
    SetAlpha(status.alpha_);
    SetMatrix(status.matrix_);
    SetEffectData(status.effectData_);
}

RSPaintFilterCanvas::CanvasStatus RSPaintFilterCanvas::GetCanvasStatus() const
{
    return { GetAlpha(), GetTotalMatrix(), GetEffectData() };
}

RSPaintFilterCanvas::CachedEffectData::CachedEffectData(std::shared_ptr<Drawing::Image>&& image,
    const Drawing::RectI& rect)
    : cachedImage_(image), cachedRect_(rect), cachedMatrix_(Drawing::Matrix())
{}

void RSPaintFilterCanvas::SetIsParallelCanvas(bool isParallel)
{
    isParallelCanvas_ = isParallel;
}

bool RSPaintFilterCanvas::GetIsParallelCanvas() const
{
    return isParallelCanvas_;
}

// UNI_MAIN_THREAD_INDEX, UNI_RENDER_THREAD_INDEX, subthread 0 1 2.
void RSPaintFilterCanvas::SetParallelThreadIdx(uint32_t idx)
{
    threadIndex_ = idx;
}

uint32_t RSPaintFilterCanvas::GetParallelThreadIdx() const
{
    return threadIndex_;
}

void RSPaintFilterCanvas::SetDisableFilterCache(bool disable)
{
    disableFilterCache_ = disable;
}

bool RSPaintFilterCanvas::GetDisableFilterCache() const
{
    return disableFilterCache_;
}

void RSPaintFilterCanvas::SetRecordDrawable(bool enable)
{
    recordDrawable_ = enable;
}

bool RSPaintFilterCanvas::GetRecordDrawable() const
{
    return recordDrawable_;
}
bool RSPaintFilterCanvas::HasOffscreenLayer() const
{
    return envStack_.top().hasOffscreenLayer_;
}
void RSPaintFilterCanvas::SaveLayer(const Drawing::SaveLayerOps& saveLayerOps)
{
    envStack_.top().hasOffscreenLayer_ = true;
    RSPaintFilterCanvasBase::SaveLayer(saveLayerOps);
}

bool RSPaintFilterCanvas::IsCapture() const
{
    return isCapture_;
}
void RSPaintFilterCanvas::SetCapture(bool isCapture)
{
    isCapture_ = isCapture;
}

ScreenId RSPaintFilterCanvas::GetScreenId() const
{
    return screenId_;
}

void RSPaintFilterCanvas::SetScreenId(ScreenId screenId)
{
    screenId_ = screenId;
}

GraphicColorGamut RSPaintFilterCanvas::GetTargetColorGamut() const
{
    return targetColorGamut_;
}

void RSPaintFilterCanvas::SetTargetColorGamut(GraphicColorGamut colorGamut)
{
    targetColorGamut_ = colorGamut;
}

float RSPaintFilterCanvas::GetBrightnessRatio() const
{
    return brightnessRatio_;
}

void RSPaintFilterCanvas::SetBrightnessRatio(float brightnessRatio)
{
    brightnessRatio_ = brightnessRatio;
}
} // namespace Rosen
} // namespace OHOS
