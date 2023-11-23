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

#ifndef RENDER_SERVICE_CORE_RS_RCD_RENDER_VISITOR_H
#define RENDER_SERVICE_CORE_RS_RCD_RENDER_VISITOR_H

#include "pipeline/rs_processor.h"
#include "rs_rcd_surface_render_node.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;

class RSRcdRenderVisitor : public RSNodeVisitor {
public:
    RSRcdRenderVisitor();
    ~RSRcdRenderVisitor() override {}

    void PrepareChildren(RSRenderNode& node) override {}
    void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override {}
    void PrepareDisplayRenderNode(RSDisplayRenderNode& node) override {}
    void PrepareProxyRenderNode(RSProxyRenderNode& node) override {}
    void PrepareRootRenderNode(RSRootRenderNode& node) override {}
    void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override {}
    void PrepareEffectRenderNode(RSEffectRenderNode& node) override {}

    void ProcessChildren(RSRenderNode& node) override {}
    void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override {}
    void ProcessDisplayRenderNode(RSDisplayRenderNode& node) override {}
    void ProcessProxyRenderNode(RSProxyRenderNode& node) override {}
    void ProcessRootRenderNode(RSRootRenderNode& node) override {}
    void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override {}
    void ProcessEffectRenderNode(RSEffectRenderNode& node) override {}

    bool ConsumeAndUpdateBuffer(RSRcdSurfaceRenderNode& node);
    void ProcessRcdSurfaceRenderNode(RSRcdSurfaceRenderNode& node, rs_rcd::RoundCornerLayer* layerInfo,
        bool resourceChanged);

    void SetUniProcessor(std::shared_ptr<RSProcessor> processor);

private:
    std::shared_ptr<RSBaseRenderEngine> renderEngine_ = nullptr;
    std::shared_ptr<RSProcessor> uniProcessor_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_RS_RCD_RENDER_VISITOR_H