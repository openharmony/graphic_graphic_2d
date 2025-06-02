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

#include "skia_color_filter.h"
#include "skia_helper.h"

#include "include/effects/SkColorMatrixFilter.h"
#include "include/effects/SkLumaColorFilter.h"
#include "include/effects/SkOverdrawColorFilter.h"
#include "src/core/SkColorFilterBase.h"

#include "effect/color_filter.h"
#include "utils/data.h"
#include "utils/log.h"
#include "utils/performanceCaculate.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaColorFilter::SkiaColorFilter() noexcept : filter_(nullptr) {}

static SkColorFilters::Clamp ConverToSkClamp(Drawing::Clamp clamp)
{
    return (clamp == Drawing::Clamp::YES_CLAMP ? SkColorFilters::Clamp::kYes : SkColorFilters::Clamp::kNo);
}

void SkiaColorFilter::InitWithBlendMode(ColorQuad c, BlendMode mode)
{
    filter_ = SkColorFilters::Blend(static_cast<SkColor>(c), static_cast<SkBlendMode>(mode));
}

void SkiaColorFilter::InitWithColorMatrix(const ColorMatrix& m, Clamp clamp)
{
    scalar dst[ColorMatrix::MATRIX_SIZE];
    m.GetArray(dst);
    filter_ = SkColorFilters::Matrix(dst, ConverToSkClamp(clamp));
}

void SkiaColorFilter::InitWithColorFloat(const float f[20], Clamp clamp)
{
    filter_ = SkColorFilters::Matrix(f, ConverToSkClamp(clamp));
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
    if (filter_ != nullptr && skColorFilterImpl != nullptr) {
        filter_ = filter_->makeComposed(skColorFilterImpl->GetColorFilter());
    }
}

void SkiaColorFilter::InitWithCompose(const float f1[MATRIX_SIZE], const float f2[MATRIX_SIZE], Clamp clamp)
{
    filter_ = SkColorFilters::Compose(SkColorFilters::Matrix(f1, ConverToSkClamp(clamp)),
        SkColorFilters::Matrix(f2, ConverToSkClamp(clamp)));
}

void SkiaColorFilter::InitWithOverDrawColor(const ColorQuad colors[OVER_DRAW_COLOR_NUM])
{
    filter_ = SkOverdrawColorFilter::MakeWithSkColors(colors);
}

void SkiaColorFilter::InitWithLighting(ColorQuad mul, ColorQuad add)
{
    filter_ = SkColorMatrixFilter::MakeLightingFilter(static_cast<SkColor>(mul), static_cast<SkColor>(add));
}

void SkiaColorFilter::InitWithLuma()
{
    filter_ = SkLumaColorFilter::Make();
}

sk_sp<SkColorFilter> SkiaColorFilter::GetColorFilter() const
{
    return filter_;
}

void SkiaColorFilter::SetColorFilter(const sk_sp<SkColorFilter>& filter)
{
    filter_ = filter;
}

std::shared_ptr<Data> SkiaColorFilter::Serialize() const
{
    if (filter_ == nullptr) {
        LOGD("SkiaColorFilter::Serialize, filter_ is nullptr!");
        return nullptr;
    }

    return SkiaHelper::FlattenableSerialize(filter_.get());
}

bool SkiaColorFilter::Deserialize(std::shared_ptr<Data> data)
{
    if (data == nullptr) {
        LOGD("SkiaColorFilter::Deserialize, data is invalid!");
        return false;
    }

    filter_ = SkiaHelper::FlattenableDeserialize<SkColorFilterBase>(data);
    return true;
}

bool SkiaColorFilter::AsAColorMatrix(scalar matrix[MATRIX_SIZE]) const
{
    if (filter_ == nullptr) {
        LOGD("SkiaColorFilter::AsAColorMatrix filter_ is nullptr!");
        return false;
    }
    return filter_->asAColorMatrix(matrix);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS