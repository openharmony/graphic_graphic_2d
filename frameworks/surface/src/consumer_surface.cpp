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

#include "buffer_log.h"
#include "buffer_queue_producer.h"

namespace OHOS {
ConsumerSurface::ConsumerSurface(const std::string &name, bool isShared)
    : name_(name), isShared_(isShared)
{
    BLOGNI("ctor");
    consumer_ = nullptr;
    producer_ = nullptr;
}

ConsumerSurface::~ConsumerSurface()
{
    BLOGNI("dtor");
    consumer_ = nullptr;
    producer_ = nullptr;
}

GSError ConsumerSurface::Init()
{
    sptr<BufferQueue> queue_ = new BufferQueue(name_, isShared_);
    GSError ret = queue_->Init();
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("queue init failed");
        return ret;
    }

    producer_ = new BufferQueueProducer(queue_);
    consumer_ = new BufferQueueConsumer(queue_);
    return GSERROR_OK;
}

bool ConsumerSurface::IsConsumer() const
{
    return true;
}

sptr<IBufferProducer> ConsumerSurface::GetProducer() const
{
    return producer_;
}

GSError ConsumerSurface::RequestBuffer(sptr<SurfaceBuffer>& buffer,
                                       int32_t &fence, BufferRequestConfig &config)
{
    return GSERROR_NOT_SUPPORT;
}

GSError ConsumerSurface::CancelBuffer(sptr<SurfaceBuffer>& buffer)
{
    return GSERROR_NOT_SUPPORT;
}

GSError ConsumerSurface::FlushBuffer(sptr<SurfaceBuffer>& buffer,
                                     int32_t fence, BufferFlushConfig &config)
{
    return GSERROR_NOT_SUPPORT;
}

GSError ConsumerSurface::AcquireBuffer(sptr<SurfaceBuffer>& buffer, int32_t &fence,
                                       int64_t &timestamp, Rect &damage)
{
    return consumer_->AcquireBuffer(buffer, fence, timestamp, damage);
}

GSError ConsumerSurface::ReleaseBuffer(sptr<SurfaceBuffer>& buffer, int32_t fence)
{
    return consumer_->ReleaseBuffer(buffer, fence);
}

GSError ConsumerSurface::AttachBuffer(sptr<SurfaceBuffer>& buffer)
{
    return consumer_->AttachBuffer(buffer);
}

GSError ConsumerSurface::DetachBuffer(sptr<SurfaceBuffer>& buffer)
{
    return consumer_->DetachBuffer(buffer);
}

uint32_t ConsumerSurface::GetQueueSize()
{
    return producer_->GetQueueSize();
}

GSError ConsumerSurface::SetQueueSize(uint32_t queueSize)
{
    return producer_->SetQueueSize(queueSize);
}

GSError ConsumerSurface::GetName(std::string &name)
{
    name = name_;
    return GSERROR_OK;
}

GSError ConsumerSurface::SetDefaultWidthAndHeight(int32_t width, int32_t height)
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

GSError ConsumerSurface::SetDefaultUsage(uint32_t usage)
{
    return consumer_->SetDefaultUsage(usage);
}

uint32_t ConsumerSurface::GetDefaultUsage()
{
    return producer_->GetDefaultUsage();
}

GSError ConsumerSurface::SetUserData(const std::string &key, const std::string &val)
{
    if (userData_.size() >= SURFACE_MAX_USER_DATA_COUNT) {
        return GSERROR_OUT_OF_RANGE;
    }
    userData_[key] = val;
    return GSERROR_OK;
}

std::string ConsumerSurface::GetUserData(const std::string &key)
{
    if (userData_.find(key) != userData_.end()) {
        return userData_[key];
    }

    return "";
}

GSError ConsumerSurface::RegisterConsumerListener(sptr<IBufferConsumerListener>& listener)
{
    return consumer_->RegisterConsumerListener(listener);
}

GSError ConsumerSurface::RegisterConsumerListener(IBufferConsumerListenerClazz *listener)
{
    return consumer_->RegisterConsumerListener(listener);
}

GSError ConsumerSurface::RegisterReleaseListener(OnReleaseFunc func)
{
    return consumer_->RegisterReleaseListener(func);
}

GSError ConsumerSurface::UnregisterConsumerListener()
{
    return consumer_->UnregisterConsumerListener();
}

GSError ConsumerSurface::CleanCache()
{
    return producer_->CleanCache();
}

uint64_t ConsumerSurface::GetUniqueId() const
{
    return producer_->GetUniqueId();
}

void ConsumerSurface::Dump(std::string &result) const
{
    return consumer_->Dump(result);
}

GSError ConsumerSurface::SetTransform(TransformType transform)
{
    return producer_->SetTransform(transform);
}

TransformType ConsumerSurface::GetTransform() const
{
    return consumer_->GetTransform();
}
} // namespace OHOS
