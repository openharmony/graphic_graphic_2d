/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "property/rs_properties_painter.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkColorFilter.h"
#include "include/core/SkMaskFilter.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPoint3.h"
#include "include/core/SkRRect.h"
#include "include/core/SkSurface.h"
#include "include/effects/Sk1DPathEffect.h"
#include "include/effects/SkDashPathEffect.h"
#include "include/effects/SkLumaColorFilter.h"
#include "include/utils/SkShadowUtils.h"

#include "common/rs_obj_abs_geometry.h"
#include "common/rs_vector2.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"
#include "render/rs_blur_filter.h"
#include "render/rs_image.h"
#include "render/rs_mask.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
namespace {
bool g_forceBgAntiAlias = true;
constexpr int PARAM_DOUBLE = 2;
constexpr float MIN_TRANS_RATIO = 0.0f;
constexpr float MAX_TRANS_RATIO = 0.95f;
constexpr float MIN_SPOT_RATIO = 1.0f;
constexpr float MAX_SPOT_RATIO = 1.95f;
constexpr float MAX_AMBIENT_RADIUS = 150.0f;
} // namespace

SkRect RSPropertiesPainter::Rect2SkRect(const RectF& r)
{
    return SkRect::MakeXYWH(r.left_, r.top_, r.width_, r.height_);
}

SkRRect RSPropertiesPainter::RRect2SkRRect(const RRect& rr)
{
    SkRect rect = SkRect::MakeXYWH(rr.rect_.left_, rr.rect_.top_, rr.rect_.width_, rr.rect_.height_);
    SkRRect rrect = SkRRect::MakeEmpty();

    // set radius for all 4 corner of RRect
    constexpr uint32_t NUM_OF_CORNERS_IN_RECT = 4;
    SkVector vec[NUM_OF_CORNERS_IN_RECT];
    for (uint32_t i = 0; i < NUM_OF_CORNERS_IN_RECT; i++) {
        vec[i].set(rr.radius_[i].x_, rr.radius_[i].y_);
    }

    rrect.setRectRadii(rect, vec);
    return rrect;
}

