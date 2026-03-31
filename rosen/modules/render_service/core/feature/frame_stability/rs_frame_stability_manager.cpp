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

#include "rs_frame_stability_manager.h"
#include "feature/dirty/rs_uni_dirty_compute_util.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

#undef LOG_TAG
#define LOG_TAG "RSFrameStabilityManager"

namespace OHOS {
namespace Rosen {
RSFrameStabilityManager& RSFrameStabilityManager::GetInstance()
{
    static RSFrameStabilityManager instance;
    return instance;
}

int32_t RSFrameStabilityManager::RegisterFrameStabilityDetection(
    pid_t pid,
    const FrameStabilityTarget& target,
    const FrameStabilityConfig& config,
    sptr<RSIFrameStabilityCallback> callback)
{
    RS_LOGI("RegisterFrameStabilityDetection: pid=%{public}d, type=%{public}d, id=%{public}" PRIu64,
        pid, target.type, target.id);
    return static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS);
}

int32_t RSFrameStabilityManager::UnregisterFrameStabilityDetection(pid_t pid, const FrameStabilityTarget& target)
{
    RS_LOGI("RegisterFrameStabilityDetection: pid=%{public}d, type=%{public}d, id=%{public}" PRIu64,
        pid, target.type, target.id);
    return static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS);
}

int32_t RSFrameStabilityManager::StartFrameStabilityCollection(
    pid_t pid,
    const FrameStabilityTarget& target,
    const FrameStabilityConfig& config)
{
    RS_LOGI("RegisterFrameStabilityDetection: pid=%{public}d, type=%{public}d, id=%{public}" PRIu64,
        pid, target.type, target.id);
    return static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS);
}

int32_t RSFrameStabilityManager::GetFrameStabilityResult(pid_t pid, const FrameStabilityTarget& target, bool& result)
{
    RS_LOGI("RegisterFrameStabilityDetection: pid=%{public}d, type=%{public}d, id=%{public}" PRIu64,
        pid, target.type, target.id);
    return static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS);
}
} // namespace Rosen
} // namespace OHOS