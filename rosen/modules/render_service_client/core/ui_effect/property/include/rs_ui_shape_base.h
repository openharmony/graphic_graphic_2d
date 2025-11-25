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

#ifndef ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_SHAPE_BASE_H
#define ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_SHAPE_BASE_H

#include "effect/rs_render_shape_base.h"
#include "ui_effect/property/include/rs_ui_property_tag.h"
#include "ui_effect/property/include/rs_ui_template.h"

namespace OHOS {
namespace Rosen {

class RSNGShapeBase : public RSNGEffectBase<RSNGShapeBase, RSNGRenderShapeBase> {
public:
    virtual ~RSNGShapeBase() = default;

    static std::shared_ptr<RSNGShapeBase> Create(RSNGEffectType type);
};

template<RSNGEffectType Type, typename... PropertyTags>
using RSNGShapeTemplate = RSNGEffectTemplate<RSNGShapeBase, Type, PropertyTags...>;

#define ADD_PROPERTY_TAG(Effect, Prop) Effect##Prop##Tag

#define DECLARE_SHAPE(ShapeName, ShapeType, ...) \
    using RSNG##ShapeName = RSNGShapeTemplate<RSNGEffectType::ShapeType, __VA_ARGS__>

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

DECLARE_SHAPE(SDFTransformShape, SDF_TRANSFORM_SHAPE,
    ADD_PROPERTY_TAG(SDFTransformShape, Matrix),
    ADD_PROPERTY_TAG(SDFTransformShape, Shape)
);

DECLARE_SHAPE(SDFPixelmapShape, SDF_PIXELMAP_SHAPE,
    ADD_PROPERTY_TAG(SDFPixelmapShape, Image)
);

#undef DECLARE_SHAPE
#undef ADD_PROPERTY_TAG

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_UI_SHAPE_BASE_H
