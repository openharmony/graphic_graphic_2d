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
#include "feature/power_off_render_skip/rs_power_off_render_controller.h"
#include "pipeline/rs_screen_render_node.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

void RSPowerOffRenderController::CheckScreenPowerRenderControlStatus(const RSRenderNodeMap& nodeMap)
{
    // Initialization for current frame.
    bool disableRenderControl = !RSSystemProperties::GetSkipDisplayIfScreenOffEnabled();
    std::unordered_map<ScreenId, RenderSkipStatus>& prevScreenStatus = screenRenderSkipStatus_;
    std::unordered_map<ScreenId, RenderSkipStatus> curScreenStatus;
    nodeMap.TraverseScreenNodes(
        [&](const std::shared_ptr<RSScreenRenderNode>& node) {
            if (node == nullptr) {
                return;
            }
            ScreenPowerStatus status = node->GetScreenProperty().GetPowerStatus();
            disableRenderControl = disableRenderControl || node->GetScreenProperty().GetDisablePowerOffRenderControl();
            ScreenId screenId = node->GetScreenId();
            RS_TRACE_NAME_FMT("CheckScreenPowerRenderControlStatus screenId:%d, powerStatus: %d, forceDisable: %d",
                screenId, static_cast<int>(status), static_cast<int>(disableRenderControl));
            if (disableRenderControl) {
                curScreenStatus[screenId] = RenderSkipStatus::DISABLE_CONTROL;
                return;
            }
            bool isPowerOff =
                (status == ScreenPowerStatus::POWER_STATUS_OFF || status == ScreenPowerStatus::POWER_STATUS_SUSPEND);
            if (!isPowerOff) {
                curScreenStatus[screenId] = RenderSkipStatus::SCREEN_RENDER;
            } else {
                auto iter = prevScreenStatus.find(screenId);
                curScreenStatus[screenId] =
                    iter != prevScreenStatus.end() && iter->second > RenderSkipStatus::POWER_OFF_SKIP ?
                        static_cast<RenderSkipStatus>(iter->second - 1) : RenderSkipStatus::POWER_OFF_SKIP;
            }
        }
    );
    // If any screen disable render control, all screens disable render control.
    if (disableRenderControl) {
        std::for_each(curScreenStatus.begin(), curScreenStatus.end(),
            [](auto& pair) {
                pair.second = RenderSkipStatus::DISABLE_CONTROL;
            }
        );
    }
    std::swap(screenRenderSkipStatus_, curScreenStatus);
}

bool RSPowerOffRenderController::GetScreenRenderSkipped(ScreenId screenId) const
{
    auto iter = screenRenderSkipStatus_.find(screenId);
    bool skipped = iter != screenRenderSkipStatus_.end() && iter->second == RenderSkipStatus::POWER_OFF_SKIP;
    RS_TRACE_NAME_FMT("GetScreenRenderSkipStatus screenId:%d, status:%d", screenId, skipped);
    return skipped;
}

bool RSPowerOffRenderController::GetAllScreenRenderSkipped() const
{
    // if any screen is not skipping, return false
    bool allScreenSkipStatus =
        !std::any_of(
            screenRenderSkipStatus_.cbegin(), screenRenderSkipStatus_.cend(),
            [](const auto& pair) {
                return pair.second != RenderSkipStatus::POWER_OFF_SKIP;
        });
    RS_TRACE_NAME_FMT("GetAllScreenRenderSkipped allScreenSkipStatus:%d", allScreenSkipStatus);
    return allScreenSkipStatus;
}
} // namespace Rosen
} // namespace OHOS