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
#include "rs_power_off_render_skip_manager.h"
#include "pipeline/rs_screen_render_node.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

RSPowerOffRenderSkipManager& RSPowerOffRenderSkipManager::Instance()
{
    static RSPowerOffRenderSkipManager instance;
    return instance;
}

void RSPowerOffRenderSkipManager::CheckRenderSkipStatus(const RSRenderNodeMap& nodeMap)
{
    std::lock_guard<std::mutex> lock(mutex_);
    screenRenderSkipStatus_.clear();
    allScreenSkipStatus_ = true;
    disableRenderControl_ = false;
    nodeMap.TraverseScreenNodes(
        [&](const std::shared_ptr<RSScreenRenderNode>& node) {
            if (node == nullptr) {
                return;
            }
            disableRenderControl_ |= node->GetScreenProperty().GetDisablePowerOffRenderControl();
            if (screenRenderSkipStatus_.find(node->GetScreenId()) != screenRenderSkipStatus_.end()) {
                return;
            }
            auto status = node->GetScreenProperty().GetPowerStatus();
            RS_TRACE_NAME_FMT("CheckRenderSkipStatus screenId:%d, powerStatus: %d, forceDisable: %d",
                node->GetScreenId(), status, node->GetScreenProperty().GetDisablePowerOffRenderControl());
            screenRenderSkipStatus_[node->GetScreenId()] =
                (status == ScreenPowerStatus::POWER_STATUS_OFF || status == ScreenPowerStatus::POWER_STATUS_SUSPEND);
            allScreenSkipStatus_ &= screenRenderSkipStatus_[node->GetScreenId()];
        }
    );
}

bool RSPowerOffRenderSkipManager::GetScreenRenderSkipStatus(ScreenId screenId) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (disableRenderControl_ || !RSSystemProperties::GetSkipDisplayIfScreenOffEnabled()) {
        return false;
    }
    auto iter = screenRenderSkipStatus_.find(screenId);
    if (iter != screenRenderSkipStatus_.end()) {
        RS_TRACE_NAME_FMT("GetScreenRenderSkipStatus screenId:%d, status:%d", screenId, iter->second);
        return iter->second;
    }
    return false;
}

bool RSPowerOffRenderSkipManager::GetAllScreenRenderSkipStatus() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (disableRenderControl_ || !RSSystemProperties::GetSkipDisplayIfScreenOffEnabled()) {
        return false;
    }
    RS_TRACE_NAME_FMT("GetAllScreenRenderSkipStatus status:%d", allScreenSkipStatus_);
    return allScreenSkipStatus_;
}
} // namespace Rosen
} // namespace OHOS