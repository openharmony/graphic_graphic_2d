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

#include "image/image.h"
#include "include/effects/SkImageFilters.h"
#include "include/core/SkTileMode.h"
#include "src/core/SkImageFilter_Base.h"

#include "skia_color_filter.h"
#include "skia_image.h"
#include "skia_shader_effect.h"

#include "effect/color_filter.h"
#include "effect/image_filter.h"
#include "utils/data.h"
#include "utils/log.h"
#include "skia_convert_utils.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static constexpr size_t NUMBER_OF_COEFFICIENTS = 4;

SkiaImageFilter::SkiaImageFilter() noexcept : filter_(nullptr) {}

#ifdef USE_M133_SKIA
static bool IsNoCropRect(const Rect& rect)
{
    return rect.GetLeft() == -std::numeric_limits<scalar>::infinity() &&
        rect.GetTop() == -std::numeric_limits<scalar>::infinity() &&
        rect.GetRight() == std::numeric_limits<scalar>::infinity() &&
        rect.GetBottom() == std::numeric_limits<scalar>::infinity();
}
#endif

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
    ImageBlurType blurType, const Rect& cropRect)
{
    // SK only support gauss, ignore the input blurType
    sk_sp<SkImageFilter> input = nullptr;
    if (f != nullptr && f->GetImpl<SkiaImageFilter>() != nullptr) {
        input = f->GetImpl<SkiaImageFilter>()->GetImageFilter();
    }
#ifdef USE_M133_SKIA
    SkImageFilters::CropRect skCropRect;
    if (!IsNoCropRect(cropRect)) {
        skCropRect = SkRect::MakeLTRB(cropRect.left_, cropRect.top_, cropRect.right_, cropRect.bottom_);
    }
#else
    SkRect skiaRect = {cropRect.left_, cropRect.top_, cropRect.right_, cropRect.bottom_};
    SkImageFilters::CropRect skCropRect(skiaRect);
#endif
    filter_ = SkImageFilters::Blur(sigmaX, sigmaY, ConvertToSkTileMode(mode), input, skCropRect);
}

void SkiaImageFilter::InitWithColor(const ColorFilter& colorFilter,
    const std::shared_ptr<ImageFilter> f, const Rect& cropRect)
{
    sk_sp<SkImageFilter> input = nullptr;
    if (f != nullptr && f->GetImpl<SkiaImageFilter>() != nullptr) {
        input = f->GetImpl<SkiaImageFilter>()->GetImageFilter();
    }
#ifdef USE_M133_SKIA
    SkImageFilters::CropRect skCropRect;
    if (!IsNoCropRect(cropRect)) {
        skCropRect = SkRect::MakeLTRB(cropRect.left_, cropRect.top_, cropRect.right_, cropRect.bottom_);
    }
#else
    SkRect skiaRect = {cropRect.left_, cropRect.top_, cropRect.right_, cropRect.bottom_};
    SkImageFilters::CropRect skCropRect(skiaRect);
#endif
    auto skColorFilterImpl = colorFilter.GetImpl<SkiaColorFilter>();
    if (skColorFilterImpl != nullptr) {
        filter_ = SkImageFilters::ColorFilter(skColorFilterImpl->GetColorFilter(), input, skCropRect);
    }
}

void SkiaImageFilter::InitWithOffset(scalar dx, scalar dy,
    const std::shared_ptr<ImageFilter> f, const Rect& cropRect)
{
    sk_sp<SkImageFilter> input = nullptr;
    if (f != nullptr && f->GetImpl<SkiaImageFilter>() != nullptr) {
        input = f->GetImpl<SkiaImageFilter>()->GetImageFilter();
    }
#ifdef USE_M133_SKIA
    SkImageFilters::CropRect skCropRect;
    if (!IsNoCropRect(cropRect)) {
        skCropRect = SkRect::MakeLTRB(cropRect.left_, cropRect.top_, cropRect.right_, cropRect.bottom_);
    }
#else
    SkRect skiaRect = {cropRect.left_, cropRect.top_, cropRect.right_, cropRect.bottom_};
    SkImageFilters::CropRect skCropRect(skiaRect);
#endif
    filter_ = SkImageFilters::Offset(dx, dy, input, skCropRect);
}

void SkiaImageFilter::InitWithColorBlur(const ColorFilter& colorFilter, scalar sigmaX, scalar sigmaY,
    ImageBlurType blurType, const Rect& cropRect)
{
    // SK only support gauss, ignore the input blurType
    auto skColorFilterImpl = colorFilter.GetImpl<SkiaColorFilter>();
#ifdef USE_M133_SKIA
    SkImageFilters::CropRect skCropRect;
    if (!IsNoCropRect(cropRect)) {
        skCropRect = SkRect::MakeLTRB(cropRect.left_, cropRect.top_, cropRect.right_, cropRect.bottom_);
    }
#else
    SkRect skiaRect = {cropRect.left_, cropRect.top_, cropRect.right_, cropRect.bottom_};
    SkImageFilters::CropRect skCropRect(skiaRect);
#endif
    filter_ = SkImageFilters::ColorFilter(
        skColorFilterImpl->GetColorFilter(),
        SkImageFilters::Blur(sigmaX, sigmaY, SkTileMode::kClamp, nullptr), skCropRect);
}

