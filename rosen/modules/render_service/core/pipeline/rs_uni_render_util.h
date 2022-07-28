/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_UTIL_H
#define RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_UTIL_H

#include "display_type.h"
#include "surface.h"
#include "sync_fence.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "common/rs_obj_abs_geometry.h"

namespace OHOS {
namespace Rosen {
struct BufferInfo {
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> acquireFence;
    sptr<Surface> consumerSurface;
};

class RSUniRenderUtil {
public:
    static void DrawBufferOnCanvas(sptr<SurfaceBuffer> buffer, const ColorGamut& dstGamut, RSPaintFilterCanvas& canvas,
        SkRect srcRect, SkRect dstRect);
#ifdef RS_ENABLE_EGLIMAGE
    static void DrawImageOnCanvas(BufferInfo& bufferInfo, RSPaintFilterCanvas& canvas, SkRect srcRect, SkRect dstRect);
#endif
    static void UpdateRenderNodeDstRect(RSRenderNode& node);
    static Occlusion::Region MergeVisibleDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node, int32_t bufferAge);
};
}
}
#endif // RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_UTIL_H
