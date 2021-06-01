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

#include <surface_type.h>
#include <ibuffer_consumer_listener.h>

#include "surface_buffer_impl.h"

namespace OHOS {
enum BufferState {
    BUFFER_STATE_RELEASED,
    BUFFER_STATE_REQUESTED,
    BUFFER_STATE_FLUSHED,
    BUFFER_STATE_ACQUIRED,
};

typedef struct {
    sptr<SurfaceBufferImpl> buffer;
    BufferState state;
    bool isDeleting;

    BufferRequestConfig config;
    int32_t fence;
    int64_t timestamp;
    Rect damage;
} BufferElement;

class BufferQueue : public RefBase {
public:
    BufferQueue();
    virtual ~BufferQueue();
    SurfaceError Init();

    SurfaceError RequestBuffer(int32_t& sequence, sptr<SurfaceBufferImpl>& buffer,
                               int32_t& fence, BufferRequestConfig& config,
                               std::vector<int32_t>& deletingBuffers);

    SurfaceError CancelBuffer(int32_t sequence);

    SurfaceError FlushBuffer(int32_t sequence, int32_t fence, BufferFlushConfig& config);

    SurfaceError AcquireBuffer(sptr<SurfaceBufferImpl>& buffer, int32_t& fence,
                               int64_t& timestamp, Rect& damage);
    SurfaceError ReleaseBuffer(sptr<SurfaceBufferImpl>& buffer, int32_t fence);

    uint32_t GetQueueSize();
    SurfaceError SetQueueSize(uint32_t queueSize);

    SurfaceError RegisterConsumerListener(sptr<IBufferConsumerListener>& listener);
    SurfaceError UnregisterConsumerListener();

    SurfaceError SetDefaultWidthAndHeight(int32_t width, int32_t height);
    int32_t GetDefaultWidth();
    int32_t GetDefaultHeight();

    SurfaceError CleanCache();

private:
    SurfaceError AllocBuffer(sptr<SurfaceBufferImpl>& buffer, BufferRequestConfig& config);
    SurfaceError FreeBuffer(sptr<SurfaceBufferImpl>& buffer);
    void DeleteBufferInCache(int sequence);

    uint32_t GetUsedSize();
    void DeleteBuffers(int32_t count);

    SurfaceError PopFromFreeList(sptr<SurfaceBufferImpl>& buffer, BufferRequestConfig& config);
    SurfaceError PopFromDirtyList(sptr<SurfaceBufferImpl>& buffer);

    SurfaceError CheckRequestConfig(BufferRequestConfig& config);
    SurfaceError CheckFlushConfig(BufferFlushConfig& config);

    int32_t defaultWidth = 0;
    int32_t defaultHeight = 0;
    uint32_t queueSize_;
    std::list<int32_t> freeList_;
    std::list<int32_t> dirtyList_;
    std::list<int32_t> deletingList_;
    std::map<int32_t, BufferElement> bufferQueueCache_;
    sptr<IBufferConsumerListener> listener_;
    std::mutex mutex_;
};
}; // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_BUFFER_QUEUE_H
