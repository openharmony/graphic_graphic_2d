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

#ifndef RS_HETERO_HDR_BUFFER_LAYER
#define RS_HETERO_HDR_BUFFER_LAYER

#include <string>

#include "common/rs_common_def.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/rs_surface_handler.h"
#include "transaction/rs_render_service_client.h"

namespace OHOS {
namespace Rosen {
class RSHeteroHDRBufferLayer {
public:
    RSHeteroHDRBufferLayer(const std::string& name, NodeId layerId);
    ~RSHeteroHDRBufferLayer();
    void FlushHDRDstBuffer(sptr<SurfaceBuffer>& dstBuffer);

    std::shared_ptr<RSSurfaceHandler> GetMutableRSSurfaceHandler()
    {
        std::lock_guard<std::mutex> lockGuard(surfaceHandleMutex_);
        return surfaceHandler_;
    }
    void ConsumeAndUpdateBuffer();
    sptr<SurfaceBuffer> PrepareHDRDstBuffer(RSSurfaceRenderParams* surfaceParams, ScreenId screenId);
    void CleanCache();
    bool ReleaseBuffer();
private:
    void FlushSurfaceBuffer(sptr<SurfaceBuffer>& buffer, int32_t acquireFence, BufferFlushConfig& flushConfig);
    sptr<SurfaceBuffer> RequestSurfaceBuffer(BufferRequestConfig& config, int32_t& releaseFence);
    bool CreateSurface(sptr<IBufferConsumerListener>& listener);
    std::shared_ptr<RSSurfaceHandler> surfaceHandler_ = nullptr;
    bool surfaceCreated_ = false;
    std::string layerName_;
    sptr<Surface> surface_ = nullptr;
    std::mutex surfaceHandleMutex_;
    sptr<SurfaceBuffer> bufferToRelease_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_HETERO_HDR_BUFFER_LAYER