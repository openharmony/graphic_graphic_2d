/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_HANDLER_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_HANDLER_H

#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <set>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#ifndef ROSEN_CROSS_PLATFORM
#include <iconsumer_surface.h>
#include <surface.h>
#include "common/rs_optional_trace.h"
#include "sync_fence.h"
#endif

namespace OHOS {
namespace Rosen {
using OnDeleteBufferFunc = std::function<void(uint64_t)>;
using OnReleaseBufferFunc = std::function<void(uint64_t)>;

/**
 * @brief GPU cache cleanup callback type.
 *
 * Used to register a GPU cache cleanup callback in RSSurfaceHandler, so that RSSurfaceHandler
 * does not need to directly depend on GPUCacheManager.
 */
using GPUCacheCleanupCallback = std::function<void(const std::set<uint64_t>&)>;

#ifndef ROSEN_CROSS_PLATFORM
/**
 * @brief Consumer-surface buffer delete listener registrar type.
 *
 * Used to register a buffer-delete listener to IConsumerSurface, so that RSSurfaceHandler does not
 * need to depend on RSBaseRenderEngine or GPUCacheManager directly.
 */
using ConsumerDeleteBufferListenerCallback = std::function<void(const sptr<IConsumerSurface>&)>;
#endif

class RSB_EXPORT RSSurfaceHandler {
public:
    // indicates which node this handler belongs to.
    explicit RSSurfaceHandler(NodeId id) : id_(id) {}
    virtual ~RSSurfaceHandler() noexcept;

    struct BufferOwnerCount {
        BufferOwnerCount() {}

        ~BufferOwnerCount() {
            if (bufferReleaseCb_ != nullptr && bufferId_ != 0) {
                RS_OPTIONAL_TRACE_NAME_FMT("BufferOwnerCount::~BufferOwnerCount bufferId %" PRIu64 " refCount_ %u",
                    bufferId_, refCount_.load());
                bufferReleaseCb_(bufferId_);
                bufferReleaseCb_ = nullptr;
            }
        }

        void AddRef()
        {
            RS_OPTIONAL_TRACE_NAME_FMT("BufferOwnerCount::AddRef bufferId %" PRIu64 " refCount_ %u", bufferId_,
                refCount_.load());
            if (bufferId_ == 0) {
                RS_LOGE("BufferOwnerCount::AddRef bufferId %{public}" PRIu64 " ret %{public}u", bufferId_,
                    refCount_.load());
                return;
            }
            refCount_.fetch_add(1, std::memory_order_relaxed);
        }

        void DecRef()
        {
            RS_OPTIONAL_TRACE_NAME_FMT("BufferOwnerCount::DecRef bufferId %" PRIu64 " refCount_ %u", bufferId_,
                refCount_.load());
            if (bufferId_ == 0) {
                RS_LOGE("BufferOwnerCount::DecRef bufferId %{public}" PRIu64 " ret %{public}u", bufferId_,
                    refCount_.load());
                return;
            }
            auto ret = refCount_.fetch_sub(1, std::memory_order_acq_rel);
            if (ret == 1) {
                if (bufferReleaseCb_ == nullptr) {
                    RS_LOGE("BufferOwnerCount::DecRef bufferReleaseCb_ is nullptr");
                    return;
                }
                bufferReleaseCb_(bufferId_);
                bufferReleaseCb_ = nullptr;
            }
        }

        void OnBufferReleased() {
            DecRef();
        }

        void InsertUniOnDrawSet(uint64_t layerId, uint64_t bufferId)
        {
            RS_OPTIONAL_TRACE_NAME_FMT("InsertUniOnDrawSet layerId:%" PRIu64 " bufferId:%" PRIu64,
                layerId, bufferId);
            std::lock_guard<std::mutex> lock(mapMutex_);
            auto iter = uniOnDrawBuffersMap_.find(layerId);
            if (iter != uniOnDrawBuffersMap_.end()) {
                iter->second.insert(bufferId);
            } else {
                uniOnDrawBuffersMap_.emplace(layerId, std::set<uint64_t>{bufferId});
            }
        }

