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

#ifndef SIZE_H
#define SIZE_H

#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SizeF;
typedef SizeF Size;

class SizeF {
public:
    inline SizeF() noexcept;
    inline SizeF(const SizeF &s) noexcept;
    inline SizeF(scalar w, scalar h) noexcept;

    inline bool IsZero() const;
    inline bool IsEmpty() const;

    inline scalar Width() const;
    inline scalar Height() const;

    inline void SetWidth(scalar w);
    inline void SetHeight(scalar h);

    friend inline bool operator==(const SizeF &s1, const SizeF &s2);
    friend inline bool operator!=(const SizeF &s1, const SizeF &s2);

private:
    scalar width_;
    scalar height_;
};

inline SizeF::SizeF() noexcept : width_(0.0), height_(0.0) {}

inline SizeF::SizeF(const SizeF &s) noexcept : width_(s.Width()), height_(s.Height()) {}

inline SizeF::SizeF(scalar w, scalar h) noexcept : width_(w), height_(h) {}

inline bool SizeF::IsZero() const
{
    return width_ == 0 && height_ == 0;
}

inline bool SizeF::IsEmpty() const
{
    return width_ <= 0 || height_ <= 0;
}

inline scalar SizeF::Width() const
{
    return width_;
}

inline scalar SizeF::Height() const
{
    return height_;
}

inline void SizeF::SetWidth(scalar w)
{
    width_ = w;
}

inline void SizeF::SetHeight(scalar h)
{
    height_ = h;
}

inline bool operator==(const SizeF &s1, const SizeF &s2)
{
    return s1.width_ == s2.width_ && s1.height_ == s2.height_;
}

inline bool operator!=(const SizeF &s1, const SizeF &s2)
{
    return s1.width_ != s2.width_ || s1.height_ != s2.height_;
}

class SizeI {
public:
    inline SizeI() noexcept;
    inline SizeI(const SizeI &s) noexcept;
    inline SizeI(int w, int h) noexcept;

    inline bool IsZero() const;
    inline bool IsEmpty() const;

    inline int Width() const;
    inline int Height() const;

    inline void SetWidth(int w);
    inline void SetHeight(int h);

    friend inline bool operator==(const SizeI &s1, const SizeI &s2);
    friend inline bool operator!=(const SizeI &s1, const SizeI &s2);

private:
    int width_;
    int height_;
};

inline SizeI::SizeI() noexcept : width_(0.0), height_(0.0) {}

inline SizeI::SizeI(const SizeI &s) noexcept : width_(s.Width()), height_(s.Height()) {}

inline SizeI::SizeI(int w, int h) noexcept : width_(w), height_(h) {}

inline bool SizeI::IsZero() const
{
    return width_ == 0 && height_ == 0;
}

inline bool SizeI::IsEmpty() const
{
    return width_ <= 0 || height_ <= 0;
}

inline int SizeI::Width() const
{
    return width_;
}

inline int SizeI::Height() const
{
    return height_;
}

inline void SizeI::SetWidth(int w)
{
    width_ = w;
}

inline void SizeI::SetHeight(int h)
{
    height_ = h;
}

inline bool operator==(const SizeI &s1, const SizeI &s2)
{
    return s1.width_ == s2.width_ && s1.height_ == s2.height_;
}

inline bool operator!=(const SizeI &s1, const SizeI &s2)
{
    return s1.width_ != s2.width_ || s1.height_ != s2.height_;
}
}
}
}
#endif