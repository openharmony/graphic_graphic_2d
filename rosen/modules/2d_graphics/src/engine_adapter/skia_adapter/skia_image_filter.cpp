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

#include "skia_image_filter.h"

#include "include/effects/SkImageFilters.h"

#if !defined(USE_CANVASKIT0310_SKIA)
#include "include/effects/SkBlurImageFilter.h"
#endif

#include "effect/color_filter.h"
#include "effect/image_filter.h"
#include "skia_color_filter.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaImageFilter::SkiaImageFilter() noexcept : filter_(nullptr) {}

void SkiaImageFilter::InitWithBlur(scalar sigmaX , scalar sigmaY, const ImageFilter& f)
{
    auto input = f.GetImpl<SkiaImageFilter>();
    if (input != nullptr) {
#if defined(USE_CANVASKIT0310_SKIA)
        filter_ = SkImageFilters::Blur(sigmaX, sigmaY, input->GetImageFilter());
#else
        filter_ = SkBlurImageFilter::Make(sigmaX, sigmaY, input->GetImageFilter());
#endif
    }
}

void SkiaImageFilter::InitWithColor(const ColorFilter& colorFilter, const ImageFilter& f)
{
    auto skColorFilterImpl = colorFilter.GetImpl<SkiaColorFilter>();
    auto input = f.GetImpl<SkiaImageFilter>();
    if (skColorFilterImpl != nullptr && input != nullptr) {
        filter_ = SkImageFilters::ColorFilter(skColorFilterImpl->GetColorFilter(), input->GetImageFilter());
    }
}

void SkiaImageFilter::InitWithOffset(scalar dx, scalar dy, const ImageFilter& f)
{
    auto input = f.GetImpl<SkiaImageFilter>();
    if (input != nullptr) {
        filter_ = SkImageFilters::Offset(dx, dy, input->GetImageFilter());
    }
}

void SkiaImageFilter::InitWithArithmetic(scalar k1, scalar k2, scalar k3, scalar k4, bool enforcePMColor,
                                         const ImageFilter& f1, const ImageFilter& f2)
{
    auto background = f1.GetImpl<SkiaImageFilter>();
    auto foreground = f2.GetImpl<SkiaImageFilter>();
    if (background != nullptr && foreground != nullptr) {
        filter_ = SkImageFilters::Arithmetic(k1, k2, k3, k4, enforcePMColor,
            background->GetImageFilter(), foreground->GetImageFilter());
    }
}

void SkiaImageFilter::InitWithCompose(const ImageFilter& f1, const ImageFilter& f2)
{
    auto outer = f1.GetImpl<SkiaImageFilter>();
    auto inner = f2.GetImpl<SkiaImageFilter>();
    if (outer != nullptr && inner != nullptr) {
        filter_ = SkImageFilters::Compose(outer->GetImageFilter(), inner->GetImageFilter());
    }
}

sk_sp<SkImageFilter> SkiaImageFilter::GetImageFilter() const
{
    return filter_;
}
}
}
}