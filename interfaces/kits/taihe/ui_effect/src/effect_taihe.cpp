/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "effect_taihe.h"
#include "mask_taihe.h"

#include "effect/include/background_color_effect_para.h"
#include "effect/include/border_light_effect_para.h"
#include "effect/include/brightness_blender.h"
#include "effect/include/color_gradient_effect_para.h"
#include "effect/include/visual_effect.h"
#include "ohos.graphics.uiEffect.uiEffect.BrightnessBlender.proj.1.hpp"
#include "ohos.graphics.uiEffect.uiEffect.VisualEffect.proj.1.hpp"
#include "ui_effect_taihe_utils.h"

using namespace ANI::UIEffect;

namespace ANI::UIEffect {
VisualEffectImpl::VisualEffectImpl()
{
    nativeVisualEffect_ = std::make_shared<OHOS::Rosen::VisualEffect>();
}

VisualEffectImpl::VisualEffectImpl(std::shared_ptr<OHOS::Rosen::VisualEffect> visualEffect)
{
    nativeVisualEffect_ = visualEffect;
}

VisualEffectImpl::~VisualEffectImpl()
{
    nativeVisualEffect_ = nullptr;
}

VisualEffect VisualEffectImpl::BackgroundColorBlender(BrightnessBlender const& brightnessBlender)
{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call backgroundColorBlender failed, is not system app");
        return make_holder<VisualEffectImpl, VisualEffect>(nativeVisualEffect_);
    }

    if (!IsVisualEffectValid()) {
        UIEFFECT_LOG_E("VisualEffectImpl::backgroundColorBlender failed, visual effect is invalid");
        return make_holder<VisualEffectImpl, VisualEffect>(nativeVisualEffect_);
    }

    auto backgroundColorEffectPara = std::make_shared<OHOS::Rosen::BackgroundColorEffectPara>();
    auto blender = std::make_shared<OHOS::Rosen::BrightnessBlender>();
    if (!ParseBrightnessBlender(*blender, brightnessBlender)) {
        UIEFFECT_LOG_E("VisualEffectImpl::backgroundColorBlender parse brightnessBlender failed");
        return make_holder<VisualEffectImpl, VisualEffect>(nativeVisualEffect_);
    }

    backgroundColorEffectPara->SetBlender(blender);

    nativeVisualEffect_->AddPara(backgroundColorEffectPara);
    return make_holder<VisualEffectImpl, VisualEffect>(nativeVisualEffect_);
}

VisualEffect VisualEffectImpl::BorderLight(uintptr_t lightPosition, uintptr_t lightColor, double lightIntensity,
    double borderWidth)
{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call borderLight failed, is not system app");
        return make_holder<VisualEffectImpl, VisualEffect>(nativeVisualEffect_);
    }

    if (!IsVisualEffectValid()) {
        UIEFFECT_LOG_E("VisualEffectImpl::borderLight failed, visual effect is invalid");
        return make_holder<VisualEffectImpl, VisualEffect>(nativeVisualEffect_);
    }

    auto para = std::make_shared<OHOS::Rosen::BorderLightEffectPara>();

    OHOS::Rosen::Vector3f lightPositionRes;
    OHOS::Rosen::Vector4f lightColorRes;

    if (!ConvertVector3fFromAniPoint3D(lightPosition, lightPositionRes)) {
        return make_holder<VisualEffectImpl, VisualEffect>(nativeVisualEffect_);
    }
    if (!ConvertVector4fFromAniColor(lightColor, lightColorRes)) {
        return make_holder<VisualEffectImpl, VisualEffect>(nativeVisualEffect_);
    }
    para->SetLightPosition(lightPositionRes);
    para->SetLightColor(lightColorRes);
    para->SetLightIntensity(static_cast<float>(lightIntensity));
    para->SetLightWidth(static_cast<float>(borderWidth));

    nativeVisualEffect_->AddPara(para);
    return make_holder<VisualEffectImpl, VisualEffect>(nativeVisualEffect_);
}

VisualEffect VisualEffectImpl::ColorGradient(array_view<Color> colors, array_view<uintptr_t> positions,
    array_view<double> strengths, optional_view<Mask> alphaMask)
{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call colorGradient failed, is not system app");
        return make_holder<VisualEffectImpl, VisualEffect>(nativeVisualEffect_);
    }

    if (!IsVisualEffectValid()) {
        UIEFFECT_LOG_E("VisualEffectImpl::colorGradient failed, visual effect is invalid");
        return make_holder<VisualEffectImpl, VisualEffect>(nativeVisualEffect_);
    }

    uint32_t arraySizeColor = colors.size();
    uint32_t arraySizePos = positions.size();
    uint32_t arraySizeStrength = strengths.size();
    if (arraySizeColor != arraySizePos || arraySizeColor != arraySizeStrength || arraySizeStrength > NUM_12) {
        UIEFFECT_LOG_E("CreateColorGradientEffect param Error");
        return make_holder<VisualEffectImpl, VisualEffect>(nativeVisualEffect_);
    }
    auto para = std::make_shared<OHOS::Rosen::ColorGradientEffectPara>();

    std::vector<OHOS::Rosen::Vector4f> colorValue;
    std::vector<OHOS::Rosen::Vector2f> posValue;
    std::vector<float> strengthValue;
    colorValue.reserve(arraySizeColor);
    posValue.reserve(arraySizeColor);
    strengthValue.reserve(arraySizeColor);

    OHOS::Rosen::Vector4f colorRes;
    OHOS::Rosen::Vector2f posRes;
    for (uint32_t i = 0; i < arraySizeColor; ++i) {
        colorRes = OHOS::Rosen::Vector4f(colors[i].red, colors[i].green, colors[i].blue, colors[i].alpha);
        if (!ConvertVector2fFromAniPoint(positions[i], posRes)) {
            return make_holder<VisualEffectImpl, VisualEffect>(nativeVisualEffect_);
        }
        colorValue.emplace_back(colorRes);
        posValue.emplace_back(posRes);
        strengthValue.emplace_back(static_cast<float>(strengths[i]));
    }
    para->SetColors(colorValue);
    para->SetPositions(posValue);
    para->SetStrengths(strengthValue);
    if (alphaMask.has_value()) {
        MaskImpl* maskImpl = reinterpret_cast<MaskImpl*>(alphaMask.value()->GetImplPtr());
        if (maskImpl && maskImpl->GetNativePtr()) {
            para->SetMask(maskImpl->GetNativePtr()->GetMaskPara());
        }
    }
    nativeVisualEffect_->AddPara(para);
    return make_holder<VisualEffectImpl, VisualEffect>(nativeVisualEffect_);
}

bool VisualEffectImpl::IsVisualEffectValid() const
{
    return nativeVisualEffect_ != nullptr;
}

VisualEffect CreateEffect()
{
    return make_holder<VisualEffectImpl, VisualEffect>();
}

} // namespace ANI::UIEffect

// NOLINTBEGIN
TH_EXPORT_CPP_API_CreateEffect(CreateEffect);
// NOLINTEND