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

#ifndef ROUND_RECT_H
#define ROUND_RECT_H

#include <vector>

#include "utils/point.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RoundRect {
public:
    enum CornerPos {
        TOP_LEFT_POS,
        TOP_RIGHT_POS,
        BOTTOM_RIGHT_POS,
        BOTTOM_LEFT_POS,
    };

    inline RoundRect() noexcept;
    inline ~RoundRect() {}

    inline RoundRect(const RoundRect& roundRect) noexcept;
    inline RoundRect(const Rect& r, scalar xRad, scalar yRad) noexcept;
    inline RoundRect(const Rect& r, std::vector<Point>& radiusXY) noexcept;

    inline void SetCornerRadius(CornerPos pos, scalar radiusX, scalar radiusY);
    inline Point GetCornerRadius(CornerPos pos) const;

    inline void SetRect(const Rect& rect);
    inline Rect GetRect() const;

    inline void Offset(scalar dx, scalar dy);

private:
    Rect rect_;
    std::vector<Point> radiusXY_;
};

inline RoundRect::RoundRect() noexcept : rect_(), radiusXY_(4, Point(0, 0)) {}

inline RoundRect::RoundRect(const RoundRect& roundRect) noexcept : RoundRect()
{
    rect_ = roundRect.rect_;
    for (size_t i = 0; i < radiusXY_.size(); ++i) {
        radiusXY_[i] = roundRect.radiusXY_[i];
    }
}

inline RoundRect::RoundRect(const Rect& r, scalar xRad, scalar yRad) noexcept : RoundRect()
{
    rect_ = r;
    for (size_t i = 0; i < radiusXY_.size(); ++i) {
        radiusXY_[i].SetX(xRad);
        radiusXY_[i].SetY(yRad);
    }
}

inline RoundRect::RoundRect(const Rect& r, std::vector<Point>& radiusXY) noexcept : RoundRect()
{
    rect_ = r;
    for (size_t i = 0; i < radiusXY_.size(); ++i) {
        radiusXY_[i] = radiusXY[i];
    }
}

inline void RoundRect::SetCornerRadius(CornerPos pos, scalar radiusX, scalar radiusY)
{
    radiusXY_[pos].SetX(radiusX);
    radiusXY_[pos].SetY(radiusY);
}

inline Point RoundRect::GetCornerRadius(CornerPos pos) const
{
    return radiusXY_[pos];
}

inline void RoundRect::SetRect(const Rect& rect)
{
    rect_ = rect;
}

inline Rect RoundRect::GetRect() const
{
    return rect_;
}

inline void RoundRect::Offset(scalar dx, scalar dy)
{
    rect_.Offset(dx, dy);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif