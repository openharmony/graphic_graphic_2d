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

#include <effect/filter.h>

namespace OHOS {
namespace Rosen {
namespace Drawing {
Filter::Filter() noexcept
    : colorFilter_(nullptr), imageFilter_(nullptr), maskFilter_(nullptr), filterQuality_(FilterQuality::NONE)
{}

void Filter::SetColorFilter(std::shared_ptr<ColorFilter> colorFilter)
{
    colorFilter_ = colorFilter;
}

std::shared_ptr<ColorFilter> Filter::GetColorFilter() const
{
    return colorFilter_;
}

void Filter::SetImageFilter(std::shared_ptr<ImageFilter> imageFilter)
{
    imageFilter_ = imageFilter;
}

std::shared_ptr<ImageFilter> Filter::GetImageFilter() const
{
    return imageFilter_;
}

void Filter::SetMaskFilter(std::shared_ptr<MaskFilter> maskFilter)
{
    maskFilter_ = maskFilter;
}

std::shared_ptr<MaskFilter> Filter::GetMaskFilter() const
{
    return maskFilter_;
}

void Filter::SetFilterQuality(FilterQuality filterQuality)
{
    filterQuality_ = filterQuality;
}

Filter::FilterQuality Filter::GetFilterQuality() const
{
    return filterQuality_;
}

void Filter::Reset()
{
    colorFilter_ = nullptr;
    imageFilter_ = nullptr;
    maskFilter_ = nullptr;
    filterQuality_ = FilterQuality::NONE;
}

bool operator==(const Filter& f1, const Filter& f2)
{
    return f1.colorFilter_ == f2.colorFilter_ && f1.imageFilter_ == f2.imageFilter_ &&
        f1.maskFilter_ == f2.maskFilter_ && f1.filterQuality_ == f2.filterQuality_;
}

bool operator!=(const Filter& f1, const Filter& f2)
{
    return f1.colorFilter_ != f2.colorFilter_ || f1.imageFilter_ != f2.imageFilter_ ||
        f1.maskFilter_ != f2.maskFilter_ || f1.filterQuality_ != f2.filterQuality_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS