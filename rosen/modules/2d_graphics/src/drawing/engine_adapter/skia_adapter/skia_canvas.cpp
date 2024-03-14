/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.. All rights reserved.
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

#include "skia_canvas.h"

#if defined(NEW_SKIA)
#include "modules/svg/include/SkSVGDOM.h"
#else
#include "experimental/svg/model/SkSVGDOM.h"
#endif

#ifdef SUPPORT_OHOS_PIXMAP
#include "pixel_map.h"
#endif
#ifdef ACE_ENABLE_GPU
#include "skia_gpu_context.h"
#endif
#include "skia_image_filter.h"
#include "skia_path.h"
#include "skia_image_info.h"
#include "skia_data.h"
#include "skia_text_blob.h"

#include "draw/core_canvas.h"
#include "draw/canvas.h"
#include "image/bitmap.h"
#include "image/image.h"
#include "utils/log.h"
#include "SkOverdrawCanvas.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaCanvas::SkiaCanvas() : skiaCanvas_(std::make_shared<SkCanvas>()), skiaPaint_()
{
    skCanvas_ = skiaCanvas_.get();
}

SkiaCanvas::SkiaCanvas(const std::shared_ptr<SkCanvas>& skCanvas) : skiaCanvas_(skCanvas), skiaPaint_()
{
    skCanvas_ = skiaCanvas_.get();
}

SkiaCanvas::SkiaCanvas(int32_t width, int32_t height)
    : skiaCanvas_(std::make_shared<SkCanvas>(width, height)), skiaPaint_()
{
    skCanvas_ = skiaCanvas_.get();
}

SkCanvas* SkiaCanvas::ExportSkCanvas() const
{
    return skCanvas_;
}

void SkiaCanvas::ImportSkCanvas(SkCanvas* skCanvas)
{
    skCanvas_ = skCanvas;
}

void SkiaCanvas::Bind(const Bitmap& bitmap)
{
    auto skBitmapImpl = bitmap.GetImpl<SkiaBitmap>();
    if (skBitmapImpl != nullptr) {
        skiaCanvas_ = std::make_shared<SkCanvas>(skBitmapImpl->ExportSkiaBitmap());
        skCanvas_ = skiaCanvas_.get();
    }
}

Matrix SkiaCanvas::GetTotalMatrix() const
{
    if (skCanvas_ == nullptr) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return Matrix();
    }
    auto skMatrix = skCanvas_->getTotalMatrix();
    Matrix matrix;
    matrix.SetMatrix(skMatrix.getScaleX(), skMatrix.getSkewX(), skMatrix.getTranslateX(),
        skMatrix.getSkewY(), skMatrix.getScaleY(), skMatrix.getTranslateY(),
        skMatrix.getPerspX(), skMatrix.getPerspY(), skMatrix.get(SkMatrix::kMPersp2));
    return matrix;
}

Rect SkiaCanvas::GetLocalClipBounds() const
{
    if (skCanvas_ == nullptr) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return Rect();
    }
    auto rect = skCanvas_->getLocalClipBounds();
    return Rect(rect.fLeft, rect.fTop, rect.fRight, rect.fBottom);
}

RectI SkiaCanvas::GetDeviceClipBounds() const
{
    if (skCanvas_ == nullptr) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return RectI();
    }
    auto iRect = skCanvas_->getDeviceClipBounds();
    return RectI(iRect.fLeft, iRect.fTop, iRect.fRight, iRect.fBottom);
}

#ifdef ACE_ENABLE_GPU
std::shared_ptr<GPUContext> SkiaCanvas::GetGPUContext() const
{
    if (skCanvas_ == nullptr || skCanvas_->recordingContext() == nullptr ||
        GrAsDirectContext(skCanvas_->recordingContext()) == nullptr) {
        LOGD("skCanvas_ or grContext is null, return on line %{public}d", __LINE__);
        return nullptr;
    }
    auto grContext = GrAsDirectContext(skCanvas_->recordingContext());

    auto gpuContext = std::make_shared<GPUContext>();
    gpuContext->GetImpl<SkiaGPUContext>()->SetGrContext(sk_ref_sp(grContext));

    return gpuContext;
}
#endif

int32_t SkiaCanvas::GetWidth() const
{
    return (skCanvas_ != nullptr) ? skCanvas_->imageInfo().width() : 0;
}

int32_t SkiaCanvas::GetHeight() const
{
    return (skCanvas_ != nullptr) ? skCanvas_->imageInfo().height() : 0;
}

ImageInfo SkiaCanvas::GetImageInfo()
{
    if (skCanvas_ == nullptr) {
        return {};
    }
    return SkiaImageInfo::ConvertToRSImageInfo(skCanvas_->imageInfo());
}

bool SkiaCanvas::ReadPixels(const ImageInfo& dstInfo, void* dstPixels, size_t dstRowBytes,
    int srcX, int srcY)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return false;
    }
    SkImageInfo info;
    if (dstInfo.GetColorSpace()) {
        info = SkImageInfo::Make(dstInfo.GetWidth(), dstInfo.GetHeight(),
                                 SkiaImageInfo::ConvertToSkColorType(dstInfo.GetColorType()),
                                 SkiaImageInfo::ConvertToSkAlphaType(dstInfo.GetAlphaType()),
                                 dstInfo.GetColorSpace()->GetImpl<SkiaColorSpace>()->GetColorSpace());
    } else {
        info = SkImageInfo::Make(dstInfo.GetWidth(), dstInfo.GetHeight(),
                                 SkiaImageInfo::ConvertToSkColorType(dstInfo.GetColorType()),
                                 SkiaImageInfo::ConvertToSkAlphaType(dstInfo.GetAlphaType()));
    }
    return skCanvas_->readPixels(info, dstPixels, dstRowBytes, srcX, srcY);
}

bool SkiaCanvas::ReadPixels(const Bitmap& dstBitmap, int srcX, int srcY)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return false;
    }
    const SkBitmap& skBitmap = dstBitmap.GetImpl<SkiaBitmap>()->ExportSkiaBitmap();
    return skCanvas_->readPixels(skBitmap, srcX, srcY);
}

void SkiaCanvas::DrawPoint(const Point& point)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }
    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    for (int i = 0; i < paints.count_; i++) {
        const SkPaint* paint = paints.paints_[i];
        skCanvas_->drawPoint(SkPoint::Make(point.GetX(), point.GetY()), *paint);
    }
}

void SkiaCanvas::DrawPoints(PointMode mode, size_t count, const Point pts[])
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }

    SkPoint skPts[count];
    for (size_t i = 0; i < count; ++i) {
        skPts[i] = {pts[i].GetX(), pts[i].GetY()};
    }

    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    for (int i = 0; i < paints.count_; i++) {
        const SkPaint* paint = paints.paints_[i];
        skCanvas_->drawPoints(static_cast<SkCanvas::PointMode>(mode), count, skPts, *paint);
    }
}

void SkiaCanvas::DrawLine(const Point& startPt, const Point& endPt)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }
    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    for (int i = 0; i < paints.count_; i++) {
        SkPaint* paint = paints.paints_[i];
        skCanvas_->drawLine(
            SkPoint::Make(startPt.GetX(), startPt.GetY()), SkPoint::Make(endPt.GetX(), endPt.GetY()), *paint);
    }
}

void SkiaCanvas::DrawRect(const Rect& rect)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }
    SkRect r = SkRect::MakeLTRB(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    for (int i = 0; i < paints.count_; i++) {
        const SkPaint* paint = paints.paints_[i];
        skCanvas_->drawRect(r, *paint);
    }
}

void SkiaCanvas::DrawRoundRect(const RoundRect& roundRect)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }
    SkRRect rRect;
    RoundRectCastToSkRRect(roundRect, rRect);
    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    for (int i = 0; i < paints.count_; i++) {
        const SkPaint* paint = paints.paints_[i];
        skCanvas_->drawRRect(rRect, *paint);
    }
}

void SkiaCanvas::DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }
    SkRRect outerRRect;
    RoundRectCastToSkRRect(outer, outerRRect);

    SkRRect innerRRect;
    RoundRectCastToSkRRect(inner, innerRRect);

    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    for (int i = 0; i < paints.count_; i++) {
        const SkPaint* paint = paints.paints_[i];
        skCanvas_->drawDRRect(outerRRect, innerRRect, *paint);
    }
}

void SkiaCanvas::DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }
    SkRect arcRect = SkRect::MakeLTRB(oval.GetLeft(), oval.GetTop(), oval.GetRight(), oval.GetBottom());
    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    for (int i = 0; i < paints.count_; i++) {
        const SkPaint* paint = paints.paints_[i];
        skCanvas_->drawArc(arcRect, startAngle, sweepAngle, false, *paint);
    }
}

void SkiaCanvas::DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }
    SkRect pieRect = SkRect::MakeLTRB(oval.GetLeft(), oval.GetTop(), oval.GetRight(), oval.GetBottom());
    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    for (int i = 0; i < paints.count_; i++) {
        const SkPaint* paint = paints.paints_[i];
        skCanvas_->drawArc(pieRect, startAngle, sweepAngle, true, *paint);
    }
}

void SkiaCanvas::DrawOval(const Rect& oval)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }
    SkRect ovalRect = SkRect::MakeLTRB(oval.GetLeft(), oval.GetTop(), oval.GetRight(), oval.GetBottom());
    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    for (int i = 0; i < paints.count_; i++) {
        const SkPaint* paint = paints.paints_[i];
        skCanvas_->drawOval(ovalRect, *paint);
    }
}

void SkiaCanvas::DrawCircle(const Point& centerPt, scalar radius)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }
    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    for (int i = 0; i < paints.count_; i++) {
        const SkPaint* paint = paints.paints_[i];
        skCanvas_->drawCircle(centerPt.GetX(), centerPt.GetY(), radius, *paint);
    }
}

void SkiaCanvas::DrawPath(const Path& path)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }
    auto skPathImpl = path.GetImpl<SkiaPath>();
    if (skPathImpl == nullptr) {
        skiaPaint_.Reset();
        return;
    }
    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    for (int i = 0; i < paints.count_; i++) {
        const SkPaint* paint = paints.paints_[i];
        skCanvas_->drawPath(skPathImpl->GetPath(), *paint);
    }
}

void SkiaCanvas::DrawBackground(const Brush& brush)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    SkPaint paint;
    SkiaPaint::BrushToSkPaint(brush, paint);
    skCanvas_->drawPaint(paint);
}

void SkiaCanvas::DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos, scalar lightRadius,
    Color ambientColor, Color spotColor, ShadowFlags flag)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    auto skPathImpl = path.GetImpl<SkiaPath>();
    SkPoint3 point1 = SkPoint3::Make(planeParams.GetX(), planeParams.GetY(), planeParams.GetZ());
    SkPoint3 point2 = SkPoint3::Make(devLightPos.GetX(), devLightPos.GetY(), devLightPos.GetZ());
    SkColor color1 = ambientColor.CastToColorQuad();
    SkColor color2 = spotColor.CastToColorQuad();
    SkShadowFlags flags = static_cast<SkShadowFlags>(flag);
    if (skPathImpl != nullptr) {
        SkShadowUtils::DrawShadow(skCanvas_, skPathImpl->GetPath(), point1, point2, lightRadius, color1, color2, flags);
    }
}

void SkiaCanvas::DrawColor(ColorQuad color, BlendMode mode)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }

    skCanvas_->drawColor(static_cast<SkColor>(color), static_cast<SkBlendMode>(mode));
}

void SkiaCanvas::DrawRegion(const Region& region)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }

    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    for (int i = 0; i < paints.count_; i++) {
        const SkPaint* paint = paints.paints_[i];
        skCanvas_->drawRegion(*region.GetImpl<SkiaRegion>()->GetSkRegion(), *paint);
    }
}

void SkiaCanvas::DrawPatch(const Point cubics[12], const ColorQuad colors[4],
    const Point texCoords[4], BlendMode mode)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }

    const size_t cubicsPointCount = 12;
    std::vector<SkPoint> skiaCubics = {};
    if (cubics != nullptr) {
        skiaCubics.resize(cubicsPointCount);
        for (size_t i = 0; i < cubicsPointCount; ++i) {
            skiaCubics[i].fX = cubics[i].GetX();
            skiaCubics[i].fY = cubics[i].GetY();
        }
    }

    const size_t colorCount = 4;
    std::vector<SkColor> skiaColors = {};
    if (colors != nullptr) {
        skiaColors.resize(colorCount);
        for (size_t i = 0; i < colorCount; ++i) {
            skiaColors[i] = static_cast<SkColor>(colors[i]);
        }
    }

    const size_t texCoordCount = 4;
    std::vector<SkPoint> skiaTexCoords = {};
    if (texCoords != nullptr) {
        skiaTexCoords.resize(texCoordCount);
        for (size_t i = 0; i < texCoordCount; ++i) {
            skiaTexCoords[i].fX = texCoords[i].GetX();
            skiaTexCoords[i].fY = texCoords[i].GetY();
        }
    }

    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    for (int i = 0; i < paints.count_; i++) {
        SkPaint* paint = paints.paints_[i];
        skCanvas_->drawPatch(
            skiaCubics.empty() ? nullptr : skiaCubics.data(),
            skiaColors.empty() ? nullptr : skiaColors.data(),
            skiaTexCoords.empty() ? nullptr : skiaTexCoords.data(),
            static_cast<SkBlendMode>(mode), *paint);
    }
}

