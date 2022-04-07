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

#include "producer_surface.h"

#include "buffer_log.h"
#include "buffer_manager.h"
#include "buffer_extra_data_impl.h"

namespace OHOS {
ProducerSurface::ProducerSurface(sptr<IBufferProducer>& producer)
{
    producer_ = producer;
    auto sret = producer_->GetName(name_);
    if (sret != GSERROR_OK) {
        BLOGNE("GetName failed, %{public}s", GSErrorStr(sret).c_str());
    }
    BLOGND("ctor");
}

ProducerSurface::~ProducerSurface()
{
    BLOGND("dtor");
    CleanCache();
    producer_ = nullptr;
}

GSError ProducerSurface::Init()
{
    return GSERROR_OK;
}

bool ProducerSurface::IsConsumer() const
{
    return false;
}

sptr<IBufferProducer> ProducerSurface::GetProducer() const
{
    return producer_;
}

GSError ProducerSurface::RequestBuffer(sptr<SurfaceBuffer>& buffer,
    int32_t &fence, BufferRequestConfig &config)
{
    IBufferProducer::RequestBufferReturnValue retval;
    sptr<BufferExtraData> bedataimpl = new BufferExtraDataImpl;
    GSError ret = GetProducer()->RequestBuffer(config, bedataimpl, retval);
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Producer report %{public}s", GSErrorStr(ret).c_str());
        return ret;
    }

    std::lock_guard<std::mutex> lockGuard(mutex_);
    // add cache
    if (retval.buffer != nullptr && IsRemote()) {
        ret = BufferManager::GetInstance()->Map(retval.buffer);
        if (ret != GSERROR_OK) {
            BLOGN_FAILURE_ID(retval.sequence, "Map failed");
            return GSERROR_API_FAILED;
        } else {
            BLOGN_SUCCESS_ID(retval.sequence, "Map");
        }
    }

    if (retval.buffer != nullptr) {
        bufferProducerCache_[retval.sequence] = retval.buffer;
    } else if (bufferProducerCache_.find(retval.sequence) == bufferProducerCache_.end()) {
        return GSERROR_API_FAILED;
    } else {
        retval.buffer = bufferProducerCache_[retval.sequence];
    }
    buffer = retval.buffer;
    fence = retval.fence;

    if (static_cast<uint32_t>(config.usage) & HBM_USE_CPU_WRITE) {
        ret = BufferManager::GetInstance()->InvalidateCache(buffer);
        if (ret != GSERROR_OK) {
            BLOGNW("Warning [%{public}d], InvalidateCache failed", retval.sequence);
        }
    }

    if (buffer != nullptr) {
        buffer->SetExtraData(bedataimpl);
    }

    for (auto it = retval.deletingBuffers.begin(); it != retval.deletingBuffers.end(); it++) {
        if (IsRemote() && bufferProducerCache_.find(*it) != bufferProducerCache_.end() &&
                bufferProducerCache_[*it]->GetVirAddr() != nullptr) {
            BufferManager::GetInstance()->Unmap(bufferProducerCache_[*it]);
        }
        bufferProducerCache_.erase(*it);
    }
    return GSERROR_OK;
}

GSError ProducerSurface::CancelBuffer(sptr<SurfaceBuffer>& buffer)
{
    if (buffer == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    sptr<BufferExtraData> bedata = nullptr;
    buffer->GetExtraData(bedata);
    return GetProducer()->CancelBuffer(buffer->GetSeqNum(), bedata);
}

GSError ProducerSurface::FlushBuffer(sptr<SurfaceBuffer>& buffer,
    int32_t fence, BufferFlushConfig &config)
{
    if (buffer == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    sptr<BufferExtraData> bedata = nullptr;
    buffer->GetExtraData(bedata);
    return GetProducer()->FlushBuffer(buffer->GetSeqNum(), bedata, fence, config);
}

GSError ProducerSurface::AcquireBuffer(sptr<SurfaceBuffer>& buffer, int32_t &fence,
    int64_t &timestamp, Rect &damage)
{
    return GSERROR_NOT_SUPPORT;
}

GSError ProducerSurface::ReleaseBuffer(sptr<SurfaceBuffer>& buffer, int32_t fence)
{
    return GSERROR_NOT_SUPPORT;
}

GSError ProducerSurface::AttachBuffer(sptr<SurfaceBuffer>& buffer)
{
    if (buffer == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    BLOGND("the addr : %{public}p", buffer.GetRefPtr());
    return GetProducer()->AttachBuffer(buffer);
}

GSError ProducerSurface::DetachBuffer(sptr<SurfaceBuffer>& buffer)
{
    if (buffer == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return GetProducer()->DetachBuffer(buffer);
}

uint32_t ProducerSurface::GetQueueSize()
{
    return producer_->GetQueueSize();
}

GSError ProducerSurface::SetQueueSize(uint32_t queueSize)
{
    return producer_->SetQueueSize(queueSize);
}

GSError ProducerSurface::GetName(std::string &name)
{
    auto sret = producer_->GetName(name);
    name_ = name;
    return sret;
}

GSError ProducerSurface::SetDefaultWidthAndHeight(int32_t width, int32_t height)
{
    return GSERROR_NOT_SUPPORT;
}

int32_t ProducerSurface::GetDefaultWidth()
{
    return producer_->GetDefaultWidth();
}

int32_t ProducerSurface::GetDefaultHeight()
{
    return producer_->GetDefaultHeight();
}

GSError ProducerSurface::SetDefaultUsage(uint32_t usage)
{
    return GSERROR_NOT_SUPPORT;
}

uint32_t ProducerSurface::GetDefaultUsage()
{
    return producer_->GetDefaultUsage();
}

GSError ProducerSurface::SetUserData(const std::string &key, const std::string &val)
{
    if (userData_.size() >= SURFACE_MAX_USER_DATA_COUNT) {
        return GSERROR_OUT_OF_RANGE;
    }
    userData_[key] = val;
    return GSERROR_OK;
}

std::string ProducerSurface::GetUserData(const std::string &key)
{
    if (userData_.find(key) != userData_.end()) {
        return userData_[key];
    }

    return "";
}

GSError ProducerSurface::RegisterConsumerListener(sptr<IBufferConsumerListener>& listener)
{
    return GSERROR_NOT_SUPPORT;
}

GSError ProducerSurface::RegisterConsumerListener(IBufferConsumerListenerClazz *listener)
{
    return GSERROR_NOT_SUPPORT;
}

GSError ProducerSurface::UnregisterConsumerListener()
{
    return GSERROR_NOT_SUPPORT;
}

GSError ProducerSurface::RegisterReleaseListener(OnReleaseFunc func)
{
    return producer_->RegisterReleaseListener(func);
}

bool ProducerSurface::IsRemote()
{
    return producer_->AsObject()->IsProxyObject();
}

GSError ProducerSurface::CleanCache()
{
    if (IsRemote()) {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        for (auto it = bufferProducerCache_.begin(); it != bufferProducerCache_.end();) {
            if (it->second != nullptr && it->second->GetVirAddr() != nullptr) {
                BufferManager::GetInstance()->Unmap(it->second);
            }
            bufferProducerCache_.erase(it++);
        }
    }
    return producer_->CleanCache();
}

uint64_t ProducerSurface::GetUniqueId() const
{
    return producer_->GetUniqueId();
}

GSError ProducerSurface::SetTransform(TransformType transform)
{
    return producer_->SetTransform(transform);
}

TransformType ProducerSurface::GetTransform() const
{
    return TransformType::ROTATE_BUTT;
}
} // namespace OHOS
