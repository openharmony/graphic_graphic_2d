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

#ifndef RS_NODE_COST_MANAGER_H
#define RS_NODE_COST_MANAGER_H

#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_uni_render_visitor.h"

namespace OHOS::Rosen {
class RSSurfaceRenderNode;
class RS_EXPORT RSNodeCostManager {
public:
    RSNodeCostManager(bool doAnimate, bool opDropped, bool isSecurityDisplay);
    ~RSNodeCostManager() = default;
    int32_t GetDirtyNodeCost() const;
    void CalcNodeCost(RSSurfaceRenderNode& node);
private:
    bool IsSkipProcessing(RSSurfaceRenderNode& node) const;
    void CalcBaseRenderNodeCost(RSBaseRenderNode& node);
    void CalcCanvasRenderNodeCost(RSCanvasRenderNode& node);
    void AddNodeCost(int32_t cost);
    void CalcSurfaceRenderNodeCost(RSSurfaceRenderNode& node);
    int32_t dirtyNodeCost_ = 0;
    bool isOpDropped_ = false;
    bool isSecurityDisplay_ = false;
    bool doAnimate_ = false;
#ifndef RS_ENABLE_VK
    PartialRenderType partialRenderType_;
#endif
    std::shared_ptr<RSSurfaceRenderNode> costSurfaceNode_;
    bool isOcclusionEnabled_ = false;
};
}
#endif // RS_NODE_COST_MANAGER_H