/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "effect/image_filter.h"

#include "impl_factory.h"

#include "impl_interface/image_filter_impl.h"
#include "utils/log.h"

#ifdef ROSEN_OHOS
#include "utils/object_helper.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {
ImageFilter::ImageFilter(FilterType t, scalar x, scalar y,
    std::shared_ptr<ImageFilter> input, const Rect& cropRect) noexcept : ImageFilter()
{
    type_ = t;
    impl_->InitWithOffset(x, y, input, cropRect);
}

ImageFilter::ImageFilter(FilterType t, scalar x, scalar y, TileMode mode, std::shared_ptr<ImageFilter> input,
    ImageBlurType blurType, const Rect& cropRect) noexcept
    : ImageFilter()
{
    type_ = t;
    impl_->InitWithBlur(x, y, mode, input, blurType, cropRect);
}

ImageFilter::ImageFilter(FilterType t, const ColorFilter& cf,
    std::shared_ptr<ImageFilter> input, const Rect& cropRect) noexcept
    : ImageFilter()
{
    type_ = t;
    impl_->InitWithColor(cf, input, cropRect);
}

ImageFilter::ImageFilter(FilterType t, const ColorFilter& cf, scalar x, scalar y,
    ImageBlurType blurType, const Rect& cropRect) noexcept
    : ImageFilter()
{
    type_ = t;
    impl_->InitWithColorBlur(cf, x, y, blurType, cropRect);
}

void ImageFilter::InitWithColorBlur(const ColorFilter& cf, scalar x, scalar y,
    ImageBlurType blurType, const Rect& cropRect)
{
    type_ = ImageFilter::FilterType::COLOR_FILTER;
    impl_->InitWithColorBlur(cf, x, y, blurType, cropRect);
}

ImageFilter::ImageFilter(FilterType t, const std::vector<scalar>& coefficients, bool enforcePMColor,
    std::shared_ptr<ImageFilter> background, std::shared_ptr<ImageFilter> foreground, const Rect& cropRect) noexcept
    :ImageFilter()
{
    type_ = t;
    impl_->InitWithArithmetic(coefficients, enforcePMColor, background, foreground, cropRect);
}

ImageFilter::ImageFilter(FilterType t, std::shared_ptr<ImageFilter> f1, std::shared_ptr<ImageFilter> f2) noexcept
    : ImageFilter()
{
    type_ = t;
    impl_->InitWithCompose(f1, f2);
}

ImageFilter::ImageFilter(FilterType t, float radius, const std::vector<std::pair<float, float>>& fractionStops,
    GradientDir direction, GradientBlurType blurType, std::shared_ptr<ImageFilter> input) noexcept
    : ImageFilter()
{
    type_ = t;
    impl_->InitWithGradientBlur(radius, fractionStops, direction, blurType, input);
}

ImageFilter::ImageFilter(FilterType t, const std::shared_ptr<Image>& image, const Rect& srcRect, const Rect& dstRect,
    const SamplingOptions& options) noexcept : ImageFilter()
{
    type_ = t;
    impl_->InitWithImage(image, srcRect, dstRect, options);
}

ImageFilter::ImageFilter(FilterType t) noexcept
    : type_(t), impl_(ImplFactory::CreateImageFilterImpl())
{}

ImageFilter::ImageFilter() noexcept
    : type_(ImageFilter::FilterType::NO_TYPE), impl_(ImplFactory::CreateImageFilterImpl())
{}

ImageFilter::FilterType ImageFilter::GetType() const
{
    return type_;
}

