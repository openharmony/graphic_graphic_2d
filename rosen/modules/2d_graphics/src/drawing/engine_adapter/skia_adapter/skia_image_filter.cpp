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

#include "skia_image_filter.h"
#include "skia_helper.h"

#include "include/effects/SkImageFilters.h"
#include "include/core/SkTileMode.h"
#include "src/core/SkImageFilter_Base.h"

#if !defined(USE_CANVASKIT0310_SKIA) && !defined(NEW_SKIA)
#include "include/effects/SkBlurImageFilter.h"
#endif

#include "skia_color_filter.h"

#include "effect/color_filter.h"
#include "effect/image_filter.h"
#include "utils/data.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static constexpr size_t numberOfCoefficients = 4;

SkiaImageFilter::SkiaImageFilter() noexcept : filter_(nullptr) {}

static inline SkTileMode ConvertToSkTileMode(const TileMode& mode)
{
    switch (mode) {
        case TileMode::CLAMP:
            return SkTileMode::kClamp;
        case TileMode::REPEAT:
            return SkTileMode::kRepeat;
        case TileMode::MIRROR:
            return SkTileMode::kMirror;
        case TileMode::DECAL:
            return SkTileMode::kDecal;
        default:
            return SkTileMode::kClamp;
    }
}

void SkiaImageFilter::InitWithBlur(scalar sigmaX, scalar sigmaY, TileMode mode, const std::shared_ptr<ImageFilter> f,
    ImageBlurType blurType)
{
    // SK only support gauss, ignore the input blurType
    sk_sp<SkImageFilter> input = nullptr;
    if (f != nullptr && f->GetImpl<SkiaImageFilter>() != nullptr) {
        input = f->GetImpl<SkiaImageFilter>()->GetImageFilter();
    }
#if defined(USE_CANVASKIT0310_SKIA) || defined(NEW_SKIA)
    filter_ = SkImageFilters::Blur(sigmaX, sigmaY, ConvertToSkTileMode(mode), input);
#else
    filter_ = SkBlurImageFilter::Make(sigmaX, sigmaY, ConvertToSkTileMode(mode), input);
#endif
}

void SkiaImageFilter::InitWithColor(const ColorFilter& colorFilter, const std::shared_ptr<ImageFilter> f)
{
    sk_sp<SkImageFilter> input = nullptr;
    if (f != nullptr && f->GetImpl<SkiaImageFilter>() != nullptr) {
        input = f->GetImpl<SkiaImageFilter>()->GetImageFilter();
    }
    auto skColorFilterImpl = colorFilter.GetImpl<SkiaColorFilter>();
    if (skColorFilterImpl != nullptr) {
        filter_ = SkImageFilters::ColorFilter(skColorFilterImpl->GetColorFilter(), input);
    }
}

void SkiaImageFilter::InitWithOffset(scalar dx, scalar dy, const std::shared_ptr<ImageFilter> f)
{
    sk_sp<SkImageFilter> input = nullptr;
    if (f != nullptr && f->GetImpl<SkiaImageFilter>() != nullptr) {
        input = f->GetImpl<SkiaImageFilter>()->GetImageFilter();
    }
    filter_ = SkImageFilters::Offset(dx, dy, input);
}

void SkiaImageFilter::InitWithColorBlur(const ColorFilter& colorFilter, scalar sigmaX, scalar sigmaY,
    ImageBlurType blurType)
{
    // SK only support gauss, ignore the input blurType
    auto skColorFilterImpl = colorFilter.GetImpl<SkiaColorFilter>();
    filter_ = SkImageFilters::ColorFilter(
        skColorFilterImpl->GetColorFilter(), SkImageFilters::Blur(sigmaX, sigmaY, SkTileMode::kClamp, nullptr));
}

void SkiaImageFilter::InitWithArithmetic(const std::vector<scalar>& coefficients,
    bool enforcePMColor, const std::shared_ptr<ImageFilter> f1, const std::shared_ptr<ImageFilter> f2)
{
    if (coefficients.size() != numberOfCoefficients) {
        LOGD("SkiaImageFilter::InitWithArithmetic: the number of coefficients must be 4");
        return;
    }

    sk_sp<SkImageFilter> background = nullptr;
    sk_sp<SkImageFilter> foreground = nullptr;
    if (f1 != nullptr && f1->GetImpl<SkiaImageFilter>() != nullptr) {
        background = f1->GetImpl<SkiaImageFilter>()->GetImageFilter();
    }
    if (f2 != nullptr && f2->GetImpl<SkiaImageFilter>() != nullptr) {
        foreground = f2->GetImpl<SkiaImageFilter>()->GetImageFilter();
    }
    filter_ = SkImageFilters::Arithmetic(
        coefficients[0], coefficients[1], coefficients[2], coefficients[3], enforcePMColor, background, foreground);
}

void SkiaImageFilter::InitWithCompose(const std::shared_ptr<ImageFilter> f1, const std::shared_ptr<ImageFilter> f2)
{
    sk_sp<SkImageFilter> outer = nullptr;
    sk_sp<SkImageFilter> inner = nullptr;
    if (f1 != nullptr && f1->GetImpl<SkiaImageFilter>() != nullptr) {
        outer = f1->GetImpl<SkiaImageFilter>()->GetImageFilter();
    }
    if (f2 != nullptr && f2->GetImpl<SkiaImageFilter>() != nullptr) {
        inner = f2->GetImpl<SkiaImageFilter>()->GetImageFilter();
    }
    filter_ = SkImageFilters::Compose(outer, inner);
}

void SkiaImageFilter::InitWithGradientBlur(float radius,
    const std::vector<std::pair<float, float>>& fractionStops, GradientDir direction,
    GradientBlurType blurType, const std::shared_ptr<ImageFilter> f)
{
    return;
}

sk_sp<SkImageFilter> SkiaImageFilter::GetImageFilter() const
{
    return filter_;
}

void SkiaImageFilter::SetSkImageFilter(const sk_sp<SkImageFilter>& filter)
{
    filter_ = filter;
}

std::shared_ptr<Data> SkiaImageFilter::Serialize() const
{
    if (filter_ == nullptr) {
        return nullptr;
    }

    SkBinaryWriteBuffer writer;
    writer.writeFlattenable(filter_.get());
    size_t length = writer.bytesWritten();
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->BuildUninitialized(length);
    writer.writeToMemory(data->WritableData());
    return data;
}

bool SkiaImageFilter::Deserialize(std::shared_ptr<Data> data)
{
    if (data == nullptr) {
        LOGD("SkiaImageFilter::Deserialize, data is invalid!");
        return false;
    }

    SkReadBuffer reader(data->GetData(), data->GetSize());
    filter_ = SkiaHelper::FlattenableDeserialize<SkImageFilter_Base>(data);
    return true;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
