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

#ifndef RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_MANAGER_H
#define RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_MANAGER_H

#include <atomic>
#include <memory>
#include <mutex>

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
class RSColorPickerManager : public std::enable_shared_from_this<RSColorPickerManager> {
public:
    RSColorPickerManager() = default;
    ~RSColorPickerManager() noexcept = default;
    /**
     * @brief Return previously picked color and conditionally schedule a new color pick task on the current canvas.
     * @param strategy Only CONTRAST is currently supported.
     */
    Drawing::ColorQuad GetColorPicked(RSPaintFilterCanvas& canvas, const Drawing::Rect* rect, uint64_t nodeId,
        ColorPickStrategyType strategy, uint64_t interval);

private:
    void ScheduleColorPick(
        RSPaintFilterCanvas& canvas, const Drawing::Rect* rect, uint64_t nodeId, ColorPickStrategyType strategy);
    void PickColor(const std::shared_ptr<Drawing::Image>& snapshot, uint64_t nodeId, ColorPickStrategyType strategy);
    void HandleColorUpdate(Drawing::ColorQuad newColor, uint64_t nodeId, ColorPickStrategyType strategy);

    inline std::pair<Drawing::ColorQuad, Drawing::ColorQuad> GetColor();

    static Drawing::ColorQuad InterpolateColor(
        Drawing::ColorQuad startColor, Drawing::ColorQuad endColor, float fraction);

    /**
     * @brief Get a contrasting black or white color with hysteresis.
     *
     * @param color input to contrast with, usually the average color of the background.
     * @param prevDark whether previously picked color was dark.
     */
    static Drawing::ColorQuad GetContrastColor(Drawing::ColorQuad color, bool prevDark);

    std::mutex colorMtx_;
    Drawing::ColorQuad colorPicked_ = Drawing::Color::COLOR_BLACK;
    Drawing::ColorQuad prevColor_ = Drawing::Color::COLOR_BLACK;

    std::atomic<uint64_t> animStartTime_ = 0;
    uint64_t lastUpdateTime_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_MANAGER_H
