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

#include "ui_effect/effect/include/border_light_effect_para.h"

#include "ui_effect/property/include/rs_ui_shader_base.h"

namespace OHOS {
namespace Rosen {
using ShaderCreator = std::function<std::shared_ptr<RSNGShaderBase>()>;
using ShaderConvertor = std::function<std::shared_ptr<RSNGShaderBase>(std::shared_ptr<VisualEffectPara)>;

static std::unordered_map<RSNGEffectType, ShaderCreator> creatorLUT = {
    {RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT, [] {
            return std::make_shared<RSNGContourDiagonalFlowLight>();
        }
    },
    {RSNGEffectType::WAVY_RIPPLE_LIGHT, [] {
            return std::make_shared<RSNGWavyRippleLight>();
        }
    },
    {RSNGEffectType::AURORA_NOISE, [] {
            return std::make_shared<RSNGAuroraNoise>();
        }
    },
    {RSNGEffectType::PARTICLE_CIRCULAR_HALO, [] {
            return std::make_shared<RSNGParticleCircularHalo>();
        }
    },
    {RSNGEffectType::BORDER_LIGHT, [] {
            return std::make_shared<RSNGBorderLight>();
        }
    }
};

namespace {
std::shared_ptr<RSNGShaderBase> ConverBorderLightPara(std::shared_ptr<VisualEffectPara> effectPara)
{
    auto effect = RSNGShaderBase::Create(RSNGEffectType::BORDER_LIGHT);
    bool isInvalid = (effect == nullptr || effectPara == nullptr);
    if (isInvalid) {
        return nullptr;
    }
    auto borderLightEffect = std::static_pointer_cast<RSBoderLight>(effect);
    auto borderLightEffectPara = std::static_pointer_cast<BorderLightEffectPara>(effectPara);

    borderLightEffect->Setter<BorderLightPositionTag>(borderLightEffectPara->GetLightPosition());
    borderLightEffect->Setter<BorderLightColorTag>(borderLightEffectPara->GetLightColor());
    borderLightEffect->Setter<BorderLightIntensityTag>(borderLightEffectPara->GetLightIntensity());
    borderLightEffect->Setter<BorderLightWidthTag>(borderLightEffectPara->GetLightWidth());
}
}
std::shared_ptr<RSNGShaderBase> RSNGShaderBase::Create(RSNGEffectType type)
{
    auto it = creatorLUT.find(type);
    return it != creatorLUT.end() ? it->second() : nullptr;
}

std::shared_ptr<RSNGShaderBase> RSNGShaderBase::Create(std::shared_ptr<VisualEffect> effectPara)
{
    if (!effectPara) {
        return nullptr;
    }

    auto it = convertorLUT.find(effectPara->GetParaType());
    return it != convertorLUT.end() ? it->second(effectPara) : nullptr;
}

} // namespace Rosen
} // namespace OHOS
