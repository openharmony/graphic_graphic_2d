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

#include "transaction/rs_transaction_proxy.h"
#include <stdlib.h>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
std::once_flag RSTransactionProxy::flag_;
RSTransactionProxy* RSTransactionProxy::instance_ = nullptr;

RSTransactionProxy* RSTransactionProxy::GetInstance()
{
    std::call_once(flag_, &RSTransactionProxy::Init);
    return instance_;
}

RSTransactionProxy::RSTransactionProxy()
{
}

RSTransactionProxy::~RSTransactionProxy()
{
}

void RSTransactionProxy::Init()
{
    instance_ = new RSTransactionProxy();
    ::atexit(&RSTransactionProxy::Destroy);
}

void RSTransactionProxy::Destroy()
{
    instance_ = nullptr;
}

void RSTransactionProxy::SetRenderThreadClient(std::unique_ptr<RSIRenderClient>& renderThreadClient)
{
    if (renderThreadClient != nullptr) {
        renderThreadClient_ = std::move(renderThreadClient);
    }
}

void RSTransactionProxy::SetRenderServiceClient(const std::shared_ptr<RSIRenderClient>& renderServiceClient)
{
    if (renderServiceClient != nullptr) {
        renderServiceClient_ = renderServiceClient;
    }
}

void RSTransactionProxy::AddCommand(std::unique_ptr<RSCommand>& command, bool isRenderServiceCommand,
                                    FollowType followType, NodeId nodeId)
{
    if ((renderServiceClient_ == nullptr && renderThreadClient_ == nullptr) || command == nullptr) {
        return;
    }

    std::unique_lock<std::mutex> cmdLock(mutex_);

    if (renderServiceClient_ != nullptr && isRenderServiceCommand) {
        AddRemoteCommand(command, nodeId, followType);
        return;
    }

    if (renderThreadClient_ != nullptr && !isRenderServiceCommand) {
        AddCommonCommand(command);
        return;
    }
    ROSEN_LOGE("RSTransactionProxy::AddCommand failed, command type and client type not match !");
}

void RSTransactionProxy::AddCommandFromRT(std::unique_ptr<RSCommand>& command, NodeId nodeId, FollowType followType)
{
    if (renderServiceClient_ == nullptr || command == nullptr) {
        return;
    }

    {
        std::unique_lock<std::mutex> cmdLock(mutexForRT_);
        implicitTransactionDataFromRT_->AddCommand(command, nodeId, followType);
    }
}

void RSTransactionProxy::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task, bool isRenderServiceTask)
{
    if ((renderServiceClient_ == nullptr && renderThreadClient_ == nullptr) || task == nullptr) {
        return;
    }

    if (renderThreadClient_ != nullptr && !isRenderServiceTask) {
        renderThreadClient_->ExecuteSynchronousTask(task);
        return;
    }

    if (renderServiceClient_ != nullptr && isRenderServiceTask) {
        renderServiceClient_->ExecuteSynchronousTask(task);
        return;
    }
}

void RSTransactionProxy::FlushImplicitTransaction(uint64_t timestamp, const std::string& abilityName)
{
    std::unique_lock<std::mutex> cmdLock(mutex_);
    timestamp_ = std::max(timestamp, timestamp_);
    if (renderThreadClient_ != nullptr && !implicitCommonTransactionData_->IsEmpty()) {
        implicitCommonTransactionData_->timestamp_ = timestamp_;
        implicitCommonTransactionData_->abilityName_ = abilityName;
        renderThreadClient_->CommitTransaction(implicitCommonTransactionData_);
        implicitCommonTransactionData_ = std::make_unique<RSTransactionData>();
    }
    if (renderServiceClient_ != nullptr && !implicitRemoteTransactionData_->IsEmpty()) {
        implicitRemoteTransactionData_->timestamp_ = timestamp_;
        renderServiceClient_->CommitTransaction(implicitRemoteTransactionData_);
        implicitRemoteTransactionData_ = std::make_unique<RSTransactionData>();
    }
}

void RSTransactionProxy::FlushImplicitTransactionFromRT(uint64_t timestamp)
{
    std::unique_lock<std::mutex> cmdLock(mutexForRT_);
    if (renderServiceClient_ != nullptr && !implicitTransactionDataFromRT_->IsEmpty()) {
        implicitTransactionDataFromRT_->timestamp_ = timestamp;
        renderServiceClient_->CommitTransaction(implicitTransactionDataFromRT_);
        implicitTransactionDataFromRT_ = std::make_unique<RSTransactionData>();
    }
}

void RSTransactionProxy::Begin()
{
    std::unique_lock<std::mutex> cmdLock(mutex_);
    implicitCommonTransactionDataStack_.emplace(std::make_unique<RSTransactionData>());
    implicitRemoteTransactionDataStack_.emplace(std::make_unique<RSTransactionData>());
}

void RSTransactionProxy::Commit(uint64_t timestamp)
{
    std::unique_lock<std::mutex> cmdLock(mutex_);
    if (!implicitCommonTransactionDataStack_.empty()) {
        implicitCommonTransactionDataStack_.pop();
    }

    if (!implicitRemoteTransactionDataStack_.empty()) {
        if (renderServiceClient_ != nullptr && !implicitRemoteTransactionDataStack_.top()->IsEmpty()) {
            implicitRemoteTransactionDataStack_.top()->timestamp_ = timestamp;
            renderServiceClient_->CommitTransaction(implicitRemoteTransactionDataStack_.top());
        }
        implicitRemoteTransactionDataStack_.pop();
    }
}

void RSTransactionProxy::AddCommonCommand(std::unique_ptr<RSCommand> &command)
{
    if (!implicitCommonTransactionDataStack_.empty()) {
        implicitCommonTransactionDataStack_.top()->AddCommand(command, 0, FollowType::NONE);
        return;
    }
    implicitCommonTransactionData_->AddCommand(command, 0, FollowType::NONE);
}

void RSTransactionProxy::AddRemoteCommand(std::unique_ptr<RSCommand>& command, NodeId nodeId, FollowType followType)
{
    if (!implicitRemoteTransactionDataStack_.empty()) {
        implicitRemoteTransactionDataStack_.top()->AddCommand(command, nodeId, followType);
        return;
    }
    implicitRemoteTransactionData_->AddCommand(command, nodeId, followType);
}

} // namespace Rosen
} // namespace OHOS
