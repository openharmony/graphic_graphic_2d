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
#include "render/rs_drawing_filter.h"
#include "render/rs_effect_luminance_manager.h"
#include "render/rs_particles_drawable.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
namespace {
constexpr int PARAM_TWO = 2;
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

void RSBinarizationDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
        RSTagTracker::SOURCETYPE::SOURCE_RSBINARIZATIONDRAWABLE);
#endif
    RSPropertyDrawableUtils::DrawBinarization(canvas, aiInvert_);
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

void RSColorFilterDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
        RSTagTracker::SOURCETYPE::SOURCE_RSCOLORFILTERDRAWABLE);
#endif
    RSPropertyDrawableUtils::DrawColorFilter(canvas, filter_);
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

void RSLightUpEffectDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
        RSTagTracker::SOURCETYPE::SOURCE_RSLIGHTUPEFFECTDRAWABLE);
#endif
    RSPropertyDrawableUtils::DrawLightUpEffect(canvas, lightUpEffectDegree_);
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

void RSDynamicDimDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
        RSTagTracker::SOURCETYPE::SOURCE_RSDYNAMICDIMDRAWABLE);
#endif
    RSPropertyDrawableUtils::DrawDynamicDim(canvas, dynamicDimDegree_);
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

void RSForegroundShaderDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
    if (canvas == nullptr || visualEffectContainer_ == nullptr || rect == nullptr) {
        return;
    }
    geRender->DrawShaderEffect(*canvas, *visualEffectContainer_, *rect);
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
    if (rsFilter->IsDrawingFilter()) {
        const auto& drawingFilter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);
        RSPropertyDrawableUtils::ApplySDFShapeToFilter(node.GetRenderProperties(), drawingFilter, node.GetId());
    }
    needSync_ = true;
    stagingBoundsRect_ = node.GetRenderProperties().GetBoundsRect();
    return true;
}

void RSForegroundFilterDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(paintFilterCanvas ? paintFilterCanvas->GetGPUContext() : nullptr,
        RSTagTracker::SOURCETYPE::SOURCE_RSFOREGROUNDFILTERDRAWABLE);
#endif
    RSPropertyDrawableUtils::BeginForegroundFilter(*paintFilterCanvas, boundsRect_);
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

void RSForegroundFilterRestoreDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    if (foregroundFilter_ && foregroundFilter_->IsDrawingFilter() && rect) {
        auto drawingFilter = std::static_pointer_cast<RSDrawingFilter>(foregroundFilter_);
        drawingFilter->SetGeometry(canvas->GetTotalMatrix(), canvas->GetDeviceClipBounds(), Drawing::Rect{},
            rect->GetWidth(), rect->GetHeight());
    }

    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(paintFilterCanvas ? paintFilterCanvas->GetGPUContext() : nullptr,
        RSTagTracker::SOURCETYPE::SOURCE_RSFOREGROUNDFILTERRESTOREDRAWABLE);
#endif
    RS_TRACE_NAME_FMT("RSForegroundFilterRestoreDrawable::OnDraw node[%llu] ", renderNodeId_);
    RSPropertyDrawableUtils::DrawForegroundFilter(*paintFilterCanvas, foregroundFilter_);
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

void RSPixelStretchDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
        RSTagTracker::SOURCETYPE::SOURCE_RSPIXELSTRETCHDRAWABLE);
#endif
    RSPropertyDrawableUtils::DrawPixelStretch(canvas, pixelStretch_, boundsRect_, boundsGeoValid_,
        static_cast<Drawing::TileMode>(pixelStretchTileMode_));
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
    auto& imageVector = particleVector.GetParticleImageVector();

    if (cachedDrawable_ == nullptr) {
        cachedDrawable_ = std::make_shared<RSParticlesDrawable>(particles, imageVector, imageCount);
    } else {
        cachedDrawable_->UpdateData(particles, imageVector, imageCount);
    }

    cachedDrawable_->Draw(canvas, bounds);
    return true;
}
} // namespace DrawableV2
} // namespace OHOS::Rosen
