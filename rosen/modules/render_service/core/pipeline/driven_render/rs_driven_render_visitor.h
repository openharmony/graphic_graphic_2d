/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CORE_RS_DRIVEN_RENDER_VISITOR_H
#define RENDER_SERVICE_CORE_RS_DRIVEN_RENDER_VISITOR_H

#include "pipeline/rs_processor.h"
#include "rs_driven_surface_render_node.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;

class RSDrivenRenderVisitor : public RSNodeVisitor {
public:
    RSDrivenRenderVisitor();
    ~RSDrivenRenderVisitor() override {}

    void PrepareBaseRenderNode(RSBaseRenderNode& node) override;
    void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override;
    void PrepareDisplayRenderNode(RSDisplayRenderNode& node) override {}
    void PrepareProxyRenderNode(RSProxyRenderNode& node) override {}
    void PrepareRootRenderNode(RSRootRenderNode& node) override {}
    void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override {}

    void ProcessBaseRenderNode(RSBaseRenderNode& node) override;
    void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override;
    void ProcessDisplayRenderNode(RSDisplayRenderNode& node) override {}
    void ProcessProxyRenderNode(RSProxyRenderNode& node) override {}
    void ProcessRootRenderNode(RSRootRenderNode& node) override {}
    void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override {}

    void PrepareDrivenSurfaceRenderNode(RSDrivenSurfaceRenderNode& node);
    void ProcessDrivenSurfaceRenderNode(RSDrivenSurfaceRenderNode& node);

    void SetDirtyInfo(bool backgroundDirty, bool contentDirty, bool nonContentDirty);
    void SetScreenRect(const RectI& rect);

    void SetUniProcessor(std::shared_ptr<RSProcessor> processor);
    void SetUniColorSpace(ColorGamut colorSpace);
    void SetUniGlobalZOrder(float globalZOrder);

private:
    void RenderExpandedFrame(RSDrivenSurfaceRenderNode& node);
    void SkipRenderExpandedFrame(RSDrivenSurfaceRenderNode& node);
    void ProcessDrivenCanvasRenderNode(RSCanvasRenderNode& node);
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
    std::shared_ptr<RSBaseRenderEngine> renderEngine_;

    std::shared_ptr<RSDrivenSurfaceRenderNode> currDrivenSurfaceNode_ = nullptr;

    bool backgroundDirty_ = false;
    bool contentDirty_ = false;
    bool nonContentDirty_ = false;
    RectI screenRect_;

    std::shared_ptr<RSProcessor> uniProcessor_ = nullptr;
    ColorGamut uniColorSpace_ = ColorGamut::COLOR_GAMUT_SRGB;
    float uniGlobalZOrder_ = 0.0f;

    bool hasTraverseDrivenNode_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_RS_DRIVEN_RENDER_VISITOR_H