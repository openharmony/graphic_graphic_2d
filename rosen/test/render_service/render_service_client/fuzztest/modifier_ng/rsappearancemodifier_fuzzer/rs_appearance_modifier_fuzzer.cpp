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

#include "rs_appearance_modifier_fuzzer.h"

#include "modifier_ng/appearance/rs_alpha_modifier.h"
#include "modifier_ng/appearance/rs_background_filter_modifier.h"
#include "modifier_ng/appearance/rs_behind_window_filter_modifier.h"
#include "modifier_ng/appearance/rs_blend_modifier.h"
#include "modifier_ng/appearance/rs_border_modifier.h"
#include "modifier_ng/appearance/rs_compositing_filter_modifier.h"
#include "modifier_ng/appearance/rs_dynamic_light_up_modifier.h"
#include "modifier_ng/appearance/rs_foreground_filter_modifier.h"
#include "modifier_ng/appearance/rs_hdr_brightness_modifier.h"
#include "modifier_ng/appearance/rs_mask_modifier.h"
#include "modifier_ng/appearance/rs_outline_modifier.h"
#include "modifier_ng/appearance/rs_particle_effect_modifier.h"
#include "modifier_ng/appearance/rs_pixel_stretch_modifier.h"
#include "modifier_ng/appearance/rs_point_light_modifier.h"
#include "modifier_ng/appearance/rs_shadow_modifier.h"
#include "modifier_ng/appearance/rs_use_effect_modifier.h"
#include "modifier_ng/appearance/rs_visibility_modifier.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool AlphaModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSAlphaModifier> modifier = std::make_shared<ModifierNG::RSAlphaModifier>();
    float alpha = GetData<float>();
    modifier->SetAlpha(alpha);
    bool res = ROSEN_EQ(modifier->GetAlpha(), alpha);

    bool offscreen = GetData<bool>();
    modifier->SetAlphaOffscreen(offscreen);
    res = res && ROSEN_EQ(modifier->GetAlphaOffscreen(), offscreen);
    return res;
}

bool BackgroundFilterModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSBackgroundFilterModifier> modifier =
        std::make_shared<ModifierNG::RSBackgroundFilterModifier>();

    bool randomBool = GetData<bool>();
    float randomFloat = GetData<float>();
    int randomInt = GetData<int>();

    modifier->SetSystemBarEffect(randomBool);
    modifier->GetSystemBarEffect();

    modifier->SetWaterRippleProgress(randomFloat);
    modifier->GetWaterRippleProgress();

    std::optional<RSWaterRipplePara> randomWaterRippleParams = std::make_optional<RSWaterRipplePara>();
    modifier->SetWaterRippleParams(randomWaterRippleParams);
    modifier->GetWaterRippleParams();

    std::shared_ptr<RSMagnifierParams> randomMagnifierParams = std::make_shared<RSMagnifierParams>();
    modifier->SetMagnifierParams(randomMagnifierParams);
    modifier->GetMagnifierParams();

    modifier->SetBackgroundBlurRadius(randomFloat);
    modifier->GetBackgroundBlurRadius();

    modifier->SetBackgroundBlurSaturation(randomFloat);
    modifier->GetBackgroundBlurSaturation();

    modifier->SetBackgroundBlurBrightness(randomFloat);
    modifier->GetBackgroundBlurBrightness();

    Color randomColor;
    modifier->SetBackgroundBlurMaskColor(randomColor);
    modifier->GetBackgroundBlurMaskColor();

    modifier->SetBackgroundBlurColorMode(randomInt);
    modifier->GetBackgroundBlurColorMode();

    modifier->SetBackgroundBlurRadiusX(randomFloat);
    modifier->GetBackgroundBlurRadiusX();

    modifier->SetBackgroundBlurRadiusY(randomFloat);
    modifier->GetBackgroundBlurRadiusY();

    modifier->SetBgBlurDisableSystemAdaptation(randomBool);
    modifier->GetBgBlurDisableSystemAdaptation();

    modifier->SetAlwaysSnapshot(randomBool);
    modifier->GetAlwaysSnapshot();

    Vector2f randomVec{randomFloat, randomFloat};
    modifier->SetGreyCoef(randomVec);
    modifier->GetGreyCoef();

    return true;
}

