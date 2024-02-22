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

#ifndef CORE_CANVAS_H
#define CORE_CANVAS_H

#include <memory>

#include "common/rs_macros.h"
#include "drawing/engine_adapter/impl_interface/core_canvas_impl.h"
#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class SrcRectConstraint {
    STRICT_SRC_RECT_CONSTRAINT,
    FAST_SRC_RECT_CONSTRAINT,
};

/*
 * @brief PointMode: Selects if an array of points are drawn as discrete points, as lines, or as
 * an open polygon.
 */
enum class PointMode {
    POINTS_POINTMODE,  // draw each point separately
    LINES_POINTMODE,   // draw each pair of points as a line segment
    POLYGON_POINTMODE, // draw the array of points as a open polygon
};

enum class QuadAAFlags {
    LEFT_QUADAAFLAG = 1,
    TOP_QUADAAFLAG = 2,
    RIGHT_QUADAAFLAG = 4,
    BOTTOM_QUADAAFLAG = 8,
    NONE_QUADAAFLAGS = 0,
    ALL_QUADAAFLAGS = 15,
};

const int DIVES_SIZE = 2;
#undef TRANSPARENT
struct Lattice {
    enum RectType : uint8_t {
        DEFAULT = 0,
        TRANSPARENT,
        FIXEDCOLOR,
    };
    int fXDivs[DIVES_SIZE];
    int fYDivs[DIVES_SIZE];
    RectType fRectTypes = RectType::DEFAULT;
    int fXCount;
    int fYCount;
    RectI fBounds;
    Color fColors;
};

enum CacheType : uint8_t {
    UNDEFINED, // do not change current cache status
    ENABLED,   // explicitly enable cache
    DISABLED,  // explicitly disable cache
    OFFSCREEN, // offscreen rendering
};

class Surface;

/*
 * @brief  Contains the option used to create the layer.
 */
class DRAWING_API SaveLayerOps {
public:
    // How to allocate layer
    enum Flags {
        INIT_WITH_PREVIOUS = 1 << 1,    // create with previous contents
    };

    SaveLayerOps() : bounds_(nullptr), brush_(nullptr), saveLayerFlags_(0) {}

    /*
     * @param bounds          The bounds of layer, may be nullptr.
     * @param brush           When restoring the current layer, attach this brush, may be nullptr.
     * @param saveLayerFlags  How to allocate layer.
     */
    SaveLayerOps(const Rect* bounds, const Brush* brush, uint32_t saveLayerFlags = 0)
        : bounds_(bounds), brush_(brush), saveLayerFlags_(saveLayerFlags) {}
    ~SaveLayerOps() {}

    /*
     * @brief  Gets the bounds of layer, may be nullptr.
     */
    const Rect* GetBounds() const
    {
        return bounds_;
    }

    /*
     * @brief  Gets the brush of layer, may be nullptr.
     */
    const Brush* GetBrush() const
    {
        return brush_;
    }

    /*
     * @brief  Gets the options to modify layer.
     */
    uint32_t GetSaveLayerFlags() const
    {
        return saveLayerFlags_;
    }

private:
    const Rect* bounds_;
    const Brush* brush_;
    uint32_t saveLayerFlags_;
};

class DRAWING_API CoreCanvas {
public:
    CoreCanvas();
    explicit CoreCanvas(void* rawCanvas);
    virtual ~CoreCanvas() {}
    void Bind(const Bitmap& bitmap);

    void BuildOverDraw(std::shared_ptr<Canvas> canvas);

    virtual DrawingType GetDrawingType() const
    {
        return DrawingType::COMMON;
    }

    /*
     * @brief  Gets the total matrix of Canvas to device.
     */
    virtual Matrix GetTotalMatrix() const;

    /*
     * @brief  Gets bounds of clip in local coordinates.
     */
    virtual Rect GetLocalClipBounds() const;

    /*
     * @brief  Gets bounds of clip in device coordinates.
     */
    virtual RectI GetDeviceClipBounds() const;

    /*
     * @brief  Gets GPU context of the GPU surface associated with Canvas.
     */
#ifdef ACE_ENABLE_GPU
    virtual std::shared_ptr<GPUContext> GetGPUContext();
#endif

    /*
     * @brief  Gets width of Canvas.
     */
    int32_t GetWidth() const;

    /*
     * @brief  Gets height of Canvas.
     */
    int32_t GetHeight() const;

    /*
     * @brief  Gets ImageInfo of Canvas.
     */
    ImageInfo GetImageInfo();

    bool ReadPixels(const ImageInfo& dstInfo, void* dstPixels, size_t dstRowBytes,
        int srcX, int srcY);

    bool ReadPixels(const Bitmap& dstBitmap, int srcX, int srcY);

    // shapes
    virtual void DrawPoint(const Point& point);
    virtual void DrawPoints(PointMode mode, size_t count, const Point pts[]);
    virtual void DrawLine(const Point& startPt, const Point& endPt);
    virtual void DrawRect(const Rect& rect);
    virtual void DrawRoundRect(const RoundRect& roundRect);
    virtual void DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner);
    virtual void DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle);
    virtual void DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle);
    virtual void DrawOval(const Rect& oval);
    virtual void DrawCircle(const Point& centerPt, scalar radius);
    virtual void DrawPath(const Path& path);
    virtual void DrawBackground(const Brush& brush);
    virtual void DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos, scalar lightRadius,
        Color ambientColor, Color spotColor, ShadowFlags flag);

    // color
    virtual void DrawColor(ColorQuad color, BlendMode mode = BlendMode::SRC_OVER);

    /*
     * @brief         Draws Region on the Canvas.
     * @param region  Region to draw.
     */
    virtual void DrawRegion(const Region& region);

    virtual void DrawPatch(const Point cubics[12], const ColorQuad colors[4],
        const Point texCoords[4], BlendMode mode);
    virtual void DrawVertices(const Vertices& vertices, BlendMode mode);

    virtual void DrawImageNine(const Image* image, const RectI& center, const Rect& dst,
        FilterMode filter, const Brush* brush = nullptr);
    virtual void DrawImageLattice(const Image* image, const Lattice& lattice, const Rect& dst,
        FilterMode filter, const Brush* brush = nullptr);

    // opinc_begin
    virtual bool BeginOpRecording(const Rect* bound = nullptr, bool isDynamic = false);
    virtual Drawing::OpListHandle EndOpRecording();
    virtual void DrawOpList(Drawing::OpListHandle handle);
    virtual int CanDrawOpList(Drawing::OpListHandle handle);
    virtual void PreOpListDrawArea(const Matrix& matrix);
    virtual bool CanUseOpListDrawArea(Drawing::OpListHandle handle, const Rect* bound = nullptr);
    virtual Drawing::OpListHandle GetOpListDrawArea();
    virtual void OpincDrawImageRect(const Image& image, Drawing::OpListHandle drawAreas,
        const SamplingOptions& sampling, SrcRectConstraint constraint);
    // opinc_end

    // image
    virtual void DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py);
    void DrawBitmap(const Bitmap& bitmap, const Rect& src, const Rect& dst, const SamplingOptions& sampling);
    void DrawBitmap(const Bitmap& bitmap, const Rect& dst, const SamplingOptions& sampling);
    virtual void DrawBitmap(Media::PixelMap& pixelMap, const scalar px, const scalar py);
    virtual void DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling);
    virtual void DrawImageRect(const Image& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling,
        SrcRectConstraint constraint = SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
    virtual void DrawImageRect(const Image& image, const Rect& dst, const SamplingOptions& sampling);
    virtual void DrawPicture(const Picture& picture);

    // temporary interface. Support drawing of SkSVGDOM
    virtual void DrawSVGDOM(const sk_sp<SkSVGDOM>& svgDom);

    // text
    virtual void DrawTextBlob(const TextBlob* blob, const scalar x, const scalar y);

    // symbol
    virtual void DrawSymbol(const DrawingHMSymbolData& symbol, Point locate);

    // clip
    /*
     * @brief              Replace the clipping area with the intersection or difference between the
                           current clipping area and Rect, and use a clipping edge that is aliased or anti-aliased.
     * @param rect         To combine with clipping area.
     * @param op           To apply to clip.
     * @param doAntiAlias  true if clip is to be anti-aliased. The default value is false.
     */
    virtual void ClipRect(const Rect& rect, ClipOp op = ClipOp::INTERSECT, bool doAntiAlias = false);

    /*
     * @brief              Replace the clipping area with the intersection or difference between the
                           current clipping area and RectI, and use a clipping edge that is aliased or anti-aliased.
     * @param rect         To combine with clipping area.
     * @param op           To apply to clip.
     * @param doAntiAlias  true if clip is to be anti-aliased. The default value is false.
     */
    virtual void ClipIRect(const RectI& rect, ClipOp op = ClipOp::INTERSECT);

    /*
     * @brief              Replace the clipping area with the intersection or difference of the
                           current clipping area and Rect, and use a clipping edge that is aliased or anti-aliased.
     * @param roundRect    To combine with clip.
     * @param op           To apply to clip.
     * @param doAntiAlias  true if clip is to be anti-aliased. The default value is false.
     */
    virtual void ClipRoundRect(const RoundRect& roundRect, ClipOp op = ClipOp::INTERSECT, bool doAntiAlias = false);

    virtual void ClipRoundRect(const Rect& rect, std::vector<Point>& pts, bool doAntiAlias = false);

    /*
     * @brief              Replace the clipping area with the intersection or difference of the
                           current clipping area and Path, and use a clipping edge that is aliased or anti-aliased.
     * @param path         To combine with clip.
     * @param op           To apply to clip.
     * @param doAntiAlias  true if clip is to be anti-aliased. The default value is false.
     */
    virtual void ClipPath(const Path& path, ClipOp op = ClipOp::INTERSECT, bool doAntiAlias = false);

    /*
     * @brief              Replace the clipping area with the intersection or difference of the
                           current clipping area and Region, and use a clipping edge that is aliased or anti-aliased.
     * @param region       To combine with clip.
     * @param op           To apply to clip.The default value is ClipOp::INTERSECT
     */
    virtual void ClipRegion(const Region& region, ClipOp op = ClipOp::INTERSECT);

    /*
     * @brief  Returns true if clip is empty.
     */
    virtual bool IsClipEmpty();

    /*
     * @brief  Returns true if clip is SkRect and not empty.
     */
    virtual bool IsClipRect();

    /*
     * @brief  Returns true if clip is emptySkRect rect, transformed by SkMatrix,
     *         can be quickly determined to be outside of clip.
     */
    virtual bool QuickReject(const Rect& rect);

    // transform
    virtual void SetMatrix(const Matrix& matrix);
    virtual void ResetMatrix();
    virtual void ConcatMatrix(const Matrix& matrix);
    virtual void Translate(scalar dx, scalar dy);
    virtual void Scale(scalar sx, scalar sy);

    /*
     * @brief      Rotates Matrix by degrees.
     * @param deg  Amount to rotate, in degrees.
     */
    void Rotate(scalar deg)
    {
        Rotate(deg, 0, 0);
    }
    virtual void Rotate(scalar deg, scalar sx, scalar sy);
    virtual void Shear(scalar sx, scalar sy);

    // state
    virtual void Flush();
    virtual void Clear(ColorQuad color);

    /*
     * @brief               Saves Matrix and clipping area, return the number of saved states.
     */
    virtual uint32_t Save();

    /*
     * @brief               Saves Matrix and clipping area, and allocates Surface for subsequent drawing.
     * @param saveLayerOps  Contains the option used to create the layer.
     */
    virtual void SaveLayer(const SaveLayerOps& saveLayerOps);
    virtual void Restore();

    /*
     * @brief  Returns the number of saved states, each containing Matrix and clipping area.
     */
    virtual uint32_t GetSaveCount() const;

    /*
     * @brief  Makes Canvas contents undefined.
     */
    virtual void Discard();

    // paint
    virtual CoreCanvas& AttachPen(const Pen& pen);
    virtual CoreCanvas& AttachBrush(const Brush& brush);
    virtual CoreCanvas& AttachPaint(const Paint& paint);
    virtual CoreCanvas& DetachPen();
    virtual CoreCanvas& DetachBrush();
    virtual CoreCanvas& DetachPaint();

    virtual ColorQuad GetEnvForegroundColor() const;
    virtual bool isHighContrastEnabled() const;
    virtual Drawing::CacheType GetCacheType() const;
    virtual Drawing::Surface* GetSurface() const;

    virtual float GetAlpha() const;
    virtual int GetAlphaSaveCount() const;

    template<typename T>
    T* GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }
    std::shared_ptr<CoreCanvasImpl> GetCanvasData() const;

protected:
    CoreCanvas(int32_t width, int32_t height);
    void BuildNoDraw(int32_t width, int32_t height);
    void Reset(int32_t width, int32_t height);
    Paint paintBrush_;
    Paint paintPen_;

private:
    void AttachPaint();
    std::shared_ptr<CoreCanvasImpl> impl_;
#ifdef ACE_ENABLE_GPU
    std::shared_ptr<GPUContext> gpuContext_;
#endif
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
