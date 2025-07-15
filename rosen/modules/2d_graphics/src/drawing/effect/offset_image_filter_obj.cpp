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

#include "effect/offset_image_filter_obj.h"
#include "effect/image_filter_lazy.h"
#include "utils/data.h"
#include "utils/log.h"

#ifdef ROSEN_OHOS
#include "utils/object_helper.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {

OffsetImageFilterObj::OffsetImageFilterObj()
    : ImageFilterObj(static_cast<int32_t>(ImageFilter::FilterType::OFFSET))
{
}

std::shared_ptr<OffsetImageFilterObj> OffsetImageFilterObj::CreateForUnmarshalling()
{
    return std::shared_ptr<OffsetImageFilterObj>(new OffsetImageFilterObj());
}

std::shared_ptr<OffsetImageFilterObj> OffsetImageFilterObj::Create(scalar dx, scalar dy,
    const std::shared_ptr<ImageFilter>& input, const Rect& cropRect)
{
    return std::shared_ptr<OffsetImageFilterObj>(new OffsetImageFilterObj(dx, dy, input, cropRect));
}

OffsetImageFilterObj::OffsetImageFilterObj(scalar dx, scalar dy,
    const std::shared_ptr<ImageFilter>& input, const Rect& cropRect)
    : ImageFilterObj(static_cast<int32_t>(ImageFilter::FilterType::OFFSET)),
      dx_(dx),
      dy_(dy),
      input_(input),
      cropRect_(cropRect)
{
}

std::shared_ptr<void> OffsetImageFilterObj::GenerateBaseObject()
{
    // Handle lazy input if needed
    std::shared_ptr<ImageFilter> actualInput = input_;
    if (input_ && input_->IsLazy()) {
        auto lazyInput = std::static_pointer_cast<ImageFilterLazy>(input_);
        actualInput = lazyInput->Materialize();
    }

    auto offsetImageFilter = ImageFilter::CreateOffsetImageFilter(dx_, dy_, actualInput, cropRect_);
    return std::static_pointer_cast<void>(offsetImageFilter);
}

#ifdef ROSEN_OHOS
bool OffsetImageFilterObj::Marshalling(Parcel& parcel)
{
    // Serialize offset parameters and cropRect (type and subType handled externally)
    bool ret = parcel.WriteFloat(dx_);
    ret &= parcel.WriteFloat(dy_);
    ret &= parcel.WriteFloat(cropRect_.GetLeft());
    ret &= parcel.WriteFloat(cropRect_.GetTop());
    ret &= parcel.WriteFloat(cropRect_.GetRight());
    ret &= parcel.WriteFloat(cropRect_.GetBottom());
    ret &= parcel.WriteBool(input_ != nullptr);
    if (!ret) {
        LOGE("OffsetImageFilterObj::Marshalling, failed to write basic data");
        return false;
    }

    if (!input_) {
        return true;
    }

    if (!parcel.WriteBool(input_->IsLazy())) {
        LOGE("OffsetImageFilterObj::Marshalling, failed to write isLazy flag");
        return false;
    }

    // Use polymorphic Marshalling - handles both Lazy and non-Lazy types
    return input_->Marshalling(parcel);
}

bool OffsetImageFilterObj::Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth)
{
    // Check maximum nesting depth to prevent stack overflow
    if (depth >= ObjectHelper::MAX_NESTING_DEPTH) {
        LOGE("OffsetImageFilterObj::Unmarshalling, depth %{public}d exceeds maximum limit %{public}d",
             depth, ObjectHelper::MAX_NESTING_DEPTH);
        return false;
    }

    // Deserialize offset parameters
    if (!parcel.ReadFloat(dx_)) {
        LOGE("OffsetImageFilterObj::Unmarshalling, failed to read dx");
        return false;
    }
    if (!parcel.ReadFloat(dy_)) {
        LOGE("OffsetImageFilterObj::Unmarshalling, failed to read dy");
        return false;
    }

    // Deserialize cropRect
    float left;
    float top;
    float right;
    float bottom;
    if (!parcel.ReadFloat(left) || !parcel.ReadFloat(top) ||
        !parcel.ReadFloat(right) || !parcel.ReadFloat(bottom)) {
        LOGE("OffsetImageFilterObj::Unmarshalling, failed to read cropRect");
        return false;
    }
    cropRect_ = Rect(left, top, right, bottom);

    // Deserialize input ImageFilter
    bool hasInput;
    if (!parcel.ReadBool(hasInput)) {
        LOGE("OffsetImageFilterObj::Unmarshalling, failed to read hasInput flag");
        return false;
    }

    if (!hasInput) {
        return true;
    }

    bool isLazy;
    if (!parcel.ReadBool(isLazy)) {
        LOGE("OffsetImageFilterObj::Unmarshalling, failed to read isLazy flag");
        return false;
    }

    if (!isLazy) {
        input_ = ImageFilter::Unmarshalling(parcel, isValid);
    } else {
        input_ = ImageFilterLazy::Unmarshalling(parcel, isValid, depth);
    }
    if (!input_) {
        LOGE("OffsetImageFilterObj::Unmarshalling, failed to unmarshal input filter, lazy[%{public}d]", isLazy);
    }
    return input_ != nullptr;
}

// Register OffsetImageFilterObj to ObjectHelper using macro
OBJECT_UNMARSHALLING_REGISTER(OffsetImageFilterObj,
    static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER),
    static_cast<int32_t>(ImageFilter::FilterType::OFFSET));

#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS