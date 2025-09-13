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
#include "ui/rs_ui_context_manager.h"

namespace OHOS {
namespace Rosen {

void RSTransaction::FlushImplicitTransaction()
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction();
    }
}

void RSTransaction::OpenSyncTransaction(std::shared_ptr<AppExecFwk::EventHandler> handler, bool isInnerProcess)
{
    if (rsTransactionHandler_ != nullptr) {
        if (syncId_ > 0) {
            RS_TRACE_NAME_FMT("OpenSyncTransaction: clear last syncId:%" PRIu64 "", syncId_);
            ROSEN_LOGI("OpenSyncTransaction: clear last syncId:%{public}" PRIu64 "", syncId_);
            ProcessAllSyncTransaction();
        }
        syncId_ = GenerateSyncId();
        RS_TRACE_NAME_FMT("OpenSyncTransaction syncId:%" PRIu64 ", isInnerProcess:%d", syncId_, isInnerProcess);
        ROSEN_LOGI(
            "OpenSyncTransaction syncId:%{public}" PRIu64 ", isInnerProcess:%{public}d", syncId_, isInnerProcess);
        rsTransactionHandler_->FlushImplicitTransaction();
        rsTransactionHandler_->StartSyncTransaction();
        rsTransactionHandler_->Begin();
        isOpenSyncTransaction_ = true;
        transactionCount_ = 0;
        isInnerProcess_ = isInnerProcess;
        parentPid_ = GetRealPid();
        RSUIContextManager::MutableInstance().StartCloseSyncTransactionFallbackTask(true, syncId_);
    } else {
        syncId_ = GenerateSyncId();
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            RS_TRACE_NAME_FMT("OpenSyncTransaction syncId:%" PRIu64 "", syncId_);
            ROSEN_LOGI("OpenSyncTransaction syncId:%{public}" PRIu64 "", syncId_);
            transactionProxy->FlushImplicitTransaction();
            transactionProxy->StartSyncTransaction();
            transactionProxy->Begin();
            isOpenSyncTransaction_ = true;
            transactionCount_ = 0;
            parentPid_ = GetRealPid();
            transactionProxy->StartCloseSyncTransactionFallbackTask(true);
        }
    }
}

void RSTransaction::CloseSyncTransaction(std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    RS_TRACE_NAME_FMT("CloseSyncTransaction syncId:%" PRIu64 "", syncId_);
    ROSEN_LOGI("CloseSyncTransaction syncId:%{public}" PRIu64 "", syncId_);
    std::unique_lock<std::recursive_mutex> lock(subSyncTransactionsMutex_);
    if (subSyncTransactions_.empty()) {
        InnerCloseSyncTransaction();
        return;
    }
    bool isAllSyncFinished = true;
    for (const auto& pair : subSyncTransactions_) {
        if (pair.second != 0) {
            isAllSyncFinished = false;
            break;
        }
    }

    if (isAllSyncFinished) {
        ProcessAllSyncTransaction();
    } else {
        isNeedCloseSyncTransaction_ = true;
        return;
    }
}

void RSTransaction::InnerCloseSyncTransaction()
{
    if (rsTransactionHandler_ != nullptr) {
        RS_TRACE_NAME_FMT("InnerCloseSyncTransaction syncId:%" PRIu64 " syncCount: %d", syncId_, transactionCount_);
        ROSEN_LOGI(
            "InnerCloseSyncTransaction syncId:%{public}" PRIu64 " syncCount: %{public}d", syncId_, transactionCount_);
        isOpenSyncTransaction_ = false;
        rsTransactionHandler_->MarkTransactionNeedCloseSync(transactionCount_);
        rsTransactionHandler_->SetSyncId(syncId_);
        rsTransactionHandler_->CommitSyncTransaction(syncId_);
        rsTransactionHandler_->CloseSyncTransaction();
        RSUIContextManager::MutableInstance().StartCloseSyncTransactionFallbackTask(false, syncId_);
    } else {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            RS_TRACE_NAME_FMT("InnerCloseSyncTransaction syncId:%" PRIu64 " syncCount: %d", syncId_, transactionCount_);
            ROSEN_LOGI("InnerCloseSyncTransaction syncId:%{public}" PRIu64 " syncCount: %{public}d", syncId_,
                transactionCount_);
            isOpenSyncTransaction_ = false;
            transactionProxy->MarkTransactionNeedCloseSync(transactionCount_);
            transactionProxy->SetSyncId(syncId_);
            transactionProxy->CommitSyncTransaction();
            transactionProxy->CloseSyncTransaction();
            transactionProxy->StartCloseSyncTransactionFallbackTask(false);
        }
    }
    ResetSyncTransactionInfo();
}

