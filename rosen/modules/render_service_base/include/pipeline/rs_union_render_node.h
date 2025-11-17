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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_UNION_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_UNION_RENDER_NODE_H

#include <queue>

#include "effect/rs_render_shape_base.h"
#include "pipeline/rs_canvas_render_node.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSUnionRenderNode : public RSCanvasRenderNode {
public:
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::UNION_NODE;
    RSRenderNodeType GetType() const override
    {
        return Type;
    }

    ~RSUnionRenderNode() override;

    void QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    void UpdateVisibleUnionChildren(RSRenderNode& childNode);
    void ResetVisibleUnionChildren();
    void ProcessSDFShape();

private:
    explicit RSUnionRenderNode(NodeId id, const std::weak_ptr<RSContext>& context = {},
        bool isTextureExportNode = false);

    template<RSNGEffectType Type, typename NonLeafClass, typename NonLeafShapeX, typename NonLeafShapeY>
    std::shared_ptr<RSNGRenderShapeBase> GenerateSDFNonLeaf(
        std::queue<std::shared_ptr<RSNGRenderShapeBase>>& shapeQueue);
    template<typename NonLeafClass, typename NonLeafShapeX, typename NonLeafShapeY>
    void GenerateSDFLeaf(std::queue<std::shared_ptr<RSNGRenderShapeBase>>& shapeQueue);

    bool GetChildRelativeMatrixToUnionNode(Drawing::Matrix& relativeMatrix, std::shared_ptr<RSRenderNode>& child);
    std::shared_ptr<RSNGRenderShapeBase> CreateSDFOpShapeWithBaseInitialization(RSNGEffectType type);
    std::shared_ptr<RSNGRenderShapeBase> GetOrCreateChildSDFShape(Drawing::Matrix& relativeMatrix,
        std::shared_ptr<RSRenderNode>& child);

    std::unordered_set<NodeId> visibleUnionChildren_;
    
    friend class UnionNodeCommandHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_UNION_RENDER_NODE_H
