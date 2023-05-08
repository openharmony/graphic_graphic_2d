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

#include <atomic>

#include "command/rs_message_processor.h"

#include "command/rs_command.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS {
namespace Rosen {
namespace {
static std::atomic_bool g_instanceValid = false;
}

RSMessageProcessor& RSMessageProcessor::Instance()
{
    static RSMessageProcessor processor;
    return processor;
}

RSMessageProcessor::RSMessageProcessor()
{
    g_instanceValid.store(true);
}

RSMessageProcessor::~RSMessageProcessor()
{
    g_instanceValid.store(false);
    std::unique_lock<std::mutex> lock(transactionMapMutex_);
    transactionMap_.clear();
}

void RSMessageProcessor::AddUIMessage(uint32_t pid, std::unique_ptr<RSCommand>& command)
{
    if (!g_instanceValid.load()) {
        return;
    }
    std::unique_lock<std::mutex> lock(transactionMapMutex_);
    transactionMap_[pid].AddCommand(std::move(command), 0, FollowType::NONE);
}

void RSMessageProcessor::AddUIMessage(uint32_t pid, std::unique_ptr<RSCommand>&& command)
{
    if (!g_instanceValid.load()) {
        return;
    }
    std::unique_lock<std::mutex> lock(transactionMapMutex_);
    transactionMap_[pid].AddCommand(std::move(command), 0, FollowType::NONE);
}

bool RSMessageProcessor::HasTransaction() const
{
    if (!g_instanceValid.load()) {
        return false;
    }
    std::unique_lock<std::mutex> lock(transactionMapMutex_);
    return !transactionMap_.empty();
}

bool RSMessageProcessor::HasTransaction(uint32_t pid) const
{
    if (!g_instanceValid.load()) {
        return false;
    }
    std::unique_lock<std::mutex> lock(transactionMapMutex_);
    auto iter = transactionMap_.find(pid);
    return iter != transactionMap_.end() && !iter->second.IsEmpty();
}

RSTransactionData&& RSMessageProcessor::GetTransaction(uint32_t pid)
{
    std::unique_lock<std::mutex> lock(transactionMapMutex_);
    return std::move(transactionMap_[pid]);
}

std::unordered_map<uint32_t, RSTransactionData>&& RSMessageProcessor::GetAllTransactions()
{
    std::unique_lock<std::mutex> lock(transactionMapMutex_);
    return std::move(transactionMap_);
}

} // namespace Rosen
} // namespace OHOS
