/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_PROXY_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_PROXY_RENDER_NODE_H

#include "common/rs_macros.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_render_node.h"

namespace OHOS {
namespace Rosen {
class RSSurfaceRenderNode;
class RSB_EXPORT RSProxyRenderNode : public RSRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSProxyRenderNode>;
    using SharedPtr = std::shared_ptr<RSProxyRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::PROXY_NODE;
    RSRenderNodeType GetType() const override
    {
        return Type;
    }

    explicit RSProxyRenderNode(NodeId id, std::weak_ptr<RSSurfaceRenderNode> target, NodeId targetId,
        const std::weak_ptr<RSContext>& context = {});
    ~RSProxyRenderNode() override;

    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    // Transfer the rendering context variables (matrix/alpha/clip) from the proxy node to the target node, usually a
    // surface node. Note that:
    // 1. All three variables (alpha, matrix, and clipRegion) are RELATIVE to its parent node.
    // 2. Alpha can be processed as an absolute value, as its parent (surface) node's alpha should always be 1.0f.
    // 3. The matrix and clipRegion should be applied according to the parent node's matrix.
    void SetContextMatrix(const std::optional<Drawing::Matrix>& transform);
    void SetContextAlpha(float alpha);
    void SetContextClipRegion(const std::optional<Drawing::Rect>& clipRegion);

    void ResetContextVariableCache();
protected:
    void OnTreeStateChanged() override;

private:
    std::weak_ptr<RSSurfaceRenderNode> target_;
    NodeId targetId_;

    std::optional<Drawing::Matrix> contextMatrix_;
    float contextAlpha_ = 0.0f;
    std::optional<Drawing::Rect> contextClipRect_;

    void CleanUp(bool removeModifiers);

    friend class RSUniRenderVisitor;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_PROXY_RENDER_NODE_H
