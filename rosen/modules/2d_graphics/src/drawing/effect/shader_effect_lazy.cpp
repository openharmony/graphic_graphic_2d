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
#include "effect/shader_effect_lazy.h"

#include "effect/blend_shader_obj.h"
#include "utils/data.h"
#include "utils/log.h"

#ifdef ROSEN_OHOS
#include <parcel.h>
#include "utils/object_helper.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {

std::shared_ptr<ShaderEffectLazy> ShaderEffectLazy::CreateBlendShader(const std::shared_ptr<ShaderEffect>& dst,
                                                                      const std::shared_ptr<ShaderEffect>& src,
                                                                      BlendMode mode)
{
    if (!dst || !src) {
        LOGD("ShaderEffectLazy::CreateBlendShader, dst or src shader is null");
        return nullptr;
    }

    auto blendShaderObj = BlendShaderObj::Create(dst, src, mode);
    if (!blendShaderObj) {
        LOGD("ShaderEffectLazy::CreateBlendShader, failed to create BlendShaderObj");
        return nullptr;
    }
    return std::shared_ptr<ShaderEffectLazy>(new ShaderEffectLazy(blendShaderObj));
}

std::shared_ptr<ShaderEffectLazy> ShaderEffectLazy::CreateFromShaderEffectObj(
    std::shared_ptr<ShaderEffectObj> object)
{
    if (!object) {
        LOGD("ShaderEffectLazy::CreateFromShaderEffectObj, object is null");
        return nullptr;
    }

    // Validate object type
    if (object->GetType() != static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT)) {
        LOGE("ShaderEffectLazy::CreateFromShaderEffectObj, invalid object type: %{public}d, expected: %{public}d",
             object->GetType(), static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT));
        return nullptr;
    }

    return std::shared_ptr<ShaderEffectLazy>(new ShaderEffectLazy(object));
}

std::shared_ptr<ShaderEffectLazy> ShaderEffectLazy::CreateForUnmarshalling()
{
    return std::shared_ptr<ShaderEffectLazy>(new ShaderEffectLazy());
}

ShaderEffectLazy::ShaderEffectLazy(std::shared_ptr<ShaderEffectObj> shaderEffectObj) noexcept
    : ShaderEffect(ShaderEffectType::LAZY_SHADER), shaderEffectObj_(shaderEffectObj)
{
}

std::shared_ptr<ShaderEffect> ShaderEffectLazy::Materialize()
{
    if (!shaderEffectCache_ && shaderEffectObj_) {
        auto baseObj = shaderEffectObj_->GenerateBaseObject();
        if (baseObj) {
            shaderEffectCache_ = std::static_pointer_cast<ShaderEffect>(baseObj);
        }
    }
    return shaderEffectCache_;
}

std::shared_ptr<Data> ShaderEffectLazy::Serialize() const
{
    LOGE("ShaderEffectLazy::Serialize, should use Marshalling() instead. Check IsLazy() first.");
    LOGD("ShaderEffectObj type: %{public}d, subType: %{public}d",
         shaderEffectObj_ ? shaderEffectObj_->GetType() : -1,
         shaderEffectObj_ ? shaderEffectObj_->GetSubType() : -1);
    return nullptr;
}

bool ShaderEffectLazy::Deserialize(std::shared_ptr<Data> data)
{
    LOGE("ShaderEffectLazy::Deserialize, should use Unmarshalling() instead. Check IsLazy() first.");
    LOGD("Attempted to deserialize %zu bytes into Lazy object", data ? data->GetSize() : 0);
    return false;
}

#ifdef ROSEN_OHOS
bool ShaderEffectLazy::Marshalling(Parcel& parcel)
{
    if (!shaderEffectObj_) {
        return false;
    }

    // Write type and subType from internal object
    if (!parcel.WriteInt32(shaderEffectObj_->GetType()) ||
        !parcel.WriteInt32(shaderEffectObj_->GetSubType())) {
        LOGE("ShaderEffectLazy::Marshalling, failed to write type and subType");
        return false;
    }

    // Use ShaderEffectObj's Marshalling for internal data
    return shaderEffectObj_->Marshalling(parcel);
}

std::shared_ptr<ShaderEffectLazy> ShaderEffectLazy::Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth)
{
    // Read type and subType with safety checks
    int32_t type;
    if (!parcel.ReadInt32(type)) {
        LOGE("ShaderEffectLazy::Unmarshalling, failed to read type");
        return nullptr;
    }
    int32_t subType;
    if (!parcel.ReadInt32(subType)) {
        LOGE("ShaderEffectLazy::Unmarshalling, failed to read subType");
        return nullptr;
    }

    // Create corresponding ShaderEffectObj (depth check is done inside func)
    auto func = ObjectHelper::Instance().GetFunc(type, subType);
    auto obj = func ? func(parcel, isValid, depth + 1) : nullptr;
    if (!obj) {
        LOGE("ShaderEffectLazy::Unmarshalling, failed to create ShaderEffectObj for "
            "type=%{public}d, subType=%{public}d", type, subType);
        return nullptr;
    }

    // Create ShaderEffectLazy to wrap DrawingObject
    auto shaderEffectObj = std::static_pointer_cast<ShaderEffectObj>(obj);
    auto lazyShader = CreateFromShaderEffectObj(shaderEffectObj);
    if (!lazyShader) {
        LOGE("ShaderEffectLazy::Unmarshalling, failed to create ShaderEffectLazy");
        return nullptr;
    }

    return lazyShader;
}
#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