void SkiaCanvas::DrawVertices(const Vertices& vertices, BlendMode mode)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }

    sk_sp<SkVertices> verts;
    auto skVerticesImpl = vertices.GetImpl<SkiaVertices>();
    if (skVerticesImpl != nullptr) {
        verts = skVerticesImpl->GetVertices();
    }

    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    for (int i = 0; i < paints.count_; i++) {
        SkPaint* paint = paints.paints_[i];
        skCanvas_->drawVertices(verts, static_cast<SkBlendMode>(mode), *paint);
    }
}

void SkiaCanvas::DrawImageNine(const Image* image, const RectI& center, const Rect& dst,
    FilterMode filter, const Brush* brush)
{
    auto skImageImpl = image->GetImpl<SkiaImage>();
    sk_sp<SkImage> img = nullptr;
    if (skImageImpl != nullptr) {
        img = skImageImpl->GetImage();
        if (img == nullptr) {
            LOGD("img is null, return on line %{public}d", __LINE__);
            return;
        }
    }

    SkIRect skCenter = SkIRect::MakeLTRB(center.GetLeft(), center.GetTop(),
        center.GetRight(), center.GetBottom());
    SkRect skDst = SkRect::MakeLTRB(dst.GetLeft(), dst.GetTop(), dst.GetRight(), dst.GetBottom());

    SkFilterMode skFilterMode = static_cast<SkFilterMode>(filter);

    std::unique_ptr<SkPaint> paint = nullptr;
    if (brush != nullptr) {
        paint = std::make_unique<SkPaint>();
        SkiaPaint::BrushToSkPaint(*brush, *paint);
    }
    skCanvas_->drawImageNine(img.get(), skCenter, skDst, skFilterMode, paint.get());
}

void SkiaCanvas::DrawImageLattice(const Image* image, const Lattice& lattice, const Rect& dst,
    FilterMode filter, const Brush* brush)
{
    auto skImageImpl = image->GetImpl<SkiaImage>();
    sk_sp<SkImage> img = nullptr;
    if (skImageImpl != nullptr) {
        img = skImageImpl->GetImage();
        if (img == nullptr) {
            LOGD("img is null, return on line %{public}d", __LINE__);
            return;
        }
    }
    const SkCanvas::Lattice::RectType skRectType =
        static_cast<const SkCanvas::Lattice::RectType>(lattice.fRectTypes);

    SkIRect skCenter = SkIRect::MakeLTRB(lattice.fBounds.GetLeft(), lattice.fBounds.GetTop(),
        lattice.fBounds.GetRight(), lattice.fBounds.GetBottom());

    SkColor color = lattice.fColors.CastToColorQuad();

    const int xdivs[] = {lattice.fXDivs[0], lattice.fXDivs[1]};
    const int ydivs[] = {lattice.fYDivs[0], lattice.fYDivs[1]};
    SkCanvas::Lattice skLattice = {xdivs, ydivs,
        &skRectType,
        lattice.fXCount, lattice.fYCount,
        &skCenter, &color};
    SkRect skDst = SkRect::MakeLTRB(dst.GetLeft(), dst.GetTop(), dst.GetRight(), dst.GetBottom());

    SkFilterMode skFilterMode = static_cast<SkFilterMode>(filter);

    std::unique_ptr<SkPaint> paint = nullptr;
    if (brush != nullptr) {
        paint = std::make_unique<SkPaint>();
        SkiaPaint::BrushToSkPaint(*brush, *paint);
    }

    skCanvas_->drawImageLattice(img.get(), skLattice, skDst, skFilterMode, paint.get());
}

void SkiaCanvas::DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }
    SkBitmap bmp;

    auto skBitmapImpl = bitmap.GetImpl<SkiaBitmap>();
    if (skBitmapImpl != nullptr) {
        bmp = skBitmapImpl->ExportSkiaBitmap();
    }

    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    if (paints.count_ == 0) {
        skCanvas_->drawImage(bmp.asImage(), px, py);
        return;
    }

    for (int i = 0; i < paints.count_; i++) {
        const SkPaint* paint = paints.paints_[i];
        skCanvas_->drawImage(bmp.asImage(), px, py, SkSamplingOptions(), paint);
    }
}

