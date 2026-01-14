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

#include "rs_render_composer_client_manager.h"
#include "rs_frame_report.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int MAX_ACCUMULATED_BUFFER_COUNT = 4;
}
// composer client
void RSRenderComposerClientManager::AddRenderComposerClient(ScreenId screenId,
    const std::shared_ptr<RSRenderComposerClient>& rsRenderComposerClient)
{
    RS_LOGI("RSRenderComposerClientManager::AddRenderComposerClient rsRenderComposerClient[%{public}d]",
        rsRenderComposerClient != nullptr);
    if (GetRSRenderComposerClient(screenId)) {
        RS_LOGI("GetRSRenderComposerClient, return.");
        return;
    }
    std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
    rsRenderComposerClients_[screenId] = rsRenderComposerClient;
}

void RSRenderComposerClientManager::DeleteRSRenderComposerClient(ScreenId screenId)
{
    std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
    rsRenderComposerClients_.erase(screenId);
}

std::shared_ptr<RSRenderComposerClient> RSRenderComposerClientManager::GetRSRenderComposerClient(ScreenId screenId)
{
    std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
    auto it = rsRenderComposerClients_.find(screenId);
    if (it != rsRenderComposerClients_.end()) {
        return it->second;
    } else {
        RS_LOGE("RSRenderComposerClientManager::GetRSRenderComposerClient return nullptr");
        return nullptr;
    }
}

void RSRenderComposerClientManager::ClearRedrawGPUCompositionCache(std::set<uint64_t> bufferIds)
{
    std::unordered_map<ScreenId, std::shared_ptr<RSRenderComposerClient>> clientMap;
    {
        std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
        clientMap = rsRenderComposerClients_;
    }
    for (const auto& [_, client] : clientMap) {
        client->ClearRedrawGPUCompositionCache(bufferIds);
    }
}

void RSRenderComposerClientManager::RenderFrameStart(uint64_t timestamp)
{
    std::unordered_map<ScreenId, std::shared_ptr<RSRenderComposerClient>> clientMap;
    {
        std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
        clientMap = rsRenderComposerClients_;
    }
    if (!clientMap.empty()) {
        uint32_t minBufferCount = INT_MAX;
        for (auto it : clientMap) {
            minBufferCount = std::min(minBufferCount, it.second->GetUnExecuteTaskNum());
        }
        RsFrameReport::GetInstance().ReportBufferCount(minBufferCount);
    }
}

void RSRenderComposerClientManager::CleanLayerBufferBySurfaceId(uint64_t surfaceId, NodeId nodeId)
{
    std::unordered_map<ScreenId, std::shared_ptr<RSRenderComposerClient>> clientMap;
    {
        std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
        clientMap = rsRenderComposerClients_;
    }
    for (auto iter = clientMap.begin(); iter != clientMap.end(); iter++) {
        if (iter->second->GetRSLayer(nodeId) != nullptr) {
            iter->second->CleanLayerBufferBySurfaceId(surfaceId);
            break;
        }
    }
}

void RSRenderComposerClientManager::SetScreenBacklight(ScreenId screenId, uint32_t level)
{
    auto client = GetRSRenderComposerClient(screenId);
    if (client != nullptr) {
        client->SetScreenBacklight(level);
    }
}

PipelineParam RSRenderComposerClientManager::GetPipelineParam(ScreenId screenId)
{
    auto client = GetRSRenderComposerClient(screenId);
    if (client == nullptr) {
        RS_LOGE("GetPipelineParam failed, screenId: %{public}" PRIu64, screenId);
        PipelineParam param;
        return param;
    }
    return client->GetPipelineParam();
}

void RSRenderComposerClientManager::UpdatePipelineParam(ScreenId screenId, const PipelineParam& pipelineParam)
{
    auto client = GetRSRenderComposerClient(screenId);
    if (client != nullptr) {
        client->UpdatePipelineParam(pipelineParam);
    } else {
        RS_LOGE("UpdatePipelineParam failed, screenId: %{public}" PRIu64, screenId);
    }
}

