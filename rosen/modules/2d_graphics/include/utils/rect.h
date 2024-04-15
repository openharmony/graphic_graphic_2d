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

#ifndef RECT_H
#define RECT_H

#include <cmath>
#include <string>
#include "utils/drawing_macros.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RectF;

typedef RectF Rect;

#define DRAWING_MAX_S32_FITS_IN_FLOAT    2147483520
#define DRAWING_MIN_S32_FITS_IN_FLOAT    (-DRAWING_MAX_S32_FITS_IN_FLOAT)

static inline int DrawingFloatSaturate2Int(float x)
{
    x = x < DRAWING_MAX_S32_FITS_IN_FLOAT ? x : DRAWING_MAX_S32_FITS_IN_FLOAT;
    x = x > DRAWING_MIN_S32_FITS_IN_FLOAT ? x : DRAWING_MIN_S32_FITS_IN_FLOAT;
    return (int)x;
}

class DRAWING_API RectI {
public:
    inline RectI() noexcept;
    inline RectI(const RectI& r) noexcept;
    inline RectI(const int l, const int t, const int r, const int b) noexcept;

    ~RectI() {}

    inline bool IsValid() const;
    inline bool IsEmpty() const;

    inline int GetLeft() const;
    inline int GetTop() const;
    inline int GetRight() const;
    inline int GetBottom() const;

    inline int GetWidth() const;
    inline int GetHeight() const;

    inline void SetLeft(int pos);
    inline void SetTop(int pos);
    inline void SetRight(int pos);
    inline void SetBottom(int pos);

    /**
     * @brief Offsets RectI by adding dx to left, right; and by adding dy to top, bottom.
     * If dx is negative, moves RectI returned to the left.
     * If dx is positive, moves RectI returned to the right.
     * If dy is negative, moves RectI returned upward.
     * If dy is positive, moves RectI returned downward.
     * @param dx  offset added to left and right
     * @param dy  offset added to top and bottom
     */
    inline void Offset(int dx, int dy);

    /**
     * @brief outset by (dx, dy).
     * If dx is negative, RectI is narrower.
     * If dx is positive, RectI is wider.
     * If dy is negative, RectI is shorter.
     * If dy is positive, RectI is taller.
     * @param dx offset subtracted to left and added from right
     * @param dy offset subtracted to top and added from bottom
     */
    inline void MakeOutset(int dx, int dy);

    /**
     * @brief Returns true if RectI contains other.
     * Returns false if RectI is empty or other is empty.
     * RectI contains other when RectI area completely includes other area.
     * @param other RectI contained
     * @return true if all sides of RectI are outside other
     */
    inline bool Contains(const RectI& other) const;

    /**
     * @brief If RectI intersects other, sets RectI to intersection.
     * @param other limit of result.
     * @return true if other and RectI have area in common.
     */
    inline bool Intersect(const RectI& other);

    /**
     * @brief If other is valid, sets RectI to the union of itself and other.
     * @param other expansion RectI.
     * @return true if other is valid.
     */
    inline bool Join(const RectI& other);

    inline std::string ToString() const;

    friend inline bool operator==(const RectI& r1, const RectI& r2);
    friend inline bool operator!=(const RectI& r1, const RectI& r2);

    int32_t left_;
    int32_t right_;
    int32_t top_;
    int32_t bottom_;
};

inline RectI::RectI() noexcept : left_(0), right_(0), top_(0), bottom_(0) {}

inline RectI::RectI(const RectI& r) noexcept
    : left_(r.GetLeft()), right_(r.GetRight()), top_(r.GetTop()), bottom_(r.GetBottom())
{}

inline RectI::RectI(const int l, const int t, const int r, const int b) noexcept
    : left_(l), right_(r), top_(t), bottom_(b)
{}

inline bool RectI::IsValid() const
{
    return !IsEmpty();
}

inline bool RectI::IsEmpty() const
{
    int64_t w = (int64_t)right_ - (int64_t)left_;
    int64_t h = (int64_t)bottom_ - (int64_t)top_;
    int64_t r = w | h;
    // return true if either under 0 or either exceed int32_t
    return r <= 0 || (r & 0xffffffff) < 0;
}

inline int RectI::GetLeft() const
{
    return left_;
}

inline int RectI::GetTop() const
{
    return top_;
}

inline int RectI::GetRight() const
{
    return right_;
}

inline int RectI::GetBottom() const
{
    return bottom_;
}

inline int RectI::GetWidth() const
{
    return right_ - left_;
}

inline int RectI::GetHeight() const
{
    return bottom_ - top_;
}

inline void RectI::SetLeft(int pos)
{
    left_ = pos;
}

inline void RectI::SetTop(int pos)
{
    top_ = pos;
}

inline void RectI::SetRight(int pos)
{
    right_ = pos;
}

inline void RectI::SetBottom(int pos)
{
    bottom_ = pos;
}

inline void RectI::Offset(int dx, int dy)
{
    left_ += dx;
    right_ += dx;
    top_ += dy;
    bottom_ += dy;
}

inline void RectI::MakeOutset(int dx, int dy)
{
    left_ -= dx;
    right_ += dx;
    top_ -= dy;
    bottom_ += dy;
}

inline bool RectI::Intersect(const RectI& other)
{
    RectI rectI(left_ > other.left_ ? left_ : other.left_, top_ > other.top_ ? top_ : other.top_,
        right_ < other.right_ ? right_ : other.right_, bottom_ < other.bottom_ ? bottom_ : other.bottom_);
    if (!rectI.IsValid()) {
        return false;
    }
    *this = rectI;
    return true;
}

inline bool RectI::Join(const RectI& other)
{
    if (!other.IsValid()) {
        return false;
    }
    if (!IsValid()) {
        *this = other;
    } else {
        *this = RectI(left_ < other.left_ ? left_ : other.left_, top_ < other.top_ ? top_ : other.top_,
            right_ > other.right_ ? right_ : other.right_, bottom_ > other.bottom_ ? bottom_ : other.bottom_);
    }
    return true;
}

inline bool RectI::Contains(const RectI& other) const
{
    return !other.IsEmpty() && !this->IsEmpty() &&
        left_ <= other.left_ && top_ <= other.top_ &&
        right_ >= other.right_ && bottom_ >= other.bottom_;
}

inline std::string RectI::ToString() const
{
    return std::string("(") + std::to_string(left_) + ", " + std::to_string(top_) + ", " +
        std::to_string(right_ - left_) + ", " + std::to_string(bottom_ - top_) + ")";
}

inline bool operator==(const RectI& r1, const RectI& r2)
{
    return r1.left_ == r2.left_ && r1.right_ == r2.right_ && r1.top_ == r2.top_ && r1.bottom_ == r2.bottom_;
}

inline bool operator!=(const RectI& r1, const RectI& r2)
{
    return r1.left_ != r2.left_ || r1.right_ != r2.right_ || r1.top_ != r2.top_ || r1.bottom_ != r2.bottom_;
}

class DRAWING_API RectF {
public:
    inline RectF() noexcept;
    inline RectF(const RectF& r) noexcept;
    inline RectF(const RectI& r) noexcept;
    inline RectF(const scalar l, const scalar t, const scalar r, const scalar b) noexcept;

    ~RectF() {}

    inline bool IsValid() const;
    inline bool IsEmpty() const;

    inline scalar GetLeft() const;
    inline scalar GetTop() const;
    inline scalar GetRight() const;
    inline scalar GetBottom() const;

    inline scalar GetWidth() const;
    inline scalar GetHeight() const;

    inline void SetLeft(scalar pos);
    inline void SetTop(scalar pos);
    inline void SetRight(scalar pos);
    inline void SetBottom(scalar pos);

    inline void Offset(scalar dx, scalar dy);
    inline void MakeOutset(scalar dx, scalar dy);
    inline void Round();
    inline RectI RoundOut();
    inline std::string ToString() const;

    /*
     * @brief        If RectF intersects other, sets RectF to intersection.
     * @param other  limit of result.
     * @return       true if other and RectF have area in common.
     */
    inline bool Intersect(const RectF& other);

    /*
     * @brief        If other is valid, sets RectF to the union of itself and other.
     * @param other  expansion RectF.
     * @return       true if other is valid.
     */
    inline bool Join(const RectF& other);

    friend inline bool operator==(const RectF& r1, const RectF& r2);
    friend inline bool operator!=(const RectF& r1, const RectF& r2);

    scalar left_;
    scalar right_;
    scalar top_;
    scalar bottom_;
};

inline RectF::RectF() noexcept : left_(0.0), right_(0.0), top_(0.0), bottom_(0.0) {}

