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
#include "platform/common/rs_log.h"

#include "ui_effect/property/include/rs_ui_shader_base.h"

#include "ui_effect/effect/include/color_gradient_effect_para.h"

#include "ui_effect/property/include/rs_ui_mask_base.h"

namespace OHOS {
namespace Rosen {
using ShaderCreator = std::function<std::shared_ptr<RSNGShaderBase>()>;
using ShaderConvertor = std::function<std::shared_ptr<RSNGShaderBase>(std::shared_ptr<VisualEffectPara>)>;

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
    {RSNGEffectType::COLOR_GRADIENT_EFFECT, [] {
            return std::make_shared<RSNGColorGradientEffect>();
        }
    }
};

const size_t MAX_GRADIENT_SIZE = 12;
static std::vector<std::function<void(std::shared_ptr<RSNGColorGradientEffect>&, const Vector4f&)>>
    g_gradientColorSetVec = {
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector4f& setColor)
        {
            target->Setter<ColorGradientEffectColor0Tag>(setColor);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector4f& setColor)
        {
            target->Setter<ColorGradientEffectColor1Tag>(setColor);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector4f& setColor)
        {
            target->Setter<ColorGradientEffectColor2Tag>(setColor);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector4f& setColor)
        {
            target->Setter<ColorGradientEffectColor3Tag>(setColor);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector4f& setColor)
        {
            target->Setter<ColorGradientEffectColor4Tag>(setColor);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector4f& setColor)
        {
            target->Setter<ColorGradientEffectColor5Tag>(setColor);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector4f& setColor)
        {
            target->Setter<ColorGradientEffectColor6Tag>(setColor);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector4f& setColor)
        {
            target->Setter<ColorGradientEffectColor7Tag>(setColor);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector4f& setColor)
        {
            target->Setter<ColorGradientEffectColor8Tag>(setColor);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector4f& setColor)
        {
            target->Setter<ColorGradientEffectColor9Tag>(setColor);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector4f& setColor)
        {
            target->Setter<ColorGradientEffectColor10Tag>(setColor);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector4f& setColor)
        {
            target->Setter<ColorGradientEffectColor11Tag>(setColor);
        }
    };

static std::vector<std::function<void(std::shared_ptr<RSNGColorGradientEffect>&, const Vector2f&)>>
    g_gradientPosSetVec = {
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector2f& setPos)
        {
            target->Setter<ColorGradientEffectPosition0Tag>(setPos);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector2f& setPos)
        {
            target->Setter<ColorGradientEffectPosition1Tag>(setPos);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector2f& setPos)
        {
            target->Setter<ColorGradientEffectPosition2Tag>(setPos);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector2f& setPos)
        {
            target->Setter<ColorGradientEffectPosition3Tag>(setPos);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector2f& setPos)
        {
            target->Setter<ColorGradientEffectPosition4Tag>(setPos);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector2f& setPos)
        {
            target->Setter<ColorGradientEffectPosition5Tag>(setPos);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector2f& setPos)
        {
            target->Setter<ColorGradientEffectPosition6Tag>(setPos);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector2f& setPos)
        {
            target->Setter<ColorGradientEffectPosition7Tag>(setPos);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector2f& setPos)
        {
            target->Setter<ColorGradientEffectPosition8Tag>(setPos);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector2f& setPos)
        {
            target->Setter<ColorGradientEffectPosition9Tag>(setPos);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector2f& setPos)
        {
            target->Setter<ColorGradientEffectPosition10Tag>(setPos);
        },
        [](std::shared_ptr<RSNGColorGradientEffect>& target, const Vector2f& setPos)
        {
            target->Setter<ColorGradientEffectPosition11Tag>(setPos);
        }
    };

