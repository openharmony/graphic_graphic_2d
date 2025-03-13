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
#include "transaction/rs_interfaces.h"
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

void RSTransaction::OpenSyncTransaction(std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    syncId_ = GenerateSyncId();
    if (rsTransactionHandler_ != nullptr) {
        RS_TRACE_NAME("OpenSyncTransaction");
        ROSEN_LOGI("OpenSyncTransaction");
        rsTransactionHandler_->FlushImplicitTransaction();
        rsTransactionHandler_->StartSyncTransaction();
        rsTransactionHandler_->Begin();
        isOpenSyncTransaction_ = true;
        transactionCount_ = 0;
        parentPid_ = GetRealPid();
        rsTransactionHandler_->StartCloseSyncTransactionFallbackTask(handler, true);
    } else {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            RS_TRACE_NAME("OpenSyncTransaction");
            ROSEN_LOGI("OpenSyncTransaction");
            transactionProxy->FlushImplicitTransaction();
            transactionProxy->StartSyncTransaction();
            transactionProxy->Begin();
            isOpenSyncTransaction_ = true;
            transactionCount_ = 0;
            parentPid_ = GetRealPid();
            transactionProxy->StartCloseSyncTransactionFallbackTask(handler, true);
        }
    }
}

void RSTransaction::CloseSyncTransaction(std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    if (rsTransactionHandler_ != nullptr) {
        RS_TRACE_NAME_FMT("CloseSyncTransaction syncId: %lu syncCount: %d", syncId_, transactionCount_);
        ROSEN_LOGI(
            "CloseSyncTransaction syncId: %{public}" PRIu64 " syncCount: %{public}d", syncId_, transactionCount_);
        isOpenSyncTransaction_ = false;
        rsTransactionHandler_->MarkTransactionNeedCloseSync(transactionCount_);
        rsTransactionHandler_->SetSyncId(syncId_);
        rsTransactionHandler_->CommitSyncTransaction();
        rsTransactionHandler_->CloseSyncTransaction();
        rsTransactionHandler_->StartCloseSyncTransactionFallbackTask(handler, false);
    } else {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            RS_TRACE_NAME_FMT("CloseSyncTransaction syncId: %lu syncCount: %d", syncId_, transactionCount_);
            ROSEN_LOGI(
                "CloseSyncTransaction syncId: %{public}" PRIu64 " syncCount: %{public}d", syncId_, transactionCount_);
            isOpenSyncTransaction_ = false;
            transactionProxy->MarkTransactionNeedCloseSync(transactionCount_);
            transactionProxy->SetSyncId(syncId_);
            transactionProxy->CommitSyncTransaction();
            transactionProxy->CloseSyncTransaction();
            transactionProxy->StartCloseSyncTransactionFallbackTask(handler, false);
        }
    }
    ResetSyncTransactionInfo();
}

void RSTransaction::Begin()
{
    if (rsTransactionHandler_ != nullptr) {
        RS_TRACE_NAME("BeginSyncTransaction");
        ROSEN_LOGI("BeginSyncTransaction");
        rsTransactionHandler_->StartSyncTransaction();
        rsTransactionHandler_->Begin();
    } else {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            RS_TRACE_NAME("BeginSyncTransaction");
            ROSEN_LOGI("BeginSyncTransaction");
            transactionProxy->StartSyncTransaction();
            transactionProxy->Begin();
        }
    }
}

void RSTransaction::Commit()
{
    if (rsTransactionHandler_ != nullptr) {
        RS_TRACE_NAME_FMT(
            "CommitSyncTransaction syncId: %lu syncCount: %d parentPid: %d", syncId_, transactionCount_, parentPid_);
        ROSEN_LOGI("CommitSyncTransaction syncId: %{public}" PRIu64 " syncCount: %{public}d parentPid: %{public}d",
            syncId_, transactionCount_, parentPid_);
        rsTransactionHandler_->SetSyncTransactionNum(transactionCount_);
        rsTransactionHandler_->SetSyncId(syncId_);
        rsTransactionHandler_->SetParentPid(parentPid_);
        rsTransactionHandler_->CommitSyncTransaction();
        rsTransactionHandler_->CloseSyncTransaction();
    } else {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            RS_TRACE_NAME_FMT("CommitSyncTransaction syncId: %lu syncCount: %d parentPid: %d", syncId_,
                transactionCount_, parentPid_);
            ROSEN_LOGI("CommitSyncTransaction syncId: %{public}" PRIu64 " syncCount: %{public}d parentPid: %{public}d",
                syncId_, transactionCount_, parentPid_);
            transactionProxy->SetSyncTransactionNum(transactionCount_);
            transactionProxy->SetSyncId(syncId_);
            transactionProxy->SetParentPid(parentPid_);
            transactionProxy->CommitSyncTransaction();
            transactionProxy->CloseSyncTransaction();
        }
    }

    ResetSyncTransactionInfo();
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
    parentPid_ = -1;
    isOpenSyncTransaction_ = false;
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
        ROSEN_LOGE("RSTransaction marshalling synchronous Id failed");
        return false;
    }
    if (!parcel.WriteInt32(duration_)) {
        ROSEN_LOGE("RSTransaction marshalling duration failed");
        return false;
    }
    if (!parcel.WriteInt32(parentPid_)) {
        ROSEN_LOGE("RSTransaction marshalling parent pid failed");
        return false;
    }
    if (!parcel.WriteBool(isOpenSyncTransaction_)) {
        ROSEN_LOGE("RSTransaction marshalling parameter of whether synchronous transaction is open failed");
        return false;
    }
    transactionCount_++;
    RS_TRACE_NAME_FMT("RSTransaction::Marshalling syncId: %lu syncCount: %d", syncId_, transactionCount_);
    ROSEN_LOGD("Marshalling syncId: %{public}" PRIu64 " syncCount: %{public}d", syncId_, transactionCount_);
    return true;
}

bool RSTransaction::UnmarshallingParam(Parcel& parcel)
{
    if (!parcel.ReadUint64(syncId_)) {
        ROSEN_LOGE("RSTransaction unmarshallingParam synchronous Id failed");
        return false;
    }
    if (!parcel.ReadInt32(duration_)) {
        ROSEN_LOGE("RSTransaction unmarshallingParam duration failed");
        return false;
    }
    if (!parcel.ReadInt32(parentPid_)) {
        ROSEN_LOGE("RSTransaction unmarshallingParam parent pid failed");
        return false;
    }
    if (!parcel.ReadBool(isOpenSyncTransaction_)) {
        ROSEN_LOGE("RSTransaction unmarshalling parameter of whether synchronous transaction is open failed");
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
