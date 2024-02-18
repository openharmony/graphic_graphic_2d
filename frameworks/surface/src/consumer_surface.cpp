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

#include <cinttypes>

#include "buffer_log.h"
#include "buffer_queue_producer.h"
#include "sync_fence.h"

namespace OHOS {
namespace {
constexpr int32_t CONSUMER_REF_COUNT_IN_CONSUMER_SURFACE = 1;
constexpr int32_t PRODUCER_REF_COUNT_IN_CONSUMER_SURFACE = 2;
}

sptr<Surface> Surface::CreateSurfaceAsConsumer(std::string name, bool isShared)
{
    sptr<ConsumerSurface> surf = new ConsumerSurface(name, isShared);
    if (!surf || surf->Init() != GSERROR_OK) {
        BLOGE("Failure, Reason: consumer surf init failed");
        return nullptr;
    }
    return surf;
}

sptr<IConsumerSurface> IConsumerSurface::Create(std::string name, bool isShared)
{
    sptr<ConsumerSurface> surf = new ConsumerSurface(name, isShared);
    if (!surf || surf->Init() != GSERROR_OK) {
        BLOGE("Failure, Reason: consumer surf init failed");
        return nullptr;
    }
    return surf;
}

ConsumerSurface::ConsumerSurface(const std::string &name, bool isShared)
    : name_(name), isShared_(isShared)
{
    BLOGND("ctor");
    consumer_ = nullptr;
    producer_ = nullptr;
}

ConsumerSurface::~ConsumerSurface()
{
    BLOGND("dtor");
    if (consumer_->GetSptrRefCount() > CONSUMER_REF_COUNT_IN_CONSUMER_SURFACE ||
        producer_->GetSptrRefCount() > PRODUCER_REF_COUNT_IN_CONSUMER_SURFACE) {
        BLOGNE("Wrong SptrRefCount! Queue Id:%{public}" PRIu64 " consumer_:%{public}d producer_:%{public}d",
            producer_->GetUniqueId(), consumer_->GetSptrRefCount(), producer_->GetSptrRefCount());
    }
    consumer_->OnConsumerDied();
    producer_->SetStatus(false);
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
                                       sptr<SyncFence>& fence, BufferRequestConfig &config)
{
    return GSERROR_NOT_SUPPORT;
}
GSError ConsumerSurface::FlushBuffer(sptr<SurfaceBuffer>& buffer,
                                     const sptr<SyncFence>& fence, BufferFlushConfig &config)
{
    return GSERROR_NOT_SUPPORT;
}

GSError ConsumerSurface::FlushBuffer(sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& fence,
                                     BufferFlushConfigWithDamages &config)
{
    return GSERROR_NOT_SUPPORT;
}

GSError ConsumerSurface::GetLastFlushedBuffer(sptr<SurfaceBuffer>& buffer,
    sptr<SyncFence>& fence, float matrix[16])
{
    return GSERROR_NOT_SUPPORT;
}

GSError ConsumerSurface::AcquireBuffer(sptr<SurfaceBuffer>& buffer, sptr<SyncFence>& fence,
                                       int64_t &timestamp, Rect &damage)
{
    std::vector<Rect> damages;
    GSError ret = AcquireBuffer(buffer, fence, timestamp, damages);
    if (ret != GSERROR_OK) {
        return ret;
    }
    if (damages.size() == 1) {
        damage = damages[0];
        return GSERROR_OK;
    }
    BLOGN_FAILURE("AcquireBuffer Success but the size of damages is %{public}zu is not 1, should Acquire damages.",
        damages.size());
    return GSERROR_INVALID_ARGUMENTS;
}

GSError ConsumerSurface::AcquireBuffer(sptr<SurfaceBuffer>& buffer, sptr<SyncFence>& fence,
                                       int64_t &timestamp, std::vector<Rect> &damages)
{
    return consumer_->AcquireBuffer(buffer, fence, timestamp, damages);
}

GSError ConsumerSurface::ReleaseBuffer(sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& fence)
{
    return consumer_->ReleaseBuffer(buffer, fence);
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
    sptr<SyncFence> syncFence = SyncFence::INVALID_FENCE;
    auto ret = AcquireBuffer(buffer, syncFence, timestamp, damage);
    if (ret != GSERROR_OK) {
        fence = -1;
        return ret;
    }
    fence = syncFence->Dup();
    return GSERROR_OK;
}

GSError ConsumerSurface::ReleaseBuffer(sptr<SurfaceBuffer>& buffer, int32_t fence)
{
    sptr<SyncFence> syncFence = new SyncFence(fence);
    return ReleaseBuffer(buffer, syncFence);
}

GSError ConsumerSurface::AttachBuffer(sptr<SurfaceBuffer>& buffer)
{
    if (consumer_ == nullptr) {
        BLOGFE("AttachBuffer failed for nullptr consumer.");
        return GSERROR_INVALID_ARGUMENTS;
    }
    return consumer_->AttachBuffer(buffer);
}

GSError ConsumerSurface::AttachBuffer(sptr<SurfaceBuffer>& buffer, int32_t timeOut)
{
    if (consumer_ == nullptr) {
        BLOGFE("AttachBuffer failed for nullptr consumer.");
        return GSERROR_INVALID_ARGUMENTS;
    }
    return consumer_->AttachBuffer(buffer, timeOut);
}

GSError ConsumerSurface::DetachBuffer(sptr<SurfaceBuffer>& buffer)
{
    return consumer_->DetachBuffer(buffer);
}

GSError ConsumerSurface::RegisterSurfaceDelegator(sptr<IRemoteObject> client)
{
    if (consumer_ == nullptr) {
        BLOGFE("RegisterSurfaceDelegator failed for nullptr consumer.");
        return GSERROR_INVALID_ARGUMENTS;
    }
    return consumer_->RegisterSurfaceDelegator(client, this);
}

bool ConsumerSurface::QueryIfBufferAvailable()
{
    return consumer_->QueryIfBufferAvailable();
}

uint32_t ConsumerSurface::GetQueueSize()
{
    return producer_->GetQueueSize();
}

GSError ConsumerSurface::SetQueueSize(uint32_t queueSize)
{
    return producer_->SetQueueSize(queueSize);
}

const std::string& ConsumerSurface::GetName()
{
    return name_;
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
    std::lock_guard<std::mutex> lockGuard(lockMutex_);
    if (userData_.size() >= SURFACE_MAX_USER_DATA_COUNT) {
        BLOGE("SetUserData failed: userData_ size out");
        return GSERROR_OUT_OF_RANGE;
    }

    auto iterUserData = userData_.find(key);
    if (iterUserData != userData_.end() && iterUserData->second == val) {
        BLOGE("SetUserData failed: key:%{public}s, val:%{public}s exist", key.c_str(), val.c_str());
        return GSERROR_API_FAILED;
    }

    userData_[key] = val;
    auto iter = onUserDataChange_.begin();
    while (iter != onUserDataChange_.end()) {
        if (iter->second != nullptr) {
            iter->second(key, val);
        }
        iter++;
    }

    return GSERROR_OK;
}

std::string ConsumerSurface::GetUserData(const std::string &key)
{
    std::lock_guard<std::mutex> lockGuard(lockMutex_);
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

GSError ConsumerSurface::RegisterReleaseListener(OnReleaseFuncWithFence func)
{
    return GSERROR_NOT_SUPPORT;
}

GSError ConsumerSurface::UnRegisterReleaseListener()
{
    return GSERROR_OK;
}

GSError ConsumerSurface::RegisterDeleteBufferListener(OnDeleteBufferFunc func, bool isForUniRedraw)
{
    return consumer_->RegisterDeleteBufferListener(func, isForUniRedraw);
}

GSError ConsumerSurface::UnregisterConsumerListener()
{
    return consumer_->UnregisterConsumerListener();
}

GSError ConsumerSurface::RegisterUserDataChangeListener(const std::string &funcName, OnUserDataChangeFunc func)
{
    std::lock_guard<std::mutex> lockGuard(lockMutex_);
    if (onUserDataChange_.find(funcName) != onUserDataChange_.end()) {
        BLOGND("func already register");
        return GSERROR_INVALID_ARGUMENTS;
    }
    
    onUserDataChange_[funcName] = func;
    return GSERROR_OK;
}

GSError ConsumerSurface::UnRegisterUserDataChangeListener(const std::string &funcName)
{
    std::lock_guard<std::mutex> lockGuard(lockMutex_);
    if (onUserDataChange_.erase(funcName) == 0) {
        BLOGND("func doesn't register");
        return GSERROR_INVALID_ARGUMENTS;
    }

    return GSERROR_OK;
}

GSError ConsumerSurface::ClearUserDataChangeListener()
{
    std::lock_guard<std::mutex> lockGuard(lockMutex_);
    onUserDataChange_.clear();
    return GSERROR_OK;
}

GSError ConsumerSurface::CleanCache()
{
    return GSERROR_NOT_SUPPORT;
}

GSError ConsumerSurface::GoBackground()
{
    BLOGND("Queue Id:%{public}" PRIu64 "", producer_->GetUniqueId());
    return consumer_->GoBackground();
}

uint64_t ConsumerSurface::GetUniqueId() const
{
    return producer_->GetUniqueId();
}

void ConsumerSurface::Dump(std::string &result) const
{
    return consumer_->Dump(result);
}

GSError ConsumerSurface::SetTransform(GraphicTransformType transform)
{
    return producer_->SetTransform(transform);
}

GraphicTransformType ConsumerSurface::GetTransform() const
{
    return consumer_->GetTransform();
}

GSError ConsumerSurface::IsSupportedAlloc(const std::vector<BufferVerifyAllocInfo> &infos,
                                          std::vector<bool> &supporteds)
{
    return GSERROR_NOT_SUPPORT;
}

GSError ConsumerSurface::Disconnect()
{
    return GSERROR_NOT_SUPPORT;
}

GSError ConsumerSurface::SetScalingMode(uint32_t sequence, ScalingMode scalingMode)
{
    if (scalingMode < ScalingMode::SCALING_MODE_FREEZE ||
        scalingMode > ScalingMode::SCALING_MODE_NO_SCALE_CROP) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return producer_->SetScalingMode(sequence, scalingMode);
}

GSError ConsumerSurface::GetScalingMode(uint32_t sequence, ScalingMode &scalingMode)
{
    return consumer_->GetScalingMode(sequence, scalingMode);
}

GSError ConsumerSurface::SetMetaData(uint32_t sequence, const std::vector<GraphicHDRMetaData> &metaData)
{
    if (metaData.size() == 0) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return producer_->SetMetaData(sequence, metaData);
}

GSError ConsumerSurface::SetMetaDataSet(uint32_t sequence, GraphicHDRMetadataKey key,
                                        const std::vector<uint8_t> &metaData)
{
    if (key < GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X ||
        key > GraphicHDRMetadataKey::GRAPHIC_MATAKEY_HDR_VIVID || metaData.size() == 0) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return producer_->SetMetaDataSet(sequence, key, metaData);
}

GSError ConsumerSurface::QueryMetaDataType(uint32_t sequence, HDRMetaDataType &type) const
{
    return consumer_->QueryMetaDataType(sequence, type);
}

GSError ConsumerSurface::GetMetaData(uint32_t sequence, std::vector<GraphicHDRMetaData> &metaData) const
{
    return consumer_->GetMetaData(sequence, metaData);
}

GSError ConsumerSurface::GetMetaDataSet(uint32_t sequence, GraphicHDRMetadataKey &key,
                                        std::vector<uint8_t> &metaData) const
{
    return consumer_->GetMetaDataSet(sequence, key, metaData);
}

GSError ConsumerSurface::SetTunnelHandle(const GraphicExtDataHandle *handle)
{
    if (handle == nullptr || handle->reserveInts == 0) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return producer_->SetTunnelHandle(handle);
}

sptr<SurfaceTunnelHandle> ConsumerSurface::GetTunnelHandle() const
{
    return consumer_->GetTunnelHandle();
}

GSError ConsumerSurface::SetPresentTimestamp(uint32_t sequence, const GraphicPresentTimestamp &timestamp)
{
    if (timestamp.type == GraphicPresentTimestampType::GRAPHIC_DISPLAY_PTS_UNSUPPORTED) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return consumer_->SetPresentTimestamp(sequence, timestamp);
}

GSError ConsumerSurface::GetPresentTimestamp(uint32_t sequence, GraphicPresentTimestampType type,
                                             int64_t &time) const
{
    return GSERROR_NOT_SUPPORT;
}

int32_t ConsumerSurface::GetDefaultFormat()
{
    BLOGND("ConsumerSurface::GetDefaultFormat not support.");
    return 0;
}

GSError ConsumerSurface::SetDefaultFormat(int32_t format)
{
    BLOGND("ConsumerSurface::SetDefaultFormat not support.");
    return GSERROR_NOT_SUPPORT;
}

int32_t ConsumerSurface::GetDefaultColorGamut()
{
    BLOGND("ConsumerSurface::GetDefaultColorGamut not support.");
    return 0;
}

GSError ConsumerSurface::SetDefaultColorGamut(int32_t colorGamut)
{
    BLOGND("ConsumerSurface::SetDefaultColorGamut not support.");
    return GSERROR_NOT_SUPPORT;
}

sptr<NativeSurface> ConsumerSurface::GetNativeSurface()
{
    BLOGND("ConsumerSurface::GetNativeSurface not support.");
    return nullptr;
}

GSError ConsumerSurface::SetWptrNativeWindowToPSurface(void* nativeWindow)
{
    BLOGND("ConsumerSurface::SetWptrNativeWindowToPSurface not support.");
    return GSERROR_NOT_SUPPORT;
}
} // namespace OHOS
