/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#ifndef RENDER_SERVICE_BASE_COMMON_RS_RECT_SHAPE_H
#define RENDER_SERVICE_BASE_COMMON_RS_RECT_SHAPE_H
 
#include "common/rs_common_def.h"
#include "common/rs_vector4.h"
 
namespace OHOS {
namespace Rosen {
 
// RectShape represents an animatable rounded rectangle defined by width, height
// and per-corner radii (left-top, right-top, right-bottom, left-bottom).
// It supports RSAnimatableArithmetic operations so it can be used with
// RSRenderAnimatableProperty for animation interpolation.
struct RectShape {
    float width_ = 0.0f;
    float height_ = 0.0f;
    Vector4f cornerRadius_ = Vector4f(0.0f); // left-top, right-top, right-bottom, left-bottom
 
    RectShape() = default;
 
    RectShape(float width, float height, const Vector4f& cornerRadius)
        : width_(width), height_(height), cornerRadius_(cornerRadius)
    {}
 
    RectShape operator+(const RectShape& other) const
    {
        return RectShape(width_ + other.width_, height_ + other.height_,
            cornerRadius_ + other.cornerRadius_);
    }
 
    RectShape operator-(const RectShape& other) const
    {
        return RectShape(width_ - other.width_, height_ - other.height_,
            cornerRadius_ - other.cornerRadius_);
    }
 
    RectShape operator*(float scale) const
    {
        return RectShape(width_ * scale, height_ * scale, cornerRadius_ * scale);
    }
 
    bool operator==(const RectShape& other) const
    {
        return ROSEN_EQ(width_, other.width_) && ROSEN_EQ(height_, other.height_) &&
            cornerRadius_ == other.cornerRadius_;
    }
 
    float GetWidth() const
    {
        return width_;
    }
 
    float GetHeight() const
    {
        return height_;
    }
 
    Vector4f GetCornerRadius() const
    {
        return cornerRadius_;
    }
};
 
} // namespace Rosen
} // namespace OHOS
 
#endif // RENDER_SERVICE_BASE_COMMON_RS_RECT_SHAPE_H