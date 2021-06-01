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

#include "consumer_surface.h"

#include <vector>

#include "buffer_log.h"
#include "buffer_queue_producer.h"

namespace OHOS {
namespace {
static constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "ConsumerSurface" };
}

ConsumerSurface::ConsumerSurface()
{
    BLOGFD("");
    consumer_ = nullptr;
    producer_ = nullptr;
}

ConsumerSurface::~ConsumerSurface()
{
    BLOGFD("");
    consumer_ = nullptr;
    producer_ = nullptr;
}

SurfaceError ConsumerSurface::Init()
{
    sptr<BufferQueue> queue_ = new BufferQueue();
    SurfaceError ret = queue_->Init();
    if (ret != SURFACE_ERROR_OK) {
        BLOG_FAILURE("queue init failed");
        return ret;
    }

    producer_ = new BufferQueueProducer(queue_);
    consumer_ = new BufferQueueConsumer(queue_);
    return SURFACE_ERROR_OK;
}

sptr<IBufferProducer> ConsumerSurface::GetProducer()
{
    return producer_;
}

SurfaceError ConsumerSurface::RequestBuffer(sptr<SurfaceBuffer>& buffer,
                                            int32_t& fence, BufferRequestConfig& config)
{
    return SURFACE_ERROR_NOT_SUPPORT;
}

SurfaceError ConsumerSurface::CancelBuffer(sptr<SurfaceBuffer>& buffer)
{
    return SURFACE_ERROR_NOT_SUPPORT;
}

SurfaceError ConsumerSurface::FlushBuffer(sptr<SurfaceBuffer>& buffer,
                                          int32_t fence, BufferFlushConfig& config)
{
    return SURFACE_ERROR_NOT_SUPPORT;
}

SurfaceError ConsumerSurface::AcquireBuffer(sptr<SurfaceBuffer>& buffer, int32_t& fence,
                                            int64_t& timestamp, Rect& damage)
{
    SurfaceError ret;
    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(buffer);
    ret = consumer_->AcquireBuffer(bufferImpl, fence, timestamp, damage);
    buffer = bufferImpl;
    return ret;
}

SurfaceError ConsumerSurface::ReleaseBuffer(sptr<SurfaceBuffer>& buffer, int32_t fence)
{
    SurfaceError ret;
    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(buffer);
    ret = consumer_->ReleaseBuffer(bufferImpl, fence);
    buffer = bufferImpl;
    return ret;
}

uint32_t     ConsumerSurface::GetQueueSize()
{
    return producer_->GetQueueSize();
}

SurfaceError ConsumerSurface::SetQueueSize(uint32_t queueSize)
{
    return producer_->SetQueueSize(queueSize);
}

SurfaceError ConsumerSurface::SetDefaultWidthAndHeight(int32_t width, int32_t height)
{
    return consumer_->SetDefaultWidthAndHeight(width, height);
}

int32_t ConsumerSurface::GetDefaultWidth()
{
    return producer_->GetDefaultWidth();
}

int32_t ConsumerSurface::GetDefaultHeight()
{
    return producer_->GetDefaultHeight();
}

SurfaceError ConsumerSurface::SetUserData(const std::string& key, const std::string& val)
{
    if (userData_.size() >= SURFACE_MAX_USER_DATA_COUNT) {
        return SURFACE_ERROR_OUT_OF_RANGE;
    }
    userData_[key] = val;
    return SURFACE_ERROR_OK;
}

std::string ConsumerSurface::GetUserData(const std::string& key)
{
    if (userData_.find(key) != userData_.end()) {
        return userData_[key];
    }

    return "";
}

SurfaceError ConsumerSurface::RegisterConsumerListener(sptr<IBufferConsumerListener>& listener)
{
    return consumer_->RegisterConsumerListener(listener);
}

SurfaceError ConsumerSurface::UnregisterConsumerListener()
{
    return consumer_->UnregisterConsumerListener();
}

SurfaceError ConsumerSurface::CleanCache()
{
    return producer_->CleanCache();
}
} // namespace OHOS
