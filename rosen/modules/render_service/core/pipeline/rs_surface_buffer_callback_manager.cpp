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
#include "platform/common/rs_log.h"
#include "rs_hardware_thread.h"
#include "rs_surface_buffer_callback_manager.h"

namespace OHOS {
namespace Rosen {
void RSSurfaceBufferCallbackManager::RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
    sptr<RSISurfaceBufferCallback> callback)
{
    std::unique_lock<std::shared_mutex> lock { registerSurfaceBufferCallbackMutex_ };
    auto iter = surfaceBufferCallbacks_.find({pid, uid});
    if (iter == std::end(surfaceBufferCallbacks_)) {
        surfaceBufferCallbacks_.insert({{pid, uid}, callback});
    } else {
        RS_LOGE("RSSurfaceBufferCallbackManager::RegisterSurfaceBufferCallback Pair:"
            "[Pid: %{public}d, Uid: %{public}s] exists.", pid, std::to_string(uid).c_str());
    }
}

void RSSurfaceBufferCallbackManager::UnregisterSurfaceBufferCallback(pid_t pid)
{
    std::unique_lock<std::shared_mutex> lock { registerSurfaceBufferCallbackMutex_ };
    EraseIf(surfaceBufferCallbacks_, [pid](auto& pair) {
        return pair.first.first == pid;
    });
}

void RSSurfaceBufferCallbackManager::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    std::unique_lock<std::shared_mutex> lock { registerSurfaceBufferCallbackMutex_ };
    auto iter = surfaceBufferCallbacks_.find({pid, uid});
    if (iter == std::end(surfaceBufferCallbacks_)) {
        RS_LOGE("RSSurfaceBufferCallbackManager::UnregisterSurfaceBufferCallback Pair:"
            "[Pid: %{public}d, Uid: %{public}s] not exists.", pid, std::to_string(uid).c_str());
    } else {
        surfaceBufferCallbacks_.erase(iter);
    }
}

std::function<void(pid_t, uint64_t, uint32_t)> RSSurfaceBufferCallbackManager::GetSurfaceBufferOpItemCallback() const
{
    auto mutablePtr = const_cast<RSSurfaceBufferCallbackManager*>(this);
    return [mutablePtr](pid_t pid, uint64_t uid, uint32_t surfaceBufferId) {
        mutablePtr->OnSurfaceBufferOpItemDestruct(pid, uid, surfaceBufferId);
    };
}

sptr<RSISurfaceBufferCallback> RSSurfaceBufferCallbackManager::GetSurfaceBufferCallback(
    pid_t pid, uint64_t uid) const
{
    std::shared_lock<std::shared_mutex> lock { registerSurfaceBufferCallbackMutex_ };
    auto iter = surfaceBufferCallbacks_.find({pid, uid});
    if (iter == std::cend(surfaceBufferCallbacks_)) {
        RS_LOGE("RSSurfaceBufferCallbackManager::GetSurfaceBufferCallback Pair:"
            "[Pid: %{public}d, Uid: %{public}s] not exists.", pid, std::to_string(uid).c_str());
        return nullptr;
    }
    return iter->second;
}

size_t RSSurfaceBufferCallbackManager::GetSurfaceBufferCallbackSize() const
{
    std::shared_lock<std::shared_mutex> lock { registerSurfaceBufferCallbackMutex_ };
    return surfaceBufferCallbacks_.size();
}

void RSSurfaceBufferCallbackManager::EnqueueSurfaceBufferId(pid_t pid, uint64_t uid, uint32_t surfaceBufferId)
{
    auto iter = stagingSurfaceBufferIds_.find({pid, uid});
    if (iter == std::end(stagingSurfaceBufferIds_)) {
        std::tie(iter, std::ignore) = stagingSurfaceBufferIds_.insert({{pid, uid}, {}});
    }
    iter->second.push_back(surfaceBufferId);
}

void RSSurfaceBufferCallbackManager::OnSurfaceBufferOpItemDestruct(
    pid_t pid, uint64_t uid, uint32_t surfaceBufferId)
{
    std::lock_guard<std::mutex> lock { surfaceBufferOpItemMutex_ };
    if (surfaceBufferCallbacks_.find({pid, uid}) == std::end(surfaceBufferCallbacks_)) {
        RS_LOGE("RSSurfaceBufferCallbackManager::OnSurfaceBufferOpItemDestruct Pair:"
            "[Pid: %{public}d, Uid: %{public}s] Callback not exists.", pid, std::to_string(uid).c_str());
        return;
    }
    EnqueueSurfaceBufferId(pid, uid, surfaceBufferId);
}

void RSSurfaceBufferCallbackManager::RunSurfaceBufferCallback()
{
    RSHardwareThread::Instance().PostTask([this]() {
        if (GetSurfaceBufferCallbackSize() == 0) {
            return;
        }
        std::map<std::pair<pid_t, uint64_t>, std::vector<uint32_t>> surfaceBufferIds;
        {
            std::lock_guard<std::mutex> lock { surfaceBufferOpItemMutex_ };
            surfaceBufferIds.swap(stagingSurfaceBufferIds_);
        }
        for (auto& [code, bufferIds] : surfaceBufferIds) {
            auto [pid, uid] = code;
            auto callback = GetSurfaceBufferCallback(pid, uid);
            if (callback) {
                callback->OnFinish(uid, bufferIds);
            }
        }
    });
}
} // namespace Rosen
} // namespace OHOS