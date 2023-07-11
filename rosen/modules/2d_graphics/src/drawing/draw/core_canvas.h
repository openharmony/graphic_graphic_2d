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

#include "drawing/engine_adapter/impl_interface/core_canvas_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class SrcRectConstraint {
    STRICT_SRC_RECT_CONSTRAINT,
    FAST_SRC_RECT_CONSTRAINT,
};
/*
 * @brief  Contains the option used to create the layer.
 */
class SaveLayerOps {
public:
    // How to allocate layer
    enum Flags {
        INIT_WITH_PREVIOUS = 1 << 1,    // create with previous contents
    };

    SaveLayerOps() : bounds_(nullptr), brush_(nullptr), imageFilter_(nullptr), saveLayerFlags_(0) {}
    SaveLayerOps(const Rect* bounds, const Brush* brush, uint32_t saveLayerFlags = 0)
        : SaveLayerOps(bounds, brush, nullptr, saveLayerFlags) {}

    /*
     * @param bounds          The bounds of layer, may be nullptr.
     * @param brush           When restoring the current layer, attach this brush, may be nullptr.
     * @param imageFilter     Use this to filter the current layer as the new layer background, may be nullptr.
     * @param saveLayerFlags  How to allocate layer.
     */
    SaveLayerOps(const Rect* bounds, const Brush* brush, const ImageFilter* imageFilter, uint32_t saveLayerFlags = 0)
        : bounds_(bounds), brush_(brush), imageFilter_(imageFilter), saveLayerFlags_(saveLayerFlags) {}
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
     * @brief  Gets the image filter of layer, may be nullptr.
     */
    const ImageFilter* GetImageFilter() const
    {
        return imageFilter_;
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
    const ImageFilter* imageFilter_;
    uint32_t saveLayerFlags_;
};

class CoreCanvas {
public:
    CoreCanvas();
    explicit CoreCanvas(void* rawCanvas);
    virtual ~CoreCanvas() {}
    void Bind(const Bitmap& bitmap);

    virtual DrawingType GetDrawingType() const
    {
        return DrawingType::COMMON;
    }

    /*
     * @brief  Gets the total matrix of Canvas to device.
     */
    Matrix GetTotalMatrix() const;

    /*
     * @brief  Gets bounds of clip in local coordinates.
     */
    Rect GetLocalClipBounds() const;

    /*
     * @brief  Gets bounds of clip in device corrdinates.
     */
    RectI GetDeviceClipBounds() const;

    /*
     * @brief  Gets GPU context of the GPU surface associated with Canvas.
     */
#ifdef ACE_ENABLE_GPU
    std::shared_ptr<GPUContext> GetGPUContext() const;
#endif

    /*
     * @brief  Gets width of Canvas.
     */
    int32_t GetWidth() const;

    /*
     * @brief  Gets height of Canvas.
     */
    int32_t GetHeight() const;

    // shapes
    virtual void DrawPoint(const Point& point);
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

    /*
     * @brief         Draws Region on the Canvas.
     * @param region  Region to draw.
     */
    virtual void DrawRegion(const Region& region);

    // image
    virtual void DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py);
    virtual void DrawBitmap(Media::PixelMap& pixelMap, const scalar px, const scalar py);
    virtual void DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling);
    virtual void DrawImageRect(const Image& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling,
        SrcRectConstraint constraint = SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
    virtual void DrawImageRect(const Image& image, const Rect& dst, const SamplingOptions& sampling);
    virtual void DrawPicture(const Picture& picture);

    // clip
    /*
     * @brief              Replace the clipping area with the intersection or difference between the
                           current clipping area and Rect, and use a clipping edge that is aliased or anti-aliased.
     * @param rect         To combine with clipping area.
     * @param op           To apply to clip.
     * @param doAntiAlias  true if clip is to be anti-aliased. The default value is false.
     */
    virtual void ClipRect(const Rect& rect, ClipOp op, bool doAntiAlias = false);

    /*
     * @brief              Replace the clipping area with the intersection or difference of the
                           current clipping area and Rect, and use a clipping edge that is aliased or anti-aliased.
     * @param roundRect    To combine with clip.
     * @param op           To apply to clip.
     * @param doAntiAlias  true if clip is to be anti-aliased. The default value is false.
     */
    virtual void ClipRoundRect(const RoundRect& roundRect, ClipOp op, bool doAntiAlias = false);

    /*
     * @brief              Replace the clipping area with the intersection or difference of the
                           current clipping area and Path, and use a clipping edge that is aliased or anti-aliased.
     * @param path         To combine with clip.
     * @param op           To apply to clip.
     * @param doAntiAlias  true if clip is to be anti-aliased. The default value is false.
     */
    virtual void ClipPath(const Path& path, ClipOp op, bool doAntiAlias = false);

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
    virtual void Save();

    /*
     * @brief               Saves Matrix and clipping area, and allocates Surface for subsequent drawing.
     * @param saveLayerOps  Contains the option used to create the layer.
     */
    virtual void SaveLayer(const SaveLayerOps& saveLayerOps);
    virtual void Restore();

    /*
     * @brief  Returns the number of saved states, each containing Matrix and clipping area.
     */
    uint32_t GetSaveCount() const;

    // paint
    virtual CoreCanvas& AttachPen(const Pen& pen);
    virtual CoreCanvas& AttachBrush(const Brush& brush);
    virtual CoreCanvas& DetachPen();
    virtual CoreCanvas& DetachBrush();

    template<typename T>
    const std::shared_ptr<T> GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }
    std::shared_ptr<CoreCanvasImpl> GetCanvasData() const;

protected:
    CoreCanvas(int32_t width, int32_t height);

private:
    std::shared_ptr<CoreCanvasImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
