/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "modifier/rs_render_modifier.h"

#include <memory>
#include <unordered_map>

#include "pixel_map.h"

#include "common/rs_obj_abs_geometry.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "draw/color.h"
#include "image/bitmap.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"
#include "property/rs_properties_def.h"
#include "property/rs_properties_painter.h"
#include "render/rs_image.h"
#include "render/rs_mask.h"
#include "render/rs_path.h"
#include "render/rs_render_filter.h"
#include "render/rs_shader.h"

namespace OHOS {
namespace Rosen {
namespace {
using ModifierUnmarshallingFunc = RSRenderModifier* (*)(Parcel& parcel);

#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, DELTA_OP, MODIFIER_TIER, THRESHOLD_TYPE) \
    {                                                                                                            \
        RSModifierType::MODIFIER_TYPE,                                                                           \
        [](Parcel& parcel) -> RSRenderModifier* {                                                                \
            std::shared_ptr<RSRenderAnimatableProperty<TYPE>> prop;                                              \
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {                                             \
                return nullptr;                                                                                  \
            }                                                                                                    \
            auto modifier = new RS##MODIFIER_NAME##RenderModifier(prop);                                         \
            return modifier;                                                                                     \
        },                                                                                                       \
    },

#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, MODIFIER_TIER)                \
    { RSModifierType::MODIFIER_TYPE, [](Parcel& parcel) -> RSRenderModifier* {                          \
            std::shared_ptr<RSRenderProperty<TYPE>> prop;                                               \
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {                                    \
                return nullptr;                                                                         \
            }                                                                                           \
            auto modifier = new RS##MODIFIER_NAME##RenderModifier(prop);                                \
            return ((!modifier) ? nullptr : modifier);                                                  \
        },                                                                                              \
    },

static std::unordered_map<RSModifierType, ModifierUnmarshallingFunc> funcLUT = {
#include "modifier/rs_modifiers_def.in"
    { RSModifierType::EXTENDED, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderProperty<std::shared_ptr<Drawing::DrawCmdList>>> prop;
            int16_t type;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop) || !parcel.ReadInt16(type)) {
                ROSEN_LOGE("RSModifierType::EXTENDE Unmarshalling or ReadInt16 failed");
                return nullptr;
            }
            if (!prop) {
                ROSEN_LOGE("RSModifierType::EXTENDE prop is nullptr");
                return nullptr;
            }
            RSDrawCmdListRenderModifier* modifier = new RSDrawCmdListRenderModifier(prop);
            modifier->SetType(static_cast<RSModifierType>(type));
            return modifier;
        },
    },
    { RSModifierType::ENV_FOREGROUND_COLOR, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderAnimatableProperty<Color>> prop;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {
                ROSEN_LOGE("RSModifierType::ENV_FOREGROUND_COLOR Unmarshalling failed");
                return nullptr;
            }
            if (!prop) {
                ROSEN_LOGE("RSModifierType::ENV_FOREGROUND_COLOR prop is nullptr");
                return nullptr;
            }
            auto modifier = new RSEnvForegroundColorRenderModifier(prop);
            return modifier;
        },
    },
    { RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderProperty<ForegroundColorStrategyType>> prop;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {
                ROSEN_LOGE("RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY Unmarshalling failed");
                return nullptr;
            }
            if (!prop) {
                ROSEN_LOGE("RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY prop is nullptr");
                return nullptr;
            }
            auto modifier = new RSEnvForegroundColorStrategyRenderModifier(prop);
            return modifier;
        },
    },
    { RSModifierType::CUSTOM_CLIP_TO_FRAME, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderAnimatableProperty<Vector4f>> prop;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {
                ROSEN_LOGE("RSModifierType::CUSTOM_CLIP_TO_FRAME Unmarshalling failed");
                return nullptr;
            }
            if (!prop) {
                ROSEN_LOGE("RSModifierType::CUSTOM_CLIP_TO_FRAME prop is nullptr");
                return nullptr;
            }
            auto modifier = new RSCustomClipToFrameRenderModifier(prop);
            return modifier;
        },
    },
    { RSModifierType::HDR_BRIGHTNESS, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderAnimatableProperty<float>> prop;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {
                ROSEN_LOGE("RSModifierType::HDR_BRIGHTNESS Unmarshalling failed");
                return nullptr;
            }
            if (!prop) {
                ROSEN_LOGE("RSModifierType::HDR_BRIGHTNESS prop is nullptr");
                return nullptr;
            }
            auto modifier = new RSHDRBrightnessRenderModifier(prop);
            return modifier;
        },
    },
    { RSModifierType::GEOMETRYTRANS, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderProperty<Drawing::Matrix>> prop;
            int16_t type;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop) || !parcel.ReadInt16(type)) {
                ROSEN_LOGE("RSModifierType::GEOMETRYTRANS Unmarshalling or ReadInt16 failed");
                return nullptr;
            }
            if (!prop) {
                ROSEN_LOGE("RSModifierType::GEOMETRYTRANS prop is nullptr");
                return nullptr;
            }
            auto modifier = new RSGeometryTransRenderModifier(prop);
            modifier->SetType(static_cast<RSModifierType>(type));
            return modifier;
        },
    },
    { RSModifierType::BEHIND_WINDOW_FILTER_RADIUS, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderAnimatableProperty<float>> prop;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {
                ROSEN_LOGE("RSModifierType::BEHIND_WINDOW_FILTER_RADIUS Unmarshalling failed");
                return nullptr;
            }
            if (!prop) {
                ROSEN_LOGE("RSModifierType::BEHIND_WINDOW_FILTER_RADIUS prop is nullptr");
                return nullptr;
            }
            auto modifier = new RSBehindWindowFilterRadiusRenderModifier(prop);
            return modifier;
        },
    },
    { RSModifierType::BEHIND_WINDOW_FILTER_SATURATION, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderAnimatableProperty<float>> prop;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {
                ROSEN_LOGE("RSModifierType::BEHIND_WINDOW_FILTER_SATURATION Unmarshalling failed");
                return nullptr;
            }
            if (!prop) {
                ROSEN_LOGE("RSModifierType::BEHIND_WINDOW_FILTER_SATURATION prop is nullptr");
                return nullptr;
            }
            auto modifier = new RSBehindWindowFilterSaturationRenderModifier(prop);
            return modifier;
        },
    },
    { RSModifierType::BEHIND_WINDOW_FILTER_BRIGHTNESS, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderAnimatableProperty<float>> prop;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {
                ROSEN_LOGE("RSModifierType::BEHIND_WINDOW_FILTER_BRIGHTNESS Unmarshalling failed");
                return nullptr;
            }
            if (!prop) {
                ROSEN_LOGE("RSModifierType::BEHIND_WINDOW_FILTER_BRIGHTNESS prop is nullptr");
                return nullptr;
            }
            auto modifier = new RSBehindWindowFilterBrightnessRenderModifier(prop);
            return modifier;
        },
    },
    { RSModifierType::BEHIND_WINDOW_FILTER_MASK_COLOR, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderAnimatableProperty<Color>> prop;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {
                ROSEN_LOGE("RSModifierType::BEHIND_WINDOW_FILTER_MASK_COLOR Unmarshalling failed");
                return nullptr;
            }
            if (!prop) {
                ROSEN_LOGE("RSModifierType::BEHIND_WINDOW_FILTER_MASK_COLOR prop is nullptr");
                return nullptr;
            }
            auto modifier = new RSBehindWindowFilterMaskColorRenderModifier(prop);
            return modifier;
        },
    },
    { RSModifierType::COMPLEX_SHADER_PARAM, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderAnimatableProperty<std::vector<float>>> prop;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {
                ROSEN_LOGE("RSModifierType::COMPLEX_SHADER_PARAM Unmarshalling failed");
                return nullptr;
            }
            if (!prop) {
                ROSEN_LOGE("RSModifierType::COMPLEX_SHADER_PARAM prop is nullptr");
                return nullptr;
            }
            auto modifier = new RSComplexShaderParamRenderModifier(prop);
            return modifier;
        },
    },
    { RSModifierType::BACKGROUND_UI_FILTER, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderPropertyBase> prop =
                std::make_shared<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>();
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {
                ROSEN_LOGE("RSModifierType::BACKGROUND_UI_FILTER Unmarshalling failed");
                return nullptr;
            }
            if (!prop) {
                ROSEN_LOGE("RSModifierType::BACKGROUND_UI_FILTER prop is nullptr");
                return nullptr;
            }
            auto modifier = new RSBackgroundUIFilterRenderModifier(prop);
            return modifier;
        },
    },
    { RSModifierType::FOREGROUND_UI_FILTER, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderPropertyBase> prop =
                std::make_shared<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>();
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {
                ROSEN_LOGE("RSModifierType::FOREGROUND_UI_FILTER Unmarshalling failed");
                return nullptr;
            }
            auto modifier = new RSForegroundUIFilterRenderModifier(prop);
            return modifier;
        },
    },
    { RSModifierType::FOREGROUND_NG_FILTER, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderPropertyBase> prop =
                std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderFilterBase>>>();
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop) || !prop) {
                ROSEN_LOGE("RSModifierType::FOREGROUND_NG_FILTER Unmarshalling failed");
                return nullptr;
            }
            if (!prop) {
                ROSEN_LOGE("RSModifierType::FOREGROUND_UI_FILTER prop is nullptr");
                return nullptr;
            }
            auto modifier = new RSForegroundNGFilterRenderModifier(prop);
            return modifier;
        },
    },
    { RSModifierType::BACKGROUND_NG_FILTER, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderPropertyBase> prop =
                std::make_shared<RSRenderProperty<std::shared_ptr<RSNGRenderFilterBase>>>();
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop) || !prop) {
                ROSEN_LOGE("RSModifierType::BACKGROUND_NG_FILTER Unmarshalling failed");
                return nullptr;
            }
            if (!prop) {
                ROSEN_LOGE("RSModifierType::BACKGROUND_NG_FILTER prop is nullptr");
                return nullptr;
            }
            auto modifier = new RSBackgroundNGFilterRenderModifier(prop);
            return modifier;
        },
    },
};

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER
}

