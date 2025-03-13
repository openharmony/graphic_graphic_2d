/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "rs_sync_transaction_handler.h"

#include "rs_transaction.h"

#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
RSSyncTransactionHandler::~RSSyncTransactionHandler() {}

void RSSyncTransactionHandler::SetTransactionHandler(std::shared_ptr<RSTransactionHandler> rsTransactionHandler)
{
    if (!rsTransaction_) {
        rsTransaction_ = std::make_shared<RSTransaction>();
    }
    rsTransaction_->SetTransactionHandler(rsTransactionHandler);
}

std::shared_ptr<RSTransaction> RSSyncTransactionHandler::GetRSTransaction()
{
    if (!needCloseSync_) {
        return nullptr;
    }
    return rsTransaction_;
}

void RSSyncTransactionHandler::OpenSyncTransaction(std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    if (!RSSystemProperties::GetSyncTransactionEnabled()) {
        return;
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (needCloseSync_) {
            return;
        }
        needCloseSync_ = true;
    }
    ROSEN_LOGD("RS sync transaction controller OpenSyncTransaction");
    if (rsTransaction_) {
        rsTransaction_->OpenSyncTransaction(handler);
    }
}

void RSSyncTransactionHandler::CloseSyncTransaction(std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    if (!needCloseSync_) {
        return;
    }

    ROSEN_LOGD("RS sync transaction controller CloseSyncTransaction");
    if (rsTransaction_) {
        rsTransaction_->CloseSyncTransaction(handler);
    }
    {
        std::unique_lock<std::mutex> lock(mutex_);
        needCloseSync_ = false;
    }
}
} // namespace Rosen
} // namespace OHOS
