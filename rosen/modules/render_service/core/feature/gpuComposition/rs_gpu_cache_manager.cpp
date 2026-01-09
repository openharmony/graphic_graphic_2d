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

#include "rs_gpu_cache_manager.h"

#include "pipeline/render_thread/rs_base_render_engine.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

GPUGuard::GPUGuard(std::shared_ptr<GPUCacheManager> manager)
    : manager_(std::move(manager))
{
    if (manager_) {
        manager_->OnGPUDrawStart();
    }
}

GPUGuard::~GPUGuard()
{
    if (manager_) {
        manager_->OnGPUDrawEnd();
    }
}

GPUGuard::GPUGuard(GPUGuard&& other) noexcept
    : manager_(std::move(other.manager_))
{
    // other.manager_ is now null, destructor will do nothing
}

GPUGuard& GPUGuard::operator=(GPUGuard&& other) noexcept
{
    if (this != &other) {
        // Clean up current manager first
        if (manager_) {
            manager_->OnGPUDrawEnd();
        }

        manager_ = std::move(other.manager_);
        // other.manager_ is now null
    }
    return *this;
}

std::shared_ptr<GPUCacheManager> GPUCacheManager::Create(std::shared_ptr<RSBaseRenderEngine> renderEngine)
{
    if (!renderEngine) {
        RS_LOGE("GPUCacheManager::Create: renderEngine is nullptr");
        return nullptr;
    }

    // Use shared_ptr constructor with private constructor
    return std::shared_ptr<GPUCacheManager>(new GPUCacheManager(std::move(renderEngine)));
}

GPUCacheManager::GPUCacheManager(std::shared_ptr<RSBaseRenderEngine> renderEngine)
    : renderEngine_(std::move(renderEngine))
{
    RS_LOGD("GPUCacheManager created");
}

GPUGuard GPUCacheManager::CreateGuard()
{
    return GPUGuard(shared_from_this());
}

void GPUCacheManager::ScheduleBufferCleanup(const std::set<uint64_t>& bufferIds)
{
    if (bufferIds.empty()) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(cleanupMutex_);
        pendingCleanupBuffers_.insert(bufferIds.begin(), bufferIds.end());
    }

    RS_LOGD("GPUCacheManager::ScheduleBufferCleanup: scheduled %{public}zu buffers for cleanup",
        bufferIds.size());

    // If no active GPU draws, cleanup immediately
    if (activeDrawCount_.load() == 0) {
        CleanupPendingBuffers();
    }
}

void GPUCacheManager::ScheduleBufferCleanup(uint64_t bufferId)
{
    {
        std::lock_guard<std::mutex> lock(cleanupMutex_);
        pendingCleanupBuffers_.insert(bufferId);
    }
    RS_LOGD("GPUCacheManager::ScheduleBufferCleanup: buffer id %{public}llu scheduled for cleanup",
        static_cast<unsigned long long>(bufferId));

    // If no active GPU draws, cleanup immediately
    if (activeDrawCount_.load() == 0) {
        CleanupPendingBuffers();
    }
}

void GPUCacheManager::SetComposerClientMapProvider(ComposerClientMapFunc callback)
{
    getComposerClientMapCallback_ = std::move(callback);
}

std::function<void(uint64_t)> GPUCacheManager::CreateBufferDeleteCallback()
{
    // Capture weak_ptr to avoid circular reference
    std::weak_ptr<GPUCacheManager> weakSelf = shared_from_this();
    return [weakSelf](uint64_t bufferId) {
        auto manager = weakSelf.lock();
        if (manager) {
            manager->ScheduleBufferCleanup(bufferId);
        }
    };
}

std::function<void(int32_t)> GPUCacheManager::CreateBufferDeleteCallback32()
{
    // Capture weak_ptr to avoid circular reference
    std::weak_ptr<GPUCacheManager> weakSelf = shared_from_this();
    return [weakSelf](int32_t bufferId) {
        auto manager = weakSelf.lock();
        if (manager) {
            manager->ScheduleBufferCleanup(static_cast<uint64_t>(bufferId));
        }
    };
}

void GPUCacheManager::OnGPUDrawStart()
{
    activeDrawCount_.fetch_add(1, std::memory_order_relaxed);
    RS_LOGD("GPUCacheManager::OnGPUDrawStart: count = %{public}d", activeDrawCount_.load());
}

void GPUCacheManager::OnGPUDrawEnd()
{
    auto prevCount = activeDrawCount_.fetch_sub(1, std::memory_order_acq_rel);
    RS_LOGD("GPUCacheManager::OnGPUDrawEnd: count = %{public}d (was %{public}d)",
        activeDrawCount_.load(), prevCount);

    // Cleanup buffers when GPU draw count reaches zero
    if (prevCount == 1) {
        CleanupPendingBuffers();
    }
}

void GPUCacheManager::CleanupPendingBuffers()
{
    std::set<uint64_t> bufferIds;

    {
        std::lock_guard<std::mutex> lock(cleanupMutex_);
        if (pendingCleanupBuffers_.empty()) {
            return;
        }
        bufferIds.swap(pendingCleanupBuffers_);
    }

    RS_LOGD("GPUCacheManager::CleanupPendingBuffers: cleaning %{public}zu buffers", bufferIds.size());

    // Cleanup RenderEngine cache
    if (renderEngine_) {
        renderEngine_->ClearCacheSet(bufferIds);
    }

    // Cleanup Composer Client cache (if callback is set)
    if (getComposerClientMapCallback_) {
        try {
            auto clientMap = getComposerClientMapCallback_();
            for (const auto& [screenId, client] : clientMap) {
                if (client) {
                    client->ClearRedrawGPUCompositionCache(bufferIds);
                }
            }
        } catch (const std::exception& e) {
            RS_LOGE("GPUCacheManager::CleanupPendingBuffers: exception in ComposerClient cleanup: %{public}s",
                e.what());
        }
    }
}

} // namespace Rosen
} // namespace OHOS
