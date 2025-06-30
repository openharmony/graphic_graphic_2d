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

#include "modifier_ng/appearance/rs_background_filter_render_modifier.h"
#include "pipeline/rs_render_node.h"

namespace OHOS::Rosen::ModifierNG {
const RSBackgroundFilterRenderModifier::LegacyPropertyApplierMap
    RSBackgroundFilterRenderModifier::LegacyPropertyApplierMap_ = {
        { RSPropertyType::SYSTEMBAREFFECT,
            RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetSystemBarEffect> },
        { RSPropertyType::WATER_RIPPLE_PROGRESS,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetWaterRippleProgress,
                &RSProperties::GetWaterRippleProgress> },
        { RSPropertyType::WATER_RIPPLE_PARAMS,
            RSRenderModifier::PropertyApplyHelper<RSWaterRipplePara, &RSProperties::SetWaterRippleParams> },
        { RSPropertyType::MAGNIFIER_PARA, RSRenderModifier::PropertyApplyHelper<std::shared_ptr<RSMagnifierParams>,
                                              &RSProperties::SetMagnifierParams> },
        { RSPropertyType::BACKGROUND_BLUR_RADIUS,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetBackgroundBlurRadius,
                &RSProperties::GetBackgroundBlurRadius> },
        { RSPropertyType::BACKGROUND_BLUR_SATURATION,
            RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetBackgroundBlurSaturation> },
        { RSPropertyType::BACKGROUND_BLUR_BRIGHTNESS,
            RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetBackgroundBlurBrightness> },
        { RSPropertyType::BACKGROUND_BLUR_MASK_COLOR,
            RSRenderModifier::PropertyApplyHelperAdd<Color, &RSProperties::SetBackgroundBlurMaskColor,
                &RSProperties::GetBackgroundBlurMaskColor> },
        { RSPropertyType::BACKGROUND_BLUR_COLOR_MODE,
            RSRenderModifier::PropertyApplyHelper<int, &RSProperties::SetBackgroundBlurColorMode> },
        { RSPropertyType::BACKGROUND_BLUR_RADIUS_X,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetBackgroundBlurRadiusX,
                &RSProperties::GetBackgroundBlurRadiusX> },
        { RSPropertyType::BACKGROUND_BLUR_RADIUS_Y,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetBackgroundBlurRadiusY,
                &RSProperties::GetBackgroundBlurRadiusY> },
        { RSPropertyType::BG_BLUR_DISABLE_SYSTEM_ADAPTATION,
            RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetBgBlurDisableSystemAdaptation> },
        { RSPropertyType::ALWAYS_SNAPSHOT,
            RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetAlwaysSnapshot> },
        { RSPropertyType::GREY_COEF, RSRenderModifier::PropertyApplyHelper<Vector2f, &RSProperties::SetGreyCoef> },
        { RSPropertyType::BACKGROUND_UI_FILTER, RSRenderModifier::PropertyApplyHelper<std::shared_ptr<RSRenderFilter>,
                                                    &RSProperties::SetBackgroundUIFilter> },
        { RSPropertyType::BACKGROUND_NG_FILTER,
            RSRenderModifier::PropertyApplyHelper<std::shared_ptr<RSNGRenderFilterBase>,
                &RSProperties::SetBackgroundNGFilter> },
    };

void RSBackgroundFilterRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetSystemBarEffect(false);
    properties.SetWaterRippleProgress(0.f);
    properties.SetWaterRippleParams(std::nullopt);
    properties.SetMagnifierParams(nullptr);
    properties.SetBackgroundBlurRadius(0.f);
    properties.SetBackgroundBlurSaturation(1.f);
    properties.SetBackgroundBlurBrightness(1.f);
    properties.SetBackgroundBlurMaskColor(Color());
    properties.SetBackgroundBlurColorMode(0);
    properties.SetBackgroundBlurRadiusX(0.f);
    properties.SetBackgroundBlurRadiusY(0.f);
    properties.SetBgBlurDisableSystemAdaptation(true);
    properties.SetAlwaysSnapshot(false);
    properties.SetGreyCoef(std::nullopt);
    properties.SetBackgroundUIFilter({});
    properties.SetBackgroundNGFilter({});
}

void RSBackgroundFilterRenderModifier::AttachRenderFilterProperty(
    const std::shared_ptr<RSRenderPropertyBase>& property, RSPropertyType type)
{
    if (type != ModifierNG::RSPropertyType::BACKGROUND_UI_FILTER) {
        return;
    }
    if (!property) {
        return;
    }
    auto node = target_.lock();
    if (!node) {
        return;
    }
    auto renderProperty = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(property);
    if (!renderProperty) {
        return;
    }
    auto& renderFilter = renderProperty->GetRef();
    for (auto paramtype : renderFilter->GetUIFilterTypes()) {
        auto propGroup = renderFilter->GetRenderFilterPara(paramtype);
        if (!propGroup) {
            continue;
        }
        for (auto& prop : propGroup->GetLeafRenderProperties()) {
            if (prop) {
                node->properties_.emplace(prop->GetId(), prop);
                prop->Attach(shared_from_this());
            }
        }
    }
}

void RSBackgroundFilterRenderModifier::DetachRenderFilterProperty(
    const std::shared_ptr<RSRenderPropertyBase>& property, RSPropertyType type)
{
    if (type != ModifierNG::RSPropertyType::BACKGROUND_UI_FILTER) {
        return;
    }
    if (!property) {
        return;
    }
    auto node = target_.lock();
    if (!node) {
        return;
    }
    auto renderProperty = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(property);
    if (!renderProperty) {
        return;
    }
    auto& renderFilter = renderProperty->GetRef();
    for (auto paramtype : renderFilter->GetUIFilterTypes()) {
        auto propGroup = renderFilter->GetRenderFilterPara(paramtype);
        if (!propGroup) {
            continue;
        }
        for (auto& prop : propGroup->GetLeafRenderProperties()) {
            if (prop) {
                node->properties_.erase(prop->GetId());
            }
        }
    }
}

} // namespace OHOS::Rosen::ModifierNG
