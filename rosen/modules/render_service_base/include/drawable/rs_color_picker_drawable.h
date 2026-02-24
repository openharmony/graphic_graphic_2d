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
class RSB_EXPORT RSColorPickerDrawable : public RSDrawable {
public:
    RSColorPickerDrawable() = default;
    RSColorPickerDrawable(bool useAlt, NodeId nodeId);
    ~RSColorPickerDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& content) override;
    void OnSync() override;
    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override;
    void SetIsSystemDarkColorMode(bool isSystemDarkColorMode);

    /**
     * @brief Prepare ColorPicker for execution on main thread
     * @param vsyncTime Current vsync time in nanoseconds
     * @param darkMode Whether current system is in dark color mode
     *
     * Checks cooldown interval and determines if color pick should execute this frame.
     * Sets needColorPick flag for render thread to check. Called during Prepare phase.
     * @return pair<needColorPick, needSync> - @c needColorPick indicates if color pick should run this frame, @c
     * needSync indicates if @c needColorPick are updated and need to be synced to render thread
     */
    RSB_EXPORT std::pair<bool, bool> PrepareForExecution(uint64_t vsyncTime, bool darkMode);

    void ResetColorMemory();

private:
    NodeId stagingNodeId_ = INVALID_NODEID;
    NodeId nodeId_ = INVALID_NODEID;
    std::shared_ptr<ColorPickerParam> stagingColorPicker_;
    ColorPickerParam params_;

    std::shared_ptr<IColorPickerManager> colorPickerManager_;

    bool stagingNeedColorPick_ = false;
    bool needColorPick_ = false;
    bool stagingIsSystemDarkColorMode_ = false;
    bool needSync_ = false;
    bool isTaskScheduled_ = false; // Flag to indicate if a task is already scheduled during cooldown
    uint64_t lastUpdateTime_ = 0;  // Set in Prepare
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_COLOR_PICKER_DRAWABLE_H
