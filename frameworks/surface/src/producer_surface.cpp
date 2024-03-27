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
#include "buffer_extra_data_impl.h"
#include "buffer_producer_listener.h"
#include "sync_fence.h"
#include "native_window.h"

namespace OHOS {
namespace {
constexpr int32_t PRODUCER_REF_COUNT_IN_PRODUCER_SURFACE = 1;
}

sptr<Surface> Surface::CreateSurfaceAsProducer(sptr<IBufferProducer>& producer)
{
    if (producer == nullptr) {
        BLOGE("Failure, Reason: producer is nullptr");
        return nullptr;
    }

    sptr<ProducerSurface> surf = new ProducerSurface(producer);
    GSError ret = surf->Init();
    if (ret != GSERROR_OK) {
        BLOGE("Failure, Reason: producer surf init failed");
        return nullptr;
    }
    return surf;
}

ProducerSurface::ProducerSurface(sptr<IBufferProducer>& producer)
{
    producer_ = producer;
    if (producer_) {
        producer_->SendDeathRecipientObject();
    }
    BLOGND("ctor");
}

ProducerSurface::~ProducerSurface()
{
    if (producer_->GetSptrRefCount() > PRODUCER_REF_COUNT_IN_PRODUCER_SURFACE) {
        BLOGND("Warning SptrRefCount! producer_:%{public}d", producer_->GetSptrRefCount());
    }
    BLOGND("dtor, name:%{public}s, Queue Id:%{public}" PRIu64, name_.c_str(), queueId_);
    auto ret = Disconnect();
    if (ret != GSERROR_OK) {
        BLOGND("Disconnect failed, %{public}s", GSErrorStr(ret).c_str());
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
            CleanCache();
        }
        BLOGND("RequestBuffer Producer report %{public}s", GSErrorStr(ret).c_str());
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
        retval.buffer->SetSurfaceBufferColorGamut(config.colorGamut);
        retval.buffer->SetSurfaceBufferTransform(config.transform);
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
        uint32_t seqNum = static_cast<uint32_t>(*it);
        bufferProducerCache_.erase(seqNum);
        auto spNativeWindow = wpNativeWindow_.promote();
        if (spNativeWindow != nullptr) {
            auto &bufferCache = spNativeWindow->bufferCache_;
            if (bufferCache.find(seqNum) != bufferCache.end()) {
                NativeObjectUnreference(bufferCache[seqNum]);
                bufferCache.erase(seqNum);
            }
        }
    }
    return GSERROR_OK;
}

GSError ProducerSurface::FlushBuffer(sptr<SurfaceBuffer>& buffer,
                                     const sptr<SyncFence>& fence, BufferFlushConfig &config)
{
    BufferFlushConfigWithDamages configWithDamages;
    configWithDamages.damages.push_back(config.damage);
    configWithDamages.timestamp = config.timestamp;
    return FlushBuffer(buffer, fence, configWithDamages);
}

GSError ProducerSurface::FlushBuffer(sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& fence,
                                     BufferFlushConfigWithDamages &config)
{
    if (buffer == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    const sptr<BufferExtraData>& bedata = buffer->GetExtraData();
    auto ret = producer_->FlushBuffer(buffer->GetSeqNum(), bedata, fence, config);
    if (ret == GSERROR_NO_CONSUMER) {
        CleanCache();
        BLOGND("FlushBuffer Producer report %{public}s", GSErrorStr(ret).c_str());
    }
    return ret;
}

GSError ProducerSurface::GetLastFlushedBuffer(sptr<SurfaceBuffer>& buffer,
    sptr<SyncFence>& fence, float matrix[16])
{
    auto ret = producer_->GetLastFlushedBuffer(buffer, fence, matrix);
    return ret;
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

GSError ProducerSurface::AttachBufferToQueue(sptr<SurfaceBuffer>& buffer)
{
    if (buffer == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    auto ret = producer_->AttachBufferToQueue(buffer);
    if (ret == GSERROR_OK) {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        if (bufferProducerCache_.find(buffer->GetSeqNum()) != bufferProducerCache_.end()) {
            BLOGNE("Attach already exist buffer %{public}d", buffer->GetSeqNum());
            return GSERROR_API_FAILED;
        }
        bufferProducerCache_[buffer->GetSeqNum()] = buffer;
    }
    return ret;
}

GSError ProducerSurface::DetachBufferFromQueue(sptr<SurfaceBuffer>& buffer)
{
    if (buffer == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    auto ret = producer_->DetachBufferFromQueue(buffer);
    if (ret == GSERROR_OK) {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        if (bufferProducerCache_.find(buffer->GetSeqNum()) == bufferProducerCache_.end()) {
            BLOGNE("Detach not exist buffer %{public}d", buffer->GetSeqNum());
            return GSERROR_API_FAILED;
        }
        bufferProducerCache_.erase(buffer->GetSeqNum());
    }
    return ret;
}

GSError ProducerSurface::AttachBuffer(sptr<SurfaceBuffer>& buffer)
{
    if (buffer == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    return producer_->AttachBuffer(buffer);
}

GSError ProducerSurface::AttachBuffer(sptr<SurfaceBuffer>& buffer, int32_t timeOut)
{
    if (buffer == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    return producer_->AttachBuffer(buffer, timeOut);
}

GSError ProducerSurface::DetachBuffer(sptr<SurfaceBuffer>& buffer)
{
    if (buffer == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return producer_->DetachBuffer(buffer);
}

GSError ProducerSurface::RegisterSurfaceDelegator(sptr<IRemoteObject> client)
{
    if (client == nullptr) {
        BLOGE("RegisterSurfaceDelegator failed for the delegator client is nullptr");
        return GSERROR_INVALID_ARGUMENTS;
    }
    sptr<ProducerSurfaceDelegator> surfaceDelegator = ProducerSurfaceDelegator::Create();
    if (surfaceDelegator == nullptr) {
        BLOGE("RegisterSurfaceDelegator failed for the surface delegator is nullptr");
        return GSERROR_INVALID_ARGUMENTS;
    }
    if (!surfaceDelegator->SetClient(client)) {
        BLOGE("Set the surface delegator client failed");
        return GSERROR_INVALID_ARGUMENTS;
    }

    surfaceDelegator->SetSurface(this);
    wpPSurfaceDelegator_ = surfaceDelegator;

    auto releaseBufferCallBack = [this] (const sptr<SurfaceBuffer> &buffer,
        const sptr<SyncFence> &fence) -> GSError {
        auto surfaceDelegator = this->wpPSurfaceDelegator_.promote();
        if (surfaceDelegator == nullptr) {
            return GSERROR_INVALID_ARGUMENTS;
        }
        int error = surfaceDelegator->ReleaseBuffer(buffer, fence);
        return static_cast<GSError>(error);
    };
    RegisterReleaseListener(releaseBufferCallBack);
    return GSERROR_OK;
}

bool ProducerSurface::QueryIfBufferAvailable()
{
    return false;
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

GSError ProducerSurface::SetDefaultUsage(uint64_t usage)
{
    return producer_->SetDefaultUsage(usage);
}

uint64_t ProducerSurface::GetDefaultUsage()
{
    return producer_->GetDefaultUsage();
}

GSError ProducerSurface::SetUserData(const std::string &key, const std::string &val)
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

std::string ProducerSurface::GetUserData(const std::string &key)
{
    std::lock_guard<std::mutex> lockGuard(lockMutex_);
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
    if (func == nullptr) {
        BLOGNE("OnReleaseFunc is nullptr, RegisterReleaseListener failed.");
        return GSERROR_INVALID_ARGUMENTS;
    }
    listener_ = new BufferReleaseProducerListener(func);
    return producer_->RegisterReleaseListener(listener_);
}

GSError ProducerSurface::RegisterReleaseListener(OnReleaseFuncWithFence funcWithFence)
{
    if (funcWithFence == nullptr) {
        BLOGNE("OnReleaseFuncWithFence is nullptr, RegisterReleaseListener failed.");
        return GSERROR_INVALID_ARGUMENTS;
    }
    listener_ = new BufferReleaseProducerListener(nullptr, funcWithFence);
    return producer_->RegisterReleaseListener(listener_);
}

GSError ProducerSurface::UnRegisterReleaseListener()
{
    if (producer_ == nullptr) {
        BLOGE("The producer in ProducerSurface is nullptr, UnRegisterReleaseListener failed");
        return GSERROR_INVALID_ARGUMENTS;
    }
    return producer_->UnRegisterReleaseListener();
}

GSError ProducerSurface::RegisterDeleteBufferListener(OnDeleteBufferFunc func, bool isForUniRedraw)
{
    return GSERROR_NOT_SUPPORT;
}

GSError ProducerSurface::RegisterUserDataChangeListener(const std::string &funcName, OnUserDataChangeFunc func)
{
    std::lock_guard<std::mutex> lockGuard(lockMutex_);
    if (onUserDataChange_.find(funcName) != onUserDataChange_.end()) {
        BLOGND("func already register");
        return GSERROR_INVALID_ARGUMENTS;
    }
    
    onUserDataChange_[funcName] = func;
    return GSERROR_OK;
}

GSError ProducerSurface::UnRegisterUserDataChangeListener(const std::string &funcName)
{
    std::lock_guard<std::mutex> lockGuard(lockMutex_);
    if (onUserDataChange_.erase(funcName) == 0) {
        BLOGND("func doesn't register");
        return GSERROR_INVALID_ARGUMENTS;
    }

    return GSERROR_OK;
}

GSError ProducerSurface::ClearUserDataChangeListener()
{
    std::lock_guard<std::mutex> lockGuard(lockMutex_);
    onUserDataChange_.clear();
    return GSERROR_OK;
}

bool ProducerSurface::IsRemote()
{
    return producer_->AsObject()->IsProxyObject();
}

void ProducerSurface::CleanAllLocked()
{
    bufferProducerCache_.clear();
    auto spNativeWindow = wpNativeWindow_.promote();
    if (spNativeWindow != nullptr) {
        auto &bufferCache = spNativeWindow->bufferCache_;
        for (auto &[seqNum, buffer] : bufferCache) {
            NativeObjectUnreference(buffer);
        }
        bufferCache.clear();
    }
}

GSError ProducerSurface::CleanCache()
{
    BLOGND("Queue Id:%{public}" PRIu64, queueId_);
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        CleanAllLocked();
    }
    return producer_->CleanCache();
}

GSError ProducerSurface::GoBackground()
{
    BLOGND("Queue Id:%{public}" PRIu64 "", queueId_);
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        CleanAllLocked();
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
    GraphicTransformType transform = GraphicTransformType::GRAPHIC_ROTATE_BUTT;
    if (producer_->GetTransform(transform) != GSERROR_OK) {
        BLOGNE("Warning ProducerSurface GetTransform failed.");
        return GraphicTransformType::GRAPHIC_ROTATE_BUTT;
    }
    return transform;
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
        CleanAllLocked();
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

GSError ProducerSurface::SetWptrNativeWindowToPSurface(void* nativeWindow)
{
    NativeWindow *nw = reinterpret_cast<NativeWindow *>(nativeWindow);
    wpNativeWindow_ = nw;
    return GSERROR_OK;
}
} // namespace OHOS
