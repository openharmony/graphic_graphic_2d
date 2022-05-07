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

#ifndef INTERFACES_INNERKITS_SURFACE_SURFACE_H
#define INTERFACES_INNERKITS_SURFACE_SURFACE_H

#include <refbase.h>

#include "ibuffer_consumer_listener.h"
#include "ibuffer_producer.h"
#include "surface_buffer.h"
#include "surface_type.h"

namespace OHOS {
class Surface : public RefBase {
public:
    static sptr<Surface> CreateSurfaceAsConsumer(std::string name = "noname", bool isShared = false);
    static sptr<Surface> CreateSurfaceAsProducer(sptr<IBufferProducer>& producer);
    static sptr<Surface> CreateEglSurfaceAsConsumer(std::string name = "noname", bool isShared = false);

    virtual ~Surface() = default;

    virtual bool IsConsumer() const = 0;
    virtual sptr<IBufferProducer> GetProducer() const = 0;

    virtual GSError RequestBuffer(sptr<SurfaceBuffer>& buffer,
                                       int32_t &fence, BufferRequestConfig &config) = 0;

    virtual GSError CancelBuffer(sptr<SurfaceBuffer>& buffer) = 0;

    virtual GSError FlushBuffer(sptr<SurfaceBuffer>& buffer,
                                     int32_t fence, BufferFlushConfig &config) = 0;

    virtual GSError AcquireBuffer(sptr<SurfaceBuffer>& buffer, int32_t &fence,
                                       int64_t &timestamp, Rect &damage) = 0;
    virtual GSError ReleaseBuffer(sptr<SurfaceBuffer>& buffer, int32_t fence) = 0;

    virtual GSError RequestBuffer(sptr<SurfaceBuffer>& buffer,
                                       sptr<SyncFence>& fence, BufferRequestConfig &config) = 0;
    virtual GSError FlushBuffer(sptr<SurfaceBuffer>& buffer,
                                     const sptr<SyncFence>& fence, BufferFlushConfig &config) = 0;
    virtual GSError AcquireBuffer(sptr<SurfaceBuffer>& buffer, sptr<SyncFence>& fence,
                                       int64_t &timestamp, Rect &damage) = 0;
    virtual GSError ReleaseBuffer(sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& fence) = 0;

    virtual GSError AttachBuffer(sptr<SurfaceBuffer>& buffer) = 0;

    virtual GSError DetachBuffer(sptr<SurfaceBuffer>& buffer) = 0;

    virtual uint32_t GetQueueSize() = 0;
    virtual GSError SetQueueSize(uint32_t queueSize) = 0;

    virtual GSError SetDefaultWidthAndHeight(int32_t width, int32_t height) = 0;
    virtual int32_t GetDefaultWidth() = 0;
    virtual int32_t GetDefaultHeight() = 0;

    virtual GSError SetDefaultUsage(uint32_t usage) = 0;
    virtual uint32_t GetDefaultUsage() = 0;

    virtual GSError SetUserData(const std::string &key, const std::string &val) = 0;
    virtual std::string GetUserData(const std::string &key) = 0;

    virtual const std::string& GetName() = 0;
    virtual uint64_t GetUniqueId() const = 0;

    virtual GSError RegisterConsumerListener(sptr<IBufferConsumerListener>& listener) = 0;
    virtual GSError RegisterConsumerListener(IBufferConsumerListenerClazz *listener) = 0;
    virtual GSError RegisterReleaseListener(OnReleaseFunc func) = 0;
    virtual GSError RegisterDeleteBufferListener(OnDeleteBufferFunc func) = 0;
    virtual GSError UnregisterConsumerListener() = 0;

    // Call carefully. This interface will empty all caches of the current process
    virtual GSError CleanCache() = 0;

    virtual GSError SetTransform(TransformType transform) = 0;
    virtual TransformType GetTransform() const = 0;

    virtual GSError IsSupportedAlloc(const std::vector<VerifyAllocInfo> &infos,
                                     std::vector<bool> &supporteds) = 0;
    
    virtual void Dump(std::string &result) const = 0;
protected:
    Surface() = default;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_SURFACE_SURFACE_H
