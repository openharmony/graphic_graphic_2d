/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ui/rs_union_node.h"

#include "command/rs_union_node_command.h"
#include "command/rs_node_command.h"
#include "modifier_ng/geometry/rs_bounds_modifier.h"
#include "platform/common/rs_log.h"
#include "pipeline/rs_node_map.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_ui_context.h"

namespace OHOS {
namespace Rosen {
RSEffectNode::SharedPtr RSUnionNode::Create(
    bool isRenderServiceNode, bool isTextureExportNode, std::shared_ptr<RSUIContext> rsUIContext)
{
    SharedPtr node(new RSUnionNode(isRenderServiceNode, isTextureExportNode, rsUIContext));
    if (rsUIContext != nullptr) {
        rsUIContext->GetMutableNodeMap().RegisterNode(node);
    } else {
        RSNodeMap::MutableInstance().RegisterNode(node);
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSUnionNodeCreate>(node->GetId(), isTextureExportNode);
    auto transaction = node->GetRSTransaction();
    if (transaction == nullptr) {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy == nullptr) {
            return node;
        }
        transactionProxy->AddCommand(command, node->IsRenderServiceNode());
        return node;
    }
    transaction->AddCommand(command, node->IsRenderServiceNode());
    node->SetUIContextToken();
    return node;
}

void RSUnionNode::SetUnionSpacing(float spacing)
{
    SetPropertyNG<ModifierNG::RSBoundsModifier, &ModifierNG::RSBoundsModifier::SetUnionSpacing>(spacing);
}

void RSUnionNode::RegisterNodeMap()
{
    auto rsContext = GetRSUIContext();
    if (rsContext == nullptr) {
        return;
    }
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.RegisterNode(shared_from_this());
}

RSUnionNode::RSUnionNode(bool isRenderServiceNode, bool isTextureExportNode, std::shared_ptr<RSUIContext> rsUIContext)
    : RSCanvasNode(isRenderServiceNode, isTextureExportNode, rsUIContext) {}
} // namespace Rosen
} // namespace OHOS
