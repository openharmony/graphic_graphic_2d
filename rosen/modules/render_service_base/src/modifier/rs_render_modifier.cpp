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

#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties.h"
#include "property/rs_properties_painter.h"

namespace OHOS {
namespace Rosen {
namespace {
using ModifierUnmarshallingFunc = RSRenderModifier* (*)(Parcel& parcel);

#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, DELTA_OP)        \
    { RSModifierType::MODIFIER_TYPE, [](Parcel& parcel) -> RSRenderModifier* {           \
            std::shared_ptr<RSRenderAnimatableProperty<TYPE>> prop;                      \
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {                     \
                return nullptr;                                                          \
            }                                                                            \
            auto modifier = new RS##MODIFIER_NAME##RenderModifier(prop);                 \
            if (!modifier) {                                                             \
                return nullptr;                                                          \
            }                                                                            \
            return modifier;                                                             \
        },                                                                               \
    },

#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE)                \
    { RSModifierType::MODIFIER_TYPE, [](Parcel& parcel) -> RSRenderModifier* {           \
            std::shared_ptr<RSRenderProperty<TYPE>> prop;                                \
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {                     \
                return nullptr;                                                          \
            }                                                                            \
            auto modifier = new RS##MODIFIER_NAME##RenderModifier(prop);                 \
            if (!modifier) {                                                             \
                return nullptr;                                                          \
            }                                                                            \
            return modifier;                                                             \
        },                                                                               \
    },

static std::unordered_map<RSModifierType, ModifierUnmarshallingFunc> funcLUT = {
#include "modifier/rs_modifiers_def.in"
    { RSModifierType::EXTENDED, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderProperty<std::shared_ptr<DrawCmdList>>> prop;
            int16_t type;
            bool hasOverlayBounds = false;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop) || !parcel.ReadInt16(type) ||
                !parcel.ReadBool(hasOverlayBounds)) {
                return nullptr;
            }
            RSDrawCmdListRenderModifier* modifier = new RSDrawCmdListRenderModifier(prop);
            modifier->SetType(static_cast<RSModifierType>(type));
            if (hasOverlayBounds) {
                // OVERLAY_STYLE
                int32_t left;
                int32_t top;
                int32_t width;
                int32_t height;
                if (!(parcel.ReadInt32(left) && parcel.ReadInt32(top) &&
                    parcel.ReadInt32(width) && parcel.ReadInt32(height))) {
                    return nullptr;
                }
                modifier->SetOverlayBounds(std::make_shared<RectI>(left, top, width, height));
            }
            return modifier;
        },
    },
};

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER
}

void RSDrawCmdListRenderModifier::Apply(RSModifierContext& context)
{
    if (context.canvas_) {
        auto cmds = property_->Get();
        RSPropertiesPainter::DrawFrame(context.property_, *context.canvas_, cmds);
    }
}

void RSDrawCmdListRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderProperty<DrawCmdListPtr>>(prop)) {
        property_->Set(property->Get());
    }
}

bool RSDrawCmdListRenderModifier::Marshalling(Parcel& parcel)
{
    if (parcel.WriteInt16(static_cast<int16_t>(RSModifierType::EXTENDED)) &&
        RSMarshallingHelper::Marshalling(parcel, property_) && parcel.WriteInt16(static_cast<int16_t>(GetType())) &&
        parcel.WriteBool(overlayRect_ != nullptr)) {
        if (overlayRect_ != nullptr) {
            return parcel.WriteInt32(overlayRect_->GetLeft()) && parcel.WriteInt32(overlayRect_->GetTop()) &&
                parcel.WriteInt32(overlayRect_->GetWidth()) && parcel.WriteInt32(overlayRect_->GetHeight());
        }
        return true;
    }
    return false;
}

RSRenderModifier* RSRenderModifier::Unmarshalling(Parcel& parcel)
{
    int16_t type = 0;
    if (!parcel.ReadInt16(type)) {
        return nullptr;
    }
    auto it = funcLUT.find(static_cast<RSModifierType>(type));
    if (it == funcLUT.end()) {
        ROSEN_LOGE("RSRenderModifier Unmarshalling cannot find func in lut %d", type);
        return nullptr;
    }
    return it->second(parcel);
}

namespace {
template<typename T>
T Add(T a, T b)
{
    return a + b;
}
template<typename T>
T Multiply(T a, T b)
{
    return a * b;
}
template<typename T>
T Replace(T a, T b)
{
    return b;
}
} // namespace

#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, DELTA_OP)                                     \
    bool RS##MODIFIER_NAME##RenderModifier::Marshalling(Parcel& parcel)                                               \
    {                                                                                                                 \
        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<TYPE>>(property_);                  \
        return parcel.WriteInt16(static_cast<int16_t>(RSModifierType::MODIFIER_TYPE)) &&                              \
               RSMarshallingHelper::Marshalling(parcel, renderProperty);                                              \
    }                                                                                                                 \
    void RS##MODIFIER_NAME##RenderModifier::Apply(RSModifierContext& context)                                         \
    {                                                                                                                 \
        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<TYPE>>(property_);                  \
        context.property_.Set##MODIFIER_NAME(                                                                         \
            DELTA_OP(context.property_.Get##MODIFIER_NAME(), renderProperty->Get()));                                 \
    }                                                                                                                 \
    void RS##MODIFIER_NAME##RenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)   \
    {                                                                                                                 \
        if (auto property = std::static_pointer_cast<RSRenderAnimatableProperty<TYPE>>(prop)) {                       \
            auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<TYPE>>(property_);              \
            renderProperty->Set(isDelta ? (renderProperty->Get() + property->Get()) : property->Get());               \
        }                                                                                                             \
    }

#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE)                                             \
    bool RS##MODIFIER_NAME##RenderModifier::Marshalling(Parcel& parcel)                                               \
    {                                                                                                                 \
        auto renderProperty = std::static_pointer_cast<RSRenderProperty<TYPE>>(property_);                            \
        return parcel.WriteInt16(static_cast<short>(RSModifierType::MODIFIER_TYPE)) &&                                \
               RSMarshallingHelper::Marshalling(parcel, renderProperty);                                              \
    }                                                                                                                 \
    void RS##MODIFIER_NAME##RenderModifier::Apply(RSModifierContext& context)                                         \
    {                                                                                                                 \
        auto renderProperty = std::static_pointer_cast<RSRenderProperty<TYPE>>(property_);                            \
        context.property_.Set##MODIFIER_NAME(renderProperty->Get());                                                  \
    }                                                                                                                 \
    void RS##MODIFIER_NAME##RenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)   \
    {                                                                                                                 \
        if (auto property = std::static_pointer_cast<RSRenderProperty<TYPE>>(prop)) {                                 \
            auto renderProperty = std::static_pointer_cast<RSRenderProperty<TYPE>>(property_);                        \
            renderProperty->Set(property->Get());                                                                     \
        }                                                                                                             \
    }

#include "modifier/rs_modifiers_def.in"

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER
} // namespace Rosen
} // namespace OHOS
