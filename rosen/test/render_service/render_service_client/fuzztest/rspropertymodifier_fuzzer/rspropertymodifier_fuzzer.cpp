/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "rspropertymodifier_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "animation/rs_animation.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_director.h"
#include "modifier/rs_property.h"
#include "modifier/rs_property_modifier.h"

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

bool DoModifier001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto boundsmodifier = std::make_shared<RSBoundsModifier>(property);
    boundsmodifier->GetModifierType();
    boundsmodifier->CreateRenderModifier();
    auto boundsSizemodifier = std::make_shared<RSBoundsSizeModifier>(property);
    boundsSizemodifier->GetModifierType();
    boundsSizemodifier->CreateRenderModifier();
    auto boundsPositionmodifier = std::make_shared<RSBoundsPositionModifier>(property);
    boundsPositionmodifier->GetModifierType();
    boundsPositionmodifier->CreateRenderModifier();
    auto framemodifier = std::make_shared<RSFrameModifier>(property);
    framemodifier->GetModifierType();
    framemodifier->CreateRenderModifier();
    auto positionZmodifier = std::make_shared<RSPositionZModifier>(property);
    positionZmodifier->GetModifierType();
    positionZmodifier->CreateRenderModifier();
    auto sandBoxmodifier = std::make_shared<RSSandBoxModifier>(property);
    sandBoxmodifier->GetModifierType();
    sandBoxmodifier->CreateRenderModifier();
    auto pivotmodifier = std::make_shared<RSPivotModifier>(property);
    pivotmodifier->GetModifierType();
    pivotmodifier->CreateRenderModifier();
    auto pivotZmodifier = std::make_shared<RSPivotZModifier>(property);
    pivotZmodifier->GetModifierType();
    pivotZmodifier->CreateRenderModifier();
    auto quaternionmodifier = std::make_shared<RSQuaternionModifier>(property);
    quaternionmodifier->GetModifierType();
    quaternionmodifier->CreateRenderModifier();
    auto rotationmodifier = std::make_shared<RSRotationModifier>(property);
    rotationmodifier->GetModifierType();
    rotationmodifier->CreateRenderModifier();
    auto rotationXmodifier = std::make_shared<RSRotationXModifier>(property);
    rotationXmodifier->GetModifierType();
    rotationXmodifier->CreateRenderModifier();
    return true;
}

bool DoModifier002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto rotationYmodifier = std::make_shared<RSRotationYModifier>(property);
    rotationYmodifier->GetModifierType();
    rotationYmodifier->CreateRenderModifier();
    auto cameraDistancemodifier = std::make_shared<RSCameraDistanceModifier>(property);
    cameraDistancemodifier->GetModifierType();
    cameraDistancemodifier->CreateRenderModifier();
    auto scalemodifier = std::make_shared<RSScaleModifier>(property);
    scalemodifier->GetModifierType();
    scalemodifier->CreateRenderModifier();
    auto skewmodifier = std::make_shared<RSSkewModifier>(property);
    skewmodifier->GetModifierType();
    skewmodifier->CreateRenderModifier();
    auto perspmodifier = std::make_shared<RSPerspModifier>(property);
    perspmodifier->GetModifierType();
    perspmodifier->CreateRenderModifier();
    auto translatemodifier = std::make_shared<RSTranslateModifier>(property);
    translatemodifier->GetModifierType();
    translatemodifier->CreateRenderModifier();
    auto translateZmodifier = std::make_shared<RSTranslateZModifier>(property);
    translateZmodifier->GetModifierType();
    translateZmodifier->CreateRenderModifier();
    auto cornerRadiusmodifier = std::make_shared<RSCornerRadiusModifier>(property);
    cornerRadiusmodifier->GetModifierType();
    cornerRadiusmodifier->CreateRenderModifier();
    auto alphamodifier = std::make_shared<RSAlphaModifier>(property);
    alphamodifier->GetModifierType();
    alphamodifier->CreateRenderModifier();
    auto alphaOffscreenmodifier = std::make_shared<RSAlphaOffscreenModifier>(property);
    alphaOffscreenmodifier->GetModifierType();
    alphaOffscreenmodifier->CreateRenderModifier();
    auto envForegroundColormodifier = std::make_shared<RSEnvForegroundColorModifier>(property);
    envForegroundColormodifier->GetModifierType();
    envForegroundColormodifier->CreateRenderModifier();
    return true;
}