std::shared_ptr<ImageFilter> ImageFilter::CreateImageImageFilter(
    const std::shared_ptr<Image>& image, const Rect& srcRect, const Rect& dstRect, const SamplingOptions& options)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::IMAGE, image, srcRect, dstRect, options);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateBlurImageFilter(scalar sigmaX, scalar sigmaY, TileMode mode,
    std::shared_ptr<ImageFilter> input, ImageBlurType blurType, const Rect& cropRect)
{
    // Check if input filter is Lazy type, not supported for direct CreateBlurImageFilter
    if (input && input->IsLazy()) {
        LOGE("ImageFilter::CreateBlurImageFilter, input filter is Lazy type, not supported.");
        return nullptr;
    }

    return std::make_shared<ImageFilter>(ImageFilter::FilterType::BLUR, sigmaX, sigmaY,
        mode, input, blurType, cropRect);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateColorFilterImageFilter(
    const ColorFilter& cf, std::shared_ptr<ImageFilter> input, const Rect& cropRect)
{
    // Check if input filter is Lazy type, not supported for direct CreateColorFilterImageFilter
    if (input && input->IsLazy()) {
        LOGE("ImageFilter::CreateColorFilterImageFilter, input filter is Lazy type, not supported.");
        return nullptr;
    }

    return std::make_shared<ImageFilter>(ImageFilter::FilterType::COLOR_FILTER, cf, input, cropRect);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateColorBlurImageFilter(const ColorFilter& cf,
    scalar sigmaX, scalar sigmaY, ImageBlurType blurType, const Rect& cropRect)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::COLOR_FILTER, cf,
        sigmaX, sigmaY, blurType, cropRect);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateOffsetImageFilter(
    scalar dx, scalar dy, std::shared_ptr<ImageFilter> input, const Rect& cropRect)
{
    // Check if input filter is Lazy type, not supported for direct CreateOffsetImageFilter
    if (input && input->IsLazy()) {
        LOGE("ImageFilter::CreateOffsetImageFilter, input filter is Lazy type, not supported.");
        return nullptr;
    }

    return std::make_shared<ImageFilter>(ImageFilter::FilterType::OFFSET,
        dx, dy, input, cropRect);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateGradientBlurImageFilter(float radius,
    const std::vector<std::pair<float, float>>& fractionStops, GradientDir direction,
    GradientBlurType blurType, std::shared_ptr<ImageFilter> input)
{
    // Check if input filter is Lazy type, not supported for direct CreateGradientBlurImageFilter
    if (input && input->IsLazy()) {
        LOGE("ImageFilter::CreateGradientBlurImageFilter, input filter is Lazy type, not supported.");
        return nullptr;
    }

    return std::make_shared<ImageFilter>(
        ImageFilter::FilterType::GRADIENT_BLUR, radius, fractionStops, direction, blurType, input);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateArithmeticImageFilter(const std::vector<scalar>& coefficients,
    bool enforcePMColor, std::shared_ptr<ImageFilter> background,
    std::shared_ptr<ImageFilter> foreground, const Rect& cropRect)
{
    // Check if background filter is Lazy type, not supported for direct CreateArithmeticImageFilter
    if (background && background->IsLazy()) {
        LOGE("ImageFilter::CreateArithmeticImageFilter, background filter is Lazy type, not supported.");
        return nullptr;
    }

    // Check if foreground filter is Lazy type, not supported for direct CreateArithmeticImageFilter
    if (foreground && foreground->IsLazy()) {
        LOGE("ImageFilter::CreateArithmeticImageFilter, foreground filter is Lazy type, not supported.");
        return nullptr;
    }

    return std::make_shared<ImageFilter>(
        ImageFilter::FilterType::ARITHMETIC, coefficients, enforcePMColor, background, foreground, cropRect);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateComposeImageFilter(
    std::shared_ptr<ImageFilter> f1, std::shared_ptr<ImageFilter> f2)
{
    // Check if f1 filter is Lazy type, not supported for direct CreateComposeImageFilter
    if (f1 && f1->IsLazy()) {
        LOGE("ImageFilter::CreateComposeImageFilter, f1 filter is Lazy type, not supported.");
        return nullptr;
    }

    // Check if f2 filter is Lazy type, not supported for direct CreateComposeImageFilter
    if (f2 && f2->IsLazy()) {
        LOGE("ImageFilter::CreateComposeImageFilter, f2 filter is Lazy type, not supported.");
        return nullptr;
    }

    return std::make_shared<ImageFilter>(ImageFilter::FilterType::COMPOSE, f1, f2);
}

std::shared_ptr<Data> ImageFilter::Serialize() const
{
    return impl_->Serialize();
}

bool ImageFilter::Deserialize(std::shared_ptr<Data> data)
{
    return impl_->Deserialize(data);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateBlendImageFilter(BlendMode mode,
    std::shared_ptr<ImageFilter> background, std::shared_ptr<ImageFilter> foreground, const Rect& cropRect)
{
    // Check if background filter is Lazy type, not supported for direct CreateBlendImageFilter
    if (background && background->IsLazy()) {
        LOGE("ImageFilter::CreateBlendImageFilter, background filter is Lazy type, not supported.");
        return nullptr;
    }

    // Check if foreground filter is Lazy type, not supported for direct CreateBlendImageFilter
    if (foreground && foreground->IsLazy()) {
        LOGE("ImageFilter::CreateBlendImageFilter, foreground filter is Lazy type, not supported.");
        return nullptr;
    }

    return std::make_shared<ImageFilter>(ImageFilter::FilterType::BLEND, mode, background, foreground, cropRect);
}

ImageFilter::ImageFilter(FilterType t, BlendMode mode, std::shared_ptr<ImageFilter> background,
    std::shared_ptr<ImageFilter> foreground, const Rect& cropRect) noexcept
    : ImageFilter()
{
    type_ = t;
    impl_->InitWithBlend(mode, cropRect, background, foreground);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateShaderImageFilter(
    std::shared_ptr<ShaderEffect> shader, const Rect& cropRect)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::SHADER, shader, cropRect);
}

ImageFilter::ImageFilter(FilterType t, std::shared_ptr<ShaderEffect> shader, const Rect& cropRect) noexcept
    : ImageFilter()
{
    type_ = t;
    impl_->InitWithShader(shader, cropRect);
}

#ifdef ROSEN_OHOS
bool ImageFilter::Marshalling(Parcel& parcel)
{
    // Write type first
    if (!parcel.WriteInt32(static_cast<int32_t>(type_))) {
        LOGE("ImageFilter::Marshalling, failed to write type");
        return false;
    }

    // Use Serialize to convert to Data then serialize
    auto data = Serialize();

    // Write flag indicating whether data is valid
    bool hasValidData = (data != nullptr && data->GetSize() > 0);
    if (!parcel.WriteBool(hasValidData)) {
        LOGE("ImageFilter::Marshalling, failed to write hasData flag");
        return false;
    }

    // If data is null or empty (empty filter), just write the flag and return success
    // This prevents parcel failure when underlying filter creation failed
    if (!hasValidData) {
        LOGD("ImageFilter::Marshalling, Serialize returned null or empty data (empty filter), "
             "continuing with empty marker");
        return true;
    }

    // Use registered callback for Data marshalling
    auto callback = ObjectHelper::Instance().GetDataMarshallingCallback();
    if (!callback) {
        LOGE("ImageFilter::Marshalling, DataMarshallingCallback is not registered");
        return false;
    }
    if (!callback(parcel, data)) {
        LOGE("ImageFilter::Marshalling, DataMarshallingCallback failed");
        return false;
    }
    return true;
}

std::shared_ptr<ImageFilter> ImageFilter::Unmarshalling(Parcel& parcel, bool& isValid)
{
    // Read type first
    int32_t typeValue;
    if (!parcel.ReadInt32(typeValue)) {
        LOGE("ImageFilter::Unmarshalling, failed to read type");
        return nullptr;
    }

    // Validate type range (should be valid filter types, excluding NO_TYPE and LAZY_IMAGE_FILTER)
    if (typeValue < static_cast<int32_t>(FilterType::NO_TYPE) ||
        typeValue >= static_cast<int32_t>(FilterType::LAZY_IMAGE_FILTER)) {
        LOGE("ImageFilter::Unmarshalling, invalid type value: %{public}d", typeValue);
        return nullptr;
    }

    // Read hasData flag
    bool hasData;
    if (!parcel.ReadBool(hasData)) {
        LOGE("ImageFilter::Unmarshalling, failed to read hasData flag");
        return nullptr;
    }

    // If no data (empty filter), create an empty ImageFilter and return
    if (!hasData) {
        LOGD("ImageFilter::Unmarshalling, empty filter marker detected, creating empty ImageFilter");
        auto imageFilter = std::make_shared<ImageFilter>(static_cast<FilterType>(typeValue));
        return imageFilter;
    }

    // Use registered callback for Data unmarshalling
    auto callback = ObjectHelper::Instance().GetDataUnmarshallingCallback();
    if (!callback) {
        LOGE("ImageFilter::Unmarshalling, DataUnmarshallingCallback is not registered");
        return nullptr;
    }
    auto data = callback(parcel);
    if (!data) {
        LOGE("ImageFilter::Unmarshalling, DataUnmarshallingCallback failed");
        return nullptr;
    }

    // Create ImageFilter with correct type
    auto imageFilter = std::make_shared<ImageFilter>(static_cast<FilterType>(typeValue));
    if (!imageFilter->Deserialize(data)) {
        LOGE("ImageFilter::Unmarshalling, Deserialize failed");
        // For compatibility: mark as invalid but return object instead of nullptr
        isValid = false;
        return imageFilter;
    }
    return imageFilter;
}
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS