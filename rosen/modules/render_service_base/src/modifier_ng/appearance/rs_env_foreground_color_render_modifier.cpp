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

#include "modifier_ng/appearance/rs_env_foreground_color_render_modifier.h"

#include "drawable/rs_property_drawable_utils.h"
#include "property/rs_properties.h"

namespace OHOS::Rosen::ModifierNG {
const RSEnvForegroundColorRenderModifier::LegacyPropertyApplierMap
    RSEnvForegroundColorRenderModifier::LegacyPropertyApplierMap_ = {
        // { RSPropertyType::ENV_FOREGROUND_COLOR,
        //     RSRenderModifier::PropertyApplyHelper<Color, &RSProperties::SetEnvForegroundColor> },
        // { RSPropertyType::ENV_FOREGROUND_COLOR_STRATEGY,
        //     RSRenderModifier::PropertyApplyHelper<ForegroundColorStrategyType,
        //         &RSProperties::SetEnvForegroundColorStrategy> },
    };

void RSEnvForegroundColorRenderModifier::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect)
{
    switch (static_cast<ForegroundColorStrategyType>(renderEnvForegroundColorStrategy_)) {
        case ForegroundColorStrategyType::INVERT_BACKGROUNDCOLOR: {
            // TODO: we need to check if RSPropertyDrawableUtils::GetInvertBackgroundColor depends on RSProperties
            // Color color = RSPropertyDrawableUtils::GetInvertBackgroundColor(
            //     canvas, renderNeedClipToBounds_, renderBoundsRect_, renderBackgroundColor_);
            // canvas.SetEnvForegroundColor(color);
        }
        default: {
            // Use the renderEnvForegroundColor if no strategy is provided
            canvas.SetEnvForegroundColor(renderEnvForegroundColor_);
        }
    }
}

bool RSEnvForegroundColorRenderModifier::OnApply(RSModifierContext& context)
{
    if (HasProperty(RSPropertyType::ENV_FOREGROUND_COLOR)) {
        stagingEnvForegroundColor_ = Getter(RSPropertyType::ENV_FOREGROUND_COLOR, Color(0.0f, 0.0f, 0.0f, 1.0f));
        stagingEnvForegroundColorStrategy_ = 0;
    } else if (HasProperty(RSPropertyType::ENV_FOREGROUND_COLOR_STRATEGY)) {
        stagingEnvForegroundColorStrategy_ = Getter<int>(RSPropertyType::ENV_FOREGROUND_COLOR_STRATEGY);
        return true;
    }
    return false;
}

void RSEnvForegroundColorRenderModifier::OnSync()
{
    std::swap(stagingEnvForegroundColor_, renderEnvForegroundColor_);
    std::swap(stagingEnvForegroundColorStrategy_, renderEnvForegroundColorStrategy_);
}
} // namespace OHOS::Rosen::ModifierNG
