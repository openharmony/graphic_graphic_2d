/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RS_TEST_UTIL_H
#define RS_TEST_UTIL_H

#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#include "iconsumer_surface.h"
#include "pipeline/render_thread/rs_base_render_util.h"
#include "pipeline/main_thread/rs_render_service_listener.h"
#include "surface.h"

namespace OHOS {
namespace Rosen {

class RSTestUtil {
public:
    static std::shared_ptr<RSSurfaceRenderNode> CreateSurfaceNode(const RSSurfaceRenderNodeConfig surfaceConfig = {});
    static std::shared_ptr<RSSurfaceRenderNode> CreateSurfaceNodeWithBuffer();
    static std::shared_ptr<RSRcdSurfaceRenderNode> CreateRcdNodeWithBuffer();
    static void InitRenderNodeGC();
private:
    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    static inline BufferFlushConfig flushConfig = {
        .damage = { .w = 0x100, .h = 0x100, },
    };
    static inline sptr<IConsumerSurface> csurf = nullptr;
    static inline sptr<IBufferProducer> producer = nullptr;
    static inline sptr<Surface> psurf = nullptr;
    static NodeId id;
};
} // namespace ROSEN
} // namespace OHOS
#endif // RS_TEST_UTIL_H
