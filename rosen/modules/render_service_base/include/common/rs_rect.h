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

#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_RECT_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_RECT_H
#include <cmath>

#include "common/rs_common_def.h"
#include "common/rs_vector2.h"

namespace OHOS {
namespace Rosen {
template<typename T>
class RectT {
public:
    union {
        struct {
            T left_;
            T top_;
            T width_;
            T height_;
        };
        T data_[4]; // 4 is size of data or structure
    };

    RectT()
    {
        data_[0] = 0;
        data_[1] = 0;
        data_[2] = 0;
        data_[3] = 0;
    }
    RectT(T left, T top, T width, T height)
    {
        data_[0] = left;
        data_[1] = top;
        data_[2] = width;
        data_[3] = height;
    }
    explicit RectT(const T* v)
    {
        data_[0] = v[0];
        data_[1] = v[1];
        data_[2] = v[2];
        data_[3] = v[3];
    }
    ~RectT() = default;

    inline bool operator==(const RectT<T>& rect) const
    {
        return (left_ == rect.left_) && (top_ == rect.top_) && (width_ == rect.width_) && (height_ == rect.height_);
    }

    inline bool operator!=(const RectT<T>& rect) const
    {
        return (left_ != rect.left_) || (top_ != rect.top_) || (width_ != rect.width_) || (height_ != rect.height_);
    }

    inline RectT& operator=(const RectT& other)
    {
        const T* oData = other.data_;
        data_[0] = oData[0];
        data_[1] = oData[1];
        data_[2] = oData[2];
        data_[3] = oData[3];
        return *this;
    }
    void SetAll(T left, T top, T width, T height)
    {
        data_[0] = left;
        data_[1] = top;
        data_[2] = width;
        data_[3] = height;
    }
    T GetRight() const
    {
        return left_ + width_;
    }
    T GetBottom() const
    {
        return top_ + height_;
    }
    void SetRight(T right)
    {
        width_ = right - left_;
    }
    void SetBottom(T bottom)
    {
        height_ = bottom - top_;
    }
    void Move(T x, T y)
    {
        left_ += x;
        top_ += y;
    }
    void Clear()
    {
        left_ = 0;
        top_ = 0;
        width_ = 0;
        height_ = 0;
    }
    bool IsEmpty() const
    {
        return width_ <= 0 || height_ <= 0;
    }
    bool Intersect(T x, T y) const
    {
        return (x >= left_) && (x < GetRight()) && (y >= top_) && (y < GetBottom());
    }
    RectT<T> IntersectRect(const RectT<T>& rect) const
    {
        T left = std::max(left_, rect.left_);
        T top = std::max(top_, rect.top_);
        T width = std::min(GetRight(), rect.GetRight()) - left;
        T height = std::min(GetBottom(), rect.GetBottom()) - top;
        return ((width <= 0) || (height <= 0)) ? RectT<T>() : RectT<T>(left, top, width, height);
    }
    RectT<T> JoinRect(const RectT<T>& rect) const
    {
        T left = std::min(left_, rect.left_);
        T top = std::min(top_, rect.top_);
        T width = std::max(GetRight(), rect.GetRight()) - left;
        T height = std::max(GetBottom(), rect.GetBottom()) - top;
        return ((width <= 0) || (height <= 0)) ? RectT<T>() : RectT<T>(left, top, width, height);
    }
};

typedef RectT<int> RectI;
typedef RectT<float> RectF;

template<typename T>
class RRectT {
public:
    RectT<T> rect_ = RectT<T>();
    Vector2f radius_[4] = { { 0, 0 } };

    RRectT() {}
    ~RRectT() = default;

    RRectT(RectT<T> rect, float rx, float ry)
    {
        rect_ = rect;
        Vector2f vec = Vector2f(rx, ry);
        radius_[0] = vec;
        radius_[1] = vec;
        radius_[2] = vec;
        radius_[3] = vec;
    }
    RRectT(RectT<T> rect, const Vector2f* radius)
    {
        rect_ = rect;
        radius_[0] = radius[0];
        radius_[1] = radius[1];
        radius_[2] = radius[2];
        radius_[3] = radius[3];
    }
};
typedef RRectT<float> RRect;
} // namespace Rosen
} // namespace OHOS
#endif
