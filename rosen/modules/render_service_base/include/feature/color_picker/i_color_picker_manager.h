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

#ifndef RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_I_COLOR_PICKER_MANAGER_H
#define RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_I_COLOR_PICKER_MANAGER_H

#include <memory>
#include <optional>

#include "draw/color.h"
#include "utils/rect.h"

namespace OHOS::Rosen {
class RSPaintFilterCanvas;
enum class ColorPickStrategyType : int16_t;
struct ColorPickerParam;

class IColorPickerManager {
public:
    virtual ~IColorPickerManager() = default;

    // Returns the interpolated color (if available), nullopt otherwise.
    // Does NOT schedule any async color pick tasks.
    virtual std::optional<Drawing::ColorQuad> GetColorPick() = 0;

    // Schedules an async color pick task on the canvas content.
    // The result will be delivered via callback or client notification.
    virtual void ScheduleColorPick(
        RSPaintFilterCanvas& canvas, const Drawing::Rect* rect, const ColorPickerParam& params) = 0;

    // Sets the system dark color mode for color picker.
    virtual void SetSystemDarkColorMode(bool isSystemDarkColorMode) = 0;
    // Reset memory of previously picked color.
    virtual void ResetColorMemory() = 0;

    // Handles color update after GPU work completes.
    // Each implementation can have different business logic (e.g., animation, luminance zones).
    virtual void HandleColorUpdate(Drawing::ColorQuad newColor) = 0;
};

} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_I_COLOR_PICKER_MANAGER_H