bool BehindWindowFilterModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSBehindWindowFilterModifier> modifier =
        std::make_shared<ModifierNG::RSBehindWindowFilterModifier>();

    float randomFloat = GetData<float>();
    modifier->SetBehindWindowFilterRadius(randomFloat);
    modifier->GetBehindWindowFilterRadius();

    modifier->SetBehindWindowFilterSaturation(randomFloat);
    modifier->GetBehindWindowFilterSaturation();

    modifier->SetBehindWindowFilterBrightness(randomFloat);
    modifier->GetBehindWindowFilterBrightness();

    Color maskColor {GetData<uint32_t>(), GetData<uint32_t>(), GetData<uint32_t>(), GetData<uint32_t>()};
    modifier->SetBehindWindowFilterMaskColor(maskColor);
    modifier->GetBehindWindowFilterMaskColor();

    return true;
}

bool BlendModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSBlendModifier> modifier = std::make_shared<ModifierNG::RSBlendModifier>();

    RSColorBlendMode blendMode = GetData<RSColorBlendMode>();
    modifier->SetColorBlendMode(blendMode);
    modifier->GetColorBlendMode();

    RSColorBlendApplyType applyType = GetData<RSColorBlendApplyType>();
    modifier->SetColorBlendApplyType(applyType);
    modifier->GetColorBlendApplyType();

    Vector4f randomVec4F(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>());
    modifier->SetFgBrightnessRates(randomVec4F);
    modifier->GetFgBrightnessRates();

    float randomFloat = GetData<float>();
    modifier->SetFgBrightnessSaturation(randomFloat);
    modifier->GetFgBrightnessSaturation();

    modifier->SetFgBrightnessPosCoeff(randomVec4F);
    modifier->GetFgBrightnessPosCoeff();

    modifier->SetFgBrightnessNegCoeff(randomVec4F);
    modifier->GetFgBrightnessNegCoeff();

    modifier->SetFgBrightnessFract(randomFloat);
    modifier->GetFgBrightnessFract();

    return true;
}

bool BorderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSBorderModifier> modifier = std::make_shared<ModifierNG::RSBorderModifier>();

    uint32_t borderStyle = GetData<uint32_t>();
    Vector4<uint32_t> style(borderStyle);
    modifier->SetBorderStyle(style);
    modifier->GetBorderStyle();

    Vector4f dashWidth{GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>()};
    modifier->SetBorderDashWidth(dashWidth);
    modifier->GetBorderDashWidth();

    Vector4f dashGap{GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>()};
    modifier->SetBorderDashGap(dashGap);
    modifier->GetBorderDashGap();

    return true;
}

bool CompositingFilterModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSCompositingFilterModifier> modifier =
        std::make_shared<ModifierNG::RSCompositingFilterModifier>();

    Vector4f aiInvert{GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>()};
    modifier->SetAiInvert(aiInvert);
    modifier->GetAiInvert();

    float randomFloat = GetData<float>();
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

    Color color {GetData<uint32_t>(), GetData<uint32_t>(), GetData<uint32_t>(), GetData<uint32_t>()};
    modifier->SetColorBlend(color);
    modifier->GetColorBlend();

    modifier->SetLightUpEffectDegree(randomFloat);
    modifier->GetLightUpEffectDegree();

    modifier->SetDynamicDimDegree(randomFloat);
    modifier->GetDynamicDimDegree();

    float blurRadius = GetData<float>();
    std::vector<std::pair<float, float>> fractionStops = {{GetData<float>(), GetData<float>()}};
    GradientDirection direction = GetData<GradientDirection>();
    std::shared_ptr<RSLinearGradientBlurPara> params =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    modifier->SetLinearGradientBlurPara(params);
    modifier->GetLinearGradientBlurPara();

    modifier->SetForegroundBlurRadius(randomFloat);
    modifier->GetForegroundBlurRadius();

    modifier->SetForegroundBlurSaturation(randomFloat);
    modifier->GetForegroundBlurSaturation();

    modifier->SetForegroundBlurBrightness(randomFloat);
    modifier->GetForegroundBlurBrightness();

    Color maskColor {GetData<uint32_t>(), GetData<uint32_t>(), GetData<uint32_t>(), GetData<uint32_t>()};
    modifier->SetForegroundBlurMaskColor(maskColor);
    modifier->GetForegroundBlurMaskColor();

    int colorMode = GetData<int>();
    modifier->SetForegroundBlurColorMode(colorMode);
    modifier->GetForegroundBlurColorMode();

    modifier->SetForegroundBlurRadiusX(randomFloat);
    modifier->GetForegroundBlurRadiusX();

    modifier->SetForegroundBlurRadiusY(randomFloat);
    modifier->GetForegroundBlurRadiusY();

    bool randomBool = GetData<bool>();
    modifier->SetFgBlurDisableSystemAdaptation(randomBool);
    modifier->GetFgBlurDisableSystemAdaptation();

    return true;
}

