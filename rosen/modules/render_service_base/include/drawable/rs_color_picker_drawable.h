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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_COLOR_PICKER_DRAWABLE_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_COLOR_PICKER_DRAWABLE_H

#include <cstdint>
#include <memory>

#include "feature/color_picker/i_color_picker_manager.h"

#include "common/rs_macros.h"
#include "drawable/rs_drawable.h"
#include "property/rs_properties_def.h"

namespace OHOS::Rosen {
class RSColorPickerManager;
class ColorPickAltManager;

namespace DrawableV2 {
// RSColorPickerDrawable, pick color for current content of canvas
enum class ColorPickerState : uint8_t {
    PREPARING = 0,
    SCHEDULED = 1,
    COLOR_PICK_THIS_FRAME = 2,
};

class RSB_EXPORT RSColorPickerDrawable : public RSDrawable {
public:
    RSColorPickerDrawable() = default;
    RSColorPickerDrawable(bool useAlt, NodeId nodeId);
    ~RSColorPickerDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& content) override;
    void OnSync() override;
    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override;

    /**
     * @brief Called every frame during Prepare phase for all color pickers
     * @param darkMode Whether current system is in dark color mode
     *
     * Handles COLOR_PICK_THIS_FRAME state transition and dark mode changes.
     * This is called regardless of dirty region intersection.
     * @return needSync - true if staging data changed and needs sync to render thread
     */
    bool OnPrepare(bool darkMode);

    /**
     * @brief Called only when color picker node intersects with dirty region
     * @param vsyncTime Current vsync time in nanoseconds
     * @return Delay time in milliseconds, -1 if no scheduling needed
     *
     * Schedules color pick when in PREPARING state.
     * Calculates delay based on cooldown interval:
     * - If cooldown passed: execute immediately (delay = 0)
     * - If cooldown not passed: delay = lastUpdateTime + interval - currentTime
     * Transitions from PREPARING to SCHEDULED state.
     */
    int64_t ScheduleColorPickIfReady(uint64_t vsyncTime);

    void ResetColorMemory();
    EquivalentDarkMode GetLastEquivalentDarkMode();

    // main thread only
    void SetState(ColorPickerState state);
    ColorPickerState GetState() const;

private:
    NodeId stagingNodeId_ = INVALID_NODEID;
    NodeId nodeId_ = INVALID_NODEID;
    std::shared_ptr<ColorPickerParam> stagingColorPicker_;
    ColorPickerParam params_;
    ColorPickerState stagingState_ = ColorPickerState::PREPARING;

    std::shared_ptr<IColorPickerManager> colorPickerManager_;

    bool stagingNeedColorPick_ = false;
    bool needColorPick_ = false;
    bool stagingIsSystemDarkColorMode_ = false;
    bool needSync_ = false;
    uint64_t lastUpdateTime_ = 0;  // Only used in Prepare
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_COLOR_PICKER_DRAWABLE_H
