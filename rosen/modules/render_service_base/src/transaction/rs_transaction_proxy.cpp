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

#include "transaction/rs_transaction_proxy.h"
#include <stdlib.h>

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
    FlushImplicitTransaction();
}

void RSTransactionProxy::Init()
{
    instance_ = new RSTransactionProxy();
    ::atexit(&RSTransactionProxy::Destory);
}

void RSTransactionProxy::Destory()
{
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
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

void RSTransactionProxy::AddCommand(std::unique_ptr<RSCommand>& command, bool isRenderServiceCommand)
{
    if ((renderServiceClient_ == nullptr && renderThreadClient_ == nullptr) || command == nullptr) {
        return;
    }

    std::unique_lock<std::mutex> cmdLock(mutex_);

    if (renderThreadClient_ == nullptr || isRenderServiceCommand) {
        AddRemoteCommand(command);
        return;
    }

    if (renderServiceClient_ == nullptr || !isRenderServiceCommand) {
        AddCommonCommand(command);
        return;
    }
}

void RSTransactionProxy::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task, bool isRenderServiceTask)
{
    if ((renderServiceClient_ == nullptr && renderThreadClient_ == nullptr) || task == nullptr) {
        return;
    }

    if (renderThreadClient_ == nullptr || isRenderServiceTask) {
        renderServiceClient_->ExecuteSynchronousTask(task);
        return;
    }

    if (renderServiceClient_ == nullptr || !isRenderServiceTask) {
        renderThreadClient_->ExecuteSynchronousTask(task);
        return;
    }
}

void RSTransactionProxy::FlushImplicitTransaction()
{
    std::unique_lock<std::mutex> cmdLock(mutex_);
    if (renderThreadClient_ != nullptr && !implicitCommonTransactionData_->IsEmpty()) {
        renderThreadClient_->CommitTransaction(implicitCommonTransactionData_);
        implicitCommonTransactionData_ = std::make_unique<RSTransactionData>();
    }
    if (renderServiceClient_ != nullptr && !implicitRemoteTransactionData_->IsEmpty()) {
        renderServiceClient_->CommitTransaction(implicitRemoteTransactionData_);
        implicitRemoteTransactionData_ = std::make_unique<RSTransactionData>();
    }
}

void RSTransactionProxy::AddCommonCommand(std::unique_ptr<RSCommand>& command)
{
    implicitCommonTransactionData_->AddCommand(command);
}

void RSTransactionProxy::AddRemoteCommand(std::unique_ptr<RSCommand>& command)
{
    implicitRemoteTransactionData_->AddCommand(command);
}

} // namespace Rosen
} // namespace OHOS
