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

#include "ge_image_cache_provider.h"
#include "ge_render.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"

#include "common/rs_optional_trace.h"
#include "common/rs_vector4.h"
#include "drawable/rs_property_drawable_utils.h"
#include "effect/rs_render_filter_base.h"
#include "effect/rs_render_property_tag.h"
#include "effect/rs_render_shader_base.h"
#include "effect/rs_render_shape_base.h"
#include "feature/filter_cache_manager/rs_filter_cache_manager.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties.h"
#include "render/rs_blur_filter.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_effect_luminance_manager.h"
#include "render/rs_foreground_effect_filter.h"
#include "render/rs_material_filter.h"
#include "render/rs_motion_blur_filter.h"
#include "render/rs_pixel_stretch_params.h"
#include "render/rs_render_kawase_blur_filter.h"
#include "render/rs_render_linear_gradient_blur_filter.h"
#include "render/rs_render_maskcolor_filter.h"
#include "render/rs_render_mesa_blur_filter.h"
#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

namespace OHOS::Rosen {
namespace DrawableV2 {

namespace {
constexpr size_t MAX_LIGHTS = 12;
constexpr int TRACE_LEVEL_TWO = 2;

// DRM bypass mask color: a nearly opaque gray replacing the blur effect, dark or light per mode.
Drawing::Color GetDRMMaskColor(bool isDark)
{
    constexpr int16_t ALPHA = 245; // give a nearly opaque mask to replace blur effect
    const int16_t rgb = isDark ? 55 : 210; // RGB values of the color filter to be replaced in the DRM scenario
    return Drawing::Color(rgb, rgb, rgb, ALPHA);
}

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

void RSEffectDrawableUtils::DrawFilterWithDRM(Drawing::Canvas* canvas, bool isDark, const Drawing::Rect* rect,
    const std::shared_ptr<RSFilter>& filter)
{
    if (UNLIKELY(canvas == nullptr)) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawFilterWithDRM canvas is null");
        return;
    }
    std::shared_ptr<Drawing::GEVisualEffectContainer> geContainer;
    if (filter != nullptr && filter->IsDrawingFilter()) {
        geContainer = std::static_pointer_cast<RSDrawingFilter>(filter)->GetGEContainer();
    }
    if (rect != nullptr && geContainer != nullptr &&
        geContainer->GetGEVisualEffect(Drawing::GE_FILTER_FROSTED_GLASS) != nullptr) {
        DrawColorUsingSDFWithDRM(canvas, rect, isDark, geContainer, Drawing::GE_FILTER_FROSTED_GLASS,
            Drawing::GE_FILTER_FROSTED_GLASS_SHAPE);
        return;
    }
    Drawing::Brush brush;
    brush.SetColor(GetDRMMaskColor(isDark));
    canvas->DrawBackground(brush);
}

void RSEffectDrawableUtils::DrawColorUsingSDFWithDRM(Drawing::Canvas* canvas, const Drawing::Rect* rect, bool isDark,
    const std::shared_ptr<Drawing::GEVisualEffectContainer>& geContainer, const std::string& effectTag,
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
    if (!geContainer) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawColorUsingSDFWithDRM geContainer null");
        return;
    }
    auto visualEffect = geContainer->GetGEVisualEffect(effectTag);
    if (!visualEffect) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawColorUsingSDFWithDRM visualEffect null");
        return;
    }
    auto geShape = visualEffect->GetGEShaderShape(shapeTag);
    auto sdfColorVisualEffect =
        std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_SHADER_SDF_COLOR, Drawing::DrawingPaintType::BRUSH);
    sdfColorVisualEffect->SetParam(Drawing::GE_SHADER_SDF_COLOR_SHAPE, geShape);
    Drawing::Color color = GetDRMMaskColor(isDark);
    Vector4f geColor(color.GetRedF(), color.GetGreenF(), color.GetBlueF(), color.GetAlphaF());
    sdfColorVisualEffect->SetParam(Drawing::GE_SHADER_SDF_COLOR_COLOR, geColor);

    auto sdfColorContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    sdfColorContainer->AddToChainedFilter(sdfColorVisualEffect);
    auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
    geRender->DrawShaderEffect(*canvas, *sdfColorContainer, *rect);
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

