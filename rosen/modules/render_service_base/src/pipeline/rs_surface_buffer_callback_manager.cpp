/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ipc_callbacks/rs_surface_buffer_callback.h"
#include "pipeline/rs_draw_cmd.h"
#include "pipeline/rs_surface_buffer_callback_manager.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSSurfaceBufferCallbackManager& RSSurfaceBufferCallbackManager::Instance()
{
    static RSSurfaceBufferCallbackManager surfaceBufferCallbackMgr;
    return surfaceBufferCallbackMgr;
}

void RSSurfaceBufferCallbackManager::SetRunPolicy(
    std::function<void(std::function<void()>)> runPolicy)
{
    runPolicy_ = runPolicy;
}

void RSSurfaceBufferCallbackManager::SetVSyncFuncs(VSyncFuncs vSyncFuncs)
{
    vSyncFuncs_ = vSyncFuncs;
}

void RSSurfaceBufferCallbackManager::SetIsUniRender(bool isUniRender)
{
    isUniRender_ = isUniRender;
}

#ifdef ROSEN_OHOS
void RSSurfaceBufferCallbackManager::SetReleaseFence(
    int releaseFenceFd, NodeId rootNodeId)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return;
    }
    std::lock_guard<std::mutex> lock { surfaceBufferOpItemMutex_ };
    for (auto& [_, data] : stagingSurfaceBufferIds_) {
        auto& fences = data.releaseFences;
        size_t idx = 0;
        for (auto& fence : fences) {
            if (data.isRenderedFlags[idx] && data.rootNodeIds[idx] == rootNodeId) {
                fence = new (std::nothrow) SyncFence(::dup(releaseFenceFd));
                if (!fence) {
                    RS_LOGE("RSSurfaceBufferCallbackManager::SetReleaseFence"
                        " Err on creating SyncFence");
                }
            }
            ++idx;
        }
    }
}
#endif

void RSSurfaceBufferCallbackManager::RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
    sptr<RSISurfaceBufferCallback> callback)
{
    std::unique_lock<std::shared_mutex> lock { registerSurfaceBufferCallbackMutex_ };
    auto iter = surfaceBufferCallbacks_.find({pid, uid});
    if (iter == std::end(surfaceBufferCallbacks_)) {
        surfaceBufferCallbacks_.insert({{pid, uid}, callback});
    } else {
        RS_LOGE("RSSurfaceBufferCallbackManager::RegisterSurfaceBufferCallback Pair:"
            "[Pid: %{public}s, Uid: %{public}s] exists.",
            std::to_string(pid).c_str(), std::to_string(uid).c_str());
    }
}

void RSSurfaceBufferCallbackManager::UnregisterSurfaceBufferCallback(pid_t pid)
{
    std::unique_lock<std::shared_mutex> lock { registerSurfaceBufferCallbackMutex_ };
    EraseIf(surfaceBufferCallbacks_, [pid](const auto& pair) {
        return pair.first.first == pid;
    });
}

void RSSurfaceBufferCallbackManager::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    std::unique_lock<std::shared_mutex> lock { registerSurfaceBufferCallbackMutex_ };
    auto iter = surfaceBufferCallbacks_.find({pid, uid});
    if (iter == std::end(surfaceBufferCallbacks_)) {
        RS_LOGE("RSSurfaceBufferCallbackManager::UnregisterSurfaceBufferCallback Pair:"
            "[Pid: %{public}s, Uid: %{public}s] not exists.",
            std::to_string(pid).c_str(), std::to_string(uid).c_str());
    } else {
        surfaceBufferCallbacks_.erase(iter);
    }
}

#ifdef ROSEN_OHOS
RSSurfaceBufferCallbackManager::OnFinishCb RSSurfaceBufferCallbackManager::GetOnFinishCb(
    ) const
{
    auto mutablePtr = const_cast<RSSurfaceBufferCallbackManager*>(this);
    return [mutablePtr](const Drawing::DrawSurfaceBufferFinishCbData& data) {
        mutablePtr->OnFinish(data);
    };
}

RSSurfaceBufferCallbackManager::OnAfterAcquireBufferCb RSSurfaceBufferCallbackManager::GetOnAfterAcquireBufferCb(
    ) const
{
    auto mutablePtr = const_cast<RSSurfaceBufferCallbackManager*>(this);
    return [mutablePtr](const Drawing::DrawSurfaceBufferAfterAcquireCbData& data) {
        mutablePtr->OnAfterAcquireBuffer(data);
    };
}
#endif

