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
    virtual ~RSNGRenderMaskBase() = default;

    static std::shared_ptr<RSNGRenderMaskBase> Create(RSNGEffectType type);

    [[nodiscard]] static bool Unmarshalling(Parcel& parcel, std::shared_ptr<RSNGRenderMaskBase>& val);

    virtual std::shared_ptr<Drawing::GEVisualEffect> GenerateGEVisualEffect() = 0;
};

template<RSNGEffectType Type, typename... PropertyTags>
class RSNGRenderMaskTemplate : public RSNGRenderEffectTemplate<RSNGRenderMaskBase, Type, PropertyTags...> {
public:
    using EffectTemplateBase = RSNGRenderEffectTemplate<RSNGRenderMaskBase, Type, PropertyTags...>;

    RSNGRenderMaskTemplate() : EffectTemplateBase() {}
    ~RSNGRenderMaskTemplate() override = default;
    RSNGRenderMaskTemplate(const std::tuple<PropertyTags...>& properties) noexcept
        : EffectTemplateBase(properties) {}

    std::shared_ptr<Drawing::GEVisualEffect> GenerateGEVisualEffect() override
    {
        auto geMask = RSNGRenderEffectHelper::CreateGEVisualEffect(Type);
        OnGenerateGEVisualEffect(geMask);
        std::apply([&geMask](const auto&... propTag) {
                (RSNGRenderEffectHelper::UpdateVisualEffectParam<std::decay_t<decltype(propTag)>>(
                    geMask, propTag), ...);
            }, EffectTemplateBase::properties_);
        // Currently only a single mask is used, so we do not handle the nextEffect_ here.
        return geMask;
    }

protected:
    virtual void OnGenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffect>) {}
};


#define ADD_PROPERTY_TAG(Effect, Prop) Effect##Prop##RenderTag

#define DECLARE_MASK(MaskName, MaskType, ...) \
    using RSNGRender##MaskName = RSNGRenderMaskTemplate<RSNGEffectType::MaskType, __VA_ARGS__>

DECLARE_MASK(RippleMask, RIPPLE_MASK,
    ADD_PROPERTY_TAG(RippleMask, Center),
    ADD_PROPERTY_TAG(RippleMask, Radius),
    ADD_PROPERTY_TAG(RippleMask, Width),
    ADD_PROPERTY_TAG(RippleMask, Offset)
);

DECLARE_MASK(PixelMapMask, PIXEL_MAP_MASK,
    ADD_PROPERTY_TAG(PixelMapMask, Src),
    ADD_PROPERTY_TAG(PixelMapMask, Dst),
    ADD_PROPERTY_TAG(PixelMapMask, FillColor),
    ADD_PROPERTY_TAG(PixelMapMask, Image)
);

#undef ADD_PROPERTY_TAG
#undef DECLARE_MASK

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_EFFECT_RS_RENDER_MASK_BASE_H
