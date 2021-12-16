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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_CONSUMER_SURFACE_H
#define FRAMEWORKS_SURFACE_INCLUDE_CONSUMER_SURFACE_H

#include <map>
#include <string>

#include <surface.h>

#include "buffer_queue.h"
#include "buffer_queue_producer.h"
#include "buffer_queue_consumer.h"

namespace OHOS {
class ConsumerSurface : public Surface {
public:
    ConsumerSurface(const std::string &name, bool isShared = false);
    virtual ~ConsumerSurface();
    GSError Init();

    bool IsConsumer() const override;
    sptr<IBufferProducer> GetProducer() const override;
    GSError RequestBuffer(sptr<SurfaceBuffer>& buffer,
        int32_t &fence, BufferRequestConfig &config) override;

    GSError RequestBufferNoFence(sptr<SurfaceBuffer>& buffer,
        BufferRequestConfig &config) override;

    GSError RequestBufferWithFence(sptr<SurfaceBuffer>& buffer,
        int32_t &fence, BufferRequestConfig &config) override;

    GSError CancelBuffer(sptr<SurfaceBuffer>& buffer) override;

    GSError FlushBuffer(sptr<SurfaceBuffer>& buffer,
        int32_t fence, BufferFlushConfig &config) override;

    GSError FlushBufferNoFence(sptr<SurfaceBuffer>& buffer, BufferFlushConfig &config) override;

    GSError AcquireBuffer(sptr<SurfaceBuffer>& buffer, int32_t &fence,
        int64_t &timestamp, Rect &damage) override;
    GSError ReleaseBuffer(sptr<SurfaceBuffer>& buffer, int32_t fence) override;

    GSError AttachBuffer(sptr<SurfaceBuffer>& buffer) override;
    GSError DetachBuffer(sptr<SurfaceBuffer>& buffer) override;

    uint32_t GetQueueSize() override;
    GSError SetQueueSize(uint32_t queueSize) override;

    GSError GetName(std::string &name) override;

    GSError SetDefaultWidthAndHeight(int32_t width, int32_t height) override;
    int32_t GetDefaultWidth() override;
    int32_t GetDefaultHeight() override;
    GSError SetDefaultUsage(uint32_t usage) override;
    uint32_t GetDefaultUsage() override;

    GSError SetUserData(const std::string &key, const std::string &val) override;
    std::string GetUserData(const std::string &key) override;

    GSError RegisterConsumerListener(sptr<IBufferConsumerListener>& listener) override;
    GSError RegisterConsumerListener(IBufferConsumerListenerClazz *listener) override;
    GSError RegisterReleaseListener(OnReleaseFunc func) override;
    GSError UnregisterConsumerListener() override;

    GSError CleanCache() override;

private:
    std::map<std::string, std::string> userData_;
    sptr<BufferQueueProducer> producer_ = nullptr;
    sptr<BufferQueueConsumer> consumer_ = nullptr;
    std::string name_ = "not init";
    bool isShared_ = false;
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_CONSUMER_SURFACE_H
