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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RS_RENDER_SERVICE_UTIL_H
#define RENDER_SERVICE_CORE_PIPELINE_RS_RENDER_SERVICE_UTIL_H

#include <surface.h>
#include "display_type.h"
#include "hdi_backend.h"
#include "hdi_layer_info.h"
#include "common/rs_obj_abs_geometry.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkMatrix.h"
#include "pipeline/rs_surface_render_node.h"
#include "property/rs_transition_properties.h"

namespace OHOS {

namespace Rosen {

struct ComposeInfo {
    IRect srcRect;
    IRect dstRect;
    int32_t zOrder{0};
    LayerAlpha alpha;
    sptr<SurfaceBuffer> buffer;
    int32_t fence;
    sptr<SurfaceBuffer> preBuffer;
    int32_t preFence;
    BlendType blendType;
};

class RsRenderServiceUtil {
public:
    static void ComposeSurface(std::shared_ptr<HdiLayerInfo> layer, sptr<Surface> consumerSurface,
        std::vector<LayerInfoPtr>& layers, ComposeInfo info, RSSurfaceRenderNode* node = nullptr);
    static void DrawBuffer(SkCanvas* canvas, sptr<OHOS::SurfaceBuffer> buffer, RSSurfaceRenderNode& node,
        bool isDrawnOnDisplay = true);
private:
    static void DealAnimation(SkCanvas* canvas, SkPaint& paint, const RSProperties& property,
        const std::unique_ptr<RSTransitionProperties>& transitionProperties);
    static bool IsNeedClient(RSSurfaceRenderNode* node);
};

} // Rosen
} // OHOS
#endif