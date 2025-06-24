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

#ifndef ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_PROPERTY_TAG_H
#define ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_PROPERTY_TAG_H

#include "modifier/rs_property.h"
#include "effect/rs_render_property_tag.h"

namespace OHOS {
namespace Rosen {

class RSUIFilterUtils {
public:
    template<class PropertyType>
    static std::shared_ptr<typename PropertyType::RenderPropertyType> GetRenderProperty(
        const std::shared_ptr<PropertyType>& value)
    {
        return std::static_pointer_cast<typename PropertyType::RenderPropertyType>(value->GetRenderProperty());
    }

    template<class PropertyType>
    static void Attach(const std::shared_ptr<PropertyType>& value, const std::shared_ptr<RSNode>& node)
    {
        return value->Attach(node);
    }

    template<class PropertyType>
    static void Detach(const std::shared_ptr<PropertyType>& value)
    {
        return value->Detach();
    }
};

template<const char* Name, class PropertyType>
struct PropertyTagBase {
    using ValueType = typename PropertyType::ValueType;
    using RenderPropertyTagType = RenderPropertyTagBase<Name, typename PropertyType::RenderPropertyType>;

    static constexpr const char* Tag = Name;
    std::shared_ptr<PropertyType> value_ = std::make_shared<PropertyType>();

    auto CreateRenderPropertyTag() const
    {
        return RenderPropertyTagType { RSUIFilterUtils::GetRenderProperty<PropertyType>(value_) };
    }
};

template<typename T, const char* Name>
using RSPropertyTag = PropertyTagBase<Name, RSProperty<T>>;

template<typename T, const char* Name>
using RSAnimatablePropertyTag  = PropertyTagBase<Name, RSAnimatableProperty<T>>;

#define DECLARE_ANIMATABLE_PROPERTY_TAG(EffectName, PropName, Type) \
    using EffectName##PropName##Tag = RSAnimatablePropertyTag<Type, EffectName##PropName##Name>

#define DECLARE_NONANIMATABLE_PROPERTY_TAG(EffectName, PropName, Type) \
    using EffectName##PropName##Tag = RSPropertyTag<Type, EffectName##PropName##Name>

class RSNGMaskBase;
#define MASK_PTR std::shared_ptr<RSNGMaskBase>

#include "effect/rs_render_property_tag_def.in"

#undef MASK_PTR
#undef DECLARE_ANIMATABLE_PROPERTY_TAG
#undef DECLARE_NONANIMATABLE_PROPERTY_TAG

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_PROPERTY_TAG_H
