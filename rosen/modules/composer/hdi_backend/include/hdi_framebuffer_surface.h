/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef HDI_BACKEND_HDI_FRAMEBUFFER_SURFACE_H
#define HDI_BACKEND_HDI_FRAMEBUFFER_SURFACE_H

#include <condition_variable>
#include <queue>
#include <refbase.h>
#include <surface.h>
#include <iconsumer_surface.h>
#include <sync_fence.h>
#include "surface_buffer.h"

namespace OHOS {
namespace Rosen {
struct FrameBufferEntry {
    FrameBufferEntry(sptr<SurfaceBuffer> buf, sptr<SyncFence> fence, int64_t timeStamp, const Rect& damage)
        : buffer(std::move(buf)), acquireFence(std::move(fence)), acquireTime(timeStamp), damageRect(damage)
    {
    }
    ~FrameBufferEntry() noexcept = default;

    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> acquireFence;
    int64_t acquireTime = 0;
    Rect damageRect = {0};
};

class HdiFramebufferSurface : public RefBase {
public:
    static sptr<HdiFramebufferSurface> CreateFramebufferSurface();
    sptr<OHOS::Surface> GetSurface();
    std::unique_ptr<FrameBufferEntry> GetFramebuffer();
    uint32_t GetBufferQueueSize();
    int32_t ReleaseFramebuffer(
        sptr<SurfaceBuffer> &buffer, const sptr<SyncFence> &releaseFence);
    void ClearFrameBuffer();
    void Dump(std::string &result);

private:
    mutable std::mutex mutex_;
    std::condition_variable bufferCond_;
    sptr<OHOS::IConsumerSurface> consumerSurface_ = nullptr;
    sptr<OHOS::Surface> producerSurface_ = nullptr;
    std::queue<std::unique_ptr<FrameBufferEntry>> availableBuffers_;

    static constexpr uint32_t MAX_BUFFER_SIZE = 3;

    HdiFramebufferSurface();
    virtual ~HdiFramebufferSurface() noexcept;

    void OnBufferAvailable();
    OHOS::SurfaceError SetBufferQueueSize(uint32_t bufferSize);
    OHOS::SurfaceError CreateSurface();

    friend class BufferConsumerListener;
    sptr<IBufferConsumerListener> listener_;
};
} // namespace Rosen
} // namespace OHOS

#endif // HDI_BACKEND_HDI_FRAMEBUFFER_SURFACE_H
