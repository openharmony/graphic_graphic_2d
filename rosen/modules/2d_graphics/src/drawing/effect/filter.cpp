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

#include "config/DrawingConfig.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
Filter::Filter() noexcept
    : colorFilter_(nullptr), imageFilter_(nullptr), maskFilter_(nullptr), filterQuality_(FilterQuality::NONE)
{}

void Filter::SetColorFilter(std::shared_ptr<ColorFilter> colorFilter)
{
#ifdef DRAWING_DISABLE_API
    if (DrawingConfig::IsDisabled(DrawingConfig::DrawingDisableFlag::DISABLE_COLOR_FILTER)) {
        colorFilter_ = nullptr;
        return;
    }
#endif
    colorFilter_ = colorFilter;
}

std::shared_ptr<ColorFilter> Filter::GetColorFilter() const
{
    return colorFilter_;
}

const ColorFilter* Filter::GetColorFilterPtr() const
{
    return colorFilter_.get();
}

void Filter::SetImageFilter(std::shared_ptr<ImageFilter> imageFilter)
{
#ifdef DRAWING_DISABLE_API
    if (DrawingConfig::IsDisabled(DrawingConfig::DrawingDisableFlag::DISABLE_IMAGE_FILTER)) {
        imageFilter_ = nullptr;
        return;
    }
#endif
    imageFilter_ = imageFilter;
}

std::shared_ptr<ImageFilter> Filter::GetImageFilter() const
{
    return imageFilter_;
}

const ImageFilter* Filter::GetImageFilterPtr() const
{
    return imageFilter_.get();
}

void Filter::SetMaskFilter(std::shared_ptr<MaskFilter> maskFilter)
{
#ifdef DRAWING_DISABLE_API
    if (DrawingConfig::IsDisabled(DrawingConfig::DrawingDisableFlag::DISABLE_MASK_FILTER)) {
        maskFilter_ = nullptr;
        return;
    }
#endif
    maskFilter_ = maskFilter;
}

std::shared_ptr<MaskFilter> Filter::GetMaskFilter() const
{
    return maskFilter_;
}

const MaskFilter* Filter::GetMaskFilterPtr() const
{
    return maskFilter_.get();
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

void Filter::Dump(std::string& out) const
{
    out += '[';
    if (colorFilter_ != nullptr) {
        out += "colorFilter:" + std::to_string(static_cast<int>(colorFilter_->GetType())) + " ";
    }
    if (imageFilter_ != nullptr) {
        out += "imageFilter:" + std::to_string(static_cast<int>(imageFilter_->GetType())) + " ";
    }
    if (maskFilter_ != nullptr) {
        out += "maskFilter:" + std::to_string(static_cast<int>(maskFilter_->GetType())) + " ";
    }
    out += "filterQuality:" + std::to_string(static_cast<int>(filterQuality_));
    out += ']';
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS