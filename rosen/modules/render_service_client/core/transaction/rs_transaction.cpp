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

#include "ipc_callbacks/rs_sync_transaction_controller_proxy.h"
#include "platform/common/rs_log.h"
#include "rs_process_transaction_controller.h"
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
        transactionProxy->StartSyncTransaction();
        transactionProxy->Begin();
    }
}

void RSTransaction::CloseSyncTransaction(const uint64_t transactionCount)
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->MarkTransactionNeedCloseSync(transactionCount);
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
        transactionProxy->StartSyncTransaction();
        transactionProxy->Begin();
    }
}

void RSTransaction::Commit()
{
    CreateTransactionFinish();
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
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
    syncId_ = 0;
    controllers_.clear();
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

void RSTransaction::MarshallTransactionSyncController(MessageParcel& parcel)
{
    sptr<RSProcessTransactionController> controller = new RSProcessTransactionController();
    if (!parcel.WriteRemoteObject(controller->AsObject())) {
        ROSEN_LOGE("RSTransaction Marshall transactionSyncController failed");
        return;
    }

    TransactionFinishedCallback callback = [this]() {
        CallCreateFinishCallback();
    };
    controller->SetTransactionFinishedCallback(callback);
    CallCreateStartCallback();
    controllers_.emplace_back(controller);
}

void RSTransaction::UnmarshallTransactionSyncController(MessageParcel& parcel)
{
    sptr<IRemoteObject> controllerObject = parcel.ReadRemoteObject();
    if (controllerObject == nullptr) {
        ROSEN_LOGE("RSTransaction unmarshalling transactionSyncController failed");
        return;
    }

    sptr<RSISyncTransactionController> controller = iface_cast<RSISyncTransactionController>(controllerObject);
    controllers_.emplace_back(controller);
}

void RSTransaction::SetCreateStartCallback(const std::function<void()>& callback)
{
    createStartCallback_ = callback;
}

void RSTransaction::SetCreateFinishCallback(const std::function<void()>& callback)
{
    createFinishCallback_ = callback;
}

void RSTransaction::CallCreateStartCallback()
{
    if (createStartCallback_) {
        createStartCallback_();
    }
}

void RSTransaction::CallCreateFinishCallback()
{
    if (createFinishCallback_) {
        createFinishCallback_();
    }
}

void RSTransaction::CreateTransactionFinish()
{
    if (!controllers_.empty() && controllers_.back()) {
        controllers_.back()->CreateTransactionFinished();
    }
    controllers_.clear();
}
} // namespace Rosen
} // namespace OHOS
