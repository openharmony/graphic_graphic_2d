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

#include "effect/blur_image_filter_obj.h"
#include "effect/image_filter_lazy.h"
#include "utils/data.h"
#include "utils/log.h"

#ifdef ROSEN_OHOS
#include "utils/object_helper.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {

BlurImageFilterObj::BlurImageFilterObj()
    : ImageFilterObj(static_cast<int32_t>(ImageFilter::FilterType::BLUR)),
      sigmaX_(0.0f),
      sigmaY_(0.0f),
      tileMode_(TileMode::DECAL),
      input_(nullptr),
      blurType_(ImageBlurType::GAUSS),
      cropRect_(noCropRect)
{
}

std::shared_ptr<BlurImageFilterObj> BlurImageFilterObj::CreateForUnmarshalling()
{
    return std::shared_ptr<BlurImageFilterObj>(new BlurImageFilterObj());
}

std::shared_ptr<BlurImageFilterObj> BlurImageFilterObj::Create(scalar sigmaX, scalar sigmaY,
    TileMode tileMode, const std::shared_ptr<ImageFilter>& input,
    ImageBlurType blurType, const Rect& cropRect)
{
    return std::shared_ptr<BlurImageFilterObj>(new BlurImageFilterObj(sigmaX, sigmaY, tileMode, input,
        blurType, cropRect));
}

BlurImageFilterObj::BlurImageFilterObj(scalar sigmaX, scalar sigmaY,
    TileMode tileMode, const std::shared_ptr<ImageFilter>& input, ImageBlurType blurType, const Rect& cropRect)
    : ImageFilterObj(static_cast<int32_t>(ImageFilter::FilterType::BLUR)),
      sigmaX_(sigmaX),
      sigmaY_(sigmaY),
      tileMode_(tileMode),
      input_(input),
      blurType_(blurType),
      cropRect_(cropRect)
{
}

std::shared_ptr<void> BlurImageFilterObj::GenerateBaseObject()
{
    // Handle lazy input if needed
    std::shared_ptr<ImageFilter> actualInput = input_;
    if (input_ && input_->IsLazy()) {
        auto lazyInput = std::static_pointer_cast<ImageFilterLazy>(input_);
        actualInput = lazyInput->Materialize();
    }

    auto blurImageFilter = ImageFilter::CreateBlurImageFilter(sigmaX_, sigmaY_, tileMode_, actualInput,
        blurType_, cropRect_);
    return std::static_pointer_cast<void>(blurImageFilter);
}

#ifdef ROSEN_OHOS
bool BlurImageFilterObj::Marshalling(Parcel& parcel)
{
    // Serialize blur parameters (type and subType handled externally)
    bool ret = true;
    ret &= parcel.WriteFloat(sigmaX_);
    ret &= parcel.WriteFloat(sigmaY_);
    ret &= parcel.WriteInt32(static_cast<int32_t>(tileMode_));
    ret &= parcel.WriteInt32(static_cast<int32_t>(blurType_));
    ret &= parcel.WriteFloat(cropRect_.GetLeft());
    ret &= parcel.WriteFloat(cropRect_.GetTop());
    ret &= parcel.WriteFloat(cropRect_.GetRight());
    ret &= parcel.WriteFloat(cropRect_.GetBottom());
    if (!ret) {
        LOGE("BlurImageFilterObj::Marshalling, failed to write basic data");
        return false;
    }

    // Serialize input ImageFilter
    if (!parcel.WriteBool(input_ != nullptr)) {
        LOGE("BlurImageFilterObj::Marshalling, failed to write hasInput flag");
        return false;
    }

    if (!input_) {
        return true;
    }

    if (!parcel.WriteBool(input_->IsLazy())) {
        LOGE("BlurImageFilterObj::Marshalling, failed to write isLazy flag");
        return false;
    }

    // Use polymorphic Marshalling - handles both Lazy and non-Lazy types
    return input_->Marshalling(parcel);
}

bool BlurImageFilterObj::Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth)
{
    // Check maximum nesting depth to prevent stack overflow
    if (depth >= ObjectHelper::MAX_NESTING_DEPTH) {
        LOGE("BlurImageFilterObj::Unmarshalling, depth %{public}d exceeds maximum limit %{public}d",
             depth, ObjectHelper::MAX_NESTING_DEPTH);
        return false;
    }

    // Deserialize blur parameters
    if (!parcel.ReadFloat(sigmaX_)) {
        LOGE("BlurImageFilterObj::Unmarshalling, failed to read sigmaX");
        return false;
    }
    if (!parcel.ReadFloat(sigmaY_)) {
        LOGE("BlurImageFilterObj::Unmarshalling, failed to read sigmaY");
        return false;
    }

    int32_t tileModeValue;
    if (!parcel.ReadInt32(tileModeValue)) {
        LOGE("BlurImageFilterObj::Unmarshalling, failed to read tileMode");
        return false;
    }
    tileMode_ = static_cast<TileMode>(tileModeValue);

    int32_t blurTypeValue;
    if (!parcel.ReadInt32(blurTypeValue)) {
        LOGE("BlurImageFilterObj::Unmarshalling, failed to read blurType");
        return false;
    }
    blurType_ = static_cast<ImageBlurType>(blurTypeValue);

    float left;
    float top;
    float right;
    float bottom;
    if (!parcel.ReadFloat(left) || !parcel.ReadFloat(top) ||
        !parcel.ReadFloat(right) || !parcel.ReadFloat(bottom)) {
        LOGE("BlurImageFilterObj::Unmarshalling, failed to read cropRect");
        return false;
    }
    cropRect_ = Rect(left, top, right, bottom);

    // Deserialize input ImageFilter
    bool hasInput;
    if (!parcel.ReadBool(hasInput)) {
        LOGE("BlurImageFilterObj::Unmarshalling, failed to read hasInput flag");
        return false;
    }

    if (!hasInput) {
        return true;
    }

    bool isLazy;
    if (!parcel.ReadBool(isLazy)) {
        LOGE("BlurImageFilterObj::Unmarshalling, failed to read isLazy flag");
        return false;
    }

    if (!isLazy) {
        input_ = ImageFilter::Unmarshalling(parcel, isValid);
    } else {
        input_ = ImageFilterLazy::Unmarshalling(parcel, isValid, depth);
    }
    if (!input_) {
        LOGE("BlurImageFilterObj::Unmarshalling, failed to unmarshal input filter, lazy[%{public}d]", isLazy);
    }
    return input_ != nullptr;
}

// Register BlurImageFilterObj to ObjectHelper using macro
OBJECT_UNMARSHALLING_REGISTER(BlurImageFilterObj,
    static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER),
    static_cast<int32_t>(ImageFilter::FilterType::BLUR));

#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS