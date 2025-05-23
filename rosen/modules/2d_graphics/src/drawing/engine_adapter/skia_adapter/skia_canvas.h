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

#ifndef SKIACANVAS_H
#define SKIACANVAS_H

#include "draw/canvas.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/HMSymbol.h"
#include "include/core/SkImage.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPicture.h"
#include "include/core/SkPoint3.h"
#include "include/core/SkRegion.h"
#include "include/core/SkRRect.h"
#include "include/effects/SkRuntimeEffect.h"
#include "include/utils/SkShadowUtils.h"
#include "skia_bitmap.h"
#include "skia_image.h"
#include "skia_matrix.h"
#include "skia_paint.h"
#include "skia_picture.h"
#include "skia_region.h"
#include "skia_vertices.h"
#include "skia_canvas_op.h"

#include "common/rs_macros.h"
#include "impl_interface/core_canvas_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API SkiaCanvas : public CoreCanvasImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaCanvas();
    explicit SkiaCanvas(DrawingType type);
    explicit SkiaCanvas(const std::shared_ptr<SkCanvas>& skCanvas);
    SkiaCanvas(int32_t width, int32_t height);
    ~SkiaCanvas() override {};

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    void Bind(const Bitmap& bitmap) override;

    void InheriteState(Canvas* canvas) override;
    void SetParallelRender(bool parallelEnable) override;

    Matrix GetTotalMatrix() const override;
    Rect GetLocalClipBounds() const override;
    RectI GetDeviceClipBounds() const override;
    RectI GetRoundInDeviceClipBounds() const override;
#ifdef RS_ENABLE_GPU
    std::shared_ptr<GPUContext> GetGPUContext() const override;
#endif
    int32_t GetWidth() const override;
    int32_t GetHeight() const override;
    ImageInfo GetImageInfo() override;
    bool ReadPixels(const ImageInfo& dstInfo, void* dstPixels, size_t dstRowBytes,
        int srcX, int srcY) override;
    bool ReadPixels(const Bitmap& dstBitmap, int srcX, int srcY) override;

    // shapes
    void DrawSdf(const SDFShapeBase& shape) override;
    void DrawPoint(const Point& point, const Paint& paint) override;
    void DrawPoints(PointMode mode, size_t count, const Point pts[], const Paint& paint) override;
    void DrawLine(const Point& startPt, const Point& endPt, const Paint& paint) override;
    void DrawRect(const Rect& rect, const Paint& paint) override;
    void DrawRoundRect(const RoundRect& roundRect, const Paint& paint) override;
    void DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner, const Paint& paint) override;
    void DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle, const Paint& paint) override;
    void DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle, const Paint& paint) override;
    void DrawOval(const Rect& oval, const Paint& paint) override;
    void DrawCircle(const Point& centerPt, scalar radius, const Paint& paint) override;
    void DrawPath(const Path& path, const Paint& paint) override;
    void DrawPathWithStencil(const Path& path, uint32_t stencilVal, const Paint& paint) override;
    void DrawBackground(const Brush& brush) override;
    void DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos, scalar lightRadius,
        Color ambientColor, Color spotColor, ShadowFlags flag) override;
    void DrawShadowStyle(const Path& path, const Point3& planeParams, const Point3& devLightPos, scalar lightRadius,
        Color ambientColor, Color spotColor, ShadowFlags flag, bool isLimitElevation) override;
    void DrawRegion(const Region& region, const Paint& paint) override;
    void DrawPatch(const Point cubics[12], const ColorQuad colors[4],
        const Point texCoords[4], BlendMode mode, const Paint& paint) override;
    void DrawVertices(const Vertices& vertices, BlendMode mode, const Paint& paint) override;

    void DrawImageNine(const Image* image, const RectI& center, const Rect& dst,
        FilterMode filter, const Brush* brush = nullptr) override;
    void DrawImageLattice(const Image* image, const Lattice& lattice, const Rect& dst,
        FilterMode filter, const Paint& paint) override;

    // color
    void DrawColor(ColorQuad color, BlendMode mode) override;

    bool OpCalculateBefore(const Matrix& matrix) override;
    std::shared_ptr<Drawing::OpListHandle> OpCalculateAfter(const Rect& bound) override;

    // image
    void DrawAtlas(const Image* atlas, const RSXform xform[], const Rect tex[], const ColorQuad colors[], int count,
        BlendMode mode, const SamplingOptions& sampling, const Rect* cullRect, const Paint& paint) override;
    void DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py, const Paint& paint) override;
    void DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling,
        const Paint& paint) override;
    void DrawImageWithStencil(const Image& image, const scalar px, const scalar py,
            const SamplingOptions& sampling, uint32_t stencilVal, const Paint& paint) override;
    void DrawImageRect(const Image& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling,
        SrcRectConstraint constraint, const Paint& paint) override;
    void DrawImageRect(const Image& image, const Rect& dst, const SamplingOptions& sampling,
        const Paint& paint) override;
    void DrawPicture(const Picture& picture) override;

    void DrawSVGDOM(const sk_sp<SkSVGDOM>& svgDom) override;

    // text
    void DrawTextBlob(const TextBlob* blob, const scalar x, const scalar y, const Paint& paint) override;

    // symbol
    void DrawSymbol(const DrawingHMSymbolData& symbol, Point locate, const Paint& paint) override;

    // stencil culling
    void ClearStencil(const RectI& rect, uint32_t stencilVal) override;

    // clip
    void ClipRect(const Rect& rect, ClipOp op, bool doAntiAlias) override;
    void ClipIRect(const RectI& rect, ClipOp op = ClipOp::INTERSECT) override;
    void ClipRoundRect(const RoundRect& roundRect, ClipOp op, bool doAntiAlias) override;
    void ClipRoundRect(const Rect& rect, std::vector<Point>& pts, bool doAntiAlias) override;
    void ClipPath(const Path& path, ClipOp op, bool doAntiAlias) override;
    void ClipRegion(const Region& region, ClipOp op = ClipOp::INTERSECT) override;
    bool IsClipEmpty() override;
    bool IsClipRect() override;
    void ResetClip() override;
    bool QuickReject(const Path& path) override;
    bool QuickReject(const Rect& rect) override;

    // transform
    void SetMatrix(const Matrix& matrix) override;
    void ResetMatrix() override;
    void ConcatMatrix(const Matrix& matrix) override;
    void Translate(scalar dx, scalar dy) override;
    void Scale(scalar sx, scalar sy) override;
    void Rotate(scalar deg, scalar sx, scalar sy) override;
    void Shear(scalar sx, scalar sy) override;

    // state
    void Flush() override;
    void Clear(ColorQuad color) override;
    uint32_t Save() override;
    void SaveLayer(const SaveLayerOps& saveLayerOps) override;
    void Restore() override;
    uint32_t GetSaveCount() const override;
    void Discard() override;

    SkCanvas* ExportSkCanvas() const;
    void ImportSkCanvas(SkCanvas* skCanvas);

    void BuildOverDraw(std::shared_ptr<Canvas> canvas) override;

    void BuildNoDraw(int32_t width, int32_t height) override;

    void BuildStateInherite(int32_t width, int32_t height) override;

    void Reset(int32_t width, int32_t height) override;

    void SetGrContextToSkiaImage(SkiaImage* skiaImage);

    bool DrawBlurImage(const Image& image, const Drawing::HpsBlurParameter& blurParams) override;

    std::array<int, 2> CalcHpsBluredImageDimension(const Drawing::HpsBlurParameter& blurParams) override;

private:
    void RoundRectCastToSkRRect(const RoundRect& roundRect, SkRRect& skRRect) const;
    bool ConvertToHMSymbolData(const DrawingHMSymbolData& symbol, HMSymbolData& skSymbol);
    bool AddSdfPara(SkRuntimeShaderBuilder& builder, const SDFShapeBase& shape);
    std::shared_ptr<SkCanvas> skiaCanvas_;
    SkCanvas* skCanvas_;
    SkCanvas* skCanvasBackup_ = nullptr;
    std::shared_ptr<SkiaCanvasOp> skiaCanvasOp_ = nullptr;
    SkPaint defaultPaint_;
    SkPaint skPaint_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
