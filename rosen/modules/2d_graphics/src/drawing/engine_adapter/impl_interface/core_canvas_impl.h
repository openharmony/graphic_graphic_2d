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

#ifndef CORECANVASIMPL_H
#define CORECANVASIMPL_H
#include "base_impl.h"
#include "securec.h"

#include "include/core/SkRefCnt.h"

#include "draw/clip.h"
#include "draw/path.h"
#include "draw/paint.h"
#include "draw/shadow.h"
#include "draw/sdf_shaper_base.h"
#include "effect/filter.h"
#include "draw/OpListHandle.h"
#include "image/bitmap.h"
#include "image/image_info.h"
#ifdef RS_ENABLE_GPU
#include "image/gpu_context.h"
#endif
#include "image/image.h"
#include "image/picture.h"
#include "text/hm_symbol.h"
#include "text/text.h"
#include "text/text_blob.h"
#include "utils/matrix.h"
#include "utils/point.h"
#include "utils/point3.h"
#include "utils/rect.h"
#include "utils/region.h"
#include "utils/round_rect.h"
#include "utils/sampling_options.h"
#include "utils/scalar.h"
#include "utils/vertices.h"
#include "drawing/draw/hps_effect_types.h"

class SkSVGDOM;

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class SrcRectConstraint;
class SaveLayerOps;
enum class PointMode;
struct Lattice;
class Canvas;
struct HpsBlurParameter;
enum class QuadAAFlags;

class CoreCanvasImpl : public BaseImpl {
public:
    CoreCanvasImpl() {};
    ~CoreCanvasImpl() override {};

    virtual void Bind(const Bitmap& bitmap) = 0;

    virtual Matrix GetTotalMatrix() const = 0;
    virtual Rect GetLocalClipBounds() const = 0;
    virtual RectI GetDeviceClipBounds() const = 0;
    virtual RectI GetRoundInDeviceClipBounds() const = 0;
#ifdef RS_ENABLE_GPU
    virtual std::shared_ptr<GPUContext> GetGPUContext() const = 0;
#endif
    virtual void InheriteState(Canvas* canvas) = 0;
    virtual void SetParallelRender(bool parallelEnable) = 0;
    virtual int32_t GetWidth() const = 0;
    virtual int32_t GetHeight() const = 0;
    virtual ImageInfo GetImageInfo() = 0;
    virtual bool ReadPixels(const ImageInfo& dstInfo, void* dstPixels, size_t dstRowBytes,
        int srcX, int srcY) = 0;
    virtual bool ReadPixels(const Bitmap& dstBitmap, int srcX, int srcY) = 0;
    // shapes
    virtual void DrawPoint(const Point& point, const Paint& paint) = 0;
    virtual void DrawSdf(const SDFShapeBase& shape) = 0;
    virtual void DrawPoints(PointMode mode, size_t count, const Point pts[], const Paint& paint) = 0;
    virtual void DrawLine(const Point& startPt, const Point& endPt, const Paint& paint) = 0;
    virtual void DrawRect(const Rect& rect, const Paint& paint) = 0;
    virtual void DrawRoundRect(const RoundRect& roundRect, const Paint& paint) = 0;
    virtual void DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner, const Paint& paint) = 0;
    virtual void DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle, const Paint& paint) = 0;
    virtual void DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle, const Paint& paint) = 0;
    virtual void DrawOval(const Rect& oval, const Paint& paint) = 0;
    virtual void DrawCircle(const Point& centerPt, scalar radius, const Paint& paint) = 0;
    virtual void DrawPath(const Path& path, const Paint& paint) = 0;
    virtual void DrawPathWithStencil(const Path& path, uint32_t stencilVal, const Paint& paint) = 0;

    virtual void DrawBackground(const Brush& brush) = 0;
    virtual void DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos, scalar lightRadius,
        Color ambientColor, Color spotColor, ShadowFlags flag) = 0;
    virtual void DrawShadowStyle(const Path& path, const Point3& planeParams, const Point3& devLightPos,
        scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag, bool isLimitElevation) = 0;
    virtual void DrawRegion(const Region& region, const Paint& paint) = 0;
    virtual void DrawPatch(const Point cubics[12], const ColorQuad colors[4],
        const Point texCoords[4], BlendMode mode, const Paint& paint) = 0;
    virtual void DrawVertices(const Vertices& vertices, BlendMode mode, const Paint& paint) = 0;

    virtual void DrawImageNine(const Image* image, const RectI& center, const Rect& dst,
        FilterMode filter, const Brush* brush = nullptr) = 0;
    virtual void DrawImageLattice(const Image* image, const Lattice& lattice, const Rect& dst,
        FilterMode filter, const Paint& paint) = 0;

    // color
    virtual void DrawColor(ColorQuad color, BlendMode mode) = 0;

    // opinc calculate realdraw rect
    virtual bool OpCalculateBefore(const Matrix& matrix) = 0;
    virtual std::shared_ptr<Drawing::OpListHandle> OpCalculateAfter(const Rect& bound) = 0;

    // image
    virtual void DrawAtlas(const Image* atlas, const RSXform xform[], const Rect tex[], const ColorQuad colors[],
        int count, BlendMode mode, const SamplingOptions& sampling, const Rect* cullRect, const Paint& paint) = 0;
    virtual void DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py, const Paint& paint) = 0;
    virtual void DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling,
        const Paint& paint) = 0;
    virtual void DrawImageWithStencil(const Image& image, const scalar px, const scalar py,
        const SamplingOptions& sampling, uint32_t stencilVal, const Paint& paint) = 0;

    virtual void DrawImageRect(const Image& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling,
        SrcRectConstraint constraint, const Paint& paint) = 0;
    virtual void DrawImageRect(const Image& image, const Rect& dst, const SamplingOptions& sampling,
        const Paint& paint) = 0;
    virtual void DrawPicture(const Picture& picture) = 0;

    // temporary interface. Support drawing of SkSVGDOM
    virtual void DrawSVGDOM(const sk_sp<SkSVGDOM>& svgDom) = 0;

    // text
    virtual void DrawTextBlob(const TextBlob* blob, const scalar x, const scalar y, const Paint& paint) = 0;

    // symbol
    virtual void DrawSymbol(const DrawingHMSymbolData& symbol, Point locate, const Paint& paint) = 0;

    // stencil culling
    virtual void ClearStencil(const RectI& rect, uint32_t stencilVal) = 0;

    // clip
    virtual void ClipRect(const Rect& rect, ClipOp op, bool doAntiAlias = false) = 0;
    virtual void ClipIRect(const RectI& rect, ClipOp op = ClipOp::INTERSECT) = 0;
    virtual void ClipRoundRect(const RoundRect& roundRect, ClipOp op, bool doAntiAlias = false) = 0;
    virtual void ClipRoundRect(const Rect& rect, std::vector<Point>& pts, bool doAntiAlias = false) = 0;
    virtual void ClipPath(const Path& path, ClipOp op, bool doAntiAlias = false) = 0;
    virtual void ClipRegion(const Region& region, ClipOp op = ClipOp::INTERSECT) = 0;
    virtual bool IsClipEmpty() = 0;
    virtual bool IsClipRect() = 0;
    virtual void ResetClip() = 0;
    virtual bool QuickReject(const Path& path) = 0;
    virtual bool QuickReject(const Rect& rect) = 0;

    // transform
    virtual void SetMatrix(const Matrix& matrix) = 0;
    virtual void ResetMatrix() = 0;
    virtual void ConcatMatrix(const Matrix& matrix) = 0;
    virtual void Translate(scalar dx, scalar dy) = 0;
    virtual void Scale(scalar sx, scalar sy) = 0;
    virtual void Rotate(scalar deg, scalar sx, scalar sy) = 0;
    virtual void Shear(scalar sx, scalar sy) = 0;

    // state
    virtual void Flush() = 0;
    virtual void Clear(ColorQuad color) = 0;
    virtual uint32_t Save() = 0;
    virtual void SaveLayer(const SaveLayerOps& saveLayerOption) = 0;
    virtual void Restore() = 0;
    virtual uint32_t  GetSaveCount() const = 0;
    virtual void Discard() = 0;

    virtual void BuildOverDraw(std::shared_ptr<Canvas> canvas) = 0;

    virtual void BuildNoDraw(int32_t width, int32_t height) = 0;

    virtual void BuildStateInherite(int32_t width, int32_t height) = 0;

    virtual void Reset(int32_t width, int32_t height) = 0;

    virtual bool DrawBlurImage(const Image& image, const Drawing::HpsBlurParameter& blurParams) = 0;

    virtual bool DrawImageEffectHPS(const Image& image,
        const std::vector<std::shared_ptr<Drawing::HpsEffectParameter>>& hpsEffectParams) = 0;

    virtual std::array<int, 2> CalcHpsBluredImageDimension(const Drawing::HpsBlurParameter& blurParams) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
