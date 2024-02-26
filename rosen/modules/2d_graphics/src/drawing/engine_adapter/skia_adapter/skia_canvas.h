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
#include "include/utils/SkShadowUtils.h"
#include "skia_bitmap.h"
#include "skia_image.h"
#include "skia_matrix.h"
#include "skia_paint.h"
#include "skia_picture.h"
#include "skia_region.h"
#include "skia_vertices.h"

#include "common/rs_macros.h"
#include "impl_interface/core_canvas_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
#ifndef USE_ROSEN_DRAWING
class RS_EXPORT SkiaCanvas : public CoreCanvasImpl {
#else
class DRAWING_API SkiaCanvas : public CoreCanvasImpl {
#endif
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaCanvas();
    explicit SkiaCanvas(const std::shared_ptr<SkCanvas>& skCanvas);
    SkiaCanvas(int32_t width, int32_t height);
    ~SkiaCanvas() override {};

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    void Bind(const Bitmap& bitmap) override;

    Matrix GetTotalMatrix() const override;
    Rect GetLocalClipBounds() const override;
    RectI GetDeviceClipBounds() const override;
#ifdef ACE_ENABLE_GPU
    std::shared_ptr<GPUContext> GetGPUContext() const override;
#endif
    int32_t GetWidth() const override;
    int32_t GetHeight() const override;
    ImageInfo GetImageInfo() override;
    bool ReadPixels(const ImageInfo& dstInfo, void* dstPixels, size_t dstRowBytes,
        int srcX, int srcY) override;
    bool ReadPixels(const Bitmap& dstBitmap, int srcX, int srcY) override;

    // shapes
    void DrawPoint(const Point& point) override;
    void DrawPoints(PointMode mode, size_t count, const Point pts[]) override;
    void DrawLine(const Point& startPt, const Point& endPt) override;
    void DrawRect(const Rect& rect) override;
    void DrawRoundRect(const RoundRect& roundRect) override;
    void DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner) override;
    void DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle) override;
    void DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle) override;
    void DrawOval(const Rect& oval) override;
    void DrawCircle(const Point& centerPt, scalar radius) override;
    void DrawPath(const Path& path) override;
    void DrawBackground(const Brush& brush) override;
    void DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos, scalar lightRadius,
        Color ambientColor, Color spotColor, ShadowFlags flag) override;
    void DrawRegion(const Region& region) override;
    void DrawPatch(const Point cubics[12], const ColorQuad colors[4],
        const Point texCoords[4], BlendMode mode) override;
    void DrawVertices(const Vertices& vertices, BlendMode mode) override;

    void DrawImageNine(const Image* image, const RectI& center, const Rect& dst,
        FilterMode filter, const Brush* brush = nullptr) override;
    void DrawImageLattice(const Image* image, const Lattice& lattice, const Rect& dst,
        FilterMode filter, const Brush* brush = nullptr) override;

    // color
    void DrawColor(ColorQuad color, BlendMode mode) override;

    // opinc_begin
    bool BeginOpRecording(const Rect* bound = nullptr, bool isDynamic = false) override;
    Drawing::OpListHandle EndOpRecording() override;
    void DrawOpList(Drawing::OpListHandle handle) override;
    int CanDrawOpList(Drawing::OpListHandle handle) override;
    void PreOpListDrawArea(const Matrix& matrix) override;
    bool CanUseOpListDrawArea(Drawing::OpListHandle handle, const Rect* bound = nullptr) override;
    Drawing::OpListHandle GetOpListDrawArea() override;
    void OpincDrawImageRect(const Image& image, Drawing::OpListHandle drawAreas,
        const SamplingOptions& sampling, SrcRectConstraint constraint) override;
    // opinc_end

    // image
    void DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py) override;
    void DrawBitmap(Media::PixelMap& pixelMap, const scalar px, const scalar py) override;
    void DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling) override;
    void DrawImageRect(const Image& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling,
        SrcRectConstraint constraint) override;
    void DrawImageRect(const Image& image, const Rect& dst, const SamplingOptions& sampling) override;
    void DrawPicture(const Picture& picture) override;

    void DrawSVGDOM(const sk_sp<SkSVGDOM>& svgDom) override;

    // text
    void DrawTextBlob(const TextBlob* blob, const scalar x, const scalar y) override;

    // symbol
    void DrawSymbol(const DrawingHMSymbolData& symbol, Point locate) override;

    // clip
    void ClipRect(const Rect& rect, ClipOp op, bool doAntiAlias) override;
    void ClipIRect(const RectI& rect, ClipOp op = ClipOp::INTERSECT) override;
    void ClipRoundRect(const RoundRect& roundRect, ClipOp op, bool doAntiAlias) override;
    void ClipRoundRect(const Rect& rect, std::vector<Point>& pts, bool doAntiAlias) override;
    void ClipPath(const Path& path, ClipOp op, bool doAntiAlias) override;
    void ClipRegion(const Region& region, ClipOp op = ClipOp::INTERSECT) override;
    bool IsClipEmpty() override;
    bool IsClipRect() override;
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

    // paint
    void AttachPaint(const Paint& paint) override;

    SkCanvas* ExportSkCanvas() const;
    void ImportSkCanvas(SkCanvas* skCanvas);

    void BuildOverDraw(std::shared_ptr<Canvas> canvas) override;

private:
    void RoundRectCastToSkRRect(const RoundRect& roundRect, SkRRect& skRRect) const;
    bool ConvertToHMSymbolData(const DrawingHMSymbolData& symbol, HMSymbolData& skSymbol);
    std::shared_ptr<SkCanvas> skiaCanvas_;
    SkCanvas* skCanvas_;
    SkiaPaint skiaPaint_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
