/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "effect/color_filter_image_filter_obj.h"
#include "effect/image_filter_lazy.h"
#include "utils/data.h"
#include "utils/log.h"

#ifdef ROSEN_OHOS
#include "utils/object_helper.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {

ColorFilterImageFilterObj::ColorFilterImageFilterObj()
    : ImageFilterObj(static_cast<int32_t>(ImageFilter::FilterType::COLOR_FILTER))
{
}

std::shared_ptr<ColorFilterImageFilterObj> ColorFilterImageFilterObj::CreateForUnmarshalling()
{
    return std::shared_ptr<ColorFilterImageFilterObj>(new ColorFilterImageFilterObj());
}

std::shared_ptr<ColorFilterImageFilterObj> ColorFilterImageFilterObj::Create(
    const std::shared_ptr<ColorFilter>& colorFilter, const std::shared_ptr<ImageFilter>& input, const Rect& cropRect)
{
    return std::shared_ptr<ColorFilterImageFilterObj>(new ColorFilterImageFilterObj(colorFilter, input, cropRect));
}

ColorFilterImageFilterObj::ColorFilterImageFilterObj(const std::shared_ptr<ColorFilter>& colorFilter,
    const std::shared_ptr<ImageFilter>& input, const Rect& cropRect)
    : ImageFilterObj(static_cast<int32_t>(ImageFilter::FilterType::COLOR_FILTER)),
      colorFilter_(colorFilter),
      input_(input),
      cropRect_(cropRect)
{
}

std::shared_ptr<void> ColorFilterImageFilterObj::GenerateBaseObject()
{
    if (!colorFilter_) {
        return nullptr;
    }

    // Handle lazy input if needed
    std::shared_ptr<ImageFilter> actualInput = input_;
    if (input_ && input_->IsLazy()) {
        auto lazyInput = std::static_pointer_cast<ImageFilterLazy>(input_);
        actualInput = lazyInput->Materialize();
    }

    auto colorFilterImageFilter = ImageFilter::CreateColorFilterImageFilter(*colorFilter_, actualInput, cropRect_);
    return std::static_pointer_cast<void>(colorFilterImageFilter);
}

#ifdef ROSEN_OHOS
bool ColorFilterImageFilterObj::Marshalling(Parcel& parcel)
{
    // Early validation: colorFilter_ must not be null
    if (!colorFilter_) {
        LOGE("ColorFilterImageFilterObj::Marshalling, colorFilter is null");
        return false;
    }

    // Serialize cropRect and ColorFilter (type and subType handled externally)
    bool ret = true;

    // Serialize cropRect
    ret &= parcel.WriteFloat(cropRect_.GetLeft());
    ret &= parcel.WriteFloat(cropRect_.GetTop());
    ret &= parcel.WriteFloat(cropRect_.GetRight());
    ret &= parcel.WriteFloat(cropRect_.GetBottom());
    if (!ret) {
        LOGE("ColorFilterImageFilterObj::Marshalling, failed to write basic data");
        return false;
    }

    // Use ColorFilter's new Marshalling method
    if (!colorFilter_->Marshalling(parcel)) {
        LOGE("ColorFilterImageFilterObj::Marshalling, failed to marshal colorFilter");
        return false;
    }

    // Serialize input ImageFilter
    if (!parcel.WriteBool(input_ != nullptr)) {
        LOGE("ColorFilterImageFilterObj::Marshalling, failed to write hasInput flag");
        return false;
    }

    if (!input_) {
        return true;
    }

    if (!parcel.WriteBool(input_->IsLazy())) {
        LOGE("ColorFilterImageFilterObj::Marshalling, failed to write isLazy flag");
        return false;
    }

    // Use polymorphic Marshalling - handles both Lazy and non-Lazy types
    if (!input_->Marshalling(parcel)) {
        LOGE("ColorFilterImageFilterObj::Marshalling, failed to marshal input filter");
        return false;
    }

    return true;
}

bool ColorFilterImageFilterObj::Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth)
{
    // Check maximum nesting depth to prevent stack overflow
    if (depth >= ObjectHelper::MAX_NESTING_DEPTH) {
        LOGE("ColorFilterImageFilterObj::Unmarshalling, depth %{public}d exceeds maximum limit %{public}d",
             depth, ObjectHelper::MAX_NESTING_DEPTH);
        return false;
    }

    // Deserialize cropRect
    float left;
    float top;
    float right;
    float bottom;
    if (!parcel.ReadFloat(left) || !parcel.ReadFloat(top) ||
        !parcel.ReadFloat(right) || !parcel.ReadFloat(bottom)) {
        LOGE("ColorFilterImageFilterObj::Unmarshalling, failed to read cropRect");
        return false;
    }
    cropRect_ = Rect(left, top, right, bottom);

    // Use ColorFilter's new Unmarshalling method
    colorFilter_ = ColorFilter::Unmarshalling(parcel, isValid);
    if (!colorFilter_) {
        LOGE("ColorFilterImageFilterObj::Unmarshalling, failed to unmarshal colorFilter");
        return false;
    }

    // Deserialize input ImageFilter
    bool hasInput;
    if (!parcel.ReadBool(hasInput)) {
        LOGE("ColorFilterImageFilterObj::Unmarshalling, failed to read hasInput flag");
        return false;
    }

    if (!hasInput) {
        return true;
    }

    bool isLazy;
    if (!parcel.ReadBool(isLazy)) {
        LOGE("ColorFilterImageFilterObj::Unmarshalling, failed to read isLazy flag");
        return false;
    }

    if (!isLazy) {
        input_ = ImageFilter::Unmarshalling(parcel, isValid);
    } else {
        input_ = ImageFilterLazy::Unmarshalling(parcel, isValid, depth);
    }
    if (!input_) {
        LOGE("ColorFilterImageFilterObj::Unmarshalling, failed to unmarshal input filter, lazy[%{public}d]", isLazy);
    }
    return input_ != nullptr;
}

// Register ColorFilterImageFilterObj to ObjectHelper using macro
OBJECT_UNMARSHALLING_REGISTER(ColorFilterImageFilterObj,
    static_cast<int32_t>(Drawing::Object::ObjectType::IMAGE_FILTER),
    static_cast<int32_t>(Drawing::ImageFilter::FilterType::COLOR_FILTER));

#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS