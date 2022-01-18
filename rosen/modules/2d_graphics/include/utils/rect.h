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

#ifndef RECT_H
#define RECT_H

#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RectF;

typedef RectF Rect;

class RectF {
public:
    inline RectF() noexcept;
    inline RectF(const RectF& r) noexcept;
    inline RectF(const scalar l, const scalar t, const scalar r, const scalar b) noexcept;

    ~RectF() {}

    inline bool IsValid() const;

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

    friend inline bool operator==(const RectF& r1, const RectF& r2);
    friend inline bool operator!=(const RectF& r1, const RectF& r2);

private:
    scalar left_;
    scalar right_;
    scalar top_;
    scalar bottom_;
};

inline RectF::RectF() noexcept : left_(0.0), right_(0.0), top_(0.0), bottom_(0.0) {}

inline RectF::RectF(const RectF& r) noexcept
    : left_(r.GetLeft()), right_(r.GetRight()), top_(r.GetTop()), bottom_(r.GetBottom())
{}

inline RectF::RectF(const scalar l, const scalar t, const scalar r, const scalar b) noexcept
    : left_(l), right_(r), top_(t), bottom_(b)
{}

inline bool RectF::IsValid() const
{
    return left_ < right_ && top_ < bottom_;
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

class RectI {
public:
    inline RectI() noexcept;
    inline RectI(const RectI& r) noexcept;
    inline RectI(const int l, const int t, const int r, const int b) noexcept;

    ~RectI() {}

    inline bool IsValid() const;

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

    inline void Offset(int dx, int dy);

    friend inline bool operator==(const RectI& r1, const RectI& r2);
    friend inline bool operator!=(const RectI& r1, const RectI& r2);

private:
    int left_;
    int right_;
    int top_;
    int bottom_;
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
    return left_ <= right_ && top_ <= bottom_;
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

inline bool operator==(const RectI& r1, const RectI& r2)
{
    return r1.left_ == r2.left_ && r1.right_ == r2.right_ && r1.top_ == r2.top_ && r1.bottom_ == r2.bottom_;
}

inline bool operator!=(const RectI& r1, const RectI& r2)
{
    return r1.left_ != r2.left_ || r1.right_ != r2.right_ || r1.top_ != r2.top_ || r1.bottom_ != r2.bottom_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
