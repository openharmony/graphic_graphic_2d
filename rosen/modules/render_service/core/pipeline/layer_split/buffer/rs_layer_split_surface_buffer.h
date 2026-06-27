/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RS_LAYER_SPLIT_SURFACE_BUFFER_H
#define RS_LAYER_SPLIT_SURFACE_BUFFER_H

#if defined(ROSEN_OHOS)
#include <atomic>
#include <string>

#include "common/rs_common_def.h"
#include "engine/rs_base_render_engine.h"
#include "image/gpu_context.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/layer_split/common/rs_layer_split_types.h"

namespace OHOS::Rosen {

constexpr uint32_t SPLIT_SURFACE_BUFFER_SIZE = 1;

class RSSplitSurfaceRenderListener : public IBufferConsumerListener {
public:
    explicit RSSplitSurfaceRenderListener(
        std::weak_ptr<RSSurfaceHandler> surfaceHandler) : surfaceHandler_(surfaceHandler) {}
    ~RSSplitSurfaceRenderListener() override {};
    void OnBufferAvailable() override
    {
        auto surfaceHandler = surfaceHandler_.lock();
        if (surfaceHandler == nullptr) {
            LAYER_SPLIT_LOGE("%{public}s surfaceHandler is nullptr", __func__);
            return;
        }
        LAYER_SPLIT_LOGD("%{public}s node id:%{public}" PRIu64, __func__, surfaceHandler->GetNodeId());
        surfaceHandler->IncreaseAvailableBuffer();
    }

private:
    std::weak_ptr<RSSurfaceHandler> surfaceHandler_;
};

class RSSplitSurfaceBuffer : public std::enable_shared_from_this<RSSplitSurfaceBuffer> {
public:
    RSSplitSurfaceBuffer(const std::string& name, const int layerId, int32_t width, int32_t height);
    ~RSSplitSurfaceBuffer();

    void Init(bool isHebc);
    void PreAllocateBuffer();
    void PreAllocateBufferImpl(const BufferRequestConfig& bufferConfig);

    std::unique_ptr<RSRenderFrame> RequestFrame(GraphicColorGamut colorSpace);
    bool FlushFrame();
    bool ReleaseBuffer();

    std::shared_ptr<Drawing::GPUContext> GetSkContext() const
    {
        return grContext_;
    }

    void* GetBufferHandle();

    std::shared_ptr<RSSurfaceHandler> GetSurfaceHandler() const
    {
        return surfaceHandler_;
    }

    bool IsSurfaceCreated() const
    {
        return surfaceCreated_;
    }

    std::shared_ptr<RSSurfaceOhos> GetRSSurface()
    {
        return rsSurface_;
    }
private:
    bool CreateSurface(sptr<IBufferConsumerListener> listener);

    std::shared_ptr<RSSurfaceHandler> surfaceHandler_;
    bool surfaceCreated_ = false;
    std::string layerName_;
    std::shared_ptr<Drawing::GPUContext> grContext_;
    sptr<Surface> producerSurface_;
    BufferRequestConfig bufferConfig_ {};
    void* bufferHandle_ = nullptr;
    std::shared_ptr<RSSurfaceOhos> rsSurface_ = nullptr;
    std::atomic<bool> isBufferPreAllocated_ = false;
    std::atomic<bool> isPreAllocInProgress_ = false;
    bool isHebc_ = false;
};
} // OHOS::Rosen

#endif // (ROSEN_OHOS)

#endif