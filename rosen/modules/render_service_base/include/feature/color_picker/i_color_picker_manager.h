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
    // Returns an optional color: if value present a color was immediately available,
    // otherwise (std::nullopt) the manager has scheduled an async pick/callback.
    virtual std::optional<Drawing::ColorQuad> GetColorPicked(RSPaintFilterCanvas& canvas,
        const Drawing::Rect* rect, uint64_t nodeId, const ColorPickerParam& params) = 0;
};

} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_I_COLOR_PICKER_MANAGER_H