bool DoModifier003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto envForegroundColorStrategymodifier = std::make_shared<RSEnvForegroundColorStrategyModifier>(property);
    envForegroundColorStrategymodifier->GetModifierType();
    envForegroundColorStrategymodifier->CreateRenderModifier();
    auto foregroundColormodifier = std::make_shared<RSForegroundColorModifier>(property);
    foregroundColormodifier->GetModifierType();
    foregroundColormodifier->CreateRenderModifier();
    auto useShadowBatchingmodifier = std::make_shared<RSUseShadowBatchingModifier>(property);
    useShadowBatchingmodifier->GetModifierType();
    useShadowBatchingmodifier->CreateRenderModifier();
    auto backgroundColormodifier = std::make_shared<RSBackgroundColorModifier>(property);
    backgroundColormodifier->GetModifierType();
    backgroundColormodifier->CreateRenderModifier();
    auto backgroundShadermodifier = std::make_shared<RSBackgroundShaderModifier>(property);
    backgroundShadermodifier->GetModifierType();
    backgroundShadermodifier->CreateRenderModifier();
    auto bgImagemodifier = std::make_shared<RSBgImageModifier>(property);
    bgImagemodifier->GetModifierType();
    bgImagemodifier->CreateRenderModifier();
    auto bgImageInnerRectmodifier = std::make_shared<RSBgImageInnerRectModifier>(property);
    bgImageInnerRectmodifier->GetModifierType();
    bgImageInnerRectmodifier->CreateRenderModifier();
    auto bgImageWidthmodifier = std::make_shared<RSBgImageWidthModifier>(property);
    bgImageWidthmodifier->GetModifierType();
    bgImageWidthmodifier->CreateRenderModifier();
    auto bgImageHeightmodifier = std::make_shared<RSBgImageHeightModifier>(property);
    bgImageHeightmodifier->GetModifierType();
    bgImageHeightmodifier->CreateRenderModifier();
    auto bgImagePositionXmodifier = std::make_shared<RSBgImagePositionXModifier>(property);
    bgImagePositionXmodifier->GetModifierType();
    bgImagePositionXmodifier->CreateRenderModifier();
    auto bgImagePositionYmodifier = std::make_shared<RSBgImagePositionYModifier>(property);
    bgImagePositionYmodifier->GetModifierType();
    bgImagePositionYmodifier->CreateRenderModifier();
    return true;
}

bool DoModifier004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto borderColormodifier = std::make_shared<RSBorderColorModifier>(property);
    borderColormodifier->GetModifierType();
    borderColormodifier->CreateRenderModifier();
    auto borderWidthmodifier = std::make_shared<RSBorderWidthModifier>(property);
    borderWidthmodifier->GetModifierType();
    borderWidthmodifier->CreateRenderModifier();
    auto borderStylemodifier = std::make_shared<RSBorderStyleModifier>(property);
    borderStylemodifier->GetModifierType();
    borderStylemodifier->CreateRenderModifier();
    auto outlineColormodifier = std::make_shared<RSOutlineColorModifier>(property);
    outlineColormodifier->GetModifierType();
    outlineColormodifier->CreateRenderModifier();
    auto outlineWidthmodifier = std::make_shared<RSOutlineWidthModifier>(property);
    outlineWidthmodifier->GetModifierType();
    outlineWidthmodifier->CreateRenderModifier();
    auto outlineStylemodifier = std::make_shared<RSOutlineStyleModifier>(property);
    outlineStylemodifier->GetModifierType();
    outlineStylemodifier->CreateRenderModifier();
    auto outlineRadiusmodifier = std::make_shared<RSOutlineRadiusModifier>(property);
    outlineRadiusmodifier->GetModifierType();
    outlineRadiusmodifier->CreateRenderModifier();
    auto foregroundEffectRadiusmodifier = std::make_shared<RSForegroundEffectRadiusModifier>(property);
    foregroundEffectRadiusmodifier->GetModifierType();
    foregroundEffectRadiusmodifier->CreateRenderModifier();
    auto backgroundBlurRadiusmodifier = std::make_shared<RSBackgroundBlurRadiusModifier>(property);
    backgroundBlurRadiusmodifier->GetModifierType();
    backgroundBlurRadiusmodifier->CreateRenderModifier();
    auto backgroundBlurSaturationmodifier = std::make_shared<RSBackgroundBlurSaturationModifier>(property);
    backgroundBlurSaturationmodifier->GetModifierType();
    backgroundBlurSaturationmodifier->CreateRenderModifier();
    return true;
}

