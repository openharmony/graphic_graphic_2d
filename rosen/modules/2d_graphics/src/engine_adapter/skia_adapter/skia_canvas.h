/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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
#include "include/core/SkImage.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPicture.h"
#include "include/core/SkPoint3.h"
#include "include/core/SkRRect.h"
#include "include/utils/SkShadowUtils.h"
#include "skia_bitmap.h"
#include "skia_image.h"
#include "skia_matrix.h"
#include "skia_paint.h"
#include "skia_picture.h"

#include "impl_interface/core_canvas_impl.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaCanvas : public CoreCanvasImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaCanvas();
    ~SkiaCanvas() override {};
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    void Bind(const Bitmap& bitmap) override;

    // shapes
    void DrawPoint(const Point& point) override;
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

    // image
    void DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py) override;
    void DrawBitmap(Media::PixelMap& pixelMap, const scalar px, const scalar py) override;
    void DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling) override;
    void DrawImageRect(const Image& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling,
        SrcRectConstraint constraint) override;
    void DrawImageRect(const Image& image, const Rect& dst, const SamplingOptions& sampling) override;
    void DrawPicture(const Picture& picture) override;

    // clip
    void ClipRect(const Rect& rect, ClipOp op) override;
    void ClipRoundRect(const RoundRect& roundRect, ClipOp op) override;
    void ClipPath(const Path& path, ClipOp op) override;

    // transform
    void SetMatrix(const Matrix& matrix) override;
    void ResetMatrix() override;
    void ConcatMatrix(const Matrix& matrix) override;
    void Translate(scalar dx, scalar dy) override;
    void Scale(scalar sx, scalar sy) override;
    void Rotate(scalar deg) override;
    void Shear(scalar sx, scalar sy) override;

    // state
    void Flush() override;
    void Clear(ColorQuad color) override;
    void Save() override;
    void SaveLayer(const Rect& rect, const Brush& brush) override;
    void Restore() override;

    // paint
    void AttachPen(const Pen& pen) override;
    void AttachBrush(const Brush& brush) override;
    void DetachPen() override;
    void DetachBrush() override;

    SkCanvas* ExportSkCanvas() const;

private:
    void RoundRectCastToSkRRect(const RoundRect& roundRect, SkRRect& skRRect) const;
    std::shared_ptr<SkCanvas> skiaCanvas_;
    SkiaPaint skiaPaint_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
