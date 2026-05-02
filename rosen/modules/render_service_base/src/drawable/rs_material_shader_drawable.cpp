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

#include "drawable/rs_material_shader_drawable.h"

#include "ge_image_cache_provider.h"
#include "ge_render.h"
#include "ge_visual_effect_container.h"

#include "drawable/rs_property_drawable_utils.h"
#include "effect/rs_render_shader_base.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
namespace DrawableV2 {

RSDrawable::Ptr RSMaterialShaderDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSMaterialShaderDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSMaterialShaderDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    const auto& shader = properties.GetMaterialShader();
    if (!shader) {
        return false;
    }
    needSync_ = true;
    stagingShader_ = shader;
    stagingCornerRadius_ = properties.GetCornerRadius().x_;
    stagingNodeId_ = node.GetId();

    auto bounds = properties.GetBoundsRect();
    stagingDrawRect_ = RSNGRenderShaderHelper::CalcRect(shader, bounds);

    return true;
}

void RSMaterialShaderDrawable::OnSync()
{
    if (needSync_ && stagingShader_) {
        auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
        stagingShader_->AppendToGEContainer(visualEffectContainer);
        visualEffectContainer->UpdateCacheDataFrom(visualEffectContainer_);
        visualEffectContainer_ = visualEffectContainer;
        drawRect_ = stagingDrawRect_;
        needSync_ = false;
    }
    cornerRadius_ = stagingCornerRadius_;
    nodeId_ = stagingNodeId_;
}

void RSMaterialShaderDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
    if (canvas == nullptr || visualEffectContainer_ == nullptr || rect == nullptr) {
        return;
    }

    visualEffectContainer_->SetGeometry(canvas->GetTotalMatrix(), *rect, *rect, rect->GetWidth(), rect->GetHeight());
    Drawing::Rect drawRect = drawRect_.IsEmpty() ? *rect : RSPropertyDrawableUtils::Rect2DrawingRect(drawRect_);

    auto effectData = RSNGRenderShaderHelper::GetCachedBlurImage(canvas);
    if (effectData != nullptr) {
        auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
        if (paintFilterCanvas->GetEffectIntersectWithDRM()) {
            RS_TRACE_NAME("RSMaterialShaderDrawable::OnDraw DrawColorUsingSDFWithDRM");
            RSPropertyDrawableUtils::DrawColorUsingSDFWithDRM(canvas, &drawRect, paintFilterCanvas->GetDarkColorMode(),
                visualEffectContainer_, Drawing::GE_SHADER_FROSTED_GLASS_EFFECT,
                Drawing::GE_SHADER_FROSTED_GLASS_EFFECT_SHAPE);
            return;
        }
        visualEffectContainer_->UpdateCachedBlurImage(
            canvas, effectData->cachedImage_, effectData->cachedRect_.GetLeft(), effectData->cachedRect_.GetTop());
        visualEffectContainer_->UpdateTotalMatrix(effectData->cachedMatrix_);
        auto blurImageForEdge = effectData->GetProviderDataChecked();
        visualEffectContainer_->UpdateFrostedGlassEffectParams(blurImageForEdge, effectData->refractOut_);
        Drawing::RectF cacheRectF(effectData->cachedRect_);
        visualEffectContainer_->UpdateSnapshotRect(cacheRectF);
        RS_TRACE_NAME_FMT("RSBackgroundNGShaderDrawable::OnDraw image[%d], fractOut[%f], cacheRectF[%s]",
            effectData->cachedImage_ != nullptr, effectData->refractOut_, cacheRectF.ToString().c_str());
    } else {
        visualEffectContainer_->UpdateCachedBlurImage(canvas, nullptr, 0, 0);
    }
    visualEffectContainer_->UpdateCornerRadius(cornerRadius_);

    geRender->DrawShaderEffect(*canvas, *visualEffectContainer_, drawRect);
}

} // namespace DrawableV2
} // namespace OHOS::Rosen