        void SetUniBufferOwner(uint64_t bufferId, uint64_t screenId)
        {
            RS_OPTIONAL_TRACE_NAME_FMT("SetUniBufferOwner seq:%" PRIu64 " uniSeq:%" PRIu64 " screenId:%",
                bufferId_, bufferId, screenId);
            std::lock_guard<std::mutex> lock(mapMutex_);
            uniBufferOwnerSeqNumMap_[screenId] = bufferId;
        }

        bool CheckLastUniBufferOwner(uint64_t bufferId, uint64_t screenId)
        {
            std::lock_guard<std::mutex> lock(mapMutex_);
            auto iter = uniBufferOwnerSeqNumMap_.find(screenId);
            // If not find screenId, true is returned to release the buffer
            return iter == uniBufferOwnerSeqNumMap_.end() || iter->second == bufferId;
        }

        std::mutex mapMutex_;
        std::map<uint64_t, std::set<uint64_t>> uniOnDrawBuffersMap_;
        std::atomic<int32_t> refCount_{1};
        uint64_t bufferId_ = 0;
        OnReleaseBufferFunc bufferReleaseCb_ = nullptr;
        std::map<uint64_t, uint64_t> uniBufferOwnerSeqNumMap_;
    };

    struct SurfaceBufferEntry {
#ifndef ROSEN_CROSS_PLATFORM
        ~SurfaceBufferEntry() noexcept
        {
            if (bufferDeleteCb_ != nullptr) {
                if (buffer) {
                    buffer->SetBufferDeletedFlag(BufferDeletedFlag::DELETED_FROM_RS);
                    bufferDeleteCb_(buffer->GetBufferId());
                }
            }
        }

        void RegisterDeleteBufferListener(OnDeleteBufferFunc bufferDeleteCb)
        {
            if (bufferDeleteCb_ == nullptr) {
                bufferDeleteCb_ = bufferDeleteCb;
            }
        }
#endif
        void Reset(bool needBufferDeleteCb = true)
        {
#ifndef ROSEN_CROSS_PLATFORM
            if (buffer == nullptr) {
                return;
            }
            if (bufferDeleteCb_ && (!RSSystemProperties::IsUseVulkan() || needBufferDeleteCb)) {
                buffer->SetBufferDeletedFlag(BufferDeletedFlag::DELETED_FROM_RS);
                bufferDeleteCb_(buffer->GetBufferId());
            }
            buffer = nullptr;
            acquireFence = SyncFence::InvalidFence();
            releaseFence = SyncFence::InvalidFence();
            damageRect = Rect {0, 0, 0, 0};
#endif
            timestamp = 0;
        }
#ifndef ROSEN_CROSS_PLATFORM
        sptr<SurfaceBuffer> buffer = nullptr;
        sptr<SyncFence> acquireFence = SyncFence::InvalidFence();
        sptr<SyncFence> releaseFence = SyncFence::InvalidFence();
        Rect damageRect = {0, 0, 0, 0};
        OnDeleteBufferFunc bufferDeleteCb_ = nullptr;
        uint64_t seqNum = 0;
#endif
        int64_t timestamp = 0;

        void RegisterReleaseBufferListener(OnReleaseBufferFunc bufferReleaseCb)
        {
            if (bufferOwnerCount_ && bufferOwnerCount_->bufferReleaseCb_ == nullptr) {
                bufferOwnerCount_->bufferReleaseCb_ = bufferReleaseCb;
            }
        }
        std::shared_ptr<BufferOwnerCount> bufferOwnerCount_ = std::make_shared<BufferOwnerCount>();
    };

    void IncreaseAvailableBuffer();
    void ReduceAvailableBuffer();

    NodeId GetNodeId() const
    {
        return id_;
    }

    void SetDefaultWidthAndHeight(int32_t width, int32_t height)
    {
#ifndef ROSEN_CROSS_PLATFORM
        if (consumer_ != nullptr) {
            consumer_->SetDefaultWidthAndHeight(width, height);
        }
#endif
    }

#ifndef ROSEN_CROSS_PLATFORM
    void SetConsumer(sptr<IConsumerSurface> consumer);

    sptr<IConsumerSurface> GetConsumer() const
    {
        return consumer_;
    }

    void EnsureConsumerDeleteBufferListenerRegistered();

    void SetHoldBuffer(std::shared_ptr<SurfaceBufferEntry> buffer)
    {
        holdBuffer_ = buffer;
    }

    inline std::shared_ptr<SurfaceBufferEntry> GetHoldBuffer()
    {
        return holdBuffer_;
    }

    void SetBuffer(
        const sptr<SurfaceBuffer>& buffer,
        const sptr<SyncFence>& acquireFence,
        const Rect& damage,
        const int64_t timestamp,
        std::shared_ptr<BufferOwnerCount> bufferOwnerCount)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        preBuffer_.Reset();
        preBuffer_ = buffer_;
        buffer_.buffer = buffer;
        buffer_.bufferOwnerCount_ = bufferOwnerCount;
        if (buffer != nullptr) {
            buffer_.seqNum = buffer->GetSeqNum();
            if (buffer_.bufferOwnerCount_) {
                buffer_.bufferOwnerCount_->bufferId_ = buffer->GetBufferId();
            }
        }
        buffer_.acquireFence = acquireFence;
        buffer_.damageRect = damage;
        buffer_.timestamp = timestamp;
    }

    const sptr<SurfaceBuffer> GetBuffer() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_.buffer;
    }

    const std::shared_ptr<BufferOwnerCount> GetBufferOwnerCount() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_.bufferOwnerCount_;
    }

    const std::shared_ptr<BufferOwnerCount> GetPreBufferOwnerCount() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return preBuffer_.bufferOwnerCount_;
    }

    uint64_t GetBufferUsage() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!buffer_.buffer) {
            return 0;
        }
        return buffer_.buffer->GetUsage();
    }

    const sptr<SyncFence> GetAcquireFence() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_.acquireFence;
    }

    const Rect GetDamageRegion() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_.damageRect;
    }

    void SetCurrentReleaseFence(sptr<SyncFence> fence)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        buffer_.releaseFence = fence;
    }

    void SetReleaseFence(sptr<SyncFence> fence)
    {
        // The fence which get from hdi is preBuffer's releaseFence now.
        std::lock_guard<std::mutex> lock(mutex_);
        preBuffer_.releaseFence = std::move(fence);
    }

    void SetBufferSizeChanged(const sptr<SurfaceBuffer>& buffer)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (preBuffer_.buffer == nullptr) {
            return;
        }
        bufferSizeChanged_ = buffer->GetWidth() != preBuffer_.buffer->GetWidth() ||
                             buffer->GetHeight() != preBuffer_.buffer->GetHeight();
    }

    void UpdateBuffer(const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence, const Rect& damage,
        const int64_t timestamp, std::shared_ptr<BufferOwnerCount> bufferOwnerCount);

    void SetBufferTransformTypeChanged(bool flag)
    {
        bufferTransformTypeChanged_ = flag;
    }

    bool CheckScalingModeChanged()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (consumer_ == nullptr || buffer_.buffer == nullptr) {
            return false;
        }

        ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_TO_WINDOW;
        consumer_->GetScalingMode(buffer_.buffer->GetSeqNum(), scalingMode);
        bool ScalingModeChanged_ = scalingMode != scalingModePre;
        scalingModePre = scalingMode;
        return ScalingModeChanged_;
    }

    sptr<SurfaceBuffer> GetPreBuffer()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return preBuffer_.buffer;
    }

    sptr<SyncFence> GetPreBufferAcquireFence()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return preBuffer_.acquireFence;
    }

    sptr<SyncFence> GetPreBufferReleaseFence()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return preBuffer_.releaseFence;
    }
