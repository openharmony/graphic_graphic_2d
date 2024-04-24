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
#include "draw/color.h"
#include "image/bitmap.h"
#include <memory>
#include <unordered_map>

#include "pixel_map.h"

#include "common/rs_obj_abs_geometry.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"
#include "property/rs_properties_def.h"
#include "property/rs_properties_painter.h"
#include "render/rs_filter.h"
#include "render/rs_image.h"
#include "render/rs_mask.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"

namespace OHOS {
namespace Rosen {
namespace {
using ModifierUnmarshallingFunc = RSRenderModifier* (*)(Parcel& parcel);

#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, DELTA_OP, MODIFIER_TIER, THRESHOLD_TYPE) \
    { RSModifierType::MODIFIER_TYPE, [](Parcel& parcel) -> RSRenderModifier* {                                   \
            std::shared_ptr<RSRenderAnimatableProperty<TYPE>> prop;                                              \
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {                                             \
                return nullptr;                                                                                  \
            }                                                                                                    \
            auto modifier = new RS##MODIFIER_NAME##RenderModifier(prop);                                         \
            return ((!modifier) ? nullptr : modifier);                                                           \
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
                return nullptr;
            }
            auto modifier = new RSEnvForegroundColorRenderModifier(prop);
            if (!modifier) {
                return nullptr;
            }
            return modifier;
        },
    },
    { RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderProperty<ForegroundColorStrategyType>> prop;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {
                return nullptr;
            }
            auto modifier = new RSEnvForegroundColorStrategyRenderModifier(prop);
            if (!modifier) {
                return nullptr;
            }
            return modifier;
        },
    },
    { RSModifierType::GEOMETRYTRANS, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderProperty<Drawing::Matrix>> prop;
            int16_t type;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop) || !parcel.ReadInt16(type)) {
                return nullptr;
            }
            auto modifier = new RSGeometryTransRenderModifier(prop);
            modifier->SetType(static_cast<RSModifierType>(type));
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
    return parcel.WriteInt16(static_cast<int16_t>(RSModifierType::EXTENDED)) &&
        RSMarshallingHelper::Marshalling(parcel, property_) && parcel.WriteInt16(static_cast<int16_t>(GetType()));
}

bool RSEnvForegroundColorRenderModifier::Marshalling(Parcel& parcel)
{
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(property_);
    return parcel.WriteInt16(static_cast<int16_t>(RSModifierType::ENV_FOREGROUND_COLOR)) &&
            RSMarshallingHelper::Marshalling(parcel, renderProperty);
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
    return parcel.WriteInt16(static_cast<short>(RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY)) &&
            RSMarshallingHelper::Marshalling(parcel, renderProperty);
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
    uint32_t a = backgroundColor.GetAlpha();
    uint32_t r = 255 - backgroundColor.GetRed();
    uint32_t g = 255 - backgroundColor.GetGreen();
    uint32_t b = 255 - backgroundColor.GetBlue();
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
    return parcel.WriteInt16(static_cast<int16_t>(RSModifierType::GEOMETRYTRANS)) &&
           RSMarshallingHelper::Marshalling(parcel, property_) && parcel.WriteInt16(static_cast<int16_t>(GetType()));
}

RSRenderModifier* RSRenderModifier::Unmarshalling(Parcel& parcel)
{
    int16_t type = 0;
    if (!parcel.ReadInt16(type)) {
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
        return parcel.WriteInt16(static_cast<int16_t>(RSModifierType::MODIFIER_TYPE)) &&                            \
               RSMarshallingHelper::Marshalling(parcel, renderProperty);                                            \
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
        return parcel.WriteInt16(static_cast<short>(RSModifierType::MODIFIER_TYPE)) &&                              \
               RSMarshallingHelper::Marshalling(parcel, renderProperty);                                            \
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