static std::vector<std::function<void(std::shared_ptr<RSNGColorGradientEffect>&, float)>> g_gradientStrengthSetVec = {
    [](std::shared_ptr<RSNGColorGradientEffect>& target, float setValue)
    {
        target->Setter<ColorGradientEffectStrength0Tag>(setValue);
    },
    [](std::shared_ptr<RSNGColorGradientEffect>& target, float setValue)
    {
        target->Setter<ColorGradientEffectStrength1Tag>(setValue);
    },
    [](std::shared_ptr<RSNGColorGradientEffect>& target, float setValue)
    {
        target->Setter<ColorGradientEffectStrength2Tag>(setValue);
    },
    [](std::shared_ptr<RSNGColorGradientEffect>& target, float setValue)
    {
        target->Setter<ColorGradientEffectStrength3Tag>(setValue);
    },
    [](std::shared_ptr<RSNGColorGradientEffect>& target, float setValue)
    {
        target->Setter<ColorGradientEffectStrength4Tag>(setValue);
    },
    [](std::shared_ptr<RSNGColorGradientEffect>& target, float setValue)
    {
        target->Setter<ColorGradientEffectStrength5Tag>(setValue);
    },
    [](std::shared_ptr<RSNGColorGradientEffect>& target, float setValue)
    {
        target->Setter<ColorGradientEffectStrength6Tag>(setValue);
    },
    [](std::shared_ptr<RSNGColorGradientEffect>& target, float setValue)
    {
        target->Setter<ColorGradientEffectStrength7Tag>(setValue);
    },
    [](std::shared_ptr<RSNGColorGradientEffect>& target, float setValue)
    {
        target->Setter<ColorGradientEffectStrength8Tag>(setValue);
    },
    [](std::shared_ptr<RSNGColorGradientEffect>& target, float setValue)
    {
        target->Setter<ColorGradientEffectStrength9Tag>(setValue);
    },
    [](std::shared_ptr<RSNGColorGradientEffect>& target, float setValue)
    {
        target->Setter<ColorGradientEffectStrength10Tag>(setValue);
    },
    [](std::shared_ptr<RSNGColorGradientEffect>& target, float setValue)
    {
        target->Setter<ColorGradientEffectStrength11Tag>(setValue);
    }
};

namespace {

std::shared_ptr<RSNGShaderBase> ConvertColorGradientEffectPara(std::shared_ptr<VisualEffectPara> effectPara)
{
    auto effect = RSNGShaderBase::Create(RSNGEffectType::COLOR_GRADIENT_EFFECT);
    auto colorGradientEffect = std::static_pointer_cast<RSNGColorGradientEffect>(effect);
    auto colorGradientEffectPara = std::static_pointer_cast<ColorGradientEffectPara>(effectPara);

    std::vector<Vector4f> colors = colorGradientEffectPara->GetColors();
    std::vector<Vector2f> positions = colorGradientEffectPara->GetPositions();
    std::vector<float> strengths = colorGradientEffectPara->GetStrengths();

    for (size_t i = 0; i < MAX_GRADIENT_SIZE; i++) {
        auto setColor = i < colors.size() ? colors[i] : Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        g_gradientColorSetVec[i](colorGradientEffect, setColor);

        auto setPos = i < positions.size() ? positions[i] : Vector2f(1.0f, 1.0f);
        g_gradientPosSetVec[i](colorGradientEffect, setPos);

        auto setValue = i < strengths.size() ? strengths[i] : 0.0f;
        g_gradientStrengthSetVec[i](colorGradientEffect, setValue);
    }

    colorGradientEffect->Setter<ColorGradientEffectMaskTag>(
        RSNGMaskBase::Create(colorGradientEffectPara->GetMask()));

    colorGradientEffect->Setter<ColorGradientEffectColorNumberTag>(static_cast<float>(strengths.size()));

    return colorGradientEffect;
}
}

static std::unordered_map<VisualEffectPara::ParaType, ShaderConvertor> convertorLUT = {
    { VisualEffectPara::ParaType::COLOR_GRADIENT_EFFECT, ConvertColorGradientEffectPara },
};

std::shared_ptr<RSNGShaderBase> RSNGShaderBase::Create(RSNGEffectType type)
{
    auto it = creatorLUT.find(type);
    return it != creatorLUT.end() ? it->second() : nullptr;
}

std::shared_ptr<RSNGShaderBase> RSNGShaderBase::Create(std::shared_ptr<VisualEffectPara> effectPara)
{
    if (!effectPara) {
        return nullptr;
    }

    auto it = convertorLUT.find(effectPara->GetParaType());
    return it != convertorLUT.end() ? it->second(effectPara) : nullptr;
}

} // namespace Rosen
} // namespace OHOS
