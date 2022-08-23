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

#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, RENDER_PROPERTY) \
    { RSModifierType::MODIFIER_TYPE, [](Parcel& parcel) -> RSRenderModifier* {           \
            std::shared_ptr<RENDER_PROPERTY<TYPE>> prop;                                 \
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {                     \
                return nullptr;                                                          \
            }                                                                            \
            auto modifier = new RS##MODIFIER_NAME##RenderModifier(prop);                 \
            if (!modifier) {                                                             \
                return nullptr;                                                          \
            }                                                                            \
            bool isAdditive = false;                                                     \
            if (!parcel.ReadBool(isAdditive)) {                                          \
                return nullptr;                                                          \
            }                                                                            \
            modifier->SetIsAdditive(isAdditive);                                         \
            return modifier;                                                             \
        },                                                                               \
    },

#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, RENDER_PROPERTY) \
    DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, RENDER_PROPERTY)

static std::unordered_map<RSModifierType, ModifierUnmarshallingFunc> funcLUT = {
#include "modifier/rs_modifiers_def.in"
    { RSModifierType::EXTENDED, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderProperty<std::shared_ptr<DrawCmdList>>> prop;
            int16_t type;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop) || !parcel.ReadInt16(type)) {
                return nullptr;
            }
            RSDrawCmdListRenderModifier* modifier = new RSDrawCmdListRenderModifier(prop);
            modifier->SetType(static_cast<RSModifierType>(type));
            return modifier;
        },
    },
};

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER
}

void RSDrawCmdListRenderModifier::Apply(RSModifyContext& context)
{
    if (context.canvas_) {
        auto cmds = property_->Get();
        RSPropertiesPainter::DrawFrame(context.property_, *context.canvas_, cmds);
    }
}

void RSDrawCmdListRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& newProp, bool isDelta)
{
    if (auto newProperty = std::static_pointer_cast<RSRenderProperty<DrawCmdListPtr>>(newProp)) {
        property_->Set(newProperty->Get());
    }
}

bool RSDrawCmdListRenderModifier::Marshalling(Parcel& parcel)
{
    return parcel.WriteInt16(static_cast<int16_t>(RSModifierType::EXTENDED)) &&
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
        ROSEN_LOGE("RSRenderModifier Unmarshalling cannot find func in lut %d", type);
        return nullptr;
    }
    return it->second(parcel);
}

#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, RENDER_PROPERTY)                            \
    bool RS##MODIFIER_NAME##RenderModifier::Marshalling(Parcel& parcel)                                             \
    {                                                                                                               \
        return parcel.WriteInt16(static_cast<int16_t>(RSModifierType::MODIFIER_TYPE)) &&                            \
            RSMarshallingHelper::Marshalling(parcel, property_) && parcel.WriteBool(isAdditive_);                   \
    }                                                                                                               \
    void RS##MODIFIER_NAME##RenderModifier::Apply(RSModifyContext& context)                                         \
    {                                                                                                               \
        TYPE setValue;                                                                                              \
        if (isFirstSet_) {                                                                                          \
            setValue = context.property_.Get##MODIFIER_NAME() + property_->Get();                                   \
            isFirstSet_ = false;                                                                                    \
        } else {                                                                                                    \
            setValue = context.property_.Get##MODIFIER_NAME() + property_->Get() - lastValue_->Get();               \
        }                                                                                                           \
        setValue = isAdditive_ ? setValue : property_->Get();                                                       \
        lastValue_->Set(property_->Get());                                                                          \
        context.property_.Set##MODIFIER_NAME(setValue);                                                             \
    }                                                                                                               \
    void RS##MODIFIER_NAME##RenderModifier::Update(                                                                 \
        const std::shared_ptr<RSRenderPropertyBase>& newProp, bool isDelta)                                         \
    {                                                                                                               \
        if (auto newProperty = std::static_pointer_cast<RENDER_PROPERTY<TYPE>>(newProp)) {                          \
            if (isDelta) {                                                                                          \
                property_->Set(property_->Get() + newProperty->Get());                                              \
            } else {                                                                                                \
                property_->Set(newProperty->Get());                                                                 \
            }                                                                                                       \
        }                                                                                                           \
    }

#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, RENDER_PROPERTY)                          \
    bool RS##MODIFIER_NAME##RenderModifier::Marshalling(Parcel& parcel)                                             \
    {                                                                                                               \
        return parcel.WriteInt16(static_cast<short>(RSModifierType::MODIFIER_TYPE)) &&                              \
            RSMarshallingHelper::Marshalling(parcel, property_) && parcel.WriteBool(isAdditive_);                   \
    }                                                                                                               \
    void RS##MODIFIER_NAME##RenderModifier::Apply(RSModifyContext& context)                                         \
    {                                                                                                               \
        context.property_.Set##MODIFIER_NAME(property_->Get());                                                     \
    }                                                                                                               \
                                                                                                                    \
    void RS##MODIFIER_NAME##RenderModifier::Update(                                                                 \
        const std::shared_ptr<RSRenderPropertyBase>& newProp, bool isDelta)                                         \
    {                                                                                                               \
        if (auto newProperty = std::static_pointer_cast<RENDER_PROPERTY<TYPE>>(newProp)) {                          \
            property_->Set(newProperty->Get());                                                                     \
        }                                                                                                           \
    }

#include "modifier/rs_modifiers_def.in"

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER
} // namespace Rosen
} // namespace OHOS
