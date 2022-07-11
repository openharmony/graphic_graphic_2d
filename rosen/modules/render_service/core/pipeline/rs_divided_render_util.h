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
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkRect.h"

#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "rs_composer_adapter.h"
#include "screen_manager/screen_types.h"
#include "sync_fence.h"
#ifdef RS_ENABLE_EGLIMAGE
#include "rs_egl_image_manager.h"
#endif // RS_ENABLE_EGLIMAGE

namespace OHOS {

namespace Rosen {
struct BufferDrawParam {
    sptr<OHOS::SurfaceBuffer> buffer;
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    SkMatrix matrix;
    SkRect srcRect;
    SkRect dstRect;
    SkRect clipRect;
    Vector4f cornerRadius;
    bool isNeedClip = true;
    SkPaint paint;
    SkColor backgroundColor = SK_ColorTRANSPARENT;
    ColorGamut targetColorGamut = ColorGamut::COLOR_GAMUT_SRGB;
};

class RSDividedRenderUtil {
public:
    using CanvasPostProcess = std::function<void(RSPaintFilterCanvas&, BufferDrawParam&)>;
    static void DrawBuffer(RSPaintFilterCanvas& canvas, BufferDrawParam& bufferDrawParam,
        CanvasPostProcess process = nullptr);
    // YUV Layer clip hole
    static void ClipHole(RSPaintFilterCanvas& canvas, const BufferDrawParam& bufferDrawParam);

#ifdef RS_ENABLE_EGLIMAGE
    static void DrawImage(std::shared_ptr<RSEglImageManager> eglImageManager, GrContext* grContext,
        RSPaintFilterCanvas& canvas, BufferDrawParam& bufferDrawParam, CanvasPostProcess process = nullptr);
#endif // RS_ENABLE_EGLIMAGE

    static BufferDrawParam CreateBufferDrawParam(RSSurfaceRenderNode& node, SkMatrix canvasMatrix = SkMatrix(),
        ScreenRotation rotation = ScreenRotation::ROTATION_0);
    static void DealAnimation(RSPaintFilterCanvas& canvas, RSSurfaceRenderNode& node, BufferDrawParam& params,
        const Vector2f& center);
    static void InitEnableClient();
    static bool IsNeedClient(RSSurfaceRenderNode& node, const ComposeInfo& info);

private:
    static SkMatrix GetCanvasTransform(const RSSurfaceRenderNode& node, const SkMatrix& canvasMatrix,
        ScreenRotation rotation, SkRect clipRect);
    static bool enableClient;
};
} // Rosen
} // OHOS
#endif
