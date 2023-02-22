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

    explicit RSProxyRenderNode(
        NodeId id, std::weak_ptr<RSSurfaceRenderNode> target, NodeId targetId, std::weak_ptr<RSContext> context = {});
    ~RSProxyRenderNode() override;

    // void ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas) override;
    // void ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas) override;

    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    // pass render context (matrix/alpha/clip) from proxy to target
    void SetContextMatrix(const SkMatrix& transform);
    void SetContextAlpha(float alpha);
    void SetContextClipRegion(SkRect clipRegion);

    void ResetContextVariableCache();

private:
    std::weak_ptr<RSSurfaceRenderNode> target_;
    NodeId targetId_;

    SkMatrix contextMatrix_ = SkMatrix::I();
    float contextAlpha_ = 0.0f;
    SkRect contextClipRect_ = SkRect::MakeEmpty();
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_PROXY_RENDER_NODE_H
