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

namespace OHOS {
namespace Rosen {
RSProxyNode::SharedPtr RSProxyNode::Create(NodeId targetNodeId, std::string name)
{
    if (auto prevNode = RSNodeMap::Instance().GetNode(targetNodeId)) {
        // if the node id is already in the map, we should not create a new node.
        return prevNode->ReinterpretCastTo<RSProxyNode>();
    }

    SharedPtr node(new RSProxyNode(targetNodeId, std::move(name)));
    RSNodeMap::MutableInstance().RegisterNode(node);

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return node;
    }
    NodeId proxyNodeId = node->GetHierarchyCommandNodeId();
    // create proxy node in RS and RT.
    std::unique_ptr<RSCommand> command = std::make_unique<RSProxyNodeCreate>(proxyNodeId, targetNodeId);
    transactionProxy->AddCommand(command, true);
    std::unique_ptr<RSCommand> extraCommand = std::make_unique<RSProxyNodeCreate>(proxyNodeId, targetNodeId);
    transactionProxy->AddCommand(extraCommand, false);

    ROSEN_LOGD("RSProxyNode::Create, target node id:%" PRIu64 ", name %s proxy node id %" PRIu64, node->GetId(),
        node->GetName().c_str(), proxyNodeId);

    return node;
}

RSProxyNode::~RSProxyNode()
{
    ROSEN_LOGD("RSProxyNode::~RSProxyNode, proxy id:%" PRIu64 " target:%" PRIu64, proxyNodeId_, GetId());

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }

    // destroy remote RSProxyRenderNode in RS and RT, NOT the target node.
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeDestroy>(proxyNodeId_);
    transactionProxy->AddCommand(command, true);
    std::unique_ptr<RSCommand> extraCommand = std::make_unique<RSBaseNodeDestroy>(proxyNodeId_);
    transactionProxy->AddCommand(extraCommand, false);

    ROSEN_LOGD("RSProxyNode::~RSProxyNode, id:%" PRIu64, GetId());
}

RSProxyNode::RSProxyNode(NodeId targetNodeId, std::string name) : RSNode(true), name_(std::move(name))
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
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    // send command to proxy node, not the target node
    std::unique_ptr<RSCommand> commandRT = std::make_unique<RSProxyNodeResetContextVariableCache>(proxyNodeId_);
    transactionProxy->AddCommand(commandRT, IsUniRenderEnabled());
    if (isRenderServiceNode_) {
        std::unique_ptr<RSCommand> commandRS = std::make_unique<RSProxyNodeResetContextVariableCache>(proxyNodeId_);
        transactionProxy->AddCommand(commandRS, false);
    }
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
