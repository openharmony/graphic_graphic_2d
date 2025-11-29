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

#include "ui_effect/property/include/rs_ui_filter_base.h"

#include <unordered_set>

#include "ui_effect/filter/include/filter_bezier_warp_para.h"
#include "ui_effect/filter/include/filter_blur_para.h"
#include "ui_effect/filter/include/filter_color_gradient_para.h"
#include "ui_effect/filter/include/filter_content_light_para.h"
#include "ui_effect/filter/include/filter_direction_light_para.h"
#include "ui_effect/filter/include/filter_dispersion_para.h"
#include "ui_effect/filter/include/filter_displacement_distort_para.h"
#include "ui_effect/filter/include/filter_frosted_glass_blur_para.h"
#include "ui_effect/filter/include/filter_frosted_glass_para.h"
#include "ui_effect/filter/include/filter_gasify_blur_para.h"
#include "ui_effect/filter/include/filter_gasify_para.h"
#include "ui_effect/filter/include/filter_gasify_scale_twist_para.h"
#include "ui_effect/filter/include/filter_edge_light_para.h"
#include "ui_effect/filter/include/filter_mask_transition_para.h"
#include "ui_effect/filter/include/filter_variable_radius_blur_para.h"

#include "ui_effect/property/include/rs_ui_color_gradient_filter.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"

#include "platform/common/rs_log.h"

#undef LOG_TAG
#define LOG_TAG "RSNGFilterBase"

namespace OHOS {
namespace Rosen {
using FilterCreator = std::function<std::shared_ptr<RSNGFilterBase>()>;
using FilterConvertor = std::function<std::shared_ptr<RSNGFilterBase>(std::shared_ptr<FilterPara>)>;

static std::unordered_map<RSNGEffectType, FilterCreator> creatorLUT = {
    {RSNGEffectType::BLUR, [] {
            return std::make_shared<RSNGBlurFilter>();
        }
    },
    {RSNGEffectType::DISPLACEMENT_DISTORT, [] {
            return std::make_shared<RSNGDispDistortFilter>();
        }
    },
    {RSNGEffectType::SOUND_WAVE, [] {
            return std::make_shared<RSNGSoundWaveFilter>();
        }
    },
    {RSNGEffectType::DISPERSION, [] {
            return std::make_shared<RSNGDispersionFilter>();
        }
    },
    {RSNGEffectType::EDGE_LIGHT, [] {
            return std::make_shared<RSNGEdgeLightFilter>();
        }
    },
    {RSNGEffectType::COLOR_GRADIENT, [] {
            return std::make_shared<RSNGColorGradientFilter>();
        }
    },
    {RSNGEffectType::DIRECTION_LIGHT, [] {
            return std::make_shared<RSNGDirectionLightFilter>();
        }
    },
    {RSNGEffectType::MASK_TRANSITION, [] {
            return std::make_shared<RSNGMaskTransitionFilter>();
        }
    },
    {RSNGEffectType::VARIABLE_RADIUS_BLUR, [] {
            return std::make_shared<RSNGVariableRadiusBlurFilter>();
        }
    },
    {RSNGEffectType::BEZIER_WARP, [] {
            return std::make_shared<RSNGBezierWarpFilter>();
        }
    },
    {RSNGEffectType::CONTENT_LIGHT, [] {
            return std::make_shared<RSNGContentLightFilter>();
        }
    },
    {RSNGEffectType::GASIFY_SCALE_TWIST, [] {
            return std::make_shared<RSNGGasifyScaleTwistFilter>();
        }
    },
    {RSNGEffectType::GASIFY_BLUR, [] {
            return std::make_shared<RSNGGasifyBlurFilter>();
        }
    },
    {RSNGEffectType::GASIFY, [] {
            return std::make_shared<RSNGGasifyFilter>();
        }
    },
    {RSNGEffectType::FROSTED_GLASS, [] {
            return std::make_shared<RSNGFrostedGlassFilter>();
        }
    },
    {RSNGEffectType::GRID_WARP, [] {
            return std::make_shared<RSNGGridWarpFilter>();
        }
    },
    {RSNGEffectType::FROSTED_GLASS_BLUR, [] {
            return std::make_shared<RSNGFrostedGlassBlurFilter>();
        }
    },
};

namespace {
std::shared_ptr<RSNGFilterBase> ConvertDisplacementDistortFilterPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::DISPLACEMENT_DISTORT);
    if (filter == nullptr) {
        return nullptr;
    }
    auto dispDistortFilter = std::static_pointer_cast<RSNGDispDistortFilter>(filter);
    auto dispDistortFilterPara = std::static_pointer_cast<DisplacementDistortPara>(filterPara);
    dispDistortFilter->Setter<DispDistortFactorTag>(dispDistortFilterPara->GetFactor());
    dispDistortFilter->Setter<DispDistortMaskTag>(RSNGMaskBase::Create(dispDistortFilterPara->GetMask()));
    return dispDistortFilter;
}

std::shared_ptr<RSNGFilterBase> ConvertGasifyScaleTwistPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::GASIFY_SCALE_TWIST);
    if (filter == nullptr) {
        return nullptr;
    }
    auto gasifyScaleTwistFilter = std::static_pointer_cast<RSNGGasifyScaleTwistFilter>(filter);
    auto gasifyScaleTwistFilterPara = std::static_pointer_cast<GasifyScaleTwistPara>(filterPara);
    gasifyScaleTwistFilter->Setter<GasifyScaleTwistProgressTag>(gasifyScaleTwistFilterPara->GetProgress());
    gasifyScaleTwistFilter->Setter<GasifyScaleTwistSourceImageTag>(gasifyScaleTwistFilterPara->GetSourceImage());
    gasifyScaleTwistFilter->Setter<GasifyScaleTwistScaleTag>(gasifyScaleTwistFilterPara->GetScale());
    gasifyScaleTwistFilter->Setter<GasifyScaleTwistMaskTag>(gasifyScaleTwistFilterPara->GetMaskImage());
    return gasifyScaleTwistFilter;
}

