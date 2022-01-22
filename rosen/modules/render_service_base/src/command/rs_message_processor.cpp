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

#include "command/rs_message_processor.h"

#include "command/rs_command.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS {
namespace Rosen {
RSMessageProcessor& RSMessageProcessor::Instance()
{
    static RSMessageProcessor processor;
    return processor;
}

RSMessageProcessor::~RSMessageProcessor() {}

void RSMessageProcessor::AddUIMessage(uint32_t pid, std::unique_ptr<RSCommand>& command)
{
    transactionMap_[pid].AddCommand(std::move(command));
}

void RSMessageProcessor::AddUIMessage(uint32_t pid, std::unique_ptr<RSCommand>&& command)
{
    transactionMap_[pid].AddCommand(std::move(command));
}

bool RSMessageProcessor::HasTransaction() const
{
    return !transactionMap_.empty();
}

bool RSMessageProcessor::HasTransaction(uint32_t pid) const
{
    auto iter = transactionMap_.find(pid);
    return iter != transactionMap_.end() && !iter->second.IsEmpty();
}

RSTransactionData&& RSMessageProcessor::GetTransaction(uint32_t pid)
{
    return std::move(transactionMap_[pid]);
}

std::unordered_map<uint32_t, RSTransactionData>&& RSMessageProcessor::GetAllTransactions()
{
    return std::move(transactionMap_);
}

} // namespace Rosen
} // namespace OHOS
