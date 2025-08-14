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

#include "modules/svg/include/SkSVGDOM.h"

#ifdef RS_ENABLE_GPU
#include "skia_gpu_context.h"
#endif
#include "skia_convert_utils.h"
#include "skia_image_filter.h"
#include "skia_path.h"
#include "skia_image_info.h"
#include "skia_data.h"
#include "skia_text_blob.h"
#include "skia_surface.h"
#include "skia_canvas_autocache.h"
#include "skia_oplist_handle.h"

#include "draw/core_canvas.h"
#include "draw/canvas.h"
#include "image/bitmap.h"
#include "image/image.h"
#include "utils/log.h"
#include "utils/performanceCaculate.h"
#include "SkOverdrawCanvas.h"
#include "include/utils/SkNoDrawCanvas.h"
#include "src/core/SkCanvasPriv.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static constexpr int32_t MAX_PARA_LEN = 15;
SkiaCanvas::SkiaCanvas() : skiaCanvas_(std::make_shared<SkCanvas>())
{
    skCanvas_ = skiaCanvas_.get();
}

SkiaCanvas::SkiaCanvas(DrawingType type)
{
    switch (type) {
        case DrawingType::OVER_DRAW:
        case DrawingType::NO_DRAW:
            skiaCanvas_ = nullptr;
            skCanvas_ = nullptr;
            break;
        default:
            SkiaCanvas();
    }
}

SkiaCanvas::SkiaCanvas(const std::shared_ptr<SkCanvas>& skCanvas) : skiaCanvas_(skCanvas)
{
    skCanvas_ = skiaCanvas_.get();
}

SkiaCanvas::SkiaCanvas(int32_t width, int32_t height)
    : skiaCanvas_(std::make_shared<SkCanvas>(width, height))
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
    Matrix matrix;
    matrix.GetImpl<SkiaMatrix>()->ImportMatrix(skCanvas_->getTotalMatrix());
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

void SkiaCanvas::InheriteState(Canvas* canvas)
{
}

void SkiaCanvas::RecordState(Canvas* canvas)
{
    LOGD("skia does not support RecordState.");
}

void SkiaCanvas::SetParallelRender(bool parallelEnable)
{
    LOGD("skia does not support subtree parallel render.");
}

void SkiaCanvas::BuildStateRecord(int32_t width, int32_t height)
{
    LOGD("skia does not support build state record.");
}

RectI SkiaCanvas::GetRoundInDeviceClipBounds() const
{
    if (skCanvas_ == nullptr) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return RectI();
    }
    auto iRect = skCanvas_->getRoundInDeviceClipBounds();
    return RectI(iRect.fLeft, iRect.fTop, iRect.fRight, iRect.fBottom);
}

#ifdef RS_ENABLE_GPU
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
    auto colorSpace = dstInfo.GetColorSpace();
    auto colorSpaceImpl = colorSpace ? colorSpace->GetImpl<SkiaColorSpace>() : nullptr;
    SkImageInfo info = SkImageInfo::Make(dstInfo.GetWidth(), dstInfo.GetHeight(),
                                         SkiaImageInfo::ConvertToSkColorType(dstInfo.GetColorType()),
                                         SkiaImageInfo::ConvertToSkAlphaType(dstInfo.GetAlphaType()),
                                         colorSpaceImpl ? colorSpaceImpl->GetColorSpace() : nullptr);
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

bool SkiaCanvas::AddSdfPara(SkRuntimeShaderBuilder& builder, const SDFShapeBase& shape)
{
    std::vector<float> para = shape.GetPara();
    std::vector<float> transPara = shape.GetTransPara();
    uint64_t transCount = transPara.size();
    uint64_t paraCount = para.size();
    for (uint64_t i = 1; i <= paraCount; i++) {
        char buf[MAX_PARA_LEN] = {0}; // maximum length of string needed is 10.
        if (sprintf_s(buf, sizeof(buf), "para%lu", i) != -1) {
            builder.uniform(buf) = para[i-1];
        } else {
            LOGE("sdf splicing para error.");
            return false;
        }
    }
    for (uint64_t i = 1; i <= transCount; i++) {
        char buf[MAX_PARA_LEN] = {0}; // maximum length of string needed is 15.
        if (sprintf_s(buf, sizeof(buf), "transpara%lu", i) != -1) {
            builder.uniform(buf) = transPara[i-1];
        } else {
            LOGE("sdf splicing para error.");
            return false;
        }
    }
    std::vector<float> color = shape.GetColorPara();
    builder.uniform("sdfalpha") = color[0]; // color_[0] is color alpha channel.
    for (uint64_t i = 1; i < color.size(); i++) {
        char buf[MAX_PARA_LEN] = {0}; // maximum length of string needed is 15.
        if (sprintf_s(buf, sizeof(buf), "colpara%lu", i) != -1) {
            builder.uniform(buf) = color[i];
        } else {
            LOGE("sdf splicing para error.");
            return false;
        }
    }
    builder.uniform("sdfsize") = shape.GetSize();
    builder.uniform("filltype") = shape.GetFillType();
    builder.uniform("translatex") = shape.GetTranslateX();
    builder.uniform("translatey") = shape.GetTranslateY();
    return true;
}

void SkiaCanvas::DrawSdf(const SDFShapeBase& shape)
{
    std::string shaderString = shape.Getshader();
    if (skCanvas_ == nullptr || skCanvas_->getSurface() == nullptr || shaderString.size() == 0) {
        LOGE("skCanvas_ or surface is null, or sdf shape is empty. return on line %{public}d", __LINE__);
        return;
    }

    SkAutoCanvasRestore acr(skCanvas_, true);
    auto [effect, err] = SkRuntimeEffect::MakeForShader(static_cast<SkString>(shaderString));
    if (effect == nullptr) {
        LOGE("sdf make shader fail : %{public}s", err.c_str());
        return;
    }
    float width = skCanvas_->imageInfo().width();
    SkRuntimeShaderBuilder builder(effect);
    if (shape.GetParaNum() <= 0 || !AddSdfPara(builder, shape)) {
        LOGE("sdf para error.");
        return;
    }
    builder.uniform("width") = width;
#ifndef USE_M133_SKIA
    auto shader = builder.makeShader(nullptr, false);
#else
    auto shader = builder.makeShader(nullptr);
#endif
    SkPaint paint;
    paint.setShader(shader);
    skCanvas_->drawPaint(paint);
}

void SkiaCanvas::DrawPoint(const Point& point, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    const SkPoint* skPoint = reinterpret_cast<const SkPoint*>(&point);
    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    DRAWING_PERFORMANCE_TEST_SKIA_NO_PARAM_RETURN;
    skCanvas_->drawPoint(*skPoint, skPaint_);
}

void SkiaCanvas::DrawPoints(PointMode mode, size_t count, const Point pts[], const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }

    const SkPoint* skPts = reinterpret_cast<const SkPoint*>(pts);
    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    skCanvas_->drawPoints(static_cast<SkCanvas::PointMode>(mode), count, skPts, skPaint_);
}

void SkiaCanvas::DrawLine(const Point& startPt, const Point& endPt, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    const SkPoint* skStartPt = reinterpret_cast<const SkPoint*>(&startPt);
    const SkPoint* skEndPt = reinterpret_cast<const SkPoint*>(&endPt);
    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    DRAWING_PERFORMANCE_TEST_SKIA_NO_PARAM_RETURN;
    skCanvas_->drawLine(*skStartPt, *skEndPt, skPaint_);
}

void SkiaCanvas::DrawRect(const Rect& rect, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    const SkRect* skRect = reinterpret_cast<const SkRect*>(&rect);
    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    DRAWING_PERFORMANCE_TEST_SKIA_NO_PARAM_RETURN;
    skCanvas_->drawRect(*skRect, skPaint_);
}

void SkiaCanvas::DrawRoundRect(const RoundRect& roundRect, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    SkRRect rRect;
    RoundRectCastToSkRRect(roundRect, rRect);
    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    skCanvas_->drawRRect(rRect, skPaint_);
}

void SkiaCanvas::DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    SkRRect outerRRect;
    RoundRectCastToSkRRect(outer, outerRRect);

    SkRRect innerRRect;
    RoundRectCastToSkRRect(inner, innerRRect);

    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    skCanvas_->drawDRRect(outerRRect, innerRRect, skPaint_);
}

void SkiaCanvas::DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    const SkRect* arcRect = reinterpret_cast<const SkRect*>(&oval);
    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    skCanvas_->drawArc(*arcRect, startAngle, sweepAngle, false, skPaint_);
}

void SkiaCanvas::DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    const SkRect* pieRect = reinterpret_cast<const SkRect*>(&oval);
    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    skCanvas_->drawArc(*pieRect, startAngle, sweepAngle, true, skPaint_);
}

void SkiaCanvas::DrawOval(const Rect& oval, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    const SkRect* ovalRect = reinterpret_cast<const SkRect*>(&oval);
    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    skCanvas_->drawOval(*ovalRect, skPaint_);
}

void SkiaCanvas::DrawCircle(const Point& centerPt, scalar radius, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    DRAWING_PERFORMANCE_TEST_SKIA_NO_PARAM_RETURN;
    skCanvas_->drawCircle(centerPt.GetX(), centerPt.GetY(), radius, skPaint_);
}

void SkiaCanvas::DrawPath(const Path& path, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    auto skPathImpl = path.GetImpl<SkiaPath>();
    if (skPathImpl == nullptr) {
        return;
    }
    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    DRAWING_PERFORMANCE_TEST_SKIA_NO_PARAM_RETURN;
    skCanvas_->drawPath(skPathImpl->GetPath(), skPaint_);
}

void SkiaCanvas::DrawPathWithStencil(const Path& path, uint32_t stencilVal, const Paint& paint)
{
#ifdef SK_ENABLE_STENCIL_CULLING_OHOS
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    auto skPathImpl = path.GetImpl<SkiaPath>();
    if (skPathImpl == nullptr) {
        LOGE("skPathImpl is null, return on line %{public}d", __LINE__);
        return;
    }
    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    DRAWING_PERFORMANCE_TEST_SKIA_NO_PARAM_RETURN;
    skCanvas_->drawPathWithStencil(skPathImpl->GetPath(), skPaint_, stencilVal);
#else
    (void)stencilVal;
    DrawPath(path, paint);
#endif
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
    const SkPoint3* point1 = reinterpret_cast<const SkPoint3*>(&planeParams);
    const SkPoint3* point2 = reinterpret_cast<const SkPoint3*>(&devLightPos);
    SkColor color1 = ambientColor.CastToColorQuad();
    SkColor color2 = spotColor.CastToColorQuad();
    SkShadowFlags flags = static_cast<SkShadowFlags>(flag);
    if (skPathImpl != nullptr) {
        SkShadowUtils::DrawShadow(
            skCanvas_, skPathImpl->GetPath(), *point1, *point2, lightRadius, color1, color2, flags);
    }
}

void SkiaCanvas::DrawShadowStyle(const Path& path, const Point3& planeParams, const Point3& devLightPos,
    scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag, bool isLimitElevation)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    auto skPathImpl = path.GetImpl<SkiaPath>();
    const SkPoint3* point1 = reinterpret_cast<const SkPoint3*>(&planeParams);
    const SkPoint3* point2 = reinterpret_cast<const SkPoint3*>(&devLightPos);
    SkColor color1 = ambientColor.CastToColorQuad();
    SkColor color2 = spotColor.CastToColorQuad();
    SkShadowFlags flags = static_cast<SkShadowFlags>(flag);
    if (skPathImpl != nullptr) {
        SkShadowUtils::DrawShadowStyle(
            skCanvas_, skPathImpl->GetPath(), *point1, *point2, lightRadius, color1, color2, flags, isLimitElevation);
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

void SkiaCanvas::DrawRegion(const Region& region, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }

    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    skCanvas_->drawRegion(*region.GetImpl<SkiaRegion>()->GetSkRegion(), skPaint_);
}

void SkiaCanvas::DrawPatch(const Point cubics[12], const ColorQuad colors[4],
    const Point texCoords[4], BlendMode mode, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }

    const size_t cubicsPointCount = 12;
    std::vector<SkPoint> skiaCubics = {};
    if (cubics != nullptr) {
        skiaCubics.resize(cubicsPointCount);
        // Tell compiler there is no alias, and unroll 2 times to 128 bits to use vector instructions.
        for (size_t i = 0; i < cubicsPointCount; i += 2) {
            scalar fX0 = cubics[i].GetX();
            scalar fY0 = cubics[i].GetY();
            scalar fX1 = cubics[i + 1].GetX();
            scalar fY1 = cubics[i + 1].GetY();
            skiaCubics[i].fX = fX0;
            skiaCubics[i].fY = fY0;
            skiaCubics[i + 1].fX = fX1;
            skiaCubics[i + 1].fY = fY1;
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
            scalar fX0 = texCoords[i].GetX();
            scalar fY0 = texCoords[i].GetY();
            skiaTexCoords[i].fX = fX0;
            skiaTexCoords[i].fY = fY0;
        }
    }

    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    skCanvas_->drawPatch(
        skiaCubics.empty() ? nullptr : skiaCubics.data(),
        skiaColors.empty() ? nullptr : skiaColors.data(),
        skiaTexCoords.empty() ? nullptr : skiaTexCoords.data(),
        static_cast<SkBlendMode>(mode), skPaint_);
}

void SkiaCanvas::DrawVertices(const Vertices& vertices, BlendMode mode, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }

    sk_sp<SkVertices> verts;
    auto skVerticesImpl = vertices.GetImpl<SkiaVertices>();
    if (skVerticesImpl != nullptr) {
        verts = skVerticesImpl->GetVertices();
    }

    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    skCanvas_->drawVertices(verts, static_cast<SkBlendMode>(mode), skPaint_);
}

void SkiaCanvas::DrawImageNine(const Image* image, const RectI& center, const Rect& dst,
    FilterMode filter, const Brush* brush)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }

    if (!image) {
        LOGD("image is null, return on line %{public}d", __LINE__);
        return;
    }

    auto skImageImpl = image->GetImpl<SkiaImage>();
    sk_sp<SkImage> img = nullptr;
    if (skImageImpl != nullptr) {
        img = skImageImpl->GetImage();
        if (img == nullptr) {
            LOGD("img is null, return on line %{public}d", __LINE__);
            return;
        }
    }

    const SkIRect* skCenter = reinterpret_cast<const SkIRect*>(&center);
    const SkRect* skDst = reinterpret_cast<const SkRect*>(&dst);

    SkFilterMode skFilterMode = static_cast<SkFilterMode>(filter);

    std::unique_ptr<SkPaint> paint = nullptr;
    if (brush != nullptr) {
        paint = std::make_unique<SkPaint>();
        SkiaPaint::BrushToSkPaint(*brush, *paint);
    }
    skCanvas_->drawImageNine(img.get(), *skCenter, *skDst, skFilterMode, paint.get());
}

void SkiaCanvas::DrawImageLattice(const Image* image, const Lattice& lattice, const Rect& dst,
    FilterMode filter, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }

    if (!image) {
        LOGD("image is null, return on line %{public}d", __LINE__);
        return;
    }

    auto skImageImpl = image->GetImpl<SkiaImage>();
    sk_sp<SkImage> img = nullptr;
    if (skImageImpl != nullptr) {
        img = skImageImpl->GetImage();
        if (img == nullptr) {
            LOGD("img is null, return on line %{public}d", __LINE__);
            return;
        }
    }

    int count = (lattice.fXCount + 1) * (lattice.fYCount + 1);
    std::vector<SkCanvas::Lattice::RectType> skRectTypes = {};
    if (!lattice.fRectTypes.empty()) {
        skRectTypes.resize(count);
        for (int i = 0; i < count; ++i) {
            skRectTypes[i] = static_cast<SkCanvas::Lattice::RectType>(lattice.fRectTypes[i]);
        }
    }
    std::vector<SkColor> skColors = {};
    if (!lattice.fColors.empty()) {
        skColors.resize(count);
        for (int i = 0; i < count; ++i) {
            skColors[i] = static_cast<SkColor>(lattice.fColors[i].CastToColorQuad());
        }
    }
    SkCanvas::Lattice skLattice = {lattice.fXDivs.empty() ? nullptr : lattice.fXDivs.data(),
        lattice.fYDivs.empty() ? nullptr : lattice.fYDivs.data(),
        skRectTypes.empty() ? nullptr : skRectTypes.data(),
        lattice.fXCount, lattice.fYCount,
        lattice.fBounds.empty() ? nullptr : reinterpret_cast<const SkIRect*>(lattice.fBounds.data()),
        skColors.empty() ? nullptr : skColors.data()};

    const SkRect* skDst = reinterpret_cast<const SkRect*>(&dst);
    SkFilterMode skFilterMode = static_cast<SkFilterMode>(filter);

    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    skCanvas_->drawImageLattice(img.get(), skLattice, *skDst, skFilterMode, &skPaint_);
}

