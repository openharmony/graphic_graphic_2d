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

#ifndef RENDER_SERVICE_BASE_PIPELINE_RS_SURFACE_BUFFER_CALLBACK_MANAGER_H
#define RENDER_SERVICE_BASE_PIPELINE_RS_SURFACE_BUFFER_CALLBACK_MANAGER_H

#include <functional>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include <vector>

#include "refbase.h"

namespace OHOS {
namespace Rosen {

class RSISurfaceBufferCallback;

class RSB_EXPORT RSSurfaceBufferCallbackManager {
public:
    void RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
        sptr<RSISurfaceBufferCallback> callback);
    void UnregisterSurfaceBufferCallback(pid_t pid);
    void UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid);

    std::function<void(pid_t, uint64_t, uint32_t)> GetSurfaceBufferOpItemCallback() const;
    void SetPolicy(std::function<void(std::function<void()>)> runPolicy);

    static RSSurfaceBufferCallbackManager& Instance();
private:
    RSSurfaceBufferCallbackManager() = default;
    ~RSSurfaceBufferCallbackManager() noexcept = default;

    RSSurfaceBufferCallbackManager(const RSSurfaceBufferCallbackManager&) = delete;
    RSSurfaceBufferCallbackManager(RSSurfaceBufferCallbackManager&&) = delete;
    RSSurfaceBufferCallbackManager& operator=(const RSSurfaceBufferCallbackManager&) = delete;
    RSSurfaceBufferCallbackManager& operator=(RSSurfaceBufferCallbackManager&&) = delete;

    sptr<RSISurfaceBufferCallback> GetSurfaceBufferCallback(pid_t pid, uint64_t uid) const;
    size_t GetSurfaceBufferCallbackSize() const;
    
    void EnqueueSurfaceBufferId(pid_t pid, uint64_t uid, uint32_t surfaceBufferId);
    void OnSurfaceBufferOpItemDestruct(pid_t pid, uint64_t uid, uint32_t surfaceBufferId);
    void RunSurfaceBufferCallback();

    std::map<std::pair<pid_t, uint64_t>, sptr<RSISurfaceBufferCallback>>
        surfaceBufferCallbacks_;
    std::map<std::pair<pid_t, uint64_t>, std::vector<uint32_t>> stagingSurfaceBufferIds_;
    mutable std::shared_mutex registerSurfaceBufferCallbackMutex_;
    std::mutex surfaceBufferOpItemMutex_;
    std::function<void(std::function<void()>) runPolicy_ = [](auto task) {
        std::invoke(task);
    };

    friend class RSDrawFrame;
    friend class RSRenderThread;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PIPELINE_RS_SURFACE_BUFFER_CALLBACK_MANAGER_H