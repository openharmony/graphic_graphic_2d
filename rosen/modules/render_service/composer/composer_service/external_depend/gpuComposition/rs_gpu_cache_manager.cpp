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

#include "gpuComposition/rs_gpu_cache_manager.h"

#include <cinttypes>
#include <unordered_set>

#include "engine/rs_base_render_engine.h"
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

std::shared_ptr<GPUCacheManager> GPUCacheManager::Create(RSBaseRenderEngine& renderEngine)
{
    // Use shared_ptr constructor with private constructor
    return std::shared_ptr<GPUCacheManager>(new GPUCacheManager(renderEngine));
}

GPUCacheManager::GPUCacheManager(RSBaseRenderEngine& renderEngine)
    : renderEngine_(renderEngine)
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
    RS_LOGD("GPUCacheManager::ScheduleBufferCleanup: buffer id %{public}" PRIu64 " scheduled for cleanup", bufferId);

    // "Idle" here means there is no in-flight GPU draw scope guarded by GPUGuard (activeDrawCount_ == 0).
    // If idle, cleanup can run immediately; otherwise it is deferred until the last GPUGuard is destroyed.
    if (activeDrawCount_.load() == 0) {
        CleanupPendingBuffers();
    }
}

void GPUCacheManager::SetComposerCacheCleanupCallback(ComposerCacheCleanupCallback callback)
{
    composerCacheCleanupCallback_ = std::move(callback);
}

std::function<void(uint64_t)> GPUCacheManager::CreateBufferDeleteCallback()
{
    // Capture weak_ptr to avoid circular reference
    return [weakSelf = weak_from_this()](uint64_t bufferId) {
        if (auto manager = weakSelf.lock()) {
            manager->ScheduleBufferCleanup(bufferId);
        }
    };
}

std::function<void(int32_t)> GPUCacheManager::CreateBufferDeleteCallback32()
{
    // Capture weak_ptr to avoid circular reference
    return [weakSelf = weak_from_this()](int32_t bufferId) {
        if (auto manager = weakSelf.lock()) {
            manager->ScheduleBufferCleanup(static_cast<uint64_t>(bufferId));
        }
    };
}

void GPUCacheManager::OnGPUDrawStart()
{
    // activeDrawCount_ tracks the number of in-flight GPU draw scopes (managed by GPUGuard).
    // The count is also used as a lightweight "busy/idle" flag (idle when it reaches 0).
    //
    // memory_order_relaxed is sufficient here because this counter is not used to protect any other data;
    // it only gates when cleanup is allowed to run.
    activeDrawCount_.fetch_add(1, std::memory_order_relaxed);
}

void GPUCacheManager::OnGPUDrawEnd()
{
    // fetch_sub returns the value *before* decrement. prevCount == 1 means we just transitioned 1 -> 0 (busy -> idle).
    //
    // Use memory_order_acq_rel so the 1 -> 0 transition becomes a clear synchronization point:
    // - release: prevents the cleanup trigger from being reordered before the logical end of the GPU draw scope
    // - acquire: ensures the thread that observes the transition can safely proceed to cleanup work
    //
    // In many cases memory_order_relaxed would also work for a pure counter, but acq_rel makes the intent explicit
    // because reaching 0 immediately enables cleanup of GPU resources.
    auto prevCount = activeDrawCount_.fetch_sub(1, std::memory_order_acq_rel);

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

    std::unordered_set<uint64_t> bufferIdSet(bufferIds.begin(), bufferIds.end());
    // Cleanup RenderEngine cache
    renderEngine_.ClearCacheSet(bufferIdSet);

    // Cleanup Composer-side cache (optional)
    if (composerCacheCleanupCallback_) {
        composerCacheCleanupCallback_(bufferIdSet);
    }
}

} // namespace Rosen
} // namespace OHOS