std::shared_ptr<RSNGFilterBase> ConvertGasifyBlurPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::GASIFY_BLUR);
    if (filter == nullptr) {
        return nullptr;
    }
    auto gasifyBlurFilter = std::static_pointer_cast<RSNGGasifyBlurFilter>(filter);
    auto gasifyBlurFilterPara = std::static_pointer_cast<GasifyBlurPara>(filterPara);
    gasifyBlurFilter->Setter<GasifyBlurProgressTag>(gasifyBlurFilterPara->GetProgress());
    gasifyBlurFilter->Setter<GasifyBlurSourceImageTag>(gasifyBlurFilterPara->GetSourceImage());
    gasifyBlurFilter->Setter<GasifyBlurMaskTag>(gasifyBlurFilterPara->GetMaskImage());
    return gasifyBlurFilter;
}

std::shared_ptr<RSNGFilterBase> ConvertGasifyPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::GASIFY);
    if (filter == nullptr) {
        return nullptr;
    }
    auto gasifyFilter = std::static_pointer_cast<RSNGGasifyFilter>(filter);
    auto gasifyFilterPara = std::static_pointer_cast<GasifyPara>(filterPara);
    gasifyFilter->Setter<GasifyProgressTag>(gasifyFilterPara->GetProgress());
    gasifyFilter->Setter<GasifySourceImageTag>(gasifyFilterPara->GetSourceImage());
    gasifyFilter->Setter<GasifyMaskTag>(gasifyFilterPara->GetMaskImage());
    return gasifyFilter;
}

std::shared_ptr<RSNGFilterBase> ConvertEdgeLightFilterPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::EDGE_LIGHT);
    if (filter == nullptr) {
        return nullptr;
    }
    auto edgeLightFilter = std::static_pointer_cast<RSNGEdgeLightFilter>(filter);
    auto edgeLightFilterPara = std::static_pointer_cast<EdgeLightPara>(filterPara);
    edgeLightFilter->Setter<EdgeLightColorTag>(edgeLightFilterPara->GetColor());
    edgeLightFilter->Setter<EdgeLightAlphaTag>(edgeLightFilterPara->GetAlpha());
    edgeLightFilter->Setter<EdgeLightMaskTag>(RSNGMaskBase::Create(edgeLightFilterPara->GetMask()));
    edgeLightFilter->Setter<EdgeLightBloomTag>(edgeLightFilterPara->GetBloom());
    edgeLightFilter->Setter<EdgeLightUseRawColorTag>(edgeLightFilterPara->GetUseRawColor());
    return edgeLightFilter;
}

