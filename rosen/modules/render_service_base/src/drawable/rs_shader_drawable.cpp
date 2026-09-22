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

#include "drawable/rs_shader_drawable.h"

#include "ge_image_cache_provider.h"
#include "ge_render.h"
#include "ge_visual_effect_container.h"
#include "rs_trace.h"

#include "common/rs_optional_trace.h"
#include "drawable/rs_effect_drawable_utils.h"
#include "drawable/rs_property_drawable_utils.h"
#include "effect/rs_render_shader_base.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "property/rs_properties.h"

namespace OHOS::Rosen {
namespace DrawableV2 {

bool RSShaderDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto shader = GetShader(properties);
    if (!shader) {
        return false;
    }
    RSEffectDrawableUtils::ApplySDFShapeToEffect(properties, shader, node.GetId());
    stagingShader_ = shader;

    stagingNodeId_ = node.GetId();
    stagingDrawRect_ = RSEffectDrawableUtils::CalcShaderDrawRect(node, properties, shader);
    stagingCornerRadius_ = properties.GetCornerRadius().x_;
    stagingEnableEDREffect_ = RSNGRenderShaderHelper::CheckEnableEDR(stagingShader_);
    stagingScreenNodeId_ = node.GetScreenNodeId();

    needSync_ = true;
    return true;
}

void RSShaderDrawable::OnSync()
{
    if (!stagingShader_) {
        return;
    }
    if (!needSync_) {
        return;
    }
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    stagingShader_->AppendToGEContainer(visualEffectContainer);
    visualEffectContainer->UpdateCacheDataFrom(visualEffectContainer_);
    visualEffectContainer_ = visualEffectContainer;

    nodeId_ = stagingNodeId_;
    drawRect_ = stagingDrawRect_;
    cornerRadius_ = stagingCornerRadius_;
    enableEDREffect_ = stagingEnableEDREffect_;
    screenNodeId_ = stagingScreenNodeId_;

    needSync_ = false;
}

bool RSShaderDrawable::PrepareFrostedGlassEffect(Drawing::Canvas* canvas, const Drawing::Rect& drawRect) const
{
    auto effectData = RSNGRenderShaderHelper::GetCachedBlurImage(canvas);
    if (effectData == nullptr) {
        visualEffectContainer_->UpdateCachedBlurImage(canvas, nullptr, 0, 0);
        return true;
    }
    // Tag GPU resources generated in the frosted glass path, null GPU context is safe here.
    RSTagTracker tag(canvas->GetGPUContext(), RSTagTracker::TAGTYPE::TAG_FROSTEDGLASS_EFFECT);
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    if (paintFilterCanvas->GetEffectIntersectWithDRM()) {
        RS_TRACE_NAME("RSShaderDrawable::OnDraw DrawColorUsingSDFWithDRM");
        RSEffectDrawableUtils::DrawColorUsingSDFWithDRM(canvas, &drawRect, paintFilterCanvas->GetDarkColorMode(),
            visualEffectContainer_, Drawing::GE_SHADER_FROSTED_GLASS_EFFECT,
            Drawing::GE_SHADER_FROSTED_GLASS_EFFECT_SHAPE);
        return false;
    }
    visualEffectContainer_->UpdateCachedBlurImage(
        canvas, effectData->cachedImage_, effectData->cachedRect_.GetLeft(), effectData->cachedRect_.GetTop());
    visualEffectContainer_->UpdateTotalMatrix(effectData->cachedMatrix_);
    auto blurImageForEdge = effectData->GetProviderDataChecked();
    visualEffectContainer_->UpdateFrostedGlassEffectParams(blurImageForEdge, effectData->refractOut_);
    Drawing::RectF cacheRectF(effectData->cachedRect_);
    visualEffectContainer_->UpdateSnapshotRect(cacheRectF);
    RS_TRACE_NAME_FMT("RSShaderDrawable::OnDraw image[%d], fractOut[%f], cacheRectF[%s]",
        effectData->cachedImage_ != nullptr, effectData->refractOut_, cacheRectF.ToString().c_str());
    return true;
}

void RSShaderDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    DfxOnDraw();
    auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
    if (canvas == nullptr || visualEffectContainer_ == nullptr || rect == nullptr) {
        return;
    }

    visualEffectContainer_->SetGeometry(canvas->GetTotalMatrix(), *rect, *rect, rect->GetWidth(), rect->GetHeight());
    // Fallback to the slot rect when drawRect_ is empty.
    Drawing::Rect drawRect = drawRect_.IsEmpty() ? *rect : RSPropertyDrawableUtils::Rect2DrawingRect(drawRect_);
    if (!PrepareFrostedGlassEffect(canvas, drawRect)) {
        return;
    }
    visualEffectContainer_->UpdateCornerRadius(cornerRadius_);
    // Dark scale drives the frosted glass dark-mode parameter interpolation (no-op for other
    // filters); the R channel of the picked surface contrast color encodes the dark fraction.
    auto color = static_cast<RSPaintFilterCanvas*>(canvas)->GetColorPicked(ColorPlaceholder::SURFACE_CONTRAST);
    constexpr float COLOR_MAX = 255.0f;
    visualEffectContainer_->UpdateDarkScale(static_cast<float>(Drawing::Color::ColorQuadGetR(color)) / COLOR_MAX);
    geRender->DrawShaderEffect(*canvas, *visualEffectContainer_, drawRect);
}

// ============================================================================
// RSMaterialShaderDrawable
RSDrawable::Ptr RSMaterialShaderDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSMaterialShaderDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

std::shared_ptr<RSNGRenderShaderBase> RSMaterialShaderDrawable::GetShader(const RSProperties& properties) const
{
    return properties.GetMaterialShader();
}

void RSMaterialShaderDrawable::DfxOnDraw() const
{
    RS_OPTIONAL_TRACE_FMT("RSMaterialShaderDrawable, nodeId:%{public}" PRIu64
                          ", drawRect:%{public}s, cornerRadius:%{public}f",
        nodeId_, drawRect_.ToString().c_str(), cornerRadius_);
}

// ============================================================================
// RSBackgroundNGShaderDrawable
RSDrawable::Ptr RSBackgroundNGShaderDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBackgroundNGShaderDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

std::shared_ptr<RSNGRenderShaderBase> RSBackgroundNGShaderDrawable::GetShader(const RSProperties& properties) const
{
    return properties.GetBackgroundNGShader();
}

void RSBackgroundNGShaderDrawable::DfxOnDraw() const
{
    RS_OPTIONAL_TRACE_FMT("RSBackgroundNGShaderDrawable, nodeId:%{public}" PRIu64
                          ", drawRect:%{public}s, cornerRadius:%{public}f",
        nodeId_, drawRect_.ToString().c_str(), cornerRadius_);
}

// ============================================================================
// RSForegroundShaderDrawable
RSDrawable::Ptr RSForegroundShaderDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSForegroundShaderDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

std::shared_ptr<RSNGRenderShaderBase> RSForegroundShaderDrawable::GetShader(const RSProperties& properties) const
{
    return properties.GetForegroundShader();
}

void RSForegroundShaderDrawable::DfxOnDraw() const
{
    RS_OPTIONAL_TRACE_FMT("RSForegroundShaderDrawable, nodeId:%{public}" PRIu64
                          ", drawRect:%{public}s, cornerRadius:%{public}f",
        nodeId_, drawRect_.ToString().c_str(), cornerRadius_);
}

// ============================================================================
// RSOverlayNGShaderDrawable
RSDrawable::Ptr RSOverlayNGShaderDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSOverlayNGShaderDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

std::shared_ptr<RSNGRenderShaderBase> RSOverlayNGShaderDrawable::GetShader(const RSProperties& properties) const
{
    return properties.GetOverlayNGShader();
}

void RSOverlayNGShaderDrawable::DfxOnDraw() const
{
    RS_OPTIONAL_TRACE_FMT("RSOverlayNGShaderDrawable, nodeId:%{public}" PRIu64
                          ", drawRect:%{public}s, cornerRadius:%{public}f",
        nodeId_, drawRect_.ToString().c_str(), cornerRadius_);
}
} // namespace DrawableV2
} // namespace OHOS::Rosen
