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

#include "feature/color_picker/rs_color_picker_thread.h"
#include "feature/color_picker/rs_color_picker_utils.h"
#include "i_color_picker_manager.h"

#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS {
namespace Rosen {

class RSColorPickerManager : public std::enable_shared_from_this<RSColorPickerManager>, public IColorPickerManager {
public:
    explicit RSColorPickerManager(NodeId nodeId) : nodeId_(nodeId) {}
    ~RSColorPickerManager() noexcept = default;

    std::optional<Drawing::ColorQuad> GetColorPick() override;
    void ScheduleColorPick(
        RSPaintFilterCanvas& canvas, const Drawing::Rect* rect, const ColorPickerParam& params) override;
    void SetSystemDarkColorMode(bool isSystemDarkColorMode) override;
    void ResetColorMemory() override {}

    void HandleColorUpdate(Drawing::ColorQuad newColor) override;

private:
    std::pair<Drawing::ColorQuad, Drawing::ColorQuad> GetColor() const;

    static Drawing::ColorQuad InterpolateColor(
        Drawing::ColorQuad startColor, Drawing::ColorQuad endColor, float fraction);

    /**
     * @brief Get a contrasting black or white color with hysteresis.
     *
     * @param color input to contrast with, usually the average color of the background.
     * @param prevDark whether previously picked color was dark.
     */
    static Drawing::ColorQuad GetContrastColor(Drawing::ColorQuad color, bool prevDark);

    // Current contrast color (black/white) that contrasts with extracted background color.
    // Uses COLOR_TRANSPARENT as sentinel for "not set yet" - defaults to white in dark mode,
    // black in light mode.
    std::atomic<Drawing::ColorQuad> colorPicked_ = Drawing::Color::COLOR_TRANSPARENT;

    // Previous color used for smooth animation transitions between contrast colors.
    // Uses COLOR_TRANSPARENT as sentinel for "not set yet".
    std::atomic<Drawing::ColorQuad> prevColor_ = Drawing::Color::COLOR_TRANSPARENT;

    // Animation start timestamp (ms since epoch) for interpolating prevColor_ to colorPicked_.
    std::atomic<uint64_t> animStartTime_ = 0;

    const NodeId nodeId_;

    // System dark mode flag - affects default color when colorPicked_/prevColor_ are not set.
    std::atomic<bool> isSystemDarkColorMode_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_MANAGER_H