bool SkiaCanvas::OpCalculateBefore(const Matrix& matrix)
{
    if (!skCanvas_) {
        LOGD("opinc before is null");
        return false;
    }
    if (skiaCanvasOp_ || skCanvasBackup_) {
        LOGD("opinc PreOpListDrawArea is nested");
        return false;
    }
    auto m = matrix.GetImpl<SkiaMatrix>();
    if (!m) {
        LOGD("opinc get matrix null");
        return false;
    }
    auto tmp = std::make_shared<SkiaCanvasAutoCache>(skCanvas_);
    tmp->Init(m->ExportSkiaMatrix());
    skiaCanvasOp_ = tmp;
    skCanvasBackup_ = skiaCanvasOp_.get();
    std::swap(skCanvas_, skCanvasBackup_);
    return true;
}

std::shared_ptr<Drawing::OpListHandle> SkiaCanvas::OpCalculateAfter(const Rect& bound)
{
    std::shared_ptr<Drawing::OpListHandle> handle = nullptr;
    if (!skCanvas_ || !skiaCanvasOp_) {
        LOGD("opinc after is null");
        return handle;
    }

    do {
        auto nodeBound = SkRect::MakeLTRB(bound.GetLeft(), bound.GetTop(), bound.GetRight(), bound.GetBottom());
        if (!skiaCanvasOp_->OpCanCache(nodeBound)) {
            LOGD("opinc opCanCache false");
            break;
        }

        int opNum = skiaCanvasOp_->GetOpsNum();
        int percent = skiaCanvasOp_->GetOpsPercent();
        auto& skUnionRect = skiaCanvasOp_->GetOpUnionRect();
        auto& skOpListDrawArea = skiaCanvasOp_->GetOpListDrawArea();
        if (skUnionRect.isEmpty() || opNum == 0 || percent == 0 || skOpListDrawArea.size() == 0) {
            LOGD("opinc opNum is zero");
            break;
        }
        Drawing::Rect unionRect(skUnionRect.left(), skUnionRect.top(), skUnionRect.right(), skUnionRect.bottom());
        std::vector<Drawing::Rect> rects;
        for (auto &item : skOpListDrawArea) {
            rects.emplace_back(Drawing::Rect(item.left(), item.top(), item.right(), item.bottom()));
        }
        Drawing::OpListHandle::OpInfo opinfo{ true, opNum, percent, std::move(unionRect), std::move(rects) };
        handle = std::make_shared<Drawing::OpListHandle>(std::move(opinfo));
    } while (false);

    std::swap(skCanvas_, skCanvasBackup_); // restore canvas
    skiaCanvasOp_ = nullptr;
    skCanvasBackup_ = nullptr;
    return handle;
}

void SkiaCanvas::DrawAtlas(const Image* atlas, const RSXform xform[], const Rect tex[], const ColorQuad colors[],
    int count, BlendMode mode, const SamplingOptions& sampling, const Rect* cullRect, const Paint& paint)
{
    if (!skCanvas_ || !atlas || !xform || !tex) {
        LOGD("skCanvas_ or atlas, xform or tex is null, return on line %{public}d", __LINE__);
        return;
    }
    const int maxCount = 2000; // max count supported is 2000
    if (count <= 0 || count > maxCount) {
        LOGD("invalid count for atlas, return on line %{public}d", __LINE__);
        return;
    }

    auto skImageImpl = atlas->GetImpl<SkiaImage>();
    if (skImageImpl == nullptr) {
        LOGD("skImageImpl is null, return on line %{public}d", __LINE__);
        return;
    }
    sk_sp<SkImage> img = skImageImpl->GetImage();
    if (img == nullptr) {
        LOGD("img is null, return on line %{public}d", __LINE__);
        return;
    }

    SkRSXform skRSXform[count];
    SkRect skTex[count];
    for (int i = 0; i < count; ++i) {
        SkiaConvertUtils::DrawingRSXformCastToSkXform(xform[i], skRSXform[i]);
        SkiaConvertUtils::DrawingRectCastToSkRect(tex[i], skTex[i]);
    }

    std::vector<SkColor> skColors = {};
    if (colors != nullptr) {
        skColors.resize(count);
        for (int i = 0; i < count; ++i) {
            skColors[i] = static_cast<SkColor>(colors[i]);
        }
    }

#ifdef USE_M133_SKIA
    SamplingOptionsUtils tempSamplingOptions;
    ConvertSamplingOptions(tempSamplingOptions, sampling);
    const SkSamplingOptions* samplingOptions = reinterpret_cast<const SkSamplingOptions*>(&tempSamplingOptions);
#else
    const SkSamplingOptions* samplingOptions = reinterpret_cast<const SkSamplingOptions*>(&sampling);
#endif
    const SkRect* skCullRect = reinterpret_cast<const SkRect*>(cullRect);
    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    skCanvas_->drawAtlas(img.get(), skRSXform, skTex, skColors.empty() ? nullptr : skColors.data(), count,
        static_cast<SkBlendMode>(mode), *samplingOptions, skCullRect, &skPaint_);
}

