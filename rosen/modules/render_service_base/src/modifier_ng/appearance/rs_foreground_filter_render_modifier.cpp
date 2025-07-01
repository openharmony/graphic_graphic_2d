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

#include "modifier_ng/appearance/rs_foreground_filter_render_modifier.h"

#include "pipeline/rs_render_node.h"

namespace OHOS::Rosen::ModifierNG {
const RSForegroundFilterRenderModifier::LegacyPropertyApplierMap
    RSForegroundFilterRenderModifier::LegacyPropertyApplierMap_ = {
        { RSPropertyType::SPHERIZE, RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetSpherize> },
        { RSPropertyType::FOREGROUND_EFFECT_RADIUS,
            RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetForegroundEffectRadius> },
        { RSPropertyType::MOTION_BLUR_PARA,
            RSRenderModifier::PropertyApplyHelper<std::shared_ptr<MotionBlurParam>, &RSProperties::SetMotionBlurPara> },
        { RSPropertyType::FLY_OUT_PARAMS,
            RSRenderModifier::PropertyApplyHelper<RSFlyOutPara, &RSProperties::SetFlyOutParams> },
        { RSPropertyType::FLY_OUT_DEGREE, RSRenderModifier::PropertyApplyHelperAdd<float,
                                              &RSProperties::SetFlyOutDegree, &RSProperties::GetFlyOutDegree> },
        { RSPropertyType::DISTORTION_K, RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetDistortionK,
                                            &RSProperties::GetDistortionK> },
        { RSPropertyType::ATTRACTION_FRACTION,
            RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetAttractionFraction> },
        { RSPropertyType::ATTRACTION_DSTPOINT,
            RSRenderModifier::PropertyApplyHelper<Vector2f, &RSProperties::SetAttractionDstPoint> },
        { RSPropertyType::FOREGROUND_UI_FILTER, RSRenderModifier::PropertyApplyHelper<std::shared_ptr<RSRenderFilter>,
                                                    &RSProperties::SetForegroundUIFilter> },
    };

void RSForegroundFilterRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetSpherize(0.f);
    properties.SetForegroundEffectRadius(0.f);
    properties.SetMotionBlurPara(nullptr);
    properties.SetFlyOutParams(std::nullopt);
    properties.SetFlyOutDegree(0.f);
    properties.SetDistortionK(std::nullopt);
    properties.SetAttractionFraction(0.f);
    properties.SetAttractionDstPoint(Vector2f());
    properties.SetForegroundUIFilter({});
}

void RSForegroundFilterRenderModifier::AttachRenderFilterProperty(
    const std::shared_ptr<RSRenderPropertyBase>& property, RSPropertyType type)
{
    if (type != ModifierNG::RSPropertyType::FOREGROUND_UI_FILTER) {
        return;
    }
    if (!property) {
        return;
    }
    auto node = target_.lock();
    if (!node) {
        return;
    }
    // static_pointer_cast will not return nullptr
    auto renderProperty = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(property);
    auto& renderFilter = renderProperty->GetRef();
    for (auto paramtype : renderFilter->GetUIFilterTypes()) {
        auto propGroup = renderFilter->GetRenderFilterPara(paramtype);
        if (!propGroup) {
            continue;
        }
        for (auto& prop : propGroup->GetLeafRenderProperties()) {
            if (prop) {
                prop->Attach(*node, shared_from_this());
            }
        }
    }
}

void RSForegroundFilterRenderModifier::DetachRenderFilterProperty(
    const std::shared_ptr<RSRenderPropertyBase>& property, RSPropertyType type)
{
    if (type != ModifierNG::RSPropertyType::FOREGROUND_UI_FILTER) {
        return;
    }
    if (!property) {
        return;
    }
    auto node = target_.lock();
    if (!node) {
        return;
    }
    // static_pointer_cast will not return nullptr
    auto renderProperty = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(property);
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