void RSRenderComposerClientManager::PreAllocProtectedFrameBuffers(ScreenId screenId, const sptr<SurfaceBuffer>& buffer)
{
    auto client = GetRSRenderComposerClient(screenId);
    if (client != nullptr) {
        client->PreAllocProtectedFrameBuffers(buffer);
    }
}

void RSRenderComposerClientManager::RegisterOnReleaseLayerBuffersCB(ScreenId screenId, OnReleaseLayerBuffersCB cb)
{
    auto client = GetRSRenderComposerClient(screenId);
    if (client != nullptr) {
        client->RegisterOnReleaseLayerBuffersCB(cb);
    }
}

void RSRenderComposerClientManager::ReleaseLayerBuffers(uint64_t screenId,
    std::vector<std::tuple<RSLayerId, bool, GraphicPresentTimestamp>>& timestampVec,
    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>>& releaseBufferFenceVec)
{
    auto client = GetRSRenderComposerClient(screenId);
    if (client != nullptr) {
        client->ReleaseLayerBuffers(screenId, timestampVec, releaseBufferFenceVec);
    }
}

int32_t RSRenderComposerClientManager::GetMinAccumulatedBufferCount()
{
    std::unordered_map<ScreenId, std::shared_ptr<RSRenderComposerClient>> clientMap;
    {
        std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
        clientMap = rsRenderComposerClients_;
    }
    int minAccumulatedBufferCount = MAX_ACCUMULATED_BUFFER_COUNT;
    if (clientMap.empty()) {
        RS_LOGE("RSUniRenderThread::GetMinAccumulatedBufferCount rsRenderComposerClients_ is empty!");
        return 0;
    }
    for (auto [screenId, client] : clientMap) {
        int curAccumulatedBufferCount = client->GetAccumulatedBufferCount();
        RS_LOGD("RSUniRenderThread::GetMinAccumulatedBufferCount screenId: %{public}" PRIu64 ", count: %{public}d",
            screenId, curAccumulatedBufferCount);
        minAccumulatedBufferCount = std::min(minAccumulatedBufferCount, curAccumulatedBufferCount);
    }
    return minAccumulatedBufferCount;
}

void RSRenderComposerClientManager::DumpSurfaceInfo(std::string& dumpString)
{
    std::unordered_map<ScreenId, std::shared_ptr<RSRenderComposerClient>> clientMap;
    {
        std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
        clientMap = rsRenderComposerClients_;
    }
    for (auto iter = clientMap.begin(); iter != clientMap.end(); iter++) {
        if (iter->second != nullptr) {
            dumpString.append("\n");
            dumpString.append("-- LayerInfo " + std::to_string(iter->first) + "\n");
            iter->second->DumpLayersInfo(dumpString);
        }
    }
}

void RSRenderComposerClientManager::DumpCurrentFrameLayers()
{
    std::unordered_map<ScreenId, std::shared_ptr<RSRenderComposerClient>> clientMap;
    {
        std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
        clientMap = rsRenderComposerClients_;
    }
    for (auto iter = clientMap.begin(); iter != clientMap.end(); iter++) {
        if (iter->second != nullptr) {
            iter->second->DumpCurrentFrameLayers();
        }
    }
}

void RSRenderComposerClientManager::ClearFrameBuffers(const std::set<ScreenId>& screenHasProtectedLayerSet)
{
    std::unordered_map<ScreenId, std::shared_ptr<RSRenderComposerClient>> clientMap;
    {
        std::lock_guard<std::mutex> lock(rsComposerMapMutex_);
        clientMap = rsRenderComposerClients_;
    }
    for (auto [screenId, client] : clientMap) {
        if (screenHasProtectedLayerSet.count(screenId)) {
            RS_TRACE_NAME_FMT("screenHasProtectedLayerSet %" PRIu64 "", screenId);
            continue;
        }
        client->ClearFrameBuffers();
    }
}
} // namespace Rosen
} // namespace OHOS