void SkiaCanvas::DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    SkBitmap bmp;
    auto skBitmapImpl = bitmap.GetImpl<SkiaBitmap>();
    if (skBitmapImpl != nullptr) {
        bmp = skBitmapImpl->ExportSkiaBitmap();
    }

    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    skCanvas_->drawImage(bmp.asImage(), px, py, SkSamplingOptions(), &skPaint_);
}

void SkiaCanvas::DrawImage(const Image& image, const scalar px, const scalar py,
    const SamplingOptions& sampling, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    sk_sp<SkImage> img;

    auto skImageImpl = image.GetImpl<SkiaImage>();
    if (skImageImpl != nullptr) {
        img = skImageImpl->GetImage();
        if (img == nullptr) {
            LOGD("img is null, return on line %{public}d", __LINE__);
            return;
        }
    }

#ifdef USE_M133_SKIA
    SamplingOptionsUtils tempSamplingOptions;
    ConvertSamplingOptions(tempSamplingOptions, sampling);
    const SkSamplingOptions* samplingOptions = reinterpret_cast<const SkSamplingOptions*>(&tempSamplingOptions);
#else
    const SkSamplingOptions* samplingOptions = reinterpret_cast<const SkSamplingOptions*>(&sampling);
#endif
    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    skCanvas_->drawImage(img, px, py, *samplingOptions, &skPaint_);
}

void SkiaCanvas::DrawImageWithStencil(const Image& image, const scalar px, const scalar py,
    const SamplingOptions& sampling, uint32_t stencilVal, const Paint& paint)
{
#ifdef SK_ENABLE_STENCIL_CULLING_OHOS
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }

    sk_sp<SkImage> img;
    auto skImageImpl = image.GetImpl<SkiaImage>();
    if (skImageImpl != nullptr) {
        img = skImageImpl->GetImage();
    }
    if (img == nullptr) {
        LOGD("img is null, return on line %{public}d", __LINE__);
        return;
    }

#ifdef USE_M133_SKIA
    SamplingOptionsUtils tempSamplingOptions;
    ConvertSamplingOptions(tempSamplingOptions, sampling);
    const SkSamplingOptions* samplingOptions = reinterpret_cast<const SkSamplingOptions*>(&tempSamplingOptions);
#else
    const SkSamplingOptions* samplingOptions = reinterpret_cast<const SkSamplingOptions*>(&sampling);
#endif
    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    skCanvas_->drawImageWithStencil(img, px, py, *samplingOptions, &skPaint_, stencilVal);
#else
    (void)stencilVal;
    DrawImage(image, px, py, sampling, paint);
#endif
}

void SkiaCanvas::DrawImageRect(const Image& image, const Rect& src, const Rect& dst,
    const SamplingOptions& sampling, SrcRectConstraint constraint, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    sk_sp<SkImage> img;
    auto skImageImpl = image.GetImpl<SkiaImage>();
    if (skImageImpl != nullptr) {
        img = skImageImpl->GetImage();
        if (img == nullptr) {
            LOGD("img is null, return on line %{public}d", __LINE__);
            return;
        }
    }

    const SkRect* srcRect = reinterpret_cast<const SkRect*>(&src);
    const SkRect* dstRect = reinterpret_cast<const SkRect*>(&dst);
#ifdef USE_M133_SKIA
    SamplingOptionsUtils tempSamplingOptions;
    ConvertSamplingOptions(tempSamplingOptions, sampling);
    const SkSamplingOptions* samplingOptions = reinterpret_cast<const SkSamplingOptions*>(&tempSamplingOptions);
#else
    const SkSamplingOptions* samplingOptions = reinterpret_cast<const SkSamplingOptions*>(&sampling);
#endif

    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    skCanvas_->drawImageRect(img, *srcRect, *dstRect, *samplingOptions, &skPaint_,
        static_cast<SkCanvas::SrcRectConstraint>(constraint));
}

