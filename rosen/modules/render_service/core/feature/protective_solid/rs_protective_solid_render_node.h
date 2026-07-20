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

#ifndef RENDER_SERVICE_CORE_RS_PROTECTIVE_SOLID_RENDER_NODE_H
#define RENDER_SERVICE_CORE_RS_PROTECTIVE_SOLID_RENDER_NODE_H

#include <memory>
#include "common/rs_rect.h"
#include "memory/rs_memory_track.h"
#include "rs_layer.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_surface_render_node.h"
#include "params/rs_surface_render_params.h"
#include "visitor/rs_node_visitor.h"
#include "surface_type.h"

namespace OHOS {
namespace Rosen {

struct ProtectiveSolidConfig {
    RectI rect;
    Vector4f bounds;
};

static const ProtectiveSolidConfig PROTECTIVE_SOLID_CONFIGS[] = {
    {{0, 0, 2232, 2128}, {0, 2128, 2232, 200}},   // LM
    {{0, 1136, 2232, 2048}, {0, 936, 2232, 200}},  // RM
    {{0, 0, 2232, 1136}, {0, 1136, 2232, 200}},   // N
};

class RSProtectiveSolidRenderNode : public RSSurfaceRenderNode {
public:
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::PROTECTIVE_SOLID_NODE;
    using WeakPtr = std::weak_ptr<RSProtectiveSolidRenderNode>;
    using SharedPtr = std::shared_ptr<RSProtectiveSolidRenderNode>;

    explicit RSProtectiveSolidRenderNode(NodeId id, const std::weak_ptr<RSContext>& context = {});
    ~RSProtectiveSolidRenderNode() override;

    RSRenderNodeType GetType() const override
    {
        return RSRenderNodeType::PROTECTIVE_SOLID_NODE;
    }

    void QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor,
        bool isParentPrepareInReverseOrder = false) override;
    
    void UpdateProtectiveSolidLayerInfo(GraphicTransformType transform);
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_RS_PROTECTIVE_SOLID_RENDER_NODE_H