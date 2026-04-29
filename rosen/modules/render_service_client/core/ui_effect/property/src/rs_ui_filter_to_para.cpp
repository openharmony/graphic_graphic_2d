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

#include "ui_effect/filter/include/filter_frosted_glass_blur_para.h"
#include "ui_effect/filter/include/filter_frosted_glass_para.h"
#include "ui_effect/filter/include/filter_para.h"
#include "ui_effect/property/include/rs_ui_filter_base.h"
#include "ui_effect/property/include/rs_ui_mask_to_para.h"
#include "ui_effect/property/include/rs_ui_property_tag.h"

namespace OHOS {
namespace Rosen {

namespace {

std::shared_ptr<FilterPara> ConvertFrostedGlassBlurFilterToPara(const RSNGFilterBase& filter)
{
    const auto& frostedGlassBlurFilter = static_cast<const RSNGFrostedGlassBlurFilter&>(filter);
    auto para = std::make_shared<FrostedGlassBlurPara>();
    para->SetBlurRadius(frostedGlassBlurFilter.Getter<FrostedGlassBlurRadiusTag>()->Get());
    para->SetBlurRadiusScaleK(frostedGlassBlurFilter.Getter<FrostedGlassBlurRadiusScaleKTag>()->Get());
    para->SetRefractOutPx(frostedGlassBlurFilter.Getter<FrostedGlassBlurRefractOutPxTag>()->Get());
    para->SetSkipFrameEnable(frostedGlassBlurFilter.Getter<FrostedGlassBlurSkipFrameEnableTag>()->Get());
    return para;
}

void CopyBasicParams(FrostedGlassPara& para, const RSNGFrostedGlassFilter& filter)
{
    Vector2f blurParams = filter.Getter<FrostedGlassBlurParamsTag>()->Get();
    para.SetBlurParams(blurParams);
    Vector2f weightsEmboss = filter.Getter<FrostedGlassWeightsEmbossTag>()->Get();
    para.SetWeightsEmboss(weightsEmboss);
    Vector2f weightsEdl = filter.Getter<FrostedGlassWeightsEdlTag>()->Get();
    para.SetWeightsEdl(weightsEdl);
    Vector2f bgRates = filter.Getter<FrostedGlassBgRatesTag>()->Get();
    para.SetBgRates(bgRates);
    Vector3f bgKBS = filter.Getter<FrostedGlassBgKBSTag>()->Get();
    para.SetBgKBS(bgKBS);
    Vector3f bgPos = filter.Getter<FrostedGlassBgPosTag>()->Get();
    para.SetBgPos(bgPos);
    Vector3f bgNeg = filter.Getter<FrostedGlassBgNegTag>()->Get();
    para.SetBgNeg(bgNeg);
    para.SetBgAlpha(filter.Getter<FrostedGlassBgAlphaTag>()->Get());
    Vector3f refractParams = filter.Getter<FrostedGlassRefractParamsTag>()->Get();
    para.SetRefractParams(refractParams);
}

void CopySdAndEnvLightParams(FrostedGlassPara& para, const RSNGFrostedGlassFilter& filter)
{
    Vector3f sdParams = filter.Getter<FrostedGlassSdParamsTag>()->Get();
    para.SetSdParams(sdParams);
    Vector2f sdRates = filter.Getter<FrostedGlassSdRatesTag>()->Get();
    para.SetSdRates(sdRates);
    Vector3f sdKBS = filter.Getter<FrostedGlassSdKBSTag>()->Get();
    para.SetSdKBS(sdKBS);
    Vector3f sdPos = filter.Getter<FrostedGlassSdPosTag>()->Get();
    para.SetSdPos(sdPos);
    Vector3f sdNeg = filter.Getter<FrostedGlassSdNegTag>()->Get();
    para.SetSdNeg(sdNeg);
    Vector3f envLightParams = filter.Getter<FrostedGlassEnvLightParamsTag>()->Get();
    para.SetEnvLightParams(envLightParams);
    Vector2f envLightRates = filter.Getter<FrostedGlassEnvLightRatesTag>()->Get();
    para.SetEnvLightRates(envLightRates);
    Vector3f envLightKBS = filter.Getter<FrostedGlassEnvLightKBSTag>()->Get();
    para.SetEnvLightKBS(envLightKBS);
    Vector3f envLightPos = filter.Getter<FrostedGlassEnvLightPosTag>()->Get();
    para.SetEnvLightPos(envLightPos);
    Vector3f envLightNeg = filter.Getter<FrostedGlassEnvLightNegTag>()->Get();
    para.SetEnvLightNeg(envLightNeg);
}

void CopyEdLightAndFinalParams(FrostedGlassPara& para, const RSNGFrostedGlassFilter& filter)
{
    Vector2f edLightParams = filter.Getter<FrostedGlassEdLightParamsTag>()->Get();
    para.SetEdLightParams(edLightParams);
    Vector2f edLightAngles = filter.Getter<FrostedGlassEdLightAnglesTag>()->Get();
    para.SetEdLightAngles(edLightAngles);
    Vector2f edLightDir = filter.Getter<FrostedGlassEdLightDirTag>()->Get();
    para.SetEdLightDir(edLightDir);
    Vector2f edLightRates = filter.Getter<FrostedGlassEdLightRatesTag>()->Get();
    para.SetEdLightRates(edLightRates);
    Vector3f edLightKBS = filter.Getter<FrostedGlassEdLightKBSTag>()->Get();
    para.SetEdLightKBS(edLightKBS);
    Vector3f edLightPos = filter.Getter<FrostedGlassEdLightPosTag>()->Get();
    para.SetEdLightPos(edLightPos);
    Vector3f edLightNeg = filter.Getter<FrostedGlassEdLightNegTag>()->Get();
    para.SetEdLightNeg(edLightNeg);
    para.SetBaseVibrancyEnabled(filter.Getter<FrostedGlassBaseVibrancyEnabledTag>()->Get());
    para.SetBaseMaterialType(filter.Getter<FrostedGlassBaseMaterialTypeTag>()->Get());
    Vector4f materialColor = filter.Getter<FrostedGlassMaterialColorTag>()->Get();
    para.SetMaterialColor(materialColor);
    para.SetSamplingScale(filter.Getter<FrostedGlassSamplingScaleTag>()->Get());
    auto mask = filter.Getter<FrostedGlassWaveMaskTag>()->Get();
    para.SetMask(mask ? RSNGMaskToParaHelper::ConvertMaskToPara(mask) : nullptr);
}

void HandleDarkAdaptiveParams(FrostedGlassPara& para, const RSNGFrostedGlassFilter& filter)
{
    auto darkModeBlurParams = filter.Getter<FrostedGlassDarkModeBlurParamsTag>()->Get();
    if (darkModeBlurParams.x_ <= 0 && darkModeBlurParams.y_ <= 0) {
        return;
    }
    auto darkParams = std::make_shared<AdaptiveFrostedGlassParams>();
    darkParams->blurParams = darkModeBlurParams;
    darkParams->weightsEmboss = filter.Getter<FrostedGlassDarkModeWeightsEmbossTag>()->Get();
    darkParams->bgRates = filter.Getter<FrostedGlassDarkModeBgRatesTag>()->Get();
    darkParams->bgKBS = filter.Getter<FrostedGlassDarkModeBgKBSTag>()->Get();
    darkParams->bgPos = filter.Getter<FrostedGlassDarkModeBgPosTag>()->Get();
    darkParams->bgNeg = filter.Getter<FrostedGlassDarkModeBgNegTag>()->Get();
    para.SetDarkAdaptiveParams(*darkParams);
}

std::shared_ptr<FilterPara> ConvertFrostedGlassFilterToPara(const RSNGFilterBase& filter)
{
    const auto& frostedGlassFilter = static_cast<const RSNGFrostedGlassFilter&>(filter);
    auto para = std::make_shared<FrostedGlassPara>();
    CopyBasicParams(*para, frostedGlassFilter);
    CopySdAndEnvLightParams(*para, frostedGlassFilter);
    CopyEdLightAndFinalParams(*para, frostedGlassFilter);
    HandleDarkAdaptiveParams(*para, frostedGlassFilter);
    para->SetSkipFrameEnable(frostedGlassFilter.Getter<FrostedGlassSkipFrameEnableTag>()->Get());
    return para;
}
} // namespace

std::shared_ptr<FilterPara> RSNGFilterToParaHelper::ConvertFilterToPara(std::shared_ptr<RSNGFilterBase> filter)
{
    using FilterToParaFunc = std::function<std::shared_ptr<FilterPara>(const RSNGFilterBase&)>;
    static std::unordered_map<RSNGEffectType, FilterToParaFunc> s_filterToParaLUT = {
        { RSNGEffectType::FROSTED_GLASS_BLUR, ConvertFrostedGlassBlurFilterToPara },
        { RSNGEffectType::FROSTED_GLASS, ConvertFrostedGlassFilterToPara },
    };
    if (!filter) {
        return nullptr;
    }
    auto it = s_filterToParaLUT.find(filter->GetType());
    return it != s_filterToParaLUT.end() ? it->second(*filter) : nullptr;
}

} // namespace Rosen
} // namespace OHOS