void RSDrawCmdListRenderModifier::Apply(RSModifierContext& context) const
{
    if (context.canvas_) {
        auto& cmds = property_->GetRef();
        RSPropertiesPainter::DrawFrame(context.properties_, *context.canvas_, cmds);
    }
}

void RSDrawCmdListRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(prop)) {
        property_->Set(property->Get());
    }
}

bool RSDrawCmdListRenderModifier::Marshalling(Parcel& parcel)
{
    bool flag = parcel.WriteInt16(static_cast<int16_t>(RSModifierType::EXTENDED)) &&
        RSMarshallingHelper::Marshalling(parcel, property_) && parcel.WriteInt16(static_cast<int16_t>(GetType()));
    if (!flag) {
        ROSEN_LOGE("RSDrawCmdListRenderModifier::Marshalling failed");
    }
    return flag;
}

bool RSEnvForegroundColorRenderModifier::Marshalling(Parcel& parcel)
{
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(property_);
    bool flag = parcel.WriteInt16(static_cast<int16_t>(RSModifierType::ENV_FOREGROUND_COLOR)) &&
            RSMarshallingHelper::Marshalling(parcel, renderProperty);
    if (!flag) {
        ROSEN_LOGE("RSEnvForegroundColorRenderModifier::Marshalling failed");
    }
    return flag;
}

void RSEnvForegroundColorRenderModifier::Apply(RSModifierContext& context) const
{
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(property_);
    context.canvas_->SetEnvForegroundColor(renderProperty->Get());
}

void RSEnvForegroundColorRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(prop)) {
        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(property_);
        renderProperty->Set(isDelta ? (renderProperty->Get() + property->Get()) : property->Get());
    }
}

bool RSEnvForegroundColorStrategyRenderModifier::Marshalling(Parcel& parcel)
{
    auto renderProperty = std::static_pointer_cast<RSRenderProperty<ForegroundColorStrategyType>>(property_);
    bool flag = parcel.WriteInt16(static_cast<short>(RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY)) &&
            RSMarshallingHelper::Marshalling(parcel, renderProperty);
    if (!flag) {
        ROSEN_LOGE("RSEnvForegroundColorStrategyRenderModifier::Marshalling failed");
    }
    return flag;
}


void RSEnvForegroundColorStrategyRenderModifier::Apply(RSModifierContext& context) const
{
    auto renderProperty = std::static_pointer_cast<RSRenderProperty<ForegroundColorStrategyType>>(property_);
    switch (renderProperty->Get()) {
        case ForegroundColorStrategyType::INVERT_BACKGROUNDCOLOR: {
            // calculate the color by screebshot
            Color color = GetInvertBackgroundColor(context);
            context.canvas_->SetEnvForegroundColor(color);
            break;
        }
        default: {
            break;
        }
    }
}

Color RSEnvForegroundColorStrategyRenderModifier::CalculateInvertColor(Color backgroundColor) const
{
    int16_t a = std::clamp<int16_t>(backgroundColor.GetAlpha(), 0, UINT8_MAX);
    int16_t r = 255 - std::clamp<int16_t>(backgroundColor.GetRed(), 0, UINT8_MAX);
    int16_t g = 255 - std::clamp<int16_t>(backgroundColor.GetGreen(), 0, UINT8_MAX);
    int16_t b = 255 - std::clamp<int16_t>(backgroundColor.GetBlue(), 0, UINT8_MAX);
    return Color(r, g, b, a);
}

Color RSEnvForegroundColorStrategyRenderModifier::GetInvertBackgroundColor(RSModifierContext& context) const
{
    Drawing::AutoCanvasRestore acr(*context.canvas_, true);
    if (!context.properties_.GetClipToBounds()) {
        RS_LOGI("RSRenderModifier::GetInvertBackgroundColor not GetClipToBounds");
        Vector4f clipRegion = context.properties_.GetBounds();
        Drawing::Rect rect = Drawing::Rect(0, 0, clipRegion.z_, clipRegion.w_);
        context.canvas_->ClipRect(rect, Drawing::ClipOp::INTERSECT, false);
    }
    Color backgroundColor = context.properties_.GetBackgroundColor();
    if (backgroundColor.GetAlpha() == 0xff) {
        RS_LOGI("RSRenderModifier::GetInvertBackgroundColor not alpha");
        return CalculateInvertColor(backgroundColor);
    }
    auto imageSnapshot = context.canvas_->GetSurface()->GetImageSnapshot(context.canvas_->GetDeviceClipBounds());
    if (imageSnapshot == nullptr) {
        RS_LOGI("RSRenderModifier::GetInvertBackgroundColor imageSnapshot null");
        return Color(0);
    }
    auto colorPicker = RSPropertiesPainter::CalcAverageColor(imageSnapshot);
    return CalculateInvertColor(Color(
        Drawing::Color::ColorQuadGetR(colorPicker), Drawing::Color::ColorQuadGetG(colorPicker),
        Drawing::Color::ColorQuadGetB(colorPicker), Drawing::Color::ColorQuadGetA(colorPicker)));
}

void RSEnvForegroundColorStrategyRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderProperty<ForegroundColorStrategyType >>(prop)) {
        auto renderProperty = std::static_pointer_cast<RSRenderProperty<ForegroundColorStrategyType >>(property_);
        renderProperty->Set(property->Get());
    }
}

bool RSCustomClipToFrameRenderModifier::Marshalling(Parcel& parcel)
{
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(property_);
    bool flag = parcel.WriteInt16(static_cast<int16_t>(RSModifierType::CUSTOM_CLIP_TO_FRAME)) &&
        RSMarshallingHelper::Marshalling(parcel, renderProperty);
    if (!flag) {
        ROSEN_LOGE("RSCustomClipToFrameRenderModifier::Marshalling failed");
    }
    return flag;
}

void RSCustomClipToFrameRenderModifier::Apply(RSModifierContext& context) const
{
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(property_);
    const auto& rect4f = renderProperty->Get();
    Drawing::Rect customClipRect(rect4f.x_, rect4f.y_, rect4f.z_, rect4f.w_);
    context.canvas_->ClipRect(customClipRect);
}

void RSCustomClipToFrameRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(prop)) {
        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(property_);
        renderProperty->Set(isDelta ? (renderProperty->Get() + property->Get()) : property->Get());
    }
}

bool RSHDRBrightnessRenderModifier::Marshalling(Parcel& parcel)
{
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(property_);
    bool flag = parcel.WriteInt16(static_cast<int16_t>(RSModifierType::HDR_BRIGHTNESS)) &&
        RSMarshallingHelper::Marshalling(parcel, renderProperty);
    if (!flag) {
        ROSEN_LOGE("RSHDRBrightnessRenderModifier::Marshalling failed");
    }
    return flag;
}

void RSHDRBrightnessRenderModifier::Apply(RSModifierContext& context) const {}

void RSHDRBrightnessRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(prop)) {
        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(property_);
        renderProperty->Set(isDelta ? (renderProperty->Get() + property->Get()) : property->Get());
    }
}

void RSGeometryTransRenderModifier::Apply(RSModifierContext& context) const
{
    auto& geoPtr = (context.properties_.GetBoundsGeometry());
    geoPtr->ConcatMatrix(property_->Get());
}

void RSGeometryTransRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderProperty<Drawing::Matrix>>(prop)) {
        property_->Set(property->Get());
    }
}

bool RSGeometryTransRenderModifier::Marshalling(Parcel& parcel)
{
    bool flag = parcel.WriteInt16(static_cast<int16_t>(RSModifierType::GEOMETRYTRANS)) &&
           RSMarshallingHelper::Marshalling(parcel, property_) && parcel.WriteInt16(static_cast<int16_t>(GetType()));
    if (!flag) {
        ROSEN_LOGE("RSGeometryTransRenderModifier::Marshalling failed");
    }
    return flag;
}

void RSBehindWindowFilterRadiusRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(prop)) {
        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(property_);
        renderProperty->Set(isDelta ? (renderProperty->Get() + property->Get()) : property->Get());
    }
}

bool RSBehindWindowFilterRadiusRenderModifier::Marshalling(Parcel& parcel)
{
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(property_);
    bool flag = parcel.WriteInt16(static_cast<int16_t>(RSModifierType::BEHIND_WINDOW_FILTER_RADIUS)) &&
        RSMarshallingHelper::Marshalling(parcel, renderProperty);
    if (!flag) {
        ROSEN_LOGE("RSBehindWindowFilterRadiusRenderModifier::Marshalling failed");
    }
    return flag;
}

void RSBehindWindowFilterSaturationRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop,
    bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(prop)) {
        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(property_);
        renderProperty->Set(isDelta ? (renderProperty->Get() + property->Get()) : property->Get());
    }
}

bool RSBehindWindowFilterSaturationRenderModifier::Marshalling(Parcel& parcel)
{
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(property_);
    bool flag = parcel.WriteInt16(static_cast<int16_t>(RSModifierType::BEHIND_WINDOW_FILTER_SATURATION)) &&
        RSMarshallingHelper::Marshalling(parcel, renderProperty);
    if (!flag) {
        ROSEN_LOGE("RSBehindWindowFilterSaturationRenderModifier::Marshalling failed");
    }
    return flag;
}

void RSBehindWindowFilterBrightnessRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop,
    bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(prop)) {
        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(property_);
        renderProperty->Set(isDelta ? (renderProperty->Get() + property->Get()) : property->Get());
    }
}

bool RSBehindWindowFilterBrightnessRenderModifier::Marshalling(Parcel& parcel)
{
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(property_);
    bool flag = parcel.WriteInt16(static_cast<int16_t>(RSModifierType::BEHIND_WINDOW_FILTER_BRIGHTNESS)) &&
        RSMarshallingHelper::Marshalling(parcel, renderProperty);
    if (!flag) {
        ROSEN_LOGE("RSBehindWindowFilterBrightnessRenderModifier::Marshalling failed");
    }
    return flag;
}

void RSBehindWindowFilterMaskColorRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop,
    bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(prop)) {
        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(property_);
        renderProperty->Set(isDelta ? (renderProperty->Get() + property->Get()) : property->Get());
    }
}

bool RSBehindWindowFilterMaskColorRenderModifier::Marshalling(Parcel& parcel)
{
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(property_);
    bool flag = parcel.WriteInt16(static_cast<int16_t>(RSModifierType::BEHIND_WINDOW_FILTER_MASK_COLOR)) &&
        RSMarshallingHelper::Marshalling(parcel, renderProperty);
    if (!flag) {
        ROSEN_LOGE("RSBehindWindowFilterMaskColorRenderModifier::Marshalling failed");
    }
    return flag;
}

void RSComplexShaderParamRenderModifier::Apply(RSModifierContext& context) const
{
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<std::vector<float>>>(property_);
    context.properties_.SetComplexShaderParam(
        Add(context.properties_.GetComplexShaderParam().value_or(std::vector<float>()), renderProperty->Get()));
}

void RSComplexShaderParamRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderAnimatableProperty<std::vector<float>>>(prop)) {
        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<std::vector<float>>>(property_);
        renderProperty->Set(isDelta ? (renderProperty->Get() + property->Get()) : property->Get());
    }
}

bool RSComplexShaderParamRenderModifier::Marshalling(Parcel& parcel)
{
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<std::vector<float>>>(property_);
    bool flag = parcel.WriteInt16(static_cast<int16_t>(RSModifierType::COMPLEX_SHADER_PARAM)) &&
        RSMarshallingHelper::Marshalling(parcel, renderProperty);
    if (!flag) {
        ROSEN_LOGE("RSComplexShaderParamRenderModifier::Marshalling failed");
    }
    return flag;
}

void RSBackgroundUIFilterRenderModifier::Apply(RSModifierContext& context) const
{
    auto renderProperty = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(property_);
    context.properties_.SetBackgroundUIFilter(renderProperty->GetRef());
}

void RSBackgroundUIFilterRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(prop)) {
        auto renderProperty =
            std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(property_);
        renderProperty->Set(property->Get());
    }
}

bool RSBackgroundUIFilterRenderModifier::Marshalling(Parcel& parcel)
{
    auto prop = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(property_);
    std::shared_ptr<RSRenderPropertyBase> renderFilter =
        std::make_shared<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(prop->Get(), prop->GetId());
    bool flag = parcel.WriteInt16(static_cast<int16_t>(RSModifierType::BACKGROUND_UI_FILTER)) &&
                RSMarshallingHelper::Marshalling(parcel, renderFilter);
    if (!flag) {
        ROSEN_LOGE("RSBackgroundUIFilterRenderModifier::Marshalling failed");
    }
    return flag;
}

void RSForegroundUIFilterRenderModifier::Apply(RSModifierContext& context) const
{
    auto renderProperty = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(property_);
    context.properties_.SetForegroundUIFilter(renderProperty->GetRef());
}

void RSForegroundUIFilterRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(prop)) {
        auto renderProperty =
            std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(property_);
        renderProperty->Set(property->Get());
    }
}

bool RSForegroundUIFilterRenderModifier::Marshalling(Parcel& parcel)
{
    auto prop = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(property_);
    std::shared_ptr<RSRenderPropertyBase> renderFilter =
        std::make_shared<RSRenderProperty<std::shared_ptr<RSRenderFilter>>>(prop->Get(), prop->GetId());
    bool flag = parcel.WriteInt16(static_cast<int16_t>(RSModifierType::FOREGROUND_UI_FILTER)) &&
                RSMarshallingHelper::Marshalling(parcel, renderFilter);
    if (!flag) {
        ROSEN_LOGE("RSForegroundUIFilterRenderModifier::Marshalling failed");
    }
    return flag;
}

void RSForegroundNGFilterRenderModifier::Apply(RSModifierContext& context) const
{
    auto renderProperty =
        std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSNGRenderFilterBase>>>(property_);
    context.properties_.SetForegroundNGFilter(renderProperty->GetRef());
}

void RSForegroundNGFilterRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSNGRenderFilterBase>>>(prop)) {
        auto renderProperty =
            std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSNGRenderFilterBase>>>(property_);
        renderProperty->Set(property->Get());
    }
}

bool RSForegroundNGFilterRenderModifier::Marshalling(Parcel& parcel)
{
    bool flag = parcel.WriteInt16(static_cast<int16_t>(RSModifierType::FOREGROUND_NG_FILTER)) &&
        RSMarshallingHelper::Marshalling(parcel, property_);
    if (!flag) {
        ROSEN_LOGE("RSForegroundNGFilterRenderModifier::Marshalling failed");
    }
    return flag;
}

void RSBackgroundNGFilterRenderModifier::Apply(RSModifierContext& context) const
{
    auto renderProperty =
        std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSNGRenderFilterBase>>>(property_);
    context.properties_.SetBackgroundNGFilter(renderProperty->Get());
}

void RSBackgroundNGFilterRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSNGRenderFilterBase>>>(prop)) {
        auto renderProperty =
            std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSNGRenderFilterBase>>>(property_);
        renderProperty->Set(property->Get());
    }
}

bool RSBackgroundNGFilterRenderModifier::Marshalling(Parcel& parcel)
{
    bool flag = parcel.WriteInt16(static_cast<int16_t>(RSModifierType::BACKGROUND_NG_FILTER)) &&
        RSMarshallingHelper::Marshalling(parcel, property_);
    if (!flag) {
        ROSEN_LOGE("RSBackgroundNGFilterRenderModifier::Marshalling failed");
    }
    return flag;
}

