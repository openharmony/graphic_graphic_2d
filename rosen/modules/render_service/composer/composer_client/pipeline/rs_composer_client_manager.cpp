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

#include "rs_composer_client_manager.h"
#include "rs_frame_report.h"
#include "rs_trace.h"

#undef LOG_TAG
#define LOG_TAG "RSComposerClientManager"
namespace OHOS {
namespace Rosen {
namespace {
constexpr int MAX_ACCUMULATED_BUFFER_COUNT = 4;
}
void RSComposerClientManager::AddComposerClient(ScreenId screenId,
    const std::shared_ptr<RSComposerClient>& rsComposerClient)
{
    RS_LOGI("%{public}s rsComposerClient[%{public}d]", __func__, rsComposerClient != nullptr);
    if (GetComposerClient(screenId)) {
        RS_LOGI("GetComposerClient, return.");
        return;
    }
    {
        std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
        composerClientMap_[screenId] = rsComposerClient;
    }
    RsFrameReport::ReportAddScreenId(screenId);
}

void RSComposerClientManager::DeleteComposerClient(ScreenId screenId)
{
    {
        std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
        composerClientMap_.erase(screenId);
    }
    RsFrameReport::ReportDelScreenId(screenId);
}

std::shared_ptr<RSComposerClient> RSComposerClientManager::GetComposerClient(ScreenId screenId)
{
    std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
    if (auto it = composerClientMap_.find(screenId); it != composerClientMap_.end()) {
        return it->second;
    } else {
        RS_LOGE("%{public}s return nullptr", __func__);
        return nullptr;
    }
}

void RSComposerClientManager::ClearRedrawGPUCompositionCache(const std::unordered_set<uint64_t>& bufferIds)
{
    std::unordered_map<ScreenId, std::shared_ptr<RSComposerClient>> clientMap;
    {
        std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
        clientMap = composerClientMap_;
    }
    for (const auto& [_, client] : clientMap) {
        client->ClearRedrawGPUCompositionCache(bufferIds);
    }
}

void RSComposerClientManager::RenderFrameStart(uint64_t timestamp)
{
    std::unordered_map<ScreenId, std::shared_ptr<RSComposerClient>> clientMap;
    {
        std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
        clientMap = composerClientMap_;
    }
    if (!clientMap.empty()) {
        uint32_t minBufferCount = INT_MAX;
        for (auto it : clientMap) {
            minBufferCount = std::min(minBufferCount, it.second->GetUnExecuteTaskNum());
        }
        RsFrameReport::ReportBufferCount(minBufferCount);
    }
}

void RSComposerClientManager::CleanLayerBufferBySurfaceId(uint64_t surfaceId, NodeId nodeId)
{
    std::unordered_map<ScreenId, std::shared_ptr<RSComposerClient>> clientMap;
    {
        std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
        clientMap = composerClientMap_;
    }
    for (auto iter = clientMap.begin(); iter != clientMap.end(); iter++) {
        if (iter->second->IsFindRSLayer(nodeId)) {
            iter->second->CleanLayerBufferBySurfaceId(surfaceId);
            break;
        }
    }
}

void RSComposerClientManager::SetScreenBacklight(ScreenId screenId, uint32_t level)
{
    if (auto client = GetComposerClient(screenId)) {
        client->SetScreenBacklight(level);
    }
}

PipelineParam RSComposerClientManager::GetPipelineParam(ScreenId screenId)
{
    if (auto client = GetComposerClient(screenId)) {
        return client->GetPipelineParam();
    } else {
        RS_LOGE("%{public}s failed, screenId: %{public}" PRIu64, __func__, screenId);
        return {};
    }
}

void RSComposerClientManager::UpdatePipelineParam(ScreenId screenId, const PipelineParam& pipelineParam)
{
    if (auto client = GetComposerClient(screenId)) {
        client->UpdatePipelineParam(pipelineParam);
    } else {
        RS_LOGE("%{public}s failed, screenId: %{public}" PRIu64, __func__, screenId);
    }
}

void RSComposerClientManager::PreAllocProtectedFrameBuffers(ScreenId screenId, const sptr<SurfaceBuffer>& buffer)
{
    if (auto client = GetComposerClient(screenId)) {
        client->PreAllocProtectedFrameBuffers(buffer);
    }
}

void RSComposerClientManager::RegisterOnReleaseLayerBuffersCB(ScreenId screenId, OnReleaseLayerBuffersCB cb)
{
    if (auto client = GetComposerClient(screenId)) {
        client->RegisterOnReleaseLayerBuffersCB(cb);
    }
}

void RSComposerClientManager::ReleaseLayerBuffers(uint64_t screenId,
    std::vector<std::tuple<RSLayerId, bool, GraphicPresentTimestamp>>& timestampVec,
    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>>& releaseBufferFenceVec)
{
    if (auto client = GetComposerClient(screenId)) {
        client->ReleaseLayerBuffers(screenId, timestampVec, releaseBufferFenceVec);
    }
}

int32_t RSComposerClientManager::GetMinAccumulatedBufferCount()
{
    std::unordered_map<ScreenId, std::shared_ptr<RSComposerClient>> clientMap;
    {
        std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
        clientMap = composerClientMap_;
    }
    if (clientMap.empty()) {
        RS_LOGE("%{public}s composerClientMap_ is empty!", __func__);
        return 0;
    }
    int32_t minAccumulatedBufferCount = MAX_ACCUMULATED_BUFFER_COUNT;
    for (auto& [screenId, client] : clientMap) {
        int32_t curAccumulatedBufferCount = client->GetAccumulatedBufferCount();
        RS_LOGD("%{public}s screenId: %{public}" PRIu64 ", count: %{public}d",
            __func__, screenId, curAccumulatedBufferCount);
        minAccumulatedBufferCount = std::min(minAccumulatedBufferCount, curAccumulatedBufferCount);
    }
    return minAccumulatedBufferCount;
}

void RSComposerClientManager::DumpSurfaceInfo(std::string& dumpString)
{
    std::unordered_map<ScreenId, std::shared_ptr<RSComposerClient>> clientMap;
    {
        std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
        clientMap = composerClientMap_;
    }
    for (auto iter = clientMap.begin(); iter != clientMap.end(); iter++) {
        if (iter->second != nullptr) {
            dumpString.append("\n");
            dumpString.append("-- LayerInfo ScreenId: " + std::to_string(iter->first) + "\n");
            iter->second->DumpLayersInfo(dumpString);
        }
    }
}

void RSComposerClientManager::DumpCurrentFrameLayers()
{
    std::unordered_map<ScreenId, std::shared_ptr<RSComposerClient>> clientMap;
    {
        std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
        clientMap = composerClientMap_;
    }
    for (auto iter = clientMap.begin(); iter != clientMap.end(); iter++) {
        if (iter->second != nullptr) {
            iter->second->DumpCurrentFrameLayers();
        }
    }
}

void RSComposerClientManager::ClearFrameBuffers(const std::unordered_set<ScreenId>& screenHasProtectedLayerSet)
{
    std::unordered_map<ScreenId, std::shared_ptr<RSComposerClient>> clientMap;
    {
        std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
        clientMap = composerClientMap_;
    }
    for (auto& [screenId, client] : clientMap) {
        if (screenHasProtectedLayerSet.count(screenId)) {
            RS_TRACE_NAME_FMT("screenHasProtectedLayerSet %" PRIu64, screenId);
            continue;
        }
        client->ClearFrameBuffers();
    }
}
} // namespace Rosen
} // namespace OHOS