#ifdef SUPPORT_OHOS_PIXMAP
static sk_sp<SkColorSpace> ColorSpaceToSkColorSpace(Media::PixelMap& pixmap)
{
    return SkColorSpace::MakeSRGB();
}

static SkAlphaType AlphaTypeToSkAlphaType(Media::AlphaType alphaType)
{
    switch (alphaType) {
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN:
            return SkAlphaType::kUnknown_SkAlphaType;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return SkAlphaType::kOpaque_SkAlphaType;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return SkAlphaType::kPremul_SkAlphaType;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return SkAlphaType::kUnpremul_SkAlphaType;
        default:
            return SkAlphaType::kUnknown_SkAlphaType;
    }
}

static SkColorType PixelFormatToSkColorType(Media::PixelFormat format)
{
    switch (format) {
        case Media::PixelFormat::RGB_565:
            return SkColorType::kRGB_565_SkColorType;
        case Media::PixelFormat::RGBA_8888:
            return SkColorType::kRGBA_8888_SkColorType;
        case Media::PixelFormat::BGRA_8888:
            return SkColorType::kBGRA_8888_SkColorType;
        case Media::PixelFormat::ALPHA_8:
            return SkColorType::kAlpha_8_SkColorType;
        case Media::PixelFormat::RGBA_F16:
            return SkColorType::kRGBA_F16_SkColorType;
        case Media::PixelFormat::UNKNOWN:
        case Media::PixelFormat::ARGB_8888:
        case Media::PixelFormat::RGB_888:
        case Media::PixelFormat::NV21:
        case Media::PixelFormat::NV12:
        case Media::PixelFormat::CMYK:
        default:
            return SkColorType::kUnknown_SkColorType;
    }
}

static SkImageInfo MakeSkImageInfoFromPixelMap(Media::PixelMap& pixelMap)
{
    SkColorType ct = PixelFormatToSkColorType(pixelMap.GetPixelFormat());
    SkAlphaType at = AlphaTypeToSkAlphaType(pixelMap.GetAlphaType());
    sk_sp<SkColorSpace> cs = ColorSpaceToSkColorSpace(pixelMap);
    return SkImageInfo::Make(pixelMap.GetWidth(), pixelMap.GetHeight(), ct, at, cs);
}
#endif

void SkiaCanvas::DrawBitmap(Media::PixelMap& pixelMap, const scalar px, const scalar py)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }
#ifdef SUPPORT_OHOS_PIXMAP
    if (pixelMap.GetPixels() == nullptr) {
        LOGD("PutPixelMap failed, pixelMap data invalid");
        skiaPaint_.Reset();
        return;
    }
    SkBitmap bitmap;
    auto imageInfo = MakeSkImageInfoFromPixelMap(pixelMap);
    bitmap.installPixels(imageInfo, (void*)pixelMap.GetPixels(), static_cast<uint32_t>(pixelMap.GetRowBytes()));

    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    if (paints.count_ == 0) {
        skCanvas_->drawImage(bitmap.asImage(), px, py);
        return;
    }

    for (int i = 0; i < paints.count_; i++) {
        SkPaint* paint = paints.paints_[i];
        skCanvas_->drawImage(bitmap.asImage(), px, py, SkSamplingOptions(), paint);
    }
#else
    LOGD("Not support drawing Media::PixelMap");
#endif
}

void SkiaCanvas::DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }
    sk_sp<SkImage> img;

    auto skImageImpl = image.GetImpl<SkiaImage>();
    if (skImageImpl != nullptr) {
        img = skImageImpl->GetImage();
        if (img == nullptr) {
            LOGD("img is null, return on line %{public}d", __LINE__);
            skiaPaint_.Reset();
            return;
        }
    }

    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    if (paints.count_ == 0) {
        skCanvas_->drawImage(img, px, py);
        return;
    }

    SkSamplingOptions samplingOptions;
    if (sampling.GetUseCubic()) {
        samplingOptions = SkSamplingOptions({ sampling.GetCubicCoffB(), sampling.GetCubicCoffC() });
    } else {
        samplingOptions = SkSamplingOptions(static_cast<SkFilterMode>(sampling.GetFilterMode()),
            static_cast<SkMipmapMode>(sampling.GetMipmapMode()));
    }
    for (int i = 0; i < paints.count_; i++) {
        const SkPaint* paint = paints.paints_[i];
        skCanvas_->drawImage(img, px, py, samplingOptions, paint);
    }
}

void SkiaCanvas::DrawImageRect(
    const Image& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling, SrcRectConstraint constraint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }
    sk_sp<SkImage> img;
    auto skImageImpl = image.GetImpl<SkiaImage>();
    if (skImageImpl != nullptr) {
        img = skImageImpl->GetImage();
        if (img == nullptr) {
            LOGD("img is null, return on line %{public}d", __LINE__);
            skiaPaint_.Reset();
            return;
        }
    }

    SkRect srcRect = SkRect::MakeLTRB(src.GetLeft(), src.GetTop(), src.GetRight(), src.GetBottom());
    SkRect dstRect = SkRect::MakeLTRB(dst.GetLeft(), dst.GetTop(), dst.GetRight(), dst.GetBottom());

    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    SkSamplingOptions samplingOptions;
    if (paints.count_ == 0) {
        skCanvas_->drawImageRect(
            img, srcRect, dstRect, samplingOptions, nullptr, static_cast<SkCanvas::SrcRectConstraint>(constraint));
        return;
    }

    if (sampling.GetUseCubic()) {
        samplingOptions = SkSamplingOptions({ sampling.GetCubicCoffB(), sampling.GetCubicCoffC() });
    } else {
        samplingOptions = SkSamplingOptions(static_cast<SkFilterMode>(sampling.GetFilterMode()),
            static_cast<SkMipmapMode>(sampling.GetMipmapMode()));
    }
    for (int i = 0; i < paints.count_; i++) {
        SkPaint* paint = paints.paints_[i];
        skCanvas_->drawImageRect(img, srcRect, dstRect, samplingOptions, paint,
            static_cast<SkCanvas::SrcRectConstraint>(constraint));
    }
}

void SkiaCanvas::DrawImageRect(const Image& image, const Rect& dst, const SamplingOptions& sampling)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }
    sk_sp<SkImage> img;
    auto skImageImpl = image.GetImpl<SkiaImage>();
    if (skImageImpl != nullptr) {
        img = skImageImpl->GetImage();
        if (img == nullptr) {
            LOGD("img is null, return on line %{public}d", __LINE__);
            skiaPaint_.Reset();
            return;
        }
    }

    SkRect dstRect = SkRect::MakeLTRB(dst.GetLeft(), dst.GetTop(), dst.GetRight(), dst.GetBottom());

    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    SkSamplingOptions samplingOptions;
    if (paints.count_ == 0) {
        skCanvas_->drawImageRect(img, dstRect, samplingOptions, nullptr);
        return;
    }

    if (sampling.GetUseCubic()) {
        samplingOptions = SkSamplingOptions({ sampling.GetCubicCoffB(), sampling.GetCubicCoffC() });
    } else {
        samplingOptions = SkSamplingOptions(static_cast<SkFilterMode>(sampling.GetFilterMode()),
            static_cast<SkMipmapMode>(sampling.GetMipmapMode()));
    }
    for (int i = 0; i < paints.count_; i++) {
        SkPaint* paint = paints.paints_[i];
        skCanvas_->drawImageRect(img, dstRect, samplingOptions, paint);
    }
}

void SkiaCanvas::DrawPicture(const Picture& picture)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    sk_sp<SkPicture> p;

    auto skPictureImpl = picture.GetImpl<SkiaPicture>();
    if (skPictureImpl != nullptr) {
        p = skPictureImpl->GetPicture();
        skCanvas_->drawPicture(p.get());
    }
}

void SkiaCanvas::DrawSVGDOM(const sk_sp<SkSVGDOM>& svgDom)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    if (!svgDom) {
        LOGD("svgDom is null, return on line %{public}d", __LINE__);
        return;
    }
    svgDom->render(skCanvas_);
}

