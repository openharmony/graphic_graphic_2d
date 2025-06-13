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

#define DECLARE_ANIM_PROPERTY_TAG(EffectName, PropName, Type) \
    inline static constexpr char EffectName##PropName##Name[] = #EffectName "_" #PropName; \
    using EffectName##PropName##RenderTag = RSRenderAnimatablePropertyTag<Type, EffectName##PropName##Name>

#define DECLARE_NOANIM_PROPERTY_TAG(EffectName, PropName, Type) \
    inline static constexpr char EffectName##PropName##Name[] = #EffectName "_" #PropName; \
    using EffectName##PropName##RenderTag = RSRenderPropertyTag<Type, EffectName##PropName##Name>

class RSNGRenderMaskBase; // forward declaration, impl in rs_render_mask_base.h
#define MASK_PTR std::shared_ptr<RSNGRenderMaskBase>

#include "render/rs_render_property_tag_def.in"

#undef MASK_PTR
#undef DECLARE_ANIM_PROPERTY_TAG
#undef DECLARE_NOANIM_PROPERTY_TAG

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
};

enum class RSUIFilterType : int16_t {
    INVALID = -1,
    NONE = 0,
    // filter type
    BLUR,
    DISPLACEMENT_DISTORT,
    COLOR_GRADIENT,
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
    PIXEL_STRETCH,
    WATER_RIPPLE,

    LINEAR_GRADIENT_BLUR,
    FLY_OUT,
    DISTORTION,
    ALWAYS_SNAPSHOT,
    // mask type
    RIPPLE_MASK,
    RADIAL_GRADIENT_MASK,
    PIXEL_MAP_MASK,

    // value type
    BLUR_RADIUS_X,  // float
    BLUR_RADIUS_Y,  // float

    // value type
    RIPPLE_MASK_CENTER, // Vector2f
    RIPPLE_MASK_RADIUS, // float
    RIPPLE_MASK_WIDTH, // float
    RIPPLE_MASK_WIDTH_CENTER_OFFSET, // float
    DISPLACEMENT_DISTORT_FACTOR, // Vector2f

    // value type
    COLOR_GRADIENT_COLOR, // vector<float>
    COLOR_GRADIENT_POSITION, // vector<float>
    COLOR_GRADIENT_STRENGTH, // vector<float>

    //value type
    SOUND_WAVE_COLOR_A, //RSCOLOR
    SOUND_WAVE_COLOR_B, //RSCOLOR
    SOUND_WAVE_COLOR_C, //RSCOLOR
    SOUND_WAVE_COLOR_PROGRESS, //float
    SOUND_INTENSITY, //float
    SHOCK_WAVE_ALPHA_A, //float
    SHOCK_WAVE_ALPHA_B, //float
    SHOCK_WAVE_PROGRESS_A, //float
    SHOCK_WAVE_PROGRESS_B, //float
    SHOCK_WAVE_TOTAL_ALPHA, //float

    // edge light value type
    EDGE_LIGHT_ALPHA, // float
    EDGE_LIGHT_COLOR, // Vector4f

    // bezier warp value type
    BEZIER_CONTROL_POINT0, // Vector2f
    BEZIER_CONTROL_POINT1, // Vector2f
    BEZIER_CONTROL_POINT2, // Vector2f
    BEZIER_CONTROL_POINT3, // Vector2f
    BEZIER_CONTROL_POINT4, // Vector2f
    BEZIER_CONTROL_POINT5, // Vector2f
    BEZIER_CONTROL_POINT6, // Vector2f
    BEZIER_CONTROL_POINT7, // Vector2f
    BEZIER_CONTROL_POINT8, // Vector2f
    BEZIER_CONTROL_POINT9, // Vector2f
    BEZIER_CONTROL_POINT10, // Vector2f
    BEZIER_CONTROL_POINT11, // Vector2f

    // pixel map mask value type
    PIXEL_MAP_MASK_PIXEL_MAP, // Media::PixelMap
    PIXEL_MAP_MASK_SRC, // Vector4f
    PIXEL_MAP_MASK_DST, // Vector4f
    PIXEL_MAP_MASK_FILL_COLOR, // Vector4f

    // dispersion value type
    DISPERSION_OPACITY, // float
    DISPERSION_RED_OFFSET, // Vector2f
    DISPERSION_GREEN_OFFSET, // Vector2f
    DISPERSION_BLUE_OFFSET, // Vector2f

    // edge light bloom value type
    EDGE_LIGHT_BLOOM, // bool

    // radial gradient mask value type
    RADIAL_GRADIENT_MASK_CENTER, // Vector2f
    RADIAL_GRADIENT_MASK_RADIUSX, // float
    RADIAL_GRADIENT_MASK_RADIUSY, // float
    RADIAL_GRADIENT_MASK_COLORS, // vector<float>
    RADIAL_GRADIENT_MASK_POSITIONS, // vector<float>
};

using RSNGEffectTypeUnderlying = std::underlying_type<RSNGEffectType>::type;
using RSUIFilterTypeUnderlying = std::underlying_type<RSUIFilterType>::type;

constexpr auto END_OF_CHAIN = static_cast<RSNGEffectTypeUnderlying>(RSNGEffectType::INVALID); // -1

} // namespace Rosen
} // namespace OHOS


#endif // RENDER_SERVICE_BASE_RENDER_PROPERTY_TAG_H