#endif

    void ResetPreBuffer(bool needBufferDeleteCb = true)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        preBuffer_.Reset(needBufferDeleteCb);
    }

    int32_t GetAvailableBufferCount() const
    {
        return bufferAvailableCount_;
    }

    void SetAvailableBufferCount(const int32_t bufferAvailableCount)
    {
        bufferAvailableCount_ = bufferAvailableCount;
    }

    int64_t GetTimestamp() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_.timestamp;
    }

    void CleanCache()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        buffer_.Reset();
        preBuffer_.Reset();
    }

    static void SetGPUCacheCleanupCallback(GPUCacheCleanupCallback callback)
    {
        std::lock_guard<std::mutex> lock(s_gpuCacheCleanupCallbackMutex_);
        s_gpuCacheCleanupCallback = std::move(callback);
    }

#ifndef ROSEN_CROSS_PLATFORM
    static void SetConsumerDeleteBufferListenerCallback(ConsumerDeleteBufferListenerCallback callback)
    {
        std::lock_guard<std::mutex> lock(s_consumerDeleteBufferListenerCallbackMutex_);
        s_consumerDeleteBufferListenerCallback = std::move(callback);
    }
#endif

    void AddGPUCacheToCleanupSet(uint64_t bufferId)
    {
        std::lock_guard<std::mutex> lock(gpuCacheCleanupMutex_);
        gpuCacheCleanupSet_.insert(bufferId);
    }

    void AddGPUCacheToCleanupSet(const std::set<uint64_t>& bufferIds)
    {
        if (bufferIds.empty()) {
            return;
        }
        std::lock_guard<std::mutex> lock(gpuCacheCleanupMutex_);
        gpuCacheCleanupSet_.insert(bufferIds.begin(), bufferIds.end());
    }

    void FlushGPUCacheCleanup()
    {
        std::set<uint64_t> bufferIds;
        {
            std::lock_guard<std::mutex> lock(gpuCacheCleanupMutex_);
            if (gpuCacheCleanupSet_.empty()) {
                return;
            }
            bufferIds.swap(gpuCacheCleanupSet_);
        }

        GPUCacheCleanupCallback callback;
        {
            std::lock_guard<std::mutex> lock(s_gpuCacheCleanupCallbackMutex_);
            callback = s_gpuCacheCleanupCallback;
        }
        if (callback) {
            callback(bufferIds);
        }
    }

    void EnqueueAndFlushGPUCacheCleanup(const std::set<uint64_t>& bufferIds)
    {
        AddGPUCacheToCleanupSet(bufferIds);
        FlushGPUCacheCleanup();
    }

    void ResetBufferAvailableCount()
    {
        bufferAvailableCount_ = 0;
    }

    void SetGlobalZOrder(float globalZOrder);
    float GetGlobalZOrder() const;

    void SetSourceType(uint32_t sourceType)
    {
        sourceType_ = sourceType;
    }

    uint32_t GetSourceType() const
    {
        return sourceType_;
    }

    void SetLastBufferId(const uint64_t bufferId) // must call thisFunc in rsMainThread
    {
        lastBufferId_ = bufferId;
        lastBufferReclaimNum_ = 0;
    }

    void ResetLastBufferInfo() // must call thisFunc in rsMainThread
    {
        lastBufferId_ = 0;
        lastBufferReclaimNum_ = 0;
    }

    bool IsNeedSwapLastBuffer() // must call thisFunc in rsMainThread
    {
        return lastBufferId_ != 0;
    }

    bool ReclaimLastBufferProcess();
    bool ReclaimLastBufferPrepare();
    void TryResumeLastBuffer();

    bool GetBufferSizeChanged()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return bufferSizeChanged_;
    }

    bool GetBufferTransformTypeChanged() const
    {
        return bufferTransformTypeChanged_;
    }

    bool HasConsumer() const
    {
#ifndef ROSEN_CROSS_PLATFORM
        return consumer_ != nullptr;
#else
        return false;
#endif
    }
    inline bool IsCurrentFrameBufferConsumed() const
    {
        return isCurrentFrameBufferConsumed_;
    }
    inline void ResetCurrentFrameBufferConsumed()
    {
        isCurrentFrameBufferConsumed_ = false;
    }
    inline void SetCurrentFrameBufferConsumed()
    {
        isCurrentFrameBufferConsumed_ = true;
    }

