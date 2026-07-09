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

#include "feature/hwc/hpae_offline/rs_offline_processor.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

RSOfflineProcessor::RSOfflineProcessor()
{
}

RSOfflineProcessor::~RSOfflineProcessor()
{
}

RSOfflineProcessor& RSOfflineProcessor::GetOfflineProcessor()
{
    static RSOfflineProcessor processor;
    return processor;
}

OfflineDeviceType RSOfflineProcessor::GetOfflineDeviceType(offlineTaskId taskId)
{
    std::lock_guard<std::mutex> lock(deviceTypeMapMutex_);
    auto it = deviceTypeMap_.find(taskId.second);
    if (it == deviceTypeMap_.end()) {
        return OfflineDeviceType::INVALID;
    } else {
        return it->second;
    }
}

std::shared_ptr<RSOfflineDevice> RSOfflineProcessor::CreateDevice(OfflineDeviceType type)
{
    switch (type) {
        case OfflineDeviceType::HPAE_OFFLINE_DEVICE:
            return std::make_shared<RSHpaeOfflineDevice>();
        case OfflineDeviceType::GPU_OFFLINE_DEVICE:
            return std::make_shared<RSGPUOfflineDevice>();
        default:
            RS_LOGW("RSOfflineProcessor::Unknown offline device type: %{public}d", static_cast<int>(type));
            return nullptr;
    }
}

std::shared_ptr<RSOfflineDevice> RSOfflineProcessor::GetOrCreateOfflineDevice(OfflineDeviceType deviceType,
    bool createIfNotExist)
{
    if (deviceType == OfflineDeviceType::INVALID) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(deviceMutex_);
    auto it = devicePool_.find(deviceType);
    if (it != devicePool_.end()) {
        return it->second;
    }
    if (!createIfNotExist) {
        return nullptr;
    }
    auto offlineDevice = CreateDevice(deviceType);
    if (offlineDevice != nullptr) {
        devicePool_[deviceType] = offlineDevice;
    }
    return offlineDevice;
}

void RSOfflineProcessor::DeleteOfflineDeviceFromType(OfflineDeviceType deviceType)
{
    RS_TRACE_NAME("RSOfflineProcessor::DeleteOfflineDeviceFromType");
    RS_LOGI("RSOfflineProcessor::DeleteOfflineDeviceFromType for type: %{public}d", static_cast<int>(deviceType));
    std::lock_guard<std::mutex> lock(deviceMutex_);
    devicePool_.erase(deviceType);
}

bool RSOfflineProcessor::IsRSOfflineProcessorReady(std::shared_ptr<RSSurfaceRenderNode> surfaceNode,
    OfflineDeviceType deviceType)
{
    RS_TRACE_NAME("RSOfflineProcessor::IsRSOfflineProcessorReady");
    RS_LOGD("RSOfflineProcessor::IsRSOfflineProcessorReady for type: %{public}d", static_cast<int>(deviceType));
    auto offlineDevice = GetOrCreateOfflineDevice(deviceType, true);
    if (offlineDevice == nullptr) {
        RS_LOGW("RSOfflineProcessor::Offline device is invalid");
        return false;
    }
    if (!offlineDevice->IsRSOfflineDeviceReady(surfaceNode)) {
        RS_LOGD("RSOfflineProcessor::Offline device is not ready");
        return false;
    }

    std::lock_guard<std::mutex> lock(deviceTypeMapMutex_);
    if (deviceTypeMap_.size() >= deviceTypeMapMaxSize_) {
        auto oldestNodeId = deviceTypeInsertOrder_.front();
        deviceTypeInsertOrder_.pop_front();
        deviceTypeMap_.erase(oldestNodeId);
        RS_LOGD("RSOfflineProcessor::IsRSOfflineProcessorReady clear deviceTypeMap_: node [%{public}" PRIu64,
            oldestNodeId);
    }
    auto result = deviceTypeMap_.emplace(surfaceNode->GetId(), deviceType);
    if (result.second) {
        deviceTypeInsertOrder_.push_back(surfaceNode->GetId());
    }
    return true;
}

void RSOfflineProcessor::CheckAndPostClearOfflineResourceTask(OfflineDeviceType deviceType,
    const std::vector<uint64_t>& offlineNodeIds)
{
    RS_TRACE_NAME("RSOfflineProcessor::CheckAndPostClearOfflineResourceTask");
    auto offlineDevice = GetOrCreateOfflineDevice(deviceType, false);
    if (offlineDevice == nullptr) {
        return;
    }
    offlineDevice->CheckAndPostClearOfflineResourceTask(offlineNodeIds);
    if (offlineDevice->CanDeleteDevice()) {
        DeleteOfflineDeviceFromType(deviceType);
    }
}

bool RSOfflineProcessor::PostProcessOfflineTask(
    std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, offlineTaskId taskId)
{
    RS_TRACE_NAME("RSOfflineProcessor::PostProcessOfflineTask by node");
    RS_LOGD("RSOfflineProcessor::start to process offline task by node [%{public}" PRIu64 "-%{public}" PRIu64 "]",
            taskId.first, taskId.second);
    OfflineDeviceType deviceType = GetOfflineDeviceType(taskId);
    auto offlineDevice = GetOrCreateOfflineDevice(deviceType, false);
    if (offlineDevice == nullptr) {
        RS_LOGD("RSOfflineProcessor::Offline device not ready in post task for node");
        return false;
    }
    return offlineDevice->PostProcessOfflineTask(surfaceNode, taskId);
}

bool RSOfflineProcessor::PostProcessOfflineTask(
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& surfaceDrawable, offlineTaskId taskId)
{
    RS_TRACE_NAME("RSOfflineProcessor::PostProcessOfflineTask by drawable");
    RS_LOGD("RSOfflineProcessor::start to process offline task by drawable [%{public}" PRIu64 "-%{public}" PRIu64 "]",
            taskId.first, taskId.second);
    OfflineDeviceType deviceType = GetOfflineDeviceType(taskId);
    auto offlineDevice = GetOrCreateOfflineDevice(deviceType, false);
    if (offlineDevice == nullptr) {
        RS_LOGD("RSOfflineProcessor::Offline device not ready in post task for drawable");
        return false;
    }
    return offlineDevice->PostProcessOfflineTask(surfaceDrawable, taskId);
}

bool RSOfflineProcessor::WaitForProcessOfflineResult(offlineTaskId taskId,
    std::chrono::milliseconds timeout, ProcessOfflineResult& processOfflineResult)
{
    RS_TRACE_NAME("RSOfflineProcessor::Wait for node offline process");
    RS_LOGD("RSOfflineProcessor::start to wait for offline result, [%{public}" PRIu64 "-%{public}" PRIu64 "]",
        taskId.first, taskId.second);
    OfflineDeviceType deviceType = GetOfflineDeviceType(taskId);
    auto offlineDevice = GetOrCreateOfflineDevice(deviceType, false);
    if (offlineDevice == nullptr) {
        RS_LOGD("RSOfflineProcessor::Offline device not ready in WaitForProcessOfflineResult");
        return false;
    }
    return offlineDevice->WaitForProcessOfflineResult(taskId, timeout, processOfflineResult);
}

} // namespace Rosen
} // namespace OHOS