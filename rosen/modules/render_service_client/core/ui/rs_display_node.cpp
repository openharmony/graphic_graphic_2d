/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
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
    ROSEN_LOGD("RSDisplayNode::Create, id:%" PRIu64, node->GetId());
    return node;
}

void RSDisplayNode::SetScreenId(uint64_t screenId)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSDisplayNodeSetScreenId>(GetId(), screenId);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
    ROSEN_LOGD("RSDisplayNode::SetScreenId, ScreenId:%" PRIu64, screenId);
}

void RSDisplayNode::SetDisplayOffset(int32_t offsetX, int32_t offsetY)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSDisplayNodeSetDisplayOffset>(GetId(), offsetX, offsetY);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
    ROSEN_LOGD("RSDisplayNode::SetDisplayOffset, offsetX:%d, offsetY:%d", offsetX, offsetY);
}

void RSDisplayNode::SetSecurityDisplay(bool isSecurityDisplay)
{
    isSecurityDisplay_ = isSecurityDisplay;
    std::unique_ptr<RSCommand> command = std::make_unique<RSDisplayNodeSetSecurityDisplay>(GetId(), isSecurityDisplay);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
    ROSEN_LOGD("RSDisplayNode::SetSecurityDisplay, displayNodeId:[%" PRIu64 "] isSecurityDisplay:[%s]", GetId(),
        isSecurityDisplay ? "true" : "false");
}

bool RSDisplayNode::GetSecurityDisplay() const
{
    return isSecurityDisplay_;
}

void RSDisplayNode::SetDisplayNodeMirrorConfig(const RSDisplayNodeConfig& displayNodeConfig)
{
    isMirroredDisplay_ = displayNodeConfig.isMirrored;
    std::unique_ptr<RSCommand> command = std::make_unique<RSDisplayNodeSetDisplayMode>(GetId(), displayNodeConfig);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
    ROSEN_LOGD("RSDisplayNode::SetDisplayNodeMirrorConfig, displayNodeId:[%" PRIu64 "] isMirrored:[%s]", GetId(),
        displayNodeConfig.isMirrored ? "true" : "false");
}

bool RSDisplayNode::IsMirrorDisplay() const
{
    return isMirroredDisplay_;
}

RSDisplayNode::RSDisplayNode(const RSDisplayNodeConfig& config)
    : RSNode(true), screenId_(config.screenId), offsetX_(0), offsetY_(0), isMirroredDisplay_(config.isMirrored)
{
    (void)screenId_;
    (void)offsetX_;
    (void)offsetY_;
}

RSDisplayNode::~RSDisplayNode() = default;

} // namespace Rosen
} // namespace OHOS