#ifndef ROSEN_CROSS_PLATFORM
    void RegisterDeleteBufferListener(OnDeleteBufferFunc bufferDeleteCb)
    {
        if (bufferDeleteCb != nullptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            buffer_.RegisterDeleteBufferListener(bufferDeleteCb);
            preBuffer_.RegisterDeleteBufferListener(bufferDeleteCb);
        }
    }

    void ConsumeAndUpdateBuffer(SurfaceBufferEntry buffer);

    void SetHoldReturnValue(const IConsumerSurface::AcquireBufferReturnValue& returnValue);
    std::shared_ptr<IConsumerSurface::AcquireBufferReturnValue> GetHoldReturnValue()
    {
        return holdReturnValue_;
    }
    void ResetHoldReturnValue()
    {
        holdReturnValue_ = nullptr;
    }
#endif

protected:
#ifndef ROSEN_CROSS_PLATFORM
    sptr<IConsumerSurface> consumer_ = nullptr;
#endif
    bool isCurrentFrameBufferConsumed_ = false;

private:
    void ConsumeAndUpdateBufferInner(SurfaceBufferEntry& buffer);

#ifndef ROSEN_CROSS_PLATFORM
    ScalingMode scalingModePre = ScalingMode::SCALING_MODE_SCALE_TO_WINDOW;
    std::shared_ptr<IConsumerSurface::AcquireBufferReturnValue> holdReturnValue_ = nullptr;
#endif
    NodeId id_ = 0;
    // mutex buffer_ & preBuffer_ & bufferCache_
    mutable std::mutex mutex_;
    SurfaceBufferEntry buffer_;
    SurfaceBufferEntry preBuffer_;
    float globalZOrder_ = 0.0f;
    std::atomic<int> bufferAvailableCount_ = 0;
    bool bufferSizeChanged_ = false;
    bool bufferTransformTypeChanged_ = false;
    uint32_t sourceType_ = 0;
    std::shared_ptr<SurfaceBufferEntry> holdBuffer_ = nullptr;

    // GPU cache cleanup set (owned per RSSurfaceHandler instance).
    std::set<uint64_t> gpuCacheCleanupSet_;
    mutable std::mutex gpuCacheCleanupMutex_;

    // GPU cache cleanup callback (shared across all RSSurfaceHandler instances).
    static GPUCacheCleanupCallback s_gpuCacheCleanupCallback;
    static std::mutex s_gpuCacheCleanupCallbackMutex_;

#ifndef ROSEN_CROSS_PLATFORM
    // Consumer-surface buffer delete listener registrar (shared across all RSSurfaceHandler instances).
    static ConsumerDeleteBufferListenerCallback s_consumerDeleteBufferListenerCallback;
    static std::mutex s_consumerDeleteBufferListenerCallbackMutex_;
#endif

    // The IConsumerSurface::GetUniqueId() that has already been registered for delete-buffer notifications.
    uint64_t registeredConsumerDeleteListenerSurfaceId_ = 0;
    uint64_t lastBufferId_ = 0;
    uint32_t lastBufferReclaimNum_ = 0;
};
}
}
#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_HANDLER_H