bool RSPropertiesPainter::GetGravityMatrix(Gravity gravity, RectF rect, float w, float h, SkMatrix& mat)
{
    if (w == rect.width_ && h == rect.height_) {
        return false;
    }
    mat.reset();
    switch (gravity) {
        case Gravity::CENTER: {
            mat.preTranslate((rect.width_ - w) / PARAM_DOUBLE, (rect.height_ - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::TOP: {
            mat.preTranslate((rect.width_ - w) / PARAM_DOUBLE, 0);
            return true;
        }
        case Gravity::BOTTOM: {
            mat.preTranslate((rect.width_ - w) / PARAM_DOUBLE, rect.height_ - h);
            return true;
        }
        case Gravity::LEFT: {
            mat.preTranslate(0, (rect.height_ - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::RIGHT: {
            mat.preTranslate(rect.width_ - w, (rect.height_ - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::TOP_LEFT: {
            return false;
        }
        case Gravity::TOP_RIGHT: {
            mat.preTranslate(rect.width_ - w, 0);
            return true;
        }
        case Gravity::BOTTOM_LEFT: {
            mat.preTranslate(0, rect.height_ - h);
            return true;
        }
        case Gravity::BOTTOM_RIGHT: {
            mat.preTranslate(rect.width_ - w, rect.height_ - h);
            return true;
        }
        case Gravity::RESIZE: {
            mat.preScale(rect.width_ / w, rect.height_ / h);
            return true;
        }
        case Gravity::RESIZE_ASPECT: {
            float scale = std::min(rect.width_ / w, rect.height_ / h);
            mat.preScale(scale, scale);
            mat.preTranslate((rect.width_ / scale - w) / PARAM_DOUBLE, (rect.height_ / scale - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::RESIZE_ASPECT_FILL: {
            float scale = std::max(rect.width_ / w, rect.height_ / h);
            mat.preScale(scale, scale);
            mat.preTranslate((rect.width_ / scale - w) / PARAM_DOUBLE, (rect.height_ / scale - h) / PARAM_DOUBLE);
            return true;
        }
        default: {
            ROSEN_LOGE("GetGravityMatrix unknow gravity=[%d]", gravity);
            return false;
        }
    }
}

void RSPropertiesPainter::Clip(SkCanvas& canvas, RectF rect)
{
    canvas.clipRect(Rect2SkRect(rect), true);
}

void RSPropertiesPainter::GetShadowDirtyRect(RectI& dirtyShadow, const RSProperties& properties, const RRect* rrect)
{
    // [Planning]: After Skia being updated, we should directly call SkShadowUtils::GetLocalBounds here.
    if (!properties.IsShadowValid()) {
        return;
    }
    SkPath skPath;
    if (properties.GetShadowPath() && !properties.GetShadowPath()->GetSkiaPath().isEmpty()) {
        skPath = properties.GetShadowPath()->GetSkiaPath();
    } else if (properties.GetClipBounds()) {
        skPath = properties.GetClipBounds()->GetSkiaPath();
    } else {
        if (rrect != nullptr) {
            skPath.addRRect(RRect2SkRRect(*rrect));
        } else {
            skPath.addRRect(RRect2SkRRect(properties.GetRRect()));
        }
    }
    skPath.offset(properties.GetShadowOffsetX(), properties.GetShadowOffsetY());

    SkRect shadowRect = skPath.getBounds();
    if (properties.shadow_->GetHardwareAcceleration()) {
        if (properties.GetShadowElevation() <= 0.f) {
            return;
        }
        float elevation = properties.GetShadowElevation() + DEFAULT_TRANSLATION_Z;

        float userTransRatio = (elevation != DEFAULT_LIGHT_HEIGHT) ?
            elevation / (DEFAULT_LIGHT_HEIGHT - elevation) : MAX_TRANS_RATIO;
        float transRatio = std::max(MIN_TRANS_RATIO, std::min(userTransRatio, MAX_TRANS_RATIO));

        float userSpotRatio = (elevation != DEFAULT_LIGHT_HEIGHT) ?
            DEFAULT_LIGHT_HEIGHT / (DEFAULT_LIGHT_HEIGHT - elevation) : MAX_SPOT_RATIO;
        float spotRatio = std::max(MIN_SPOT_RATIO, std::min(userSpotRatio, MAX_SPOT_RATIO));

        SkRect ambientRect = skPath.getBounds();
        SkRect spotRect = SkRect::MakeLTRB(ambientRect.left() * spotRatio, ambientRect.top() * spotRatio,
            ambientRect.right() * spotRatio, ambientRect.bottom() * spotRatio);
        spotRect.offset(-transRatio * DEFAULT_LIGHT_POSITION_X, -transRatio * DEFAULT_LIGHT_POSITION_Y);
        spotRect.outset(transRatio * DEFAULT_LIGHT_RADIUS, transRatio * DEFAULT_LIGHT_RADIUS);

        shadowRect = ambientRect;
        float ambientBlur = std::min(elevation * 0.5f, MAX_AMBIENT_RADIUS);
        shadowRect.outset(ambientBlur, ambientBlur);
        shadowRect.join(spotRect);
        shadowRect.outset(1, 1);
    } else {
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, properties.GetShadowRadius()));
        if (paint.canComputeFastBounds()) {
            paint.computeFastBounds(shadowRect, &shadowRect);
        }
    }

    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(properties.GetBoundsGeometry());
    SkMatrix matrix = geoPtr ? geoPtr->GetAbsMatrix() : SkMatrix::I();
    matrix.mapRect(&shadowRect);

    dirtyShadow.left_ = shadowRect.left();
    dirtyShadow.top_ = shadowRect.top();
    dirtyShadow.width_ = shadowRect.width();
    dirtyShadow.height_ = shadowRect.height();
}

void RSPropertiesPainter::DrawShadow(const RSProperties& properties, RSPaintFilterCanvas& canvas, const RRect* rrect)
{
    // skip shadow if not valid or cache is enabled
    if (!properties.IsShadowValid() || canvas.isCacheEnabled()) {
        return;
    }
    SkAutoCanvasRestore acr(&canvas, true);
    SkPath skPath;
    if (properties.GetShadowPath() && !properties.GetShadowPath()->GetSkiaPath().isEmpty()) {
        skPath = properties.GetShadowPath()->GetSkiaPath();
        canvas.clipPath(skPath, SkClipOp::kDifference, true);
    } else if (properties.GetClipBounds()) {
        skPath = properties.GetClipBounds()->GetSkiaPath();
        canvas.clipPath(skPath, SkClipOp::kDifference, true);
    } else {
        if (rrect != nullptr) {
            skPath.addRRect(RRect2SkRRect(*rrect));
            canvas.clipRRect(RRect2SkRRect(*rrect), SkClipOp::kDifference, true);
        } else {
            skPath.addRRect(RRect2SkRRect(properties.GetRRect()));
            canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), SkClipOp::kDifference, true);
        }
    }
    skPath.offset(properties.GetShadowOffsetX(), properties.GetShadowOffsetY());
    Color spotColor = properties.GetShadowColor();
    if (properties.shadow_->GetHardwareAcceleration()) {
        if (properties.GetShadowElevation() <= 0.f) {
            return;
        }
        SkPoint3 planeParams = { 0.0f, 0.0f, properties.GetShadowElevation() };
        SkPoint3 lightPos = { canvas.getTotalMatrix().getTranslateX() + skPath.getBounds().centerX(),
            canvas.getTotalMatrix().getTranslateY() + skPath.getBounds().centerY(), DEFAULT_LIGHT_HEIGHT };
        Color ambientColor = Color::FromArgbInt(DEFAULT_AMBIENT_COLOR);
        ambientColor.MultiplyAlpha(canvas.GetAlpha());
        spotColor.MultiplyAlpha(canvas.GetAlpha());
        SkShadowUtils::DrawShadow(&canvas, skPath, planeParams, lightPos, DEFAULT_LIGHT_RADIUS,
            ambientColor.AsArgbInt(), spotColor.AsArgbInt(), SkShadowFlags::kTransparentOccluder_ShadowFlag);
    } else {
        SkPaint paint;
        paint.setColor(spotColor.AsArgbInt());
        paint.setAntiAlias(true);
        paint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, properties.GetShadowRadius()));
        canvas.drawPath(skPath, paint);
    }
}

void RSPropertiesPainter::DrawFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas,
    std::shared_ptr<RSSkiaFilter>& filter, const std::unique_ptr<SkRect>& rect, SkSurface* skSurface)
{
    g_blurCnt++;
    SkAutoCanvasRestore acr(&canvas, true);
    if (rect != nullptr) {
        canvas.clipRect((*rect), true);
    } else if (properties.GetClipBounds() != nullptr) {
        canvas.clipPath(properties.GetClipBounds()->GetSkiaPath(), true);
    } else {
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), true);
    }
    auto paint = filter->GetPaint();
    if (skSurface == nullptr) {
        ROSEN_LOGD("RSPropertiesPainter::DrawFilter skSurface null");
        SkCanvas::SaveLayerRec slr(nullptr, &paint, SkCanvas::kInitWithPrevious_SaveLayerFlag);
        canvas.saveLayer(slr);
        filter->PostProcess(canvas);
        return;
    }

    // canvas draw by snapshot instead of SaveLayer, since the blur layer moves while using saveLayer
    auto imageSnapshot = skSurface->makeImageSnapshot(canvas.getDeviceClipBounds());
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawFilter image null");
        return;
    }

    filter->PreProcess(imageSnapshot);
    auto clipBounds = SkRect::Make(canvas.getDeviceClipBounds());
    canvas.resetMatrix();
    auto visibleRect = canvas.GetVisibleRect();
    if (visibleRect.intersect(clipBounds)) {
        // the snapshot only contains the clip region, so we need to offset the src rect
        canvas.drawImageRect(
            imageSnapshot.get(), visibleRect.makeOffset(-clipBounds.left(), -clipBounds.top()), visibleRect, &paint);
    } else {
        // the snapshot only contains the clip region, so we need to offset the src rect
        canvas.drawImageRect(
            imageSnapshot.get(), clipBounds.makeOffset(-clipBounds.left(), -clipBounds.top()), clipBounds, &paint);
    }
    filter->PostProcess(canvas);
}

SkColor RSPropertiesPainter::CalcAverageColor(sk_sp<SkImage> imageSnapshot)
{
    // create a 1x1 SkPixmap
    uint32_t pixel[1] = { 0 };
    auto single_pixel_info = SkImageInfo::MakeN32Premul(1, 1);
    SkPixmap single_pixel(single_pixel_info, pixel, single_pixel_info.bytesPerPixel());

    // resize snapshot to 1x1 to calculate average color
    // kMedium_SkFilterQuality will do bilerp + mipmaps for down-scaling, we can easily get average color
    imageSnapshot->scalePixels(single_pixel, SkFilterQuality::kMedium_SkFilterQuality);

    // convert color format and return average color
    return SkColor4f::FromBytes_RGBA(pixel[0]).toSkColor();
}

int RSPropertiesPainter::GetAndResetBlurCnt()
{
    auto blurCnt = g_blurCnt;
    g_blurCnt = 0;
    return blurCnt;
}

void RSPropertiesPainter::DrawBackground(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    DrawShadow(properties, canvas);
    // only disable antialias when background is rect and g_forceBgAntiAlias is true
    bool antiAlias = g_forceBgAntiAlias || !properties.GetCornerRadius().IsZero();
    // clip
    if (properties.GetClipBounds() != nullptr) {
        canvas.clipPath(properties.GetClipBounds()->GetSkiaPath(), antiAlias);
    } else if (properties.GetClipToBounds()) {
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), antiAlias);
    }
    // paint backgroundColor
    SkPaint paint;
    paint.setAntiAlias(antiAlias);
    canvas.save();
    auto bgColor = properties.GetBackgroundColor();
    if (bgColor != RgbPalette::Transparent()) {
        paint.setColor(bgColor.AsArgbInt());
        canvas.drawRRect(RRect2SkRRect(properties.GetRRect()), paint);
    } else if (const auto& bgImage = properties.GetBgImage()) {
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), antiAlias);
        auto boundsRect = Rect2SkRect(properties.GetBoundsRect());
        bgImage->SetDstRect(properties.GetBgImageRect());
        bgImage->CanvasDrawImage(canvas, boundsRect, paint, true);
    } else if (const auto& bgShader = properties.GetBackgroundShader()) {
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), antiAlias);
        paint.setShader(bgShader->GetSkShader());
        canvas.drawPaint(paint);
    }
    canvas.restore();
}

