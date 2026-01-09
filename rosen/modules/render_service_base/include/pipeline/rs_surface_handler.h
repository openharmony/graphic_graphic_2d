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
#include <mutex>
#include <set>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
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
using OnDeleteBufferFunc = std::function<void(uint32_t)>;
using OnReleaseBufferFunc = std::function<void(uint64_t)>;

/**
 * @brief GPU 缓存清理回调类型
 *
 * 用于在 RSSurfaceHandler 中注册 GPU 缓存清理回调
 * 这样 RSSurfaceHandler 不需要直接依赖 RSGPUCacheManager 单例
 */
using GPUCacheCleanupCallback = std::function<void(const std::set<uint64_t>&)>;

class RSB_EXPORT RSSurfaceHandler {
public:
    // indicates which node this handler belongs to.
    explicit RSSurfaceHandler(NodeId id) : id_(id) {}
    virtual ~RSSurfaceHandler() noexcept;

    struct BufferOwnerCount {
        BufferOwnerCount() {}

        ~BufferOwnerCount() {
            if (bufferReleaseCb_ != nullptr && seqNum_ != 0) {
                RS_OPTIONAL_TRACE_NAME_FMT("BufferOwnerCount::~BufferOwnerCount seqNum %u refCount_ %u",
                    uint32_t(seqNum_), refCount_.load());
                bufferReleaseCb_(seqNum_);
                bufferReleaseCb_ = nullptr;
            }
        }

        void AddRef()
        {
            if (seqNum_ == 0) {
                RS_LOGE("BufferOwnerCount::AddRef seqNum %{public}u ret %{public}u", uint32_t(seqNum_),
                    refCount_.load());
                return;
            }
            refCount_.fetch_add(1, std::memory_order_relaxed);
        }

        void DecRef()
        {
            if (seqNum_ == 0) {
                RS_LOGE("BufferOwnerCount::AddRef seqNum %{public}u ret %{public}u", uint32_t(seqNum_),
                    refCount_.load());
                return;
            }
            auto ret = refCount_.fetch_sub(1, std::memory_order_acq_rel);
            if (ret == 1) {
                if (bufferReleaseCb_ == nullptr) {
                    RS_LOGE("BufferOwnerCount::DecRef bufferReleaseCb_ is nullptr");
                    return;
                }
                bufferReleaseCb_(seqNum_);
                bufferReleaseCb_ = nullptr;
            }
        }

        void OnBufferReleased() {
            DecRef();
        }

        void InsertUniOnDrawSet(uint64_t layerId, uint64_t seqNum)
        {
            std::lock_guard<std::mutex> lock(mapMutex_);
            uniOnDrawBufferMap_.erase(layerId);
            uniOnDrawBufferMap_[layerId] = seqNum;
        }

        void SetUniBufferOwner(uint64_t seqNum)
        {
            std::lock_guard<std::mutex> lock(mapMutex_);
            uniBufferOwnerSeqNum_ = seqNum;
        }

        bool CheckLastUniBufferOwner(uint64_t seqNum)
        {
            std::lock_guard<std::mutex> lock(mapMutex_);
            return uniBufferOwnerSeqNum_ == seqNum;
        }

        std::mutex mapMutex_;
        std::map<uint64_t, uint64_t> uniOnDrawBufferMap_;
        std::atomic<int32_t> refCount_{1};
        uint64_t seqNum_ = 0;
        OnReleaseBufferFunc bufferReleaseCb_ = nullptr;
        uint64_t uniBufferOwnerSeqNum_ = 0;
    };

    struct SurfaceBufferEntry {
#ifndef ROSEN_CROSS_PLATFORM
        ~SurfaceBufferEntry() noexcept
        {
            if (bufferDeleteCb_ != nullptr) {
                if (buffer) {
                    buffer->SetBufferDeletedFlag(BufferDeletedFlag::DELETED_FROM_RS);
                }
                bufferDeleteCb_(seqNum);
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
            buffer_.seqNum = buffer->GetBufferId();
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

    /**
     * @brief 注册 GPU 缓存清理回调
     *
     * 允许外部组件（如 RSMainThread）注册一个回调函数，
     * 当 RSSurfaceHandler 需要清理 GPU 缓存时会调用该回调。
     * 这样 RSSurfaceHandler 不需要直接依赖 RSGPUCacheManager 单例。
     *
     * @param callback GPU 缓存清理回调函数
     *
     * @note 该回调是线程安全的，可以从任意线程调用
     * @note 通常在 RSMainThread::Init() 中注册全局清理回调
     */
    static void SetGPUCacheCleanupCallback(GPUCacheCleanupCallback callback)
    {
        s_gpuCacheCleanupCallback = std::move(callback);
    }

    /**
     * @brief 添加 Buffer ID 到 GPU 缓存清理集合
     *
     * 将指定的 Buffer ID 添加到待清理集合中
     *
     * @param bufferId Buffer ID
     */
    void AddGPUCacheToCleanupSet(uint64_t bufferId)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        gpuCacheCleanupSet_.insert(bufferId);
    }

    /**
     * @brief 批量添加 Buffer ID 到 GPU 缓存清理集合
     *
     * @param bufferIds Buffer ID 集合
     */
    void AddGPUCacheToCleanupSet(const std::set<uint64_t>& bufferIds)
    {
        if (bufferIds.empty()) {
            return;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        gpuCacheCleanupSet_.insert(bufferIds.begin(), bufferIds.end());
    }

    void FlushGPUCacheCleanup()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (gpuCacheCleanupSet_.empty()) {
            return;
        }
        if (s_gpuCacheCleanupCallback) {
            s_gpuCacheCleanupCallback(gpuCacheCleanupSet_);
        }
        gpuCacheCleanupSet_.clear();
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

    // GPU 缓存清理集合（每个 RSSurfaceHandler 实例维护自己的集合）
    std::set<uint64_t> gpuCacheCleanupSet_;

    // GPU 缓存清理回调（所有 RSSurfaceHandler 实例共享）
    static GPUCacheCleanupCallback s_gpuCacheCleanupCallback;
};
}
}
#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_HANDLER_H