void RSTransaction::Begin()
{
    if (rsTransactionHandler_ != nullptr) {
        RS_TRACE_NAME_FMT("BeginSyncTransaction syncId:%" PRIu64 "", syncId_);
        ROSEN_LOGI("BeginSyncTransaction syncId:%{public}" PRIu64 "", syncId_);
        rsTransactionHandler_->StartSyncTransaction();
        rsTransactionHandler_->Begin(syncId_);
    } else {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            RS_TRACE_NAME_FMT("BeginSyncTransaction syncId:%" PRIu64 "", syncId_);
            ROSEN_LOGI("BeginSyncTransaction syncId:%{public}" PRIu64 "", syncId_);
            transactionProxy->StartSyncTransaction();
            transactionProxy->Begin();
        }
    }
}

void RSTransaction::Commit()
{
    if (rsTransactionHandler_ != nullptr) {
        RS_TRACE_NAME_FMT("CommitSyncTransaction syncId:%" PRIu64 " syncCount: %d parentPid: %d", syncId_,
            transactionCount_, parentPid_);
        ROSEN_LOGI("CommitSyncTransaction syncId:%{public}" PRIu64 " syncCount: %{public}d parentPid: %{public}d",
            syncId_, transactionCount_, parentPid_);
        rsTransactionHandler_->SetSyncTransactionNum(transactionCount_);
        rsTransactionHandler_->SetSyncId(syncId_);
        rsTransactionHandler_->SetParentPid(parentPid_);
        rsTransactionHandler_->CommitSyncTransaction(syncId_);
        rsTransactionHandler_->CloseSyncTransaction();
    } else {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            RS_TRACE_NAME_FMT("CommitSyncTransaction syncId:%" PRIu64 " syncCount: %d parentPid: %d", syncId_,
                transactionCount_, parentPid_);
            ROSEN_LOGI("CommitSyncTransaction syncId:%{public}" PRIu64 " syncCount: %{public}d parentPid: %{public}d",
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

void RSTransaction::StartSubTransaction(const uint64_t syncId)
{
    if (rsTransactionHandler_ != nullptr) {
        RS_TRACE_NAME_FMT("StartSubTransaction syncId:%" PRIu64 "", syncId);
        ROSEN_LOGI("StartSubTransaction syncId:%{public}" PRIu64 "", syncId);
        rsTransactionHandler_->FlushImplicitTransaction();
        rsTransactionHandler_->Begin(syncId);
    }
}

void RSTransaction::AddSubSyncTransaction(
    std::shared_ptr<RSTransaction> subRSTransaction, const uint64_t token, const uint64_t syncId)
{
    if (syncId != syncId_) {
        RS_TRACE_NAME_FMT(
            "AddSubSyncTransaction: syncId is not same syncId:%" PRIu64 ", currentSyncId:%" PRIu64 "", syncId, syncId_);
        ROSEN_LOGI("AddSubSyncTransaction: syncId is not same syncId:%{public}" PRIu64 ", "
            "currentSyncId:%{public}" PRIu64 "", syncId, syncId_);
        return;
    }
    RS_TRACE_NAME_FMT("AddSubSyncTransaction syncId:%" PRIu64 ", token:%" PRIu64 ", transactionHandler:%zu", syncId,
        token, std::hash<RSTransactionHandler*>()(rsTransactionHandler_.get()));
    ROSEN_LOGI("AddSubSyncTransaction syncId:%{public}" PRIu64 ", token:%{public}" PRIu64 ", "
        "transactionHandler:%{public}zu", syncId, token,
        std::hash<RSTransactionHandler*>()(rsTransactionHandler_.get()));
    subRSTransaction->StartSubTransaction(syncId);
    std::unique_lock<std::recursive_mutex> lock(subSyncTransactionsMutex_);
    subSyncTransactions_[token]++;
}

void RSTransaction::RemoveSubSyncTransaction(const uint64_t token, const uint64_t syncId)
{
    if (syncId != syncId_) {
        RS_TRACE_NAME_FMT("RemoveSubSyncTransaction: syncId is not same syncId:%" PRIu64 ", currentSyncId:%" PRIu64 "",
            syncId, syncId_);
        ROSEN_LOGI("RemoveSubSyncTransaction: syncId is not same syncId:%{public}" PRIu64 ", "
            "currentSyncId:%{public}" PRIu64 "", syncId, syncId_);
        return;
    }
    RS_TRACE_NAME_FMT("RemoveSubSyncTransaction syncId:%" PRIu64 ", token:%" PRIu64 ", transactionHandler:%zu", syncId,
        token, std::hash<RSTransactionHandler*>()(rsTransactionHandler_.get()));
    ROSEN_LOGI("RemoveSubSyncTransaction syncId:%{public}" PRIu64 ", token:%{public}" PRIu64 ", "
        "transactionHandler:%{public}zu", syncId, token,
        std::hash<RSTransactionHandler*>()(rsTransactionHandler_.get()));
    bool isAllSyncFinished = true;
    std::unique_lock<std::recursive_mutex> lock(subSyncTransactionsMutex_);
    subSyncTransactions_[token]--;
    for (const auto& pair : subSyncTransactions_) {
        if (pair.second != 0) {
            isAllSyncFinished = false;
            break;
        }
    }
    if (isAllSyncFinished && isNeedCloseSyncTransaction_) {
        ProcessAllSyncTransaction();
    }
}

void RSTransaction::ProcessAllSyncTransaction()
{
    RS_TRACE_NAME_FMT("ProcessAllSyncTransaction syncId:%" PRIu64 "", syncId_);
    ROSEN_LOGI("ProcessAllSyncTransaction syncId:%{public}" PRIu64 "", syncId_);
    std::unique_lock<std::recursive_mutex> lock(subSyncTransactionsMutex_);
    if (rsTransactionHandler_ != nullptr) {
        for (const auto& pair : subSyncTransactions_) {
            auto rsUIContext = RSUIContextManager::Instance().GetRSUIContext(pair.first);
            if (rsUIContext == nullptr) {
                ROSEN_LOGE("ProcessAllSyncTransaction: can not find RSUIContext syncId:%{public}" PRIu64 ", "
                    "token:%{public}" PRIu64 "", syncId_, pair.first);
                continue;
            }
            rsTransactionHandler_->MergeSyncTransaction(rsUIContext->GetRSTransaction());
        }
    }
    InnerCloseSyncTransaction();
    subSyncTransactions_.clear();
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
    RS_TRACE_NAME_FMT("ResetSyncTransactionInfo syncId:%" PRIu64 "", syncId_);
    ROSEN_LOGI("ResetSyncTransactionInfo syncId:%{public}" PRIu64 "", syncId_);
    std::unique_lock<std::mutex> lock(mutex_);
    syncId_ = 0;
    transactionCount_ = 0;
    parentPid_ = -1;
    isOpenSyncTransaction_ = false;
    isNeedCloseSyncTransaction_ = false;
    isInnerProcess_ = false;
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
    RS_TRACE_NAME_FMT("RSTransaction::Marshalling syncId:%" PRIu64 " syncCount: %d", syncId_, transactionCount_);
    ROSEN_LOGD("Marshalling syncId:%{public}" PRIu64 " syncCount: %{public}d", syncId_, transactionCount_);
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
