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

#include "ui/rs_proxy_node.h"

#include "command/rs_base_node_command.h"
#include "command/rs_proxy_node_command.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_ui_context.h"

namespace OHOS {
namespace Rosen {
RSProxyNode::SharedPtr RSProxyNode::Create(
    NodeId targetNodeId, std::string name, std::shared_ptr<RSUIContext> rsUIContext)
{
    auto prevNode =
        rsUIContext ? rsUIContext->GetNodeMap().GetNode(targetNodeId) : RSNodeMap::Instance().GetNode(targetNodeId);
    if (prevNode) {
        // if the node id is already in the map, we should not create a new node.
        return prevNode->ReinterpretCastTo<RSProxyNode>();
    }

    SharedPtr node(new RSProxyNode(targetNodeId, std::move(name), rsUIContext));
    if (rsUIContext != nullptr) {
        rsUIContext->GetMutableNodeMap().RegisterNode(node);
        auto transaction = rsUIContext->GetRSTransaction();
        if (transaction == nullptr) {
            ROSEN_LOGW("multi-client RSProxyNode::Create, transaction is nullptr");
            return node;
        }
        NodeId proxyNodeId = node->GetHierarchyCommandNodeId();
        std::unique_ptr<RSCommand> command = std::make_unique<RSProxyNodeCreate>(proxyNodeId, targetNodeId);
        transaction->AddCommand(command, node->IsUniRenderEnabled());
        // create proxy node in RS even if uni render not enabled.
        if (!node->IsUniRenderEnabled()) {
            std::unique_ptr<RSCommand> command = std::make_unique<RSProxyNodeCreate>(proxyNodeId, targetNodeId);
            transaction->AddCommand(command, true);
        }
        ROSEN_LOGD("RSProxyNode::Create, target node id:%{public}" PRIu64 ", name %s proxy node id %{public}" PRIu64,
            node->GetId(), node->GetName().c_str(), proxyNodeId);
    } else {
        RSNodeMap::MutableInstance().RegisterNode(node);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy == nullptr) {
            return node;
        }
        NodeId proxyNodeId = node->GetHierarchyCommandNodeId();
        std::unique_ptr<RSCommand> command = std::make_unique<RSProxyNodeCreate>(proxyNodeId, targetNodeId);
        transactionProxy->AddCommand(command, node->IsUniRenderEnabled());
        // create proxy node in RS even if uni render not enabled.
        if (!node->IsUniRenderEnabled()) {
            std::unique_ptr<RSCommand> command = std::make_unique<RSProxyNodeCreate>(proxyNodeId, targetNodeId);
            transactionProxy->AddCommand(command, true);
        }
        ROSEN_LOGD("RSProxyNode::Create, target node id:%{public}" PRIu64 ", name %s proxy node id %{public}" PRIu64,
            node->GetId(), node->GetName().c_str(), proxyNodeId);
    }
    node->SetUIContextToken();
    return node;
}

RSProxyNode::~RSProxyNode()
{
    ROSEN_LOGD("RSProxyNode::~RSProxyNode, proxy id:%{public}" PRIu64 " target:%{public}" PRIu64,
        proxyNodeId_, GetId());
    // destroy remote RSProxyRenderNode, NOT the target node.
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeDestroy>(proxyNodeId_);
    AddCommand(command, IsUniRenderEnabled());
    // destroy corresponding RSProxyRenderNode in RS even if uni render not enabled.
    if (!IsUniRenderEnabled()) {
        command = std::make_unique<RSBaseNodeDestroy>(proxyNodeId_);
        AddCommand(command, true);
    }
    ROSEN_LOGD("RSProxyNode::~RSProxyNode, id:%{public}" PRIu64, GetId());
}

void RSProxyNode::RegisterNodeMap()
{
    auto rsContext = GetRSUIContext();
    if (rsContext == nullptr) {
        return;
    }
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.RegisterNode(shared_from_this());
}

RSProxyNode::RSProxyNode(NodeId targetNodeId, std::string name, std::shared_ptr<RSUIContext> rsUIContext)
    : RSNode(true, false, rsUIContext), name_(std::move(name))
{
    // hacky trick: replace self node id with provided targetNodeId, use self generated id as proxyNodeId
    proxyNodeId_ = GetId();
    SetId(targetNodeId);
    // disable base node destructor, we will destroy the proxy node in the destructor of this class.
    skipDestroyCommandInDestructor_ = true;
}

void RSProxyNode::ResetContextVariableCache() const
{
    // reset context variable cache in RSProxyRenderNode, make sure next visit will flush correct context variables.
    // send command to proxy node, not the target node
    std::unique_ptr<RSCommand> command = std::make_unique<RSProxyNodeResetContextVariableCache>(proxyNodeId_);
    AddCommand(command, IsUniRenderEnabled());
}

void RSProxyNode::AddChild(std::shared_ptr<RSBaseNode> child, int index)
{
    // RSProxyNode::AddChild for proxyNode is not allowed
}

void RSProxyNode::RemoveChild(std::shared_ptr<RSBaseNode> child)
{
    // RSProxyNode::RemoveChild for proxyNode is not allowed
}

void RSProxyNode::ClearChildren()
{
    // RSProxyNode::ClearChildren for proxyNode is not allowed
}
} // namespace Rosen
} // namespace OHOS
