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

#include "ui/rs_canvas_drawing_node.h"

#include "command/rs_canvas_drawing_node_command.h"
#include "common/rs_obj_geometry.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
RSCanvasDrawingNode::SharedPtr RSCanvasDrawingNode::Create(bool isRenderServiceNode)
{
    SharedPtr node(new RSCanvasDrawingNode(isRenderServiceNode));
    RSNodeMap::MutableInstance().RegisterNode(node);

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (!transactionProxy) {
        return node;
    }

    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasDrawingNodeCreate>(node->GetId());
    transactionProxy->AddCommand(command, node->IsRenderServiceNode());
    return node;
}

RSCanvasDrawingNode::RSCanvasDrawingNode(bool isRenderServiceNode) : RSCanvasNode(isRenderServiceNode) {}

RSCanvasDrawingNode::~RSCanvasDrawingNode() {}
} // namespace Rosen
} // namespace OHOS