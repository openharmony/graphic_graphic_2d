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

#ifndef RS_POWER_OFF_RENDER_SKIP_CONTROLLER_H
#define RS_POWER_OFF_RENDER_SKIP_CONTROLLER_H
#include "pipeline/rs_render_node_map.h"
#include "screen_manager/screen_types.h"

#include <unordered_map>

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSPowerOffRenderController {
public:
    RSPowerOffRenderController() = default;
    ~RSPowerOffRenderController() = default;
    void SyncFrom(const RSPowerOffRenderController& sourceController)
    {
        screenRenderSkipStatus_ = sourceController.screenRenderSkipStatus_;
    }
    // This calculation function is called in the main thread.
    void CheckScreenPowerRenderControlStatus(const RSRenderNodeMap& nodeMap);
    bool GetScreenRenderSkipped(ScreenId screenId) const;
    bool GetAllScreenRenderSkipped() const;

private:
    enum RenderSkipStatus {
        DISABLE_CONTROL = 0,    // disable power off render skip control
        POWER_OFF_SKIP,         // screen is power off, screen rendering can be skipped
        FORCE_REFRESH,          // screen is power off, force refresh to render
        SCREEN_RENDER,          // screen is power on, normal rendering
    };
    std::unordered_map<ScreenId, RenderSkipStatus> screenRenderSkipStatus_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_POWER_OFF_RENDER_SKIP_CONTROLLER_H