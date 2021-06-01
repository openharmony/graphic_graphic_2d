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

#include <map>
#include <string>

#include <surface.h>
#include <ibuffer_producer.h>

#include "buffer_queue.h"
#include "buffer_queue_consumer.h"
#include "surface_buffer_impl.h"

namespace OHOS {
class ProducerSurface : public Surface {
public:
    ProducerSurface(sptr<IBufferProducer>& producer);
    virtual ~ProducerSurface();
    SurfaceError Init();

    sptr<IBufferProducer> GetProducer() override;
    SurfaceError RequestBuffer(sptr<SurfaceBuffer>& buffer,
                               int32_t& fence, BufferRequestConfig& config) override;

    SurfaceError CancelBuffer(sptr<SurfaceBuffer>& buffer) override;

    SurfaceError FlushBuffer(sptr<SurfaceBuffer>& buffer,
                             int32_t fence, BufferFlushConfig& config) override;

    SurfaceError AcquireBuffer(sptr<SurfaceBuffer>& buffer, int32_t& fence,
                               int64_t& timestamp, Rect& damage) override;
    SurfaceError ReleaseBuffer(sptr<SurfaceBuffer>& buffer, int32_t fence) override;

    uint32_t     GetQueueSize() override;
    SurfaceError SetQueueSize(uint32_t queueSize) override;

    SurfaceError SetDefaultWidthAndHeight(int32_t width, int32_t height) override;
    int32_t GetDefaultWidth() override;
    int32_t GetDefaultHeight() override;

    SurfaceError SetUserData(const std::string& key, const std::string& val) override;
    std::string  GetUserData(const std::string& key) override;

    SurfaceError RegisterConsumerListener(sptr<IBufferConsumerListener>& listener) override;
    SurfaceError UnregisterConsumerListener() override;

    SurfaceError CleanCache() override;

private:
    bool IsRemote();

    std::map<int32_t, sptr<SurfaceBufferImpl>> bufferProducerCache_;
    std::map<std::string, std::string> userData_;
    sptr<IBufferProducer> producer_;
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_PRODUCER_SURFACE_H
