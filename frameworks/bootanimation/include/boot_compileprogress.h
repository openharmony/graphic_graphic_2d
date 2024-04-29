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

#ifndef FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_COMPILEPROGRESS_H
#define FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_COMPILEPROGRESS_H

#ifdef NEW_RENDER_CONTEXT
#include "render_context_bash.h"
#include "rs_render_surface.h"
#else
#include <render_context/render_context.h>
#endif
#include <ui/rs_canvas_node.h>
#include <ui/rs_display_node.h>
#include <ui/rs_surface_extractor.h>
#include "event_handler.h"
#include "log.h"
#include "util.h"
#include "vsync_receiver.h"
#include "draw/canvas.h"


namespace OHOS {
class BootCompileProgress {
public:
    bool Init(Rosen::ScreenId screenId, int32_t height, int32_t width);
    void CheckNeedOatCompile();
    bool CreateCanvasNode(int32_t rotateDegree, float positionZ);
    bool RegisterVsyncAndStart();
    bool NeedUpdate();
    bool IsUpdateDone();
    bool IsStarted();
    void SetStart(bool start);

private:
    std::shared_ptr<Rosen::RSSurfaceNode> rsSurfaceNode_;
    std::shared_ptr<Rosen::RSCanvasNode> rsCanvasNode_;
    std::shared_ptr<Rosen::Drawing::Typeface> tf_;
    std::shared_ptr<OHOS::Rosen::VSyncReceiver> receiver_;
    Rosen::ScreenId screenId_;
    int windowWidth_ = 0;
    int windowHeight_ = 0;
    int32_t times = 0;
    int32_t freq_ = 4;
    
    int64_t startTimeMs_ = -1;
    int64_t endTimePredictMs_ = -1;
    int32_t progress_ = 0;
    int32_t timeLimitSec_ = -1;
    bool isBmsCompileDone_ = false;
    std::string displayInfo_ = "";

    volatile bool needUpdateOpt_ = false;
    volatile bool isUpdateOptEnd_ = false;
    volatile bool started_ = false;

    void OnVsync();
    void DrawCompileProgress();
    void UpdateCompileProgress();
    bool WaitBmsStartIfNeeded();
};
} // namespace OHOS


#endif //FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_COMPILEPROGRESS_H