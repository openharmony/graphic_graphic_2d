/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_property_drawable_foreground.h"

#include "ge_render.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"

#include "common/rs_obj_abs_geometry.h"
#include "drawable/rs_property_drawable_utils.h"
#include "effect/rs_render_shader_base.h"
#include "effect/rs_render_shape_base.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_point_light_manager.h"
#include "render/rs_effect_luminance_manager.h"
#include "render/rs_particles_drawable.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
namespace {
constexpr int PARAM_TWO = 2;
constexpr char NORMAL_LIGHT_SHADER_STRING[](R"(
    uniform vec2 iResolution;
    uniform float cornerRadius;
    uniform vec4 lightPos[12];
    uniform vec4 viewPos[12];
    uniform vec4 specularLightColor[12];
    uniform float specularStrength[12];
    uniform float roundCurvature;
    uniform float bumpFactor;
    uniform float gradientRadius;
    uniform float bulgeRadius;
    uniform float attenuationCoeff;
    uniform float eps;

    float sdf_g2_rounded_box(vec2 p, vec2 b, float r, float k)
    {
        vec2 q = abs(p) - b;
        float v_x =  max(q.x, 0.0);
        float v_y =  max(q.y, 0.0);
        float dist_corner_shape;
        if (abs(k - 2.0) < 0.001) {
            dist_corner_shape = length(vec2(v_x, v_y));
            return dist_corner_shape + min(max(q.x, q.y), 0.0) - r;
        } else {
            if (v_x == 0.0 && v_y == 0.0) {
                dist_corner_shape = 0.0;
            } else {
                dist_corner_shape = pow(pow(v_x, k) + pow(v_y, k), 1.0 / k);
            }
            return dist_corner_shape + min(max(q.x, q.y), 0.0) - r;
        }
    }

    float sdScene(vec2 fragCoord)
    {
        vec2 boxSize = iResolution.xy * 0.5 - cornerRadius;
        vec2 boxCenter = iResolution.xy * 0.5;
        vec2 p = fragCoord - boxCenter;
        return sdf_g2_rounded_box(p, boxSize, cornerRadius, roundCurvature);
    }

    float height(vec2 p, float radius)
    {
        float d = sdScene(p);
        float h = 0.0;
        if (d < 0.0 && d > -radius) {
            float arg = 2.0 * radius * abs(d) - d * d;
            h = sqrt(max(arg, 0.0));
        } else if (d <= -radius) {
            h = radius;
        }
        return h;
    }

    vec2 computeGradient(vec2 p) {
        vec2 epsVec = vec2(eps, 0.0);
        float dx = height(p - epsVec.xy, gradientRadius) - height(p + epsVec.xy, gradientRadius);
        float dy = height(p - epsVec.yx, gradientRadius) - height(p + epsVec.yx, gradientRadius);
        return vec2(dx, dy);
    }

    mediump vec4 main(vec2 drawing_coord)
    {
        float shininess = 8.0;
        mediump vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0);
        vec2 grad = computeGradient(drawing_coord);
        float h = height(drawing_coord, bulgeRadius);
        vec3 norm = normalize(vec3(grad * bumpFactor, h));

        for (int i = 0; i < 12; i++) {
            if (abs(specularStrength[i]) > 0.01) {
                vec3 lightVec = vec3(lightPos[i].xy - drawing_coord, lightPos[i].z);
                vec3 lightDir = normalize(lightVec);
                vec3 viewDir = normalize(lightVec); // view vector
                vec3 halfwayDir = normalize(lightDir + viewDir); // half vector

                // lightPos[i].w stores the maximum lighting radius
                float normalizedDistance = length(lightVec) / lightPos[i].w;
                float smoothFactor = smoothstep(0.8, 1.0, normalizedDistance);
                float attenuation = (1.0 - smoothFactor) / (1.0 + attenuationCoeff *
                    normalizedDistance * normalizedDistance);
                float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
                vec4 specularColor = specularLightColor[i];
                fragColor += specularColor * spec * specularStrength[i] * attenuation;
            }
        }
        return vec4(fragColor.rgb, clamp(fragColor.a, 0.0, 1.0));
    }
)");

constexpr float SDR_LUMINANCE = 1.0f;
} // namespace

const bool FOREGROUND_FILTER_ENABLED = RSSystemProperties::GetForegroundFilterEnabled();

// ====================================
// Binarization
RSDrawable::Ptr RSBinarizationDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBinarizationDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSBinarizationDrawable::OnUpdate(const RSRenderNode& node)
{
    auto& aiInvert = node.GetRenderProperties().GetAiInvert();
    if (!aiInvert.has_value()) {
        return false;
    }
    needSync_ = true;
    stagingAiInvert_ = aiInvert;
    return true;
}

void RSBinarizationDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    aiInvert_ = std::move(stagingAiInvert_);
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSBinarizationDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSBinarizationDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
            RSTagTracker::SOURCETYPE::SOURCE_RSBINARIZATIONDRAWABLE);
#endif
        RSPropertyDrawableUtils::DrawBinarization(canvas, ptr->aiInvert_);
    };
}

RSDrawable::Ptr RSColorFilterDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSColorFilterDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSColorFilterDrawable::OnUpdate(const RSRenderNode& node)
{
    auto& colorFilter = node.GetRenderProperties().GetColorFilter();
    if (colorFilter == nullptr) {
        return false;
    }
    needSync_ = true;
    stagingFilter_ = colorFilter;
    return true;
}

void RSColorFilterDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    filter_ = std::move(stagingFilter_);
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSColorFilterDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSColorFilterDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
            RSTagTracker::SOURCETYPE::SOURCE_RSCOLORFILTERDRAWABLE);
#endif
        RSPropertyDrawableUtils::DrawColorFilter(canvas, ptr->filter_);
    };
}
RSDrawable::Ptr RSLightUpEffectDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSLightUpEffectDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSLightUpEffectDrawable::OnUpdate(const RSRenderNode& node)
{
    if (!node.GetRenderProperties().IsLightUpEffectValid()) {
        return false;
    }
    needSync_ = true;
    stagingLightUpEffectDegree_ = node.GetRenderProperties().GetLightUpEffect();
    return true;
}

void RSLightUpEffectDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    lightUpEffectDegree_ = stagingLightUpEffectDegree_;
    stagingLightUpEffectDegree_ = 1.0f;
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSLightUpEffectDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSLightUpEffectDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
            RSTagTracker::SOURCETYPE::SOURCE_RSLIGHTUPEFFECTDRAWABLE);
#endif
        RSPropertyDrawableUtils::DrawLightUpEffect(canvas, ptr->lightUpEffectDegree_);
    };
}

RSDrawable::Ptr RSDynamicDimDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSDynamicDimDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSDynamicDimDrawable::OnUpdate(const RSRenderNode& node)
{
    if (!node.GetRenderProperties().IsDynamicDimValid()) {
        return false;
    }
    needSync_ = true;
    stagingDynamicDimDegree_ = node.GetRenderProperties().GetDynamicDimDegree().value();
    return true;
}

void RSDynamicDimDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    dynamicDimDegree_ = stagingDynamicDimDegree_;
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSDynamicDimDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSDynamicDimDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
            RSTagTracker::SOURCETYPE::SOURCE_RSDYNAMICDIMDRAWABLE);
#endif
        RSPropertyDrawableUtils::DrawDynamicDim(canvas, ptr->dynamicDimDegree_);
    };
}

RSDrawable::Ptr RSForegroundColorDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSForegroundColorDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSForegroundColorDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto fgColor = properties.GetForegroundColor();
    if (fgColor == RgbPalette::Transparent()) {
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color(fgColor.AsArgbInt()));
    brush.SetAntiAlias(true);
    canvas.AttachBrush(brush);
    canvas.DrawRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(properties.GetRRect()));
    canvas.DetachBrush();
    return true;
}

RSDrawable::Ptr RSForegroundShaderDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSForegroundShaderDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

void RSForegroundShaderDrawable::PostUpdate(const RSRenderNode& node)
{
    enableEDREffect_ = RSNGRenderShaderHelper::CheckEnableEDR(stagingShader_);
    if (enableEDREffect_) {
        screenNodeId_ = node.GetScreenNodeId();
    }
}

bool RSForegroundShaderDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    const auto& shader = properties.GetForegroundShader();
    if (!shader) {
        return false;
    }
    needSync_ = true;
    stagingShader_ = shader;
    PostUpdate(node);
    return true;
}

void RSForegroundShaderDrawable::OnSync()
{
    if (needSync_ && stagingShader_) {
        auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
        stagingShader_->AppendToGEContainer(visualEffectContainer);
        visualEffectContainer->UpdateCacheDataFrom(visualEffectContainer_);
        visualEffectContainer_ = visualEffectContainer;
        needSync_ = false;
    }
}

Drawing::RecordingCanvas::DrawFunc RSForegroundShaderDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSForegroundShaderDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
        if (canvas == nullptr || ptr->visualEffectContainer_ == nullptr || rect == nullptr) {
            return;
        }
        geRender->DrawShaderEffect(*canvas, *(ptr->visualEffectContainer_), *rect);
    };
}

RSDrawable::Ptr RSCompositingFilterDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSCompositingFilterDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSCompositingFilterDrawable::OnUpdate(const RSRenderNode& node)
{
    stagingNodeId_ = node.GetId();
    stagingNodeName_ = node.GetNodeName();
    auto& rsFilter = node.GetRenderProperties().GetFilter();
    if (rsFilter == nullptr) {
        return false;
    }
    RecordFilterInfos(rsFilter);
    needSync_ = true;
    stagingFilter_ = rsFilter;
    PostUpdate(node);
    return true;
}

// foregroundFilter
RSDrawable::Ptr RSForegroundFilterDrawable::OnGenerate(const RSRenderNode& node)
{
    if (!FOREGROUND_FILTER_ENABLED) {
        ROSEN_LOGD("RSForegroundFilterDrawable::OnGenerate close blur.");
        return nullptr;
    }
    auto& rsFilter = node.GetRenderProperties().GetForegroundFilter();
    if (rsFilter == nullptr) {
        return nullptr;
    }

    if (auto ret = std::make_shared<RSForegroundFilterDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSForegroundFilterDrawable::OnUpdate(const RSRenderNode& node)
{
    auto& rsFilter = node.GetRenderProperties().GetForegroundFilter();
    if (rsFilter == nullptr) {
        return false;
    }
    needSync_ = true;
    stagingBoundsRect_ = node.GetRenderProperties().GetBoundsRect();
    return true;
}

Drawing::RecordingCanvas::DrawFunc RSForegroundFilterDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSForegroundFilterDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(paintFilterCanvas ? paintFilterCanvas->GetGPUContext() : nullptr,
            RSTagTracker::SOURCETYPE::SOURCE_RSFOREGROUNDFILTERDRAWABLE);
#endif
        RSPropertyDrawableUtils::BeginForegroundFilter(*paintFilterCanvas, ptr->boundsRect_);
    };
}

