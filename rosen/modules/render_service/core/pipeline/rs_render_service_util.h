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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RS_RENDER_SERVICE_UTIL_H
#define RENDER_SERVICE_CORE_PIPELINE_RS_RENDER_SERVICE_UTIL_H

#include <surface.h>
#include "display_type.h"
#include "hdi_backend.h"
#include "hdi_layer_info.h"
#include "common/rs_obj_abs_geometry.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkRect.h"
#include "pipeline/rs_surface_render_node.h"
#include "property/rs_transition_properties.h"
#include "screen_manager/screen_types.h"
#include "sync_fence.h"

namespace OHOS {

namespace Rosen {
struct BufferDrawParam {
    sptr<OHOS::SurfaceBuffer> buffer;
    SkMatrix matrix;
    SkRect srcRect;
    SkRect dstRect;
    SkRect clipRect;
    SkPaint paint;
    ColorGamut targetColorGamut = ColorGamut::COLOR_GAMUT_SRGB;
};

struct AnimationInfo {
    Vector2f pivot = { 0.0f, 0.0f };
    Vector3f scale = { 1.0f, 1.0f, 1.0f };
    Vector3f translate = { 0.0f, 0.0f, 0.0f };
    float alpha = 1.0f;
    SkMatrix44 rotateMatrix = SkMatrix44::I();
};

struct ComposeInfo {
    IRect srcRect;
    IRect dstRect;
    IRect visibleRect;
    int32_t zOrder{0};
    LayerAlpha alpha;
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> fence = SyncFence::INVALID_FENCE;
    sptr<SurfaceBuffer> preBuffer;
    sptr<SyncFence> preFence = SyncFence::INVALID_FENCE;
    BlendType blendType;
};

class RsRenderServiceUtil {
public:
    using CanvasPostProcess = std::function<void(SkCanvas&, BufferDrawParam&)>;
    static void ComposeSurface(std::shared_ptr<HdiLayerInfo> layer, sptr<Surface> consumerSurface,
        std::vector<LayerInfoPtr>& layers, ComposeInfo info, RSSurfaceRenderNode* node = nullptr);
    static void DrawBuffer(SkCanvas& canvas, BufferDrawParam& bufferDrawParam, CanvasPostProcess process = nullptr);
    static BufferDrawParam CreateBufferDrawParam(RSSurfaceRenderNode& node, SkMatrix canvasMatrix = SkMatrix(),
        ScreenRotation rotation = ScreenRotation::ROTATION_0);
    static void DealAnimation(SkCanvas& canvas, RSSurfaceRenderNode& node, BufferDrawParam& params);
    static void ExtractAnimationInfo(const std::unique_ptr<RSTransitionProperties>& transitionProperties,
        RSSurfaceRenderNode& node, AnimationInfo& info);
    static void InitEnableClient();
private:
    static SkMatrix GetCanvasTransform(const RSSurfaceRenderNode& node, const SkMatrix& canvasMatrix,
        ScreenRotation rotation);
    static bool IsNeedClient(RSSurfaceRenderNode* node);
    static bool CreateBitmap(sptr<OHOS::SurfaceBuffer> buffer, SkBitmap& bitmap);
    static bool CreateYuvToRGBABitMap(sptr<OHOS::SurfaceBuffer> buffer, std::vector<uint8_t>& newBuffer,
        SkBitmap& bitmap);
    static bool CreateNewColorGamutBitmap(sptr<OHOS::SurfaceBuffer> buffer, std::vector<uint8_t>& newGamutBuffer,
        SkBitmap& bitmap, ColorGamut srcGamut, ColorGamut dstGamut);
    static bool enableClient;
};
} // Rosen
} // OHOS
#endif
