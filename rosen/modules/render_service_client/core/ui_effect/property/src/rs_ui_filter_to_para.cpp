/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "ui_effect/property/include/rs_ui_filter_to_para.h"
#include "ui_effect/property/include/rs_ui_mask_to_para.h"

#include "ui_effect/filter/include/filter_frosted_glass_blur_para.h"
#include "ui_effect/filter/include/filter_frosted_glass_para.h"
#include "ui_effect/filter/include/filter_para.h"
#include "ui_effect/property/include/rs_ui_filter_base.h"
#include "ui_effect/property/include/rs_ui_property_tag.h"

namespace OHOS {
namespace Rosen {

namespace {

std::shared_ptr<FilterPara> ConvertFrostedGlassBlurFilterToPara(std::shared_ptr<RSNGFilterBase> filter)
{
    if (!filter) {
        return nullptr;
    }
    auto frostedGlassBlurFilter = std::static_pointer_cast<RSNGFrostedGlassBlurFilter>(filter);
    auto para = std::make_shared<FrostedGlassBlurPara>();
    para->SetBlurRadius(frostedGlassBlurFilter->Getter<FrostedGlassBlurRadiusTag>()->Get());
    para->SetBlurRadiusScaleK(frostedGlassBlurFilter->Getter<FrostedGlassBlurRadiusScaleKTag>()->Get());
    para->SetRefractOutPx(frostedGlassBlurFilter->Getter<FrostedGlassBlurRefractOutPxTag>()->Get());
    para->SetSkipFrameEnable(frostedGlassBlurFilter->Getter<FrostedGlassBlurSkipFrameEnableTag>()->Get());
    return para;
}

std::shared_ptr<FilterPara> ConvertFrostedGlassFilterToPara(std::shared_ptr<RSNGFilterBase> filter)
{
    if (!filter) {
        return nullptr;
    }
    auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);
    auto para = std::make_shared<FrostedGlassPara>();
    Vector2f blurParams = frostedGlassFilter->Getter<FrostedGlassBlurParamsTag>()->Get();
    para->SetBlurParams(blurParams);
    Vector2f weightsEmboss = frostedGlassFilter->Getter<FrostedGlassWeightsEmbossTag>()->Get();
    para->SetWeightsEmboss(weightsEmboss);
    Vector2f weightsEdl = frostedGlassFilter->Getter<FrostedGlassWeightsEdlTag>()->Get();
    para->SetWeightsEdl(weightsEdl);
    Vector2f bgRates = frostedGlassFilter->Getter<FrostedGlassBgRatesTag>()->Get();
    para->SetBgRates(bgRates);
    Vector3f bgKBS = frostedGlassFilter->Getter<FrostedGlassBgKBSTag>()->Get();
    para->SetBgKBS(bgKBS);
    Vector3f bgPos = frostedGlassFilter->Getter<FrostedGlassBgPosTag>()->Get();
    para->SetBgPos(bgPos);
    Vector3f bgNeg = frostedGlassFilter->Getter<FrostedGlassBgNegTag>()->Get();
    para->SetBgNeg(bgNeg);
    para->SetBgAlpha(frostedGlassFilter->Getter<FrostedGlassBgAlphaTag>()->Get());
    Vector3f refractParams = frostedGlassFilter->Getter<FrostedGlassRefractParamsTag>()->Get();
    para->SetRefractParams(refractParams);
    Vector3f sdParams = frostedGlassFilter->Getter<FrostedGlassSdParamsTag>()->Get();
    para->SetSdParams(sdParams);
    Vector2f sdRates = frostedGlassFilter->Getter<FrostedGlassSdRatesTag>()->Get();
    para->SetSdRates(sdRates);
    Vector3f sdKBS = frostedGlassFilter->Getter<FrostedGlassSdKBSTag>()->Get();
    para->SetSdKBS(sdKBS);
    Vector3f sdPos = frostedGlassFilter->Getter<FrostedGlassSdPosTag>()->Get();
    para->SetSdPos(sdPos);
    Vector3f sdNeg = frostedGlassFilter->Getter<FrostedGlassSdNegTag>()->Get();
    para->SetSdNeg(sdNeg);
    Vector3f envLightParams = frostedGlassFilter->Getter<FrostedGlassEnvLightParamsTag>()->Get();
    para->SetEnvLightParams(envLightParams);
    Vector2f envLightRates = frostedGlassFilter->Getter<FrostedGlassEnvLightRatesTag>()->Get();
    para->SetEnvLightRates(envLightRates);
    Vector3f envLightKBS = frostedGlassFilter->Getter<FrostedGlassEnvLightKBSTag>()->Get();
    para->SetEnvLightKBS(envLightKBS);
    Vector3f envLightPos = frostedGlassFilter->Getter<FrostedGlassEnvLightPosTag>()->Get();
    para->SetEnvLightPos(envLightPos);
    Vector3f envLightNeg = frostedGlassFilter->Getter<FrostedGlassEnvLightNegTag>()->Get();
    para->SetEnvLightNeg(envLightNeg);
    Vector2f edLightParams = frostedGlassFilter->Getter<FrostedGlassEdLightParamsTag>()->Get();
    para->SetEdLightParams(edLightParams);
    Vector2f edLightAngles = frostedGlassFilter->Getter<FrostedGlassEdLightAnglesTag>()->Get();
    para->SetEdLightAngles(edLightAngles);
    Vector2f edLightDir = frostedGlassFilter->Getter<FrostedGlassEdLightDirTag>()->Get();
    para->SetEdLightDir(edLightDir);
    Vector2f edLightRates = frostedGlassFilter->Getter<FrostedGlassEdLightRatesTag>()->Get();
    para->SetEdLightRates(edLightRates);
    Vector3f edLightKBS = frostedGlassFilter->Getter<FrostedGlassEdLightKBSTag>()->Get();
    para->SetEdLightKBS(edLightKBS);
    Vector3f edLightPos = frostedGlassFilter->Getter<FrostedGlassEdLightPosTag>()->Get();
    para->SetEdLightPos(edLightPos);
    Vector3f edLightNeg = frostedGlassFilter->Getter<FrostedGlassEdLightNegTag>()->Get();
    para->SetEdLightNeg(edLightNeg);
    para->SetBaseVibrancyEnabled(frostedGlassFilter->Getter<FrostedGlassBaseVibrancyEnabledTag>()->Get());
    para->SetBaseMaterialType(frostedGlassFilter->Getter<FrostedGlassBaseMaterialTypeTag>()->Get());
    Vector4f materialColor = frostedGlassFilter->Getter<FrostedGlassMaterialColorTag>()->Get();
    para->SetMaterialColor(materialColor);
    para->SetSamplingScale(frostedGlassFilter->Getter<FrostedGlassSamplingScaleTag>()->Get());
    auto mask = frostedGlassFilter->Getter<FrostedGlassWaveMaskTag>()->Get();
    para->SetMask(mask ? RSNGMaskToParaHelper::ConvertMaskToPara(mask) : nullptr);
    auto darkModeBlurParams = frostedGlassFilter->Getter<FrostedGlassDarkModeBlurParamsTag>()->Get();
    if (darkModeBlurParams.x_ > 0 || darkModeBlurParams.y_ > 0) {
        auto darkParams = std::make_shared<AdaptiveFrostedGlassParams>();
        darkParams->blurParams = darkModeBlurParams;
        darkParams->weightsEmboss = frostedGlassFilter->Getter<FrostedGlassDarkModeWeightsEmbossTag>()->Get();
        darkParams->bgRates = frostedGlassFilter->Getter<FrostedGlassDarkModeBgRatesTag>()->Get();
        darkParams->bgKBS = frostedGlassFilter->Getter<FrostedGlassDarkModeBgKBSTag>()->Get();
        darkParams->bgPos = frostedGlassFilter->Getter<FrostedGlassDarkModeBgPosTag>()->Get();
        darkParams->bgNeg = frostedGlassFilter->Getter<FrostedGlassDarkModeBgNegTag>()->Get();
        para->SetDarkAdaptiveParams(*darkParams);
    }
    para->SetSkipFrameEnable(frostedGlassFilter->Getter<FrostedGlassSkipFrameEnableTag>()->Get());
    return para;
}
} // namespace

std::shared_ptr<FilterPara> RSNGFilterToParaHelper::ConvertFilterToPara(std::shared_ptr<RSNGFilterBase> filter)
{
    using FilterToParaFunc = std::function<std::shared_ptr<FilterPara>(std::shared_ptr<RSNGFilterBase>)>;
    static std::unordered_map<RSNGEffectType, FilterToParaFunc> s_filterToParaLUT = {
        { RSNGEffectType::FROSTED_GLASS_BLUR, ConvertFrostedGlassBlurFilterToPara },
        { RSNGEffectType::FROSTED_GLASS, ConvertFrostedGlassFilterToPara },
    };
    if (!filter) {
        return nullptr;
    }
    auto it = s_filterToParaLUT.find(filter->GetType());
    return it != s_filterToParaLUT.end() ? it->second(filter) : nullptr;
}

} // namespace Rosen
} // namespace OHOS