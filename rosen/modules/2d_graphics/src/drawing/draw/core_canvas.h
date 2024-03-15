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

/**
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

/**
 * @brief Contains the option used to create the layer.
 */
class DRAWING_API SaveLayerOps {
public:
    // How to allocate layer
    enum Flags {
        INIT_WITH_PREVIOUS = 1 << 1,    // create with previous contents
    };

    SaveLayerOps() : bounds_(nullptr), brush_(nullptr), saveLayerFlags_(0) {}

    /**
     * @param bounds         The bounds of layer, may be nullptr.
     * @param brush          When restoring the current layer, attach this brush, may be nullptr.
     * @param saveLayerFlags How to allocate layer.
     */
    SaveLayerOps(const Rect* bounds, const Brush* brush, uint32_t saveLayerFlags = 0)
        : bounds_(bounds), brush_(brush), saveLayerFlags_(saveLayerFlags) {}
    ~SaveLayerOps() {}

    /**
     * @brief Gets the bounds of layer, may be nullptr.
     */
    const Rect* GetBounds() const
    {
        return bounds_;
    }

    /**
     * @brief Gets the brush of layer, may be nullptr.
     */
    const Brush* GetBrush() const
    {
        return brush_;
    }

    /**
     * @brief Gets the options to modify layer.
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

    /**
     * @brief Gets the total matrix of Canvas to device.
     */
    virtual Matrix GetTotalMatrix() const;

    /**
     * @brief Gets bounds of clip in local coordinates.
     */
    virtual Rect GetLocalClipBounds() const;

    /**
     * @brief Gets bounds of clip in device corrdinates.
     */
    virtual RectI GetDeviceClipBounds() const;

    /**
     * @brief Gets GPU context of the GPU surface associated with Canvas.
     */
#ifdef ACE_ENABLE_GPU
    virtual std::shared_ptr<GPUContext> GetGPUContext();
#endif

    /**
     * @brief Gets width of Canvas.
     */
    int32_t GetWidth() const;

    /**
     * @brief Gets height of Canvas.
     */
    int32_t GetHeight() const;

    /**
     * @brief Gets ImageInfo of Canvas.
     */
    ImageInfo GetImageInfo();

    bool ReadPixels(const ImageInfo& dstInfo, void* dstPixels, size_t dstRowBytes,
        int srcX, int srcY);

    bool ReadPixels(const Bitmap& dstBitmap, int srcX, int srcY);

    // shapes
    /**
     * @brief The shape of point drawn depends on pen. If pen is set to Pen::CapStyle::ROUND_CAP,
     * draw a circle of diameter pen stroke width. If pen is set to Pen::CapStyle::SQUAER_CAP,
     * draw a square of width and height pen stroke width. 
     * @param point top-left edge of circle or square
     */
    virtual void DrawPoint(const Point& point);

    /**
     * @brief If mode is LINES_POINTMODE, each pair of points draws a line segment. One line
     * is drawn for every two points; each point is used once. If count is odd, the final point is ignored.
     * If mode is POLYGON_POINTMODE, each adjacent pair of points draws a line segment. count minus one lines
     * are drawn; the first and last point are used once.
     * @param mode  whether pts draws points or lines
     * @param count number of points in the array
     * @param pts   array of points to draw
     */
    virtual void DrawPoints(PointMode mode, size_t count, const Point pts[]);

    /**
     * @brief Draws line segment from startPt to endPt.
     * @param startPt start of line segment
     * @param endPt   end of line segment
     */
    virtual void DrawLine(const Point& startPt, const Point& endPt);

    /**
     * @brief If rectangle is stroked, use pen to stroke width describes the line thickness,
     * else use brush to fill the rectangle.
     * @param rect rectangle to draw
     */
    virtual void DrawRect(const Rect& rect);

    /**
     * @brief If round rectangle is stroked, use pen to stroke width describes the line thickness,
     * else use brush to fill the round rectangle. Round rect may represent a rectangle, circle,
     * oval, uniformly rounded rectangle, or may have any combination of positive non-square radii
     * for the four corners.
     * @param roundRect RRect with up to eight corner radii to draw
     */
    virtual void DrawRoundRect(const RoundRect& roundRect);

