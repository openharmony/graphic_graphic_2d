/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_COMPILE_PROGRESS_H
#define FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_COMPILE_PROGRESS_H

#ifdef NEW_RENDER_CONTEXT
#include "render_context_bash.h"
#include "rs_render_surface.h"
#else
#include <render_context/render_context.h>
#endif
#include <ui/rs_canvas_node.h>
#include <ui/rs_display_node.h>
#include <ui/rs_surface_extractor.h>

#include "draw/canvas.h"
#include "event_handler.h"
#include "log.h"
#include "util.h"
#include "vsync_receiver.h"

namespace OHOS {
class BootCompileProgress {
public:
    void Init(const BootAnimationConfig& config);

private:
    void OnVsync();
    void DrawCompileProgress();
    void UpdateCompileProgress();
    bool WaitBmsStartIfNeeded();
    bool CreateCanvasNode();
    bool RegisterVsyncCallback();
    Rosen::Drawing::Brush DrawProgressPoint(int32_t idx, int32_t frameNum);

    int32_t windowWidth_ = 0;
    int32_t windowHeight_ = 0;
    int32_t times_ = 0;
    int32_t rotateDegree_ = 0;
    int32_t progress_ = 0;
    int32_t timeLimitSec_ = -1;
    int64_t startTimeMs_ = -1;
    int64_t endTimePredictMs_ = -1;
    Rosen::ScreenId screenId_;
    std::string displayInfo_ = "";

    bool isBmsCompileDone_ = false;
    volatile bool isUpdateOptEnd_ = false;

    std::shared_ptr<Rosen::RSSurfaceNode> rsSurfaceNode_;
    std::shared_ptr<Rosen::RSCanvasNode> rsCanvasNode_;
    std::shared_ptr<Rosen::Drawing::Typeface> tf_;
    std::shared_ptr<Rosen::VSyncReceiver> receiver_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> compileHandler_;
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> compileRunner_;
    std::shared_ptr<Rosen::RSInterpolator> sharpCurve_;
};
} // namespace OHOS

#endif //FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_COMPILEPROGRESS_H