RSRenderModifier* RSRenderModifier::Unmarshalling(Parcel& parcel)
{
    int16_t type = 0;
    if (!parcel.ReadInt16(type)) {
        ROSEN_LOGE("RSRenderModifier::Unmarshalling ReadInt16 failed");
        return nullptr;
    }
    auto it = funcLUT.find(static_cast<RSModifierType>(type));
    if (it == funcLUT.end()) {
        ROSEN_LOGE("RSRenderModifier Unmarshalling cannot find func in lut %{public}d", type);
        return nullptr;
    }
    return it->second(parcel);
}

namespace {
template<typename T>
T Add(const T& a, const T&& b)
{
    return a + b;
}
template<typename T>
T Add(const std::optional<T>& a, const T&& b)
{
    return a.has_value() ? *a + b : b;
}

template<typename T>
T Multiply(const T& a, const T&& b)
{
    return a * b;
}
template<typename T>
T Multiply(const std::optional<T>& a, const T&& b)
{
    return a.has_value() ? *a * b : b;
}

template<typename T>
const T& Replace(const T& a, const T&& b)
{
    return b;
}
template<typename T>
const T& Replace(const std::optional<T>& a, T&& b)
{
    return b;
}
} // namespace

#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, DELTA_OP, MODIFIER_TIER, THRESHOLD_TYPE)    \
    bool RS##MODIFIER_NAME##RenderModifier::Marshalling(Parcel& parcel)                                             \
    {                                                                                                               \
        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<TYPE>>(property_);                \
        bool flag = parcel.WriteInt16(static_cast<int16_t>(RSModifierType::MODIFIER_TYPE)) &&                       \
               RSMarshallingHelper::Marshalling(parcel, renderProperty);                                            \
        if (!flag) {                                                                                                \
            ROSEN_LOGE("ANIMATABLE_MODIFIER RenderModifier::Marshalling WriteInt16 or Marshalling failed");         \
        }                                                                                                           \
        return flag;                                                                                                \
    }                                                                                                               \
    void RS##MODIFIER_NAME##RenderModifier::Apply(RSModifierContext& context) const                                 \
    {                                                                                                               \
        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<TYPE>>(property_);                \
        context.properties_.Set##MODIFIER_NAME(                                                                     \
            DELTA_OP(context.properties_.Get##MODIFIER_NAME(), renderProperty->Get()));                             \
    }                                                                                                               \
    void RS##MODIFIER_NAME##RenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta) \
    {                                                                                                               \
        if (auto property = std::static_pointer_cast<RSRenderAnimatableProperty<TYPE>>(prop)) {                     \
            auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<TYPE>>(property_);            \
            renderProperty->Set(isDelta ? (renderProperty->Get() + property->Get()) : property->Get());             \
        }                                                                                                           \
    }

#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, MODIFIER_TIER)                            \
    bool RS##MODIFIER_NAME##RenderModifier::Marshalling(Parcel& parcel)                                             \
    {                                                                                                               \
        auto renderProperty = std::static_pointer_cast<RSRenderProperty<TYPE>>(property_);                          \
        bool flag = parcel.WriteInt16(static_cast<short>(RSModifierType::MODIFIER_TYPE)) &&                         \
               RSMarshallingHelper::Marshalling(parcel, renderProperty);                                            \
        if (!flag) {                                                                                                \
            ROSEN_LOGE("NOANIMATABLE_MODIFIER RenderModifier::Marshalling WriteInt16 or Marshalling failed");       \
        }                                                                                                           \
        return flag;                                                                                                \
    }                                                                                                               \
    void RS##MODIFIER_NAME##RenderModifier::Apply(RSModifierContext& context) const                                 \
    {                                                                                                               \
        auto renderProperty = std::static_pointer_cast<RSRenderProperty<TYPE>>(property_);                          \
        context.properties_.Set##MODIFIER_NAME(renderProperty->GetRef());                                              \
    }                                                                                                               \
    void RS##MODIFIER_NAME##RenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta) \
    {                                                                                                               \
        if (auto property = std::static_pointer_cast<RSRenderProperty<TYPE>>(prop)) {                               \
            auto renderProperty = std::static_pointer_cast<RSRenderProperty<TYPE>>(property_);                      \
            renderProperty->Set(property->GetRef());                                                                   \
        }                                                                                                           \
    }

#include "modifier/rs_modifiers_def.in"
DECLARE_NOANIMATABLE_MODIFIER(Particles, RSRenderParticleVector, PARTICLE, Foreground)

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER
} // namespace Rosen
} // namespace OHOS
