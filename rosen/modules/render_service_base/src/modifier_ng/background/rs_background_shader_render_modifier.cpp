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

#include "modifier_ng/background/rs_background_shader_render_modifier.h"

#include "drawable/rs_property_drawable_utils.h"
#include "modifier_ng/rs_render_modifier_utils.h"
#include "pipeline/rs_recording_canvas.h"
#include "property/rs_properties_painter.h"

namespace OHOS::Rosen::ModifierNG {
const RSBackgroundShaderRenderModifier::LegacyPropertyApplierMap
    RSBackgroundShaderRenderModifier::LegacyPropertyApplierMap_ = {
        { RSPropertyType::BACKGROUND_SHADER,
            RSRenderModifier::PropertyApplyHelper<std::shared_ptr<RSShader>, &RSProperties::SetBackgroundShader> },
        { RSPropertyType::BACKGROUND_SHADER_PROGRESS,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetBackgroundShaderProgress,
                &RSProperties::GetBackgroundShaderProgress> },
    };

void RSBackgroundShaderRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetBackgroundShader({});
    properties.SetBackgroundShaderProgress(0.f);
}

bool RSBackgroundShaderRenderModifier::OnApply(RSModifierContext& context)
{
    if (!HasProperty(RSPropertyType::BACKGROUND_SHADER)) {
        return false;
    }
    const auto& bgShader = Getter<std::shared_ptr<RSShader>>(RSPropertyType::BACKGROUND_SHADER, nullptr);
    if (!bgShader) {
        return false;
    }

    // regenerate stagingDrawCmdList_
    RSDisplayListModifierUpdater updater(this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    Drawing::Brush brush;
    auto shaderEffect = bgShader->GetDrawingShader();
    // do not draw if shaderEffect is nullptr and keep RSShader behavior consistent
    if (shaderEffect == nullptr && bgShader->GetShaderType() != RSShader::ShaderType::DRAWING) {
        return false;
    }
    brush.SetShaderEffect(shaderEffect);
    canvas.AttachBrush(brush);
    canvas.DrawRect(RSPropertiesPainter::Rect2DrawingRect(context.GetBoundsRect()));
    canvas.DetachBrush();
    return true;
}
} // namespace OHOS::Rosen::ModifierNG