void SkiaCanvas::DrawTextBlob(const TextBlob* blob, const scalar x, const scalar y)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }
    if (!blob) {
        LOGD("blob is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }
    auto skiaTextBlob = blob->GetImpl<SkiaTextBlob>();
    if (!skiaTextBlob) {
        LOGD("skiaTextBlob is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }
    SkTextBlob* skTextBlob = skiaTextBlob->GetTextBlob().get();
    if (!skTextBlob) {
        skiaPaint_.Reset();
        LOGD("skTextBlob is null, return on line %{public}d", __LINE__);
        return;
    }
    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    for (int i = 0; i < paints.count_; i++) {
        const SkPaint* paint = paints.paints_[i];
        skCanvas_->drawTextBlob(skTextBlob, x, y, *paint);
    }
}

void SkiaCanvas::DrawSymbol(const DrawingHMSymbolData& symbol, Point locate)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }

    HMSymbolData skSymbol;
    if (!ConvertToHMSymbolData(symbol, skSymbol)) {
        LOGD("ConvertToHMSymbolData failed, return on line %{public}d", __LINE__);
        skiaPaint_.Reset();
        return;
    }

    SkPoint skLocate = SkPoint::Make(locate.GetX(), locate.GetY());

    SortedPaints& paints = skiaPaint_.GetSortedPaints();
    for (int i = 0; i < paints.count_; i++) {
        const SkPaint* paint = paints.paints_[i];
        skCanvas_->drawSymbol(skSymbol, skLocate, *paint);
    }
}

void SkiaCanvas::ClipRect(const Rect& rect, ClipOp op, bool doAntiAlias)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    SkRect clipRect = SkRect::MakeLTRB(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
    SkClipOp clipOp = static_cast<SkClipOp>(op);
    skCanvas_->clipRect(clipRect, clipOp, doAntiAlias);
}

void SkiaCanvas::ClipIRect(const RectI& rect, ClipOp op)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    SkRect clipRect = SkRect::MakeLTRB(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
    SkClipOp clipOp = static_cast<SkClipOp>(op);
    skCanvas_->clipRect(clipRect, clipOp, false);
}

void SkiaCanvas::ClipRoundRect(const RoundRect& roundRect, ClipOp op, bool doAntiAlias)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    SkRRect rRect;
    RoundRectCastToSkRRect(roundRect, rRect);
    SkClipOp clipOp = static_cast<SkClipOp>(op);
    skCanvas_->clipRRect(rRect, clipOp, doAntiAlias);
}

void SkiaCanvas::ClipRoundRect(const Rect& rect, std::vector<Point>& pts, bool doAntiAlias)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    SkRRect rRect;
    rRect.setRectRadii(SkRect::MakeLTRB(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom()),
        reinterpret_cast<const SkVector *>(pts.data()));
    skCanvas_->clipRRect(rRect, doAntiAlias);
}

void SkiaCanvas::ClipPath(const Path& path, ClipOp op, bool doAntiAlias)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    auto skPathImpl = path.GetImpl<SkiaPath>();
    if (skPathImpl != nullptr) {
        SkClipOp clipOp = static_cast<SkClipOp>(op);
        skCanvas_->clipPath(skPathImpl->GetPath(), clipOp, doAntiAlias);
    }
}

void SkiaCanvas::ClipRegion(const Region& region, ClipOp op)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    auto skRegionImpl = region.GetImpl<SkiaRegion>();
    if (skRegionImpl != nullptr) {
        SkClipOp clipOp = static_cast<SkClipOp>(op);
        skCanvas_->clipRegion(*(skRegionImpl->GetSkRegion()), clipOp);
    }
}

bool SkiaCanvas::IsClipEmpty()
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return false;
    }
    return skCanvas_->isClipEmpty();
}

bool SkiaCanvas::IsClipRect()
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return false;
    }
    return skCanvas_->isClipRect();
}

bool SkiaCanvas::QuickReject(const Rect& rect)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return false;
    }
    SkRect clipRect = SkRect::MakeLTRB(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
    return skCanvas_->quickReject(clipRect);
}

void SkiaCanvas::SetMatrix(const Matrix& matrix)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    auto m = matrix.GetImplPtr<SkiaMatrix>();
    if (m != nullptr) {
        skCanvas_->setMatrix(m->ExportSkiaMatrix());
    }
}

void SkiaCanvas::ResetMatrix()
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    skCanvas_->resetMatrix();
}

void SkiaCanvas::ConcatMatrix(const Matrix& matrix)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    auto m = matrix.GetImplPtr<SkiaMatrix>();
    if (m != nullptr) {
        skCanvas_->concat(m->ExportSkiaMatrix());
    }
}

void SkiaCanvas::Translate(scalar dx, scalar dy)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    skCanvas_->translate(dx, dy);
}

void SkiaCanvas::Scale(scalar sx, scalar sy)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    skCanvas_->scale(sx, sy);
}

void SkiaCanvas::Rotate(scalar deg, scalar sx, scalar sy)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    skCanvas_->rotate(deg, sx, sy);
}

void SkiaCanvas::Shear(scalar sx, scalar sy)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    skCanvas_->skew(sx, sy);
}

void SkiaCanvas::Flush()
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    skCanvas_->flush();
}

void SkiaCanvas::Clear(ColorQuad color)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    skCanvas_->clear(color);
}

uint32_t SkiaCanvas::Save()
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return 0;
    }
    return static_cast<uint32_t>(skCanvas_->save());
}

