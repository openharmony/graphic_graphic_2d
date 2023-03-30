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

#include "rs_trace.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColorFilter.h"
#include "include/core/SkMaskFilter.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPoint3.h"
#include "include/core/SkRRect.h"
#include "include/core/SkSurface.h"
#include "include/effects/Sk1DPathEffect.h"
#include "include/effects/SkBlurImageFilter.h"
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

        float userTransRatio =
            (elevation != DEFAULT_LIGHT_HEIGHT) ? elevation / (DEFAULT_LIGHT_HEIGHT - elevation) : MAX_TRANS_RATIO;
        float transRatio = std::max(MIN_TRANS_RATIO, std::min(userTransRatio, MAX_TRANS_RATIO));

        float userSpotRatio = (elevation != DEFAULT_LIGHT_HEIGHT)
                                  ? DEFAULT_LIGHT_HEIGHT / (DEFAULT_LIGHT_HEIGHT - elevation)
                                  : MAX_SPOT_RATIO;
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
    if (properties.IsSpherizeValid() || !properties.IsShadowValid() || canvas.isCacheEnabled()) {
        return;
    }
    RSAutoCanvasRestore acr(&canvas);
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
    if (properties.GetShadowMask()) {
        DrawColorfulShadowInner(properties, canvas, skPath);
    } else {
        DrawShadowInner(properties, canvas, skPath);
    }
}

void RSPropertiesPainter::DrawColorfulShadowInner(
    const RSProperties& properties, RSPaintFilterCanvas& canvas, SkPath& skPath)
{
    // blurRadius calculation is based on the formula in SkShadowUtils::DrawShadow, 0.25f and 128.0f are constants
    const SkScalar blurRadius =
        properties.shadow_->GetHardwareAcceleration()
            ? 0.25f * properties.GetShadowElevation() * (1 + properties.GetShadowElevation() / 128.0f)
            : properties.GetShadowRadius();

    // save layer, draw image with clipPath, blur and draw back
    SkPaint blurPaint;
    blurPaint.setImageFilter(SkBlurImageFilter::Make(blurRadius, blurRadius, SkTileMode::kDecal, nullptr));
    canvas.saveLayer(nullptr, &blurPaint);

    canvas.translate(properties.GetShadowOffsetX(), properties.GetShadowOffsetY());

    canvas.clipPath(skPath);
    // draw node content as shadow
    // [PLANNING]: maybe we should also draw background color / image here, and we should cache the shadow image
    if (auto node = RSBaseRenderNode::ReinterpretCast<RSCanvasRenderNode>(properties.backref_.lock())) {
        node->InternalDrawContent(canvas);
    }
}

void RSPropertiesPainter::DrawShadowInner(const RSProperties& properties, RSPaintFilterCanvas& canvas, SkPath& skPath)
{
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
    RS_TRACE_NAME("DrawFilter:" + std::to_string(filter->GetFilterType()));
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

static Vector4f GetStretchSize(const RSProperties& properties)
{
    Vector4f stretchSize;
    if (properties.IsPixelStretchValid()) {
        stretchSize = properties.GetPixelStretch();
    } else {
        if (properties.IsPixelStretchPercentValid()) {
            stretchSize = properties.GetPixelStretchByPercent();
        }
    }

    return stretchSize;
}

void RSPropertiesPainter::DrawPixelStretch(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    if (!properties.IsPixelStretchValid() && !properties.IsPixelStretchPercentValid()) {
        return;
    }

    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch skSurface null");
        return;
    }

    canvas.save();
    auto bounds = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());
    canvas.clipRect(bounds);
    auto clipBounds = canvas.getDeviceClipBounds();
    clipBounds.setXYWH(clipBounds.left(), clipBounds.top(), clipBounds.width() - 1, clipBounds.height() - 1);
    canvas.restore();

    Vector4f stretchSize = GetStretchSize(properties);
     
    auto scaledBounds = SkRect::MakeLTRB(bounds.left() - stretchSize.x_, bounds.top() - stretchSize.y_,
        bounds.right() + stretchSize.z_, bounds.bottom() + stretchSize.w_);
    if (scaledBounds.isEmpty() || clipBounds.isEmpty()) {
        ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch invalid scaled bounds");
        return;
    }

    auto image = skSurface->makeImageSnapshot(clipBounds);
    if (image == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch image null");
        return;
    }

    SkPaint paint;
    SkMatrix inverseMat, scaleMat;
    auto boundsGeo = std::static_pointer_cast<RSObjAbsGeometry>(properties.GetBoundsGeometry());
    if (boundsGeo && !boundsGeo->IsEmpty()) {
        if (!canvas.getTotalMatrix().invert(&inverseMat)) {
            ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch get inverse matrix failed.");
        }
        scaleMat.setScale(inverseMat.getScaleX(), inverseMat.getScaleY());
    }

    if (properties.IsPixelStretchExpanded()) {
        paint.setShader(image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, &scaleMat));
        canvas.drawRect(SkRect::MakeXYWH(-stretchSize.x_, -stretchSize.y_, scaledBounds.width(), scaledBounds.height()),
            paint);
    } else {
        scaleMat.setScale(scaledBounds.width() / bounds.width(), scaledBounds.height() / bounds.height());
        paint.setShader(image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, &scaleMat));
        canvas.save();
        canvas.translate(-stretchSize.x_, -stretchSize.y_);
        canvas.drawRect(SkRect::MakeXYWH(stretchSize.x_, stretchSize.y_, bounds.width(), bounds.height()), paint);
        canvas.restore();
    }
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

