/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BUFFER_THREAD_RS_BUFFER_MANAGER_H
#define RENDER_SERVICE_BUFFER_THREAD_RS_BUFFER_MANAGER_H

#ifndef ROSEN_CROSS_PLATFORM
#include <iconsumer_surface.h>
#include <surface.h>
#include "sync_fence.h"
#endif

#include <list>
#include <set>

#include "rs_layer.h"
#include "pipeline/rs_surface_handler.h"

namespace OHOS {
namespace Rosen {
class RSBufferCollectorHelper {
public:
    RSBufferCollectorHelper() = default;
    ~RSBufferCollectorHelper()
    {
        OnCanvasDrawEnd();
    }
    void OnCanvasDrawBuffer(sptr<IConsumerSurface> consumer, sptr<SurfaceBuffer> buffer,
        std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> bufferOwnerCount);

    void SetAcquireFence(sptr<SyncFence> fence);

private:
    void OnCanvasDrawEnd();

    struct BufferInfo {
        sptr<IConsumerSurface> consumer_ = nullptr;
        sptr<SurfaceBuffer> buffer_ = nullptr;
        std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> bufferOwnerCount_ = nullptr;
    };
    std::list<BufferInfo> pendingReleaseBufferInfos_;

    sptr<SyncFence> fence_ = SyncFence::InvalidFence();
};

// run in uniRenderThread
class RSBufferManager {
public:
    RSBufferManager() = default;
    ~RSBufferManager() = default;

    void AddPendingReleaseBuffer(sptr<IConsumerSurface> consumer, sptr<SurfaceBuffer> buffer, sptr<SyncFence> fence);

    void AddPendingReleaseBuffer(uint64_t bufferId, sptr<SyncFence> fence);

    void OnDrawStart()
    {
        bufferCollector_ = std::make_unique<RSBufferCollectorHelper>();
    }
    void OnDrawBuffer(sptr<IConsumerSurface> consumer, sptr<SurfaceBuffer> buffer,
        std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> bufferOwnerCount)
    {
        if (bufferCollector_ != nullptr) {
            bufferCollector_->OnCanvasDrawBuffer(consumer, buffer, bufferOwnerCount);
        }
    }
    void OnDrawEnd(sptr<SyncFence> canvasAcquireFence, bool needDump = false)
    {
        if (bufferCollector_ != nullptr) {
            bufferCollector_->SetAcquireFence(canvasAcquireFence);
            bufferCollector_.reset();
        }
        if (needDump) {
            std::string output;
            DumpPendingReleaseBuffers(output);
            RS_LOGD("%{public}s", output.c_str());
        }
    }

    void OnReleaseLayerBuffers(std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>>& rsLayers,
        std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>>& releaseBufferFenceVec,
        uint64_t screenId);
    void ReleaseUniOnDrawBuffers(std::shared_ptr<RSSurfaceHandler::BufferOwnerCount>& uniBufferCount,
        sptr<SyncFence>& uniFence, std::set<uint64_t>& decedSet,
        std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>>& rsLayers, uint64_t screenId);
    void ReleaseBufferById(uint64_t bufferId);

#ifndef ROSEN_CROSS_PLATFORM
    void DumpPendingReleaseBuffers(std::string& output);
#endif

private:
    struct PendingReleaseBufferInfo {
        sptr<IConsumerSurface> consumer_;
        wptr<SurfaceBuffer> buffer_ = nullptr;
        std::vector<sptr<SyncFence>> mergedFences_;
    };

    mutable std::mutex screenNodeBufferReleasedMutex_;
    std::map<uint64_t, PendingReleaseBufferInfo> pendingReleaseBuffers_;

    static inline thread_local std::unique_ptr<RSBufferCollectorHelper> bufferCollector_ = nullptr;
};
} // OHOS
} // Rosen
#endif // RENDER_SERVICE_BUFFER_THREAD_RS_BUFFER_MANAGER_H