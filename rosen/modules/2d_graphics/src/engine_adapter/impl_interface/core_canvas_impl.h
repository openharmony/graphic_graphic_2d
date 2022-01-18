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

#ifndef CORECANVASIMPL_H
#define CORECANVASIMPL_H

#include "base_impl.h"
#include "securec.h"

#include "draw/brush.h"
#include "draw/clip.h"
#include "draw/path.h"
#include "draw/pen.h"
#include "draw/shadow.h"
#include "effect/filter.h"
#include "image/bitmap.h"
#include "image/image.h"
#include "text/text.h"
#include "utils/log.h"
#include "utils/matrix.h"
#include "utils/point.h"
#include "utils/point3.h"
#include "utils/rect.h"
#include "utils/round_rect.h"
#include "utils/sampling_options.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
namespace Drawing {
class CoreCanvasImpl : public BaseImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::BASE_INTERFACE;
    CoreCanvasImpl() {};
    virtual ~CoreCanvasImpl() {};
    AdapterType GetType() const override
    {
        return AdapterType::BASE_INTERFACE;
    }

    virtual void Bind(const Bitmap& bitmap) = 0;

    // shapes
    virtual void DrawPoint(const Point& point) = 0;
    virtual void DrawLine(const Point& startPt, const Point& endPt) = 0;
    virtual void DrawRect(const Rect& rect) = 0;
    virtual void DrawRoundRect(const RoundRect& roundRect) = 0;
    virtual void DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner) = 0;
    virtual void DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle) = 0;
    virtual void DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle) = 0;
    virtual void DrawOval(const Rect& oval) = 0;
    virtual void DrawCircle(const Point& centerPt, scalar radius) = 0;
    virtual void DrawPath(const Path& path) = 0;
    virtual void DrawBackground(const Brush& brush) = 0;
    virtual void DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos, scalar lightRadius,
        Color ambientColor, Color spotColor, ShadowFlags flag) = 0;

    // image
    virtual void DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py) = 0;
    virtual void DrawBitmap(Media::PixelMap& pixelMap, const scalar px, const scalar py) = 0;
    virtual void DrawImage(const Image& image, const scalar px, const scalar p, const SamplingOptions& sampling) = 0;

    // text
    virtual void DrawText(const Text& text) = 0; // TODO...

    // clip
    virtual void ClipRect(const Rect& rect, ClipOp op) = 0;
    virtual void ClipRoundRect(const RoundRect& roundRect, ClipOp op) = 0;
    virtual void ClipPath(const Path& path, ClipOp op) = 0;

    // transform
    virtual void SetMatrix(const Matrix& matrix) = 0;
    virtual void ResetMatrix() = 0;
    virtual void ConcatMatrix(const Matrix& matrix) = 0;
    virtual void Translate(scalar dx, scalar dy) = 0;
    virtual void Scale(scalar sx, scalar sy) = 0;
    virtual void Rotate(scalar deg) = 0;
    virtual void Shear(scalar sx, scalar sy) = 0;

    // state
    virtual void Flush() = 0;
    virtual void Clear(ColorQuad color) = 0;
    virtual void Save() = 0;
    virtual void SaveLayer(const Rect& rect, const Brush& brush) = 0;
    virtual void Restore() = 0;

    // paint
    virtual void AttachPen(const Pen& pen) = 0;
    virtual void AttachBrush(const Brush& brush) = 0;
    virtual void DetachPen() = 0;
    virtual void DetachBrush() = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
