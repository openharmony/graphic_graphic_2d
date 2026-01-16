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

#ifndef RS_GPU_CACHE_MANAGER_V2_H
#define RS_GPU_CACHE_MANAGER_V2_H

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <set>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

// Forward declarations
class RSBaseRenderEngine;
class RSComposerClientManager;

/**
 * @brief GPU Operation Guard (RAII)
 *
 * Manages GPU operation lifecycle using reference counting.
 * Automatically calls EndGPUDraw when destroyed.
 *
 * Usage:
 * @code
 * {
 *     auto guard = cacheManager->CreateGuard();
 *     // ... GPU operations ...
 * }  // Automatically calls EndGPUDraw()
 * @endcode
 */
class GPUGuard {
public:
    explicit GPUGuard(std::shared_ptr<class GPUCacheManager> manager);
    ~GPUGuard();

    // Non-copyable
    GPUGuard(const GPUGuard&) = delete;
    GPUGuard& operator=(const GPUGuard&) = delete;

    // Movable
    GPUGuard(GPUGuard&& other) noexcept;
    GPUGuard& operator=(GPUGuard&& other) noexcept;

private:
    std::shared_ptr<class GPUCacheManager> manager_;
};

/**
 * @brief GPU Cache Manager
 *
 * Manages GPU cache cleanup for cross-component coordination.
 * Owned by RSMainThread, not a singleton.
 *
 * Design Principles:
 * - SRP: Only responsible for GPU cache coordination
 * - DIP: Dependencies injected via constructor
 * - RAII: Automatic resource management for GPU operations
 *
 * Based on AOSP's callback-driven pattern:
 * - Event-driven cleanup (via callbacks from graphic_surface)
 * - RAII-based lifecycle for GPU operations (GPUGuard)
 */
class GPUCacheManager : public std::enable_shared_from_this<GPUCacheManager> {
public:

    /**
     * @brief Factory method to create GPUCacheManager
     * @param renderEngine RenderEngine to use for cleanup
     * @return std::shared_ptr<GPUCacheManager> Shared pointer to manager
     *
     * @note Uses shared_ptr to enable shared_from_this for weak references
     */
    static std::shared_ptr<GPUCacheManager> Create(RSBaseRenderEngine& renderEngine);

    /**
     * @brief Create GPU operation guard (RAII)
     * @return GPUGuard Guard that automatically manages GPU draw lifecycle
     *
     * Usage:
     * @code
     * auto guard = manager->CreateGuard();
     * // ... GPU operations ...
     * // guard automatically destroyed, calling EndGPUDraw()
     * @endcode
     */
    GPUGuard CreateGuard();

    /**
     * @brief Schedule batch buffer cleanup
     * @param bufferIds Set of buffer IDs to cleanup
     *
     * This method is called when buffer deletion events occur (via callbacks).
     * Buffers will be cleaned up when GPU draw count reaches zero.
     *
     * Usage:
     * @code
     * std::set<uint64_t> bufferIds = {...};
     * manager->ScheduleBufferCleanup(bufferIds);
     * @endcode
     */
    void ScheduleBufferCleanup(const std::set<uint64_t>& bufferIds);

    /**
     * @brief Schedule single buffer cleanup
     * @param bufferId Buffer ID to cleanup
     */
    void ScheduleBufferCleanup(uint64_t bufferId);

    /**
     * @brief Set RSComposerClientManager for cache cleanup
     *
     * Using dependency injection pattern to avoid hard-coded dependency.
     *
     * @param manager RSComposerClientManager to use for cleanup
     */
    void SetComposerClientManager(const std::shared_ptr<RSComposerClientManager>& manager);

    /**
     * @brief Create buffer delete callback (uint64_t version)
     *
     * Instance method for creating buffer delete callbacks.
     * Used by RSBaseRenderEngine to register cleanup callbacks to graphic_surface.
     *
     * @return std::function<void(uint64_t)> Buffer delete callback
     */
    std::function<void(uint64_t)> CreateBufferDeleteCallback();

    /**
     * @brief Create buffer delete callback (int32_t version)
     *
     * Instance method for creating buffer delete callbacks.
     * Used by RSBaseRenderEngine to register cleanup callbacks to graphic_surface.
     *
     * @return std::function<void(int32_t)> Buffer delete callback
     */
    std::function<void(int32_t)> CreateBufferDeleteCallback32();

    /**
     * @brief Get current GPU draw count
     * @return int32_t Current active GPU operation count
     */
    int32_t GetGPUDrawCount() const { return activeDrawCount_.load(); }

    // Disable copy and move (use shared_ptr for sharing)
    GPUCacheManager(const GPUCacheManager&) = delete;
    GPUCacheManager& operator=(const GPUCacheManager&) = delete;
    GPUCacheManager(GPUCacheManager&&) = delete;
    GPUCacheManager& operator=(GPUCacheManager&&) = delete;

private:
    /**
     * @brief Private constructor (use Create() factory method)
     * @param renderEngine RenderEngine to use for GPU cache cleanup
     */
    explicit GPUCacheManager(RSBaseRenderEngine& renderEngine);

    // Internal cleanup methods (called by GPUGuard)
    void OnGPUDrawStart();
    void OnGPUDrawEnd();

    // Cleanup pending buffers (called when GPU draw count reaches zero)
    void CleanupPendingBuffers();

    // RenderEngine for cache cleanup (global object, won't be destroyed)
    RSBaseRenderEngine& renderEngine_;

    // RSComposerClientManager for cache cleanup (dependency injection)
    std::weak_ptr<RSComposerClientManager> composerClientManager_;

    // Active GPU draw count (atomic operation, thread-safe)
    std::atomic<int32_t> activeDrawCount_{0};

    // Pending cleanup buffer set (needs mutex protection)
    std::set<uint64_t> pendingCleanupBuffers_;
    mutable std::mutex cleanupMutex_;

    // Allow GPUGuard to call private methods
    friend class GPUGuard;
};

} // namespace Rosen
} // namespace OHOS

#endif // RS_GPU_CACHE_MANAGER_V2_H
