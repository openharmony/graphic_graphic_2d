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
#include "pipeline/rs_render_node.h"

namespace OHOS::Rosen {
// ============================================================================
// alias (reference or soft link) of another drawable
RSAliasDrawable::RSAliasDrawable(Slot::RSPropertyDrawableSlot slot) : slot_(slot) {}
void RSAliasDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    auto& it = node.propertyDrawablesVec_[slot_];
    if (it) {
        it->Draw(node, canvas);
    }
}

// ============================================================================
// Save and Restore
RSSaveDrawable::RSSaveDrawable(std::shared_ptr<int> content) : content_(std::move(content)) {}
void RSSaveDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    *content_ = canvas.save();
}

RSRestoreDrawable::RSRestoreDrawable(std::shared_ptr<int> content) : content_(std::move(content)) {}
void RSRestoreDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    canvas.restoreToCount(*content_);
}

RSCustomSaveDrawable::RSCustomSaveDrawable(
    std::shared_ptr<RSPaintFilterCanvas::SaveStatus> content, RSPaintFilterCanvas::SaveType type)
    : content_(std::move(content)), type_(type)
{}
void RSCustomSaveDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    *content_ = canvas.Save(type_);
}

RSCustomRestoreDrawable::RSCustomRestoreDrawable(std::shared_ptr<RSPaintFilterCanvas::SaveStatus> content)
    : content_(std::move(content))
{}
void RSCustomRestoreDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    canvas.RestoreStatus(*content_);
}

// ============================================================================
// Adapter for RSRenderModifier
RSModifierDrawable::RSModifierDrawable(RSModifierType type) : type_(type) {}
void RSModifierDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    auto itr = node.drawCmdModifiers_.find(type_);
    if (itr == node.drawCmdModifiers_.end() || itr->second.empty()) {
        return;
    }
    RSModifierContext context = { node.GetMutableRenderProperties(), &canvas };
    for (const auto& modifier : itr->second) {
        modifier->Apply(context);
    }
}

// ============================================================================
// Alpha
RSAlphaDrawable::RSAlphaDrawable(float alpha) : RSPropertyDrawable(), alpha_(alpha) {}
void RSAlphaDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    canvas.MultiplyAlpha(alpha_);
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
void RSAlphaOffscreenDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    auto rect = RSPropertiesPainter::Rect2SkRect(node.GetRenderProperties().GetBoundsRect());
    canvas.saveLayerAlpha(&rect, std::clamp(alpha_, 0.f, 1.f) * UINT8_MAX);
#else
    auto rect = RSPropertiesPainter::Rect2DrawingRect(node.GetRenderProperties().GetBoundsRect());
    Drawing::Brush brush;
    brush.SetAlphaF(std::clamp(alpha_, 0.f, 1.f) * UINT8_MAX);
    Drawing::SaveLayerOps slr(&rect, &brush);
    canvas.SaveLayer(slr);
#endif
}

} // namespace OHOS::Rosen
