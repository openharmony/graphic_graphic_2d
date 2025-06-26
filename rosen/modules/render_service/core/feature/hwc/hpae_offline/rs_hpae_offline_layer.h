/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_DRAWABLE_RS_HPAE_OFFLINE_LAYER_H
#define RENDER_SERVICE_DRAWABLE_RS_HPAE_OFFLINE_LAYER_H

#include "common/rs_common_def.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "transaction/rs_render_service_client.h"
#include "params/rs_display_render_params.h"
#include <string>

namespace OHOS::Rosen {
class RSHpaeOfflineLayer {
public:
    RSHpaeOfflineLayer(const std::string& name, NodeId layerId);
    ~RSHpaeOfflineLayer();

    bool CreateSurface(sptr<IBufferConsumerListener>& listener);
    void FlushSurfaceBuffer(sptr<SurfaceBuffer>& buffer, int32_t acquireFence,
        BufferFlushConfig& flushConfig);

    bool PreAllocBuffers(const BufferRequestConfig& config);
    sptr<SurfaceBuffer> RequestSurfaceBuffer(BufferRequestConfig& config, int32_t& releaseFence);
    void CleanCache(bool cleanAll);

    std::shared_ptr<RSSurfaceHandler> GetMutableRSSurfaceHandler() const
    {
        return surfaceHandler_;
    }
private:
    uint32_t bufferSize_ = 5;
    std::shared_ptr<RSSurfaceHandler> surfaceHandler_;
    bool surfaceCreated_ = false;
    std::string layerName_;
    sptr<Surface> surface_ = nullptr;
};

} // OHOS::Rosen

#endif