bool DoModifier005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto backgroundBlurBrightnessmodifier = std::make_shared<RSBackgroundBlurBrightnessModifier>(property);
    backgroundBlurBrightnessmodifier->GetModifierType();
    backgroundBlurBrightnessmodifier->CreateRenderModifier();
    auto backgroundBlurMaskColormodifier = std::make_shared<RSBackgroundBlurMaskColorModifier>(property);
    backgroundBlurMaskColormodifier->GetModifierType();
    backgroundBlurMaskColormodifier->CreateRenderModifier();
    auto backgroundBlurColorModemodifier = std::make_shared<RSBackgroundBlurColorModeModifier>(property);
    backgroundBlurColorModemodifier->GetModifierType();
    backgroundBlurColorModemodifier->CreateRenderModifier();
    auto backgroundBlurRadiusXmodifier = std::make_shared<RSBackgroundBlurRadiusXModifier>(property);
    backgroundBlurRadiusXmodifier->GetModifierType();
    backgroundBlurRadiusXmodifier->CreateRenderModifier();
    auto backgroundBlurRadiusYmodifier = std::make_shared<RSBackgroundBlurRadiusYModifier>(property);
    backgroundBlurRadiusYmodifier->GetModifierType();
    backgroundBlurRadiusYmodifier->CreateRenderModifier();
    auto foregroundBlurRadiusmodifier = std::make_shared<RSForegroundBlurRadiusModifier>(property);
    foregroundBlurRadiusmodifier->GetModifierType();
    foregroundBlurRadiusmodifier->CreateRenderModifier();
    auto foregroundBlurSaturationmodifier = std::make_shared<RSForegroundBlurSaturationModifier>(property);
    foregroundBlurSaturationmodifier->GetModifierType();
    foregroundBlurSaturationmodifier->CreateRenderModifier();
    auto foregroundBlurBrightnessmodifier = std::make_shared<RSForegroundBlurBrightnessModifier>(property);
    foregroundBlurBrightnessmodifier->GetModifierType();
    foregroundBlurBrightnessmodifier->CreateRenderModifier();
    auto foregroundBlurMaskColormodifier = std::make_shared<RSForegroundBlurMaskColorModifier>(property);
    foregroundBlurMaskColormodifier->GetModifierType();
    foregroundBlurMaskColormodifier->CreateRenderModifier();
    auto foregroundBlurColorModemodifier = std::make_shared<RSForegroundBlurColorModeModifier>(property);
    foregroundBlurColorModemodifier->GetModifierType();
    foregroundBlurColorModemodifier->CreateRenderModifier();
    auto foregroundBlurRadiusXmodifier = std::make_shared<RSForegroundBlurRadiusXModifier>(property);
    foregroundBlurRadiusXmodifier->GetModifierType();
    foregroundBlurRadiusXmodifier->CreateRenderModifier();
    return true;
}