void RSForegroundFilterDrawable::OnSync()
{
    if (needSync_ == false) {
        return;
    }
    boundsRect_ = stagingBoundsRect_;
    needSync_ = false;
}

// Restore RSForegroundFilter
RSDrawable::Ptr RSForegroundFilterRestoreDrawable::OnGenerate(const RSRenderNode& node)
{
    if (!FOREGROUND_FILTER_ENABLED) {
        ROSEN_LOGD("RSForegroundFilterRestoreDrawable::OnGenerate close blur.");
        return nullptr;
    }
    auto& rsFilter = node.GetRenderProperties().GetForegroundFilter();
    if (rsFilter == nullptr) {
        return nullptr;
    }

    if (auto ret = std::make_shared<RSForegroundFilterRestoreDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSForegroundFilterRestoreDrawable::OnUpdate(const RSRenderNode& node)
{
    stagingNodeId_ = node.GetId();
    auto& rsFilter = node.GetRenderProperties().GetForegroundFilter();
    if (rsFilter == nullptr) {
        return false;
    }
    needSync_ = true;
    stagingForegroundFilter_ = rsFilter;
    return true;
}

Drawing::RecordingCanvas::DrawFunc RSForegroundFilterRestoreDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSForegroundFilterRestoreDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(paintFilterCanvas ? paintFilterCanvas->GetGPUContext() : nullptr,
            RSTagTracker::SOURCETYPE::SOURCE_RSFOREGROUNDFILTERRESTOREDRAWABLE);
#endif
        RS_TRACE_NAME_FMT("RSForegroundFilterRestoreDrawable::CreateDrawFunc node[%llu] ", ptr->renderNodeId_);
        RSPropertyDrawableUtils::DrawForegroundFilter(*paintFilterCanvas, ptr->foregroundFilter_);
    };
}

void RSForegroundFilterRestoreDrawable::OnSync()
{
    if (needSync_ == false) {
        return;
    }
    renderNodeId_ = stagingNodeId_;
    foregroundFilter_ = std::move(stagingForegroundFilter_);
    if (foregroundFilter_) {
        foregroundFilter_->OnSync();
    }
    needSync_ = false;
}

RSDrawable::Ptr RSPixelStretchDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSPixelStretchDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSPixelStretchDrawable::OnUpdate(const RSRenderNode& node)
{
    auto& pixelStretch = node.GetRenderProperties().GetPixelStretch();
    if (!pixelStretch.has_value()) {
        return false;
    }
    needSync_ = true;
    stagingNodeId_ = node.GetId();
    stagingPixelStretch_ = pixelStretch;
    stagePixelStretchTileMode_ = node.GetRenderProperties().GetPixelStretchTileMode();
    const auto& boundsGeo = node.GetRenderProperties().GetBoundsGeometry();
    stagingBoundsGeoValid_ = boundsGeo && !boundsGeo->IsEmpty();
    stagingBoundsRect_ = node.GetRenderProperties().GetBoundsRect();
    return true;
}

void RSPixelStretchDrawable::SetPixelStretch(const std::optional<Vector4f>& pixelStretch)
{
    stagingPixelStretch_ = pixelStretch;
}

const std::optional<Vector4f>& RSPixelStretchDrawable::GetPixelStretch() const
{
    return stagingPixelStretch_;
}

void RSPixelStretchDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    renderNodeId_ = stagingNodeId_;
    pixelStretch_ = std::move(stagingPixelStretch_);
    pixelStretchTileMode_ = stagePixelStretchTileMode_;
    boundsGeoValid_ = stagingBoundsGeoValid_;
    stagingBoundsGeoValid_ = false;
    boundsRect_ = stagingBoundsRect_;
    stagingBoundsRect_.Clear();
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSPixelStretchDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSPixelStretchDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
            RSTagTracker::SOURCETYPE::SOURCE_RSPIXELSTRETCHDRAWABLE);
#endif
        RSPropertyDrawableUtils::DrawPixelStretch(canvas, ptr->pixelStretch_, ptr->boundsRect_, ptr->boundsGeoValid_,
            static_cast<Drawing::TileMode>(ptr->pixelStretchTileMode_));
    };
}

RSDrawable::Ptr RSBorderDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBorderDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSBorderDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto& border = properties.GetBorder();
    if (!border || !border->HasBorder()) {
        return false;
    }
    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    DrawBorder(properties, *updater.GetRecordingCanvas(), border, false);
    return true;
}

void RSBorderDrawable::DrawBorder(const RSProperties& properties, Drawing::Canvas& canvas,
    const std::shared_ptr<RSBorder>& border, const bool& isOutline)
{
    auto sdfShape = properties.GetSDFShape();
    if (sdfShape && border->GetStyle() == BorderStyle::SOLID) {
        std::shared_ptr<Drawing::GEVisualEffect> geVisualEffect = sdfShape->GenerateGEVisualEffect();
        std::shared_ptr<Drawing::GEShaderShape> geShape =
            geVisualEffect ? geVisualEffect->GenerateShaderShape() : nullptr;
        auto geFilter = std::make_shared<Drawing::GEVisualEffect>(
            Drawing::GE_SHADER_SDF_BORDER, Drawing::DrawingPaintType::BRUSH);
        geFilter->SetParam(Drawing::GE_SHADER_SDF_BORDER_SHAPE, geShape);
        Drawing::GESDFBorderParams borderParam;
        auto borderColor = border->GetColor();
        borderParam.color = Drawing::Color(
            borderColor.GetRed(), borderColor.GetGreen(), borderColor.GetBlue(), borderColor.GetAlpha());
        borderParam.width = border->GetWidth();
        geFilter->SetParam(Drawing::GE_SHADER_SDF_BORDER_BORDER, borderParam);
        std::shared_ptr<Drawing::GEVisualEffectContainer> geContainer_ =
            std::make_shared<Drawing::GEVisualEffectContainer>();
        geContainer_->AddToChainedFilter(geFilter);
        auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
        bool isZero = isOutline ? border->GetRadiusFour().IsZero() : properties.GetCornerRadius().IsZero();
        Drawing::Rect rect;
        if (isZero) {
            rect = RSPropertyDrawableUtils::Rect2DrawingRect(isOutline ?
                properties.GetBoundsRect().MakeOutset(border->GetWidthFour()) : properties.GetBoundsRect());
        } else {
            rect = RSPropertyDrawableUtils::RRect2DrawingRRect(
                RSPropertyDrawableUtils::GetRRectForDrawingBorder(properties, border, isOutline)).GetRect();
        }
        geRender->DrawShaderEffect(canvas, *geContainer_, rect);
        return;
    }
    Drawing::Brush brush;
    Drawing::Pen pen;
    brush.SetAntiAlias(true);
    pen.SetAntiAlias(true);
    if (border->ApplyFillStyle(brush)) {
        auto roundRect = RSPropertyDrawableUtils::RRect2DrawingRRect(
            RSPropertyDrawableUtils::GetRRectForDrawingBorder(properties, border, isOutline));
        auto innerRoundRect = RSPropertyDrawableUtils::RRect2DrawingRRect(
            RSPropertyDrawableUtils::GetInnerRRectForDrawingBorder(properties, border, isOutline));
        canvas.AttachBrush(brush);
        canvas.DrawNestedRoundRect(roundRect, innerRoundRect);
        canvas.DetachBrush();
        return;
    }
    bool isZero = isOutline ? border->GetRadiusFour().IsZero() : properties.GetCornerRadius().IsZero();
    if (isZero && border->ApplyFourLine(pen)) {
        RectF rectf =
            isOutline ? properties.GetBoundsRect().MakeOutset(border->GetWidthFour()) : properties.GetBoundsRect();
        border->PaintFourLine(canvas, pen, rectf);
        return;
    }
    if (border->ApplyPathStyle(pen)) {
        auto borderWidth = border->GetWidth();
        RRect rrect = RSPropertyDrawableUtils::GetRRectForDrawingBorder(properties, border, isOutline);
        rrect.rect_.width_ -= borderWidth;
        rrect.rect_.height_ -= borderWidth;
        rrect.rect_.Move(borderWidth / PARAM_TWO, borderWidth / PARAM_TWO);
        Drawing::Path borderPath;
        borderPath.AddRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(rrect));
        canvas.AttachPen(pen);
        canvas.DrawPath(borderPath);
        canvas.DetachPen();
        return;
    }

    RSBorderGeo borderGeo;
    borderGeo.rrect = RSPropertyDrawableUtils::RRect2DrawingRRect(
        RSPropertyDrawableUtils::GetRRectForDrawingBorder(properties, border, isOutline));
    borderGeo.innerRRect = RSPropertyDrawableUtils::RRect2DrawingRRect(
        RSPropertyDrawableUtils::GetInnerRRectForDrawingBorder(properties, border, isOutline));
    auto centerX = borderGeo.innerRRect.GetRect().GetLeft() + borderGeo.innerRRect.GetRect().GetWidth() / 2;
    auto centerY = borderGeo.innerRRect.GetRect().GetTop() + borderGeo.innerRRect.GetRect().GetHeight() / 2;
    borderGeo.center = { centerX, centerY };
    auto rect = borderGeo.rrect.GetRect();
    Drawing::AutoCanvasRestore acr(canvas, false);
    Drawing::SaveLayerOps slr(&rect, nullptr);
    canvas.SaveLayer(slr);
    border->DrawBorders(canvas, pen, borderGeo);
}

RSDrawable::Ptr RSOutlineDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSOutlineDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSOutlineDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto& outline = properties.GetOutline();
    if (!outline || !outline->HasBorder()) {
        return false;
    }
    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    RSBorderDrawable::DrawBorder(properties, *updater.GetRecordingCanvas(), outline, true);
    return true;
}

RSDrawable::Ptr RSPointLightDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSPointLightDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

Drawing::RecordingCanvas::DrawFunc RSPointLightDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSPointLightDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
            RSTagTracker::SOURCETYPE::SOURCE_RSPOINTLIGHTDRAWABLE);
#endif
        ptr->DrawLight(canvas);
    };
}

bool RSPointLightDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    const auto& illuminatedPtr = properties.GetIlluminated();
    if (!illuminatedPtr || !illuminatedPtr->IsIlluminatedValid()) {
        return false;
    }
    const auto& lightSourcesAndPosMap = illuminatedPtr->GetLightSourcesAndPosMap();
    if (lightSourcesAndPosMap.empty()) {
        stagingEnableEDREffect_ = false;
        return true;
    }
    stagingLightSourcesAndPosVec_.clear();
    stagingLightSourcesAndPosVec_.reserve(lightSourcesAndPosMap.size());
    stagingLightSourcesAndPosVec_.assign(lightSourcesAndPosMap.begin(), lightSourcesAndPosMap.end());
    stagingIlluminatedType_ = illuminatedPtr->GetIlluminatedType();
    stagingBorderWidth_ = std::max(0.0f, std::ceil(properties.GetIlluminatedBorderWidth()));
    stagingRRect_ = RRect(properties.GetRRect());
    stagingNodeId_ = node.GetId();
    stagingScreenNodeId_ = node.GetScreenNodeId();
    auto begin = stagingLightSourcesAndPosVec_.begin();
    auto end = begin + std::min(static_cast<size_t>(MAX_LIGHT_SOURCES), stagingLightSourcesAndPosVec_.size());
    bool needEDR =
        std::any_of(begin, end, [](const auto& pair) { return ROSEN_GNE(pair.first->GetLightIntensity(), 1.0f); });
    stagingEnableEDREffect_ = needEDR && stagingIlluminatedType_ == IlluminatedType::NORMAL_BORDER_CONTENT;
    needSync_ = true;
    return true;
}

void RSPointLightDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    lightSourcesAndPosVec_ = std::move(stagingLightSourcesAndPosVec_);
    if (lightSourcesAndPosVec_.size() > MAX_LIGHT_SOURCES) {
        std::sort(lightSourcesAndPosVec_.begin(), lightSourcesAndPosVec_.end(), [](const auto& x, const auto& y) {
            return x.second.x_ * x.second.x_ + x.second.y_ * x.second.y_ <
                   y.second.x_ * y.second.x_ + y.second.y_ * y.second.y_;
        });
    }
    illuminatedType_ = stagingIlluminatedType_;
    enableEDREffect_ = stagingEnableEDREffect_;
    borderWidth_ = stagingBorderWidth_;
    screenNodeId_ = stagingScreenNodeId_;
    nodeId_ = stagingNodeId_;
    // half width and half height requires divide by 2.0f
    Vector4f width = { borderWidth_ / 2.0f };
    auto borderRRect = stagingRRect_.Inset(width);
    borderRRect_ = RSPropertyDrawableUtils::RRect2DrawingRRect(borderRRect);
    contentRRect_ = RSPropertyDrawableUtils::RRect2DrawingRRect(stagingRRect_);

    if (enableEDREffect_) {
        displayHeadroom_ = RSEffectLuminanceManager::GetInstance().GetDisplayHeadroom(screenNodeId_);
    }
    needSync_ = false;
}

float RSPointLightDrawable::GetBrightnessMapping(float headroom, float input)
{
    if (ROSEN_GE(headroom, EFFECT_MAX_LUMINANCE)) {
        return input;
    }
    // Bezier curves describing tone mapping rule, anchors define
    static const std::vector<Vector2f> hdrCurveAnchors = {
        {0.0f, 0.0f}, {0.75f, 0.75f}, {1.0f, 0.95f}, {1.25f, 0.97f},
        {1.5f, 0.98f}, {1.75, 0.9875f}, {1.90f, 0.995f}, {2.0f, 1.0f},
    };

    // Bezier curves describing tone mapping rule, control points define
    static const std::vector<Vector2f> hdrCurveControlPoint = {
        {0.375f, 0.375f}, {0.875, 0.875f}, {1.125f, 0.9625f}, {1.375f, 0.9725f},
        {1.625f, 0.98f}, {1.825f, 0.99f}, {1.95f, 0.998f}, // control points one less than anchor points
    };

    int rangeIndex = -1;
    const float epsilon = 1e-6f;
    for (size_t i = 0; i < hdrCurveAnchors.size(); ++i) {
        if (input - hdrCurveAnchors[i].x_ < epsilon) {
            rangeIndex = static_cast<int>(i) - 1;
            break;
        }
    }
    if (rangeIndex == -1) {
        rangeIndex = static_cast<int>(hdrCurveAnchors.size()) - PARAM_TWO;
    }
    if (rangeIndex >= static_cast<int>(hdrCurveAnchors.size()) - 1) {
        return headroom;
    }
    if (rangeIndex >= static_cast<int>(hdrCurveControlPoint.size())) {
        return headroom;
    }

    // calculate new hdr bightness via bezier curve
    Vector2f start = hdrCurveAnchors[rangeIndex];
    Vector2f end = hdrCurveAnchors[rangeIndex + 1];
    Vector2f control = hdrCurveControlPoint[rangeIndex];

    std::optional<float> resultYOptional = CalcBezierResultY(start, end, control, input);
    const float bezierY = resultYOptional.value_or(input);
    const float weightHDR = (EFFECT_MAX_LUMINANCE - headroom) / (EFFECT_MAX_LUMINANCE - SDR_LUMINANCE);
    const float weightSDR = (headroom - SDR_LUMINANCE) / (EFFECT_MAX_LUMINANCE - SDR_LUMINANCE);
    const float interpolationedY = weightHDR * bezierY + weightSDR * input;
    return std::clamp(interpolationedY, 0.0f, headroom);
}

std::optional<float> RSPointLightDrawable::CalcBezierResultY(
    const Vector2f& start, const Vector2f& end, const Vector2f& control, float x)
{
    // Solve quadratic beziier formula with root formula
    const float a = start[0] - 2 * control[0] + end[0];
    const float b = 2 * (control[0] - start[0]);
    const float c = start[0] - x;
    constexpr float FOUR = 4.0f;
    constexpr float TWO = 2.0f;
    const float discriminant = b * b - FOUR * a * c;

    if (ROSEN_LNE(discriminant, 0.0f)) {
        return std::nullopt;
    }
    float t = 0.0f;
    if (ROSEN_EQ(a, 0.0f) && ROSEN_NE(b, 0.0f)) {
        t = -c / b;
    } else {
        const float sqrtD = std::sqrt(discriminant);
        const float t1 = (-b + sqrtD) / (TWO * a);
        const float t2 = (-b - sqrtD) / (TWO * a);
        if (ROSEN_GE(t1, 0.0f) && ROSEN_LE(t1, 1.0f)) {
            t = t1;
        } else if (ROSEN_GE(t2, 0.0f) && ROSEN_LE(t2, 1.0f)) {
            t = t2;
        }
    }

    return start[1] + t * (TWO * (control[1] - start[1]) + t * (start[1] - TWO * control[1] + end[1]));
}