std::shared_ptr<RSNGFilterBase> ConvertDispersionFilterPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::DISPERSION);
    if (filter == nullptr || filterPara == nullptr) {
        return nullptr;
    }
    auto dispersionFilter = std::static_pointer_cast<RSNGDispersionFilter>(filter);
    auto dispersionFilterPara = std::static_pointer_cast<DispersionPara>(filterPara);
    dispersionFilter->Setter<DispersionMaskTag>(RSNGMaskBase::Create(dispersionFilterPara->GetMask()));
    dispersionFilter->Setter<DispersionOpacityTag>(dispersionFilterPara->GetOpacity());
    dispersionFilter->Setter<DispersionRedOffsetTag>(dispersionFilterPara->GetRedOffset());
    dispersionFilter->Setter<DispersionGreenOffsetTag>(dispersionFilterPara->GetGreenOffset());
    dispersionFilter->Setter<DispersionBlueOffsetTag>(dispersionFilterPara->GetBlueOffset());
    return dispersionFilter;
}

std::shared_ptr<RSNGFilterBase> ConvertColorGradientFilterPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::COLOR_GRADIENT);
    if (filter == nullptr || filterPara == nullptr) {
        return nullptr;
    }
    auto colorGradientFilter = std::static_pointer_cast<RSNGColorGradientFilter>(filter);
    auto colorGradientFilterPara = std::static_pointer_cast<ColorGradientPara>(filterPara);
    colorGradientFilter->Setter<ColorGradientColorsTag>(colorGradientFilterPara->GetColors());
    colorGradientFilter->Setter<ColorGradientPositionsTag>(colorGradientFilterPara->GetPositions());
    colorGradientFilter->Setter<ColorGradientStrengthsTag>(colorGradientFilterPara->GetStrengths());
    colorGradientFilter->Setter<ColorGradientMaskTag>(RSNGMaskBase::Create(colorGradientFilterPara->GetMask()));
    return colorGradientFilter;
}

std::shared_ptr<RSNGFilterBase> ConvertDirectionLightFilterPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::DIRECTION_LIGHT);
    bool isInvalid = (filter == nullptr || filterPara == nullptr);
    if (isInvalid) {
        return nullptr;
    }
    auto directionLightFilter = std::static_pointer_cast<RSNGDirectionLightFilter>(filter);
    auto directionLightFilterPara = std::static_pointer_cast<DirectionLightPara>(filterPara);
    directionLightFilter->Setter<DirectionLightMaskTag>(RSNGMaskBase::Create(directionLightFilterPara->GetMask()));
    directionLightFilter->Setter<DirectionLightFactorTag>(directionLightFilterPara->GetMaskFactor());
    directionLightFilter->Setter<DirectionLightDirectionTag>(directionLightFilterPara->GetLightDirection());
    directionLightFilter->Setter<DirectionLightColorTag>(directionLightFilterPara->GetLightColor());
    directionLightFilter->Setter<DirectionLightIntensityTag>(directionLightFilterPara->GetLightIntensity());
    return directionLightFilter;
}

std::shared_ptr<RSNGFilterBase> ConvertMaskTransitionFilterPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::MASK_TRANSITION);
    bool isInvalid = (filter == nullptr || filterPara == nullptr);
    if (isInvalid) {
        return nullptr;
    }
    auto maskTransitionFilter = std::static_pointer_cast<RSNGMaskTransitionFilter>(filter);
    auto maskTransitionFilterPara = std::static_pointer_cast<MaskTransitionPara>(filterPara);
    maskTransitionFilter->Setter<MaskTransitionMaskTag>(RSNGMaskBase::Create(maskTransitionFilterPara->GetMask()));
    maskTransitionFilter->Setter<MaskTransitionFactorTag>(maskTransitionFilterPara->GetFactor());
    maskTransitionFilter->Setter<MaskTransitionInverseTag>(maskTransitionFilterPara->GetInverse());
    return maskTransitionFilter;
}

std::shared_ptr<RSNGFilterBase> ConvertVariableRadiusBlurFilterPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::VARIABLE_RADIUS_BLUR);
    if (filter == nullptr || filterPara == nullptr) {
        return nullptr;
    }
    auto variableRadiusBlurFilter = std::static_pointer_cast<RSNGVariableRadiusBlurFilter>(filter);
    auto variableRadiusBlurFilterPara = std::static_pointer_cast<VariableRadiusBlurPara>(filterPara);
    variableRadiusBlurFilter->Setter<VariableRadiusBlurRadiusTag>(variableRadiusBlurFilterPara->GetBlurRadius());
    variableRadiusBlurFilter->Setter<VariableRadiusBlurMaskTag>(
        RSNGMaskBase::Create(variableRadiusBlurFilterPara->GetMask()));
    return variableRadiusBlurFilter;
}

std::shared_ptr<RSNGFilterBase> ConvertBezierWarpFilterPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::BEZIER_WARP);
    if (filter == nullptr || filterPara == nullptr) {
        return nullptr;
    }
    auto bezierWarpFilter = std::static_pointer_cast<RSNGBezierWarpFilter>(filter);
    auto bezierWarpFilterPara = std::static_pointer_cast<BezierWarpPara>(filterPara);
    auto& bezierCtrlPoints = bezierWarpFilterPara->GetBezierControlPoints();
    // note: the order of values has to be same with the order of ADD_PROPERTY_ATG in DECLARE_TILTER
    const auto& values = std::apply([](auto&... args) { return std::tie(args...); }, bezierCtrlPoints);
    bezierWarpFilter->Setter(values);
    return bezierWarpFilter;
}

std::shared_ptr<RSNGFilterBase> ConvertContentLightFilterPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::CONTENT_LIGHT);
    if (filter == nullptr || filterPara == nullptr) {
        ROSEN_LOGE("ConvertContentLightFilterPara filter or filterPara is nullptr");
        return nullptr;
    }
    auto contentLightFilter = std::static_pointer_cast<RSNGContentLightFilter>(filter);
    auto contentLightFilterPara = std::static_pointer_cast<ContentLightPara>(filterPara);
    contentLightFilter->Setter<ContentLightPositionTag>(contentLightFilterPara->GetLightPosition());
    contentLightFilter->Setter<ContentLightColorTag>(contentLightFilterPara->GetLightColor());
    contentLightFilter->Setter<ContentLightIntensityTag>(contentLightFilterPara->GetLightIntensity());
    return contentLightFilter;
}

std::shared_ptr<RSNGFilterBase> ConvertFrostedGlassPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::FROSTED_GLASS);
    if (filter == nullptr || filterPara == nullptr) {
        ROSEN_LOGE("ConvertFrostedGlassPara filter or filterPara is nullptr");
        return nullptr;
    }
    auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);
    auto frostedGlassFilterPara = std::static_pointer_cast<FrostedGlassPara>(filterPara);
    frostedGlassFilter->Setter<FrostedGlassBorderSizeTag>(frostedGlassFilterPara->GetBorderSize());
    frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(frostedGlassFilterPara->GetBlurParams());
    frostedGlassFilter->Setter<FrostedGlassCornerRadiusTag>(frostedGlassFilterPara->GetCornerRadius());
    frostedGlassFilter->Setter<FrostedGlassBorderWidthTag>(frostedGlassFilterPara->GetBorderWidth());
    frostedGlassFilter->Setter<FrostedGlassOffsetTag>(frostedGlassFilterPara->GetOffset());
    frostedGlassFilter->Setter<FrostedGlassDownSampleFactorTag>(frostedGlassFilterPara->GetDownSampleFactor());
    frostedGlassFilter->Setter<FrostedGlassBgFactorTag>(frostedGlassFilterPara->GetBgFactor());
    frostedGlassFilter->Setter<FrostedGlassInnerShadowParamsTag>(frostedGlassFilterPara->GetInnerShadowParams());
    frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(frostedGlassFilterPara->GetSdParams());
    frostedGlassFilter->Setter<FrostedGlassRefractOutPxTag>(frostedGlassFilterPara->GetRefractOutPx());
    frostedGlassFilter->Setter<FrostedGlassEnvParamsTag>(frostedGlassFilterPara->GetEnvParams());
    frostedGlassFilter->Setter<FrostedGlassEdgeLightAngleParamsTag>(frostedGlassFilterPara->GetEdgeLightAngleParams());
    frostedGlassFilter->Setter<FrostedGlassEdgeLightBlurParamsTag>(frostedGlassFilterPara->GetEdgeLightBlurParams());
    frostedGlassFilter->Setter<FrostedGlassEdgeLightDirParamsTag>(frostedGlassFilterPara->GetEdgeLightDirParams());
    frostedGlassFilter->Setter<FrostedGlassHlParamsTag>(frostedGlassFilterPara->GetHlParams());
    return frostedGlassFilter;
}

std::shared_ptr<RSNGFilterBase> ConvertFrostedGlassBlurPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::FROSTED_GLASS_BLUR);
    if (filter == nullptr || filterPara == nullptr) {
        ROSEN_LOGE("ConvertFrostedGlassPara filter or filterPara is nullptr");
        return nullptr;
    }
    auto frostedGlassBlurFilter = std::static_pointer_cast<RSNGFrostedGlassBlurFilter>(filter);
    auto frostedGlassBlurFilterPara = std::static_pointer_cast<FrostedGlassBlurPara>(filterPara);
    frostedGlassBlurFilter->Setter<FrostedGlassBlurRadiusTag>(frostedGlassBlurFilterPara->GetBlurRadius());
    frostedGlassBlurFilter->Setter<FrostedGlassBlurRefractOutPxTag>(frostedGlassBlurFilterPara->GetRefractOutPx());
    return frostedGlassBlurFilter;
}
}

static std::unordered_map<FilterPara::ParaType, FilterConvertor> convertorLUT = {
    { FilterPara::ParaType::DISPLACEMENT_DISTORT, ConvertDisplacementDistortFilterPara },
    { FilterPara::ParaType::EDGE_LIGHT, ConvertEdgeLightFilterPara },
    { FilterPara::ParaType::DISPERSION, ConvertDispersionFilterPara },
    { FilterPara::ParaType::COLOR_GRADIENT, ConvertColorGradientFilterPara },
    { FilterPara::ParaType::DIRECTION_LIGHT, ConvertDirectionLightFilterPara },
    { FilterPara::ParaType::MASK_TRANSITION, ConvertMaskTransitionFilterPara },
    { FilterPara::ParaType::VARIABLE_RADIUS_BLUR, ConvertVariableRadiusBlurFilterPara },
    { FilterPara::ParaType::BEZIER_WARP, ConvertBezierWarpFilterPara },
    { FilterPara::ParaType::CONTENT_LIGHT, ConvertContentLightFilterPara },
    { FilterPara::ParaType::GASIFY_SCALE_TWIST, ConvertGasifyScaleTwistPara },
    { FilterPara::ParaType::GASIFY_BLUR, ConvertGasifyBlurPara },
    { FilterPara::ParaType::GASIFY, ConvertGasifyPara },
    { FilterPara::ParaType::FROSTED_GLASS, ConvertFrostedGlassPara },
    { FilterPara::ParaType::FROSTED_GLASS_BLUR, ConvertFrostedGlassBlurPara },
};

std::shared_ptr<RSNGFilterBase> RSNGFilterBase::Create(RSNGEffectType type)
{
    auto it = creatorLUT.find(type);
    return it != creatorLUT.end() ? it->second() : nullptr;
}

std::shared_ptr<RSNGFilterBase> RSNGFilterBase::Create(std::shared_ptr<FilterPara> filterPara)
{
    if (!filterPara) {
        return nullptr;
    }

    auto it = convertorLUT.find(filterPara->GetParaType());
    return it != convertorLUT.end() ? it->second(filterPara) : nullptr;
}

} // namespace Rosen
} // namespace OHOS