bool DoModifier006(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto foregroundBlurRadiusYmodifier = std::make_shared<RSForegroundBlurRadiusYModifier>(property);
    foregroundBlurRadiusYmodifier->GetModifierType();
    foregroundBlurRadiusYmodifier->CreateRenderModifier();
    auto lightUpEffectmodifier = std::make_shared<RSLightUpEffectModifier>(property);
    lightUpEffectmodifier->GetModifierType();
    lightUpEffectmodifier->CreateRenderModifier();
    auto dynamicLightUpRatemodifier = std::make_shared<RSDynamicLightUpRateModifier>(property);
    dynamicLightUpRatemodifier->GetModifierType();
    dynamicLightUpRatemodifier->CreateRenderModifier();
    auto dynamicLightUpDegreemodifier = std::make_shared<RSDynamicLightUpDegreeModifier>(property);
    dynamicLightUpDegreemodifier->GetModifierType();
    dynamicLightUpDegreemodifier->CreateRenderModifier();
    auto fgBrightnessParamsmodifier = std::make_shared<RSFgBrightnessRatesModifier>(property);
    fgBrightnessParamsmodifier->GetModifierType();
    fgBrightnessParamsmodifier->CreateRenderModifier();
    auto fgBrightnessFractmodifier = std::make_shared<RSFgBrightnessFractModifier>(property);
    fgBrightnessFractmodifier->GetModifierType();
    fgBrightnessFractmodifier->CreateRenderModifier();
    auto bgBrightnessParamsmodifier = std::make_shared<RSBgBrightnessRatesModifier>(property);
    bgBrightnessParamsmodifier->GetModifierType();
    bgBrightnessParamsmodifier->CreateRenderModifier();
    auto bgBrightnessFractmodifier = std::make_shared<RSBgBrightnessFractModifier>(property);
    bgBrightnessFractmodifier->GetModifierType();
    bgBrightnessFractmodifier->CreateRenderModifier();
    auto emitterUpdatermodifier = std::make_shared<RSEmitterUpdaterModifier>(property);
    emitterUpdatermodifier->GetModifierType();
    emitterUpdatermodifier->CreateRenderModifier();
    auto particleNoiseFieldsmodifier = std::make_shared<RSParticleNoiseFieldsModifier>(property);
    particleNoiseFieldsmodifier->GetModifierType();
    particleNoiseFieldsmodifier->CreateRenderModifier();
    auto dynamicDimDegreemodifier = std::make_shared<RSDynamicDimDegreeModifier>(property);
    dynamicDimDegreemodifier->GetModifierType();
    dynamicDimDegreemodifier->CreateRenderModifier();
    return true;
}

bool DoModifier007(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto linearGradientBlurParamodifier = std::make_shared<RSLinearGradientBlurParaModifier>(property);
    linearGradientBlurParamodifier->GetModifierType();
    linearGradientBlurParamodifier->CreateRenderModifier();
    auto motionBlurParamodifier = std::make_shared<RSMotionBlurParaModifier>(property);
    motionBlurParamodifier->GetModifierType();
    motionBlurParamodifier->CreateRenderModifier();
    auto frameGravitymodifier = std::make_shared<RSFrameGravityModifier>(property);
    frameGravitymodifier->GetModifierType();
    frameGravitymodifier->CreateRenderModifier();
    auto clipRRectmodifier = std::make_shared<RSClipRRectModifier>(property);
    clipRRectmodifier->GetModifierType();
    clipRRectmodifier->CreateRenderModifier();
    auto clipBoundsmodifier = std::make_shared<RSClipBoundsModifier>(property);
    clipBoundsmodifier->GetModifierType();
    clipBoundsmodifier->CreateRenderModifier();
    auto clipToBoundsmodifier = std::make_shared<RSClipToBoundsModifier>(property);
    clipToBoundsmodifier->GetModifierType();
    clipToBoundsmodifier->CreateRenderModifier();
    auto clipToFramemodifier = std::make_shared<RSClipToFrameModifier>(property);
    clipToFramemodifier->GetModifierType();
    clipToFramemodifier->CreateRenderModifier();
    auto visiblemodifier = std::make_shared<RSVisibleModifier>(property);
    visiblemodifier->GetModifierType();
    visiblemodifier->CreateRenderModifier();
    auto spherizemodifier = std::make_shared<RSSpherizeModifier>(property);
    spherizemodifier->GetModifierType();
    spherizemodifier->CreateRenderModifier();
    auto hdrUIBrightnessmodifier = std::make_shared<RSHDRUIBrightnessModifier>(property);
    hdrUIBrightnessmodifier->GetModifierType();
    hdrUIBrightnessmodifier->CreateRenderModifier();
    auto shadowColormodifier = std::make_shared<RSShadowColorModifier>(property);
    shadowColormodifier->GetModifierType();
    shadowColormodifier->CreateRenderModifier();
    return true;
}

