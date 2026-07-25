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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DEPTH_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DEPTH_RENDER_NODE_H

#include "common/rs_macros.h"
#include "pipeline/rs_render_node.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSDepthRenderNode : public RSRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSDepthRenderNode>;
    using SharedPtr = std::shared_ptr<RSDepthRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::DEPTH_NODE;
    virtual ~RSDepthRenderNode();

    RSRenderNodeType GetType() const override
    {
        return Type;
    }

    void SetDepthSpaceType(DepthSpaceType depthSpaceType);
    DepthSpaceType GetDepthSpaceType() const;

    void InitRenderParams() override;
    void UpdateRenderParams() override;
    void QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor,
        bool isParentPrepareInReverseOrder = false) override;
    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    void ApplyDepthSpaceModifier();

private:
    explicit RSDepthRenderNode(NodeId id, const std::weak_ptr<RSContext>& context = {},
        bool isTextureExportNode = false);

    DepthSpaceType depthSpaceType_ = DepthSpaceType::INSTANCE;

    friend class RSDepthNodeCommandHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DEPTH_RENDER_NODE_H