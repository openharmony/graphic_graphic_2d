/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_THREAD_UTIL
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_THREAD_UTIL

#include "pipeline/rs_surface_render_node.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
struct TextureExportBufferDrawParam {
#ifdef ROSEN_OHOS
    sptr<OHOS::SurfaceBuffer> buffer;
#endif
    Drawing::Matrix matrix; // for moving canvas to layer(surface)'s leftTop point.
    Drawing::Rect srcRect; // surface's bufferSize
    Drawing::Rect dstRect; // surface's boundsSize
};

#ifdef ROSEN_OHOS
namespace {
constexpr uint32_t API_VERSION_MOD = 1000;
constexpr uint32_t INVALID_API_COMPATIBLE_VERSION = 0;
} // namespace
#endif

class RSRenderThreadUtil {
public:
    static void SrcRectScaleDown(TextureExportBufferDrawParam& params, const RectF& localBounds);
    static void SrcRectScaleFit(TextureExportBufferDrawParam& params, const RectF& localBounds);
    static GraphicTransformType GetRotateTransform(GraphicTransformType transform);
    static GraphicTransformType GetFlipTransform(GraphicTransformType transform);
    static Drawing::Matrix GetGravityMatrix(Gravity gravity, const RectF& bufferBounds, const RectF& bounds);
    static Drawing::Matrix GetSurfaceTransformMatrix(GraphicTransformType rotationTransform, const RectF &bufferBounds);
    static void FlipMatrix(GraphicTransformType transform, TextureExportBufferDrawParam& params);
    
#ifdef ROSEN_OHOS
    static TextureExportBufferDrawParam CreateTextureExportBufferDrawParams(
        RSSurfaceRenderNode& surfaceNode, GraphicTransformType graphicTransformType, sptr<OHOS::SurfaceBuffer> buffer);
    static void DealWithSurfaceRotationAndGravity(GraphicTransformType transform, Gravity gravity,
        RectF& localBounds, TextureExportBufferDrawParam& params);
#endif
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_THREAD_UTIL