sptr<RSISurfaceBufferCallback> RSSurfaceBufferCallbackManager::GetSurfaceBufferCallback(
    pid_t pid, uint64_t uid) const
{
    std::shared_lock<std::shared_mutex> lock { registerSurfaceBufferCallbackMutex_ };
    auto iter = surfaceBufferCallbacks_.find({pid, uid});
    if (iter == std::cend(surfaceBufferCallbacks_)) {
        RS_LOGE("RSSurfaceBufferCallbackManager::GetSurfaceBufferCallback Pair:"
            "[Pid: %{public}s, Uid: %{public}s] not exists.",
            std::to_string(pid).c_str(), std::to_string(uid).c_str());
        return nullptr;
    }
    return iter->second;
}

size_t RSSurfaceBufferCallbackManager::GetSurfaceBufferCallbackSize() const
{
    std::shared_lock<std::shared_mutex> lock { registerSurfaceBufferCallbackMutex_ };
    return surfaceBufferCallbacks_.size();
}

#ifdef ROSEN_OHOS
void RSSurfaceBufferCallbackManager::EnqueueSurfaceBufferId(
    const Drawing::DrawSurfaceBufferFinishCbData& data)
{
    auto iter = stagingSurfaceBufferIds_.find({data.pid, data.uid});
    if (iter == std::end(stagingSurfaceBufferIds_)) {
        std::tie(iter, std::ignore) = stagingSurfaceBufferIds_.insert({{data.pid, data.uid}, {}});
    }
    auto& [surfaceBufferIds, isRenderedFlags, fences, rootNodeIds] = iter->second;
    surfaceBufferIds.push_back(data.surfaceBufferId);
    isRenderedFlags.push_back(static_cast<uint8_t>(data.isRendered));
    fences.push_back(data.releaseFence);
    rootNodeIds.push_back(data.rootNodeId);
}
#endif

void RSSurfaceBufferCallbackManager::RequestNextVSync()
{
    if (vSyncFuncs_.isRequestedNextVSync && !std::invoke(vSyncFuncs_.isRequestedNextVSync)) {
        if (vSyncFuncs_.requestNextVsync) {
            std::invoke(vSyncFuncs_.requestNextVsync);
        }
    }
}

#ifdef ROSEN_OHOS
void RSSurfaceBufferCallbackManager::OnFinish(
    const Drawing::DrawSurfaceBufferFinishCbData& data)
{
    if (auto callback = GetSurfaceBufferCallback(data.pid, data.uid)) {
        if (data.isNeedTriggerCbDirectly) {
            callback->OnFinish({
                .uid = data.uid,
                .surfaceBufferIds = { data.surfaceBufferId },
                .isRenderedFlags = { static_cast<uint8_t>(data.isRendered) },
                .releaseFences = { data.releaseFence },
                .isUniRender = isUniRender_,
            });
        } else {
            {
                std::lock_guard<std::mutex> lock { surfaceBufferOpItemMutex_ };
                EnqueueSurfaceBufferId(data);
            }
            RequestNextVSync();
        }
    } else {
        RS_LOGE("RSSurfaceBufferCallbackManager::OnFinish Pair:"
            "[Pid: %{public}s, Uid: %{public}s] Callback not exists.",
            std::to_string(data.pid).c_str(), std::to_string(data.uid).c_str());
    }
}

void RSSurfaceBufferCallbackManager::OnAfterAcquireBuffer(
    const Drawing::DrawSurfaceBufferAfterAcquireCbData& data)
{
    if (auto callback = GetSurfaceBufferCallback(data.pid, data.uid)) {
        callback->OnAfterAcquireBuffer({
            .uid = data.uid,
            .isUniRender = isUniRender_,
        });
    } else {
        RS_LOGE("RSSurfaceBufferCallbackManager::OnAfterAcquireBuffer Pair:"
            "[Pid: %{public}s, Uid: %{public}s] Callback not exists.",
            std::to_string(data.pid).c_str(), std::to_string(data.uid).c_str());
    }
}
#endif

std::string RSSurfaceBufferCallbackManager::SerializeBufferIdVec(
    const std::vector<uint32_t>& bufferIdVec)
{
    std::string ret;
    for (const auto& id : bufferIdVec) {
        ret += std::to_string(id) + ",";
    }
    if (!ret.empty()) {
        ret.pop_back();
    }
    return ret;
}

