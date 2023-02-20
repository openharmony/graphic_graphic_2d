/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "rs_transaction.h"

#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {

void RSTransaction::FlushImplicitTransaction()
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction();
    }
}

void RSTransaction::OpenSyncTransaction()
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->StartSyncTransaction();
        transactionProxy->Begin();
    }
}

void RSTransaction::CloseSyncTransaction(const uint64_t syncId, const int32_t transactonCount)
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->MarkTransactionNeedCloseSync(transactonCount);
        transactionProxy->SetSyncId(syncId);
        transactionProxy->CommitSyncTransaction();
        transactionProxy->CloseSyncTransaction();
    }
}

void RSTransaction::StartSyncTransactionForProcess()
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->StartSyncTransaction();
        transactionProxy->Begin();
    }
}

void RSTransaction::CloseSyncTransactionForProcess(const uint64_t syncId)
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->SetSyncId(syncId);
        transactionProxy->CommitSyncTransaction();
        transactionProxy->CloseSyncTransaction();
    }
}
} // namespace Rosen
} // namespace OHOS
