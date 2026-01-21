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

#ifndef RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_COLOR_PICK_ALT_MANAGER_H
#define RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_COLOR_PICK_ALT_MANAGER_H

#include <atomic>
#include <memory>

#include "i_color_picker_manager.h"

#include "draw/canvas.h"
#include "draw/color.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_color_picker_def.h"
#include "property/rs_properties_def.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Image;
}
class ColorPickAltManager : public std::enable_shared_from_this<ColorPickAltManager>, public IColorPickerManager {
public:
    ColorPickAltManager() = default;
    ~ColorPickAltManager() noexcept = default;
    std::optional<Drawing::ColorQuad> GetColorPicked(RSPaintFilterCanvas& canvas, const Drawing::Rect* rect,
        uint64_t nodeId, const ColorPickerParam& params) override;

private:
    void ScheduleColorPick(
        RSPaintFilterCanvas& canvas, const Drawing::Rect* rect, uint64_t nodeId);
    void HandleColorUpdate(Drawing::ColorQuad newColor, uint64_t nodeId);

    std::atomic<Drawing::ColorQuad> pickedColor_ = Drawing::Color::COLOR_BLACK;
    uint64_t lastUpdateTime_ = 0;
    std::atomic<uint32_t> notifyThreshold_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_COLOR_PICK_ALT_MANAGER_H
