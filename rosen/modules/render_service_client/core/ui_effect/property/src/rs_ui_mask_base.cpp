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

#include "ui_effect/property/include/rs_ui_mask_base.h"

#include "platform/common/rs_log.h"
#include "pixel_map.h"
#include "ui_effect/mask/include/pixel_map_mask_para.h"
#include "ui_effect/mask/include/radial_gradient_mask_para.h"
#include "ui_effect/mask/include/ripple_mask_para.h"
#include "ui_effect/mask/include/warped_ring_mask_para.h"
#include "ui_effect/mask/include/wave_gradient_mask_para.h"
#include "ui_effect/mask/include/image_mask_para.h"
#include "ui_effect/mask/include/use_effect_mask_para.h"
#include "ui_effect/mask/include/wave_disturbance_mask_para.h"
#include "ui_effect/mask/include/fractal_glass_mask_para.h"
#include "ui_effect/mask/include/binocular_mask_para.h"
#include "ui_effect/mask/include/sweep_refraction_mask_para.h"

#undef LOG_TAG
#define LOG_TAG "RSNGMaskBase"

namespace OHOS {
namespace Rosen {
using MaskCreator = std::function<std::shared_ptr<RSNGMaskBase>()>;
using MaskConvertor = std::function<std::shared_ptr<RSNGMaskBase>(std::shared_ptr<MaskPara>)>;

thread_local std::unordered_map<RSNGEffectType, MaskCreator> creatorLUT = {
    {RSNGEffectType::RIPPLE_MASK, [] {
            return std::make_shared<RSNGRippleMask>();
        }
    },
    {RSNGEffectType::WARPED_RING_MASK, [] {
            return std::make_shared<RSNGWarpedRingMask>();
        }
    },
    {RSNGEffectType::PIXEL_MAP_MASK, [] {
            return std::make_shared<RSNGPixelMapMask>();
        }
    },
    {RSNGEffectType::RADIAL_GRADIENT_MASK, [] {
            return std::make_shared<RSNGRadialGradientMask>();
        }
    },
    {RSNGEffectType::WAVE_GRADIENT_MASK, [] {
            return std::make_shared<RSNGWaveGradientMask>();
        }
    },
    {RSNGEffectType::WAVE_DISTURBANCE_MASK, [] {
 	        return std::make_shared<RSNGWaveDisturbanceMask>();
 	    }
 	},
    {RSNGEffectType::IMAGE_MASK, [] {
            return std::make_shared<RSNGImageMask>();
        }
    },
    {RSNGEffectType::USE_EFFECT_MASK, [] {
            return std::make_shared<RSNGUseEffectMask>();
        }
    },
    {RSNGEffectType::FRAME_GRADIENT_MASK, [] {
            return std::make_shared<RSNGFrameGradientMask>();
        }
    },
    {RSNGEffectType::DUPOLI_NOISE_MASK, [] {
            return std::make_shared<RSNGDupoliNoiseMask>();
        }
    },
    {RSNGEffectType::NOISY_FRAME_GRADIENT_MASK, [] {
            return std::make_shared<RSNGNoisyFrameGradientMask>();
        }
    },
    {RSNGEffectType::FRACTAL_GLASS_MASK, [] {
            return std::make_shared<RSNGFractalGlassMask>();
        }
    },
    {RSNGEffectType::BINOCULAR_MASK, [] {
            return std::make_shared<RSNGBinocularMask>();
        }
    },
    {RSNGEffectType::SWEEP_REFRACTION_MASK, [] {
            return std::make_shared<RSNGSweepRefractionMask>();
        }
    },
};

namespace {
std::shared_ptr<RSNGMaskBase> ConvertRippleMaskPara(std::shared_ptr<MaskPara> maskPara)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::RIPPLE_MASK);
    if (mask == nullptr) {
        return nullptr;
    }
    auto rippleMask = std::static_pointer_cast<RSNGRippleMask>(mask);
    auto rippleMaskPara = std::static_pointer_cast<RippleMaskPara>(maskPara);
    rippleMask->Setter<RippleMaskRadiusTag>(rippleMaskPara->GetRadius());
    rippleMask->Setter<RippleMaskWidthTag>(rippleMaskPara->GetWidth());
    rippleMask->Setter<RippleMaskCenterTag>(rippleMaskPara->GetCenter());
    rippleMask->Setter<RippleMaskOffsetTag>(rippleMaskPara->GetWidthCenterOffset());
    return rippleMask;
}

std::shared_ptr<RSNGMaskBase> ConvertWarpedRingMaskPara(std::shared_ptr<MaskPara> maskPara)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::WARPED_RING_MASK);
    if (mask == nullptr) {
        return nullptr;
    }
    auto warpedRingMask = std::static_pointer_cast<RSNGWarpedRingMask>(mask);
    auto warpedRingMaskPara = std::static_pointer_cast<WarpedRingMaskPara>(maskPara);
    const auto& ringParam = warpedRingMaskPara->GetRingParam();
    warpedRingMask->Setter<WarpedRingMaskRadiusTag>(ringParam.radius);
    warpedRingMask->Setter<WarpedRingMaskBaseHalfWidthTag>(ringParam.baseHalfWidth);
    warpedRingMask->Setter<WarpedRingMaskWidthVariationTag>(ringParam.widthVariation);
    warpedRingMask->Setter<WarpedRingMaskRotateAngleTag>(ringParam.rotateAngle);
    warpedRingMask->Setter<WarpedRingMaskRotate3DProgressTag>(ringParam.rotate3DProgress);
    warpedRingMask->Setter<WarpedRingMaskNoiseEvolutionTag>(ringParam.noiseEvolution);
    return warpedRingMask;
}

