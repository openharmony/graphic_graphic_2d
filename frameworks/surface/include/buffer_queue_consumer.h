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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_BUFFER_QUEUE_CONSUMER_H
#define FRAMEWORKS_SURFACE_INCLUDE_BUFFER_QUEUE_CONSUMER_H

#include <refbase.h>

#include <surface_type.h>
#include "surface_buffer.h"
#include "buffer_queue.h"

namespace OHOS {
class BufferQueueConsumer : public RefBase {
public:
    BufferQueueConsumer(sptr<BufferQueue>& bufferQueue);
    virtual ~BufferQueueConsumer();

    GSError AcquireBuffer(sptr<SurfaceBuffer>& buffer, int32_t &fence,
                               int64_t &timestamp, Rect &damage);

    GSError ReleaseBuffer(sptr<SurfaceBuffer>& buffer, int32_t fence);

    GSError AttachBuffer(sptr<SurfaceBuffer>& buffer);

    GSError DetachBuffer(sptr<SurfaceBuffer>& buffer);

    GSError RegisterConsumerListener(sptr<IBufferConsumerListener>& listener);
    GSError RegisterConsumerListener(IBufferConsumerListenerClazz *listener);
    GSError RegisterReleaseListener(OnReleaseFunc func);
    GSError UnregisterConsumerListener();

    GSError SetDefaultWidthAndHeight(int32_t width, int32_t height);
    GSError SetDefaultUsage(uint32_t usage);
    void Dump(std::string &result) const;
    TransformType GetTransform() const;

private:
    sptr<BufferQueue> bufferQueue_ = nullptr;
    std::string name_ = "not init";
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_BUFFER_QUEUE_CONSUMER_H
