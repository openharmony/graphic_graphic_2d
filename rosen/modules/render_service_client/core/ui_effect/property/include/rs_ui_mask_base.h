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

#ifndef ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_MASK_BASE_H
#define ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_MASK_BASE_H

#include "effect/rs_render_mask_base.h"
#include "ui_effect/property/include/rs_ui_property_tag.h"
#include "ui_effect/property/include/rs_ui_template.h"

namespace OHOS {
namespace Rosen {

class RSNGMaskBase : public RSNGEffectBase<RSNGMaskBase, RSNGRenderMaskBase> {
};

template<RSNGEffectType Type, typename... PropertyTags>
using RSNGMaskTemplate = RSNGEffectTemplate<RSNGMaskBase, Type, PropertyTags...>;

#define ADD_PROPERTY_TAG(Effect, Prop) Effect##Prop##Tag

#define DECLARE_MASK(MaskName, MaskType, ...) \
    using RSNG##MaskName = RSNGMaskTemplate<RSNGEffectType::MaskType, __VA_ARGS__>

DECLARE_MASK(RippleMask, RIPPLE_MASK,
    ADD_PROPERTY_TAG(RippleMask, Center),
    ADD_PROPERTY_TAG(RippleMask, Radius),
    ADD_PROPERTY_TAG(RippleMask, Width),
    ADD_PROPERTY_TAG(RippleMask, WidthCenterOffset)
);

DECLARE_MASK(PixelMapMask, PIXEL_MAP_MASK,
    ADD_PROPERTY_TAG(PixelMapMask, Src),
    ADD_PROPERTY_TAG(PixelMapMask, Dst),
    ADD_PROPERTY_TAG(PixelMapMask, FillColor)
);

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_MASK_BASE_H
