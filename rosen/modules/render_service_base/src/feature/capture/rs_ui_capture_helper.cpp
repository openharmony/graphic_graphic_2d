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

#include "feature/capture/rs_ui_capture_helper.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr const uint64_t INVALID_TIME_STAMP = 0;
}
void RSUiCaptureHelper::InsertUiCaptureCmdsExecutedFlag(NodeId nodeId, bool flag)
{
    std::lock_guard<std::mutex> lock(uiCaptureCmdsExecutedMutex_);
    if (!flag) {
        auto iter = uiCaptureCmdsExecutedFlag_.find(nodeId);
        if (iter == uiCaptureCmdsExecutedFlag_.end()) {
            uiCaptureCmdsExecutedFlag_[nodeId] = std::make_pair(false, GetCurrentSteadyTimeMs());
        }
        return;
    }
    uiCaptureCmdsExecutedFlag_[nodeId] = std::make_pair(flag, GetCurrentSteadyTimeMs());
}

void RSUiCaptureHelper::EraseUiCaptureCmdsExecutedFlag(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(uiCaptureCmdsExecutedMutex_);
    uiCaptureCmdsExecutedFlag_.erase(nodeId);
}

std::pair<bool, uint64_t> RSUiCaptureHelper::GetUiCaptureCmdsExecutedFlag(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(uiCaptureCmdsExecutedMutex_);
    auto iter = uiCaptureCmdsExecutedFlag_.find(nodeId);
    if (iter != uiCaptureCmdsExecutedFlag_.end()) {
        return iter->second;
    } else {
        return std::make_pair(true, INVALID_TIME_STAMP);
    }
}

uint64_t RSUiCaptureHelper::GetCurrentSteadyTimeMs() const
{
    auto curTime = std::chrono::steady_clock::now().time_since_epoch();
    uint64_t curSteadyTime = std::chrono::duration_cast<std::chrono::milliseconds>(curTime).count();
    return curSteadyTime;
}
} // namespace Rosen
} // namespace OHOS