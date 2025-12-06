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

#include "ng_sdf_test_utils.h"

namespace OHOS::Rosen {
using ShapeCreator = std::function<std::shared_ptr<RSNGShapeBase>()>;
static std::unordered_map<RSNGEffectType, ShapeCreator> creatorShape = {
    {RSNGEffectType::SDF_UNION_OP_SHAPE,
        [] { return std::make_shared<RSNGSDFUnionOpShape>(); }},
    {RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE,
        [] { return std::make_shared<RSNGSDFSmoothUnionOpShape>(); }},
    {RSNGEffectType::SDF_RRECT_SHAPE,
        [] { return std::make_shared<RSNGSDFRRectShape>(); }},
    {RSNGEffectType::SDF_TRANSFORM_SHAPE,
        [] { return std::make_shared<RSNGSDFTransformShape>(); }}
};

std::shared_ptr<RSNGShapeBase> CreateShape(RSNGEffectType type)
{
    auto it = creatorShape.find(type);
    return it != creatorShape.end() ? it->second() : nullptr;
}

void InitSmoothUnionShapes(
    std::shared_ptr<RSNGShapeBase>& rootShape, RRect rRectX, RRect rRectY, float spacing)
{
    rootShape = CreateShape(RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE);
    auto sdfUnionRootShape = std::static_pointer_cast<RSNGSDFSmoothUnionOpShape>(rootShape);

    auto childShapeX = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
    auto rRectChildShapeX = std::static_pointer_cast<RSNGSDFRRectShape>(childShapeX);
    rRectChildShapeX->Setter<SDFRRectShapeRRectTag>(rRectX);
    sdfUnionRootShape->Setter<SDFSmoothUnionOpShapeShapeXTag>(childShapeX);

    auto childShapeY = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
    auto rRectChildShapeY = std::static_pointer_cast<RSNGSDFRRectShape>(childShapeY);
    rRectChildShapeY->Setter<SDFRRectShapeRRectTag>(rRectY);
    sdfUnionRootShape->Setter<SDFSmoothUnionOpShapeShapeYTag>(childShapeY);

    sdfUnionRootShape->Setter<SDFSmoothUnionOpShapeSpacingTag>(spacing);
}
} // namespace OHOS::Rosen
