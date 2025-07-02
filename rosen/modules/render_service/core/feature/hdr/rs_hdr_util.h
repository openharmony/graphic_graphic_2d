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

#ifndef RS_HDR_UTIL_H
#define RS_HDR_UTIL_H

#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS {
namespace Rosen {
class RSScreenManager;

class RSHdrUtil {
public:
    RSHdrUtil() = default;
    ~RSHdrUtil() = default;

    static HdrStatus CheckIsHdrSurface(const RSSurfaceRenderNode& surfaceNode);
    static HdrStatus CheckIsHdrSurfaceBuffer(const sptr<SurfaceBuffer> surfaceBuffer);
    static bool CheckIsSurfaceWithMetadata(const RSSurfaceRenderNode& surfaceNode);
    static bool CheckIsSurfaceBufferWithMetadata(const sptr<SurfaceBuffer> surfaceBuffer);
    static void UpdateSurfaceNodeLayerLinearMatrix(RSSurfaceRenderNode& surfaceNode, ScreenId screenId);
    static void UpdatePixelFormatAfterHwcCalc(RSScreenRenderNode& node);
    static void CheckPixelFormatWithSelfDrawingNode(RSSurfaceRenderNode& surfaceNode, RSScreenRenderNode& displayNode);
    static void UpdateSurfaceNodeNit(RSSurfaceRenderNode& surfaceNode, ScreenId screenId);
    static void SetHDRParam(RSSurfaceRenderNode& node, bool flag);
    static bool GetRGBA1010108Enabled();
    static void HandleVirtualScreenHDRStatus(RSScreenRenderNode& node, const sptr<RSScreenManager>& screenManager);
    std::shared_ptr<Drawing::ShaderEffect> MakeHdrHeadroomShader(float hrRatio,
        std::shared_ptr<Drawing::ShaderEffect> imageShader);
};

} // namespace Rosen
} // namespace OHOS
#endif // RS_HDR_UTIL_H