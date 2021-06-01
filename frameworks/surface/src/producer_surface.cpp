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

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "ProducerSurface" };
}

ProducerSurface::ProducerSurface(sptr<IBufferProducer>& producer)
{
    BLOGFD("");
    producer_ = producer;
}

ProducerSurface::~ProducerSurface()
{
    BLOGFD("");
    if (IsRemote()) {
        for (auto it = bufferProducerCache_.begin(); it != bufferProducerCache_.end(); it++) {
            if (it->second->GetVirAddr() != nullptr) {
                BufferManager::GetInstance()->Unmap(it->second);
            }
        }
    }

    producer_ = nullptr;
}

SurfaceError ProducerSurface::Init()
{
    return SURFACE_ERROR_OK;
}

sptr<IBufferProducer> ProducerSurface::GetProducer()
{
    return producer_;
}

SurfaceError ProducerSurface::RequestBuffer(sptr<SurfaceBuffer>& buffer,
                                            int32_t& fence, BufferRequestConfig& config)
{
    std::vector<int32_t> deletingBuffers;
    int32_t sequence;

    SurfaceError ret = GetProducer()->RequestBuffer(sequence, buffer, fence, config, deletingBuffers);
    if (ret != SURFACE_ERROR_OK) {
        BLOG_FAILURE("Producer report %{public}s", SurfaceErrorStr(ret).c_str());
        return ret;
    }

    // add cache
    if (buffer != nullptr && IsRemote()) {
        sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(buffer);
        ret = BufferManager::GetInstance()->Map(bufferImpl);
        if (ret != SURFACE_ERROR_OK) {
            BLOG_FAILURE_ID(sequence, "Map failed");
        } else {
            BLOG_SUCCESS_ID(sequence, "Map");
        }
    }

    if (buffer != nullptr) {
        bufferProducerCache_[sequence] = SurfaceBufferImpl::FromBase(buffer);
    } else {
        buffer = bufferProducerCache_[sequence];
    }

    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(buffer);
    ret = BufferManager::GetInstance()->InvalidateCache(bufferImpl);
    if (ret != SURFACE_ERROR_OK) {
        BLOGFW("Warning [%{public}d], InvalidateCache failed", sequence);
    }

    for (auto it = deletingBuffers.begin(); it != deletingBuffers.end(); it++) {
        if (IsRemote() && bufferProducerCache_[*it]->GetVirAddr() != nullptr) {
            BufferManager::GetInstance()->Unmap(bufferProducerCache_[*it]);
        }
        bufferProducerCache_.erase(*it);
    }
    return ret;
}

SurfaceError ProducerSurface::CancelBuffer(sptr<SurfaceBuffer>& buffer)
{
    if (buffer == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }

    return GetProducer()->CancelBuffer(SurfaceBufferImpl::FromBase(buffer)->GetSeqNum());
}

SurfaceError ProducerSurface::FlushBuffer(sptr<SurfaceBuffer>& buffer,
    int32_t fence, BufferFlushConfig& config)
{
    if (buffer == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }

    return GetProducer()->FlushBuffer(SurfaceBufferImpl::FromBase(buffer)->GetSeqNum(), fence, config);
}

SurfaceError ProducerSurface::AcquireBuffer(sptr<SurfaceBuffer>& buffer, int32_t& fence,
                                            int64_t& timestamp, Rect& damage)
{
    return SURFACE_ERROR_NOT_SUPPORT;
}

SurfaceError ProducerSurface::ReleaseBuffer(sptr<SurfaceBuffer>& buffer, int32_t fence)
{
    return SURFACE_ERROR_NOT_SUPPORT;
}

uint32_t     ProducerSurface::GetQueueSize()
{
    return producer_->GetQueueSize();
}

SurfaceError ProducerSurface::SetQueueSize(uint32_t queueSize)
{
    return producer_->SetQueueSize(queueSize);
}

SurfaceError ProducerSurface::SetDefaultWidthAndHeight(int32_t width, int32_t height)
{
    return SURFACE_ERROR_NOT_SUPPORT;
}

int32_t ProducerSurface::GetDefaultWidth()
{
    return producer_->GetDefaultWidth();
}

int32_t ProducerSurface::GetDefaultHeight()
{
    return producer_->GetDefaultHeight();
}

SurfaceError ProducerSurface::SetUserData(const std::string& key, const std::string& val)
{
    if (userData_.size() >= SURFACE_MAX_USER_DATA_COUNT) {
        return SURFACE_ERROR_OUT_OF_RANGE;
    }
    userData_[key] = val;
    return SURFACE_ERROR_OK;
}

std::string  ProducerSurface::GetUserData(const std::string& key) 
{
    if (userData_.find(key) != userData_.end()) {
        return userData_[key];
    }

    return "";
}

SurfaceError ProducerSurface::RegisterConsumerListener(sptr<IBufferConsumerListener>& listener)
{
    return SURFACE_ERROR_NOT_SUPPORT;
}

SurfaceError ProducerSurface::UnregisterConsumerListener()
{
    return SURFACE_ERROR_NOT_SUPPORT;
}

bool ProducerSurface::IsRemote()
{
    return producer_->AsObject()->IsProxyObject();
}

SurfaceError ProducerSurface::CleanCache()
{
    return producer_->CleanCache();
}
} // namespace OHOS
