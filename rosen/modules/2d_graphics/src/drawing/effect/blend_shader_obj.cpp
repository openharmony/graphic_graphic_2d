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

#include "effect/blend_shader_obj.h"
#include "effect/shader_effect_lazy.h"
#include "utils/log.h"

#ifdef ROSEN_OHOS
#include "utils/object_helper.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {

BlendShaderObj::BlendShaderObj()
    : ShaderEffectObj(static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND)),
      dstShader_(nullptr),
      srcShader_(nullptr),
      blendMode_(BlendMode::SRC_OVER)
{
}

std::shared_ptr<BlendShaderObj> BlendShaderObj::CreateForUnmarshalling()
{
    return std::shared_ptr<BlendShaderObj>(new BlendShaderObj());
}

std::shared_ptr<BlendShaderObj> BlendShaderObj::Create(const std::shared_ptr<ShaderEffect>& dst,
                                                       const std::shared_ptr<ShaderEffect>& src,
                                                       BlendMode mode)
{
    if (!dst || !src) {
        LOGE("BlendShaderObj::Create, dst or src shader is null");
        return nullptr;
    }
    return std::shared_ptr<BlendShaderObj>(new BlendShaderObj(dst, src, mode));
}

BlendShaderObj::BlendShaderObj(const std::shared_ptr<ShaderEffect>& dst,
                               const std::shared_ptr<ShaderEffect>& src,
                               BlendMode mode)
    : ShaderEffectObj(static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND)),
      dstShader_(dst),
      srcShader_(src),
      blendMode_(mode)
{
}

std::shared_ptr<void> BlendShaderObj::GenerateBaseObject()
{
    if (!dstShader_ || !srcShader_) {
        return nullptr;
    }

    // If shaders are Lazy type, need to Materialize first
    std::shared_ptr<ShaderEffect> actualDst = dstShader_;
    std::shared_ptr<ShaderEffect> actualSrc = srcShader_;

    if (dstShader_->IsLazy()) {
        auto lazyDst = std::static_pointer_cast<ShaderEffectLazy>(dstShader_);
        actualDst = lazyDst->Materialize();
        if (!actualDst) {
            return nullptr;
        }
    }

    if (srcShader_->IsLazy()) {
        auto lazySrc = std::static_pointer_cast<ShaderEffectLazy>(srcShader_);
        actualSrc = lazySrc->Materialize();
        if (!actualSrc) {
            return nullptr;
        }
    }

    // Use correct BlendShader constructor
    auto blendShader = ShaderEffect::CreateBlendShader(*actualDst, *actualSrc, blendMode_);

    return std::static_pointer_cast<void>(blendShader);
}

#ifdef ROSEN_OHOS
bool BlendShaderObj::Marshalling(Parcel& parcel)
{
    // Return failure if either shader is null
    if (!dstShader_ || !srcShader_) {
        return false;
    }

    // Serialize BlendMode (type and subType handled externally)
    bool ret = parcel.WriteInt32(static_cast<int32_t>(blendMode_));
    if (!ret) {
        LOGE("BlendShaderObj::Marshalling, failed to write basic data");
        return false;
    }

    // Serialize dst shader
    if (!parcel.WriteBool(dstShader_->IsLazy())) {
        LOGE("BlendShaderObj::Marshalling, failed to write isDstLazy flag");
        return false;
    }

    // Use polymorphic Marshalling - handles both Lazy and non-Lazy types
    if (!dstShader_->Marshalling(parcel)) {
        LOGE("BlendShaderObj::Marshalling, failed to marshal dst shader");
        return false;
    }

    // Serialize src shader
    if (!parcel.WriteBool(srcShader_->IsLazy())) {
        LOGE("BlendShaderObj::Marshalling, failed to write isSrcLazy flag");
        return false;
    }

    // Use polymorphic Marshalling - handles both Lazy and non-Lazy types
    return srcShader_->Marshalling(parcel);
}

bool BlendShaderObj::Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth)
{
    // Check maximum nesting depth to prevent stack overflow
    if (depth >= ObjectHelper::MAX_NESTING_DEPTH) {
        LOGE("BlendShaderObj::Unmarshalling, depth %{public}d exceeds maximum limit %{public}d",
             depth, ObjectHelper::MAX_NESTING_DEPTH);
        return false;
    }

    // Read BlendMode
    int32_t blendModeValue;
    if (!parcel.ReadInt32(blendModeValue)) {
        LOGE("BlendShaderObj::Unmarshalling, failed to read blendMode");
        return false;
    }
    blendMode_ = static_cast<BlendMode>(blendModeValue);

    // Deserialize dst shader
    bool isDstLazy;
    if (!parcel.ReadBool(isDstLazy)) {
        LOGE("BlendShaderObj::Unmarshalling, failed to read isDstLazy flag");
        return false;
    }

    if (!isDstLazy) {
        dstShader_ = ShaderEffect::Unmarshalling(parcel, isValid);
    } else {
        dstShader_ = ShaderEffectLazy::Unmarshalling(parcel, isValid, depth);
    }
    if (!dstShader_) {
        LOGE("BlendShaderObj::Unmarshalling, failed to unmarshal dst shader, lazy[%{public}d]", isDstLazy);
        return false;
    }

    // Deserialize src shader
    bool isSrcLazy;
    if (!parcel.ReadBool(isSrcLazy)) {
        LOGE("BlendShaderObj::Unmarshalling, failed to read isSrcLazy flag");
        return false;
    }

    if (!isSrcLazy) {
        srcShader_ = ShaderEffect::Unmarshalling(parcel, isValid);
    } else {
        srcShader_ = ShaderEffectLazy::Unmarshalling(parcel, isValid, depth);
    }
    if (!srcShader_) {
        LOGE("BlendShaderObj::Unmarshalling, failed to unmarshal src shader, lazy[%{public}d]", isSrcLazy);
        return false;
    }

    return true;
}

// Register BlendShaderObj to ObjectHelper using macro
OBJECT_UNMARSHALLING_REGISTER(BlendShaderObj,
    static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT),
    static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND));

#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS