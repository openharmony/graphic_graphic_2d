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

#ifndef RENDER_SERVICE_BASE_RENDER_PROPERTY_TAG_H
#define RENDER_SERVICE_BASE_RENDER_PROPERTY_TAG_H

#include "modifier/rs_render_property.h"
#include "pixel_map.h"

namespace OHOS {
namespace Rosen {

template<const char* Name, class PropertyType>
struct RenderPropertyTagBase {
    using ValueType = typename PropertyType::ValueType;
    static constexpr const char* NAME = Name;
    std::shared_ptr<PropertyType> value_ = std::make_shared<PropertyType>();
};

template<typename T, const char* Name>
using RSRenderPropertyTag = RenderPropertyTagBase<Name, RSRenderProperty<T>>;
template<typename T, const char* Name>
using RSRenderAnimatablePropertyTag  = RenderPropertyTagBase<Name, RSRenderAnimatableProperty<T>>;

#define DECLARE_ANIMATABLE_PROPERTY_TAG(EffectName, PropName, Type) \
    inline static constexpr char EffectName##PropName##Name[] = #EffectName "_" #PropName; \
    using EffectName##PropName##RenderTag = RSRenderAnimatablePropertyTag<Type, EffectName##PropName##Name>

#define DECLARE_NONANIMATABLE_PROPERTY_TAG(EffectName, PropName, Type) \
    inline static constexpr char EffectName##PropName##Name[] = #EffectName "_" #PropName; \
    using EffectName##PropName##RenderTag = RSRenderPropertyTag<Type, EffectName##PropName##Name>

class RSNGRenderMaskBase; // forward declaration, impl in rs_render_mask_base.h
#define MASK_PTR std::shared_ptr<RSNGRenderMaskBase>

#include "effect/rs_render_property_tag_def.in"

#undef MASK_PTR
#undef DECLARE_ANIMATABLE_PROPERTY_TAG
#undef DECLARE_NONANIMATABLE_PROPERTY_TAG

enum class RSNGEffectType : int16_t {
    INVALID = -1,
    NONE = 0,
    // filter type
    BLUR,
    PIXEL_STRETCH,
    WATER_RIPPLE,
    FLY_OUT,
    DISTORT,
    RADIUS_GRADIENT_BLUR,
    COLOR_GRADIENT,
    DISPLACEMENT_DISTORT,
    SOUND_WAVE,
    EDGE_LIGHT,
    BEZIER_WARP,
    DISPERSION,

    AIBAR,
    GREY,
    MATERIAL,
    MAGNIFIER,
    MESA,
    MASK_COLOR,
    KAWASE,
    LIGHT_BLUR,
    LINEAR_GRADIENT_BLUR,
    ALWAYS_SNAPSHOT,
    // mask type
    RIPPLE_MASK,
    RADIAL_GRADIENT_MASK,
    PIXEL_MAP_MASK,
    // shader type
    CONTOUR_DIAGONAL_FLOW_LIGHT,
    WAVY_RIPPLE_LIGHT,
    AURORA_NOISE,
};

using RSNGEffectTypeUnderlying = std::underlying_type<RSNGEffectType>::type;
constexpr auto END_OF_CHAIN = static_cast<RSNGEffectTypeUnderlying>(RSNGEffectType::INVALID); // -1

} // namespace Rosen
} // namespace OHOS


#endif // RENDER_SERVICE_BASE_RENDER_PROPERTY_TAG_H
