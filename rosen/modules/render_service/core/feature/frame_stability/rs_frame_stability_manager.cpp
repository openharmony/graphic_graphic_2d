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
#include "rs_frame_stability_state.h"
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

int32_t RSFrameStabilityManager::RegisterFrameStabilityDetection(
    pid_t pid,
    const FrameStabilityTarget& target,
    const FrameStabilityConfig& config,
    sptr<RSIFrameStabilityCallback> callback)
{
    if (!callback) {
        RS_LOGE("RegisterFrameStabilityDetection id: %{public}" PRIu64 ", "
            "callback is null", target.id);
        return static_cast<int32_t>(FrameStabilityErrorCode::NULL_CALLBACK);
    }

    std::lock_guard<std::mutex> lock(detectorMutex_);
    if (screenDetectorContexts_.find(target.id) != screenDetectorContexts_.end()) {
        RS_LOGE("RegisterFrameStabilityDetection id: %{public}" PRIu64 ", "
            "target id is already registered", target.id);
        return static_cast<int32_t>(FrameStabilityErrorCode::TARGET_ID_ALREADY_REGISTERED);
    }
    if (screenDetectorContexts_.size() >= MAX_FRAME_STABILITY_CONNECTION_NUM) {
        RS_LOGE("RegisterFrameStabilityDetection id: %{public}" PRIu64 ", "
            "connections is occupied", target.id);
        return static_cast<int32_t>(FrameStabilityErrorCode::CONNECTIONS_OCCUPIED);
    }
    auto detectorContext = std::make_shared<DetectorContext>();
    detectorContext->config = config;
    detectorContext->callback = callback;
    detectorContext->state = DetectionState::INIT;
    detectorContext->accumulatedDirtyRegion = Occlusion::Region();
    detectorContext->startTime = std::chrono::steady_clock::now();
    detectorContext->pid = pid;
    detectorContext->targetType = target.type;
    screenDetectorContexts_[target.id] = detectorContext;

    auto task = [this, id = target.id]() {
        this->HandleStabilityTimeout(id);
    };
    // post delay task incase there is no vsync, callback stable
    RSUniRenderThread::Instance().PostTask(
        task, FRAME_STABILITY_TAG + std::to_string(target.id), detectorContext->config.stableDuration);
    detectorContext->hasPendingStabilityTask = true;

    RS_LOGI("RegisterFrameStabilityDetection success, pid=%{public}d, "
        "type=%{public}d, id=%{public}" PRIu64 ", stableDuration=%{public}" PRIu32 ", changePercent=%{public}f",
        pid, target.type, target.id, config.stableDuration, config.changePercent);

    return static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS);
}

int32_t RSFrameStabilityManager::UnregisterFrameStabilityDetection(pid_t pid, const FrameStabilityTarget& target)
{
    std::lock_guard<std::mutex> lock(detectorMutex_);
    auto it = screenDetectorContexts_.find(target.id);
    if (it == screenDetectorContexts_.end()) {
        RS_LOGE("UnregisterFrameStabilityDetection id: %{public}" PRIu64 ", "
            "target id is not registered", target.id);
        return static_cast<int32_t>(FrameStabilityErrorCode::TARGET_ID_NOT_REGISTERED);
    }
    auto detectorContext = it->second;
    if (detectorContext->hasPendingStabilityTask) {
        RSUniRenderThread::Instance().RemoveTask(FRAME_STABILITY_TAG + std::to_string(target.id));
    }
    screenDetectorContexts_.erase(target.id);
    RS_LOGI("UnregisterFrameStabilityDetection, pid=%{public}d, "
        "type=%{public}d, id=%{public}" PRIu64, pid, target.type, target.id);
    return static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS);
}

int32_t RSFrameStabilityManager::StartFrameStabilityCollection(
    pid_t pid,
    const FrameStabilityTarget& target,
    const FrameStabilityConfig& config)
{
    std::lock_guard<std::mutex> lock(collectorMutex_);
    if (screenCollectorContexts_.find(target.id) != screenCollectorContexts_.end()) {
        RS_LOGE("StartFrameStabilityCollection id: %{public}" PRIu64 ", "
            "target id is already start", target.id);
        return static_cast<int32_t>(FrameStabilityErrorCode::TARGET_ID_ALREADY_REGISTERED);
    }
    if (screenCollectorContexts_.size() >= MAX_FRAME_STABILITY_CONNECTION_NUM) {
        RS_LOGE("StartFrameStabilityCollection id: %{public}" PRIu64 ", "
            "connections is occupied", target.id);
        return static_cast<int32_t>(FrameStabilityErrorCode::CONNECTIONS_OCCUPIED);
    }
    auto collectorContext = std::make_shared<CollectorContext>();
    collectorContext->config = config;
    collectorContext->collectionDirtyRegion = Occlusion::Region();
    collectorContext->pid = pid;
    collectorContext->targetType = target.type;
    screenCollectorContexts_[target.id] = collectorContext;
    RS_LOGI("StartFrameStabilityCollection success, pid=%{public}d, "
        "type=%{public}d, id=%{public}" PRIu64 ", changePercent=%{public}f",
        pid, target.type, target.id, config.changePercent);

    return static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS);
}