void SkiaCanvas::DrawImageRect(const Image& image, const Rect& dst,
    const SamplingOptions& sampling, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    sk_sp<SkImage> img;
    auto skImageImpl = image.GetImpl<SkiaImage>();
    if (skImageImpl != nullptr) {
        img = skImageImpl->GetImage();
        if (img == nullptr) {
            LOGD("img is null, return on line %{public}d", __LINE__);
            return;
        }
    }

    const SkRect* dstRect = reinterpret_cast<const SkRect*>(&dst);
#ifdef USE_M133_SKIA
    SamplingOptionsUtils tempSamplingOptions;
    ConvertSamplingOptions(tempSamplingOptions, sampling);
    const SkSamplingOptions* samplingOptions = reinterpret_cast<const SkSamplingOptions*>(&tempSamplingOptions);
#else
    const SkSamplingOptions* samplingOptions = reinterpret_cast<const SkSamplingOptions*>(&sampling);
#endif
    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    skCanvas_->drawImageRect(img, *dstRect, *samplingOptions, &skPaint_);
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

void SkiaCanvas::DrawTextBlob(const TextBlob* blob, const scalar x, const scalar y, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    if (!blob) {
        LOGD("blob is null, return on line %{public}d", __LINE__);
        return;
    }
    auto skiaTextBlob = blob->GetImpl<SkiaTextBlob>();
    if (!skiaTextBlob) {
        LOGD("skiaTextBlob is null, return on line %{public}d", __LINE__);
        return;
    }
    SkTextBlob* skTextBlob = skiaTextBlob->GetTextBlob().get();
    if (!skTextBlob) {
        LOGD("skTextBlob is null, return on line %{public}d", __LINE__);
        return;
    }

    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    skCanvas_->drawTextBlob(skTextBlob, x, y, skPaint_);
}

void SkiaCanvas::DrawSymbol(const DrawingHMSymbolData& symbol, Point locate, const Paint& paint)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }

#ifndef TODO_M133_SKIA
    HMSymbolData skSymbol;
    if (!ConvertToHMSymbolData(symbol, skSymbol)) {
        LOGD("ConvertToHMSymbolData failed, return on line %{public}d", __LINE__);
        return;
    }

    const SkPoint* skLocate = reinterpret_cast<const SkPoint*>(&locate);

    skPaint_ = defaultPaint_;
    SkiaPaint::PaintToSkPaint(paint, skPaint_);
    skCanvas_->drawSymbol(skSymbol, *skLocate, skPaint_);
#endif
}

void SkiaCanvas::ClearStencil(const RectI& rect, uint32_t stencilVal)
{
#ifdef SK_ENABLE_STENCIL_CULLING_OHOS
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    const SkIRect* skIRect = reinterpret_cast<const SkIRect*>(&rect);
    skCanvas_->clearStencil(*skIRect, stencilVal);
#else
    (void)rect;
    (void)stencilVal;
#endif
}

void SkiaCanvas::ClipRect(const Rect& rect, ClipOp op, bool doAntiAlias)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    const SkRect* clipRect = reinterpret_cast<const SkRect*>(&rect);
    SkClipOp clipOp = static_cast<SkClipOp>(op);
    skCanvas_->clipRect(*clipRect, clipOp, doAntiAlias);
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

void SkiaCanvas::ResetClip()
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    return SkCanvasPriv::ResetClip(skCanvas_);
}

bool SkiaCanvas::QuickReject(const Path& path)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return false;
    }
    const SkPath& clipPath = path.GetImpl<SkiaPath>()->GetPath();
    DRAWING_PERFORMANCE_TEST_SKIA_RETURN(false);
    return skCanvas_->quickReject(clipPath);
}

bool SkiaCanvas::QuickReject(const Rect& rect)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return false;
    }
    const SkRect* clipRect = reinterpret_cast<const SkRect*>(&rect);
    return skCanvas_->quickReject(*clipRect);
}

