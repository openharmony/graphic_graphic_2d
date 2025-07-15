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

#include "effect/shader_image_filter_obj.h"
#include "effect/image_filter_lazy.h"
#include "effect/shader_effect_lazy.h"
#include "utils/data.h"
#include "utils/log.h"

#ifdef ROSEN_OHOS
#include "utils/object_helper.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {
ShaderImageFilterObj::ShaderImageFilterObj()
    : ImageFilterObj(static_cast<int32_t>(ImageFilter::FilterType::SHADER))
{
}

std::shared_ptr<ShaderImageFilterObj> ShaderImageFilterObj::CreateForUnmarshalling()
{
    return std::shared_ptr<ShaderImageFilterObj>(new ShaderImageFilterObj());
}

std::shared_ptr<ShaderImageFilterObj> ShaderImageFilterObj::Create(const std::shared_ptr<ShaderEffect>& shader,
    const Rect& cropRect)
{
    return std::shared_ptr<ShaderImageFilterObj>(new ShaderImageFilterObj(shader, cropRect));
}

ShaderImageFilterObj::ShaderImageFilterObj(const std::shared_ptr<ShaderEffect>& shader,
    const Rect& cropRect)
    : ImageFilterObj(static_cast<int32_t>(ImageFilter::FilterType::SHADER)),
      shader_(shader),
      cropRect_(cropRect)
{
}

std::shared_ptr<void> ShaderImageFilterObj::GenerateBaseObject()
{
    // If shader is Lazy type, need to Materialize first
    std::shared_ptr<ShaderEffect> actualShader = shader_;
    if (shader_ && shader_->IsLazy()) {
        auto lazyShader = std::static_pointer_cast<ShaderEffectLazy>(shader_);
        actualShader = lazyShader->Materialize();
    }

    // Use correct CreateShaderImageFilter
    auto shaderImageFilter = ImageFilter::CreateShaderImageFilter(actualShader, cropRect_);

    return std::static_pointer_cast<void>(shaderImageFilter);
}

#ifdef ROSEN_OHOS
bool ShaderImageFilterObj::Marshalling(Parcel& parcel)
{
    // Return failure if shader is null
    if (!shader_) {
        return false;
    }

    // Serialize cropRect and ShaderEffect (type and subType handled externally)
    bool ret = parcel.WriteFloat(cropRect_.GetLeft());
    ret &= parcel.WriteFloat(cropRect_.GetTop());
    ret &= parcel.WriteFloat(cropRect_.GetRight());
    ret &= parcel.WriteFloat(cropRect_.GetBottom());
    if (!ret) {
        LOGE("ShaderImageFilterObj::Marshalling, failed to write basic data");
        return false;
    }

    // Serialize whether it's Lazy type
    if (!parcel.WriteBool(shader_->IsLazy())) {
        LOGE("ShaderImageFilterObj::Marshalling, failed to write isLazy flag");
        return false;
    }

    // Use polymorphic Marshalling - handles both Lazy and non-Lazy types
    return shader_->Marshalling(parcel);
}

bool ShaderImageFilterObj::Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth)
{
    // Check maximum nesting depth to prevent stack overflow
    if (depth >= ObjectHelper::MAX_NESTING_DEPTH) {
        LOGE("ShaderImageFilterObj::Unmarshalling, depth %{public}d exceeds maximum limit %{public}d",
             depth, ObjectHelper::MAX_NESTING_DEPTH);
        return false;
    }

    // Read type and subType (already read by ObjectHelper before calling)
    // Here directly read ShaderImageFilterObj specific data

    // Deserialize cropRect
    float left;
    float top;
    float right;
    float bottom;
    if (!parcel.ReadFloat(left) || !parcel.ReadFloat(top) ||
        !parcel.ReadFloat(right) || !parcel.ReadFloat(bottom)) {
        LOGE("ShaderImageFilterObj::Unmarshalling, failed to read cropRect");
        return false;
    }
    cropRect_ = Rect(left, top, right, bottom);

    // Deserialize whether it's Lazy type
    bool isLazy;
    if (!parcel.ReadBool(isLazy)) {
        LOGE("ShaderImageFilterObj::Unmarshalling, failed to read isLazy flag");
        return false;
    }

    if (!isLazy) {
        shader_ = ShaderEffect::Unmarshalling(parcel, isValid);
    } else {
        shader_ = ShaderEffectLazy::Unmarshalling(parcel, isValid, depth);
    }
    if (!shader_) {
        LOGE("ShaderImageFilterObj::Unmarshalling, failed to unmarshal shader, lazy[%{public}d]", isLazy);
    }
    return shader_ != nullptr;
}

// Register ShaderImageFilterObj to ObjectHelper using macro
OBJECT_UNMARSHALLING_REGISTER(ShaderImageFilterObj,
    static_cast<int32_t>(Drawing::Object::ObjectType::IMAGE_FILTER),
    static_cast<int32_t>(Drawing::ImageFilter::FilterType::SHADER));

#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS