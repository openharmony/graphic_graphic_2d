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
#include "render/rs_effect_luminance_manager.h"

namespace OHOS::Rosen {
namespace DrawableV2 {

namespace {
constexpr size_t MAX_LIGHTS = 12;

void ClearPointLight(RSNGRenderPointLight& shader)
{
    shader.Setter<PointLightLightPositionsRenderTag>({});
    shader.Setter<PointLightLightColorsRenderTag>({});
    shader.Setter<PointLightLightIntensitiesRenderTag>({});
}

void CollectLightSource(std::vector<std::pair<RSLightSource, Vector4f>>& entries,
    const std::unordered_map<NodeId, std::pair<RSLightSource, Vector4f>>& lightMap)
{
    entries.clear();
    entries.reserve(lightMap.size());
    for (const auto& [nodeId, pair] : lightMap) {
        if (!pair.first.IsLightSourceValid()) {
            continue;
        }
        entries.push_back(pair);
    }
}

void SetPointLightPropToShader(const RSProperties& prop, RSNGRenderPointLight& shader)
{
    const auto& illuminated = prop.GetIlluminated();
    if (!illuminated || !illuminated->IsIlluminatedValid()) {
        return;
    }
    auto& lightMap = illuminated->GetLightSourcesAndPosMap();
    if (lightMap.empty()) {
        ClearPointLight(shader);
        return;
    }
    // Collect light sources with distance for sorting
    std::vector<std::pair<RSLightSource, Vector4f>> entries;
    CollectLightSource(entries, lightMap);

    // Sort by distance (closest first) and truncate to MAX_LIGHTS;
    if (entries.size() > MAX_LIGHTS) {
        std::sort(entries.begin(), entries.end(), [] (const auto& x, const auto& y) {
            return x.second.x_ * x.second.x_ + x.second.y_ * x.second.y_ <
                y.second.x_ * y.second.x_ + y.second.y_ * y.second.y_;
        });
        entries.resize(MAX_LIGHTS);
    }
    // Populate shader property tags
    std::vector<Vector4f> lightPositions;
    std::vector<Vector4f> lightColors;
    std::vector<float> lightIntensities;
    lightPositions.reserve(entries.size());
    lightColors.reserve(entries.size());
    lightIntensities.reserve(entries.size());
    for (const auto& entry : entries) {
        lightPositions.push_back(entry.second);
        auto color = entry.first.GetLightColor();
        auto colorVec = Vector4f(static_cast<float>(color.GetRed()), static_cast<float>(color.GetGreen()),
            static_cast<float>(color.GetBlue()), static_cast<float>(color.GetAlpha()));
        lightColors.push_back(colorVec);
        lightIntensities.push_back(entry.first.GetLightIntensity());
    }

    shader.Setter<PointLightLightPositionsRenderTag>(lightPositions);
    shader.Setter<PointLightLightColorsRenderTag>(lightColors);
    shader.Setter<PointLightLightIntensitiesRenderTag>(lightIntensities);

    bool enableEDREffect = illuminated->GetIlluminatedType() == IlluminatedType::NORMAL_BORDER_CONTENT &&
        std::any_of(entries.begin(), entries.end(), [] (const auto& pair) {
            return ROSEN_GNE(pair.first.GetLightIntensity(), 1.0f);
        });
    shader.Setter<PointLightEnableEDREffectRenderTag>(enableEDREffect);
}
} // namespace

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

void RSEffectDrawableUtils::DrawColorNGFilter(Drawing::Canvas* canvas, const Drawing::Rect* rect,
    const std::shared_ptr<Drawing::GEVisualEffectContainer>& visualEffectContainer)
{
    if (canvas == nullptr || rect == nullptr || visualEffectContainer == nullptr) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawColorNGFilter invalid input param");
        return;
    }
    auto surface = canvas->GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawColorNGFilter surface is nullptr");
        return;
    }
    Drawing::RectI drawRect = canvas->GetDeviceClipBounds();
    if (drawRect.IsEmpty()) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawColorNGFilter drawRect is empty");
        return;
    }
    auto image = surface->GetImageSnapshot(drawRect);
    if (image == nullptr) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawColorNGFilter image snapshot is nullptr");
        return;
    }
    Drawing::Rect localRect(0.f, 0.f,
        static_cast<float>(drawRect.GetWidth()), static_cast<float>(drawRect.GetHeight()));
    auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
    GraphicsEffectEngine::GERender::ShaderFilterEffectContext geContext {image, localRect, localRect, nullptr};
    auto resImage = geRender->ApplyImageEffect(*canvas, *visualEffectContainer, geContext,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE));
    if (resImage == nullptr) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawColorNGFilter ApplyImageEffect failed");
        return;
    }
    Drawing::Matrix identityMatrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*resImage, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), identityMatrix);
    if (imageShader == nullptr) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawColorNGFilter imageShader is nullptr");
        return;
    }
    Drawing::Brush brush;
    brush.SetShaderEffect(imageShader);
    brush.SetAntiAlias(true);
    Drawing::AutoCanvasRestore acr(*canvas, true);
    canvas->ResetMatrix();
    canvas->Translate(static_cast<float>(drawRect.GetLeft()), static_cast<float>(drawRect.GetTop()));
    canvas->DrawBackground(brush);
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

std::shared_ptr<RSNGRenderShaderBase> RSEffectDrawableUtils::FindShader(
    const std::shared_ptr<RSNGRenderShaderBase>& shader, RSNGEffectType type)
{
    if (!shader) {
        return nullptr;
    }

    // walk the shader chain to find such as POINT_LIGHT shaders
    auto current = shader;
    while (current) {
        if (current->GetType() == type) {
            return current;
        }
        current = current->GetNext();
    }
    return nullptr;
}

void RSEffectDrawableUtils::SetPointLightData(
    const std::shared_ptr<RSNGRenderShaderBase>& shader, const RSRenderNode& node)
{
    auto rsNGShader = FindShader(shader, RSNGEffectType::POINT_LIGHT);
    if (!rsNGShader) {
        return; // no POINT_LIGHT shader in the chain
    }
    auto pointLightShader = std::static_pointer_cast<RSNGRenderPointLight>(rsNGShader);
    const auto& properties = node.GetRenderProperties();

    pointLightShader->Setter<PointLightIlluminatedTypeRenderTag>(static_cast<int>(properties.GetIlluminatedType()));
    pointLightShader->Setter<PointLightBloomRenderTag>(properties.GetBloom());

    SetPointLightPropToShader(properties, *pointLightShader);

    // GEO property
    auto sdfShape = RSPropertyDrawableUtils::GetResolvedSDFShape(properties);
    pointLightShader->Setter<PointLightShapeRenderTag>(sdfShape);

    auto propRRect = properties.GetRRect();
    auto borderWidth = std::max(0.0f, std::ceil(properties.GetIlluminatedBorderWidth()));
    pointLightShader->Setter<PointLightIlluminatedTypeRenderTag>(borderWidth);
    Vector4f width = { borderWidth / 2.0f, borderWidth / 2.0f, borderWidth / 2.0f, borderWidth / 2.0f};
    auto borderRRect = propRRect.Inset(width);
    auto sdfDrawRect = RSPropertyDrawableUtils::Rect2Vector4f(node.CalcBoundingBox());
    pointLightShader->Setter<PointLightContentRRectRenderTag>(propRRect);
    pointLightShader->Setter<PointLightBorderRRectRenderTag>(borderRRect);
    pointLightShader->Setter<PointLightDrawRectRenderTag>(sdfDrawRect);
    RS_LOGD_IF(DEBUG_PIPELINE, "RSEffectDrawableUtils::SetPointLightData CoverageNGDrawLight contentRect:%{public}s "
        "borderRRect: %{public}s", propRRect.ToString().c_str(), borderRRect.ToString().c_str());
    // EDR: get displayHeadroom and determine if tone mapping is needed
    auto screenNodeId = node.GetScreenNodeId();
    float headRoom = RSEffectLuminanceManager::GetInstance().GetDisplayHeadroom(screenNodeId);
    pointLightShader->Setter<PointLightDisplayHeadroomRenderTag>(headRoom);
}
} // namespace DrawableV2
} // namespace OHOS::Rosen
