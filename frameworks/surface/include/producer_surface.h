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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_PRODUCER_SURFACE_H
#define FRAMEWORKS_SURFACE_INCLUDE_PRODUCER_SURFACE_H

#include <atomic>
#include <map>
#include <string>

#include <surface.h>
#include <ibuffer_producer.h>

#include "buffer_queue.h"
#include "buffer_queue_consumer.h"
#include "surface_buffer.h"

namespace OHOS {
class ProducerSurface : public Surface {
public:
    ProducerSurface(sptr<IBufferProducer>& producer);
    virtual ~ProducerSurface();

    // thread unsafe
    GSError Init();

    bool IsConsumer() const override;
    sptr<IBufferProducer> GetProducer() const override;
    GSError RequestBuffer(sptr<SurfaceBuffer>& buffer,
                          int32_t &fence, BufferRequestConfig &config) override;

    GSError CancelBuffer(sptr<SurfaceBuffer>& buffer) override;

    GSError FlushBuffer(sptr<SurfaceBuffer>& buffer,
                        int32_t fence, BufferFlushConfig &config) override;

    GSError RequestBuffer(sptr<SurfaceBuffer>& buffer,
                          sptr<SyncFence>& fence, BufferRequestConfig &config) override;
    GSError FlushBuffer(sptr<SurfaceBuffer>& buffer,
                        const sptr<SyncFence>& fence, BufferFlushConfig &config) override;

    GSError AttachBuffer(sptr<SurfaceBuffer>& buffer) override;

    GSError DetachBuffer(sptr<SurfaceBuffer>& buffer) override;

    bool QueryIfBufferAvailable() override;

    uint32_t GetQueueSize() override;
    GSError SetQueueSize(uint32_t queueSize) override;

    const std::string& GetName() override;
    uint64_t GetUniqueId() const override;

    int32_t GetDefaultWidth() override;
    int32_t GetDefaultHeight() override;

    uint32_t GetDefaultUsage() override;

    GSError SetUserData(const std::string &key, const std::string &val) override;
    std::string GetUserData(const std::string &key) override;

    GSError RegisterReleaseListener(OnReleaseFunc func) override;

    void Dump(std::string &result) const override {};

    // Call carefully. This interface will empty all caches of the current process
    GSError CleanCache() override;
    GSError GoBackground() override;

    GSError SetTransform(GraphicTransformType transform) override;

    GSError IsSupportedAlloc(const std::vector<BufferVerifyAllocInfo> &infos, std::vector<bool> &supporteds) override;
    GSError Disconnect() override;
    GSError SetScalingMode(uint32_t sequence, ScalingMode scalingMode) override;

    GSError SetMetaData(uint32_t sequence, const std::vector<GraphicHDRMetaData> &metaData) override;
    GSError SetMetaDataSet(uint32_t sequence, GraphicHDRMetadataKey key, const std::vector<uint8_t> &metaData) override;

    GSError SetTunnelHandle(const OHExtDataHandle *handle) override;

    GSError GetPresentTimestamp(uint32_t sequence, GraphicPresentTimestampType type, int64_t &time) const override;

private:
    bool IsRemote();

    std::mutex mutex_;
    std::atomic_bool inited_ = false;
    std::map<int32_t, sptr<SurfaceBuffer>> bufferProducerCache_;
    std::map<std::string, std::string> userData_;
    sptr<IBufferProducer> producer_ = nullptr;
    std::string name_ = "not init";
    uint64_t queueId_ = 0;
    bool isDisconnected = true;
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_PRODUCER_SURFACE_H
