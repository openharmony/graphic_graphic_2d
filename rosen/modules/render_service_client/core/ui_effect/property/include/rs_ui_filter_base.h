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

#ifndef ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_FILTER_BASE_H
#define ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_FILTER_BASE_H

#include "ui_effect/filter/include/filter_para.h"
#include "ui_effect/property/include/rs_ui_property_tag.h"
#include "ui_effect/property/include/rs_ui_template.h"

#include "effect/rs_render_filter_base.h"
#include "modifier/rs_property.h"
#include "render/rs_render_filter_base.h"

namespace OHOS {
namespace Rosen {

class RSNGFilterBase : public RSNGEffectBase<RSNGFilterBase, RSNGRenderFilterBase>  {
public:
    virtual ~RSNGFilterBase() = default;

    static std::shared_ptr<RSNGFilterBase> Create(RSNGEffectType type);

    static std::shared_ptr<RSNGFilterBase> Create(std::shared_ptr<FilterPara> filterPara);
};

template<RSNGEffectType Type, typename... PropertyTags>
using RSNGFilterTemplate = RSNGEffectTemplate<RSNGFilterBase, Type, PropertyTags...>;

#define ADD_PROPERTY_TAG(Effect, Prop) Effect##Prop##Tag

#define DECLARE_FILTER(FilterName, FilterType, ...) \
    using RSNG##FilterName##Filter = RSNGFilterTemplate<RSNGEffectType::FilterType, __VA_ARGS__>

DECLARE_FILTER(Blur, BLUR,
    ADD_PROPERTY_TAG(Blur, RadiusX),
    ADD_PROPERTY_TAG(Blur, RadiusY)
);

DECLARE_FILTER(DispDistort, DISPLACEMENT_DISTORT,
    ADD_PROPERTY_TAG(DispDistort, Factor),
    ADD_PROPERTY_TAG(DispDistort, Mask)
);

DECLARE_FILTER(SoundWave, SOUND_WAVE,
    ADD_PROPERTY_TAG(SoundWave, ColorA),
    ADD_PROPERTY_TAG(SoundWave, ColorB),
    ADD_PROPERTY_TAG(SoundWave, ColorC),
    ADD_PROPERTY_TAG(SoundWave, ColorProgress),
    ADD_PROPERTY_TAG(SoundWave, CenterBrightness),
    ADD_PROPERTY_TAG(SoundWave, Intensity),
    ADD_PROPERTY_TAG(SoundWave, AlphaA),
    ADD_PROPERTY_TAG(SoundWave, AlphaB),
    ADD_PROPERTY_TAG(SoundWave, ProgressA),
    ADD_PROPERTY_TAG(SoundWave, ProgressB)
);

DECLARE_FILTER(EdgeLight, EDGE_LIGHT,
    ADD_PROPERTY_TAG(EdgeLight, Color),
    ADD_PROPERTY_TAG(EdgeLight, Alpha),
    ADD_PROPERTY_TAG(EdgeLight, Mask)
);

DECLARE_FILTER(Dispersion, DISPERSION,
    ADD_PROPERTY_TAG(Dispersion, Opacity),
    ADD_PROPERTY_TAG(Dispersion, RedOffset),
    ADD_PROPERTY_TAG(Dispersion, GreenOffset),
    ADD_PROPERTY_TAG(Dispersion, BlueOffset)
);

DECLARE_FILTER(BezierWarp, BEZIER_WARP,
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint0),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint1),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint2),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint3),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint4),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint5),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint6),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint7),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint8),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint9),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint10),
    ADD_PROPERTY_TAG(BezierWarp, ControlPoint11)
);

DECLARE_FILTER(DirectionLight, DIRECTION_LIGHT,
    ADD_PROPERTY_TAG(DirectionLight, Mask),
    ADD_PROPERTY_TAG(DirectionLight, Direction),
    ADD_PROPERTY_TAG(DirectionLight, Color),
    ADD_PROPERTY_TAG(DirectionLight, Intensity)
);

#undef ADD_PROPERTY_TAG
#undef DECLARE_FILTER

} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_FILTER_BASE_H