void SkiaCanvas::SetMatrix(const Matrix& matrix)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    auto m = matrix.GetImpl<SkiaMatrix>();
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
    auto m = matrix.GetImpl<SkiaMatrix>();
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
#ifdef USE_M133_SKIA
    if (auto dContext = GrAsDirectContext(skCanvas_->recordingContext())) {
        dContext->flushAndSubmit();
    }
#else
    skCanvas_->flush();
#endif
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
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return;
    }
    skCanvas_->discard();
}

void SkiaCanvas::RoundRectCastToSkRRect(const RoundRect& roundRect, SkRRect& skRRect) const
{
    const SkRect* outer = reinterpret_cast<const SkRect*>(&roundRect.GetRect());
    if (roundRect.IsSimpleRoundRect()) {
        skRRect.setRectXY(*outer, roundRect.GetSimpleX(), roundRect.GetSimpleY());
        return;
    }

    const SkVector* radii = reinterpret_cast<const SkVector*>(&roundRect.GetCornerRadius(RoundRect::TOP_LEFT_POS));
    skRRect.setRectRadii(*outer, radii);
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

void SkiaCanvas::BuildNoDraw(int32_t width, int32_t height)
{
    skiaCanvas_ = std::make_shared<SkNoDrawCanvas>(width, height);
    skCanvas_ = skiaCanvas_.get();
}

void SkiaCanvas::BuildStateInherite(int32_t width, int32_t height)
{
}

void SkiaCanvas::Reset(int32_t width, int32_t height)
{
    SkNoDrawCanvas* noDraw = reinterpret_cast<SkNoDrawCanvas*>(skCanvas_);
    noDraw->resetCanvas(width, height);
}

bool SkiaCanvas::DrawBlurImage(const Image& image, const Drawing::HpsBlurParameter& blurParams)
{
    if (!skCanvas_) {
        LOGD("skCanvas_ is null, return on line %{public}d", __LINE__);
        return false;
    }

    auto skImageImpl = image.GetImpl<SkiaImage>();
    if (skImageImpl == nullptr) {
        LOGE("skImageImpl is null, return on line %{public}d", __LINE__);
        return false;
    }

    sk_sp<SkImage> img = skImageImpl->GetImage();
    if (img == nullptr) {
        LOGE("img is null, return on line %{public}d", __LINE__);
        return false;
    }

#ifdef USE_M133_SKIA
    return false;
#else
    SkRect srcRect = SkRect::MakeLTRB(blurParams.src.GetLeft(), blurParams.src.GetTop(),
        blurParams.src.GetRight(), blurParams.src.GetBottom());
    SkRect dstRect = SkRect::MakeLTRB(blurParams.dst.GetLeft(), blurParams.dst.GetTop(),
        blurParams.dst.GetRight(), blurParams.dst.GetBottom());

    SkBlurArg blurArg(srcRect, dstRect, blurParams.sigma, blurParams.saturation, blurParams.brightness);
    return skCanvas_->drawBlurImage(img.get(), blurArg);
#endif
}

bool SkiaCanvas::DrawImageEffectHPS(const Image& image,
    const std::vector<std::shared_ptr<Drawing::HpsEffectParameter>>& hpsEffectParams)
{
    LOGD("skia does not support DrawImageEffectHPS");
    return false;
}

std::array<int, 2> SkiaCanvas::CalcHpsBluredImageDimension(const Drawing::HpsBlurParameter& blurParams)
{
    if (!skCanvas_) {
        LOGD("SkiaCanvas::CalcHpsBluredImageDimension, skCanvas_ is nullptr");
        return {0, 0};
    }

#ifdef USE_M133_SKIA
    return {0, 0};
#else
    SkRect srcRect = SkRect::MakeLTRB(blurParams.src.GetLeft(), blurParams.src.GetTop(),
        blurParams.src.GetRight(), blurParams.src.GetBottom());
    SkRect dstRect = SkRect::MakeLTRB(blurParams.dst.GetLeft(), blurParams.dst.GetTop(),
        blurParams.dst.GetRight(), blurParams.dst.GetBottom());

    SkBlurArg blurArg(srcRect, dstRect, blurParams.sigma, blurParams.saturation, blurParams.brightness);
    return skCanvas_->CalcHpsBluredImageDimension(blurArg);
#endif
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