RectF RSPropertiesPainter::GetCmdsClipRect(DrawCmdListPtr& cmds)
{
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    RectF clipRect;
    if (cmds == nullptr) {
        return clipRect;
    }
    SkRect rect;
    cmds->CheckClipRect(rect);
    clipRect = { rect.left(), rect.top(), rect.width(), rect.height() };
    return clipRect;
#else
    return RectF { 0.0f, 0.0f, 0.0f, 0.0f };
#endif
}

void RSPropertiesPainter::DrawFrameForDriven(const RSProperties& properties, RSPaintFilterCanvas& canvas,
                                             DrawCmdListPtr& cmds)
{
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
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
    // temporary solution for driven content clip
    cmds->ReplaceDrivenCmds();
    cmds->Playback(canvas, &frameRect);
    cmds->RestoreOriginCmdsForDriven();
#endif
}

void RSPropertiesPainter::DrawCachedSpherizeSurface(const RSRenderNode& node, RSPaintFilterCanvas& canvas,
    const sk_sp<SkSurface>& cacheSurface)
{
    if (cacheSurface == nullptr) {
        return;
    }
    SkAutoCanvasRestore acr(&canvas, true);
    const RSProperties& properties = node.GetRenderProperties();
    float canvasWidth = properties.GetBoundsRect().GetWidth();
    float canvasHeight = properties.GetBoundsRect().GetHeight();
    if (cacheSurface->width() == 0 || cacheSurface->height() == 0) {
        return;
    }
    canvas.scale(canvasWidth / cacheSurface->width(), canvasHeight / cacheSurface->height());

    auto imageSnapshot = cacheSurface->makeImageSnapshot();
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawCachedSpherizeSurface image  is null");
        return;
    }

    SkPaint paint;
    paint.setBlendMode(SkBlendMode::kSrcOver);
    paint.setShader(imageSnapshot->makeShader(SkTileMode::kClamp, SkTileMode::kClamp));
    float width = imageSnapshot->width();
    float height = imageSnapshot->height();
    float degree = properties.GetSpherize();
    bool isWidthGreater = width > height;
    ROSEN_LOGI("RSPropertiesPainter::DrawCachedSpherizeSurface spherize degree [%f]", degree);

    const SkPoint texCoords[4] = {
        {0.0f, 0.0f}, {width, 0.0f}, {width, height}, {0.0f, height}
    };
    float offsetSquare = 0.f;
    if (isWidthGreater) {
        offsetSquare = (width - height) * degree / 2.0; // half of the change distance
        width = width - (width - height) * degree;
    } else {
        offsetSquare = (height - width) * degree / 2.0; // half of the change distance
        height = height - (height - width) * degree;
    }

    float segmentWidthOne = width / 3.0;
    float segmentWidthTwo = width / 3.0 * 2.0;
    float segmentHeightOne = height / 3.0;
    float segmentHeightTwo = height / 3.0 * 2.0;
    float offsetSphereWidth = width / 6 * degree;
    float offsetSphereHeight = height / 6  * degree;

    SkPoint ctrlPoints[12] = {
        // top edge control points
        {0.0f, 0.0f}, {segmentWidthOne, 0.0f}, {segmentWidthTwo, 0.0f}, {width, 0.0f},
        // right edge control points
        {width, segmentHeightOne}, {width, segmentHeightTwo},
        // bottom edge control points
        {width, height}, {segmentWidthTwo, height}, {segmentWidthOne, height}, {0.0f, height},
        // left edge control points
        {0.0f, segmentHeightTwo}, {0.0f, segmentHeightOne}
    };
    ctrlPoints[0].offset(offsetSphereWidth, offsetSphereHeight); // top left control point
    ctrlPoints[3].offset(-offsetSphereWidth, offsetSphereHeight); // top right control point
    ctrlPoints[6].offset(-offsetSphereWidth, -offsetSphereHeight); // bottom right control point
    ctrlPoints[9].offset(offsetSphereWidth, -offsetSphereHeight); // bottom left control point
    if (isWidthGreater) {
        SkPoint::Offset(ctrlPoints, SK_ARRAY_COUNT(ctrlPoints), offsetSquare, 0);
    } else {
        SkPoint::Offset(ctrlPoints, SK_ARRAY_COUNT(ctrlPoints), 0, offsetSquare);
    }
    SkPath path;
    path.moveTo(ctrlPoints[0]);
    path.cubicTo(ctrlPoints[1], ctrlPoints[2], ctrlPoints[3]); // upper edge
    path.cubicTo(ctrlPoints[4], ctrlPoints[5], ctrlPoints[6]); // right edge
    path.cubicTo(ctrlPoints[7], ctrlPoints[8], ctrlPoints[9]); // bottom edge
    path.cubicTo(ctrlPoints[10], ctrlPoints[11], ctrlPoints[0]); // left edge
    canvas.clipPath(path, true);
    canvas.drawPatch(ctrlPoints, nullptr, texCoords, SkBlendMode::kSrcOver, paint);
}
} // namespace Rosen
} // namespace OHOS
