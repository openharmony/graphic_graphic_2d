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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_PROXY_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_PROXY_NODE_H

#include <parcel.h>
#include <string>

#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {
class RS_EXPORT RSProxyNode : public RSNode {
public:
    using WeakPtr = std::weak_ptr<RSProxyNode>;
    using SharedPtr = std::shared_ptr<RSProxyNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::PROXY_NODE;
    RSUINodeType GetType() const override
    {
        return Type;
    }

    static SharedPtr Create(NodeId targetNodeId, std::string name = "ProxyNode");
    ~RSProxyNode() override;

    void ResetContextVariableCache() const;

    const std::string& GetName() const
    {
        return name_;
    }

    void AddChild(std::shared_ptr<RSBaseNode> child, int index) override;
    void RemoveChild(std::shared_ptr<RSBaseNode> child) override;
    void ClearChildren() override;

protected:
    explicit RSProxyNode(NodeId targetNodeId, std::string name);

    // when add/remove/update child, construct command using proxy node id, not target node id
    NodeId GetHierarchyCommandNodeId() const override
    {
        return proxyNodeId_;
    }

private:
    void CreateProxyRenderNode();
    NodeId proxyNodeId_;
    std::string name_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_PROXY_NODE_H
