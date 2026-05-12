/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "transaction/rs_transaction_handler_manager.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSTransactionHandlerManager& RSTransactionHandlerManager::Instance()
{
    static RSTransactionHandlerManager instance;
    return instance;
}

void RSTransactionHandlerManager::Register(uint64_t token, std::shared_ptr<RSTransactionHandler> handler)
{
    if (handler == nullptr) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    handlers_[token] = handler;
}

std::shared_ptr<RSTransactionHandler> RSTransactionHandlerManager::Get(uint64_t token)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = handlers_.find(token);
    if (it != handlers_.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<RSTransactionHandler> RSTransactionHandlerManager::GetAny()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handlers_.empty()) {
        return nullptr;
    }
    RS_LOGW("RSTransactionHandlerManager::GetAny: using fallback handler, size=%{public}zu", handlers_.size());
    return handlers_.begin()->second;
}

void RSTransactionHandlerManager::Unregister(uint64_t token)
{
    std::lock_guard<std::mutex> lock(mutex_);
    handlers_.erase(token);
}
} // namespace Rosen
} // namespace OHOS
