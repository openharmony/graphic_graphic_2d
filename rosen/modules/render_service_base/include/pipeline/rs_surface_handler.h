/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <surface.h>

#include "common/rs_common_def.h"
#include "sync_fence.h"

namespace OHOS {
namespace Rosen {
class RSSurfaceHandler {
public:
    // indicates which node this handler belongs to.
    explicit RSSurfaceHandler(NodeId id) : id_(id) {}
    virtual ~RSSurfaceHandler() noexcept = default;

    struct SurfaceBufferEntry {
        void Reset()
        {
            buffer = nullptr;
            acquireFence = SyncFence::INVALID_FENCE;
            releaseFence = SyncFence::INVALID_FENCE;
            damageRect = Rect {0, 0, 0, 0};
            timestamp = 0;
        }
        sptr<SurfaceBuffer> buffer;
        sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
        sptr<SyncFence> releaseFence = SyncFence::INVALID_FENCE;
        Rect damageRect = {0, 0, 0, 0};
        int64_t timestamp = 0;
    };

    void SetConsumer(const sptr<Surface>& consumer);
    void IncreaseAvailableBuffer();
    int32_t ReduceAvailableBuffer();

    NodeId GetNodeId() const
    {
        return id_;
    }

    void SetDefaultWidthAndHeight(int32_t width, int32_t height)
    {
        if (consumer_ != nullptr) {
            consumer_->SetDefaultWidthAndHeight(width, height);
        }
    }

    void SetBuffer(
        const sptr<SurfaceBuffer>& buffer,
        const sptr<SyncFence>& acquireFence,
        const Rect& damage,
        const int64_t timestamp)
    {
        preBuffer_ = buffer_;
        buffer_.buffer = buffer;
        buffer_.acquireFence = acquireFence;
        buffer_.damageRect = damage;
        buffer_.timestamp = timestamp;
    }

    sptr<SurfaceBuffer> GetBuffer()
    {
        return buffer_.buffer;
    }

    sptr<SyncFence> GetAcquireFence() const
    {
        return buffer_.acquireFence;
    }

    const Rect& GetDamageRegion() const
    {
        return buffer_.damageRect;
    }

    void SetReleaseFence(sptr<SyncFence> fence)
    {
        // The fence which get from hdi is preBuffer's releaseFence now.
        preBuffer_.releaseFence = std::move(fence);
    }

    SurfaceBufferEntry& GetPreBuffer()
    {
        return preBuffer_;
    }

    const sptr<Surface>& GetConsumer() const
    {
        return consumer_;
    }

    int32_t GetAvailableBufferCount() const
    {
        return bufferAvailableCount_;
    }

    int64_t GetTimestamp() const
    {
        return buffer_.timestamp;
    }

    void CleanCache()
    {
        buffer_.Reset();
        preBuffer_.Reset();
        bufferAvailableCount_ = 0;
    }

    void SetGlobalZOrder(float globalZOrder);
    float GetGlobalZOrder() const;

protected:
    sptr<Surface> consumer_;

private:
    NodeId id_ = 0;
    SurfaceBufferEntry buffer_;
    SurfaceBufferEntry preBuffer_;
    float globalZOrder_ = 0.0f;
    std::atomic<int> bufferAvailableCount_ = 0;
};
}
}
#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_HANDLER_H
