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

#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_RECT_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_RECT_H
#include <cmath>
#include <unordered_set>
#include <cinttypes>

#include "common/rs_common_def.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "transaction/rs_marshalling_helper.h"

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
    RectT(Vector4<T> vector)
    {
        data_[0] = vector[0];
        data_[1] = vector[1];
        data_[2] = vector[2];
        data_[3] = vector[3];
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
        return ROSEN_EQ<T>(left_, rect.left_) && ROSEN_EQ<T>(top_, rect.top_) &&
            ROSEN_EQ<T>(width_, rect.width_) && ROSEN_EQ<T>(height_, rect.height_);
    }

    inline bool operator!=(const RectT<T>& rect) const
    {
        return !operator==(rect);
    }

    inline bool IsNearEqual(const RectT<T>& rect, T threshold = std::numeric_limits<T>::epsilon()) const
    {
        return ROSEN_EQ<T>(left_, rect.left_, threshold) && ROSEN_EQ<T>(top_, rect.top_, threshold) &&
            ROSEN_EQ<T>(width_, rect.width_, threshold) && ROSEN_EQ<T>(height_, rect.height_, threshold);
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
    T GetLeft() const
    {
        return left_;
    }
    T GetBottom() const
    {
        return top_ + height_;
    }
    T GetTop() const
    {
        return top_;
    }
    T GetWidth() const
    {
        return width_;
    }
    T GetHeight() const
    {
        return height_;
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
    bool IsInsideOf(const RectT<T>& rect) const
    {
        return (top_ >= rect.top_ && left_ >= rect.left_ &&
            GetBottom() <= rect.GetBottom() && GetRight() <= rect.GetRight());
    }
    bool Intersect(const RectT<T>& other) const
    {
        return !IsEmpty() && !other.IsEmpty() && (left_ <= other.left_ + other.width_) &&
               (other.left_ <= left_ + width_) && (top_ <= other.top_ + other.height_) &&
               (other.top_ <= top_ + height_);
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
        if (rect.IsEmpty()) {
            return RectT<T>(left_, top_, width_, height_);
        }
        if (IsEmpty()) {
            return rect;
        }
        T left = std::min(left_, rect.left_);
        T top = std::min(top_, rect.top_);
        T width = std::max(GetRight(), rect.GetRight()) - left;
        T height = std::max(GetBottom(), rect.GetBottom()) - top;
        return ((width <= 0) || (height <= 0)) ? RectT<T>() : RectT<T>(left, top, width, height);
    }
    RectT<T> Offset(const T x, const T y) const
    {
        return RectT<T>(left_ + x, top_ + y, width_, height_);
    }
    template<typename P>
    RectT<P> ConvertTo()
    {
        return RectT<P>(static_cast<P>(left_), static_cast<P>(top_), static_cast<P>(width_), static_cast<P>(height_));
    }
    std::string ToString() const
    {
        return std::string("(") + std::to_string(left_) + ", " + std::to_string(top_) + ", " +
            std::to_string(width_) + ", " + std::to_string(height_) + ")";
    }

    // outset: left, top, right, bottom
    RectT<T> MakeOutset(Vector4<T> outset) const
    {
        return RectT(left_ - outset.x_,
                     top_ - outset.y_,
                     width_ + outset.x_ + outset.z_,
                     height_ + outset.y_ + outset.w_);
    }

    #ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const
    {
        if (!(RSMarshallingHelper::Marshalling(parcel, left_) &&
                RSMarshallingHelper::Marshalling(parcel, top_) &&
                RSMarshallingHelper::Marshalling(parcel, width_) &&
                RSMarshallingHelper::Marshalling(parcel, height_))) {
            return false;
        }
        return true;
    }

    [[nodiscard]] static RectT<T>* Unmarshalling(Parcel& parcel)
    {
        auto rect = std::make_unique<RectT<T>>();
        if (!(RSMarshallingHelper::Unmarshalling(parcel, rect->left_) &&
                RSMarshallingHelper::Unmarshalling(parcel, rect->top_) &&
                RSMarshallingHelper::Unmarshalling(parcel, rect->width_) &&
                RSMarshallingHelper::Unmarshalling(parcel, rect->height_))) {
            return nullptr;
        }
        return rect.release();
    }
    #endif
};

typedef RectT<int> RectI;
typedef RectT<float> RectF;

/*
    RectIComparator: Used for comparing rects
    RectI_Hash_Func: provide hash value for rect comparing
*/
struct RectIComparator {
    bool operator()(const std::pair<NodeId, RectI>& p1, const std::pair<NodeId, RectI>& p2) const
    {
        return p2.second.IsInsideOf(p1.second);
    }
};

struct RectI_Hash_Func {
    size_t operator()(const std::pair<NodeId, RectI>& p) const
    {
        // this is set for all rects can be compared
        int hash_value = 0;
        return std::hash<int>()(hash_value);
    }
};

typedef std::unordered_set<std::pair<NodeId, RectI>, RectI_Hash_Func, RectIComparator> OcclusionRectISet;

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
    RRectT(RectT<T> rect, const Vector4f& radius)
    {
        rect_ = rect;
        radius_[0] = { radius[0], radius[0] };
        radius_[1] = { radius[1], radius[1] };
        radius_[2] = { radius[2], radius[2] };
        radius_[3] = { radius[3], radius[3] };
    }

    void SetValues(RectT<T> rect, const Vector2f* radius)
    {
        rect_ = rect;
        radius_[0] = radius[0];
        radius_[1] = radius[1];
        radius_[2] = radius[2];
        radius_[3] = radius[3];
    }

    RRectT operator-(const RRectT<T>& other) const;
    RRectT operator+(const RRectT<T>& other) const;
    RRectT operator/(float scale) const;
    RRectT operator*(float scale) const;
    RRectT& operator-=(const RRectT<T>& other);
    RRectT& operator+=(const RRectT<T>& other);
    RRectT& operator*=(float scale);
    RRectT& operator=(const RRectT<T>& other);
    bool operator==(const RRectT& other) const;
    bool operator!=(const RRectT& other) const;
    bool IsNearEqual(const RRectT& other, T threshold = std::numeric_limits<T>::epsilon()) const;
};

