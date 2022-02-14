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

#include "ui/rs_display_node.h"

#include "command/rs_display_node_command.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
#include "pipeline/rs_node_map.h"
namespace OHOS {
namespace Rosen {

RSDisplayNode::SharedPtr RSDisplayNode::Create(const RSDisplayNodeConfig& displayNodeConfig)
{
    SharedPtr node(new RSDisplayNode(displayNodeConfig));
    RSNodeMap::MutableInstance().RegisterNode(node);

    std::unique_ptr<RSCommand> command = std::make_unique<RSDisplayNodeCreate>(node->GetId(), displayNodeConfig);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
    ROSEN_LOGD("RSDisplayNode::Create, id:%llu", node->GetId());
    return node;
}

void RSDisplayNode::SetScreenId(uint64_t screenId)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSDisplayNodeSetScreenId>(GetId(), screenId);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
    ROSEN_LOGD("RSDisplayNode::SetScreenId, ScreenId:%llu", screenId);
}

RSDisplayNode::RSDisplayNode(const RSDisplayNodeConfig& config)
    : RSBaseNode(true), screenId_(config.screenId)
{
    (void)screenId_;
}

RSDisplayNode::~RSDisplayNode() {}

} // namespace Rosen
} // namespace OHOS
