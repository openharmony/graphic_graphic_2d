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

#include <ibuffer_consumer_listener.h>
#include <ibuffer_producer.h>
#include <surface_type.h>
#include <buffer_manager.h>

#include "surface_buffer.h"

namespace OHOS {
enum BufferState {
    BUFFER_STATE_RELEASED,
    BUFFER_STATE_REQUESTED,
    BUFFER_STATE_FLUSHED,
    BUFFER_STATE_ACQUIRED,
    BUFFER_STATE_ATTACHED,
};

typedef struct {
    sptr<SurfaceBuffer> buffer;
    BufferState state;
    bool isDeleting;

    BufferRequestConfig config;
    sptr<SyncFence> fence;
    int64_t timestamp;
    Rect damage;
} BufferElement;

class BufferQueue : public RefBase {
public:
    BufferQueue(const std::string &name, bool isShared = false);
    virtual ~BufferQueue();
    GSError Init();

    GSError RequestBuffer(const BufferRequestConfig &config, sptr<BufferExtraData> &bedata,
                               struct IBufferProducer::RequestBufferReturnValue &retval);

    GSError ReuseBuffer(const BufferRequestConfig &config, sptr<BufferExtraData> &bedata,
                             struct IBufferProducer::RequestBufferReturnValue &retval);

    GSError CancelBuffer(int32_t sequence, const sptr<BufferExtraData> &bedata);

    GSError FlushBuffer(int32_t sequence, const sptr<BufferExtraData> &bedata,
                             const sptr<SyncFence>& fence, const BufferFlushConfig &config);

    GSError DoFlushBuffer(int32_t sequence, const sptr<BufferExtraData> &bedata,
                               const sptr<SyncFence>& fence, const BufferFlushConfig &config);

    GSError AcquireBuffer(sptr<SurfaceBuffer>& buffer, sptr<SyncFence>& fence,
                               int64_t &timestamp, Rect &damage);
    GSError ReleaseBuffer(sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& fence);

    GSError AttachBuffer(sptr<SurfaceBuffer>& buffer);

    GSError DetachBuffer(sptr<SurfaceBuffer>& buffer);

    uint32_t GetQueueSize();
    GSError SetQueueSize(uint32_t queueSize);

    GSError GetName(std::string &name);

    GSError RegisterConsumerListener(sptr<IBufferConsumerListener>& listener);
    GSError RegisterConsumerListener(IBufferConsumerListenerClazz *listener);
    GSError RegisterReleaseListener(OnReleaseFunc func);
    GSError RegisterDeleteBufferListener(OnDeleteBufferFunc func);
    GSError UnregisterConsumerListener();

    GSError SetDefaultWidthAndHeight(int32_t width, int32_t height);
    int32_t GetDefaultWidth();
    int32_t GetDefaultHeight();
    GSError SetDefaultUsage(uint32_t usage);
    uint32_t GetDefaultUsage();

    GSError CleanCache();

    uint64_t GetUniqueId() const;

    void Dump(std::string &result);

    GSError SetTransform(TransformType transform);
    TransformType GetTransform() const;

    GSError IsSupportedAlloc(const std::vector<VerifyAllocInfo> &infos,
                             std::vector<bool> &supporteds) const;

private:
    GSError AllocBuffer(sptr<SurfaceBuffer>& buffer, const BufferRequestConfig &config);
    void DeleteBufferInCache(int sequence);
    void DumpToFile(int32_t sequence);

    uint32_t GetUsedSize();
    void DeleteBuffers(int32_t count);

    GSError PopFromFreeList(sptr<SurfaceBuffer>& buffer, const BufferRequestConfig &config);
    GSError PopFromDirtyList(sptr<SurfaceBuffer>& buffer);

    GSError CheckRequestConfig(const BufferRequestConfig &config);
    GSError CheckFlushConfig(const BufferFlushConfig &config);
    void DumpCache(std::string &result);

    int32_t defaultWidth = 0;
    int32_t defaultHeight = 0;
    uint32_t defaultUsage = 0;
    uint32_t queueSize_ = SURFACE_DEFAULT_QUEUE_SIZE;
    TransformType transform_ = TransformType::ROTATE_NONE;
    std::string name_;
    std::list<int32_t> freeList_;
    std::list<int32_t> dirtyList_;
    std::list<int32_t> deletingList_;
    std::map<int32_t, BufferElement> bufferQueueCache_;
    sptr<IBufferConsumerListener> listener_ = nullptr;
    IBufferConsumerListenerClazz *listenerClazz_ = nullptr;
    std::mutex mutex_;
    const uint64_t uniqueId_;
    sptr<BufferManager> bufferManager_ = nullptr;
    OnReleaseFunc onBufferRelease = nullptr;
    OnDeleteBufferFunc onBufferDelete_ = nullptr;
    bool isShared_ = false;
    std::condition_variable waitReqCon_;
};
}; // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_BUFFER_QUEUE_H
