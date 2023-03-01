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

#include <cinttypes>

#include "buffer_log.h"
#include "buffer_manager.h"
#include "buffer_extra_data_impl.h"
#include "sync_fence.h"

namespace OHOS {
namespace {
constexpr int32_t PRODUCER_REF_COUNT_IN_PRODUCER_SURFACE = 1;
}

ProducerSurface::ProducerSurface(sptr<IBufferProducer>& producer)
{
    producer_ = producer;
    BLOGND("ctor");
}

ProducerSurface::~ProducerSurface()
{
    if (producer_->GetSptrRefCount() > PRODUCER_REF_COUNT_IN_PRODUCER_SURFACE) {
        BLOGNE("Wrong SptrRefCount! producer_:%{public}d", producer_->GetSptrRefCount());
    }
    BLOGND("dtor, name:%{public}s, Queue Id:%{public}" PRIu64, name_.c_str(), queueId_);
    auto ret = Disconnect();
    if (ret != GSERROR_OK) {
        BLOGNE("Disconnect failed, %{public}s", GSErrorStr(ret).c_str());
    }
}

GSError ProducerSurface::Init()
{
    if (inited_.load()) {
        return GSERROR_OK;
    }
    auto ret = producer_->GetNameAndUniqueId(name_, queueId_);
    if (ret != GSERROR_OK) {
        BLOGNE("GetNameAndUniqueId failed, %{public}s", GSErrorStr(ret).c_str());
    }
    inited_.store(true);
    BLOGND("ctor, name:%{public}s, Queue Id:%{public}" PRIu64, name_.c_str(), queueId_);
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
                                       sptr<SyncFence>& fence, BufferRequestConfig &config)
{
    IBufferProducer::RequestBufferReturnValue retval;
    sptr<BufferExtraData> bedataimpl = new BufferExtraDataImpl;
    GSError ret = producer_->RequestBuffer(config, bedataimpl, retval);
    if (ret != GSERROR_OK) {
        if (ret == GSERROR_NO_CONSUMER) {
            std::lock_guard<std::mutex> lockGuard(mutex_);
            bufferProducerCache_.clear();
        }
        BLOGND("Producer report %{public}s", GSErrorStr(ret).c_str());
        return ret;
    }

    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (isDisconnected) {
        isDisconnected = false;
    }
    // add cache
    if (retval.buffer != nullptr) {
        bufferProducerCache_[retval.sequence] = retval.buffer;
    } else if (bufferProducerCache_.find(retval.sequence) == bufferProducerCache_.end()) {
        return GSERROR_API_FAILED;
    } else {
        retval.buffer = bufferProducerCache_[retval.sequence];
    }
    buffer = retval.buffer;
    fence = retval.fence;

    if (static_cast<uint64_t>(config.usage) & BUFFER_USAGE_CPU_WRITE) {
        ret = buffer->InvalidateCache();
        if (ret != GSERROR_OK) {
            BLOGNW("Warning [%{public}d], InvalidateCache failed", retval.sequence);
        }
    }

    if (buffer != nullptr) {
        buffer->SetExtraData(bedataimpl);
    }

    for (auto it = retval.deletingBuffers.begin(); it != retval.deletingBuffers.end(); it++) {
        bufferProducerCache_.erase(*it);
    }
    return GSERROR_OK;
}
GSError ProducerSurface::FlushBuffer(sptr<SurfaceBuffer>& buffer,
                                     const sptr<SyncFence>& fence, BufferFlushConfig &config)
{
    if (buffer == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    const sptr<BufferExtraData>& bedata = buffer->GetExtraData();
    return producer_->FlushBuffer(buffer->GetSeqNum(), bedata, fence, config);
}
GSError ProducerSurface::AcquireBuffer(sptr<SurfaceBuffer>& buffer, sptr<SyncFence>& fence,
                                       int64_t &timestamp, Rect &damage)
{
    return GSERROR_NOT_SUPPORT;
}
GSError ProducerSurface::ReleaseBuffer(sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& fence)
{
    return GSERROR_NOT_SUPPORT;
}

GSError ProducerSurface::RequestBuffer(sptr<SurfaceBuffer>& buffer,
    int32_t &fence, BufferRequestConfig &config)
{
    sptr<SyncFence> syncFence = SyncFence::INVALID_FENCE;
    auto ret = RequestBuffer(buffer, syncFence, config);
    if (ret != GSERROR_OK) {
        fence = -1;
        return ret;
    }
    fence = syncFence->Dup();
    return GSERROR_OK;
}

GSError ProducerSurface::CancelBuffer(sptr<SurfaceBuffer>& buffer)
{
    if (buffer == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    const sptr<BufferExtraData>& bedata = buffer->GetExtraData();
    return producer_->CancelBuffer(buffer->GetSeqNum(), bedata);
}

GSError ProducerSurface::FlushBuffer(sptr<SurfaceBuffer>& buffer,
    int32_t fence, BufferFlushConfig &config)
{
    // fence need close?
    sptr<SyncFence> syncFence = new SyncFence(fence);
    return FlushBuffer(buffer, syncFence, config);
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
    return producer_->AttachBuffer(buffer);
}

GSError ProducerSurface::DetachBuffer(sptr<SurfaceBuffer>& buffer)
{
    if (buffer == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return producer_->DetachBuffer(buffer);
}

uint32_t ProducerSurface::GetQueueSize()
{
    return producer_->GetQueueSize();
}

GSError ProducerSurface::SetQueueSize(uint32_t queueSize)
{
    return producer_->SetQueueSize(queueSize);
}

const std::string& ProducerSurface::GetName()
{
    if (!inited_.load()) {
        BLOGNW("Warning ProducerSurface is not initialized, the name you get is uninitialized.");
    }
    return name_;
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

GSError ProducerSurface::RegisterDeleteBufferListener(OnDeleteBufferFunc func)
{
    return GSERROR_NOT_SUPPORT;
}

bool ProducerSurface::IsRemote()
{
    return producer_->AsObject()->IsProxyObject();
}

GSError ProducerSurface::CleanCache()
{
    BLOGND("Queue Id:%{public}" PRIu64, queueId_);
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        bufferProducerCache_.clear();
    }
    return producer_->CleanCache();
}

GSError ProducerSurface::GoBackground()
{
    BLOGND("Queue Id:%{public}" PRIu64 "", queueId_);
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        bufferProducerCache_.clear();
    }
    return producer_->GoBackground();
}

uint64_t ProducerSurface::GetUniqueId() const
{
    if (!inited_.load()) {
        BLOGNW("Warning ProducerSurface is not initialized, the uniquedId you get is uninitialized.");
    }
    return queueId_;
}

GSError ProducerSurface::SetTransform(GraphicTransformType transform)
{
    return producer_->SetTransform(transform);
}

GraphicTransformType ProducerSurface::GetTransform() const
{
    return GraphicTransformType::GRAPHIC_ROTATE_BUTT;
}

GSError ProducerSurface::IsSupportedAlloc(const std::vector<BufferVerifyAllocInfo> &infos,
                                          std::vector<bool> &supporteds)
{
    if (infos.size() == 0 || infos.size() != supporteds.size()) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return producer_->IsSupportedAlloc(infos, supporteds);
}

GSError ProducerSurface::Disconnect()
{
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        if (isDisconnected) {
            return GSERROR_INVALID_OPERATING;
        }
    }
    BLOGND("Queue Id:%{public}" PRIu64 "", queueId_);
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        bufferProducerCache_.clear();
    }
    GSError ret = producer_->Disconnect();
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        if (ret == GSERROR_OK) {
            isDisconnected = true;
        }
    }
    return ret;
}

