/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "property/rs_property_drawable_utilities.h"

#include "property/rs_properties.h"
#include "property/rs_properties_painter.h"

namespace OHOS::Rosen {
// ============================================================================
// alias (reference or soft link) of another drawable
RSAliasDrawable::RSAliasDrawable(RSPropertyDrawableSlot slot) : slot_(slot) {}
void RSAliasDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    auto it = context.drawableMap_.find(slot_);
    if (it != context.drawableMap_.end() && it->second != nullptr) {
        it->second->Draw(context);
    }
}

// ============================================================================
// Save and Restore
RSSaveDrawable::RSSaveDrawable(std::shared_ptr<int> content) : content_(std::move(content)) {}
void RSSaveDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    *content_ = context.canvas_->save();
}

RSRestoreDrawable::RSRestoreDrawable(std::shared_ptr<int> content) : content_(std::move(content)) {}
void RSRestoreDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    context.canvas_->restoreToCount(*content_);
}

RSCustomSaveDrawable::RSCustomSaveDrawable(
    std::shared_ptr<RSPaintFilterCanvas::SaveStatus> content, RSPaintFilterCanvas::SaveType type)
    : content_(std::move(content)), type_(type)
{}
void RSCustomSaveDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    *content_ = context.canvas_->Save(type_);
}

RSCustomRestoreDrawable::RSCustomRestoreDrawable(std::shared_ptr<RSPaintFilterCanvas::SaveStatus> content)
    : content_(std::move(content))
{}
void RSCustomRestoreDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    context.canvas_->RestoreStatus(*content_);
}

// ============================================================================
// Adapter for RSRenderModifier
RSModifierDrawable::RSModifierDrawable(RSModifierType type) : type_(type) {}
void RSModifierDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    auto itr = context.drawCmdModifiers_.find(type_);
    if (itr == context.drawCmdModifiers_.end() || itr->second.empty()) {
        return;
    }
    for (const auto& modifier : itr->second) {
        modifier->Apply(context);
    }
}

// ============================================================================
// Alpha
RSAlphaDrawable::RSAlphaDrawable(float alpha) : RSPropertyDrawable(), alpha_(alpha) {}
void RSAlphaDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    context.canvas_->MultiplyAlpha(alpha_);
}
RSPropertyDrawable::DrawablePtr RSAlphaDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    auto alpha = context.properties_.GetAlpha();
    if (alpha == 1) {
        return nullptr;
    }
    return context.properties_.GetAlphaOffscreen() ? std::make_unique<RSAlphaOffscreenDrawable>(alpha)
                                                   : std::make_unique<RSAlphaDrawable>(alpha);
}

RSAlphaOffscreenDrawable::RSAlphaOffscreenDrawable(float alpha) : RSAlphaDrawable(alpha) {}
void RSAlphaOffscreenDrawable::OnBoundsChange(const RSProperties& properties)
{
#ifndef USE_ROSEN_DRAWING
    rect_ = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());
#else
    rect_ = RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect());
#endif
}
void RSAlphaOffscreenDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
#ifndef USE_ROSEN_DRAWING
    context.canvas_->saveLayerAlpha(&rect_, std::clamp(alpha_, 0.f, 1.f) * UINT8_MAX);
#else
    Drawing::Brush brush;
    brush.SetAlphaF(std::clamp(alpha, 0.f, 1.f) * UINT8_MAX);
    Drawing::SaveLayerOps slr(&rect, &brush);
    canvas.SaveLayer(slr);
#endif
}

} // namespace OHOS::Rosen
