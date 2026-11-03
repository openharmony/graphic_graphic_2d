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

#include "drawable/rs_effect_drawable_utils.h"

#include "ge_render.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"

#include "common/rs_vector4.h"
#include "drawable/rs_property_drawable_utils.h"
#include "effect/rs_render_property_tag.h"
#include "effect/rs_render_shader_base.h"
#include "effect/rs_render_shape_base.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"

namespace OHOS::Rosen {
namespace DrawableV2 {

std::shared_ptr<RSNGRenderShapeBase> RSEffectDrawableUtils::CreateDefaultRRectShape(
    const RRect& sdfRRect, NodeId nodeId)
{
    auto sdfRRectShape =
        std::static_pointer_cast<RSNGRenderSDFRRectShape>(RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE));
    if (sdfRRectShape == nullptr) {
        ROSEN_LOGE(
            "RSEffectDrawableUtils::CreateDefaultRRectShape, SDF_RRECT_SHAPE is null, node %{public}" PRIu64, nodeId);
        return nullptr;
    }
    sdfRRectShape->Setter<SDFRRectShapeRRectRenderTag>(sdfRRect);
    return sdfRRectShape;
}

void RSEffectDrawableUtils::ApplySDFShapeToEffect(
    const RSProperties& properties, const std::shared_ptr<RSNGRenderShaderBase>& shader, NodeId nodeId)
{
    if (!shader) {
        return;
    }
    auto sdfShape = RSPropertyDrawableUtils::GetResolvedSDFShape(properties);
    if (sdfShape) {
        RSNGRenderShaderHelper::SetSDFShape(shader, sdfShape);
    } else {
        auto sdfRRect = properties.GetRRectForSDF();
        ROSEN_LOGD("RSEffectDrawableUtils::ApplySDFShapeToEffect, rrect %{public}s, node %{public}" PRIu64,
            sdfRRect.ToString().c_str(), nodeId);
        RSNGRenderShaderHelper::SetSDFShape(shader, CreateDefaultRRectShape(sdfRRect, nodeId));
    }
}

void RSEffectDrawableUtils::DrawColorUsingSDFWithDRM(Drawing::Canvas* canvas, const Drawing::Rect* rect, bool isDark,
    const std::shared_ptr<Drawing::GEVisualEffectContainer>& filterGEContainer, const std::string& filterTag,
    const std::string& shapeTag)
{
    if (UNLIKELY(canvas == nullptr)) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawColorUsingSDFWithDRM canvas is null");
        return;
    }
    if (rect == nullptr) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawColorUsingSDFWithDRM rect is null");
        return;
    }
    if (!filterGEContainer) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawColorUsingSDFWithDRM filterGEContainer null");
        return;
    }
    auto visualEffect = filterGEContainer->GetGEVisualEffect(filterTag);
    if (!visualEffect) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawColorUsingSDFWithDRM visualEffect null");
        return;
    }
    auto geShape = visualEffect->GetGEShaderShape(shapeTag);
    auto sdfColorVisualEffect =
        std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_SHADER_SDF_COLOR, Drawing::DrawingPaintType::BRUSH);
    sdfColorVisualEffect->SetParam(Drawing::GE_SHADER_SDF_COLOR_SHAPE, geShape);
    int16_t alpha = 245;             // give a nearly opaque mask to replace blur effect
    int16_t rgb = isDark ? 55 : 210; // RGB values of the color filter to be replaced in the DRM scenario
    Drawing::Color color(rgb, rgb, rgb, alpha);
    Vector4f geColor(color.GetRedF(), color.GetGreenF(), color.GetBlueF(), color.GetAlphaF());
    sdfColorVisualEffect->SetParam(Drawing::GE_SHADER_SDF_COLOR_COLOR, geColor);

    auto geContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    geContainer->AddToChainedFilter(sdfColorVisualEffect);
    auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
    geRender->DrawShaderEffect(*canvas, *geContainer, *rect);
}

RectF RSEffectDrawableUtils::CalcShaderDrawRect(
    const RSRenderNode& node, const RSProperties& properties, const std::shared_ptr<RSNGRenderShaderBase>& shader)
{
    // Expanded for out-of-bounds effects, equals bounds for effect types without an expansion
    // rule; empty-check consumers fall back to the slot rect.
    auto drawRect = RSNGRenderShaderHelper::CalcRect(shader, properties.GetBoundsRect());
    // Union nodes (gravity pull mode) have a bounding box larger than bounds; prefer the effect
    // rect only when the whole node fits in bounds, else cover the whole bounding box.
    return (properties.GetBoundsRect() == node.CalcBoundingBox() && !drawRect.IsEmpty()) ? drawRect
                                                                                         : node.CalcBoundingBox();
}
} // namespace DrawableV2
} // namespace OHOS::Rosen
