/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "render/rs_shader.h"
#include "platform/common/rs_log.h"
#include "render/rs_dot_matrix_shader.h"
#include "ge_visual_effect_impl.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSShader> RSShader::CreateRSShader()
{
    return std::make_shared<RSShader>();
}

std::shared_ptr<RSShader> RSShader::CreateRSShader(const std::shared_ptr<Drawing::ShaderEffect>& drShader)
{
    auto rsShader = std::make_shared<RSShader>();
    rsShader->SetDrawingShader(drShader);
    return rsShader;
}

std::shared_ptr<RSShader> RSShader::CreateRSShader(Parcel& parcel)
{
    std::shared_ptr<RSShader> shader = nullptr;
    auto effectType = static_cast<ShaderType>(parcel.ReadUint32());
    switch (effectType) {
        case ShaderType::DOT_MATRIX: {
            shader = std::make_shared<RSDotMatrixShader>();
            break;
        }
        case ShaderType::DRAWING: {
            shader = std::make_shared<RSShader>();
            break;
        }
        default:
            break;
    }
    if (shader == nullptr) {
        ROSEN_LOGE("RSShader::CreateRSShader shader is nullptr");
        return nullptr;
    }

    if (!shader->Unmarshalling(parcel)) {
        return nullptr;
    }

    return shader;
}

void RSShader::SetDrawingShader(const std::shared_ptr<Drawing::ShaderEffect>& drShader)
{
    drShader_ = drShader;
}

const std::shared_ptr<Drawing::ShaderEffect>& RSShader::GetDrawingShader() const
{
    return drShader_;
}

bool RSShader::Marshalling(Parcel& parcel)
{
    if (!parcel.WriteUint32((uint32_t)type_)) {
        ROSEN_LOGE("unirender: RSShader::Marshalling write ge effect type failed");
        return false;
    }

    auto& shaderEffect = GetDrawingShader();
    if (!shaderEffect) {
        ROSEN_LOGD("unirender: RSShader::Marshalling Drawing Shader is nullptr");
        return false;
    }

    int32_t type = static_cast<int32_t>(shaderEffect->GetType());
    std::shared_ptr<Drawing::Data> data = shaderEffect->Serialize();
    if (!data) {
        ROSEN_LOGE("unirender: RSShader::Marshalling, data is nullptr");
        return false;
    }
    return parcel.WriteInt32(type) && RSMarshallingHelper::Marshalling(parcel, data);
}

bool RSShader::Unmarshalling(Parcel& parcel)
{
    int32_t type = parcel.ReadInt32();
    if (type == -1) {
        return false;
    }
    std::shared_ptr<Drawing::Data> data;
    if (!RSMarshallingHelper::Unmarshalling(parcel, data) || !data) {
        ROSEN_LOGE("unirender: RSShader::Unmarshalling RSShader, data is nullptr");
        return false;
    }
    Drawing::ShaderEffect::ShaderEffectType shaderEffectType = Drawing::ShaderEffect::ShaderEffectType::NO_TYPE;
    if (type >= static_cast<int32_t>(Drawing::ShaderEffect::ShaderEffectType::COLOR_SHADER) &&
        type <= static_cast<int32_t>(Drawing::ShaderEffect::ShaderEffectType::EXTEND_SHADER)) {
        shaderEffectType = static_cast<Drawing::ShaderEffect::ShaderEffectType>(type);
    }
    auto shaderEffect = std::make_shared<Drawing::ShaderEffect>(shaderEffectType);
    if (!shaderEffect->Deserialize(data)) {
        ROSEN_LOGE("unirender: RSMarshallingHelper::Unmarshalling RSShader, Deserialize failed");
        return false;
    }
    SetDrawingShader(shaderEffect);
    return true;
}
} // namespace Rosen
} // namespace OHOS