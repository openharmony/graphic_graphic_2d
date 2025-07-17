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

#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_RENDER_SERVICE_VISITOR_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_RENDER_SERVICE_VISITOR_H

#include <memory>

#include "draw/canvas.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_processor.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {

class RSRenderServiceVisitor : public RSNodeVisitor {
public:
    RSRenderServiceVisitor(bool parallel = false);
    ~RSRenderServiceVisitor();

    void PrepareChildren(RSRenderNode &node) override;
    void PrepareCanvasRenderNode(RSCanvasRenderNode &node) override {}
    void PrepareScreenRenderNode(RSScreenRenderNode &node) override;
    void PrepareLogicalDisplayRenderNode(RSLogicalDisplayRenderNode& node) override;
    void PrepareProxyRenderNode(RSProxyRenderNode& node) override {}
    void PrepareRootRenderNode(RSRootRenderNode& node) override {}
    void PrepareSurfaceRenderNode(RSSurfaceRenderNode &node) override;
    void PrepareEffectRenderNode(RSEffectRenderNode& node) override {}

    void ProcessChildren(RSRenderNode &node) override;
    void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override {}
    void ProcessScreenRenderNode(RSScreenRenderNode &node) override;
    void ProcessLogicalDisplayRenderNode(RSLogicalDisplayRenderNode& node) override;
    void ProcessProxyRenderNode(RSProxyRenderNode& node) override {}
    void ProcessRootRenderNode(RSRootRenderNode& node) override {}
    void ProcessSurfaceRenderNode(RSSurfaceRenderNode &node) override;
    void ProcessEffectRenderNode(RSEffectRenderNode& node) override {}

    void SetAnimateState(bool doAnimate)
    {
        doAnimate_ = doAnimate;
    }

    bool ShouldForceSerial()
    {
        return mForceSerial;
    }

private:
    void CreateCanvas(int32_t width, int32_t height, bool isMirrored = false);
    bool CreateProcessor(RSScreenRenderNode& node);
    void UpdateScreenNodeCompositeType(RSScreenRenderNode& node, const ScreenInfo& screenInfo);
    void StoreSurfaceNodeAttrsToScreenNode(RSScreenRenderNode& screenNode, const RSSurfaceRenderNode& surfaceNode);
    void RestoreSurfaceNodeAttrsFromScreenNode(
        const RSScreenRenderNode& screenNode, RSSurfaceRenderNode& surfaceNode);
    void ResetSurfaceNodeAttrsInScreenNode(RSScreenRenderNode& screenNode);

private:
    bool isSecurityDisplay_ = false;
    bool mParallelEnable = false;
    bool mForceSerial = false;
    bool doAnimate_ = false;
    float globalZOrder_ = 0.0f;
    int32_t offsetX_ = 0;
    int32_t offsetY_ = 0;
    ScreenId currentVisitDisplay_ = INVALID_SCREEN_ID;
    std::unique_ptr<Drawing::Canvas> drawingCanvas_;
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
    std::shared_ptr<RSProcessor> processor_ = nullptr;
    std::shared_ptr<RSBaseRenderEngine> processorRenderEngine_ = nullptr;
    std::shared_ptr<RSScreenRenderNode> curScreenNode_;
    std::map<ScreenId, bool> displayHasSecSurface_;
    std::unordered_map<NodeId, std::vector<std::function<void()>>> foregroundSurfaces_ = {};
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_RENDER_SERVICE_VISITOR_H
