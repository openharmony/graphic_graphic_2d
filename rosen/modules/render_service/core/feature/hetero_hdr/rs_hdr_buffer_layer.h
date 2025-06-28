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
 
#ifndef RENDER_SERVICE_HDR_BUFFER_LAYER_H
#define RENDER_SERVICE_HDR_BUFFER_LAYER_H
 
#include "common/rs_common_def.h"
#include "params/rs_display_render_params.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/rs_surface_handler.h"
#include "transaction/rs_render_service_client.h"
#include <string>

namespace OHOS::Rosen {
namespace DrawableV2 {
class RSHDRBUfferLayer {
public:
    RSHDRBUfferLayer(const std::string& name, NodeId layerId);
    ~RSHDRBUfferLayer();
    void FlushHDRDstBuffer(sptr<SurfaceBuffer> dstBuffer);
    
    std::shared_ptr<RSSurfaceHandler> GetMutableRSSurfaceHandler() const
    {
        return surfaceHandler_;
    }
    void ConsumeAndUpdateBuffer();
    sptr<SurfaceBuffer> PrepareHDRDstBuffer(RSSurfaceRenderParams *surfaceParams);
 
private:
    void FlushSurfaceBuffer(sptr<SurfaceBuffer> &buffer, int32_t acquireFence,
        BufferFlushConfig &flushConfig);
    sptr<SurfaceBuffer> RequestSurfaceBuffer(BufferRequestConfig& config, int32_t &releaseFence);
    bool CreateSurface(sptr<IBufferConsumerListener> listener);
    static constexpr uint32_t BUFFER_SIZE = 4;
    std::shared_ptr<RSSurfaceHandler> surfaceHandler_;
    bool surfaceCreated_ = false;
    std::string layerName_;
    sptr<Surface> surface_ = nullptr;
};
}
} // OHOS::Rosen

#endif