bool DoModifier008(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto shadowOffsetXmodifier = std::make_shared<RSShadowOffsetXModifier>(property);
    shadowOffsetXmodifier->GetModifierType();
    shadowOffsetXmodifier->CreateRenderModifier();
    auto shadowOffsetYmodifier = std::make_shared<RSShadowOffsetYModifier>(property);
    shadowOffsetYmodifier->GetModifierType();
    shadowOffsetYmodifier->CreateRenderModifier();
    auto shadowAlphamodifier = std::make_shared<RSShadowAlphaModifier>(property);
    shadowAlphamodifier->GetModifierType();
    shadowAlphamodifier->CreateRenderModifier();
    auto shadowElevationmodifier = std::make_shared<RSShadowElevationModifier>(property);
    shadowElevationmodifier->GetModifierType();
    shadowElevationmodifier->CreateRenderModifier();
    auto shadowRadiusmodifier = std::make_shared<RSShadowRadiusModifier>(property);
    shadowRadiusmodifier->GetModifierType();
    shadowRadiusmodifier->CreateRenderModifier();
    auto shadowPathmodifier = std::make_shared<RSShadowPathModifier>(property);
    shadowPathmodifier->GetModifierType();
    shadowPathmodifier->CreateRenderModifier();
    auto shadowMaskmodifier = std::make_shared<RSShadowMaskModifier>(property);
    shadowMaskmodifier->GetModifierType();
    shadowMaskmodifier->CreateRenderModifier();
    auto shadowIsFilledmodifier = std::make_shared<RSShadowIsFilledModifier>(property);
    shadowIsFilledmodifier->GetModifierType();
    shadowIsFilledmodifier->CreateRenderModifier();
    auto greyCoefmodifier = std::make_shared<RSGreyCoefModifier>(property);
    greyCoefmodifier->GetModifierType();
    greyCoefmodifier->CreateRenderModifier();
    auto maskmodifier = std::make_shared<RSMaskModifier>(property);
    maskmodifier->GetModifierType();
    maskmodifier->CreateRenderModifier();
    auto pixelStretchmodifier = std::make_shared<RSPixelStretchModifier>(property);
    pixelStretchmodifier->GetModifierType();
    pixelStretchmodifier->CreateRenderModifier();
    return true;
}

bool DoModifier009(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto pixelStretchPercentmodifier = std::make_shared<RSPixelStretchPercentModifier>(property);
    pixelStretchPercentmodifier->GetModifierType();
    pixelStretchPercentmodifier->CreateRenderModifier();
    auto pixelStretchTileModemodifier = std::make_shared<RSPixelStretchTileModeModifier>(property);
    pixelStretchTileModemodifier->GetModifierType();
    pixelStretchTileModemodifier->CreateRenderModifier();
    auto grayScalemodifier = std::make_shared<RSGrayScaleModifier>(property);
    grayScalemodifier->GetModifierType();
    grayScalemodifier->CreateRenderModifier();
    auto lightIntensitymodifier = std::make_shared<RSLightIntensityModifier>(property);
    lightIntensitymodifier->GetModifierType();
    lightIntensitymodifier->CreateRenderModifier();
    auto lightColormodifier = std::make_shared<RSLightColorModifier>(property);
    lightColormodifier->GetModifierType();
    lightColormodifier->CreateRenderModifier();
    auto illuminatedTypemodifier = std::make_shared<RSIlluminatedTypeModifier>(property);
    illuminatedTypemodifier->GetModifierType();
    illuminatedTypemodifier->CreateRenderModifier();
    auto lightPositionmodifier = std::make_shared<RSLightPositionModifier>(property);
    lightPositionmodifier->GetModifierType();
    lightPositionmodifier->CreateRenderModifier();
    auto illuminatedBorderWidthmodifier = std::make_shared<RSIlluminatedBorderWidthModifier>(property);
    illuminatedBorderWidthmodifier->GetModifierType();
    illuminatedBorderWidthmodifier->CreateRenderModifier();
    auto bloommodifier = std::make_shared<RSBloomModifier>(property);
    bloommodifier->GetModifierType();
    bloommodifier->CreateRenderModifier();
    auto brightnessmodifier = std::make_shared<RSBrightnessModifier>(property);
    brightnessmodifier->GetModifierType();
    brightnessmodifier->CreateRenderModifier();
    auto contrastmodifier = std::make_shared<RSContrastModifier>(property);
    contrastmodifier->GetModifierType();
    contrastmodifier->CreateRenderModifier();
    return true;
}

bool DoModifier010(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto saturatemodifier = std::make_shared<RSSaturateModifier>(property);
    saturatemodifier->GetModifierType();
    saturatemodifier->CreateRenderModifier();
    auto sepiamodifier = std::make_shared<RSSepiaModifier>(property);
    sepiamodifier->GetModifierType();
    sepiamodifier->CreateRenderModifier();
    auto invertmodifier = std::make_shared<RSInvertModifier>(property);
    invertmodifier->GetModifierType();
    invertmodifier->CreateRenderModifier();
    auto aiInvertmodifier = std::make_shared<RSAiInvertModifier>(property);
    aiInvertmodifier->GetModifierType();
    aiInvertmodifier->CreateRenderModifier();
    auto systemBarEffectmodifier = std::make_shared<RSSystemBarEffectModifier>(property);
    systemBarEffectmodifier->GetModifierType();
    systemBarEffectmodifier->CreateRenderModifier();
    auto hueRotatemodifier = std::make_shared<RSHueRotateModifier>(property);
    hueRotatemodifier->GetModifierType();
    hueRotatemodifier->CreateRenderModifier();
    auto colorBlendmodifier = std::make_shared<RSColorBlendModifier>(property);
    colorBlendmodifier->GetModifierType();
    colorBlendmodifier->CreateRenderModifier();
    auto useEffectmodifier = std::make_shared<RSUseEffectModifier>(property);
    useEffectmodifier->GetModifierType();
    useEffectmodifier->CreateRenderModifier();
    auto useEffectTypeModifier = std::make_shared<RSUseEffectTypeModifier>(property);
    useEffectTypeModifier->GetModifierType();
    useEffectTypeModifier->CreateRenderModifier();
    auto colorBlendModemodifier = std::make_shared<RSColorBlendModeModifier>(property);
    colorBlendModemodifier->GetModifierType();
    colorBlendModemodifier->CreateRenderModifier();
    auto colorBlendApplyTypemodifier = std::make_shared<RSColorBlendApplyTypeModifier>(property);
    colorBlendApplyTypemodifier->GetModifierType();
    colorBlendApplyTypemodifier->CreateRenderModifier();
    auto shadowColorStrategymodifier = std::make_shared<RSShadowColorStrategyModifier>(property);
    shadowColorStrategymodifier->GetModifierType();
    shadowColorStrategymodifier->CreateRenderModifier();
    return true;
}

