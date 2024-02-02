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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_BUFFER_QUEUE_PRODUCER_H
#define FRAMEWORKS_SURFACE_INCLUDE_BUFFER_QUEUE_PRODUCER_H

#include <vector>
#include <mutex>
#include <refbase.h>
#include <iremote_stub.h>
#include <message_parcel.h>
#include <message_option.h>

#include "surface_type.h"
#include <ibuffer_producer.h>

#include "buffer_queue.h"

namespace OHOS {
class BufferQueueProducer : public IRemoteStub<IBufferProducer> {
public:
    BufferQueueProducer(const sptr<BufferQueue> &bufferQueue);
    virtual ~BufferQueueProducer();

    virtual int OnRemoteRequest(uint32_t code, MessageParcel &arguments,
                                MessageParcel &reply, MessageOption &option) override;

    virtual GSError RequestBuffer(const BufferRequestConfig &config, sptr<BufferExtraData> &bedata,
                                  RequestBufferReturnValue &retval) override;

    GSError CancelBuffer(uint32_t sequence, const sptr<BufferExtraData> &bedata) override;

    GSError FlushBuffer(uint32_t sequence, const sptr<BufferExtraData> &bedata,
                        const sptr<SyncFence>& fence, BufferFlushConfigWithDamages &config) override;

    GSError GetLastFlushedBuffer(sptr<SurfaceBuffer>& buffer, sptr<SyncFence>& fence,
        float matrix[16]) override;

    GSError AttachBuffer(sptr<SurfaceBuffer>& buffer) override;
    GSError AttachBuffer(sptr<SurfaceBuffer>& buffer, int32_t timeOut) override;

    GSError DetachBuffer(sptr<SurfaceBuffer>& buffer) override;

    uint32_t GetQueueSize() override;
    GSError SetQueueSize(uint32_t queueSize) override;

    GSError GetName(std::string &name) override;
    uint64_t GetUniqueId() override;
    GSError GetNameAndUniqueId(std::string& name, uint64_t& uniqueId) override;

    int32_t GetDefaultWidth() override;
    int32_t GetDefaultHeight() override;
    uint32_t GetDefaultUsage() override;

    GSError CleanCache() override;
    GSError GoBackground() override;

    GSError RegisterReleaseListener(sptr<IProducerListener> listener) override;
    GSError UnRegisterReleaseListener() override;

    GSError SetTransform(GraphicTransformType transform) override;
    GSError GetTransform(GraphicTransformType &transform) override;

    GSError IsSupportedAlloc(const std::vector<BufferVerifyAllocInfo> &infos, std::vector<bool> &supporteds) override;

    GSError Disconnect() override;

    GSError SetScalingMode(uint32_t sequence, ScalingMode scalingMode) override;
    GSError SetMetaData(uint32_t sequence, const std::vector<GraphicHDRMetaData> &metaData) override;
    GSError SetMetaDataSet(uint32_t sequence, GraphicHDRMetadataKey key,
                           const std::vector<uint8_t> &metaData) override;
    GSError SetTunnelHandle(const GraphicExtDataHandle *handle) override;
    GSError GetPresentTimestamp(uint32_t sequence, GraphicPresentTimestampType type, int64_t &time) override;

    bool GetStatus() const;
    void SetStatus(bool status);

    sptr<NativeSurface> GetNativeSurface() override;

    GSError SendDeathRecipientObject() override;
    void OnBufferProducerRemoteDied();

private:
    GSError CheckConnectLocked();
    GSError SetTunnelHandle(const sptr<SurfaceTunnelHandle> &handle);

    int32_t RequestBufferRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t CancelBufferRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t FlushBufferRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t AttachBufferRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t DetachBufferRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t GetQueueSizeRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t SetQueueSizeRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t GetNameRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t GetDefaultWidthRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t GetDefaultHeightRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t GetDefaultUsageRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t GetUniqueIdRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t CleanCacheRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t RegisterReleaseListenerRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t UnRegisterReleaseListenerRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t SetTransformRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t IsSupportedAllocRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t GetNameAndUniqueIdRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t DisconnectRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t SetScalingModeRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t SetMetaDataRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t SetMetaDataSetRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t SetTunnelHandleRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t GoBackgroundRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t GetPresentTimestampRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t GetLastFlushedBufferRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t RegisterDeathRecipient(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t GetTransformRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);

    using BufferQueueProducerFunc = int32_t (BufferQueueProducer::*)(MessageParcel &arguments,
        MessageParcel &reply, MessageOption &option);
    std::map<uint32_t, BufferQueueProducerFunc> memberFuncMap_;

    class ProducerSurfaceDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit ProducerSurfaceDeathRecipient(wptr<BufferQueueProducer> producer);
        virtual ~ProducerSurfaceDeathRecipient() = default;

        void OnRemoteDied(const wptr<IRemoteObject>& remoteObject) override;
    private:
        wptr<BufferQueueProducer> producer_;
        std::string name_ = "DeathRecipient";
    };
    sptr<ProducerSurfaceDeathRecipient> producerSurfaceDeathRecipient_ = nullptr;
    sptr<IRemoteObject> token_;

    int32_t connectedPid_ = 0;
    sptr<BufferQueue> bufferQueue_ = nullptr;
    std::string name_ = "not init";
    std::mutex mutex_;
};
}; // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_BUFFER_QUEUE_PRODUCER_H
