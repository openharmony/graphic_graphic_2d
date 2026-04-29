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
#include "ui_effect/property/include/rs_ui_mask_to_para.h"

#include "ui_effect/mask/include/image_mask_para.h"
#include "ui_effect/mask/include/mask_para.h"
#include "ui_effect/mask/include/pixel_map_mask_para.h"
#include "ui_effect/mask/include/radial_gradient_mask_para.h"
#include "ui_effect/mask/include/ripple_mask_para.h"
#include "ui_effect/mask/include/use_effect_mask_para.h"
#include "ui_effect/mask/include/wave_disturbance_mask_para.h"
#include "ui_effect/mask/include/wave_gradient_mask_para.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/property/include/rs_ui_property_tag.h"

namespace OHOS {
namespace Rosen {

namespace {
std::shared_ptr<MaskPara> ConvertRippleMaskToPara(const RSNGMaskBase& mask)
{
    const auto& rippleMask = static_cast<const RSNGRippleMask&>(mask);
    auto para = std::make_shared<RippleMaskPara>();
    Vector2f center = rippleMask.Getter<RippleMaskCenterTag>()->Get();
    para->SetCenter(center);
    para->SetRadius(rippleMask.Getter<RippleMaskRadiusTag>()->Get());
    para->SetWidth(rippleMask.Getter<RippleMaskWidthTag>()->Get());
    para->SetWidthCenterOffset(rippleMask.Getter<RippleMaskOffsetTag>()->Get());
    return para;
}

std::shared_ptr<MaskPara> ConvertRadialGradientMaskToPara(const RSNGMaskBase& mask)
{
    const auto& radialGradientMask = static_cast<const RSNGRadialGradientMask&>(mask);
    auto para = std::make_shared<RadialGradientMaskPara>();
    Vector2f center = radialGradientMask.Getter<RadialGradientMaskCenterTag>()->Get();
    para->SetCenter(center);
    para->SetRadiusX(radialGradientMask.Getter<RadialGradientMaskRadiusXTag>()->Get());
    para->SetRadiusY(radialGradientMask.Getter<RadialGradientMaskRadiusYTag>()->Get());
    para->SetColors(radialGradientMask.Getter<RadialGradientMaskColorsTag>()->Get());
    para->SetPositions(radialGradientMask.Getter<RadialGradientMaskPositionsTag>()->Get());
    return para;
}

std::shared_ptr<MaskPara> ConvertPixelMapMaskToPara(const RSNGMaskBase& mask)
{
    const auto& pixelMapMask = static_cast<const RSNGPixelMapMask&>(mask);
    auto para = std::make_shared<PixelMapMaskPara>();
    auto pixelMap = pixelMapMask.Getter<PixelMapMaskImageTag>()->Get();
    para->SetPixelMap(pixelMap);
    Vector4f src = pixelMapMask.Getter<PixelMapMaskSrcTag>()->Get();
    para->SetSrc(src);
    Vector4f dst = pixelMapMask.Getter<PixelMapMaskDstTag>()->Get();
    para->SetDst(dst);
    Vector4f fillColor = pixelMapMask.Getter<PixelMapMaskFillColorTag>()->Get();
    para->SetFillColor(fillColor);
    return para;
}

std::shared_ptr<MaskPara> ConvertWaveGradientMaskToPara(const RSNGMaskBase& mask)
{
    const auto& waveGradientMask = static_cast<const RSNGWaveGradientMask&>(mask);
    auto para = std::make_shared<WaveGradientMaskPara>();
    Vector2f waveCenter = waveGradientMask.Getter<WaveGradientMaskWaveCenterTag>()->Get();
    para->SetWaveCenter(waveCenter);
    para->SetWaveWidth(waveGradientMask.Getter<WaveGradientMaskWaveWidthTag>()->Get());
    para->SetPropagationRadius(waveGradientMask.Getter<WaveGradientMaskPropagationRadiusTag>()->Get());
    para->SetBlurRadius(waveGradientMask.Getter<WaveGradientMaskBlurRadiusTag>()->Get());
    para->SetTurbulenceStrength(waveGradientMask.Getter<WaveGradientMaskTurbulenceStrengthTag>()->Get());
    return para;
}

std::shared_ptr<MaskPara> ConvertWaveDisturbanceMaskToPara(const RSNGMaskBase& mask)
{
    const auto& waveDisturbanceMask = static_cast<const RSNGWaveDisturbanceMask&>(mask);
    auto para = std::make_shared<WaveDisturbanceMaskPara>();
    para->SetProgress(waveDisturbanceMask.Getter<WaveDisturbanceMaskProgressTag>()->Get());
    Vector2f clickPos = waveDisturbanceMask.Getter<WaveDisturbanceMaskClickPosTag>()->Get();
    para->SetClickPos(clickPos);
    Vector2f waveRD = waveDisturbanceMask.Getter<WaveDisturbanceMaskWaveRDTag>()->Get();
    para->SetWaveRD(waveRD);
    Vector3f waveLWH = waveDisturbanceMask.Getter<WaveDisturbanceMaskWaveLWHTag>()->Get();
    para->SetWaveLWH(waveLWH);
    return para;
}

std::shared_ptr<MaskPara> ConvertImageMaskToPara(const RSNGMaskBase& mask)
{
    const auto& imageMask = static_cast<const RSNGImageMask&>(mask);
    auto para = std::make_shared<ImageMaskPara>();
    auto pixelMap = imageMask.Getter<ImageMaskImageTag>()->Get();
    para->SetPixelMap(pixelMap);
    return para;
}

std::shared_ptr<MaskPara> ConvertUseEffectMaskToPara(const RSNGMaskBase& mask)
{
    const auto& useEffectMask = static_cast<const RSNGUseEffectMask&>(mask);
    auto para = std::make_shared<UseEffectMaskPara>();
    auto pixelMap = useEffectMask.Getter<UseEffectMaskImageTag>()->Get();
    para->SetPixelMap(pixelMap);
    para->SetUseEffect(useEffectMask.Getter<UseEffectMaskUseEffectTag>()->Get());
    return para;
}
} // namespace

std::shared_ptr<MaskPara> RSNGMaskToParaHelper::ConvertMaskToPara(std::shared_ptr<RSNGMaskBase> mask)
{
    using MaskToParaFunc = std::function<std::shared_ptr<MaskPara>(const RSNGMaskBase&)>;
    static std::unordered_map<RSNGEffectType, MaskToParaFunc> s_maskToParaLUT = {
        { RSNGEffectType::RIPPLE_MASK, ConvertRippleMaskToPara },
        { RSNGEffectType::RADIAL_GRADIENT_MASK, ConvertRadialGradientMaskToPara },
        { RSNGEffectType::PIXEL_MAP_MASK, ConvertPixelMapMaskToPara },
        { RSNGEffectType::WAVE_GRADIENT_MASK, ConvertWaveGradientMaskToPara },
        { RSNGEffectType::WAVE_DISTURBANCE_MASK, ConvertWaveDisturbanceMaskToPara },
        { RSNGEffectType::IMAGE_MASK, ConvertImageMaskToPara },
        { RSNGEffectType::USE_EFFECT_MASK, ConvertUseEffectMaskToPara },
    };
    if (!mask) {
        return nullptr;
    }
    auto it = s_maskToParaLUT.find(mask->GetType());
    return it != s_maskToParaLUT.end() ? it->second(*mask) : nullptr;
}

} // namespace Rosen
} // namespace OHOS