bool DoModifier011(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto attractionFractionModifier = std::make_shared<RSAttractionFractionModifier>(property);
    attractionFractionModifier->GetModifierType();
    attractionFractionModifier->CreateRenderModifier();
    auto attractionDstPointModifier = std::make_shared<RSAttractionDstPointModifier>(property);
    attractionDstPointModifier->GetModifierType();
    attractionDstPointModifier->CreateRenderModifier();
    auto customClipToFramemodifier = std::make_shared<RSCustomClipToFrameModifier>(property);
    customClipToFramemodifier->GetModifierType();
    customClipToFramemodifier->CreateRenderModifier();
    auto hdrBrightnessModifier = std::make_shared<RSHDRBrightnessModifier>(property);
    hdrBrightnessModifier->GetModifierType();
    hdrBrightnessModifier->CreateRenderModifier();
    auto behindWindowFilterRadiusModifier = std::make_shared<RSBehindWindowFilterRadiusModifier>(property);
    behindWindowFilterRadiusModifier->GetModifierType();
    behindWindowFilterRadiusModifier->CreateRenderModifier();
    auto behindWindowFilterSaturationModifier = std::make_shared<RSBehindWindowFilterSaturationModifier>(property);
    behindWindowFilterSaturationModifier->GetModifierType();
    behindWindowFilterSaturationModifier->CreateRenderModifier();
    auto behindWindowFilterBrightnessModifier = std::make_shared<RSBehindWindowFilterBrightnessModifier>(property);
    behindWindowFilterBrightnessModifier->GetModifierType();
    behindWindowFilterBrightnessModifier->CreateRenderModifier();
    auto behindWindowFilterMaskColorModifier = std::make_shared<RSBehindWindowFilterMaskColorModifier>(property);
    behindWindowFilterMaskColorModifier->GetModifierType();
    behindWindowFilterMaskColorModifier->CreateRenderModifier();
    return true;
}
        
bool DoApply(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    auto geometry = std::make_shared<RSObjAbsGeometry>();
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto rsboundsModifier = std::make_shared<RSBoundsModifier>(property);
    rsboundsModifier->Apply(geometry);
    auto rsboundsSizeModifier = std::make_shared<RSBoundsSizeModifier>(property);
    rsboundsSizeModifier->Apply(geometry);
    auto rsboundsPositionModifier = std::make_shared<RSBoundsPositionModifier>(property);
    rsboundsPositionModifier->Apply(geometry);
    auto rspivotModifier = std::make_shared<RSPivotModifier>(property);
    rspivotModifier->Apply(geometry);
    auto rspivotZModifier = std::make_shared<RSPivotZModifier>(property);
    rspivotZModifier->Apply(geometry);
    auto rsquaternionModifier = std::make_shared<RSQuaternionModifier>(property);
    rsquaternionModifier->Apply(geometry);
    auto rsrotationModifier = std::make_shared<RSRotationModifier>(property);
    rsrotationModifier->Apply(geometry);
    auto rsrotationXModifier = std::make_shared<RSRotationXModifier>(property);
    rsrotationXModifier->Apply(geometry);
    auto rsrotationYModifier = std::make_shared<RSRotationYModifier>(property);
    rsrotationYModifier->Apply(geometry);
    auto rscameraDistanceModifier = std::make_shared<RSCameraDistanceModifier>(property);
    rscameraDistanceModifier->Apply(geometry);
    auto rsscaleModifier = std::make_shared<RSScaleModifier>(property);
    rsscaleModifier->Apply(geometry);
    auto rsskewModifier = std::make_shared<RSSkewModifier>(property);
    rsskewModifier->Apply(geometry);
    auto rsperspModifier = std::make_shared<RSPerspModifier>(property);
    rsperspModifier->Apply(geometry);
    auto rstranslateModifier = std::make_shared<RSTranslateModifier>(property);
    rstranslateModifier->Apply(geometry);
    auto rstranslateZModifier = std::make_shared<RSTranslateZModifier>(property);
    rstranslateZModifier->Apply(geometry);
    return true ;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoModifier001(data, size);
    OHOS::Rosen::DoModifier002(data, size);
    OHOS::Rosen::DoModifier003(data, size);
    OHOS::Rosen::DoModifier004(data, size);
    OHOS::Rosen::DoModifier005(data, size);
    OHOS::Rosen::DoModifier006(data, size);
    OHOS::Rosen::DoModifier007(data, size);
    OHOS::Rosen::DoModifier008(data, size);
    OHOS::Rosen::DoModifier009(data, size);
    OHOS::Rosen::DoModifier010(data, size);
    OHOS::Rosen::DoModifier011(data, size);
    OHOS::Rosen::DoApply(data, size);
    return 0;
}

