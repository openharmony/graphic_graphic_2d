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
#include "ui_effect/mask/include/wave_gradient_mask_para.h"
#include "ui_effect/mask/include/image_mask_para.h"
#include "ui_effect/mask/include/use_effect_mask_para.h"

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
    {RSNGEffectType::FRAME_GRADIENT_MASK, [] {
            return std::make_shared<RSNGFrameGradientMask>();
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

}

thread_local std::unordered_map<MaskPara::Type, MaskConvertor> convertorLUT = {
    { MaskPara::Type::RIPPLE_MASK, ConvertRippleMaskPara },
    { MaskPara::Type::PIXEL_MAP_MASK, ConvertPixelMapMaskPara },
    { MaskPara::Type::RADIAL_GRADIENT_MASK, ConvertRadialGradientMaskPara },
    { MaskPara::Type::WAVE_GRADIENT_MASK, ConvertWaveGradientMaskPara },
    { MaskPara::Type::IMAGE_MASK, ConvertImageMaskPara},
    { MaskPara::Type::USE_EFFECT_MASK, ConvertUseEffectMaskPara },
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
