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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_BUFFER_QUEUE_H
#define FRAMEWORKS_SURFACE_INCLUDE_BUFFER_QUEUE_H

#include <map>
#include <list>
#include <vector>
#include <mutex>

#include <buffer_producer_listener.h>
#include <ibuffer_consumer_listener.h>
#include <ibuffer_producer.h>
#include "surface_type.h"
#include <surface_tunnel_handle.h>
#include "surface_buffer.h"
#include "consumer_surface_delegator.h"

namespace OHOS {
enum BufferState {
    BUFFER_STATE_RELEASED,
    BUFFER_STATE_REQUESTED,
    BUFFER_STATE_FLUSHED,
    BUFFER_STATE_ACQUIRED,
    BUFFER_STATE_ATTACHED,
};

using BufferElement = struct BufferElement {
    sptr<SurfaceBuffer> buffer;
    BufferState state;
    bool isDeleting;

    BufferRequestConfig config;
    sptr<SyncFence> fence;
    int64_t timestamp;
    std::vector<Rect> damages;
    ScalingMode scalingMode;
    HDRMetaDataType hdrMetaDataType = HDRMetaDataType::HDR_NOT_USED;
    std::vector<GraphicHDRMetaData> metaData;
    GraphicHDRMetadataKey key;
    std::vector<uint8_t> metaDataSet;
    GraphicPresentTimestamp presentTimestamp = {GRAPHIC_DISPLAY_PTS_UNSUPPORTED, 0};
};

class BufferQueue : public RefBase {
public:
    BufferQueue(const std::string &name, bool isShared = false);
    virtual ~BufferQueue();
    GSError Init();

    GSError RequestBuffer(const BufferRequestConfig &config, sptr<BufferExtraData> &bedata,
                          struct IBufferProducer::RequestBufferReturnValue &retval);

    GSError ReuseBuffer(const BufferRequestConfig &config, sptr<BufferExtraData> &bedata,
                        struct IBufferProducer::RequestBufferReturnValue &retval);

    GSError CancelBuffer(uint32_t sequence, const sptr<BufferExtraData> &bedata);

    GSError FlushBuffer(uint32_t sequence, const sptr<BufferExtraData> &bedata,
                        const sptr<SyncFence>& fence, const BufferFlushConfigWithDamages &config);

    GSError DoFlushBuffer(uint32_t sequence, const sptr<BufferExtraData> &bedata,
                          const sptr<SyncFence>& fence, const BufferFlushConfigWithDamages &config);

    GSError GetLastFlushedBuffer(sptr<SurfaceBuffer>& buffer, sptr<SyncFence>& fence,
        float matrix[16]);

    GSError AcquireBuffer(sptr<SurfaceBuffer>& buffer, sptr<SyncFence>& fence,
                          int64_t &timestamp, std::vector<Rect> &damages);
    GSError ReleaseBuffer(sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& fence);

    GSError AttachBuffer(sptr<SurfaceBuffer>& buffer, int32_t timeOut);

    GSError DetachBuffer(sptr<SurfaceBuffer>& buffer);

    GSError RegisterSurfaceDelegator(sptr<IRemoteObject> client, sptr<Surface> cSurface);

    bool QueryIfBufferAvailable();

    uint32_t GetQueueSize();
    GSError SetQueueSize(uint32_t queueSize);

    GSError GetName(std::string &name);

    GSError RegisterConsumerListener(sptr<IBufferConsumerListener>& listener);
    GSError RegisterConsumerListener(IBufferConsumerListenerClazz *listener);
    GSError RegisterReleaseListener(OnReleaseFunc func);
    GSError RegisterProducerReleaseListener(sptr<IProducerListener> listener);
    GSError UnRegisterProducerReleaseListener();
    GSError RegisterDeleteBufferListener(OnDeleteBufferFunc func, bool isForUniRedraw = false);
    GSError UnregisterConsumerListener();

    GSError SetDefaultWidthAndHeight(int32_t width, int32_t height);
    int32_t GetDefaultWidth();
    int32_t GetDefaultHeight();
    GSError SetDefaultUsage(uint64_t usage);
    uint64_t GetDefaultUsage();

    GSError CleanCache();
    GSError GoBackground();
    GSError OnConsumerDied();

    uint64_t GetUniqueId() const;

    void Dump(std::string &result);

    GSError SetTransform(GraphicTransformType transform);
    GraphicTransformType GetTransform() const;

    GSError IsSupportedAlloc(const std::vector<BufferVerifyAllocInfo> &infos,
                             std::vector<bool> &supporteds) const;

