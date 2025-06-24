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

#ifndef RENDER_SERVICE_BASE_EFFECT_RS_RENDER_MASK_BASE_H
#define RENDER_SERVICE_BASE_EFFECT_RS_RENDER_MASK_BASE_H

#include "effect/rs_render_effect_template.h"
#include "effect/rs_render_property_tag.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {

namespace Drawing {
    class GEShaderMask;
} // namespace Drawing

class RSB_EXPORT RSNGRenderMaskBase : public RSNGRenderEffectBase<RSNGRenderMaskBase> {
public:
    virtual std::shared_ptr<Drawing::GEShaderMask> GenerateGEShaderMask()
    {
        return nullptr;
    }
};

template<RSNGEffectType Type, typename... PropertyTags>
using RSNGRenderMaskTemplate = RSNGRenderEffectTemplate<RSNGRenderMaskBase, Type, PropertyTags...>;

#define ADD_PROPERTY_TAG(Effect, Prop) Effect##Prop##RenderTag

#define DECLARE_MASK(MaskName, MaskType, ...) \
    using RSNGRender##MaskName = RSNGRenderMaskTemplate<RSNGEffectType::MaskType, __VA_ARGS__>

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

#undef ADD_PROPERTY_TAG
#undef DECLARE_MASK

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_EFFECT_RS_RENDER_MASK_BASE_H