bool DynamicLightUpModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSDynamicLightUpModifier> modifier =
        std::make_shared<ModifierNG::RSDynamicLightUpModifier>();

    float randomFloat = GetData<float>();
    modifier->SetDynamicLightUpRate(randomFloat);
    modifier->GetDynamicLightUpRate();

    modifier->SetDynamicLightUpDegree(randomFloat);
    modifier->GetDynamicLightUpDegree();

    return true;
}

bool ForegroundFilterModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSForegroundFilterModifier> modifier =
        std::make_shared<ModifierNG::RSForegroundFilterModifier>();

    float randomFloat = GetData<float>();
    modifier->SetSpherize(randomFloat);
    modifier->GetSpherize();

    modifier->SetForegroundEffectRadius(randomFloat);
    modifier->GetForegroundEffectRadius();

    float radius = GetData<float>();
    Vector2f scaleAnchor = {GetData<float>(), GetData<float>()};
    std::shared_ptr<MotionBlurParam> params = std::make_shared<MotionBlurParam>(radius, scaleAnchor);
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

    Vector2f dstPoint{GetData<float>(), GetData<float>()};
    modifier->SetAttractionDstPoint(dstPoint);
    modifier->GetAttractionDstPoint();

    return true;
}

bool HDRBrightnessModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSHDRBrightnessModifier> modifier =
        std::make_shared<ModifierNG::RSHDRBrightnessModifier>();
    float randomFloat = GetData<float>();
    modifier->SetHDRBrightness(randomFloat);
    modifier->GetHDRBrightness();

    return true;
}

bool MaskModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSMaskModifier> modifier = std::make_shared<ModifierNG::RSMaskModifier>();
    std::shared_ptr<RSMask> mask = std::make_shared<RSMask>();
    modifier->SetMask(mask);
    modifier->GetMask();

    return true;
}

bool OutlineModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSOutlineModifier> modifier = std::make_shared<ModifierNG::RSOutlineModifier>();

    Color color {GetData<uint32_t>(), GetData<uint32_t>(), GetData<uint32_t>(), GetData<uint32_t>()};
    Vector4<Color> outlineColor{ color, color, color, color };
    modifier->SetOutlineColor(outlineColor);
    modifier->GetOutlineColor();

    Vector4f randomVec4F{GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>()};
    modifier->SetOutlineWidth(randomVec4F);
    modifier->GetOutlineWidth();

    uint32_t style = GetData<uint32_t>();
    Vector4<uint32_t> outlineStyle{style, style, style, style};
    modifier->SetOutlineStyle(outlineStyle);
    modifier->GetOutlineStyle();

    modifier->SetOutlineDashWidth(randomVec4F);
    modifier->GetOutlineDashWidth();

    modifier->SetOutlineDashGap(randomVec4F);
    modifier->GetOutlineDashGap();

    modifier->SetOutlineRadius(randomVec4F);
    modifier->GetOutlineRadius();

    return true;
}

bool ParticleEffectModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSParticleEffectModifier> modifier =
        std::make_shared<ModifierNG::RSParticleEffectModifier>();

    uint32_t emitterIndex = GetData<uint32_t>();
    std::optional<Vector2f> position = std::make_optional<Vector2f>();
    std::optional<Vector2f> emitSize = std::make_optional<Vector2f>();
    std::optional<int> emitRate = std::make_optional<int>(GetData<int>());
    std::vector<std::shared_ptr<EmitterUpdater>> para =
        { std::make_shared<EmitterUpdater>(emitterIndex, position, emitSize, emitRate) };
    modifier->SetEmitterUpdater(para);
    modifier->GetEmitterUpdater();

    std::shared_ptr<ParticleNoiseFields> param = std::make_shared<ParticleNoiseFields>();
    modifier->SetParticleNoiseFields(param);
    modifier->GetParticleNoiseFields();
    return true;
}

bool PixelStretchModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSPixelStretchModifier> modifier =
        std::make_shared<ModifierNG::RSPixelStretchModifier>();
    Vector4f randomVec4F(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>());
    modifier->SetPixelStretchSize(randomVec4F);
    modifier->GetPixelStretchSize();

    int mode = GetData<int>();
    modifier->SetPixelStretchTileMode(mode);
    modifier->GetPixelStretchTileMode();

    modifier->SetPixelStretchPercent(randomVec4F);
    modifier->GetPixelStretchPercent();
    return true;
}