void SkiaImageFilter::InitWithArithmetic(const std::vector<scalar>& coefficients,
    bool enforcePMColor, const std::shared_ptr<ImageFilter> f1,
    const std::shared_ptr<ImageFilter> f2, const Rect& cropRect)
{
    if (coefficients.size() != NUMBER_OF_COEFFICIENTS) {
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
#ifdef USE_M133_SKIA
    SkImageFilters::CropRect skCropRect;
    if (!IsNoCropRect(cropRect)) {
        skCropRect = SkRect::MakeLTRB(cropRect.left_, cropRect.top_, cropRect.right_, cropRect.bottom_);
    }
#else
    SkRect skiaRect = {cropRect.left_, cropRect.top_, cropRect.right_, cropRect.bottom_};
    SkImageFilters::CropRect skCropRect(skiaRect);
#endif
    filter_ = SkImageFilters::Arithmetic(coefficients[0],
        coefficients[1], coefficients[2], coefficients[3], // 0 1 2 3 used to combine the foreground and background.
        enforcePMColor, background, foreground, skCropRect);
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

void SkiaImageFilter::InitWithImage(const std::shared_ptr<Image>& image, const Rect& srcRect, const Rect& dstRect,
    const SamplingOptions& options)
{
    if (image == nullptr) {
        LOGD("SkiaImageFilter::InitWithImage: image is nullptr!");
        return;
    }
    SkRect skSrcRect = SkRect::MakeLTRB(srcRect.GetLeft(), srcRect.GetTop(), srcRect.GetRight(), srcRect.GetBottom());
    SkRect skDstRect = SkRect::MakeLTRB(dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(), dstRect.GetBottom());
    auto imageImpl = image->GetImpl<SkiaImage>();
    if (imageImpl == nullptr) {
        LOGD("SkiaImageFilter::InitWithImage: imageImpl is nullptr!");
        return;
    }
    SkSamplingOptions samplingOptions(static_cast<SkFilterMode>(options.GetFilterMode()),
        static_cast<SkMipmapMode>(options.GetMipmapMode()));
    filter_ = SkImageFilters::Image(imageImpl->GetImage(), skSrcRect, skDstRect, samplingOptions);
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

#ifdef USE_M133_SKIA
    SkBinaryWriteBuffer writer({});
#else
    SkBinaryWriteBuffer writer;
#endif
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

void SkiaImageFilter::InitWithBlend(BlendMode mode, const Rect& cropRect,
    std::shared_ptr<ImageFilter> background, std::shared_ptr<ImageFilter> foreground)
{
    sk_sp<SkImageFilter> outer = nullptr;
    sk_sp<SkImageFilter> inner = nullptr;
    if (background != nullptr && background->GetImpl<SkiaImageFilter>() != nullptr) {
        outer = background->GetImpl<SkiaImageFilter>()->GetImageFilter();
    }
    if (foreground != nullptr && foreground->GetImpl<SkiaImageFilter>() != nullptr) {
        inner = foreground->GetImpl<SkiaImageFilter>()->GetImageFilter();
    }
#ifdef USE_M133_SKIA
    SkImageFilters::CropRect skCropRect;
    if (!IsNoCropRect(cropRect)) {
        skCropRect = SkRect::MakeLTRB(cropRect.left_, cropRect.top_, cropRect.right_, cropRect.bottom_);
    }
#else
    SkRect skiaRect = {cropRect.left_, cropRect.top_, cropRect.right_, cropRect.bottom_};
    SkImageFilters::CropRect skCropRect(skiaRect);
#endif
    filter_ = SkImageFilters::Blend(static_cast<SkBlendMode>(mode), outer, inner, skCropRect);
}

void SkiaImageFilter::InitWithShader(std::shared_ptr<ShaderEffect> shader, const Rect& cropRect)
{
    sk_sp<SkShader> skShader = nullptr;
    if (shader != nullptr && shader->GetImpl<SkiaShaderEffect>() != nullptr) {
        skShader = shader->GetImpl<SkiaShaderEffect>()->GetShader();
    }
#ifdef USE_M133_SKIA
    SkImageFilters::CropRect skCropRect;
    if (!IsNoCropRect(cropRect)) {
        skCropRect = SkRect::MakeLTRB(cropRect.left_, cropRect.top_, cropRect.right_, cropRect.bottom_);
    }
#else
    SkRect skiaRect = {cropRect.left_, cropRect.top_, cropRect.right_, cropRect.bottom_};
    SkImageFilters::CropRect skCropRect(skiaRect);
#endif
    filter_ = SkImageFilters::Shader(skShader, skCropRect);
}

void SkiaImageFilter::InitWithHDSample(
    const std::shared_ptr<Image>& image, const Rect& src, const Rect& dst, const HDSampleInfo& info)
{
    SamplingOptions options(FilterMode::LINEAR);
    InitWithImage(image, src, dst, options);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
