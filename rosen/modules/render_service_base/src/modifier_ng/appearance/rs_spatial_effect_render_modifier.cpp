/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "modifier_ng/appearance/rs_spatial_effect_render_modifier.h"

namespace OHOS::Rosen::ModifierNG {
const RSSpatialEffectRenderModifier::LegacyPropertyApplierMap
    RSSpatialEffectRenderModifier::LegacyPropertyApplierMap_ = {
        { RSPropertyType::SPATIAL_EFFECT_DEPTH,
            RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetSpatialEffectDepth> },
        { RSPropertyType::SPATIAL_EFFECT_LEFT_TOP,
            RSRenderModifier::PropertyApplyHelper<Vector3f, &RSProperties::SetSpatialEffectLeftTop> },
        { RSPropertyType::SPATIAL_EFFECT_RIGHT_TOP,
            RSRenderModifier::PropertyApplyHelper<Vector3f, &RSProperties::SetSpatialEffectRightTop> },
        { RSPropertyType::SPATIAL_EFFECT_LEFT_BOTTOM,
            RSRenderModifier::PropertyApplyHelper<Vector3f, &RSProperties::SetSpatialEffectLeftBottom> },
        { RSPropertyType::SPATIAL_EFFECT_RIGHT_BOTTOM,
            RSRenderModifier::PropertyApplyHelper<Vector3f, &RSProperties::SetSpatialEffectRightBottom> },
        { RSPropertyType::SPATIAL_EFFECT_OCCLUSION_WEIGHT,
            RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetSpatialEffectOcclusionWeight> },
        { RSPropertyType::SPATIAL_EFFECT_MODE,
            RSRenderModifier::PropertyApplyHelper<int, &RSProperties::SetSpatialEffectMode> },
    };

void RSSpatialEffectRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetSpatialEffectVariantPara(std::nullopt);
}
} // namespace OHOS::Rosen::ModifierNG