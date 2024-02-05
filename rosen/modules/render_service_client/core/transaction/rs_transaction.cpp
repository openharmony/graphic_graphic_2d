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

#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "sandbox_utils.h"
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
    syncId_ = GenerateSyncId();
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        RS_TRACE_NAME("OpenSyncTransaction");
        transactionProxy->StartSyncTransaction();
        transactionProxy->Begin();
    }
}

void RSTransaction::CloseSyncTransaction()
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        RS_TRACE_NAME_FMT("CloseSyncTransaction syncId: %lu syncCount: %d", syncId_, transactionCount_);
        transactionProxy->MarkTransactionNeedCloseSync(transactionCount_);
        transactionProxy->SetSyncId(syncId_);
        transactionProxy->CommitSyncTransaction();
        transactionProxy->CloseSyncTransaction();
    }
    ResetSyncTransactionInfo();
}

void RSTransaction::Begin()
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        RS_TRACE_NAME("BeginSyncTransaction");
        transactionProxy->StartSyncTransaction();
        transactionProxy->Begin();
    }
}

void RSTransaction::Commit()
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        RS_TRACE_NAME_FMT("CommitSyncTransaction syncId: %lu", syncId_);
        transactionProxy->SetSyncTransactionNum(transactionCount_);
        transactionProxy->SetSyncId(syncId_);
        transactionProxy->CommitSyncTransaction();
        transactionProxy->CloseSyncTransaction();
    }
}

uint64_t RSTransaction::GenerateSyncId()
{
    static pid_t pid_ = GetRealPid();
    static std::atomic<uint32_t> currentId_ = 0;

    auto currentId = currentId_.fetch_add(1, std::memory_order_relaxed);
    if (currentId == UINT32_MAX) {
        // [PLANNING]:process the overflow situations
        ROSEN_LOGE("Transaction sync Id overflow");
    }

    // concat two 32-bit numbers to one 64-bit number
    return ((uint64_t)pid_ << 32) | currentId;
}

void RSTransaction::ResetSyncTransactionInfo()
{
    std::unique_lock<std::mutex> lock(mutex_);
    syncId_ = 0;
    transactionCount_ = 0;
}

RSTransaction* RSTransaction::Unmarshalling(Parcel& parcel)
{
    auto rsTransaction = new RSTransaction();
    if (rsTransaction->UnmarshallingParam(parcel)) {
        return rsTransaction;
    }
    ROSEN_LOGE("RSTransactionData Unmarshalling Failed");
    delete rsTransaction;
    return nullptr;
}

bool RSTransaction::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteUint64(syncId_)) {
        ROSEN_LOGE("RSTransaction marshalling failed");
        return false;
    }
    transactionCount_++;
    return true;
}

bool RSTransaction::UnmarshallingParam(Parcel& parcel)
{
    if (!parcel.ReadUint64(syncId_)) {
        ROSEN_LOGE("RSTransaction unmarshallingParam failed");
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
