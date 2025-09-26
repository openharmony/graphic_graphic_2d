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

#include "ui_effect/effect/include/border_light_effect_para.h"

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
    {RSNGEffectType::BORDER_LIGHT, [] {
            return std::make_shared<RSNGBorderLight>();
        }
    },
    {RSNGEffectType::COLOR_GRADIENT_EFFECT, [] {
            return std::make_shared<RSNGColorGradientEffect>();
        }
    }
};

namespace {

// for type inference
using ColorGradientEffectColorTags = std::tuple<
    ColorGradientEffectColor0Tag,
    ColorGradientEffectColor1Tag,
    ColorGradientEffectColor2Tag,
    ColorGradientEffectColor3Tag,
    ColorGradientEffectColor4Tag,
    ColorGradientEffectColor5Tag,
    ColorGradientEffectColor6Tag,
    ColorGradientEffectColor7Tag,
    ColorGradientEffectColor8Tag,
    ColorGradientEffectColor9Tag,
    ColorGradientEffectColor10Tag,
    ColorGradientEffectColor11Tag>;

// for type inference
using ColorGradientEffectPositionTags = std::tuple<
    ColorGradientEffectPosition0Tag,
    ColorGradientEffectPosition1Tag,
    ColorGradientEffectPosition2Tag,
    ColorGradientEffectPosition3Tag,
    ColorGradientEffectPosition4Tag,
    ColorGradientEffectPosition5Tag,
    ColorGradientEffectPosition6Tag,
    ColorGradientEffectPosition7Tag,
    ColorGradientEffectPosition8Tag,
    ColorGradientEffectPosition9Tag,
    ColorGradientEffectPosition10Tag,
    ColorGradientEffectPosition11Tag>;

// for type inference
using ColorGradientEffectStrengthTags = std::tuple<
    ColorGradientEffectStrength0Tag,
    ColorGradientEffectStrength1Tag,
    ColorGradientEffectStrength2Tag,
    ColorGradientEffectStrength3Tag,
    ColorGradientEffectStrength4Tag,
    ColorGradientEffectStrength5Tag,
    ColorGradientEffectStrength6Tag,
    ColorGradientEffectStrength7Tag,
    ColorGradientEffectStrength8Tag,
    ColorGradientEffectStrength9Tag,
    ColorGradientEffectStrength10Tag,
    ColorGradientEffectStrength11Tag>;

std::shared_ptr<RSNGShaderBase> ConvertColorGradientEffectPara(std::shared_ptr<VisualEffectPara> effectPara)
{
    auto effect = RSNGShaderBase::Create(RSNGEffectType::COLOR_GRADIENT_EFFECT);
    auto colorGradientEffect = std::static_pointer_cast<RSNGColorGradientEffect>(effect);
    auto colorGradientEffectPara = std::static_pointer_cast<ColorGradientEffectPara>(effectPara);

    std::vector<Vector4f> colors = colorGradientEffectPara->GetColors();
    std::vector<Vector2f> positions = colorGradientEffectPara->GetPositions();
    std::vector<float> strths = colorGradientEffectPara->GetStrengths();

    float blend = colorGradientEffectPara->GetDefaultBlend();
    float blendK = colorGradientEffectPara->GetDefaultBlendK();
    Vector4f color4 = colorGradientEffectPara->GetDefaultColor4f();
    Vector2f pos2 = colorGradientEffectPara->GetDefaultPoint();
    float strth = colorGradientEffectPara->GetDefaultStrength();

    colorGradientEffect->Setter<ColorGradientEffectBlendTag>(blend);
    colorGradientEffect->Setter<ColorGradientEffectBlendKTag>(blendK);

    ColorGradientEffectColorTags colorTag{};
    std::apply([&colorGradientEffect, &colors, color4] (auto&&... args) {
        size_t i = 0;
        (colorGradientEffect->Setter<std::decay_t<decltype(args)>>(i < colors.size() ? colors[i++] : color4), ...);
        }, colorTag);

    ColorGradientEffectPositionTags posTag{};
    std::apply([&colorGradientEffect, &positions, pos2] (auto&&... args) {
        size_t i = 0;
        (colorGradientEffect->Setter<std::decay_t<decltype(args)>>(i < positions.size() ? positions[i++] : pos2), ...);
        }, posTag);

    ColorGradientEffectStrengthTags strthTag{};
    std::apply([&colorGradientEffect, &strths, strth] (auto&&... args) {
        size_t i = 0;
        (colorGradientEffect->Setter<std::decay_t<decltype(args)>>(i < strths.size() ? strths[i++] : strth), ...);
        }, strthTag);

    colorGradientEffect->Setter<ColorGradientEffectMaskTag>(
        RSNGMaskBase::Create(colorGradientEffectPara->GetMask()));

    colorGradientEffect->Setter<ColorGradientEffectColorNumberTag>(static_cast<float>(strths.size()));

    return colorGradientEffect;
}

std::shared_ptr<RSNGShaderBase> ConvertBorderLightPara(std::shared_ptr<VisualEffectPara> effectPara)
{
    auto effect = RSNGShaderBase::Create(RSNGEffectType::BORDER_LIGHT);
    bool isInvalid = (effect == nullptr || effectPara == nullptr);
    if (isInvalid) {
        return nullptr;
    }
    auto borderLightEffect = std::static_pointer_cast<RSNGBorderLight>(effect);
    auto borderLightEffectPara = std::static_pointer_cast<BorderLightEffectPara>(effectPara);

    borderLightEffect->Setter<BorderLightPositionTag>(borderLightEffectPara->GetLightPosition());
    borderLightEffect->Setter<BorderLightColorTag>(borderLightEffectPara->GetLightColor());
    borderLightEffect->Setter<BorderLightIntensityTag>(borderLightEffectPara->GetLightIntensity());
    borderLightEffect->Setter<BorderLightWidthTag>(borderLightEffectPara->GetLightWidth());
    return borderLightEffect;
}
}

static std::unordered_map<VisualEffectPara::ParaType, ShaderConvertor> convertorLUT = {
    { VisualEffectPara::ParaType::BORDER_LIGHT_EFFECT, ConvertBorderLightPara },
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
