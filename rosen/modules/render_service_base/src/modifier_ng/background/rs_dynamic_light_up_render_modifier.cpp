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

#include "modifier_ng/background/rs_dynamic_light_up_render_modifier.h"

#include "common/rs_optional_trace.h"
#include "effect/runtime_blender_builder.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"

namespace OHOS::Rosen::ModifierNG {
constexpr int RS_TRACE_LEVEL_TWO = 2;

const RSDynamicLightUpRenderModifier::LegacyPropertyApplierMap
    RSDynamicLightUpRenderModifier::LegacyPropertyApplierMap_ = {
        { RSPropertyType::DYNAMIC_LIGHT_UP_RATE,
            RSRenderModifier::PropertyApplyHelper<std::optional<float>, &RSProperties::SetDynamicLightUpRate> },
        { RSPropertyType::DYNAMIC_LIGHT_UP_DEGREE,
            RSRenderModifier::PropertyApplyHelper<std::optional<float>, &RSProperties::SetDynamicLightUpDegree> },
    };

void RSDynamicLightUpRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetDynamicLightUpRate({});
    properties.SetDynamicLightUpDegree({});
}

bool RSDynamicLightUpRenderModifier::OnApply(RSModifierContext& context)
{
    if (!HasProperty(RSPropertyType::DYNAMIC_LIGHT_UP_RATE) &&
        !HasProperty(RSPropertyType::DYNAMIC_LIGHT_UP_DEGREE)) {
        return false;
    }
    auto rate = Getter<float>(RSPropertyType::DYNAMIC_LIGHT_UP_RATE);
    auto degree = Getter<float>(RSPropertyType::DYNAMIC_LIGHT_UP_DEGREE);
    if (ROSEN_GNE(rate, 0.0) && ROSEN_GE(degree, -1.0) && ROSEN_LE(degree, 1.0)) {
        stagingDynamicLightUpRate_ = Getter<float>(RSPropertyType::DYNAMIC_LIGHT_UP_RATE, 0.f);
        stagingDynamicLightUpDeg_ = Getter<float>(RSPropertyType::DYNAMIC_LIGHT_UP_DEGREE, 0.f);
        return true;
    }
    return false;
}

void RSDynamicLightUpRenderModifier::OnSync()
{
    renderDynamicLightUpRate_ = stagingDynamicLightUpRate_;
    renderDynamicLightUpDeg_ = stagingDynamicLightUpDeg_;
}

void RSDynamicLightUpRenderModifier::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect)
{
    if (canvas.GetUICapture()) {
        return;
    }

    auto alpha = canvas.GetAlpha();
    auto blender = RSDynamicLightUpRenderModifier::MakeDynamicLightUpBlender(
        renderDynamicLightUpRate_, renderDynamicLightUpDeg_, alpha);
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(RS_TRACE_LEVEL_TWO,
        "RSDynamicLightUpRenderModifier::Draw, rate: %f, degree: %f, bounds: %s", renderDynamicLightUpRate_,
        renderDynamicLightUpDeg_, rect.ToString().c_str());
    Drawing::Brush brush;
    brush.SetBlender(blender);
    canvas.DrawBackground(brush);
}

std::shared_ptr<Drawing::Blender> RSDynamicLightUpRenderModifier::MakeDynamicLightUpBlender(
    float rate, float degree, float alpha)
{
    static constexpr char prog[] = R"(
         uniform float dynamicLightUpRate;
         uniform float dynamicLightUpDeg;
 
         vec4 main(vec4 drawing_src, vec4 drawing_dst) {
             float x = 0.299 * drawing_dst.r + 0.587 * drawing_dst.g + 0.114 * drawing_dst.b;
             float y = (0 - dynamicLightUpRate) * x + dynamicLightUpDeg;
             float R = clamp((drawing_dst.r + y), 0.0, 1.0);
             float G = clamp((drawing_dst.g + y), 0.0, 1.0);
             float B = clamp((drawing_dst.b + y), 0.0, 1.0);
             return vec4(R, G, B, 1.0);
         }
     )";
    static std::shared_ptr<Drawing::RuntimeEffect> dynamicLightUpBlenderEffect_ = nullptr;

    if (dynamicLightUpBlenderEffect_ == nullptr) {
        dynamicLightUpBlenderEffect_ = Drawing::RuntimeEffect::CreateForBlender(prog);
        if (dynamicLightUpBlenderEffect_ == nullptr) {
            ROSEN_LOGE("RSDynamicLightUpDrawable::MakeDynamicLightUpBlender effect error!");
            return nullptr;
        }
    }
    auto builder = std::make_shared<Drawing::RuntimeBlenderBuilder>(dynamicLightUpBlenderEffect_);
    builder->SetUniform("dynamicLightUpRate", rate * alpha);
    builder->SetUniform("dynamicLightUpDeg", degree * alpha);
    return builder->MakeBlender();
}
} // namespace OHOS::Rosen::ModifierNG