void RSSurfaceBufferCallbackManager::RunSurfaceBufferCallback()
{
    if (GetSurfaceBufferCallbackSize() == 0) {
        return;
    }
    std::map<std::pair<pid_t, uint64_t>, BufferQueueData> surfaceBufferIds;
    {
        std::lock_guard<std::mutex> lock { surfaceBufferOpItemMutex_ };
        surfaceBufferIds.swap(stagingSurfaceBufferIds_);
    }
    for (auto& [code, data] : surfaceBufferIds) {
        auto [pid, uid] = code;
        auto callback = GetSurfaceBufferCallback(pid, uid);
        if (callback) {
            if (data.bufferIds.empty()) {
                continue;
            } 
            RS_TRACE_NAME_FMT("RSSurfaceBufferCallbackManager::RunSurfaceBufferCallback"
                "Release Buffer %s", SerializeBufferIdVec(data.bufferIds).c_str());
            callback->OnFinish({
                .uid = uid,
                .surfaceBufferIds = std::move(data.bufferIds),
                .isRenderedFlags = std::move(data.isRenderedFlags),
#ifdef ROSEN_OHOS
                .releaseFences = std::move(data.releaseFences),
#endif
                .isUniRender = isUniRender_,
            });
        }
    }
}
 
#ifdef RS_ENABLE_VK
void RSSurfaceBufferCallbackManager::RunSurfaceBufferSubCallbackForVulkan(NodeId rootNodeId)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return;
    }
 
    if (GetSurfaceBufferCallbackSize() == 0) {
        return;
    }
 
    // Step 1: Extract BufferQueueData by RootNodeId
    std::map<std::pair<pid_t, uint64_t>, BufferQueueData> surfaceBufferIds;
    {
        std::lock_guard<std::mutex> lock { surfaceBufferOpItemMutex_ };
        for (auto &[code, data] : stagingSurfaceBufferIds_) {
            auto [pid, uid] = code;
            auto& dstBufferQueueData = surfaceBufferIds[{pid, uid}];
            auto bufferQueueDataBegin = std::tuple {
                data.bufferIds.begin(), data.isRenderedFlags.begin(),
                data.releaseFences.begin(), data.rootNodeIds.begin() };
            auto bufferQueueDataEnd = std::tuple {
                data.bufferIds.end(), data.isRenderedFlags.end(),
                data.releaseFences.end(), data.rootNodeIds.end() };
            static auto runCondition = [rootNodeId](auto iterTuple) {
                return rootNodeId == *std::get<ROOTNODEIDS_POS>(iterTuple);
            };
            (void) RSSurfaceBufferCallbackMgrUtil::CopyIf(
                bufferQueueDataBegin, bufferQueueDataEnd,
                std::tuple {
                    std::back_inserter(dstBufferQueueData.bufferIds),
                    std::back_inserter(dstBufferQueueData.isRenderedFlags),
                    std::back_inserter(dstBufferQueueData.releaseFences),
                    std::back_inserter(dstBufferQueueData.rootNodeIds),
                }, runCondition);
            auto partitionPoint = RSSurfaceBufferCallbackMgrUtil::RemoveIf(
                bufferQueueDataBegin, bufferQueueDataEnd, runCondition);
            auto resizeSize = std::distance(
                data.bufferIds.begin(), std::get<0>(partitionPoint));
            data.bufferIds.resize(resizeSize);
            data.isRenderedFlags.resize(resizeSize);
            data.releaseFences.resize(resizeSize);
            data.rootNodeIds.resize(resizeSize);
        }
    }
 
    // step 2: Send BufferQueueData to Arkui
    bool isNeedRequestNextVSync = false;
    for (auto& [code, data] : surfaceBufferIds) {
        auto [pid, uid] = code;
        auto callback = GetSurfaceBufferCallback(pid, uid);
        if (callback) {
            if (!data.bufferIds.empty()) {
                isNeedRequestNextVSync = true;
            } else {
                continue;
            }
            RS_TRACE_NAME_FMT("RSSurfaceBufferCallbackManager::RunSurfaceBufferSubCallbackForVulkan"
                "Release Buffer %s", SerializeBufferIdVec(data.bufferIds).c_str());
            callback->OnFinish({
                .uid = uid,
                .surfaceBufferIds = std::move(data.bufferIds),
                .isRenderedFlags = std::move(data.isRenderedFlags),
                .releaseFences = std::move(data.releaseFences),
                .isUniRender = isUniRender_,
            });
        }
    }
    if (isNeedRequestNextVSync) {
        RequestNextVSync();
    }
}
#endif
} // namespace Rosen
} // namespace OHOS