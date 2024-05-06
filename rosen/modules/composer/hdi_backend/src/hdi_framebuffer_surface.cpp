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

#include "hdi_framebuffer_surface.h"

#include "hdi_log.h"
#include "sync_fence.h"

using namespace OHOS;

namespace OHOS {
namespace Rosen {

class BufferConsumerListener : public IBufferConsumerListener {
public:
    explicit BufferConsumerListener(HdiFramebufferSurface& fbSurface) : fbSurface_(fbSurface) {}

    ~BufferConsumerListener() {}

private:
    void OnBufferAvailable() override
    {
        fbSurface_.OnBufferAvailable();
    }

    HdiFramebufferSurface& fbSurface_;
};

HdiFramebufferSurface::HdiFramebufferSurface()
{
}

HdiFramebufferSurface::~HdiFramebufferSurface() noexcept
{
}

sptr<HdiFramebufferSurface> HdiFramebufferSurface::CreateFramebufferSurface()
{
    sptr<HdiFramebufferSurface> fbSurface = new HdiFramebufferSurface();

    SurfaceError ret = fbSurface->CreateSurface();
    if (ret != SURFACE_ERROR_OK) {
        HLOGE("FramebufferSurface CreateSurface failed, ret is %{public}d", ret);
        return nullptr;
    }

    ret = fbSurface->SetBufferQueueSize(MAX_BUFFER_SIZE);
    if (ret != SURFACE_ERROR_OK) {
        HLOGE("FramebufferSurface SetBufferQueueSize failed, ret is %{public}d", ret);
        return nullptr;
    }

    return fbSurface;
}

SurfaceError HdiFramebufferSurface::CreateSurface()
{
    consumerSurface_ = IConsumerSurface::Create("FrameBuffer");

    sptr<IBufferProducer> producer = consumerSurface_->GetProducer();
    producerSurface_ = Surface::CreateSurfaceAsProducer(producer);

    listener_ = new BufferConsumerListener(*this);
    SurfaceError ret = consumerSurface_->RegisterConsumerListener(listener_);
    if (ret != SURFACE_ERROR_OK) {
        return SURFACE_ERROR_NO_CONSUMER;
    }

    return SURFACE_ERROR_OK;
}

SurfaceError HdiFramebufferSurface::SetBufferQueueSize(uint32_t bufferSize)
{
    SurfaceError ret = consumerSurface_->SetQueueSize(bufferSize);
    if (ret != SURFACE_ERROR_OK) {
        HLOGE("fb SetQueueSize failed, ret is %{public}d", ret);
        return ret;
    }

    return SURFACE_ERROR_OK;
}

uint32_t HdiFramebufferSurface::GetBufferQueueSize()
{
    if (consumerSurface_ == nullptr) {
        HLOGE("consumer surface is nullptr.");
        return 0;
    }
    return consumerSurface_->GetQueueSize();
}

void HdiFramebufferSurface::OnBufferAvailable()
{
    sptr<SurfaceBuffer> buffer;
    int64_t timestamp = 0;
    Rect damage = {0};
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    SurfaceError ret = consumerSurface_->AcquireBuffer(buffer, acquireFence,
                                                       timestamp, damage);
    if (ret != SURFACE_ERROR_OK || buffer == nullptr) {
        HLOGE("AcquireBuffer failed, ret is %{public}d", ret);
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    availableBuffers_.push(std::make_unique<FrameBufferEntry>(buffer, acquireFence, timestamp, damage));
    bufferCond_.notify_one();
}

sptr<OHOS::Surface> HdiFramebufferSurface::GetSurface()
{
    return producerSurface_;
}

std::unique_ptr<FrameBufferEntry> HdiFramebufferSurface::GetFramebuffer()
{
    using namespace std::chrono_literals;
    std::unique_lock<std::mutex> lock(mutex_);
    if (availableBuffers_.empty()) {
        bufferCond_.wait_for(lock, 1000ms, [this]() { return !availableBuffers_.empty(); });
    }
    if (availableBuffers_.empty()) {
        return nullptr;
    }
    auto fbEntry = std::move(availableBuffers_.front());
    availableBuffers_.pop();
    return fbEntry;
}

int32_t HdiFramebufferSurface::ReleaseFramebuffer(
    sptr<SurfaceBuffer> &buffer, const sptr<SyncFence>& releaseFence)
{
    if (buffer == nullptr) {
        HLOGI("HdiFramebufferSurface::ReleaseFramebuffer: buffer is null, no need to release.");
        return 0;
    }

    SurfaceError ret = consumerSurface_->ReleaseBuffer(buffer, releaseFence);
    if (ret != SURFACE_ERROR_OK) {
        HLOGE("ReleaseBuffer failed ret is %{public}d", ret);
    }

    return ret;
}

void HdiFramebufferSurface::ClearFrameBuffer()
{
    std::unique_lock<std::mutex> lock(mutex_);
    while (!availableBuffers_.empty()) {
        availableBuffers_.pop();
    }
}

void HdiFramebufferSurface::Dump(std::string &result)
{
    if (consumerSurface_ != nullptr) {
        consumerSurface_->Dump(result);
    }
}
} // namespace Rosen
} // namespace OHOS