void RSEffectDrawableUtils::DrawBackgroundEffect(RSPaintFilterCanvas* canvas,
    const std::shared_ptr<RSFilter>& rsFilter, NodeId filterId,
    const std::unique_ptr<RSFilterCacheManager>& cacheManager,
    const Drawing::RectI& bounds, bool behindWindow)
{
    if (rsFilter == nullptr) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawBackgroundEffect null filter");
        return;
    }
    if (canvas == nullptr) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawBackgroundEffect null canvas");
        return;
    }
    auto surface = canvas->GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawBackgroundEffect surface null");
        return;
    }
    RSEffectDrawableUtils::IncBlurCnt();
    auto clipIBounds = bounds;
    auto filter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);
    RS_OPTIONAL_TRACE_NAME("RSEffectDrawableUtils::DrawBackgroundEffect " + rsFilter->GetDescription());
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO, "EffectComponent, %s, bounds: %s",
        rsFilter->GetDetailedDescription().c_str(), clipIBounds.ToString().c_str());
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    // Optional use cacheManager to draw filter
    if (RSFilterCacheManager::IsFilterCacheEnabled() && cacheManager != nullptr && !canvas->GetDisableFilterCache() &&
        !cacheManager->IsFilterCacheMemExceedThreshold()) {
        if (cacheManager->GetCachedType() == FilterCacheType::FILTERED_SNAPSHOT) {
            RSEffectDrawableUtils::DecBlurCnt();
        }
        if (canvas->GetDeviceClipBounds().IsEmpty()) {
            return;
        }
        auto&& data = cacheManager->GeneratedCachedEffectData(*canvas, filter, filterId, clipIBounds, clipIBounds);
        cacheManager->CompactFilterCache(); // flag for clear witch cache after drawing
        behindWindow ? canvas->SetBehindWindowData(data) : canvas->SetEffectData(data);
        return;
    }
#endif
    auto imageRect = clipIBounds;
    auto imageSnapshot = surface->GetImageSnapshot(imageRect);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawBackgroundEffect image snapshot null");
        return;
    }
    filter->PreProcess(imageSnapshot);
    // create a offscreen skSurface
    std::shared_ptr<Drawing::Surface> offscreenSurface =
        surface->MakeSurface(imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
    if (offscreenSurface == nullptr) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawBackgroundEffect offscreenSurface null");
        return;
    }
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());
    auto clipBounds = Drawing::Rect(0, 0, imageRect.GetWidth(), imageRect.GetHeight());
    auto imageSnapshotBounds = Drawing::Rect(0, 0, imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
    filter->DrawImageRect(offscreenCanvas, imageSnapshot, imageSnapshotBounds, clipBounds);
    filter->PostProcess(offscreenCanvas);

    auto imageCache = offscreenSurface->GetImageSnapshot();
    if (imageCache == nullptr) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawBackgroundEffect imageCache snapshot null");
        return;
    }
    auto data = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(imageCache), std::move(imageRect));
    behindWindow ? canvas->SetBehindWindowData(std::move(data)) : canvas->SetEffectData(std::move(data));
}

void RSEffectDrawableUtils::ApplyAdaptiveFrostedGlassParams(
    Drawing::Canvas* canvas, const std::shared_ptr<RSDrawingFilter>& filter)
{
    if (canvas == nullptr || filter == nullptr) {
        return;
    }
    if (!filter->GetGEContainer() || !filter->GetGEContainer()->GetGEVisualEffect(Drawing::GE_FILTER_FROSTED_GLASS)) {
        return;
    }
    auto color = static_cast<RSPaintFilterCanvas*>(canvas)->GetColorPicked(ColorPlaceholder::SURFACE_CONTRAST);
    // assuming r, g, b are interpolated the same way
    constexpr float COLOR_MAX = 255.0f;
    // we use opposite color: dark if foreground is light
    const float darkScale = static_cast<float>(Drawing::Color::ColorQuadGetR(color)) / COLOR_MAX;
    filter->SetDarkScale(darkScale);
}

inline void RSEffectDrawableUtils::ClipVisibleRect(RSPaintFilterCanvas* canvas)
{
    auto visibleRectF = canvas->GetVisibleRect();
    visibleRectF.Round();
    auto visibleIRect = Drawing::RectI(
        static_cast<int32_t>(visibleRectF.GetLeft()), static_cast<int32_t>(visibleRectF.GetTop()),
        static_cast<int32_t>(visibleRectF.GetRight()), static_cast<int32_t>(visibleRectF.GetBottom()));
    auto deviceClipRect = canvas->GetDeviceClipBounds();
    if (!visibleIRect.IsEmpty() && deviceClipRect.Intersect(visibleIRect)) {
        canvas->ClipIRect(visibleIRect, Drawing::ClipOp::INTERSECT);
    }
}

void RSEffectDrawableUtils::DrawFilter(Drawing::Canvas* canvas,
    const std::shared_ptr<RSFilter>& rsFilter, const std::unique_ptr<RSFilterCacheManager>& cacheManager,
    NodeId nodeId, const bool isForegroundFilter,
    const std::optional<Drawing::RectI>& snapshotRect, const std::optional<Drawing::RectI>& drawRect)
{
    if (!RSSystemProperties::GetBlurEnabled()) {
        ROSEN_LOGD("RSEffectDrawableUtils::DrawFilter close blur.");
        return;
    }
    if (rsFilter == nullptr) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawFilter null filter.");
        return;
    }
    if (canvas == nullptr) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawFilter null canvas.");
        return;
    }

    auto filter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);
    auto clipIBounds = canvas->GetRoundInDeviceClipBounds();
    RS_OPTIONAL_TRACE_NAME("DrawFilter " + rsFilter->GetDescription());
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO, "DrawFilter, filterType: %d, %s, bounds: %s", isForegroundFilter,
        rsFilter->GetDetailedDescription().c_str(), clipIBounds.ToString().c_str());
    RSEffectDrawableUtils::IncBlurCnt();

    auto surface = canvas->GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawFilter surface null");
        Drawing::Brush brush = filter->GetBrush();
        Drawing::SaveLayerOps slr(nullptr, &brush, Drawing::SaveLayerOps::Flags::INIT_WITH_PREVIOUS);
        canvas->SaveLayer(slr);
        filter->PostProcess(*canvas);
        return;
    }

    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    if (paintFilterCanvas == nullptr) {
        return;
    }
    // Only draw within the visible rect.
    ClipVisibleRect(paintFilterCanvas);
    if (canvas->GetDeviceClipBounds().IsEmpty()) {
        return;
    }
    RSAutoCanvasRestore autoCanvasStore(paintFilterCanvas,
        isForegroundFilter ? RSPaintFilterCanvas::SaveType::kAlpha : RSPaintFilterCanvas::SaveType::kNone);
    if (isForegroundFilter) {
        paintFilterCanvas->SetAlpha(1.0);
    }
    auto imageClipIBounds = snapshotRect.value_or(clipIBounds);

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    // Optional use cacheManager to draw filter
    auto enableCache =
        (!paintFilterCanvas->GetDisableFilterCache() && cacheManager != nullptr &&
            RSFilterCacheManager::IsFilterCacheEnabled() && !cacheManager->IsFilterCacheMemExceedThreshold());
    if (enableCache) {
        if (cacheManager->GetCachedType() == FilterCacheType::FILTERED_SNAPSHOT) {
            RSEffectDrawableUtils::DecBlurCnt();
        }

        auto rsShaderFilter = filter->GetShaderFilterWithType(RSUIFilterType::LINEAR_GRADIENT_BLUR);
        if (rsShaderFilter != nullptr) {
            auto tmpFilter = std::static_pointer_cast<RSLinearGradientBlurShaderFilter>(rsShaderFilter);
            tmpFilter->IsOffscreenCanvas(true);
        }
        cacheManager->DrawFilter(*paintFilterCanvas, filter, nodeId, false, true, snapshotRect, drawRect);
        cacheManager->CompactFilterCache(); // flag for clear witch cache after drawing
        return;
    }