bool RSPointLightDrawable::NeedToneMapping(float supportHeadroom)
{
    return ROSEN_GNE(supportHeadroom, 0.0f) && ROSEN_LNE(supportHeadroom, EFFECT_MAX_LUMINANCE);
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> RSPointLightDrawable::MakeFeatheringBoardLightShaderBuilder() const
{
    auto builder = GetFeatheringBoardLightShaderBuilder();
    if (!builder) {
        return nullptr;
    }
    float rectWidth = contentRRect_.GetRect().GetWidth();
    float rectHeight = contentRRect_.GetRect().GetHeight();
    builder->SetUniform("iResolution", rectWidth, rectHeight);
    builder->SetUniform("contentBorderRadius",
        contentRRect_.GetCornerRadius(Drawing::RoundRect::CornerPos::TOP_LEFT_POS).GetX());
    builder->SetUniform("borderWidth", borderWidth_);
    return builder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> RSPointLightDrawable::MakeNormalLightShaderBuilder() const
{
    auto builder = GetNormalLightShaderBuilder();
    if (!builder) {
        return nullptr;
    }
    constexpr float DEFAULT_BUMP_FACTOR = 1.0f;
    constexpr float DEFAULT_GRADIENT_RADIUS = 6.0f;
    constexpr float DEFAULT_BULGE_RADIUS = 8.0f;
    constexpr float DEFAULT_EPSILON = 2.0f;
    constexpr float DEFAULT_ATTENUATION_COEFF = 0.3f;
    constexpr float CORNER_RADIUS_SCALE_FACTOR = 1.31f;
    constexpr float CORNER_THRESHOLD_FACTOR = 1.7f;
    constexpr float G2_CURVEATURE_K = 2.77f;
    constexpr float G1_CURVEATURE_K = 2.0f;
    float rectWidth = contentRRect_.GetRect().GetWidth();
    float rectHeight = contentRRect_.GetRect().GetHeight();
    builder->SetUniform("iResolution", rectWidth, rectHeight);
    float cornerRadius = contentRRect_.GetCornerRadius(Drawing::RoundRect::CornerPos::TOP_LEFT_POS).GetX();
    builder->SetUniform("bumpFactor", DEFAULT_BUMP_FACTOR);
    builder->SetUniform("gradientRadius", DEFAULT_GRADIENT_RADIUS);
    builder->SetUniform("bulgeRadius", DEFAULT_BULGE_RADIUS);
    builder->SetUniform("eps", DEFAULT_EPSILON);
    builder->SetUniform("attenuationCoeff", DEFAULT_ATTENUATION_COEFF);
    if (cornerRadius * CORNER_THRESHOLD_FACTOR * PARAM_TWO < std::min(rectWidth, rectHeight)) {
        builder->SetUniform("roundCurvature", G2_CURVEATURE_K);
        builder->SetUniform("cornerRadius", cornerRadius * CORNER_RADIUS_SCALE_FACTOR);
    } else {
        builder->SetUniform("roundCurvature", G1_CURVEATURE_K);
        builder->SetUniform("cornerRadius", cornerRadius);
    }
    return builder;
}

void RSPointLightDrawable::DrawLight(Drawing::Canvas* canvas) const
{
    if (lightSourcesAndPosVec_.empty()) {
        return;
    }
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    if (illuminatedType_ == IlluminatedType::NORMAL_BORDER_CONTENT) {
        builder = MakeNormalLightShaderBuilder();
    } else if (illuminatedType_ == IlluminatedType::FEATHERING_BORDER) {
        builder = MakeFeatheringBoardLightShaderBuilder();
    } else {
        builder = GetPhongShaderBuilder();
    }
    if (!builder) {
        return;
    }
    constexpr int vectorLen = 4;
    float lightPosArray[vectorLen * MAX_LIGHT_SOURCES] = { 0 };
    float viewPosArray[vectorLen * MAX_LIGHT_SOURCES] = { 0 };
    float lightColorArray[vectorLen * MAX_LIGHT_SOURCES] = { 0 };
    std::array<float, MAX_LIGHT_SOURCES> lightIntensityArray = { 0 };

    auto iter = lightSourcesAndPosVec_.begin();
    auto cnt = 0;
    bool needToneMapping = NeedToneMapping(displayHeadroom_);
    while (iter != lightSourcesAndPosVec_.end() && cnt < MAX_LIGHT_SOURCES) {
        auto lightPos = iter->second;
        auto lightIntensity = iter->first->GetLightIntensity();
        auto lightColor = iter->first->GetLightColor();
        Vector4f lightColorVec =
            Vector4f(lightColor.GetRed(), lightColor.GetGreen(), lightColor.GetBlue(), lightColor.GetAlpha());
        for (int i = 0; i < vectorLen; i++) {
            lightPosArray[cnt * vectorLen + i] = lightPos[i];
            viewPosArray[cnt * vectorLen + i] = lightPos[i];
            float lightColorNorm = lightColorVec[i] / UINT8_MAX;
            if (enableEDREffect_ && needToneMapping) {
                lightColorNorm = GetBrightnessMapping(displayHeadroom_, lightColorNorm);
            }
            lightColorArray[cnt * vectorLen + i] = lightColorNorm;
        }
        lightIntensityArray[cnt] = std::abs(lightIntensity);
        iter++;
        cnt++;
    }
    builder->SetUniform("lightPos", lightPosArray, vectorLen * MAX_LIGHT_SOURCES);
    builder->SetUniform("viewPos", viewPosArray, vectorLen * MAX_LIGHT_SOURCES);
    builder->SetUniform("specularLightColor", lightColorArray, vectorLen * MAX_LIGHT_SOURCES);
    Drawing::Pen pen;
    Drawing::Brush brush;
    pen.SetAntiAlias(true);
    brush.SetAntiAlias(true);
    ROSEN_LOGD("RSPointLightDrawable::DrawLight illuminatedType:%{public}d displayHeadroom_:%{public}f",
        illuminatedType_, displayHeadroom_);
    if ((illuminatedType_ == IlluminatedType::BORDER_CONTENT) ||
        (illuminatedType_ == IlluminatedType::BLEND_BORDER_CONTENT) ||
        (illuminatedType_ == IlluminatedType::NORMAL_BORDER_CONTENT)) {
        DrawContentLight(*canvas, builder, brush, lightIntensityArray);
        DrawBorderLight(*canvas, builder, pen, lightIntensityArray);
    } else if ((illuminatedType_ == IlluminatedType::CONTENT) ||
        (illuminatedType_ == IlluminatedType::BLEND_CONTENT)) {
        DrawContentLight(*canvas, builder, brush, lightIntensityArray);
    } else if ((illuminatedType_ == IlluminatedType::BORDER) ||
        (illuminatedType_ == IlluminatedType::BLEND_BORDER) ||
        (illuminatedType_ == IlluminatedType::FEATHERING_BORDER)) {
        DrawBorderLight(*canvas, builder, pen, lightIntensityArray);
    }
}

const std::shared_ptr<Drawing::RuntimeShaderBuilder>& RSPointLightDrawable::GetPhongShaderBuilder()
{
    static constexpr char phongShaderString[](R"(
        uniform vec4 lightPos[12];
        uniform vec4 viewPos[12];
        uniform vec4 specularLightColor[12];
        uniform float specularStrength[12];

        mediump vec4 main(vec2 drawing_coord) {
            vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
            float ambientStrength = 0.0;
            vec4 diffuseColor = vec4(1.0, 1.0, 1.0, 1.0);
            float diffuseStrength = 0.0;
            float shininess = 8.0;
            mediump vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0);
            vec4 NormalMap = vec4(0.0, 0.0, 1.0, 0.0);
            // ambient
            vec4 ambient = lightColor * ambientStrength;
            vec3 norm = normalize(NormalMap.rgb);

            for (int i = 0; i < 12; i++) {
                if (abs(specularStrength[i]) > 0.01) {
                    vec3 lightDir = normalize(vec3(lightPos[i].xy - drawing_coord, lightPos[i].z));
                    float diff = max(dot(norm, lightDir), 0.0);
                    vec4 diffuse = diff * lightColor;
                    vec3 viewDir = normalize(vec3(viewPos[i].xy - drawing_coord, viewPos[i].z)); // view vector
                    vec3 halfwayDir = normalize(lightDir + viewDir); // half vector
                    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess); // exponential relationship of angle
                    vec4 specular = lightColor * spec; // multiply color of incident light
                    vec4 o = ambient + diffuse * diffuseStrength * diffuseColor; // diffuse reflection
                    vec4 specularColor = specularLightColor[i];
                    fragColor = fragColor + o + specular * specularStrength[i] * specularColor;
                }
            }
            return fragColor;
        }
    )");
    return GetLightShaderBuilder<phongShaderString>();
}