GSError ProducerSurface::SetScalingMode(uint32_t sequence, ScalingMode scalingMode)
{
    if (scalingMode < ScalingMode::SCALING_MODE_FREEZE ||
        scalingMode > ScalingMode::SCALING_MODE_NO_SCALE_CROP) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return producer_->SetScalingMode(sequence, scalingMode);
}

GSError ProducerSurface::GetScalingMode(uint32_t sequence, ScalingMode &scalingMode)
{
    return GSERROR_NOT_SUPPORT;
}

GSError ProducerSurface::SetMetaData(uint32_t sequence, const std::vector<GraphicHDRMetaData> &metaData)
{
    if (metaData.size() == 0) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return producer_->SetMetaData(sequence, metaData);
}

GSError ProducerSurface::SetMetaDataSet(uint32_t sequence, GraphicHDRMetadataKey key,
                                        const std::vector<uint8_t> &metaData)
{
    if (key < GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X ||
        key > GraphicHDRMetadataKey::GRAPHIC_MATAKEY_HDR_VIVID || metaData.size() == 0) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return producer_->SetMetaDataSet(sequence, key, metaData);
}

GSError ProducerSurface::QueryMetaDataType(uint32_t sequence, HDRMetaDataType &type) const
{
    return GSERROR_NOT_SUPPORT;
}

GSError ProducerSurface::GetMetaData(uint32_t sequence, std::vector<GraphicHDRMetaData> &metaData) const
{
    return GSERROR_NOT_SUPPORT;
}

GSError ProducerSurface::GetMetaDataSet(uint32_t sequence, GraphicHDRMetadataKey &key,
                                        std::vector<uint8_t> &metaData) const
{
    return GSERROR_NOT_SUPPORT;
}

GSError ProducerSurface::SetTunnelHandle(const GraphicExtDataHandle *handle)
{
    return producer_->SetTunnelHandle(handle);
}

sptr<SurfaceTunnelHandle> ProducerSurface::GetTunnelHandle() const
{
    // not support
    return nullptr;
}

GSError ProducerSurface::SetPresentTimestamp(uint32_t sequence, const GraphicPresentTimestamp &timestamp)
{
    return GSERROR_NOT_SUPPORT;
}

GSError ProducerSurface::GetPresentTimestamp(uint32_t sequence, GraphicPresentTimestampType type,
                                             int64_t &time) const
{
    if (type <= GraphicPresentTimestampType::GRAPHIC_DISPLAY_PTS_UNSUPPORTED ||
        type > GraphicPresentTimestampType::GRAPHIC_DISPLAY_PTS_TIMESTAMP) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return producer_->GetPresentTimestamp(sequence, type, time);
}

int32_t ProducerSurface::GetDefaultFormat()
{
    BLOGND("ProducerSurface::GetDefaultFormat not support.");
    return 0;
}

GSError ProducerSurface::SetDefaultFormat(int32_t format)
{
    BLOGND("ProducerSurface::SetDefaultFormat not support.");
    return GSERROR_NOT_SUPPORT;
}

int32_t ProducerSurface::GetDefaultColorGamut()
{
    BLOGND("ProducerSurface::GetDefaultColorGamut not support.");
    return 0;
}

GSError ProducerSurface::SetDefaultColorGamut(int32_t colorGamut)
{
    BLOGND("ProducerSurface::SetDefaultColorGamut not support.");
    return GSERROR_NOT_SUPPORT;
}

sptr<NativeSurface> ProducerSurface::GetNativeSurface()
{
    BLOGND("ProducerSurface::GetNativeSurface not support.");
    return nullptr;
}
} // namespace OHOS
