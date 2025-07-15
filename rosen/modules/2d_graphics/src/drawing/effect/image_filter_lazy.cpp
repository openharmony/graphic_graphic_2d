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

#include "effect/image_filter_lazy.h"

#include "effect/blur_image_filter_obj.h"
#include "effect/color_filter_image_filter_obj.h"
#include "effect/offset_image_filter_obj.h"
#include "effect/shader_image_filter_obj.h"
#include "utils/data.h"
#include "utils/log.h"

#ifdef ROSEN_OHOS
#include "utils/object_helper.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {

ImageFilterLazy::ImageFilterLazy(std::shared_ptr<ImageFilterObj> imageFilterObj) noexcept
    : ImageFilter(FilterType::LAZY_IMAGE_FILTER), imageFilterObj_(imageFilterObj)
{
}

std::shared_ptr<ImageFilterLazy> ImageFilterLazy::CreateBlur(scalar sigmaX, scalar sigmaY,
    TileMode tileMode, const std::shared_ptr<ImageFilter>& input, ImageBlurType blurType, const Rect& cropRect)
{
    auto blurImageFilterObj = BlurImageFilterObj::Create(sigmaX, sigmaY, tileMode, input, blurType, cropRect);
    if (!blurImageFilterObj) {
        LOGD("ImageFilterLazy::CreateBlur, failed to create BlurImageFilterObj");
        return nullptr;
    }
    return CreateFromImageFilterObj(blurImageFilterObj);
}

std::shared_ptr<ImageFilterLazy> ImageFilterLazy::CreateColorFilter(const std::shared_ptr<ColorFilter>& colorFilter,
    const std::shared_ptr<ImageFilter>& input, const Rect& cropRect)
{
    if (!colorFilter) {
        LOGD("ImageFilterLazy::CreateColorFilter, colorFilter is null");
        return nullptr;
    }

    auto colorFilterImageFilterObj = ColorFilterImageFilterObj::Create(colorFilter, input, cropRect);
    if (!colorFilterImageFilterObj) {
        LOGD("ImageFilterLazy::CreateColorFilter, failed to create ColorFilterImageFilterObj");
        return nullptr;
    }
    return CreateFromImageFilterObj(colorFilterImageFilterObj);
}

std::shared_ptr<ImageFilterLazy> ImageFilterLazy::CreateOffset(scalar dx, scalar dy,
    const std::shared_ptr<ImageFilter>& input, const Rect& cropRect)
{
    auto offsetImageFilterObj = OffsetImageFilterObj::Create(dx, dy, input, cropRect);
    if (!offsetImageFilterObj) {
        LOGD("ImageFilterLazy::CreateOffset, failed to create OffsetImageFilterObj");
        return nullptr;
    }
    return CreateFromImageFilterObj(offsetImageFilterObj);
}

std::shared_ptr<ImageFilterLazy> ImageFilterLazy::CreateShader(const std::shared_ptr<ShaderEffect>& shader,
    const Rect& cropRect)
{
    if (!shader) {
        LOGD("ImageFilterLazy::CreateShader, shader is null");
        return nullptr;
    }

    auto shaderImageFilterObj = ShaderImageFilterObj::Create(shader, cropRect);
    if (!shaderImageFilterObj) {
        LOGD("ImageFilterLazy::CreateShader, failed to create ShaderImageFilterObj");
        return nullptr;
    }
    return CreateFromImageFilterObj(shaderImageFilterObj);
}

std::shared_ptr<ImageFilterLazy> ImageFilterLazy::CreateFromImageFilterObj(std::shared_ptr<ImageFilterObj> object)
{
    if (!object) {
        LOGD("ImageFilterLazy::CreateFromImageFilterObj, object is null");
        return nullptr;
    }

    // Validate object type
    if (object->GetType() != static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER)) {
        LOGE("ImageFilterLazy::CreateFromImageFilterObj, invalid object type: %{public}d, expected: %{public}d",
            object->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
        return nullptr;
    }

    return std::shared_ptr<ImageFilterLazy>(new ImageFilterLazy(object));
}

std::shared_ptr<ImageFilter> ImageFilterLazy::Materialize()
{
    if (imageFilterCache_) {
        return imageFilterCache_;
    }

    if (!imageFilterObj_) {
        return nullptr;
    }

    auto baseObject = imageFilterObj_->GenerateBaseObject();
    if (!baseObject) {
        return nullptr;
    }

    imageFilterCache_ = std::static_pointer_cast<ImageFilter>(baseObject);
    return imageFilterCache_;
}

std::shared_ptr<Data> ImageFilterLazy::Serialize() const
{
    LOGE("ImageFilterLazy::Serialize, should use Marshalling() instead. Check IsLazy() first.");
    LOGD("ImageFilterObj type: %{public}d, subType: %{public}d",
         imageFilterObj_ ? imageFilterObj_->GetType() : -1,
         imageFilterObj_ ? imageFilterObj_->GetSubType() : -1);
    return nullptr;
}

bool ImageFilterLazy::Deserialize(std::shared_ptr<Data> data)
{
    LOGE("ImageFilterLazy::Deserialize, should use Unmarshalling() instead. Check IsLazy() first.");
    LOGD("Attempted to deserialize %zu bytes into Lazy object", data ? data->GetSize() : 0);
    return false;
}

#ifdef ROSEN_OHOS
bool ImageFilterLazy::Marshalling(Parcel& parcel)
{
    if (!imageFilterObj_) {
        return false;
    }

    // Write type and subType from internal object
    if (!parcel.WriteInt32(imageFilterObj_->GetType()) ||
        !parcel.WriteInt32(imageFilterObj_->GetSubType())) {
        LOGE("ImageFilterLazy::Marshalling, failed to write type and subType");
        return false;
    }

    // Use ImageFilterObj's Marshalling for internal data
    return imageFilterObj_->Marshalling(parcel);
}

std::shared_ptr<ImageFilterLazy> ImageFilterLazy::Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth)
{
    // Read type and subType with safety checks
    int32_t type;
    if (!parcel.ReadInt32(type)) {
        LOGE("ImageFilterLazy::Unmarshalling, failed to read type");
        return nullptr;
    }
    int32_t subType;
    if (!parcel.ReadInt32(subType)) {
        LOGE("ImageFilterLazy::Unmarshalling, failed to read subType");
        return nullptr;
    }

    // Create corresponding ImageFilterObj (depth check is done inside func)
    auto func = ObjectHelper::Instance().GetFunc(type, subType);
    auto obj = func ? func(parcel, isValid, depth + 1) : nullptr;
    if (!obj) {
        LOGE("ImageFilterLazy::Unmarshalling, failed to create ImageFilterObj for type=%{public}d, subType=%{public}d",
             type, subType);
        return nullptr;
    }

    // Create ImageFilterLazy to wrap ExtendObject
    auto imageFilterObj = std::static_pointer_cast<ImageFilterObj>(obj);
    auto lazyFilter = CreateFromImageFilterObj(imageFilterObj);
    if (!lazyFilter) {
        LOGE("ImageFilterLazy::Unmarshalling, failed to create ImageFilterLazy");
        return nullptr;
    }

    return lazyFilter;
}
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS