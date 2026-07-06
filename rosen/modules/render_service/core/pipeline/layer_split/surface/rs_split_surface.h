/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RS_SPLIT_SURFACE_H
#define RS_SPLIT_SURFACE_H

#include "pipeline/layer_split/common/rs_layer_split_types.h"
#include "pipeline/layer_split/buffer/rs_layer_split_surface_buffer.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_processor.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
class RSSurfaceRenderNodeDrawable;
}

class SplitSurface {
public:
    SplitSurface(uint32_t screenWidth, uint32_t screenHeight);

    void Init(std::shared_ptr<RSRenderNode> opIncParentNode_);
    bool CheckParentNodeOnTheTree();
    void UnregisterSplitSurfaceNode();
    bool IsBufferConsumed();
    bool IsBufferReleased();
    bool RequestFrame(RSSurfaceRenderParams& params);
    void SetBufferNull();
    void DrawDfx(Drawing::Color color);
    NodeId GetSurfaceNodeId();
    void FlushFrame();
    void SetColorSpace(GraphicColorGamut colorSpace);
    GraphicColorGamut GetColorSpace() const;

public:
    std::string name_ = "SplitSurface";
    SurfaceStatus surfaceStatus_ = SurfaceStatus::INIT;

    std::shared_ptr<RSSplitSurfaceBuffer> splitSurfaceBuffer_ = nullptr;
    std::shared_ptr<RSSurfaceRenderNode> splitSurfaceNode_ = nullptr;
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> splitSurfaceDrawable_ = nullptr;
    std::shared_ptr<RSPaintFilterCanvas> splitCanvas_ = nullptr;
    std::shared_ptr<RSRenderFrame> splitRenderFrame_ = nullptr;
    GraphicColorGamut colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    uint32_t bufferWidth_;
    uint32_t bufferHeight_;
    bool isDfxDrawed_ = false;
};

} // namespace OHOS::Rosen

#endif // RS_SPLIT_SURFACE_H