int32_t RSFrameStabilityManager::GetFrameStabilityResult(pid_t pid, const FrameStabilityTarget& target, bool& result)
{
    std::lock_guard<std::mutex> lock(collectorMutex_);
    auto it = screenCollectorContexts_.find(target.id);
    if (it == screenCollectorContexts_.end()) {
        RS_LOGE("GetFrameStabilityResult target not found, pid=%{public}d, "
            "type=%{public}d, id=%{public}" PRIu64, pid, target.type, target.id);
        return static_cast<int32_t>(FrameStabilityErrorCode::TARGET_ID_NOT_REGISTERED);
    }

    auto collectorContext = it->second;

    float percentage = CalculateRegionPercentage(
        collectorContext->collectionDirtyRegion, collectorContext->screenArea);
    result = ROSEN_LE(percentage, collectorContext->config.changePercent);

    screenCollectorContexts_.erase(target.id);
    RS_LOGI("GetFrameStabilityResult success, pid=%{public}d, "
        "type=%{public}d, id=%{public}" PRIu64 ", percentage=%{public}f, result=%{public}d",
        pid, target.type, target.id, percentage, result);

    return static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS);
}

float RSFrameStabilityManager::CalculateRegionPercentage(const Occlusion::Region& region, float screenArea)
{
    if (screenArea <= 0) {
        return 0.0f;
    }

    int64_t regionArea = region.Area();

    return static_cast<float>(regionArea) / screenArea;
}

void RSFrameStabilityManager::TriggerCallback(uint64_t targetId, bool isStable)
{
    auto it = screenDetectorContexts_.find(targetId);
    if (it == screenDetectorContexts_.end()) {
        RS_LOGE("TriggerCallback target not found, id=%{public}" PRIu64, targetId);
        return;
    }

    auto detectorContext = it->second;
    if (!detectorContext->callback) {
        RS_LOGE("TriggerCallback callback is null");
        return;
    }
    RS_LOGI("TriggerCallback, id=%{public}" PRIu64 ", isStable=%{public}d",
        targetId, isStable);
    detectorContext->callback->OnFrameStabilityChanged(isStable);
}

void RSFrameStabilityManager::HandleStabilityTimeout(uint64_t targetId)
{
    // trigger callback only when there is no vsync and state is init or notstable
    std::lock_guard<std::mutex> lock(detectorMutex_);
    auto it = screenDetectorContexts_.find(targetId);
    if (it == screenDetectorContexts_.end()) {
        return;
    }

    auto detectorContext = it->second;
    if (!detectorContext->hasPendingStabilityTask) {
        return;
    }

    detectorContext->hasPendingStabilityTask = false;
    if (detectorContext->state != DetectionState::STABLE) {
        TriggerCallback(targetId, true);
        detectorContext->state = DetectionState::STABLE;
    }
    detectorContext->accumulatedDirtyRegion = Occlusion::Region();
}

void RSFrameStabilityManager::RecordCurrentFrameDirty(
    uint64_t id, const std::vector<RectI>& damageRegionRects, float screenArea)
{
    RecordDirtyToDetector(id, damageRegionRects, screenArea);
    RecordDirtyToCollector(id, damageRegionRects, screenArea);
}

void RSFrameStabilityManager::RecordDirtyToDetector(
    uint64_t id, const std::vector<RectI>& damageRegionRects, float screenArea)
{
    std::lock_guard<std::mutex> lock(detectorMutex_);
    auto it = screenDetectorContexts_.find(id);
    if (it == screenDetectorContexts_.end()) {
        return;
    }
    auto detectorContext = it->second;
    if (detectorContext->callback == nullptr) {
        return;
    }

    for (const auto& rect : damageRegionRects) {
        Occlusion::Region tmpRegion(
            Occlusion::Rect(rect.left_, rect.top_, rect.left_ + rect.width_, rect.top_ + rect.height_));
        detectorContext->accumulatedDirtyRegion = detectorContext->accumulatedDirtyRegion.Or(tmpRegion);
    }

    float percentage = CalculateRegionPercentage(detectorContext->accumulatedDirtyRegion, screenArea);
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - detectorContext->startTime).count();
    
    if (ROSEN_GE(percentage, detectorContext->config.changePercent)) {
        if (detectorContext->state != DetectionState::NOTSTABLE) {
            TriggerCallback(id, false);
            detectorContext->state = DetectionState::NOTSTABLE;
        }
        detectorContext->startTime = currentTime;
        detectorContext->accumulatedDirtyRegion = Occlusion::Region();

        auto task = [this, id]() {
            this->HandleStabilityTimeout(id);
        };
        if (detectorContext->hasPendingStabilityTask) {
            RSUniRenderThread::Instance().RemoveTask(FRAME_STABILITY_TAG + std::to_string(id));
        }
        RSUniRenderThread::Instance().PostTask(
            task, FRAME_STABILITY_TAG + std::to_string(id), detectorContext->config.stableDuration);
        detectorContext->hasPendingStabilityTask = true;
    } else if (elapsed >= detectorContext->config.stableDuration) {
        if (detectorContext->state != DetectionState::STABLE) {
            TriggerCallback(id, true);
            detectorContext->state = DetectionState::STABLE;
        }
        detectorContext->startTime = currentTime;
        detectorContext->accumulatedDirtyRegion = Occlusion::Region();
    }
}

void RSFrameStabilityManager::RecordDirtyToCollector(
    uint64_t id, const std::vector<RectI>& damageRegionRects, float screenArea)
{
    std::lock_guard<std::mutex> lock(collectorMutex_);
    auto it = screenCollectorContexts_.find(id);
    if (it == screenCollectorContexts_.end()) {
        return;
    }

    auto collectorContext = it->second;
    collectorContext->screenArea = screenArea;

    for (const auto& rect : damageRegionRects) {
        Occlusion::Region tmpRegion(
            Occlusion::Rect(rect.left_, rect.top_, rect.left_ + rect.width_, rect.top_ + rect.height_));
        collectorContext->collectionDirtyRegion = collectorContext->collectionDirtyRegion.Or(tmpRegion);
    }
}
} // namespace Rosen
} // namespace OHOS