    /**
     * @brief Outer must contain inner or the drawing is undefined. If RRect is stroked, use pen to stroke
     * width describes the line thickness. If stroked and RRect corner has zero length radii,
     * Pen::JoinStyle can draw corners rounded or square.
     * @param outer RRect outer bounds to draw
     * @param inner RRect inner bounds to draw
     */
    virtual void DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner);

    /**
     * @brief Arc is part of oval bounded by oval, sweeping from startAngle to startAngle plus sweepAngle.
     * startAngle and sweepAngle are in degrees. StartAngle of zero places start point at the right
     * middle edge of oval. A positive sweepAngle places arc end point clockwise from start point;
     * A negative sweepAngle places arc end point counterclockwise from start point. sweepAngle may
     * exceed 360 degrees, a full circle. Draw a wedge that includes lines from oval center to arc end points
     * @param oval       rect bounds of oval containing arc to draw
     * @param startAngle angle in degrees where arc begins
     * @param sweepAngle sweep angle in degrees, positive is clockwise
     */
    virtual void DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle);

    /**
     * @brief Draw arc between end points.
     * @param oval       rect bounds of oval containing arc to draw
     * @param startAngle angle in degrees where arc begins
     * @param sweepAngle sweep angle in degrees, positive is clockwise
     */
    virtual void DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle);

    /**
     * @brief If oval is stroked, use pen to stroke width describes the line thickness, else use brush to fill the oval.
     * @param oval rect bounds of oval
     */
    virtual void DrawOval(const Rect& oval);

    /**
     * @brief If radius is zero or less, nothing is drawn. If circle is stroked, use pen to
     * stroke width describes the line thickness, else use brush to fill the circle.
     * @param centerPt circle center
     * @param radius   half the diameter of circle
     */
    virtual void DrawCircle(const Point& centerPt, scalar radius);

    /**
     * @brief Path contains an array of path contour, each of which may be open or closed.
     * If RRect is filled, Path::PathFillType determines whether path contour
     * describes inside or outside of fill; If stroked, use pen to stroke width
     * describes the line thickness, Pen::CapStyle describes line ends, and
     * Pen::Join describes how corners are drawn.
     * @param path Path to draw
     */
    virtual void DrawPath(const Path& path);

    /**
     * @brief              Use brush to fill the canvas.
     * @param brush        used to fill Canvas
     */
    virtual void DrawBackground(const Brush& brush);

    virtual void DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos, scalar lightRadius,
        Color ambientColor, Color spotColor, ShadowFlags flag);

    // color
    /**
     * @brief Fills clip with color color. Mode determines how ARGB is combined with destination.
     * @param color ColorQuad representing unpremultiplied color.
     * @param mode  BlendMode used to combine source color and destination.
     */
    virtual void DrawColor(ColorQuad color, BlendMode mode = BlendMode::SRC_OVER);

    /**
     * @brief Draws Region on the Canvas.
     * @param region Region to draw.
     */
    virtual void DrawRegion(const Region& region);

    /**
     * @brief Draws a Coons patch: the interpolation of four cubics with shared corners, associating a color,
     * and optionally a texture Point, with each corner. Point array cubics specifies four Path cubic
     * starting at the top-left corner, in clockwise order, sharing every fourth point. The last Path
     * cubic ends at the first point. Color array color associates colors with corners in top-left,
     * top-right, bottom-right, bottom-left order. If brush contains Shader, Point array texCoords maps
     * Shader as texture to corners in top-left, top-right, bottom-right, bottom-left order. If
     * texCoords is nullptr, Shader is mapped using positions (derived from cubics).
     * @param cubics    Path cubic array, sharing common points
     * @param colors	color array, one for each corner
     * @param texCoords	Point array of texture coordinates, mapping Shader to corners; may be nullptr
     * @param mode	    combines patch's colors with Shader if present or brush opaque color if not.
     *                  Ignored if colors is null.
     */
    virtual void DrawPatch(const Point cubics[12], const ColorQuad colors[4],
        const Point texCoords[4], BlendMode mode);

    /**
     * @brief If brush or pen contains an Shader and vertices does not contain texCoords,
     * the shader is mapped using the vertices' positions. If vertices colors are defined
     * in vertices, and brush/pen contains Shader, BlendMode mode combines vertices colors with Shader.
     * MaskFilter and PathEffect on paint are ignored.
     * @param vertices triangle mesh to draw
     * @param mode	   combines vertices' colors with Shader if present or brush/pen opaque color if not.
     *                 Ignored if the vertices do not contain color.
     */
    virtual void DrawVertices(const Vertices& vertices, BlendMode mode);

    /**
     * @brief Draw image stretched proportionally to fit into Rect dst. IRect
     * center divides the image into nine sections: four sides, four corners, and
     * the center. Corners are unmodified or scaled down proportionately if their sides
     * are larger than dst; center and four sides are scaled to fit remaining space, if any.
     * Additionally transform draw using clip, Matrix, and optional brush. If brush is attached,
     * apply ColorFilter, alpha, ImageFilter, and BlendMode. If image is COLORTYPE_ALPHA_8, apply Shader.
     * If brush contains MaskFilter, generate mask from image bounds. Any MaskFilter on paint is
     * ignored as is paint anti-aliasing state.
     * @param image  Image containing pixels, dimensions, and format
     * @param center IRect edge of image corners and sides
     * @param dst    destination Rect of image to draw to
     * @param filter what technique to use when sampling the image
     */
    virtual void DrawImageNine(const Image* image, const RectI& center, const Rect& dst,
        FilterMode filter, const Brush* brush = nullptr);

    /**
     * @brief Draws Image image stretched proportionally to fit into Rect dst.
     * Canvas::Lattice lattice divides image into a rectangular grid.
     * Each intersection of an even-numbered row and column is fixed;
     * fixed lattice elements never scale larger than their initial
     * size and shrink proportionately when all fixed elements exceed the bitmap
     * dimension. All other grid elements scale to fill the available space, if any.
     * 
     * If brush is attached, apply ColorFilter, alpha, ImageFilter, and
     * BlendMode. If image is COLORTYPE_ALPHA_8, apply Shader.
     * If brush contains MaskFilter, generate mask from image bounds.
     * Any MaskFilter on paint is ignored as is paint anti-aliasing state.
     * 
     * @param image   Image containing pixels, dimensions, and format
     * @param lattice division of bitmap into fixed and variable rectangles
     * @param dst     destination Rect of image to draw to
     * @param filter  what technique to use when sampling the image
     * @param brush   brush containing BlendMode, ColorFilter, ImageFilter, and so on; or nullptr
     */
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
    virtual void DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling);
    virtual void DrawImageRect(const Image& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling,
        SrcRectConstraint constraint = SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
    virtual void DrawImageRect(const Image& image, const Rect& dst, const SamplingOptions& sampling);

    /**
     * @brief Clip and Matrix are unchanged by picture contents, as if Save() was called
     * before and Restore() was called after DrawPicture(). Picture records a series of
     * draw commands for later playback.
     * @param picture recorded drawing commands to play
     */
    virtual void DrawPicture(const Picture& picture);

    // temporary interface. Support drawing of SkSVGDOM
    virtual void DrawSVGDOM(const sk_sp<SkSVGDOM>& svgDom);

    // text
    /**
     * @brief blob contains glyphs, their positions, and paint attributes specific to text:
     * Typeface, text size, text scale x, text skew x, anti-alias, fake bold,
     * font embedded bitmaps, pen/brush full hinting spacing, LCD text, linear text,
     * and subpixel text. TextEncoding must be set to TextEncoding::GLYPH_ID.
     * Elements of pen/brush: anti-alias, BlendMode, color including alpha,
     * ColorFilter, MaskFilter, PathEffect, Shader, and Brush::Style; apply to blob.
     * If attach pen to draw text, set Pen::Cap, Pen::Join, and stroke width;
     * apply to Path created from blob.
     * @param blob glyphs, positions, and their paints' text size, typeface, and so on
     * @param x    horizontal offset applied to blob
     * @param y    vertical offset applied to blob
    */
    virtual void DrawTextBlob(const TextBlob* blob, const scalar x, const scalar y);

    // symbol
    virtual void DrawSymbol(const DrawingHMSymbolData& symbol, Point locate);

    // clip
    /**
     * @brief Replace the clipping area with the intersection or difference between the
     * current clipping area and Rect, and use a clipping edge that is aliased or anti-aliased.
     * @param rect        To combine with clipping area.
     * @param op          To apply to clip.
     * @param doAntiAlias true if clip is to be anti-aliased. The default value is false.
     */
    virtual void ClipRect(const Rect& rect, ClipOp op = ClipOp::INTERSECT, bool doAntiAlias = false);

    /**
     * @brief Replace the clipping area with the intersection or difference between the
     * current clipping area and RectI, and use a clipping edge that is aliased or anti-aliased.
     * @param rect        To combine with clipping area.
     * @param op          To apply to clip.
     * @param doAntiAlias true if clip is to be anti-aliased. The default value is false.
     */
    virtual void ClipIRect(const RectI& rect, ClipOp op = ClipOp::INTERSECT);

    /**
     * @brief Replace the clipping area with the intersection or difference of the
     * current clipping area and Rect, and use a clipping edge that is aliased or anti-aliased.
     * @param roundRect   To combine with clip.
     * @param op          To apply to clip.
     * @param doAntiAlias true if clip is to be anti-aliased. The default value is false.
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

    /**
     * @brief Replace the clipping area with the intersection or difference of the
     * current clipping area and Region, and use a clipping edge that is aliased or anti-aliased.
     * @param region To combine with clip.
     * @param op     To apply to clip.The default value is ClipOp::INTERSECT
     */
    virtual void ClipRegion(const Region& region, ClipOp op = ClipOp::INTERSECT);

    /**
     * @brief Returns true if clip is empty.
     * @return true if clip is empty
     */
    virtual bool IsClipEmpty();

    /**
     * @brief Returns true if clip is Rect and not empty.
     * @return true if clip is rect and not empty
     */
    virtual bool IsClipRect();

    /**
     * @brief Returns true if clip is empty Rect rect, transformed by Matrix,
     * can be quickly determined to be outside of clip.
     * @param rect Rect to compare with clip
     * @return true if rect, transformed by Matrix, does not intersect clip
     */
    virtual bool QuickReject(const Rect& rect);

    // transform
    /** 
     * @brief Replaces RSMatrix with matrix. Unlike Concat(), any prior matrix state is overwritten.
     * @param matrix matrix to copy, replacing existing RSMatrix
     */
    virtual void SetMatrix(const Matrix& matrix);

    /**
     * @brief Sets RSMatrix to the identity matrix. Any prior matrix state is overwritten.
     */
    virtual void ResetMatrix();
    
    /**
     * @brief Replaces RSMatrix with matrix premultiplied with existing RSMatrix.
     * This has the effect of transforming the drawn geometry by matrix, before
     * transforming the result with existing RSMatrix.
     * @param matrix matrix to premultiply with existing RSMatrix
     */
    virtual void ConcatMatrix(const Matrix& matrix);
    
    /**
     * @brief Translates RSMatrix by dx along the x-axis and dy along the y-axis.
     * Mathematically, replaces RSMatrix with a translation matrix premultiplied with RSMatrix.
     * This has the effect of moving the drawing by (dx, dy) before transforming the result with RSMatrix.
     * @param dx distance to translate on x-axis
     * @param dy distance to translate on y-axis
     */
    virtual void Translate(scalar dx, scalar dy);
    
    /**
     * @brief Scales RSMatrix by sx on the x-axis and sy on the y-axis.
     * Mathematically, replaces RSMatrix with a scale matrix premultiplied with RSMatrix.
     * This has the effect of scaling the drawing by (sx, sy) before transforming the result with RSMatrix.
     * @param sx amount to scale on x-axis
     * @param sy amount to scale on y-axis
     */
    virtual void Scale(scalar sx, scalar sy);

    /**
     * @brief Rotates Matrix by degrees.
     * @param deg Amount to rotate, in degrees.
     */
    void Rotate(scalar deg)
    {
        Rotate(deg, 0, 0);
    }

    /**
     * @brief Rotates RSMatrix by degrees about a point at (sx, sy). Positive degrees rotates clockwise.
     * Mathematically, constructs a rotation matrix; premultiplies the rotation matrix by
     * a translation matrix; then replaces RSMatrix with the resulting matrix premultiplied with RSMatrix.
     * This has the effect of rotating the drawing about a given point before transforming the result with RSMatrix.
     * @param degrees amount to rotate, in degrees
     * @param sx      x-axis value of the point to rotate about
     * @param sy      y-axis value of the point to rotate about
     */
    virtual void Rotate(scalar deg, scalar sx, scalar sy);

    /**
     * @brief Shear RSMatrix by sx on the x-axis and sy on the y-axis. A positive value of sx
     * skews the drawing right as y-axis values increase; a positive value of sy skews
     * the drawing down as x-axis values increase. Mathematically, replaces RSMatrix with a
     * skew matrix premultiplied with RSMatrix.
     * This has the effect of skewing the drawing by (sx, sy) before transforming the result with RSMatrix.
     * @param sx amount to skew on x-axis
     * @param sy amount to skew on y-axis
     */
    virtual void Shear(scalar sx, scalar sy);

    // state
    /**
     * @brief Triggers the immediate execution of all pending draw operations.
     * If Canvas is associated with GPU surface, resolves all pending GPU operations.
     * If Canvas is associated with raster surface, has no effect; raster draw 
     * operations are never deferred.
     */
    virtual void Flush();

    /**
     * @brief Fills clip with color color using BlendMode::SRC.
     * This has the effect of replacing all pixels contained by clip with color.
     * @param color unpremultiplied ARGB
     */
    virtual void Clear(ColorQuad color);

    /**
     * @brief Saves Matrix and clipping area, return the number of saved states.
     */
    virtual uint32_t Save();

    /**
     * @brief Saves Matrix and clipping area, and allocates Surface for subsequent drawing.
     * @param saveLayerOps Contains the option used to create the layer.
     */
    virtual void SaveLayer(const SaveLayerOps& saveLayerOps);
    /**
     * @brief Removes changes to Matrix and clip since Canvas state was last saved. 
     * The state is removed from the stack. Does nothing if the stack is empty.
     */
    virtual void Restore();

    /**
     * @brief Returns the number of saved states, each containing Matrix and clipping area.
     * 
     * @return uint32_t type, represent depth of save state stack
     */
    virtual uint32_t GetSaveCount() const;

    /**
     * @brief Makes Canvas contents undefined.
     */
    virtual void Discard();

    // paint
    /**
     * @brief Attach pen to canvas and stroke something.
     * @param pen tool to stroke
     * @return CoreCanvas& 
     */
    virtual CoreCanvas& AttachPen(const Pen& pen);

    /**
     * @brief Attach brush to canvas and fill something.
     * @param brush tool to fill
     * @return CoreCanvas& 
     */
    virtual CoreCanvas& AttachBrush(const Brush& brush);

    /**
     * @brief Attach paint to canvas to draw something.
     * @param paint tool to fill or stroke something
     * @return CoreCanvas& 
     */
    virtual CoreCanvas& AttachPaint(const Paint& paint);
    
    /**
     * @brief Detach pen from canvas.
     * @return CoreCanvas& 
     */
    virtual CoreCanvas& DetachPen();
    
    /**
     * @brief Detach brush from canvas.
     * @return CoreCanvas& 
     */
    virtual CoreCanvas& DetachBrush();

    /**
     * @brief Detach paint from canvas.
     * @return CoreCanvas& 
     */
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
