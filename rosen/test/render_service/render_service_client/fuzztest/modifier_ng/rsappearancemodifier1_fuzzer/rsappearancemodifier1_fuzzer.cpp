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

#include "rsappearancemodifier1_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "modifier_ng/appearance/rs_alpha_modifier.h"
#include "modifier_ng/appearance/rs_background_filter_modifier.h"
#include "modifier_ng/appearance/rs_behind_window_filter_modifier.h"
#include "modifier_ng/appearance/rs_blend_modifier.h"
#include "modifier_ng/appearance/rs_border_modifier.h"
#include "modifier_ng/appearance/rs_compositing_filter_modifier.h"
#include "modifier_ng/appearance/rs_dynamic_light_up_modifier.h"
#include "modifier_ng/appearance/rs_foreground_filter_modifier.h"
#include "modifier_ng/appearance/rs_hdr_brightness_modifier.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t COLOR_BLEND_MODE_MAX = 30;
constexpr uint32_t COLOR_BLEND_APPLY_TYPE_MAX = 3;
constexpr uint32_t GRADIENT_DIRECTION_MAX = 10;
const uint8_t DO_ALPHA = 0;
const uint8_t DO_BACKGROUND_FILTER = 1;
const uint8_t DO_BEHIND_WINDOW_FILTER = 2;
const uint8_t DO_BLEND = 3;
const uint8_t DO_BORDER = 4;
const uint8_t DO_COMPOSITING_FILTER = 5;
const uint8_t DO_DYNAMIC_LIGHT_UP = 6;
const uint8_t DO_FOREGROUND_FILTER = 7;
const uint8_t DO_HDR_BRIGHTNESS = 8;
const uint8_t TARGET_SIZE = 9;
}

void DoAlphaModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSAlphaModifier>();
    float alpha = fdp.ConsumeFloatingPoint<float>();
    modifier->SetAlpha(alpha);
    modifier->GetAlpha();
    bool offscreen = fdp.ConsumeBool();
    modifier->SetAlphaOffscreen(offscreen);
    modifier->GetAlphaOffscreen();
}

void DoBackgroundFilterModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSBackgroundFilterModifier>();
    modifier->SetSystemBarEffect(fdp.ConsumeBool());
    modifier->GetSystemBarEffect();
    float randomFloat = fdp.ConsumeFloatingPoint<float>();
    modifier->SetWaterRippleProgress(randomFloat);
    modifier->GetWaterRippleProgress();
    auto randomWaterRippleParams = std::make_optional<RSWaterRipplePara>();
    modifier->SetWaterRippleParams(randomWaterRippleParams);
    modifier->GetWaterRippleParams();
    modifier->SetBackgroundBlurRadius(fdp.ConsumeFloatingPoint<float>());
    modifier->GetBackgroundBlurRadius();
    modifier->SetBackgroundBlurSaturation(fdp.ConsumeFloatingPoint<float>());
    modifier->GetBackgroundBlurSaturation();
    modifier->SetBackgroundBlurBrightness(fdp.ConsumeFloatingPoint<float>());
    modifier->GetBackgroundBlurBrightness();
    Color randomColor{fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>(),
        fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>()};
    modifier->SetBackgroundBlurMaskColor(randomColor);
    modifier->GetBackgroundBlurMaskColor();
    modifier->SetBackgroundBlurColorMode(fdp.ConsumeIntegral<int>());
    modifier->GetBackgroundBlurColorMode();
    modifier->SetBackgroundBlurRadiusX(fdp.ConsumeFloatingPoint<float>());
    modifier->GetBackgroundBlurRadiusX();
    modifier->SetBackgroundBlurRadiusY(fdp.ConsumeFloatingPoint<float>());
    modifier->GetBackgroundBlurRadiusY();
    modifier->SetBgBlurDisableSystemAdaptation(fdp.ConsumeBool());
    modifier->GetBgBlurDisableSystemAdaptation();
    modifier->SetAlwaysSnapshot(fdp.ConsumeBool());
    modifier->GetAlwaysSnapshot();
    Vector2f randomVec{fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    modifier->SetGreyCoef(randomVec);
    modifier->GetGreyCoef();
}

void DoBehindWindowFilterModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSBehindWindowFilterModifier>();
    modifier->SetBehindWindowFilterRadius(fdp.ConsumeFloatingPoint<float>());
    modifier->GetBehindWindowFilterRadius();
    modifier->SetBehindWindowFilterSaturation(fdp.ConsumeFloatingPoint<float>());
    modifier->GetBehindWindowFilterSaturation();
    modifier->SetBehindWindowFilterBrightness(fdp.ConsumeFloatingPoint<float>());
    modifier->GetBehindWindowFilterBrightness();
    Color maskColor{fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>(),
        fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>()};
    modifier->SetBehindWindowFilterMaskColor(maskColor);
    modifier->GetBehindWindowFilterMaskColor();
}

void DoBlendModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSBlendModifier>();
    RSColorBlendMode blendMode = static_cast<RSColorBlendMode>(fdp.ConsumeIntegral<uint32_t>() % COLOR_BLEND_MODE_MAX);
    modifier->SetColorBlendMode(blendMode);
    modifier->GetColorBlendMode();
    RSColorBlendApplyType applyType =
        static_cast<RSColorBlendApplyType>(fdp.ConsumeIntegral<uint32_t>() % COLOR_BLEND_APPLY_TYPE_MAX);
    modifier->SetColorBlendApplyType(applyType);
    modifier->GetColorBlendApplyType();
    Vector4f randomVec4F(fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>());
    modifier->SetFgBrightnessRates(randomVec4F);
    modifier->GetFgBrightnessRates();
    float randomFloat = fdp.ConsumeFloatingPoint<float>();
    modifier->SetFgBrightnessSaturation(randomFloat);
    modifier->GetFgBrightnessSaturation();
    modifier->SetFgBrightnessPosCoeff(randomVec4F);
    modifier->GetFgBrightnessPosCoeff();
    modifier->SetFgBrightnessNegCoeff(randomVec4F);
    modifier->GetFgBrightnessNegCoeff();
    modifier->SetFgBrightnessFract(randomFloat);
    modifier->GetFgBrightnessFract();
}

void DoBorderModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSBorderModifier>();
    uint32_t borderStyle = fdp.ConsumeIntegral<uint32_t>();
    Vector4<uint32_t> style(borderStyle);
    modifier->SetBorderStyle(style);
    modifier->GetBorderStyle();
    Vector4f dashWidth{fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    modifier->SetBorderDashWidth(dashWidth);
    modifier->GetBorderDashWidth();
    Vector4f dashGap{fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    modifier->SetBorderDashGap(dashGap);
    modifier->GetBorderDashGap();
}

void DoCompositingFilterModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSCompositingFilterModifier>();
    Vector4f aiInvert{fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    modifier->SetAiInvert(aiInvert);
    modifier->GetAiInvert();
    float randomFloat = fdp.ConsumeFloatingPoint<float>();
    modifier->SetGrayScale(randomFloat);
    modifier->GetGrayScale();
    modifier->SetBrightness(randomFloat);
    modifier->GetBrightness();
    modifier->SetContrast(randomFloat);
    modifier->GetContrast();
    modifier->SetSaturate(randomFloat);
    modifier->GetSaturate();
    modifier->SetSepia(randomFloat);
    modifier->GetSepia();
    modifier->SetInvert(randomFloat);
    modifier->GetInvert();
    modifier->SetHueRotate(randomFloat);
    modifier->GetHueRotate();
    Color color{fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>(),
        fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>()};
    modifier->SetColorBlend(color);
    modifier->GetColorBlend();
    modifier->SetLightUpEffectDegree(randomFloat);
    modifier->GetLightUpEffectDegree();
    modifier->SetDynamicDimDegree(randomFloat);
    modifier->GetDynamicDimDegree();
    float blurRadius = fdp.ConsumeFloatingPoint<float>();
    std::vector<std::pair<float, float>> fractionStops = {
        {fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()}};
    GradientDirection direction =
        static_cast<GradientDirection>(fdp.ConsumeIntegral<uint32_t>() % GRADIENT_DIRECTION_MAX);
    auto params = std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    modifier->SetLinearGradientBlurPara(params);
    modifier->GetLinearGradientBlurPara();
    modifier->SetForegroundBlurRadius(randomFloat);
    modifier->GetForegroundBlurRadius();
    modifier->SetForegroundBlurSaturation(randomFloat);
    modifier->GetForegroundBlurSaturation();
    modifier->SetForegroundBlurBrightness(randomFloat);
    modifier->GetForegroundBlurBrightness();
    Color maskColor{fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>(),
        fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>()};
    modifier->SetForegroundBlurMaskColor(maskColor);
    modifier->GetForegroundBlurMaskColor();
    modifier->SetForegroundBlurColorMode(fdp.ConsumeIntegral<int>());
    modifier->GetForegroundBlurColorMode();
    modifier->SetForegroundBlurRadiusX(randomFloat);
    modifier->GetForegroundBlurRadiusX();
    modifier->SetForegroundBlurRadiusY(randomFloat);
    modifier->GetForegroundBlurRadiusY();
    modifier->SetFgBlurDisableSystemAdaptation(fdp.ConsumeBool());
    modifier->GetFgBlurDisableSystemAdaptation();
}

void DoDynamicLightUpModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSDynamicLightUpModifier>();
    float randomFloat = fdp.ConsumeFloatingPoint<float>();
    modifier->SetDynamicLightUpRate(randomFloat);
    modifier->GetDynamicLightUpRate();
    modifier->SetDynamicLightUpDegree(randomFloat);
    modifier->GetDynamicLightUpDegree();
}

void DoForegroundFilterModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSForegroundFilterModifier>();
    float randomFloat = fdp.ConsumeFloatingPoint<float>();
    modifier->SetSpherize(randomFloat);
    modifier->GetSpherize();
    modifier->SetForegroundEffectRadius(randomFloat);
    modifier->GetForegroundEffectRadius();
    float radius = fdp.ConsumeFloatingPoint<float>();
    Vector2f scaleAnchor = {fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    auto params = std::make_shared<MotionBlurParam>(radius, scaleAnchor);
    modifier->SetMotionBlurParam(params);
    modifier->GetMotionBlurParam();
    RSFlyOutPara para;
    modifier->SetFlyOutParams(para);
    modifier->GetFlyOutParams();
    modifier->SetFlyOutDegree(randomFloat);
    modifier->GetFlyOutDegree();
    modifier->SetDistortionK(randomFloat);
    modifier->GetDistortionK();
    modifier->SetAttractionFraction(randomFloat);
    modifier->GetAttractionFraction();
    Vector2f dstPoint{fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    modifier->SetAttractionDstPoint(dstPoint);
    modifier->GetAttractionDstPoint();
}

void DoHDRBrightnessModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSHDRBrightnessModifier>();
    modifier->SetHDRBrightness(fdp.ConsumeFloatingPoint<float>());
    modifier->GetHDRBrightness();
    modifier->SetHDRColorHeadroom(fdp.ConsumeFloatingPoint<float>());
    modifier->GetHDRColorHeadroom();
}

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_ALPHA:
            OHOS::Rosen::DoAlphaModifier(fdp);
            break;
        case OHOS::Rosen::DO_BACKGROUND_FILTER:
            OHOS::Rosen::DoBackgroundFilterModifier(fdp);
            break;
        case OHOS::Rosen::DO_BEHIND_WINDOW_FILTER:
            OHOS::Rosen::DoBehindWindowFilterModifier(fdp);
            break;
        case OHOS::Rosen::DO_BLEND:
            OHOS::Rosen::DoBlendModifier(fdp);
            break;
        case OHOS::Rosen::DO_BORDER:
            OHOS::Rosen::DoBorderModifier(fdp);
            break;
        case OHOS::Rosen::DO_COMPOSITING_FILTER:
            OHOS::Rosen::DoCompositingFilterModifier(fdp);
            break;
        case OHOS::Rosen::DO_DYNAMIC_LIGHT_UP:
            OHOS::Rosen::DoDynamicLightUpModifier(fdp);
            break;
        case OHOS::Rosen::DO_FOREGROUND_FILTER:
            OHOS::Rosen::DoForegroundFilterModifier(fdp);
            break;
        case OHOS::Rosen::DO_HDR_BRIGHTNESS:
            OHOS::Rosen::DoHDRBrightnessModifier(fdp);
            break;
        default:
            break;
    }
    return 0;
}
