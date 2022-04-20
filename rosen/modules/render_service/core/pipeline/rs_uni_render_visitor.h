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
#ifndef RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_VISITOR_H
#define RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_VISITOR_H

#include <set>
#include <string>

#include "pipeline/rs_processor.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "screen_manager/rs_screen_manager.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {

class RSUniRenderVisitor : public RSNodeVisitor {
public:
    RSUniRenderVisitor();
    ~RSUniRenderVisitor() override;

    void PrepareBaseRenderNode(RSBaseRenderNode& node) override;
    void PrepareDisplayRenderNode(RSDisplayRenderNode& node) override;
    void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override;
    void PrepareRootRenderNode(RSRootRenderNode& node) override;
    void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override;

    void ProcessBaseRenderNode(RSBaseRenderNode& node) override;
    void ProcessDisplayRenderNode(RSDisplayRenderNode& node) override;
    void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override;
    void ProcessRootRenderNode(RSRootRenderNode& node) override;
    void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override;

private:
    void DrawBufferOnCanvas(RSSurfaceRenderNode& node);
    static bool IsChildOfDisplayNode(RSBaseRenderNode& node);
    static bool IsChildOfSurfaceNode(RSBaseRenderNode& node);

    ScreenInfo screenInfo_;
    RSDirtyRegionManager dirtyManager_;
    RSRenderNode* parent_ = nullptr;
    bool dirtyFlag_ { false };
    RSPaintFilterCanvas* canvas_ = nullptr;

    float globalZOrder_ { 0.0f };
    float uniZOrder_ { 0.0f };
    std::shared_ptr<RSProcessor> processor_;
    bool isUniRender_ { false };
    bool hasUniRender_ { false };
    bool isUniRenderForAll_ { false };
    std::set<std::string> uniRenderList_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_VISITOR_H