void RSPropertiesPainter::SetBgAntiAlias(bool forceBgAntiAlias)
{
    g_forceBgAntiAlias = forceBgAntiAlias;
}

bool RSPropertiesPainter::GetBgAntiAlias()
{
    return g_forceBgAntiAlias;
}

void RSPropertiesPainter::DrawFrame(const RSProperties& properties, RSPaintFilterCanvas& canvas, DrawCmdListPtr& cmds)
{
    if (cmds == nullptr) {
        return;
    }
    SkMatrix mat;
    if (GetGravityMatrix(
            properties.GetFrameGravity(), properties.GetFrameRect(), cmds->GetWidth(), cmds->GetHeight(), mat)) {
        canvas.concat(mat);
    }
    auto frameRect = Rect2SkRect(properties.GetFrameRect());
    // Generate or clear cache on demand
    if (canvas.isCacheEnabled()) {
        cmds->GenerateCache(canvas.GetSurface());
    } else {
        cmds->ClearCache();
    }
    cmds->Playback(canvas, &frameRect);
}

void RSPropertiesPainter::DrawBorder(const RSProperties& properties, SkCanvas& canvas)
{
    auto border = properties.GetBorder();
    if (!border || !border->HasBorder()) {
        return;
    }
    SkPaint paint;
    paint.setAntiAlias(true);
    if (properties.GetCornerRadius().IsZero() && border->ApplyFourLine(paint)) {
        RectF rect = properties.GetBoundsRect();
        border->PaintFourLine(canvas, paint, rect);
    } else if (border->ApplyFillStyle(paint)) {
        canvas.drawDRRect(RRect2SkRRect(properties.GetRRect()), RRect2SkRRect(properties.GetInnerRRect()), paint);
    } else if (border->ApplyPathStyle(paint)) {
        auto borderWidth = border->GetWidth();
        RRect rrect = properties.GetRRect();
        rrect.rect_.width_ -= borderWidth;
        rrect.rect_.height_ -= borderWidth;
        rrect.rect_.Move(borderWidth / PARAM_DOUBLE, borderWidth / PARAM_DOUBLE);
        SkPath borderPath;
        borderPath.addRRect(RRect2SkRRect(rrect));
        canvas.drawPath(borderPath, paint);
    } else {
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.clipRRect(RRect2SkRRect(properties.GetInnerRRect()), SkClipOp::kDifference, true);
        SkRRect rrect = RRect2SkRRect(properties.GetRRect());
        paint.setStyle(SkPaint::Style::kStroke_Style);
        border->PaintTopPath(canvas, paint, rrect);
        border->PaintRightPath(canvas, paint, rrect);
        border->PaintBottomPath(canvas, paint, rrect);
        border->PaintLeftPath(canvas, paint, rrect);
    }
}

