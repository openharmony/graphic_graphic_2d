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

#include "ui_effect/filter/include/filter_blur_para.h"
#include "ui_effect/filter/include/filter_color_gradient_para.h"
#include "ui_effect/filter/include/filter_direction_light_para.h"
#include "ui_effect/filter/include/filter_dispersion_para.h"
#include "ui_effect/filter/include/filter_displacement_distort_para.h"
#include "ui_effect/filter/include/filter_edge_light_para.h"
#include "ui_effect/filter/include/filter_mask_transition_para.h"
#include "ui_effect/filter/include/filter_variable_radius_blur_para.h"
#include "ui_effect/filter/include/filter_bezier_warp_para.h"

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
    {RSNGEffectType::DIRECTION_LIGHT, [] {
            return std::make_shared<RSNGDirectionLightFilter>();
        }
    },
    {RSNGEffectType::COLOR_GRADIENT, [] {
            return std::make_shared<RSNGColorGradientFilter>();
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
};

namespace {
std::shared_ptr<RSNGFilterBase> ConvertDisplacementDistortFilterPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::DISPLACEMENT_DISTORT);
    if (filter == nullptr || filterPara == nullptr) {
        return nullptr;
    }
    auto dispDistortFilter = std::static_pointer_cast<RSNGDispDistortFilter>(filter);
    auto dispDistortFilterPara = std::static_pointer_cast<DisplacementDistortPara>(filterPara);
    dispDistortFilter->Setter<DispDistortFactorTag>(dispDistortFilterPara->GetFactor());
    dispDistortFilter->Setter<DispDistortMaskTag>(RSNGMaskBase::Create(dispDistortFilterPara->GetMask()));
    return dispDistortFilter;
}

std::shared_ptr<RSNGFilterBase> ConvertEdgeLightFilterPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::EDGE_LIGHT);
    if (filter == nullptr || filterPara == nullptr) {
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

std::shared_ptr<RSNGFilterBase> ConvertDirectionLightFilterPara(std::shared_ptr<FilterPara> filterPara)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::DIRECTION_LIGHT);
    if (filter == nullptr || filterPara == nullptr) {
        return nullptr;
    }
    auto directionLightFilter = std::static_pointer_cast<RSNGDirectionLightFilter>(filter);
    auto directionLightFilterPara = std::static_pointer_cast<DirectionLightPara>(filterPara);
    directionLightFilter->Setter<DirectionLightMaskTag>(RSNGMaskBase::Create(directionLightFilterPara->GetMask()));
    directionLightFilter->Setter<DirectionLightDirectionTag>(directionLightFilterPara->GetLightDirection());
    directionLightFilter->Setter<DirectionLightColorTag>(directionLightFilterPara->GetLightColor());
    directionLightFilter->Setter<DirectionLightIntensityTag>(directionLightFilterPara->GetLightIntensity());
    return directionLightFilter;
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
    auto bezierCtrlPoints = bezierWarpFilterPara->GetBezierControlPoints();
    std::array<RSNGEffectType, BEZIER_WARP_POINT_NUM> bezierCtrlPointsNGType = {
        RSNGEffectType::BezierWarpControlPoint0Tag,
        RSNGEffectType::BezierWarpControlPoint1Tag,
        RSNGEffectType::BezierWarpControlPoint2Tag,
        RSNGEffectType::BezierWarpControlPoint3Tag,
        RSNGEffectType::BezierWarpControlPoint4Tag,
        RSNGEffectType::BezierWarpControlPoint5Tag,
        RSNGEffectType::BezierWarpControlPoint6Tag,
        RSNGEffectType::BezierWarpControlPoint7Tag,
        RSNGEffectType::BezierWarpControlPoint8Tag,
        RSNGEffectType::BezierWarpControlPoint9Tag,
        RSNGEffectType::BezierWarpControlPoint10Tag,
        RSNGEffectType::BezierWarpControlPoint11Tag
    };
    for (size_t i = 0; i < BEZIER_WARP_POINT_NUM; ++i) {
        bezierWarpFilter->Setter<bezierCtrlPointsNGType[i]>(bezierCtrlPoints[i]);
    }
    return bezierWarpFilter;
}
}

static std::unordered_map<FilterPara::ParaType, FilterConvertor> convertorLUT = {
    { FilterPara::ParaType::DISPLACEMENT_DISTORT, ConvertDisplacementDistortFilterPara },
    { FilterPara::ParaType::EDGE_LIGHT, ConvertEdgeLightFilterPara },
    { FilterPara::ParaType::DIRECTION_LIGHT, ConvertDirectionLightFilterPara },
    { FilterPara::ParaType::DISPERSION, ConvertDispersionFilterPara },
    { FilterPara::ParaType::COLOR_GRADIENT, ConvertColorGradientFilterPara },
    { FilterPara::ParaType::MASK_TRANSITION, ConvertMaskTransitionFilterPara },
    { FilterPara::ParaType::VARIABLE_RADIUS_BLUR, ConvertVariableRadiusBlurFilterPara },
    { FilterPara::ParaType::BEZIER_WARP, ConvertBezierWarpFilterPara },
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

    // Disable temporarily, enable after full verification
    static std::unordered_set<FilterPara::ParaType> forceDisableTypes = {
        FilterPara::ParaType::DISPLACEMENT_DISTORT,
        FilterPara::ParaType::EDGE_LIGHT,
        FilterPara::ParaType::DISPERSION,
        FilterPara::ParaType::COLOR_GRADIENT,
    };
    if (forceDisableTypes.find(filterPara->GetParaType()) != forceDisableTypes.end()) {
        return nullptr;
    }

    auto it = convertorLUT.find(filterPara->GetParaType());
    return it != convertorLUT.end() ? it->second(filterPara) : nullptr;
}

} // namespace Rosen
} // namespace OHOS
