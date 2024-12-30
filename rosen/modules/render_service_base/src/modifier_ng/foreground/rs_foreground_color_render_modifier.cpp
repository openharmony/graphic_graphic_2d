/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "modifier_ng/foreground/rs_foreground_color_render_modifier.h"

#include "drawable/rs_property_drawable_utils.h"
#include "modifier/rs_render_property.h"
#include "modifier_ng/rs_render_modifier_utils.h"
#include "pipeline/rs_recording_canvas.h"
#include "property/rs_properties.h"

namespace OHOS::Rosen::ModifierNG {
const RSForegroundColorRenderModifier::LegacyPropertyApplierMap
    RSForegroundColorRenderModifier::LegacyPropertyApplierMap_ = {
        { RSPropertyType::FOREGROUND_COLOR, RSRenderModifier::PropertyApplyHelperAdd<Color,
                                                &RSProperties::SetForegroundColor, &RSProperties::GetForegroundColor> },
    };

void RSForegroundColorRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetForegroundColor({});
}

void RSForegroundColorRenderModifier::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect) {}

/*
 * Protected Methods
 */
bool RSForegroundColorRenderModifier::OnApply(RSModifierContext& context)
{
    if (!HasProperty(RSPropertyType::FOREGROUND_COLOR)) {
        return false;
    }
    stagingFgColor_ = Getter<Color>(RSPropertyType::FOREGROUND_COLOR);

    if (stagingFgColor_ == RgbPalette::Transparent()) {
        return false;
    }

    RSDisplayListModifierUpdater updater(this);
    auto& canvas = updater.GetRecordingCanvas();
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color(stagingFgColor_.AsArgbInt()));
    brush.SetAntiAlias(true);
    canvas->AttachBrush(brush);
    canvas->DrawRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(context.GetRRect()));
    canvas->DetachBrush();
    return true;
}

void RSForegroundColorRenderModifier::OnSync()
{
    std::swap(stagingFgColor_, renderFgColor_);
}
} // namespace OHOS::Rosen::ModifierNG
