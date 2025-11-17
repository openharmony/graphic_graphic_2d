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

#ifndef RENDER_SERVICE_BASE_EFFECT_RS_RENDER_SHAPE_BASE_H
#define RENDER_SERVICE_BASE_EFFECT_RS_RENDER_SHAPE_BASE_H

#include "effect/rs_render_effect_template.h"
#include "effect/rs_render_property_tag.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {

namespace Drawing {
    class GEShaderShape;
} // namespace Drawing

class RSB_EXPORT RSNGRenderShapeBase : public RSNGRenderEffectBase<RSNGRenderShapeBase> {
public:
    virtual ~RSNGRenderShapeBase() = default;

    static std::shared_ptr<RSNGRenderShapeBase> Create(RSNGEffectType type);

    [[nodiscard]] static bool Unmarshalling(Parcel& parcel, std::shared_ptr<RSNGRenderShapeBase>& val);

    virtual std::shared_ptr<Drawing::GEVisualEffect> GenerateGEVisualEffect() { return nullptr; }
};

template<RSNGEffectType Type, typename... PropertyTags>
class RSNGRenderShapeTemplate : public RSNGRenderEffectTemplate<RSNGRenderShapeBase, Type, PropertyTags...> {
public:
    using EffectTemplateBase = RSNGRenderEffectTemplate<RSNGRenderShapeBase, Type, PropertyTags...>;

    RSNGRenderShapeTemplate() : EffectTemplateBase() {}
    ~RSNGRenderShapeTemplate() override = default;
    RSNGRenderShapeTemplate(const std::tuple<PropertyTags...>& properties) noexcept
        : EffectTemplateBase(properties) {}

    std::shared_ptr<Drawing::GEVisualEffect> GenerateGEVisualEffect() override
    {
        RS_OPTIONAL_TRACE_FMT("RSNGRenderShapeTemplate::GenerateGEVisualEffect, Type: %s",
            RSNGRenderEffectHelper::GetEffectTypeString(Type).c_str());
        auto geShape = RSNGRenderEffectHelper::CreateGEVisualEffect(Type);
        OnGenerateGEVisualEffect(geShape);
        std::apply([&geShape](const auto&... propTag) {
                (RSNGRenderEffectHelper::UpdateVisualEffectParam<std::decay_t<decltype(propTag)>>(
                    geShape, propTag), ...);
            }, EffectTemplateBase::properties_);
        // Currently only a single Shape is used, so we do not handle the nextEffect_ here.
        return geShape;
    }

protected:
    virtual void OnGenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffect>) {}
};

#define ADD_PROPERTY_TAG(Effect, Prop) Effect##Prop##RenderTag
#define DECLARE_SHAPE(ShapeName, ShapeType, ...) \
    using RSNGRender##ShapeName = RSNGRenderShapeTemplate<RSNGEffectType::ShapeType, __VA_ARGS__>

// SDF OP Shape
DECLARE_SHAPE(SDFUnionOpShape, SDF_UNION_OP_SHAPE,
    ADD_PROPERTY_TAG(SDFUnionOpShape, ShapeX),
    ADD_PROPERTY_TAG(SDFUnionOpShape, ShapeY)
);

DECLARE_SHAPE(SDFSmoothUnionOpShape, SDF_SMOOTH_UNION_OP_SHAPE,
    ADD_PROPERTY_TAG(SDFSmoothUnionOpShape, Spacing),
    ADD_PROPERTY_TAG(SDFSmoothUnionOpShape, ShapeX),
    ADD_PROPERTY_TAG(SDFSmoothUnionOpShape, ShapeY)
);

// SDF shape
DECLARE_SHAPE(SDFRRectShape, SDF_RRECT_SHAPE,
    ADD_PROPERTY_TAG(SDFRRectShape, RRect)
);

#undef ADD_PROPERTY_TAG
#undef DECLARE_SHAPE

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_EFFECT_RS_RENDER_SHAPE_BASE_H