void SkiaCanvas::SaveLayer(const SaveLayerOps& saveLayerOps)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    std::unique_ptr<SkRect> skBounds = nullptr;
    auto bounds = saveLayerOps.GetBounds();
    if (bounds != nullptr) {
        skBounds = std::make_unique<SkRect>();
        skBounds->setLTRB(bounds->GetLeft(), bounds->GetTop(), bounds->GetRight(), bounds->GetBottom());
    }
    std::unique_ptr<SkPaint> paint = nullptr;
    auto brush = saveLayerOps.GetBrush();
    if (brush != nullptr) {
        paint = std::make_unique<SkPaint>();
        SkiaPaint::BrushToSkPaint(*brush, *paint);
    }

    SkCanvas::SaveLayerRec slr(skBounds.get(), paint.get(), nullptr,
        static_cast<SkCanvas::SaveLayerFlags>(saveLayerOps.GetSaveLayerFlags() << 1));  // Offset 1 bit
    skCanvas_->saveLayer(slr);
}

void SkiaCanvas::Restore()
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    skCanvas_->restore();
}

uint32_t SkiaCanvas::GetSaveCount() const
{
    return (skCanvas_ != nullptr) ? skCanvas_->getSaveCount() : 0;
}

void SkiaCanvas::Discard()
{
    skCanvas_->discard();
}

void SkiaCanvas::AttachPaint(const Paint& paint)
{
    skiaPaint_.ApplyPaint(paint);
}

void SkiaCanvas::RoundRectCastToSkRRect(const RoundRect& roundRect, SkRRect& skRRect) const
{
    Rect rect = roundRect.GetRect();
    SkRect outer = SkRect::MakeLTRB(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());

    SkVector radii[4];
    Point p;

    p = roundRect.GetCornerRadius(RoundRect::TOP_LEFT_POS);
    radii[SkRRect::kUpperLeft_Corner] = { p.GetX(), p.GetY() };
    p = roundRect.GetCornerRadius(RoundRect::TOP_RIGHT_POS);
    radii[SkRRect::kUpperRight_Corner] = { p.GetX(), p.GetY() };
    p = roundRect.GetCornerRadius(RoundRect::BOTTOM_RIGHT_POS);
    radii[SkRRect::kLowerRight_Corner] = { p.GetX(), p.GetY() };
    p = roundRect.GetCornerRadius(RoundRect::BOTTOM_LEFT_POS);
    radii[SkRRect::kLowerLeft_Corner] = { p.GetX(), p.GetY() };

    skRRect.setRectRadii(outer, radii);
}

bool SkiaCanvas::ConvertToHMSymbolData(const DrawingHMSymbolData& symbol, HMSymbolData& skSymbol)
{
    Path path = symbol.path_;
    auto skPathImpl = path.GetImpl<SkiaPath>();
    if (skPathImpl == nullptr) {
        return false;
    }
    skSymbol.path_ = skPathImpl->GetPath();

    skSymbol.symbolInfo_.symbolGlyphId = symbol.symbolInfo_.symbolGlyphId;
    skSymbol.symbolInfo_.layers = symbol.symbolInfo_.layers;

    std::vector<RenderGroup> groups;
    auto drawingGroup = symbol.symbolInfo_.renderGroups;
    for (size_t i = 0; i < drawingGroup.size(); i++) {
        RenderGroup group;
        group.color.a = drawingGroup.at(i).color.a;
        group.color.r = drawingGroup.at(i).color.r;
        group.color.g = drawingGroup.at(i).color.g;
        group.color.b = drawingGroup.at(i).color.b;
        auto drawingGroupInfos = drawingGroup.at(i).groupInfos;
        std::vector<GroupInfo> infos;
        for (size_t j = 0; j < drawingGroupInfos.size(); j++) {
            GroupInfo info;
            info.layerIndexes = drawingGroupInfos.at(j).layerIndexes;
            info.maskIndexes = drawingGroupInfos.at(j).maskIndexes;
            infos.push_back(info);
        }
        group.groupInfos = infos;
        groups.push_back(group);
    }
    skSymbol.symbolInfo_.renderGroups = groups;
    return true;
}

void SkiaCanvas::BuildOverDraw(std::shared_ptr<Canvas> canvas)
{
    auto skiaCanvas = canvas->GetImpl<SkiaCanvas>();
    skiaCanvas_ = std::make_shared<SkOverdrawCanvas>(skiaCanvas->ExportSkCanvas());
    skCanvas_ = skiaCanvas_.get();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
