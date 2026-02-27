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
#include "pipeline/rs_context.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t ROOT_VOLUME = 2;
}
class RSB_EXPORT RSUnionRenderNode : public RSCanvasRenderNode {
public:
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::UNION_NODE;
    RSRenderNodeType GetType() const override
    {
        return Type;
    }

    ~RSUnionRenderNode() override;

    void QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    void AddUnionChild(NodeId id);
    void RemoveUnionChild(NodeId id);
    void ResetUnionChildren();
    void ProcessSDFShape(RSDirtyRegionManager& dirtyManager);

    static void ProcessUnionInfoOnTreeStateChanged(const std::shared_ptr<RSRenderNode> node);
    static void ProcessUnionInfoAfterApplyModifiers(const std::shared_ptr<RSRenderNode> node);

private:
    explicit RSUnionRenderNode(NodeId id, const std::weak_ptr<RSContext>& context = {},
        bool isTextureExportNode = false);

    template<RSNGEffectType Type, typename NonLeafClass, typename NonLeafShapeX, typename NonLeafShapeY>
    std::shared_ptr<RSNGRenderShapeBase> GenerateSDFNonLeaf(
        std::queue<std::shared_ptr<RSNGRenderShapeBase>>& shapeQueue)
    {
        auto root = CreateSDFOpShapeWithBaseInitialization(Type);
        shapeQueue.push(root);
        // one Op can have two leaf shape, now we have a root Op
        auto count = unionChildren_.size();
        if (count <= ROOT_VOLUME) {
            return root;
        } else {
            count -= ROOT_VOLUME;
        }
        while (!shapeQueue.empty() && count > 0) {
            auto curShape = std::static_pointer_cast<NonLeafClass>(shapeQueue.front());
            auto addedOpShape = CreateSDFOpShapeWithBaseInitialization(Type);
            if (!curShape->template Getter<NonLeafShapeX>()->Get()) {
                curShape->template Setter<NonLeafShapeX>(addedOpShape);
            } else {
                curShape->template Setter<NonLeafShapeY>(addedOpShape);
                shapeQueue.pop();
            }
            shapeQueue.push(addedOpShape);
            count--;
        }
        return root;
    }

    template<typename NonLeafClass, typename NonLeafShapeX, typename NonLeafShapeY>
    void GenerateSDFLeaf(std::queue<std::shared_ptr<RSNGRenderShapeBase>>& shapeQueue)
    {
        auto context = GetContext().lock();
        if (!context) {
            RS_LOGE("RSUnionRenderNode::GenerateSDFLeaf GetContext fail");
            return;
        }
        for (auto& childId : unionChildren_) {
            auto child = context->GetNodeMap().GetRenderNode<RSRenderNode>(childId);
            if (!child) {
                RS_LOGE("RSUnionRenderNode::GenerateSDFLeaf, child[%{public}" PRIu64 "] Get fail", childId);
                continue;
            }
            auto childShape = GetOrCreateChildSDFShape(child);
            auto transformShape = CreateChildToContainerSDFTransformShape(child, childShape);
            if (shapeQueue.empty()) {
                RS_LOGE("RSUnionRenderNode::GenerateSDFLeaf, shapeTree full");
                break;
            } else {
                auto curShape = std::static_pointer_cast<NonLeafClass>(shapeQueue.front());
                if (!curShape->template Getter<NonLeafShapeX>()->Get()) {
                    curShape->template Setter<NonLeafShapeX>(transformShape);
                } else {
                    curShape->template Setter<NonLeafShapeY>(transformShape);
                    shapeQueue.pop();
                }
            }
        }
    }

    bool GetChildRelativeMatrixToUnionNode(Drawing::Matrix& relativeMatrix, std::shared_ptr<RSRenderNode>& child);
    std::shared_ptr<RSNGRenderShapeBase> CreateSDFOpShapeWithBaseInitialization(RSNGEffectType type);
    std::shared_ptr<RSNGRenderShapeBase> CreateChildToContainerSDFTransformShape(
        std::shared_ptr<RSRenderNode>& child, std::shared_ptr<RSNGRenderShapeBase>& childShape);
    std::shared_ptr<RSNGRenderShapeBase> GetOrCreateChildSDFShape(std::shared_ptr<RSRenderNode>& child);

    static std::shared_ptr<RSUnionRenderNode> FindClosestUnionAncestor(const std::shared_ptr<RSRenderNode> node);

    std::unordered_set<NodeId> unionChildren_;
    
    friend class UnionNodeCommandHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_UNION_RENDER_NODE_H