    GSError SetScalingMode(uint32_t sequence, ScalingMode scalingMode);
    GSError GetScalingMode(uint32_t sequence, ScalingMode &scalingMode);
    GSError SetMetaData(uint32_t sequence, const std::vector<GraphicHDRMetaData> &metaData);
    GSError SetMetaDataSet(uint32_t sequence, GraphicHDRMetadataKey key,
                           const std::vector<uint8_t> &metaData);
    GSError QueryMetaDataType(uint32_t sequence, HDRMetaDataType &type);
    GSError GetMetaData(uint32_t sequence, std::vector<GraphicHDRMetaData> &metaData);
    GSError GetMetaDataSet(uint32_t sequence, GraphicHDRMetadataKey &key,
                           std::vector<uint8_t> &metaData);
    GSError SetTunnelHandle(const sptr<SurfaceTunnelHandle> &handle);
    sptr<SurfaceTunnelHandle> GetTunnelHandle();
    GSError SetPresentTimestamp(uint32_t sequence, const GraphicPresentTimestamp &timestamp);
    GSError GetPresentTimestamp(uint32_t sequence, GraphicPresentTimestampType type, int64_t &time);

    bool GetStatus() const;
    void SetStatus(bool status);

    GSError SetProducerCacheCleanFlag(bool flag);
    inline void ConsumerRequestCpuAccess(bool on)
    {
        isCpuAccessable_ = on;
    }

private:
    GSError AllocBuffer(sptr<SurfaceBuffer>& buffer, const BufferRequestConfig &config);
    void DeleteBufferInCache(uint32_t sequence);
    void DumpToFile(uint32_t sequence);

    uint32_t GetUsedSize();
    void DeleteBuffersLocked(int32_t count);

    GSError PopFromFreeList(sptr<SurfaceBuffer>& buffer, const BufferRequestConfig &config);
    GSError PopFromDirtyList(sptr<SurfaceBuffer>& buffer);

    GSError CheckRequestConfig(const BufferRequestConfig &config);
    GSError CheckFlushConfig(const BufferFlushConfigWithDamages &config);
    void DumpCache(std::string &result);
    void ClearLocked();
    bool CheckProducerCacheList();
    GSError SetProducerCacheCleanFlagLocked(bool flag);
    GSError AttachBufferUpdateStatus(std::unique_lock<std::mutex> &lock, uint32_t sequence, int32_t timeOut);
    void AttachBufferUpdateBufferInfo(sptr<SurfaceBuffer>& buffer);
    void ListenerBufferReleasedCb(sptr<SurfaceBuffer> &buffer, const sptr<SyncFence> &fence);
    GSError CheckBufferQueueCache(uint32_t sequence);
    GSError ReallocBuffer(const BufferRequestConfig &config, struct IBufferProducer::RequestBufferReturnValue &retval);
    void SetSurfaceBufferHebcMetaLocked(sptr<SurfaceBuffer> buffer);
    GSError RequestBufferCheckStatus();

    int32_t defaultWidth = 0;
    int32_t defaultHeight = 0;
    uint64_t defaultUsage = 0;
    uint32_t queueSize_ = SURFACE_DEFAULT_QUEUE_SIZE;
    GraphicTransformType transform_ = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    GraphicTransformType lastFlushedTransform_ = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    std::string name_;
    std::list<uint32_t> freeList_;
    std::list<uint32_t> dirtyList_;
    std::list<uint32_t> deletingList_;
    std::list<uint32_t> producerCacheList_;
    std::map<uint32_t, BufferElement> bufferQueueCache_;
    sptr<IBufferConsumerListener> listener_ = nullptr;
    IBufferConsumerListenerClazz *listenerClazz_ = nullptr;
    std::mutex mutex_;
    std::mutex listenerMutex_;
    std::mutex producerListenerMutex_;
    const uint64_t uniqueId_;
    OnReleaseFunc onBufferRelease_ = nullptr;
    std::mutex onBufferReleaseMutex_;
    sptr<IProducerListener> producerListener_ = nullptr;
    OnDeleteBufferFunc onBufferDeleteForRSMainThread_;
    OnDeleteBufferFunc onBufferDeleteForRSHardwareThread_;
    bool isShared_ = false;
    std::condition_variable waitReqCon_;
    std::condition_variable waitAttachCon_;
    sptr<SurfaceTunnelHandle> tunnelHandle_ = nullptr;
    std::atomic_bool isValidStatus_ = true;
    std::atomic_bool producerCacheClean_ = false;
    const bool isLocalRender_;
    uint32_t lastFlusedSequence_ = 0;
    sptr<SyncFence> lastFlusedFence_;
    wptr<ConsumerSurfaceDelegator> wpCSurfaceDelegator_;
    bool isCpuAccessable_ = false;
};
}; // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_BUFFER_QUEUE_H