std::shared_ptr<RSNGMaskBase> ConvertPixelMapMaskPara(std::shared_ptr<MaskPara> maskPara)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::PIXEL_MAP_MASK);
    if (mask == nullptr) {
        return nullptr;
    }
    auto pixelMapMask = std::static_pointer_cast<RSNGPixelMapMask>(mask);
    auto pixelMapMaskPara = std::static_pointer_cast<PixelMapMaskPara>(maskPara);
    pixelMapMask->Setter<PixelMapMaskSrcTag>(pixelMapMaskPara->GetSrc());
    pixelMapMask->Setter<PixelMapMaskDstTag>(pixelMapMaskPara->GetDst());
    pixelMapMask->Setter<PixelMapMaskFillColorTag>(pixelMapMaskPara->GetFillColor());
    pixelMapMask->Setter<PixelMapMaskImageTag>(pixelMapMaskPara->GetPixelMap());
    return pixelMapMask;
}

std::shared_ptr<RSNGMaskBase> ConvertImageMaskPara(std::shared_ptr<MaskPara> maskPara)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::IMAGE_MASK);
    if (mask == nullptr) {
        return nullptr;
    }
    auto imageMask = std::static_pointer_cast<RSNGImageMask>(mask);
    auto imageMaskPara = std::static_pointer_cast<ImageMaskPara>(maskPara);
    imageMask->Setter<ImageMaskImageTag>(imageMaskPara->GetPixelMap());
    return imageMask;
}

std::shared_ptr<RSNGMaskBase> ConvertRadialGradientMaskPara(std::shared_ptr<MaskPara> maskPara)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::RADIAL_GRADIENT_MASK);
    if (mask == nullptr) {
        return nullptr;
    }
    auto radialGradientMask = std::static_pointer_cast<RSNGRadialGradientMask>(mask);
    auto radialGradientMaskPara = std::static_pointer_cast<RadialGradientMaskPara>(maskPara);
    radialGradientMask->Setter<RadialGradientMaskCenterTag>(radialGradientMaskPara->GetCenter());
    radialGradientMask->Setter<RadialGradientMaskRadiusXTag>(radialGradientMaskPara->GetRadiusX());
    radialGradientMask->Setter<RadialGradientMaskRadiusYTag>(radialGradientMaskPara->GetRadiusY());
    radialGradientMask->Setter<RadialGradientMaskColorsTag>(radialGradientMaskPara->GetColors());
    radialGradientMask->Setter<RadialGradientMaskPositionsTag>(radialGradientMaskPara->GetPositions());
    return radialGradientMask;
}

std::shared_ptr<RSNGMaskBase> ConvertWaveGradientMaskPara(std::shared_ptr<MaskPara> maskPara)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::WAVE_GRADIENT_MASK);
    if (mask == nullptr) {
        return nullptr;
    }
    auto waveGradientMask = std::static_pointer_cast<RSNGWaveGradientMask>(mask);
    auto waveGradientMaskPara = std::static_pointer_cast<WaveGradientMaskPara>(maskPara);
    waveGradientMask->Setter<WaveGradientMaskWaveCenterTag>(waveGradientMaskPara->GetWaveCenter());
    waveGradientMask->Setter<WaveGradientMaskWaveWidthTag>(waveGradientMaskPara->GetWaveWidth());
    waveGradientMask->Setter<WaveGradientMaskPropagationRadiusTag>(waveGradientMaskPara->GetPropagationRadius());
    waveGradientMask->Setter<WaveGradientMaskBlurRadiusTag>(waveGradientMaskPara->GetBlurRadius());
    waveGradientMask->Setter<WaveGradientMaskTurbulenceStrengthTag>(waveGradientMaskPara->GetTurbulenceStrength());

    return waveGradientMask;
}

std::shared_ptr<RSNGMaskBase> ConvertWaveDisturbanceMaskPara(std::shared_ptr<MaskPara> maskPara)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::WAVE_DISTURBANCE_MASK);
    if (mask == nullptr) {
        return nullptr;
    }
    auto waveDisturbanceMask = std::static_pointer_cast<RSNGWaveDisturbanceMask>(mask);
    auto waveDisturbanceMaskPara = std::static_pointer_cast<WaveDisturbanceMaskPara>(maskPara);
    waveDisturbanceMask->Setter<WaveDisturbanceMaskProgressTag>(waveDisturbanceMaskPara->GetProgress());
    waveDisturbanceMask->Setter<WaveDisturbanceMaskClickPosTag>(waveDisturbanceMaskPara->GetClickPos());
    waveDisturbanceMask->Setter<WaveDisturbanceMaskWaveRDTag>(waveDisturbanceMaskPara->GetWaveRD());
    waveDisturbanceMask->Setter<WaveDisturbanceMaskWaveLWHTag>(waveDisturbanceMaskPara->GetWaveLWH());
    return waveDisturbanceMask;
}

std::shared_ptr<RSNGMaskBase> ConvertUseEffectMaskPara(std::shared_ptr<MaskPara> maskPara)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::USE_EFFECT_MASK);
    if (mask == nullptr) {
        return nullptr;
    }
    auto useEffectMask = std::static_pointer_cast<RSNGUseEffectMask>(mask);
    auto useEffectMaskPara = std::static_pointer_cast<UseEffectMaskPara>(maskPara);
    useEffectMask->Setter<UseEffectMaskImageTag>(useEffectMaskPara->GetPixelMap());
    useEffectMask->Setter<UseEffectMaskUseEffectTag>(useEffectMaskPara->GetUseEffect());
    return useEffectMask;
}

std::shared_ptr<RSNGMaskBase> ConvertFractalGlassMaskPara(std::shared_ptr<MaskPara> maskPara)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::FRACTAL_GLASS_MASK);
    if (mask == nullptr) {
        ROSEN_LOGE("[ui_effect] ConvertFractalGlassMaskPara Create mask failed");
        return nullptr;
    }
    auto fractalGlassMask = std::static_pointer_cast<RSNGFractalGlassMask>(mask);
    auto fractalGlassMaskPara = std::static_pointer_cast<FractalGlassMaskPara>(maskPara);
    fractalGlassMask->Setter<FractalGlassMaskImageTag>(fractalGlassMaskPara->GetPixelMap());
    fractalGlassMask->Setter<FractalGlassMaskSrcTag>(fractalGlassMaskPara->GetSrc());
    fractalGlassMask->Setter<FractalGlassMaskDstTag>(fractalGlassMaskPara->GetDst());
    fractalGlassMask->Setter<FractalGlassMaskNumTag>(fractalGlassMaskPara->GetGlassNum());
    fractalGlassMask->Setter<FractalGlassMaskStrengthTag>(fractalGlassMaskPara->GetGlassStrength());
    fractalGlassMask->Setter<FractalGlassMaskSoftnessTag>(fractalGlassMaskPara->GetGlassSoftness());
    fractalGlassMask->Setter<FractalGlassMaskSymmetricTag>(fractalGlassMaskPara->GetIsSymmetric());
    return fractalGlassMask;
}

std::shared_ptr<RSNGMaskBase> ConvertBinocularMaskPara(std::shared_ptr<MaskPara> maskPara)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::BINOCULAR_MASK);
    if (mask == nullptr) {
        ROSEN_LOGE("[ui_effect] ConvertBinocularMaskPara Create mask failed");
        return nullptr;
    }
    auto binocularMask = std::static_pointer_cast<RSNGBinocularMask>(mask);
    auto binocularMaskPara = std::static_pointer_cast<BinocularMaskPara>(maskPara);
    binocularMask->Setter<BinocularMaskRadiusXTag>(binocularMaskPara->GetRadiusX());
    binocularMask->Setter<BinocularMaskRadiusYTag>(binocularMaskPara->GetRadiusY());
    binocularMask->Setter<BinocularMaskGapTag>(binocularMaskPara->GetGap());
    binocularMask->Setter<BinocularMaskSoftnessTag>(binocularMaskPara->GetSoftness());
    return mask;
}

std::shared_ptr<RSNGMaskBase> ConvertSweepRefractionMaskPara(std::shared_ptr<MaskPara> maskPara)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::SWEEP_REFRACTION_MASK);
    if (mask == nullptr) {
        return nullptr;
    }
    auto sweepRefractionMask = std::static_pointer_cast<RSNGSweepRefractionMask>(mask);
    auto sweepRefractionMaskPara = std::static_pointer_cast<SweepRefractionMaskPara>(maskPara);
    sweepRefractionMask->Setter<SweepRefractionMaskMaskRadiusTag>(sweepRefractionMaskPara->GetMaskRadius());
    sweepRefractionMask->Setter<SweepRefractionMaskEdgeThicknessTag>(sweepRefractionMaskPara->GetEdgeThickness());
    sweepRefractionMask->Setter<SweepRefractionMaskRefractAmountTag>(sweepRefractionMaskPara->GetRefractAmount());
    sweepRefractionMask->Setter<SweepRefractionMaskRippleWidthTag>(sweepRefractionMaskPara->GetRippleWidth());
    sweepRefractionMask->Setter<SweepRefractionMaskSweepOffsetTag>(sweepRefractionMaskPara->GetSweepOffset());
    sweepRefractionMask->Setter<SweepRefractionMaskChromaDeltaTag>(sweepRefractionMaskPara->GetChromaDelta());
    sweepRefractionMask->Setter<SweepRefractionMaskShapeTypeTag>(sweepRefractionMaskPara->GetShapeType());
    sweepRefractionMask->Setter<SweepRefractionMaskCornerRadiusTag>(sweepRefractionMaskPara->GetCornerRadius());
    sweepRefractionMask->Setter<SweepRefractionMaskPrismWidthTag>(sweepRefractionMaskPara->GetPrismWidth());
    sweepRefractionMask->Setter<SweepRefractionMaskPrismHeightTag>(sweepRefractionMaskPara->GetPrismHeight());
    sweepRefractionMask->Setter<SweepRefractionMaskSweepCenterXTag>(sweepRefractionMaskPara->GetSweepCenterX());
    sweepRefractionMask->Setter<SweepRefractionMaskSweepCenterYTag>(sweepRefractionMaskPara->GetSweepCenterY());
    return sweepRefractionMask;
}

}

thread_local std::unordered_map<MaskPara::Type, MaskConvertor> convertorLUT = {
    { MaskPara::Type::RIPPLE_MASK, ConvertRippleMaskPara },
    { MaskPara::Type::WARPED_RING_MASK, ConvertWarpedRingMaskPara },
    { MaskPara::Type::PIXEL_MAP_MASK, ConvertPixelMapMaskPara },
    { MaskPara::Type::RADIAL_GRADIENT_MASK, ConvertRadialGradientMaskPara },
    { MaskPara::Type::WAVE_GRADIENT_MASK, ConvertWaveGradientMaskPara },
    { MaskPara::Type::WAVE_DISTURBANCE_MASK, ConvertWaveDisturbanceMaskPara },
    { MaskPara::Type::IMAGE_MASK, ConvertImageMaskPara},
    { MaskPara::Type::USE_EFFECT_MASK, ConvertUseEffectMaskPara },
    { MaskPara::Type::FRACTAL_GLASS_MASK, ConvertFractalGlassMaskPara },
    { MaskPara::Type::BINOCULAR_MASK, ConvertBinocularMaskPara },
    { MaskPara::Type::SWEEP_REFRACTION_MASK, ConvertSweepRefractionMaskPara },
};

std::shared_ptr<RSNGMaskBase> RSNGMaskBase::Create(RSNGEffectType type)
{
    auto it = creatorLUT.find(type);
    return it != creatorLUT.end() ? it->second() : nullptr;
}

std::shared_ptr<RSNGMaskBase> RSNGMaskBase::Create(std::shared_ptr<MaskPara> maskPara)
{
    if (!maskPara) {
        return nullptr;
    }

    auto it = convertorLUT.find(maskPara->GetMaskParaType());
    return it != convertorLUT.end() ? it->second(maskPara) : nullptr;
}
} // namespace Rosen
} // namespace OHOS
