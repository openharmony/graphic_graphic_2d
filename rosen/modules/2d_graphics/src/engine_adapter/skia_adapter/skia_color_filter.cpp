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

#include "skia_color_filter.h"

#include "effect/color_filter.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaColorFilter::SkiaColorFilter() noexcept : filter_(nullptr) {}

void SkiaColorFilter::InitWithBlendMode(ColorQuad c, BlendMode mode)
{
    filter_ = SkColorFilters::Blend(static_cast<SkColor>(c), static_cast<SkBlendMode>(mode));
}

void SkiaColorFilter::InitWithColorMatrix(const ColorMatrix& m)
{
    scalar dst[ColorMatrix::MATRIX_SIZE];
    m.GetArray(dst);
    filter_ = SkColorFilters::Matrix(dst);
}

void SkiaColorFilter::InitWithLinearToSrgbGamma()
{
    filter_ = SkColorFilters::LinearToSRGBGamma();
}

void SkiaColorFilter::InitWithSrgbGammaToLinear()
{
    filter_ = SkColorFilters::SRGBToLinearGamma();
}

void SkiaColorFilter::InitWithCompose(const ColorFilter& f1, const ColorFilter& f2)
{
    auto outer = f1.GetImpl<SkiaColorFilter>();
    auto inner = f2.GetImpl<SkiaColorFilter>();
    if (outer != nullptr && inner != nullptr) {
        filter_ = SkColorFilters::Compose(outer->GetColorFilter(), inner->GetColorFilter());
    }
}

void SkiaColorFilter::Compose(const ColorFilter& f)
{
    auto skColorFilterImpl = f.GetImpl<SkiaColorFilter>();
    if (skColorFilterImpl != nullptr) {
        filter_ = filter_->makeComposed(skColorFilterImpl->GetColorFilter());
    }
}

sk_sp<SkColorFilter> SkiaColorFilter::GetColorFilter() const
{
    return filter_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS