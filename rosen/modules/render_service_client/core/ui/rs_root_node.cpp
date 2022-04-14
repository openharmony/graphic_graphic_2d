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

#include "ui/rs_root_node.h"

#include "command/rs_root_node_command.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSNode> RSRootNode::Create(bool isRenderServiceNode)
{
    std::shared_ptr<RSRootNode> node(new RSRootNode(isRenderServiceNode));
    RSNodeMap::MutableInstance().RegisterNode(node);

    std::unique_ptr<RSCommand> command = std::make_unique<RSRootNodeCreate>(node->GetId());
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, isRenderServiceNode);
    }
    return node;
}

RSRootNode::RSRootNode(bool isRenderServiceNode) : RSCanvasNode(isRenderServiceNode) {}

void RSRootNode::AttachRSSurfaceNode(std::shared_ptr<RSSurfaceNode> surfaceNode) const
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSRootNodeAttachRSSurfaceNode>(GetId(), surfaceNode->GetId());
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
    }
}

} // namespace Rosen
} // namespace OHOS
