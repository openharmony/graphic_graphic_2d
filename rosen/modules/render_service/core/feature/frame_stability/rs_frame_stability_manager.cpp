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

#include "frame_stability/rs_frame_stability_manager.h"
#include "frame_stability/rs_frame_stability_state.h"
#include "feature/dirty/rs_uni_dirty_compute_util.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "screen_manager/rs_screen_manager.h"

#undef LOG_TAG
#define LOG_TAG "RSFrameStabilityManager"

namespace OHOS {
namespace Rosen {
RSFrameStabilityManager& RSFrameStabilityManager::GetInstance()
{
    static RSFrameStabilityManager instance;
    return instance;
}

void RSFrameStabilityManager::CleanResourcesByPid(pid_t pid)
{
}

void RSFrameStabilityManager::CleanResourcesByNodeId(NodeId id)
{
}

int32_t RSFrameStabilityManager::RegisterFrameStabilityDetection(
    pid_t pid,
    const FrameStabilityTarget& target,
    const FrameStabilityConfig& config,
    sptr<RSIFrameStabilityCallback> callback)
{
    return static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS);
}

int32_t RSFrameStabilityManager::UnregisterFrameStabilityDetection(pid_t pid, const FrameStabilityTarget& target)
{
    return static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS);
}

int32_t RSFrameStabilityManager::StartFrameStabilityCollection(
    pid_t pid,
    const FrameStabilityTarget& target,
    const FrameStabilityConfig& config)
{
    return static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS);
}

int32_t RSFrameStabilityManager::GetFrameStabilityResult(pid_t pid, const FrameStabilityTarget& target, bool& result)
{
    return static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS);
}

int32_t RSFrameStabilityManager::UpdateFrameStabilityDetection(
    pid_t pid,
    const FrameStabilityTarget& oldTarget,
    const FrameStabilityTarget& newTarget)
{
    return static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS);
}

float RSFrameStabilityManager::CalculateRegionPercentage(const Occlusion::Region& region, float screenArea)
{
    return 0.0f;
}

void RSFrameStabilityManager::TriggerCallback(uint64_t targetId, bool isStable,
    sptr<RSIFrameStabilityCallback> callback)
{
}

void RSFrameStabilityManager::HandleStabilityTimeout(uint64_t targetId)
{
    // trigger callback only when there is no vsync and state is init or notstable
}

void RSFrameStabilityManager::RecordCurrentFrameDirty(
    uint64_t id, const std::vector<RectI>& damageRegionRects, float screenArea)
{
}

void RSFrameStabilityManager::RecordDirtyToDetector(
    uint64_t id, const std::vector<RectI>& damageRegionRects, float screenArea)
{
}

void RSFrameStabilityManager::RecordDirtyToCollector(
    uint64_t id, const std::vector<RectI>& damageRegionRects, float screenArea)
{
}
} // namespace Rosen
} // namespace OHOS