typedef RRectT<float> RRect;

template<typename T>
RRectT<T> RRectT<T>::operator-(const RRectT<T>& other) const
{
    RRectT<T> rrect;
    rrect.rect_.SetAll(rect_.GetLeft() - other.rect_.GetLeft(), rect_.GetTop() - other.rect_.GetTop(),
        rect_.GetWidth() - other.rect_.GetWidth(), rect_.GetHeight() - other.rect_.GetHeight());
    for (int index = 0; index < 4; index++) {
        rrect.radius_[index] = radius_[index] - other.radius_[index];
    }
    return rrect;
}

template<typename T>
RRectT<T> RRectT<T>::operator+(const RRectT<T>& other) const
{
    RRectT<T> rrect;
    rrect.rect_.SetAll(rect_.GetLeft() + other.rect_.GetLeft(), rect_.GetTop() + other.rect_.GetTop(),
        rect_.GetWidth() + other.rect_.GetWidth(), rect_.GetHeight() + other.rect_.GetHeight());
    for (int index = 0; index < 4; index++) {
        rrect.radius_[index] = radius_[index] + other.radius_[index];
    }
    return rrect;
}

template<typename T>
RRectT<T> RRectT<T>::operator/(float scale) const
{
    if (ROSEN_EQ<float>(scale, 0)) {
        return *this;
    }
    RRectT<T> rrect;
    rrect.rect_.SetAll(rect_.GetLeft() / scale, rect_.GetTop() / scale,
        rect_.GetWidth() / scale, rect_.GetHeight() / scale);
    for (int index = 0; index < 4; index++) {
        rrect.radius_[index] = radius_[index] / scale;
    }
    return rrect;
}

template<typename T>
RRectT<T> RRectT<T>::operator*(float scale) const
{
    RRectT<T> rrect;
    rrect.rect_.SetAll(rect_.GetLeft() * scale, rect_.GetTop() * scale,
        rect_.GetWidth() * scale, rect_.GetHeight() * scale);
    for (int index = 0; index < 4; index++) {
        rrect.radius_[index] = radius_[index] * scale;
    }
    return rrect;
}

template<typename T>
RRectT<T>& RRectT<T>::operator-=(const RRectT<T>& other)
{
    rect_.SetAll(rect_.GetLeft() - other.rect_.GetLeft(), rect_.GetTop() - other.rect_.GetTop(),
        rect_.GetWidth() - other.rect_.GetWidth(), rect_.GetHeight() - other.rect_.GetHeight());
    for (int index = 0; index < 4; index++) {
        radius_[index] = radius_[index] - other.radius_[index];
    }
    return *this;
}

template<typename T>
RRectT<T>& RRectT<T>::operator+=(const RRectT<T>& other)
{
    rect_.SetAll(rect_.GetLeft() + other.rect_.GetLeft(), rect_.GetTop() + other.rect_.GetTop(),
        rect_.GetWidth() + other.rect_.GetWidth(), rect_.GetHeight() + other.rect_.GetHeight());
    for (int index = 0; index < 4; index++) {
        radius_[index] = radius_[index] + other.radius_[index];
    }
    return *this;
}

template<typename T>
RRectT<T>& RRectT<T>::operator*=(float scale)
{
    rect_.SetAll(rect_.GetLeft() * scale, rect_.GetTop() * scale,
        rect_.GetWidth() * scale, rect_.GetHeight() * scale);
    for (int index = 0; index < 4; index++) {
        radius_[index] = radius_[index] * scale;
    }
    return *this;
}

template<typename T>
RRectT<T>& RRectT<T>::operator=(const RRectT<T>& other)
{
    rect_ = other.rect_;
    for (int index = 0; index < 4; index++) {
        radius_[index] = other.radius_[index];
    }
    return *this;
}

template<typename T>
inline bool RRectT<T>::operator==(const RRectT& other) const
{
    return (rect_ == other.rect_) && (radius_[0] == other.radius_[0]) &&
        (radius_[1] == other.radius_[1]) && (radius_[2] == other.radius_[2]) &&
        (radius_[3] == other.radius_[3]);
}

template<typename T>
inline bool RRectT<T>::operator!=(const RRectT& other) const
{
    return !operator==(other);
}

template<typename T>
inline bool RRectT<T>::IsNearEqual(const RRectT& rrectT, T threshold) const
{
    return (rect_.IsNearEqual(rrectT.rect_, threshold)) && (radius_[0].IsNearEqual(rrectT.radius_[0], threshold)) &&
        (radius_[1].IsNearEqual(rrectT.radius_[1], threshold)) &&
        (radius_[2].IsNearEqual(rrectT.radius_[2], threshold)) &&
        (radius_[3].IsNearEqual(rrectT.radius_[3], threshold));
}
} // namespace Rosen
} // namespace OHOS
#endif
