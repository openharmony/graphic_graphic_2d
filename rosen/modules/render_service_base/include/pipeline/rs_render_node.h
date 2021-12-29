/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_H

#include <memory>

#include "pipeline/rs_property_render_node.h"

namespace OHOS {
namespace Rosen {
class DrawCmdList;
class RSPaintFilterCanvas;

class RSRenderNode : public RSPropertyRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSRenderNode>;
    using SharedPtr = std::shared_ptr<RSRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::NODE;

    explicit RSRenderNode(NodeId id);
    virtual ~RSRenderNode();

    void UpdateRecording(std::shared_ptr<DrawCmdList> drawCmds, bool drawContentLast);

    void ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas) override;
    void ProcessRenderContents(RSPaintFilterCanvas& canvas) override;
    void ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas) override;

    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    RSRenderNodeType GetType() override
    {
        return RSRenderNodeType::NODE;
    }

private:
    std::shared_ptr<DrawCmdList> drawCmdList_ { nullptr };
    bool drawContentLast_ = false;

    friend class RSRenderTransition;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_H