const std::shared_ptr<Drawing::RuntimeShaderBuilder>& RSPointLightDrawable::GetFeatheringBoardLightShaderBuilder()
{
    static constexpr char featheringBoardLightShaderString[](R"(
        uniform vec2 iResolution;
        uniform float contentBorderRadius;
        uniform vec4 lightPos[12];
        uniform vec4 viewPos[12];
        uniform vec4 specularLightColor[12];
        uniform float specularStrength[12];
        uniform float borderWidth;

        float sdRoundedBox(vec2 p, vec2 b, float r)
        {
            vec2 q = abs(p) - b + r;
            return (min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r);
        }

        vec2 sdRoundedBoxGradient(vec2 p, vec2 b, float r)
        {
            vec2 signs = vec2(p.x >= 0.0 ? 1.0 : -1.0, p.y >= 0.0 ? 1.0 : -1.0);
            vec2 q = abs(p) - b + r;
            vec2 nor = (q.y > q.x) ? vec2(0.0, 1.0) : vec2(1.0, 0.0);
            nor = (q.x > 0.0 && q.y > 0.0) ? normalize(q) : nor;
            return signs * nor;
        }

        mediump vec4 main(vec2 drawing_coord)
        {
            float shininess = 8.0;
            mediump vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0);
            vec2 halfResolution = iResolution.xy * 0.5;

            float gradRadius = min(max(contentBorderRadius, abs(borderWidth) * 3.0), iResolution.y * 0.5);
            float drawRadius = min(max(contentBorderRadius, abs(borderWidth) * 1.1), iResolution.y * 0.5);
            float realRoundedBoxSDF =
                sdRoundedBox(drawing_coord.xy - halfResolution, halfResolution, contentBorderRadius);
            float virtualRoundedBoxSDF = sdRoundedBox(drawing_coord.xy - halfResolution, halfResolution, drawRadius);
            vec2 grad = sdRoundedBoxGradient(drawing_coord.xy - halfResolution, halfResolution, gradRadius);
            for (int i = 0; i < 12; i++) {
                if (abs(specularStrength[i]) > 0.01) {
                    vec2 lightGrad = sdRoundedBoxGradient(lightPos[i].xy - halfResolution,
                        halfResolution,
                        contentBorderRadius); // lightGrad could be pre-computed
                    float angleEfficient = dot(grad, lightGrad);
                    if (angleEfficient > 0.0) {
                        vec3 lightDir = normalize(vec3(lightPos[i].xy - drawing_coord, lightPos[i].z));
                        vec3 viewDir = normalize(vec3(viewPos[i].xy - drawing_coord, viewPos[i].z)); // view vector
                        vec3 halfwayDir = normalize(lightDir + viewDir);                             // half vector
                        // exponential relationship of angle
                        float spec = pow(max(halfwayDir.z, 0.0), shininess); // norm is (0.0, 0.0, 1.0)
                        spec *= specularStrength[i];
                        spec *= smoothstep(-borderWidth, 0.0, virtualRoundedBoxSDF);
                        spec *= (smoothstep(1.0, 0.0, spec) * 0.2 + 0.8);
                        vec4 specularColor = specularLightColor[i];
                        fragColor += specularColor * (spec * angleEfficient);
                    }
                }
            }
            return vec4(fragColor.rgb, clamp(fragColor.a, 0.0, 1.0));
        }
    )");
    return GetLightShaderBuilder<featheringBoardLightShaderString>();
}