#endif

    auto rsShaderFilter = filter->GetShaderFilterWithType(RSUIFilterType::LINEAR_GRADIENT_BLUR);
    if (rsShaderFilter != nullptr) {
        auto tmpFilter = std::static_pointer_cast<RSLinearGradientBlurShaderFilter>(rsShaderFilter);
        tmpFilter->IsOffscreenCanvas(true);
    }

    if (imageClipIBounds.IsEmpty()) {
        return;
    }
    auto imageSnapshot = surface->GetImageSnapshot(imageClipIBounds, false);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSEffectDrawableUtils::DrawFilter image null");
        return;
    }
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(imageSnapshot->GetWidth(), imageSnapshot->GetHeight())) {
        ROSEN_LOGD("RSEffectDrawableUtils::DrawFilter cache image resource(w:%{public}d, h:%{public}d).",
            imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
        imageSnapshot->HintCacheGpuResource();
    }

    filter->PreProcess(imageSnapshot);
    Drawing::AutoCanvasRestore acr(*canvas, true);
    canvas->ResetMatrix();
    Drawing::Rect srcRect = Drawing::Rect(0, 0, imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
    Drawing::Rect dstRect = drawRect.value_or(clipIBounds);
    filter->DrawImageRect(*canvas, imageSnapshot, srcRect, dstRect);
    filter->PostProcess(*canvas);
}

bool RSEffectDrawableUtils::RSFilterSetPixelStretch(
    const RSProperties& property, const std::shared_ptr<RSFilter>& filter)
{
    if (!filter || !RSSystemProperties::GetMESABlurFuzedEnabled()) {
        return false;
    }
    auto drawingFilter = std::static_pointer_cast<RSDrawingFilter>(filter);
    auto mesaShaderFilter = drawingFilter->GetShaderFilterWithType(RSUIFilterType::MESA);
    if (!mesaShaderFilter) {
        return false;
    }

    auto& pixelStretch = property.GetPixelStretch();
    if (pixelStretch.IsZero()) {
        return false;
    }

    constexpr static float EPS = 1e-5f;
    // The pixel stretch is fuzed only when the stretch factors are negative
    if (pixelStretch.x_ > EPS || pixelStretch.y_ > EPS || pixelStretch.z_ > EPS || pixelStretch.w_ > EPS) {
        return false;
    }

    ROSEN_LOGD("RSEffectDrawableUtils::DrawPixelStretch fuzed with MESABlur.");
    const auto& boundsRect = property.GetBoundsRect();
    auto tileMode = property.GetPixelStretchTileMode();
    auto pixelStretchParams =
        std::make_shared<RSPixelStretchParams>(std::abs(pixelStretch.x_), std::abs(pixelStretch.y_),
                                               std::abs(pixelStretch.z_), std::abs(pixelStretch.w_),
                                               tileMode, boundsRect.width_, boundsRect.height_);
    auto mesaBlurFilter = std::static_pointer_cast<RSMESABlurShaderFilter>(mesaShaderFilter);
    mesaBlurFilter->SetPixelStretchParams(pixelStretchParams);
    return true;
}

void RSEffectDrawableUtils::RSFilterRemovePixelStretch(const std::shared_ptr<RSFilter>& filter)
{
    if (!filter) {
        return;
    }
    auto drawingFilter = std::static_pointer_cast<RSDrawingFilter>(filter);
    auto mesaShaderFilter = drawingFilter->GetShaderFilterWithType(RSUIFilterType::MESA);
    if (!mesaShaderFilter) {
        return;
    }

    ROSEN_LOGD("RSEffectDrawableUtils::remove pixel stretch from the fuzed blur");
    auto mesaBlurFilter = std::static_pointer_cast<RSMESABlurShaderFilter>(mesaShaderFilter);
    std::shared_ptr<RSPixelStretchParams> pixelStretchParams = nullptr;
    mesaBlurFilter->SetPixelStretchParams(pixelStretchParams);
}

std::shared_ptr<Drawing::ColorFilter> RSEffectDrawableUtils::GenerateMaterialColorFilter(float sat, float brt)
{
    float normalizedDegree = brt - 1.0;
    const float brightnessMat[] = {
        1.000000f, 0.000000f, 0.000000f, 0.000000f, normalizedDegree,
        0.000000f, 1.000000f, 0.000000f, 0.000000f, normalizedDegree,
        0.000000f, 0.000000f, 1.000000f, 0.000000f, normalizedDegree,
        0.000000f, 0.000000f, 0.000000f, 1.000000f, 0.000000f,
    };
    Drawing::ColorMatrix cm;
    cm.SetSaturation(sat);
    float cmArray[Drawing::ColorMatrix::MATRIX_SIZE];
    cm.GetArray(cmArray);
    return Drawing::ColorFilter::CreateComposeColorFilter(cmArray, brightnessMat, Drawing::Clamp::NO_CLAMP);
}

std::shared_ptr<RSFilter> RSEffectDrawableUtils::GenerateBehindWindowFilter(
    float radius, float saturation, float brightness, RSColor maskColor)
{
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO,
        "RSEffectDrawableUtils::GenerateBehindWindowFilter, Radius: %f, Saturation: %f, "
        "Brightness: %f, MaskColor: %08X", radius, saturation, brightness, maskColor.AsArgbInt());
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    uint32_t hash = hashFunc(&radius, sizeof(radius), 0);
    hash = hashFunc(&saturation, sizeof(saturation), hash);
    hash = hashFunc(&brightness, sizeof(brightness), hash);
    std::shared_ptr<Drawing::ColorFilter> colorFilter = GenerateMaterialColorFilter(saturation, brightness);
    std::shared_ptr<Drawing::ImageFilter> blurColorFilter =
        Drawing::ImageFilter::CreateColorBlurImageFilter(*colorFilter, radius, radius);
    std::shared_ptr<RSDrawingFilter> filter = nullptr;
    if (RSSystemProperties::GetKawaseEnabled()) {
        std::shared_ptr<RSKawaseBlurShaderFilter> kawaseBlurFilter = std::make_shared<RSKawaseBlurShaderFilter>(radius);
        auto colorImageFilter = Drawing::ImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr);
        filter = filter ? filter->Compose(colorImageFilter, hash)
                        : std::make_shared<RSDrawingFilter>(colorImageFilter, hash);
        filter = filter->Compose(std::static_pointer_cast<RSRenderFilterParaBase>(kawaseBlurFilter));
    } else {
        filter =
            filter ? filter->Compose(blurColorFilter, hash) : std::make_shared<RSDrawingFilter>(blurColorFilter, hash);
    }
    std::shared_ptr<RSMaskColorShaderFilter> maskColorShaderFilter =
        std::make_shared<RSMaskColorShaderFilter>(BLUR_COLOR_MODE::DEFAULT, maskColor);
    filter = filter->Compose(std::static_pointer_cast<RSRenderFilterParaBase>(maskColorShaderFilter));
    filter->SetSkipFrame(RSDrawingFilter::CanSkipFrame(radius));
    filter->SetSaturationForHPS(saturation);
    filter->SetBrightnessForHPS(brightness);
    filter->SetFilterType(RSFilter::MATERIAL);
    return filter;
}

bool RSEffectDrawableUtils::IsBlurFilterType(const RSFilter::FilterType& filterType)
{
    switch (filterType) {
        case RSFilter::BLUR:
        case RSFilter::MATERIAL:
        case RSFilter::LINEAR_GRADIENT_BLUR:
        case RSFilter::FOREGROUND_EFFECT:
        case RSFilter::MOTION_BLUR:
            return true;
        default:
            return false;
    }
}

float RSEffectDrawableUtils::GetBlurFilterRadius(const std::shared_ptr<RSFilter>& rsFilter)
{
    if (!rsFilter) {
        return 0;
    }
    switch (rsFilter->GetFilterType()) {
        case RSFilter::BLUR:
            return std::static_pointer_cast<RSBlurFilter>(rsFilter)->GetBlurRadiusX();
        case RSFilter::MATERIAL:
            return std::static_pointer_cast<RSMaterialFilter>(rsFilter)->GetRadius();
        case RSFilter::LINEAR_GRADIENT_BLUR: {
            auto filter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);
            auto rsShaderFilter = filter->GetShaderFilterWithType(RSUIFilterType::LINEAR_GRADIENT_BLUR);
            if (rsShaderFilter != nullptr) {
                auto tmpFilter = std::static_pointer_cast<RSLinearGradientBlurShaderFilter>(rsShaderFilter);
                return tmpFilter->GetLinearGradientBlurRadius();
            }
            return 0;
        }
        case RSFilter::FOREGROUND_EFFECT:
            return std::static_pointer_cast<RSForegroundEffectFilter>(rsFilter)->GetRadius();
        case RSFilter::MOTION_BLUR:
            return std::static_pointer_cast<RSMotionBlurFilter>(rsFilter)->GetRadius();
        default:
            return 0;
    }
}

void RSEffectDrawableUtils::ApplySDFShapeToMagnifier(
    const RSProperties& properties, const std::shared_ptr<RSNGRenderFilterBase>& renderFilter, NodeId nodeId)
{
    if (!renderFilter || renderFilter->GetType() != RSNGEffectType::MAGNIFIER) {
        return;
    }

    const auto& filter = std::static_pointer_cast<RSNGRenderMagnifierFilter>(renderFilter);
    auto sdfShape = properties.GetSDFShape();
    if (sdfShape) {
        filter->Setter<MagnifierSDFShapeRenderTag>(sdfShape, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
        ROSEN_LOGD("RSEffectDrawableUtils::ApplySDFShapeToMagnifier properties.GetSDFShape()");
        return;
    }
    auto nodeRRect = properties.GetRRectForSDF();
    auto width = filter->Getter<OHOS::Rosen::MagnifierWidthRenderTag>()->Get();
    auto height = filter->Getter<OHOS::Rosen::MagnifierHeightRenderTag>()->Get();
    auto radius = filter->Getter<OHOS::Rosen::MagnifierCornerRadiusRenderTag>()->Get();
    auto left = (nodeRRect.rect_.GetWidth() - width) * 0.5f;
    auto top = (nodeRRect.rect_.GetHeight() - height) * 0.5f;
    auto sdfRRect = RRect { RectT<float> { left, top, width, height }, radius, radius };
    auto shapeBase = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    auto sdfRRectShape = std::static_pointer_cast<RSNGRenderSDFRRectShape>(shapeBase);
    ROSEN_LOGD("RSEffectDrawableUtils::ApplySDFShapeToMagnifier, rrect %{public}s, node %{public}" PRIu64,
        sdfRRect.ToString().c_str(), nodeId);
    sdfRRectShape->Setter<SDFRRectShapeRRectRenderTag>(sdfRRect);
    filter->Setter<MagnifierSDFShapeRenderTag>(sdfRRectShape, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
}

void RSEffectDrawableUtils::ApplySDFShapeToFilter(
    const RSProperties& properties, const std::shared_ptr<RSDrawingFilter>& drawingFilter, NodeId nodeId)
{
    if (!drawingFilter) {
        return;
    }
    const auto& renderFilter = drawingFilter->GetNGRenderFilter();
    if (!renderFilter) {
        return;
    }
    // refactor if more SDF-based filters are added
    if (renderFilter->GetType() == RSNGEffectType::SDF_EDGE_LIGHT) {
        const auto& filter = std::static_pointer_cast<RSNGRenderSDFEdgeLightFilter>(renderFilter);
        auto sdfShape = properties.GetSDFShape();
        if (sdfShape) {
            ROSEN_LOGD("RSEffectDrawableUtils::ApplySDFShapeToFilter, SDF_EDGE_LIGHT, node %{public}" PRIu64, nodeId);
            filter->Setter<SDFEdgeLightSDFShapeRenderTag>(sdfShape, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
            drawingFilter->SetNGRenderFilter(filter);
        }
        return;
    }
    ApplySDFShapeToMagnifier(properties, renderFilter, nodeId);
    if (renderFilter->GetType() != RSNGEffectType::FROSTED_GLASS) {
        return;
    }
    const auto& filter = std::static_pointer_cast<RSNGRenderFrostedGlassFilter>(renderFilter);
    auto sdfShape = properties.GetSDFShape();
    if (sdfShape) {
        ROSEN_LOGD("RSEffectDrawableUtils::ApplySDFShapeToFilter, use sdfShape, node %{public}" PRIu64, nodeId);
        filter->Setter<FrostedGlassShapeRenderTag>(sdfShape, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
        drawingFilter->SetNGRenderFilter(filter);
        return;
    }
    auto sdfRRect = properties.GetRRectForSDF();
    auto sdfRRectShape =
        std::static_pointer_cast<RSNGRenderSDFRRectShape>(RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE));
    ROSEN_LOGD("RSEffectDrawableUtils::ApplySDFShapeToFilter, rrect %{public}s, node %{public}" PRIu64,
        sdfRRect.ToString().c_str(), nodeId);
    sdfRRectShape->Setter<SDFRRectShapeRRectRenderTag>(sdfRRect);
    filter->Setter<FrostedGlassShapeRenderTag>(sdfRRectShape, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    drawingFilter->SetNGRenderFilter(filter);
}

int RSEffectDrawableUtils::GetAndResetBlurCnt()
{
    return g_blurCnt.exchange(0);
}
} // namespace DrawableV2
} // namespace OHOS::Rosen