void RSPropertiesPainter::DrawForegroundColor(const RSProperties& properties, SkCanvas& canvas)
{
    auto bgColor = properties.GetForegroundColor();
    if (bgColor == RgbPalette::Transparent()) {
        return;
    }
    // clip
    if (properties.GetClipBounds() != nullptr) {
        canvas.clipPath(properties.GetClipBounds()->GetSkiaPath(), true);
    } else if (properties.GetClipToBounds()) {
        canvas.clipRect(Rect2SkRect(properties.GetBoundsRect()), true);
    }

    SkPaint paint;
    paint.setColor(bgColor.AsArgbInt());
    paint.setAntiAlias(true);
    canvas.drawRRect(RRect2SkRRect(properties.GetRRect()), paint);
}

void RSPropertiesPainter::DrawMask(const RSProperties& properties, SkCanvas& canvas, SkRect maskBounds)
{
    std::shared_ptr<RSMask> mask = properties.GetMask();
    if (mask == nullptr) {
        return;
    }
    if (mask->IsSvgMask() && !mask->GetSvgDom() && !mask->GetSvgPicture()) {
        ROSEN_LOGD("RSPropertiesPainter::DrawMask not has Svg Mask property");
        return;
    }

    canvas.save();
    canvas.saveLayer(maskBounds, nullptr);
    int tmpLayer = canvas.getSaveCount();

    SkPaint maskfilter;
    auto filter = SkColorFilters::Compose(SkLumaColorFilter::Make(), SkColorFilters::SRGBToLinearGamma());
    maskfilter.setColorFilter(filter);
    canvas.saveLayer(maskBounds, &maskfilter);
    if (mask->IsSvgMask()) {
        SkAutoCanvasRestore maskSave(&canvas, true);
        canvas.translate(maskBounds.fLeft + mask->GetSvgX(), maskBounds.fTop + mask->GetSvgY());
        canvas.scale(mask->GetScaleX(), mask->GetScaleY());
        if (mask->GetSvgDom()) {
            mask->GetSvgDom()->render(&canvas);
        } else if (mask->GetSvgPicture()) {
            canvas.drawPicture(mask->GetSvgPicture());
        }
    } else if (mask->IsGradientMask()) {
        SkAutoCanvasRestore maskSave(&canvas, true);
        canvas.translate(maskBounds.fLeft, maskBounds.fTop);
        SkRect skRect = SkRect::MakeIWH(maskBounds.fRight - maskBounds.fLeft, maskBounds.fBottom - maskBounds.fTop);
        canvas.drawRect(skRect, mask->GetMaskPaint());
    } else if (mask->IsPathMask()) {
        SkAutoCanvasRestore maskSave(&canvas, true);
        canvas.translate(maskBounds.fLeft, maskBounds.fTop);
        canvas.drawPath(mask->GetMaskPath(), mask->GetMaskPaint());
    }

    // back to mask layer
    canvas.restoreToCount(tmpLayer);
    // create content layer
    SkPaint maskPaint;
    maskPaint.setBlendMode(SkBlendMode::kSrcIn);
    canvas.saveLayer(maskBounds, &maskPaint);
    canvas.clipRect(maskBounds, true);
}

void RSPropertiesPainter::DrawMask(const RSProperties& properties, SkCanvas& canvas)
{
    SkRect maskBounds = Rect2SkRect(properties.GetBoundsRect());
    DrawMask(properties, canvas, maskBounds);
}
} // namespace Rosen
} // namespace OHOS