inline RectF::RectF(const RectF& r) noexcept
    : left_(r.GetLeft()), right_(r.GetRight()), top_(r.GetTop()), bottom_(r.GetBottom())
{}

inline RectF::RectF(const RectI& r) noexcept
    : left_(r.GetLeft()), right_(r.GetRight()), top_(r.GetTop()), bottom_(r.GetBottom())
{}

inline RectF::RectF(const scalar l, const scalar t, const scalar r, const scalar b) noexcept
    : left_(l), right_(r), top_(t), bottom_(b)
{}

inline bool RectF::IsValid() const
{
    return left_ < right_ && top_ < bottom_;
}

inline bool RectF::IsEmpty() const
{
    return !(left_ < right_ && top_ < bottom_);
}

inline scalar RectF::GetLeft() const
{
    return left_;
}

inline scalar RectF::GetTop() const
{
    return top_;
}

inline scalar RectF::GetRight() const
{
    return right_;
}

inline scalar RectF::GetBottom() const
{
    return bottom_;
}

inline scalar RectF::GetWidth() const
{
    return right_ - left_;
}

inline scalar RectF::GetHeight() const
{
    return bottom_ - top_;
}

inline void RectF::SetLeft(scalar pos)
{
    left_ = pos;
}

inline void RectF::SetTop(scalar pos)
{
    top_ = pos;
}

inline void RectF::SetRight(scalar pos)
{
    right_ = pos;
}

inline void RectF::SetBottom(scalar pos)
{
    bottom_ = pos;
}

inline void RectF::Offset(scalar dx, scalar dy)
{
    left_ += dx;
    right_ += dx;
    top_ += dy;
    bottom_ += dy;
}

inline void RectF::MakeOutset(scalar dx, scalar dy)
{
    left_ -= dx;
    right_ += dx;
    top_ -= dy;
    bottom_ += dy;
}

inline void RectF::Round()
{
    left_ = DrawingFloatSaturate2Int(left_ + 0.5f);
    right_ = DrawingFloatSaturate2Int(right_ + 0.5f);
    top_ = DrawingFloatSaturate2Int(top_ + 0.5f);
    bottom_ = DrawingFloatSaturate2Int(bottom_ + 0.5f);
}

inline RectI RectF::RoundOut()
{
    int32_t left = DrawingFloatSaturate2Int(floorf(left_));
    int32_t right = DrawingFloatSaturate2Int(ceilf(right_));
    int32_t top = DrawingFloatSaturate2Int(floorf(top_));
    int32_t bottom = DrawingFloatSaturate2Int(ceilf(bottom_));
    return RectI(left, top, right, bottom);
}

inline bool RectF::Intersect(const RectF& other)
{
    RectF rectF(left_ > other.left_ ? left_ : other.left_, top_ > other.top_ ? top_ : other.top_,
                right_ < other.right_ ? right_ : other.right_, bottom_ < other.bottom_ ? bottom_ : other.bottom_);
    if (!rectF.IsValid()) {
        return false;
    }
    *this = rectF;
    return true;
}

inline bool RectF::Join(const RectF& other)
{
    if (!other.IsValid()) {
        return false;
    }
    if (!IsValid()) {
        *this = other;
    } else {
        *this = RectF(left_ < other.left_ ? left_ : other.left_, top_ < other.top_ ? top_ : other.top_,
            right_ > other.right_ ? right_ : other.right_, bottom_ > other.bottom_ ? bottom_ : other.bottom_);
    }
    return true;
}

inline std::string RectF::ToString() const
{
    return std::string("(") + std::to_string(left_) + ", " + std::to_string(top_) + ", " +
        std::to_string(right_ - left_) + ", " + std::to_string(bottom_ - top_) + ")";
}

inline bool operator==(const RectF& r1, const RectF& r2)
{
    return IsScalarAlmostEqual(r1.left_, r2.left_) && IsScalarAlmostEqual(r1.right_, r2.right_) &&
        IsScalarAlmostEqual(r1.top_, r2.top_) && IsScalarAlmostEqual(r1.bottom_, r2.bottom_);
}

inline bool operator!=(const RectF& r1, const RectF& r2)
{
    return !IsScalarAlmostEqual(r1.left_, r2.left_) || !IsScalarAlmostEqual(r1.right_, r2.right_) ||
        !IsScalarAlmostEqual(r1.top_, r2.top_) || !IsScalarAlmostEqual(r1.bottom_, r2.bottom_);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
