/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "engine_adapter/impl_interface/core_canvas_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class CoreCanvas {
public:
    CoreCanvas();
    virtual ~CoreCanvas() {}
    void Bind(const Bitmap& bitmap);

    // shapes
    void DrawPoint(const Point& point);
    void DrawLine(const Point& startPt, const Point& endPt);
    void DrawRect(const Rect& rect);
    void DrawRoundRect(const RoundRect& roundRect);
    void DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner);
    void DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle);
    void DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle);
    void DrawOval(const Rect& oval);
    void DrawCircle(const Point& centerPt, scalar radius);
    void DrawPath(const Path& path);
    void DrawBackground(const Brush& brush);
    void DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos,
                    scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag);

    // image
    void DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py);
    void DrawBitmap(Media::PixelMap& pixelMap, const scalar px, const scalar py);
    void DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling);

    // text
    void DrawText(const Text& text); // TODO...

    // clip
    void ClipRect(const Rect& rect, ClipOp op);
    void ClipRoundRect(const RoundRect& roundRect, ClipOp op);
    void ClipPath(const Path& path, ClipOp op);

    // transform
    void SetMatrix(const Matrix& matrix);
    void ResetMatrix();
    void ConcatMatrix(const Matrix& matrix);
    void Translate(scalar dx, scalar dy);
    void Scale(scalar sx, scalar sy);
    void Rotate(scalar deg);
    void Shear(scalar sx, scalar sy);

    // state
    void Flush();
    void Clear(ColorQuad color);
    void Save();
    void SaveLayer(const Rect& rect, const Brush& brush);
    void Restore();

    // paint
    CoreCanvas& AttachPen(const Pen& pen);
    CoreCanvas& AttachBrush(const Brush& brush);
    CoreCanvas& DetachPen();
    CoreCanvas& DetachBrush();

    template<typename T> const std::shared_ptr<T> GetImpl() const { return impl_->DowncastingTo<T>(); }
private:
    std::shared_ptr<CoreCanvasImpl> impl_;
};
}
}
}
#endif