bool PointLightModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSPointLightModifier> modifier = std::make_shared<ModifierNG::RSPointLightModifier>();

    float randomFloat = GetData<float>();
    modifier->SetLightIntensity(randomFloat);
    modifier->GetLightIntensity();

    Vector4f randomVec4F(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>());
    modifier->SetLightPosition(randomVec4F);
    modifier->GetLightPosition();

    modifier->SetIlluminatedBorderWidth(randomFloat);
    modifier->GetIlluminatedBorderWidth();

    int type = GetData<int>();
    modifier->SetIlluminatedType(type);
    modifier->GetIlluminatedType();

    modifier->SetBloom(randomFloat);
    modifier->GetBloom();
    return true;
}

bool ShadowModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSShadowModifier> modifier = std::make_shared<ModifierNG::RSShadowModifier>();
    Color color{GetData<uint32_t>(), GetData<uint32_t>(), GetData<uint32_t>(), GetData<uint32_t>()};
    modifier->SetShadowColor(color);
    modifier->GetShadowColor();

    float randomFloat = GetData<float>();
    modifier->SetShadowOffsetX(randomFloat);
    modifier->GetShadowOffsetX();

    modifier->SetShadowOffsetY(randomFloat);
    modifier->GetShadowOffsetY();

    modifier->SetShadowAlpha(randomFloat);
    modifier->GetShadowAlpha();

    modifier->SetShadowElevation(randomFloat);
    modifier->GetShadowElevation();

    modifier->SetShadowRadius(randomFloat);
    modifier->GetShadowRadius();

    std::shared_ptr<RSPath> path = std::make_shared<RSPath>();
    modifier->SetShadowPath(path);
    modifier->GetShadowPath();

    float randomBool = GetData<bool>();
    modifier->SetShadowMask(randomBool);
    modifier->GetShadowMask();

    int strategy = GetData<int>();
    modifier->SetShadowColorStrategy(strategy);
    modifier->GetShadowColorStrategy();

    modifier->SetShadowIsFilled(randomBool);
    modifier->GetShadowIsFilled();

    modifier->SetUseShadowBatching(randomBool);
    modifier->GetUseShadowBatching();
    return true;
}

bool UseEffectModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSUseEffectModifier> modifier = std::make_shared<ModifierNG::RSUseEffectModifier>();
    float randomBool = GetData<bool>();
    modifier->SetUseEffect(randomBool);
    modifier->GetUseEffect();

    UseEffectType type = GetData<UseEffectType>();
    modifier->SetUseEffectType(type);
    modifier->GetUseEffectType();

    return true;
}

bool VisibilityModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSVisibilityModifier> modifier = std::make_shared<ModifierNG::RSVisibilityModifier>();
    bool randomBool = GetData<bool>();
    modifier->SetVisible(randomBool);
    modifier->GetVisible();

    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::AlphaModifierFuzzTest(data, size);
    OHOS::Rosen::BackgroundFilterModifierFuzzTest(data, size);
    OHOS::Rosen::BehindWindowFilterModifierFuzzTest(data, size);
    OHOS::Rosen::BlendModifierFuzzTest(data, size);
    OHOS::Rosen::BorderModifierFuzzTest(data, size);
    OHOS::Rosen::CompositingFilterModifierFuzzTest(data, size);
    OHOS::Rosen::DynamicLightUpModifierFuzzTest(data, size);
    OHOS::Rosen::ForegroundFilterModifierFuzzTest(data, size);
    OHOS::Rosen::HDRBrightnessModifierFuzzTest(data, size);
    OHOS::Rosen::MaskModifierFuzzTest(data, size);
    OHOS::Rosen::OutlineModifierFuzzTest(data, size);
    OHOS::Rosen::ParticleEffectModifierFuzzTest(data, size);
    OHOS::Rosen::PixelStretchModifierFuzzTest(data, size);
    OHOS::Rosen::PointLightModifierFuzzTest(data, size);
    OHOS::Rosen::ShadowModifierFuzzTest(data, size);
    OHOS::Rosen::UseEffectModifierFuzzTest(data, size);
    OHOS::Rosen::VisibilityModifierFuzzTest(data, size);
    return 0;
}