const std::shared_ptr<Drawing::RuntimeShaderBuilder>& RSPointLightDrawable::GetNormalLightShaderBuilder()
{
    return GetLightShaderBuilder<NORMAL_LIGHT_SHADER_STRING>();
}

void RSPointLightDrawable::DrawContentLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush,
    const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const
{
    float contentIntensityCoefficient = illuminatedType_ == IlluminatedType::NORMAL_BORDER_CONTENT ? 0.5f : 0.3f;
    float specularStrengthArr[MAX_LIGHT_SOURCES] = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        specularStrengthArr[i] = lightIntensityArray[i] * contentIntensityCoefficient;
    }
    lightBuilder->SetUniform("specularStrength", specularStrengthArr, MAX_LIGHT_SOURCES);
    std::shared_ptr<Drawing::ShaderEffect> shader = lightBuilder->MakeShader(nullptr, false);
    brush.SetShaderEffect(shader);
    if ((illuminatedType_ == IlluminatedType::BLEND_CONTENT) ||
        (illuminatedType_ == IlluminatedType::BLEND_BORDER_CONTENT)) {
        brush.SetAntiAlias(true);
        brush.SetBlendMode(Drawing::BlendMode::OVERLAY);
        Drawing::SaveLayerOps slo(&contentRRect_.GetRect(), &brush);
        canvas.SaveLayer(slo);
        canvas.AttachBrush(brush);
        canvas.DrawRoundRect(contentRRect_);
        canvas.DetachBrush();
        canvas.Restore();
    } else {
        canvas.AttachBrush(brush);
        canvas.DrawRoundRect(contentRRect_);
        canvas.DetachBrush();
    }
}

void RSPointLightDrawable::DrawBorderLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Pen& pen,
    const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const
{
    float specularStrengthArr[MAX_LIGHT_SOURCES] = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        specularStrengthArr[i] = lightIntensityArray[i];
    }
    lightBuilder->SetUniform("specularStrength", specularStrengthArr, MAX_LIGHT_SOURCES);
    std::shared_ptr<Drawing::ShaderEffect> shader = lightBuilder->MakeShader(nullptr, false);
    pen.SetShaderEffect(shader);
    float borderWidth = std::ceil(borderWidth_);
    pen.SetWidth(borderWidth);
    if ((illuminatedType_ == IlluminatedType::BLEND_BORDER) ||
        (illuminatedType_ == IlluminatedType::BLEND_BORDER_CONTENT)) {
        Drawing::Brush maskPaint;
        pen.SetBlendMode(Drawing::BlendMode::OVERLAY);
        Drawing::SaveLayerOps slo(&borderRRect_.GetRect(), &maskPaint);
        canvas.SaveLayer(slo);
        canvas.AttachPen(pen);
        canvas.DrawRoundRect(borderRRect_);
        canvas.DetachPen();
        canvas.Restore();
    } else {
        canvas.AttachPen(pen);
        canvas.DrawRoundRect(borderRRect_);
        canvas.DetachPen();
    }
}

RSDrawable::Ptr RSParticleDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSParticleDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSParticleDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    const auto& particleVector = properties.GetParticles();
    if (particleVector.GetParticleSize() == 0) {
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    const auto& particles = particleVector.GetParticleVector();
    auto bounds = properties.GetDrawRegion();
    auto imageCount = particleVector.GetParticleImageCount();
    auto imageVector = particleVector.GetParticleImageVector();
    auto particleDrawable = std::make_shared<RSParticlesDrawable>(particles, imageVector, imageCount);
    if (particleDrawable != nullptr) {
        particleDrawable->Draw(canvas, bounds);
    }
    return true;
}
} // namespace DrawableV2
} // namespace OHOS::Rosen
